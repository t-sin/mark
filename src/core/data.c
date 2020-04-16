#include "obj.h"
#include "lstring.h"
#include "lerror.h"
#include "basic_stream.h"
#include "eval.h"
#include "print.h"
#include "list.h"
#include "environment.h"

#include "data.h"

lis_obj * lis_sf_quote(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"quote"))) {
        return NULL;
    }
    return args->data.cons->car;
}

lis_obj * lis_sf_function(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    lis_obj * sym = args->data.cons->car;
    if (LIS_TAG_BASE(sym) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(sym) != LIS_TAG_TYPE_SYM) {
        lis_stream * stream = genv->data.env->env.global->stream_stderr->data.stream;
        print(genv, sym, stream);
        stream_write_string(stream, LSTR(U" is not a symbol."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(stream));
        return NULL;
    }
    if (sym->data.sym->fn == NULL) {
        return genv->data.env->env.global->symbol_nil;
    } else {
        return sym->data.sym->fn;
    }
}

lis_obj * lis_sf_setq(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"setq"))) {
        return NULL;
    }

    lis_obj * sym = _list_nth(genv, _make_int(0), args);
    lis_obj * val = eval(genv, lenv, _list_nth(genv, _make_int(1), args));

    if (lenv == NULL) {
        sym->data.sym->value = val;
        return val;
    }

    lis_obj * value = get_lexical_value(lenv, sym);

    if (value != NULL) {
        find_and_set_lexical_value(lenv, sym, val);
    } else {
        sym->data.sym->value = val;
    }

    return val;
}

lis_obj * lis_sf_progn(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    lis_obj * ret = LIS_GENV(genv)->symbol_nil;

    lis_obj * rest = args;
    while (rest != LIS_GENV(genv)->symbol_nil) {
        ret = eval(genv, lenv, _list_car(genv, rest));
        rest = _list_cdr(genv, rest);
    }

    return ret;
}

lis_obj * lis_sf_if(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (!check_arglen(genv, args, 3, LSTR(U"if"))) {
        return NULL;
    }

    lis_obj * condition = eval(genv, lenv, _list_nth(genv, _make_int(0), args));
    if (condition != LIS_GENV(genv)->symbol_nil) {
        return eval(genv, lenv, _list_nth(genv, _make_int(1), args));
    } else {
        return eval(genv, lenv, _list_nth(genv, _make_int(2), args));
    }
}

lis_obj * lis_sf_let(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    lis_obj * new_lenv = _make_lexical_env(LIS_ENV_LEXICAL);
    LIS_ENV(new_lenv)->parent = lenv;

    lis_obj * binding_list = _list_car(genv, args);
    if (!_list_listp(genv, binding_list)) {
        not_list_error(genv, binding_list);
        return NULL;
    }

    lis_obj * rest = binding_list;
    while (rest != LIS_GENV(genv)->symbol_nil) {
        lis_obj * binding = _list_car(genv, rest);

        if (!check_arglen(genv, binding, 2, LSTR(U"binding list of let"))) {
            return NULL;
        }

        lis_obj * name = _list_car(genv, binding);
        lis_obj * value = eval(genv, lenv, _list_car(genv, _list_cdr(genv, binding)));
        set_lexical_value(new_lenv, name, value);

        rest = _list_cdr(genv, rest);
    }

    return lis_sf_progn(genv, new_lenv, _list_cdr(genv, args));
}

bool has_free_variables(lis_obj * genv, lis_obj * exp) {
    // if (_list_consp(exp)) {
    // } else if (
    return true;
}

lis_obj * lis_sf_lambda(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (_list_length(genv, args)->data.num < 2) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        stream_write_string(buffer, LSTR(U"malformed lambda expression: "));
        print(genv, args, buffer);
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    lis_obj * _lambdalist = _list_nth(genv, _make_int(0), args);
    lis_lambdalist * lambdalist = validate_lambdalist(genv, lenv, _lambdalist);
    lis_obj * body = _list_cdr(genv, args);

    if (lambdalist == NULL) {
        return NULL;
    }

    if (has_free_variables(genv, body)) {
        return _make_lisp_closure(lambdalist, body, lenv);
    } else {
        return _make_lisp_function(lambdalist, body);
    }
}

lis_obj * lis_sf_multiple_value_call(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
   if (!check_argge(genv, args, 1, LSTR(U"multiple-value-call"))) {
        return NULL;
    }

    lis_obj * fn = eval(genv, lenv, _list_nth(genv, INT(0), args));
    if (LIS_TAG_BASE(fn) != LIS_TAG_BASE_BUILTIN ||
        !(LIS_TAG_TYPE(fn) == LIS_TAG_TYPE_FN ||
          LIS_TAG_TYPE(fn) == LIS_TAG_TYPE_CLS)) {
        not_function_error(genv, fn);
        return NULL;
    }

    lis_obj * rest = _list_cdr(genv, args);
    lis_obj * new_args = LIS_NIL;
    lis_obj * tail = new_args;
    while (rest != LIS_NIL) {
        lis_obj * car = eval(genv, lenv, _list_car(genv, rest));

        if (LIS_TAG_BASE(car) == LIS_TAG_BASE_BUILTIN &&
            LIS_TAG_TYPE(car) == LIS_TAG_TYPE_MVAL) {

            lis_obj * mval_rest = LIS_MVAL(car);
            while (mval_rest != LIS_NIL) {
                lis_obj * val = _list_car(genv, mval_rest);
                lis_obj * cons = _list_cons(genv, val, LIS_NIL);

                if (new_args == LIS_NIL) {
                    tail = new_args = cons;
                } else {
                    LIS_CONS(tail)->cdr = cons;
                    tail = cons;
                }

                mval_rest = _list_cdr(genv, mval_rest);
            }

        } else {
            lis_obj * cons = _list_cons(genv, car, LIS_NIL);

            if (new_args == LIS_NIL) {
                tail = new_args = cons;

            } else {
                LIS_CONS(tail)->cdr = cons;
                tail = cons;
            }
        }

        rest = _list_cdr(genv, rest);
    }

    return apply(genv, fn, new_args);
}

lis_obj * lisp_values(lis_obj * genv, lis_obj * args) {
    if (args == LIS_NIL) {
        return LIS_NIL;
    }

    lis_obj * rest = args;
    lis_obj * list = LIS_NIL;
    while (rest != LIS_GENV(genv)->symbol_nil) {
        list = _list_cons(genv, _list_car(genv, rest), list);
        rest = _list_cdr(genv, rest);
    }

    lis_obj * mv = _make_multiple_value();
    LIS_MVAL(mv) = _list_reverse(genv, list);
    return mv;
}
