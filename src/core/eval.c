#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../util/table.h"

#include "obj.h"
#include "lstring.h"
#include "basic_stream.h"
#include "symbol.h"
#include "list.h"
#include "print.h"
#include "environment.h"
#include "data.h"

#include "eval.h"

bool check_arglen(lis_obj * genv, lis_obj * args, int len, lis_obj * opname) {
    if (LIS_INT(_list_length(genv, args)) != len) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        stream_write_string(buffer, LSTR(U"wrong number of args for `"));
        stream_write_string(buffer, opname);
        stream_write_string(buffer, LSTR(U"`."));

        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return false;
    } else {
        return true;
    }
}

bool check_argge(lis_obj * genv, lis_obj * args, int len, lis_obj * opname) {
    if (LIS_INT(_list_length(genv, args)) < len) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        stream_write_string(buffer, LSTR(U"few number of args for `"));
        stream_write_string(buffer, opname);
        stream_write_string(buffer, LSTR(U"`."));

        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return false;
    } else {
        return true;
    }
}

bool check_argeven(lis_obj * genv, lis_obj * args, lis_obj * opname) {
    if (LIS_INT(_list_length(genv, args)) % 2 == 0) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        stream_write_string(buffer, LSTR(U"odd number of args for `"));
        stream_write_string(buffer, opname);
        stream_write_string(buffer, LSTR(U"`."));

        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return false;
    } else {
        return true;
    }
}

