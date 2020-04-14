#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "list.h"
#include "print.h"
#include "environment.h"

#include "eval.h"

bool check_arglen(lis_obj * genv, lis_obj * args, int len, lis_obj * opname) {
    if (_list_length(genv, args)->data.num != len) {
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

bool check_argeven(lis_obj * genv, lis_obj * args, lis_obj * opname) {
    if (_list_length(genv, args)->data.num % 2 == 0) {
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

bool validate_lambdalist(lis_obj * genv, lis_obj * lambdalist) {
    return true;
}

lis_obj * bind_lambdalist(lis_obj * fn, lis_obj * args) {
    return NULL;
}

lis_obj * apply(lis_obj * genv, lis_obj * fn, lis_obj * args) {
    if (LIS_FN(fn)->type != LIS_FUNC_RAW &&
        LIS_FN(fn)->type != LIS_FUNC_NORMAL) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, fn, buffer);
        stream_write_string(buffer, LSTR(U" is not a lisp function."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));

        return NULL;
    }

    if (LIS_FN(fn)->type == LIS_FUNC_RAW) {
        return LIS_FN(fn)->body.raw(genv, args);
    } else {
        lis_obj * new_lenv = bind_lambdalist(fn, args);
        return eval(genv, new_lenv, LIS_FN(fn)->body.lisp);
    }
}

lis_obj * eval_apply(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"apply"))) {
        return NULL;
    }

    lis_obj * fn = _list_nth(genv, _make_int(0), args);
    lis_obj * lambdalist = _list_nth(genv, _make_int(1), args);
    return apply(genv, fn, lambdalist);
}

lis_obj * eval_args(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (args == LIS_GENV(genv)->symbol_nil) {
        return LIS_GENV(genv)->symbol_nil;
    } else if (LIS_TAG3(args) != LIS_TAG3_BUILTIN ||
               LIS_TAG_TYPE(args) != LIS_TAG_TYPE_CONS) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_OUT, LIS_STREAM_TEXT);
        print(genv, args, buffer);
        stream_write_string(buffer, LSTR(U" is not a list."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));

        return NULL;
    } else {
        lis_obj * cons = _make_cons();
        LIS_CONS(cons)->car = eval(genv, lenv, LIS_CONS(args)->car);
        LIS_CONS(cons)->cdr = eval_args(genv, lenv, LIS_CONS(args)->cdr);
        return cons;
    }
}

lis_obj * eval_cons(lis_obj * genv, lis_obj * lenv, lis_obj * cons) {
    lis_obj * name = cons->data.cons->car;
    lis_obj * cdr = cons->data.cons->cdr;

    if (LIS_TAG3(name) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(name) != LIS_TAG_TYPE_SYM) {
        printf("eval_cons(): is's not a symbol!\n");
        return NULL;
    }

    if (cdr != LIS_GENV(genv)->symbol_nil &&
        !(LIS_TAG3(cdr) == LIS_TAG3_BUILTIN &&
          LIS_TAG_TYPE(cdr) == LIS_TAG_TYPE_CONS)) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_OUT, LIS_STREAM_TEXT);
        print(genv, cdr, buffer);
        stream_write_string(buffer, LSTR(U" is not a list."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (name->data.sym->fn != NULL) {
        lis_obj * fn = name->data.sym->fn;
        assert(LIS_TAG3(fn) == LIS_TAG3_BUILTIN);
        assert(LIS_TAG_TYPE(fn) == LIS_TAG_TYPE_FN);

        lis_obj * args;
        lis_obj * ret;
        switch (fn->data.fn->type) {
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
            ret =  fn->data.fn->body.sf(genv, lenv, cdr);
            if (LIS_GENV(genv)->error != NULL) return NULL;
            return ret;

        case LIS_FUNC_MACRO:
            break;

        default:
            printf("invalid function\n");
            assert(false);
            break;
        }

        return NULL;

    } else {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        stream_write_string(buffer, LSTR(U"unknown operator: "));
        stream_write_string(buffer, LIS_SYM(name)->name);
        stream_write_string(buffer, LSTR(U"."));

        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }
}

lis_obj * eval(lis_obj * genv, lis_obj * lenv, lis_obj * obj) {
    if (obj == NULL) return NULL;

    if (LIS_TAG3(obj) == LIS_TAG3_INT) {
        return obj;  // integer
    } else if (LIS_TAG3(obj) == LIS_TAG3_CHAR) {
        return obj;  // char

    } else if (LIS_TAG3(obj) == LIS_TAG3_BUILTIN) {
        // built-in types
        lis_obj * val;
        lis_stream * buffer;

        switch (LIS_TAG_TYPE(obj)) {
        case LIS_TAG_TYPE_ARY:
        case LIS_TAG_TYPE_STR:
        case LIS_TAG_TYPE_TS:
            return obj;

        case LIS_TAG_TYPE_SYM:
            val = get_lexical_value(lenv, obj);
            if (val != NULL) {
                return val;
            } else {
                return obj->data.sym->value;
            }

        case LIS_TAG_TYPE_CONS:
            return eval_cons(genv, lenv, obj);

        case LIS_TAG_TYPE_ENV:
        case LIS_TAG_TYPE_FN:
        case LIS_TAG_TYPE_CLS:
        case LIS_TAG_TYPE_PKG:
            return obj;
            break;

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

lis_obj * eval_eval(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"eval"))) {
        return NULL;
    }
    return eval(genv, NULL, _list_nth(genv, _make_int(0), args));
}
