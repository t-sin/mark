#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "environment.h"
#include "eval.h"
#include "print.h"

lis_obj * apply(lis_obj * genv, lis_obj * fn, lis_obj * args) {
    if (LIS_FN(fn)->raw_body != NULL) {
        return fn->data.fn->raw_body(genv, args);
    } else {
        // do check lambda list
        // do apply
        printf("aaaaaaaaaaaaaaaaaa");
        return NULL;
    }
}

lis_obj * eval_args(lis_obj * genv, lis_obj * args) {
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
        LIS_CONS(cons)->car = eval(genv, LIS_CONS(args)->car);
        LIS_CONS(cons)->cdr = eval_args(genv, LIS_CONS(args)->cdr);
        return cons;
    }
}

lis_obj * eval_cons(lis_obj * genv, lis_obj * cons) {
    lis_obj * name = cons->data.cons->car;
    lis_obj * cdr = cons->data.cons->cdr;

    if (LIS_TAG3(name) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(name) != LIS_TAG_TYPE_SYM) {
        printf("eval_cons(): is's not a symbol!\n");
        return NULL;
    }

    if (LIS_TAG3(cdr) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(cdr) != LIS_TAG_TYPE_CONS) {
        printf("args is not a list\n");
        return NULL;
    }

    if (name->data.sym->fn != NULL) {
        lis_obj * fn = name->data.sym->fn;
        assert(LIS_TAG3(fn) == LIS_TAG3_BUILTIN);
        assert(LIS_TAG_TYPE(fn) == LIS_TAG_TYPE_FN);

        lis_obj * args;
        switch (fn->data.fn->type) {
        case LIS_FUNC_NORMAL:
            args = eval_args(genv, cdr);
            if (args == NULL) return NULL;
            return apply(genv, fn, args);
            break;
        case LIS_FUNC_SPECIAL_FORM:
            return fn->data.fn->raw_body(genv, cdr);
        case LIS_FUNC_MACRO:
            break;
        default:
            printf("invalid function\n");
            assert(false);
            break;
        }

        return NULL;

    } else {
        printf("unknown operator\n");
        return NULL;
    }
}

lis_obj * eval(lis_obj * genv, lis_obj * obj) {
    if (LIS_TAG3(obj) == LIS_TAG3_INT) {
        return obj;  // integer
    } else if (LIS_TAG3(obj) == LIS_TAG3_CHAR) {
        return obj;  // char

    } else if (LIS_TAG3(obj) == LIS_TAG3_BUILTIN) {
        // built-in types
        switch (LIS_TAG_TYPE(obj)) {
        case LIS_TAG_TYPE_ARY:
            return obj;

        case LIS_TAG_TYPE_STR:
            return obj;

        case LIS_TAG_TYPE_TS:
        case LIS_TAG_TYPE_SYM:
            return obj->data.sym->value;

        case LIS_TAG_TYPE_CONS:
            return eval_cons(genv, obj);

        case LIS_TAG_TYPE_ENV:
        case LIS_TAG_TYPE_FN:
        case LIS_TAG_TYPE_CLS:
        case LIS_TAG_TYPE_PKG:
            break;
        }
    } else {
        return obj;
    }
}