lis_lambdalist * validate_lambdalist(lis_obj * genv, lis_obj * lenv, lis_obj * lambdalist) {
    lis_obj * rest = lambdalist;
    bool optional_p = false;
    bool keyword_p = false;
    bool rest_p = false;

    lis_arg * optional_rest = NULL;

    _table * name_table = _make_table(48);

    lis_lambdalist * llist = (lis_lambdalist *)malloc(sizeof(lis_lambdalist));
    memset(llist, 0, sizeof(lis_lambdalist));
    llist->required = LIS_NIL(genv);

    while (rest != LIS_NIL(genv)) {
        lis_obj * car = _list_car(genv, rest);

        if (car == LIS_GENV(genv)->symbol_optional) {
            if (keyword_p || rest_p) {
                lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
                stream_write_string(buffer, LSTR(U"misplaced `&optional` in lambda list: "));
                print(genv, lambdalist, buffer);
                LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
                return NULL;
            }

            optional_p = true;

        } else if (car == LIS_GENV(genv)->symbol_rest) {
            if (keyword_p) {
                lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
                stream_write_string(buffer, LSTR(U"misplaced `&rest` in lambda list: "));
                print(genv, lambdalist, buffer);
                LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
                return NULL;
            }

            rest_p = true;

        } else if (car == LIS_GENV(genv)->symbol_key) {
            keyword_p = true;
            llist->keyword = _make_table(256);

        } else {
            if (_table_find(name_table, (void *)car) == NULL) {
                _table_add(name_table, (void *)car, (void *)car);
            } else {
                lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
                stream_write_string(buffer, LSTR(U"the variable `"));
                print(genv, lambdalist, buffer);
                stream_write_string(buffer, LSTR(U"` occurs more than once in lambda list."));
                LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
                return NULL;
            }

            if (rest_p && llist->rest == NULL) {
                llist->rest = car;

            } else if (keyword_p) {
                if (LIS_TAG_BASE(car) == LIS_TAG_BASE_BUILTIN &&
                    LIS_TAG_TYPE(car) == LIS_TAG_TYPE_SYM) {
                    lis_arg * arg = (lis_arg *)malloc(sizeof(lis_arg));
                    arg->default_value = LIS_NIL(genv);
                    _table_add(llist->keyword, (void *)car, (void *)arg);

                } else if (LIS_TAG_BASE(car) == LIS_TAG_BASE_BUILTIN &&
                           LIS_TAG_TYPE(car) == LIS_TAG_TYPE_CONS) {

                    if (LIS_INT(_list_length(genv, car)) != 2) {
                        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
                        stream_write_string(buffer, LSTR(U"malformed keyword arg: "));
                        print(genv, car, buffer);
                        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
                    }

                    lis_obj * name = _list_nth(genv, INT(0), car);
                    lis_arg * arg = (lis_arg *)malloc(sizeof(lis_arg));
                    arg->default_value = eval(genv, lenv, _list_nth(genv, INT(1), car));
                    _table_add(llist->keyword, (void *)name, (void *)arg);

                } else {
                    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
                    print(genv, car, buffer);
                    stream_write_string(buffer, LSTR(U" is nether symbol nor list."));
                    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
                    return NULL;
                }

            } else if (optional_p) {
                lis_arg * arg;

                if (LIS_TAG_BASE(car) == LIS_TAG_BASE_BUILTIN &&
                    LIS_TAG_TYPE(car) == LIS_TAG_TYPE_SYM) {
                    arg = (lis_arg *)malloc(sizeof(lis_arg));
                    arg->name = car;
                    arg->default_value = LIS_NIL(genv);
                    arg->next = NULL;

                } else if (LIS_TAG_BASE(car) == LIS_TAG_BASE_BUILTIN &&
                           LIS_TAG_TYPE(car) == LIS_TAG_TYPE_CONS) {

                    if (LIS_INT(_list_length(genv, car)) != 2) {
                        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
                        stream_write_string(buffer, LSTR(U"malformed optional arg: "));
                        print(genv, car, buffer);
                        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
                    }

                    arg = (lis_arg *)malloc(sizeof(lis_arg));
                    arg->name = _list_nth(genv, INT(0), car);
                    arg->default_value = eval(genv, lenv, _list_nth(genv, INT(1), car));
                    arg->next = NULL;

                } else {
                    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
                    print(genv, car, buffer);
                    stream_write_string(buffer, LSTR(U" is nether symbol nor list."));
                    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
                    return NULL;
                }

                if (llist->optional == NULL) {
                    llist->optional = arg;
                }

                if (optional_rest != NULL) {
                    optional_rest->next = arg;
                }

                optional_rest = arg;

            } else {
                if (LIS_TAG_BASE(car) != LIS_TAG_BASE_BUILTIN ||
                    LIS_TAG_TYPE(car) != LIS_TAG_TYPE_SYM) {
                    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
                    print(genv, car, buffer);
                    stream_write_string(buffer, LSTR(U" is not a symbol."));
                    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
                    return NULL;
                }
                llist->required = _list_cons(genv, car, llist->required);
            }
        }

        rest = _list_cdr(genv, rest);
    }

    _free_table(name_table);

    if (llist->required != LIS_NIL(genv)) {
        llist->required = _list_reverse(genv, llist->required);
    }
    return llist;
}

