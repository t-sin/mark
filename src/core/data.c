#include "../util/table.h"

#include "obj.h"
#include "lstring.h"
#include "lerror.h"
#include "basic_stream.h"
#include "symbol.h"
#include "package.h"
#include "eval.h"
#include "print.h"
#include "list.h"
#include "environment.h"

#include "data.h"

lis_obj * lis_sf_quote(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"quote"))) {
        return NULL;
    }
    return LIS_CONS(args)->car;
}

lis_obj * lis_sf_function(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    lis_obj * sym = LIS_CONS(args)->car;
    if (LIS_TAG_BASE(sym) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(sym) != LIS_TAG_TYPE_SYM) {
        lis_stream * stream = genv->data.env->env.global->stream_stderr->data.stream;
        print(genv, sym, stream);
        stream_write_string(stream, LSTR(U" is not a symbol."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(stream));
        return NULL;
    }
    if (sym->data.sym->fn == NULL) {
        return LIS_NIL(genv);
    } else {
        return sym->data.sym->fn;
    }
}

lis_obj * lis_sf_setq(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"setq"))) {
        return NULL;
    }

    lis_obj * sym = _list_nth(genv, INT(0), args);
    lis_obj * val = eval(genv, lenv, _list_nth(genv, INT(1), args));

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
    lis_obj * ret = LIS_NIL(genv);

    lis_obj * rest = args;
    while (rest != LIS_NIL(genv)) {
        ret = eval(genv, lenv, _list_car(genv, rest));
        rest = _list_cdr(genv, rest);
    }

    return ret;
}

lis_obj * lis_sf_if(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (!check_arglen(genv, args, 3, LSTR(U"if"))) {
        return NULL;
    }

    lis_obj * condition = eval(genv, lenv, _list_nth(genv, INT(0), args));
    if (condition != LIS_NIL(genv)) {
        return eval(genv, lenv, _list_nth(genv, INT(1), args));
    } else {
        return eval(genv, lenv, _list_nth(genv, INT(2), args));
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
    while (rest != LIS_NIL(genv)) {
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

lis_obj * lis_sf_lambda(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (LIS_INT(_list_length(genv, args)) < 2) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        stream_write_string(buffer, LSTR(U"malformed lambda expression: "));
        print(genv, args, buffer);
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    lis_obj * _lambdalist = _list_nth(genv, INT(0), args);
    lis_lambdalist * lambdalist = validate_lambdalist(genv, lenv, _lambdalist);
    lis_obj * body = _list_cdr(genv, args);

    if (lambdalist == NULL) {
        return NULL;
    }

    return _make_lisp_function(lambdalist, body, lenv);
}

lis_obj * lis_sf_multiple_value_call(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
   if (!check_argge(genv, args, 1, LSTR(U"multiple-value-call"))) {
        return NULL;
    }

    lis_obj * fn = eval(genv, lenv, _list_nth(genv, INT(0), args));
    if (LIS_TAG_BASE(fn) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(fn) == LIS_TAG_TYPE_FN) {
        not_function_error(genv, fn);
        return NULL;
    }

    lis_obj * rest = _list_cdr(genv, args);
    lis_obj * new_args = LIS_NIL(genv);
    lis_obj * tail = new_args;
    while (rest != LIS_NIL(genv)) {
        lis_obj * car = eval(genv, lenv, _list_car(genv, rest));

        if (LIS_TAG_BASE(car) == LIS_TAG_BASE_INTERNAL &&
            LIS_TAG_TYPE(car) == LIS_TAG_TYPE_MVAL) {

            lis_obj * mval_rest = LIS_MVAL(car);
            while (mval_rest != LIS_NIL(genv)) {
                lis_obj * val = _list_car(genv, mval_rest);
                lis_obj * cons = _list_cons(genv, val, LIS_NIL(genv));

                if (new_args == LIS_NIL(genv)) {
                    tail = new_args = cons;
                } else {
                    LIS_CONS(tail)->cdr = cons;
                    tail = cons;
                }

                mval_rest = _list_cdr(genv, mval_rest);
            }

        } else {
            lis_obj * cons = _list_cons(genv, car, LIS_NIL(genv));

            if (new_args == LIS_NIL(genv)) {
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

lis_obj * lisp_eq(lis_obj * genv, lis_obj * args) {
    if (!check_argge(genv, args, 2, LSTR(U"eq"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, INT(0), args);
    lis_obj * arg2 = _list_nth(genv, INT(1), args);
    if (arg1 == arg2) {
        return LIS_T(genv);
    } else {
        return LIS_NIL(genv);
    }
}

lis_obj * lisp_values(lis_obj * genv, lis_obj * args) {
    if (args == LIS_NIL(genv)) {
        return LIS_NIL(genv);
    }

    lis_obj * rest = args;
    lis_obj * list = LIS_NIL(genv);
    while (rest != LIS_NIL(genv)) {
        list = _list_cons(genv, _list_car(genv, rest), list);
        rest = _list_cdr(genv, rest);
    }

    lis_obj * mv = _make_multiple_value();
    LIS_MVAL(mv) = _list_reverse(genv, list);
    return mv;
}

lis_obj * lis_sf_fset(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"%fset"))) {
        return NULL;
    }

    lis_obj * sym = _list_nth(genv, INT(0), args);
    lis_obj * fn = eval(genv, lenv, _list_nth(genv, INT(1), args));

    if (fn == NULL) {
        return NULL;
    }

    if (LIS_TAG_BASE(fn) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(fn) != LIS_TAG_TYPE_FN) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        stream_write_string(buffer, LSTR(U"it's not a function: "));
        print(genv, fn, buffer);
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    // TODO: modify lexical function bindings
    LIS_SYM(sym)->fn = fn;
    return fn;
}

lis_obj * lis_macro_defun(lis_obj * genv, lis_obj * args) {
    if (LIS_INT(_list_length(genv, args)) < 2) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"too few arguments for `defun`"));
        return NULL;
    }

    lis_obj * name = _list_nth(genv, INT(0), args);
    lis_obj * lambdalist = _list_nth(genv, INT(1), args);
    lis_obj * body = _list_cdr(genv, _list_cdr(genv, args));

    lis_obj * lambda_sym;
    intern(genv, LIS_GENV(genv)->lis_package, LSTR(U"lambda"), &lambda_sym);
    lis_obj * lambda_form = _list_cons(genv, lambda_sym, _list_cons(genv, lambdalist, body));

    lis_obj * fset_sym;
    intern(genv, LIS_GENV(genv)->lis_package, LSTR(U"%fset"), &fset_sym);
    lis_obj * fset_form = _list_cons(genv, fset_sym, _list_cons(genv, name, _list_cons(genv, lambda_form, LIS_NIL(genv))));

    lis_obj * progn_sym;
    intern(genv, LIS_GENV(genv)->lis_package, LSTR(U"progn"), &progn_sym);
    lis_obj * quote_sym;
    intern(genv, LIS_GENV(genv)->lis_package, LSTR(U"quote"), &quote_sym);
    lis_obj * quote_form = _list_cons(genv, quote_sym, _list_cons(genv, name, LIS_NIL(genv)));
    lis_obj * expansion = _list_cons(genv, progn_sym, _list_cons(genv, fset_form, _list_cons(genv, quote_form, LIS_NIL(genv))));

    return expansion;
}
