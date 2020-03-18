#include <stdio.h>
#include <stdlib.h>

#include "obj.h"
#include "eval.h"

lis_obj * eval(lis_obj * obj) {
    if (LIS_TAG3(obj) == LIS_TAG3_INT) {
        return obj;  // integer
    } else if (LIS_TAG3(obj) == LIS_TAG3_CHAR) {
        return obj;  // char
    } else if (LIS_TAG3(obj) == LIS_TAG3_SPECIAL) {
        if (LIS_TAG_TYPE(obj) == LIS_TAG_TYPE_T) {
            return obj;  // t
        } else if (LIS_TAG_TYPE(obj) == LIS_TAG_TYPE_NIL) {
            return obj;  // nil
        }
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
            // TODO: apply
            return obj;

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