lis_obj * bind_lambdalist(lis_obj * genv, lis_obj * fn, lis_obj * args) {
    lis_obj * lenv = _make_lexical_env(LIS_ENV_LEXICAL);
    lis_obj * args_rest = args;
    lis_lambdalist * lambdalist = LIS_FN(fn)->lambdalist;

    lis_obj * required_rest = lambdalist->required;
    while (required_rest != LIS_NIL(genv)) {
        if (args_rest == LIS_NIL(genv)) {
            LIS_GENV(genv)->error = _make_error(LSTR(U"too few argument."));
            return NULL;
        }

        lis_obj * name = _list_car(genv, required_rest);
        lis_obj * value = _list_car(genv, args_rest);
        set_lexical_value(lenv, name, value);

        args_rest = _list_cdr(genv, args_rest);
        required_rest = _list_cdr(genv, required_rest);
    }

    lis_arg * optional_rest = lambdalist->optional;
    while (optional_rest != NULL) {
        lis_obj * name = optional_rest->name;

        if (args_rest == LIS_NIL(genv)) {
            set_lexical_value(lenv, name, optional_rest->default_value);

        } else {
            lis_obj * value = _list_car(genv, args_rest);
            set_lexical_value(lenv, name, value);
            args_rest = _list_cdr(genv, args_rest);
        }

        optional_rest = optional_rest->next;
    }

    if (lambdalist->rest != NULL) {
        set_lexical_value(lenv, lambdalist->rest, args_rest);
    }

    if (lambdalist->keyword != NULL) {
        int keyword_arg_num = 0;

        while (args_rest != LIS_NIL(genv)) {
            lis_obj * name = _list_car(genv, args_rest);
            _table_entry * e = _table_find(lambdalist->keyword, (void *)name);

            if (e == NULL) {
                lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
                stream_write_string(buffer, LSTR(U"unknown keyword arg: "));
                print(genv, name, buffer);
                LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
                return NULL;
            }

            args_rest = _list_cdr(genv, args_rest);

            if (args_rest == LIS_NIL(genv)) {
                LIS_GENV(genv)->error = _make_error(LSTR(U"odd nunber of &key argument."));
                return NULL;
            }

            lis_obj * value = _list_car(genv, args_rest);
            set_lexical_value(lenv, name, value);
            keyword_arg_num++;

            args_rest = _list_cdr(genv, args_rest);

            if (keyword_arg_num == lambdalist->keyword->num) {
                break;
            }
        }

        for (size_t i=0; i<lambdalist->keyword->size; i++) {
            _table_entry * e = lambdalist->keyword->array + i;
            if (e->key == NULL) continue;
            while (e != NULL) {
                if (_table_find(LIS_LENV(lenv)->var, e->key) == NULL) {
                    lis_obj * value = ((lis_arg *)e->value)->default_value;
                    set_lexical_value(lenv, e->key, value);
                }
                e = e->next;
            }
        }
    }

    if (lambdalist->rest == NULL && args_rest != LIS_NIL(genv)) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"too many argument."));
    }

    return lenv;
}

lis_obj * apply(lis_obj * genv, lis_obj * fn, lis_obj * args) {
    if (LIS_TAG_BASE(fn) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(fn) != LIS_TAG_TYPE_FN) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, fn, buffer);
        stream_write_string(buffer, LSTR(U" is not a lisp function."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));

        return NULL;
    }

    if (LIS_TAG_TYPE(fn) == LIS_TAG_TYPE_FN &&
        (LIS_FN(fn)->type == LIS_FUNC_RAW ||
         LIS_FN(fn)->type == LIS_FUNC_MACRO_RAW)) {
        return LIS_FN(fn)->body.raw(genv, args);

    } else if (LIS_TAG_TYPE(fn) == LIS_TAG_TYPE_FN &&
               (LIS_FN(fn)->type == LIS_FUNC_NORMAL ||
                LIS_FN(fn)->type == LIS_FUNC_MACRO)) {
        lis_obj * new_lenv = bind_lambdalist(genv, fn, args);
        if (new_lenv == NULL) {
            return NULL;
        }
        LIS_ENV(new_lenv)->parent = LIS_FN(fn)->env;

        return lis_sf_progn(genv, new_lenv, LIS_FN(fn)->body.lisp);

    } else {
        printf("something wrong...\n");
        return NULL;
    }
}

lis_obj * lisp_apply(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"apply"))) {
        return NULL;
    }

    lis_obj * fn = _list_nth(genv, INT(0), args);

    if (LIS_TAG_BASE(fn) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(fn) != LIS_TAG_TYPE_FN ||
        !(LIS_FN(fn)->type == LIS_FUNC_RAW ||
          LIS_FN(fn)->type == LIS_FUNC_NORMAL)) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"cannot apply for this function."));
        return NULL;
    }

    lis_obj * lambdalist = _list_nth(genv, INT(1), args);
    return apply(genv, fn, lambdalist);
}

