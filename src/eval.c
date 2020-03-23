#include <stdio.h>
#include <stdlib.h>

#include "obj.h"
#include "lstring.h"
#include "runtime.h"
#include "eval.h"

lis_obj * apply(lis_obj * obj, lis_obj * args) {
    return NULL;
}

lis_obj * eval_cons(lis_runtime * runtime, lis_obj * cons) {
    // TODO: apply
    lis_obj * name = cons->data.cons->car;
    lis_obj * args = cons->data.cons->cdr;

    if (LIS_TAG3(name) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(name) != LIS_TAG_TYPE_SYM) {
        printf("eval_cons(): is's not a symbol!\n");
        return NULL;
    }

    if (name == runtime->special_forms[LIS_SPECIAL_FORM_QUOTE]) {
        // TODO: length check = 1
        if (LIS_TAG3(args) == LIS_TAG3_BUILTIN &&
            LIS_TAG_TYPE(args) == LIS_TAG_TYPE_CONS) {
            return args->data.cons->car;

        } else {
            printf("invalid argument\n");
            return NULL;
        }

    } else {
        printf("unknown special operator\n");
        return NULL;
    }
}

lis_obj * eval(lis_runtime * runtime, lis_obj * obj) {
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
            return eval_cons(runtime, obj);

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