lis_obj * eval_args(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (args == LIS_NIL(genv)) {
        return LIS_NIL(genv);
    } else if (LIS_TAG_BASE(args) != LIS_TAG_BASE_BUILTIN ||
               LIS_TAG_TYPE(args) != LIS_TAG_TYPE_CONS) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_OUT, LIS_STREAM_TEXT);
        print(genv, args, buffer);
        stream_write_string(buffer, LSTR(U" is not a list."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));

        return NULL;
    } else {
        lis_obj * cons = _make_cons();
        lis_obj * value = eval(genv, lenv, LIS_CONS(args)->car);

        if (LIS_GENV(genv)->error != NULL) {
            return NULL;
        }

        if (LIS_TAG_BASE(value) == LIS_TAG_BASE_INTERNAL &&
            LIS_TAG_TYPE(value) == LIS_TAG_TYPE_MVAL) {
            value = _list_car(genv, LIS_MVAL(value));
        }
        LIS_CONS(cons)->car = value;
        LIS_CONS(cons)->cdr = eval_args(genv, lenv, LIS_CONS(args)->cdr);
        return cons;
    }
}

lis_obj * eval_cons(lis_obj * genv, lis_obj * lenv, lis_obj * cons) {
    lis_obj * name = LIS_CONS(cons)->car;
    lis_obj * cdr = LIS_CONS(cons)->cdr;

    if (LIS_TAG_BASE(name) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(name) != LIS_TAG_TYPE_SYM) {
        printf("eval_cons(): is's not a symbol!\n");
        return NULL;
    }

    if (cdr != LIS_NIL(genv) &&
        !(LIS_TAG_BASE(cdr) == LIS_TAG_BASE_BUILTIN &&
          LIS_TAG_TYPE(cdr) == LIS_TAG_TYPE_CONS)) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_OUT, LIS_STREAM_TEXT);
        print(genv, cdr, buffer);
        stream_write_string(buffer, LSTR(U" is not a list."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (LIS_SYM(name)->fn != NULL) {
        lis_obj * fn = LIS_SYM(name)->fn;
        assert(LIS_TAG_BASE(fn) == LIS_TAG_BASE_BUILTIN);
        assert(LIS_TAG_TYPE(fn) == LIS_TAG_TYPE_FN);

        lis_obj * args;
        lis_obj * ret;
        switch (LIS_FN(fn)->type) {
        case LIS_FUNC_RAW:
        case LIS_FUNC_NORMAL:
            args = eval_args(genv, lenv, cdr);
            if (args == NULL) return NULL;
            if (LIS_GENV(genv)->error != NULL) return NULL;
            ret = apply(genv, fn, args);
            if (LIS_GENV(genv)->error != NULL) return NULL;
            return ret;
            break;

        case LIS_FUNC_SPECIAL_FORM:
            ret = LIS_FN(fn)->body.sf(genv, lenv, cdr);
            if (LIS_GENV(genv)->error != NULL) return NULL;
            return ret;

        case LIS_FUNC_MACRO_RAW:
        case LIS_FUNC_MACRO:
            return eval(genv, lenv, apply(genv, fn, cdr));
            break;

        default:
            printf("invalid function\n");
            assert(false);
            break;
        }

        return NULL;

    } else {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        stream_write_string(buffer, LSTR(U"unknown operator: `"));
        stream_write_string(buffer, LIS_SYM(name)->name);
        stream_write_string(buffer, LSTR(U"`."));

        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }
}

lis_obj * eval(lis_obj * genv, lis_obj * lenv, lis_obj * obj) {
    if (obj == NULL) return NULL;

    if (LIS_TAG_BASE(obj) == LIS_TAG_BASE_INT) {
        return obj;  // integer
    } else if (LIS_TAG_BASE(obj) == LIS_TAG_BASE_CHAR) {
        return obj;  // char

    } else if (LIS_TAG_BASE(obj) == LIS_TAG_BASE_BUILTIN) {
        // built-in types
        lis_obj * val;
        lis_stream * buffer;

        switch (LIS_TAG_TYPE(obj)) {
        case LIS_TAG_TYPE_ARY:
        case LIS_TAG_TYPE_STR:
        case LIS_TAG_TYPE_TS:
            return obj;

        case LIS_TAG_TYPE_SYM:
            if (LIS_SYM(obj)->package == LIS_GENV(genv)->keyword_package) {
                return obj;
            }

            val = get_lexical_value(lenv, obj);
            if (val != NULL) {
                return val;
            } else if (LIS_SYM(obj)->value != NULL) {
                return LIS_SYM(obj)->value;
            } else {
                buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
                stream_write_string(buffer, LSTR(U"unbound variable: `"));
                print(genv, obj, buffer);
                stream_write_string(buffer, LSTR(U"`."));
                LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
                return NULL;
            }

        case LIS_TAG_TYPE_CONS:
            return eval_cons(genv, lenv, obj);

        default:
            buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
            print(genv, obj, buffer);
            stream_write_string(buffer, LSTR(U" is unknown type... it may be a bug!"));
            LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
            return NULL;
        }

    } else if (LIS_TAG_BASE(obj) == LIS_TAG_BASE_INTERNAL) {
        lis_stream * buffer;

        switch (LIS_TAG_TYPE(obj)) {
        case LIS_TAG_TYPE_ENV:
        case LIS_TAG_TYPE_FN:
        case LIS_TAG_TYPE_PKG:
            return obj;

        case LIS_TAG_TYPE_MVAL:
            return eval(genv, lenv, _list_car(genv, LIS_MVAL(obj)));

        default:
            buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
            print(genv, obj, buffer);
            stream_write_string(buffer, LSTR(U" is unknown type... it may be a bug!"));
            LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
            return NULL;
        }
    } else {
        return obj;
    }
}

lis_obj * lisp_eval(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"eval"))) {
        return NULL;
    }
    return eval(genv, NULL, _list_nth(genv, INT(0), args));
}

bool macroexpand_1(lis_obj * genv, lis_obj * form, lis_obj * env, lis_obj ** expansion) {
    if (_list_listp(genv, form)) {
        lis_obj * opname = _list_car(genv, form);
        lis_obj * args = _list_cdr(genv, form);

        if (LIS_TAG_BASE(opname) != LIS_TAG_BASE_BUILTIN ||
            LIS_TAG_TYPE(opname) != LIS_TAG_TYPE_SYM) {
            *expansion = form;
            return false;

        } else if (LIS_SYM(opname)->fn == NULL) {
            *expansion = form;
            return false;

        } else {
            lis_obj * fn = LIS_SYM(opname)->fn;
            if (LIS_FN(fn)->type != LIS_FUNC_MACRO_RAW &&
                LIS_FN(fn)->type != LIS_FUNC_MACRO) {
                *expansion = form;
                return false;
            }

            lis_obj * value = apply(genv, fn, args);

            if (value == NULL) {
                lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
                stream_write_string(buffer, LSTR(U"cannot expand form: "));
                print(genv, form, buffer);
                LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
                return false;
            }

            *expansion = value;
            return true;
        }

    } else if (_symbol_symbolp(genv, form)) {
        // symbol macro expansion...?
        *expansion = form;
        return false;

    } else {
        *expansion = form;
        return false;
    }
}

bool macroexpand(lis_obj * genv, lis_obj * form, lis_obj * env, lis_obj ** expansion) {
    return NULL;
}

lis_obj * lisp_macroexpand_1(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"macroexpand-1"))) {
        return NULL;
    }

    lis_obj * form = _list_nth(genv, INT(0), args);
    lis_obj * env = _list_nth(genv, INT(1), args);
    lis_obj * expansion;
    lis_obj * expanded_p;

    if (macroexpand_1(genv, form, env, &expansion)) {
        expanded_p = LIS_T(genv);
    } else {
        expanded_p = LIS_NIL(genv);
    }

    lis_obj * mv = _make_multiple_value();
    LIS_MVAL(mv) = _list_cons(genv, expansion, _list_cons(genv, expanded_p, LIS_NIL(genv)));
    return mv;
}

lis_obj * lisp_macroexpand(lis_obj * genv, lis_obj * args) {
    return NULL;
}
