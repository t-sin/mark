#include <stdio.h>
#include <stdlib.h>

#include "obj.h"
#include "eval.h"

lis_obj make_nil() {
    lis_obj nil;
    nil.tags = LIS_TAG3_SPECIAL << 1 | LIS_TAG_TYPE_NIL << 4;
    return nil;
}

lis_obj make_t() {
    lis_obj t;
    t.tags = LIS_TAG3_SPECIAL << 1 | LIS_TAG_TYPE_T << 4;
    return t;
}


lis_obj make_int(lis_int n) {
    lis_obj num;
    num.tags = LIS_TAG3_INT << 1;
    num.data.num = n;
    return num;
}

lis_obj make_char(lis_int ch) {
    lis_obj c;
    c.tags = LIS_TAG3_CHAR << 1;
    c.data.ch = ch;
    return c;
}

lis_obj make_array() {
    lis_obj arr;
    arr.tags = LIS_TAG3_BUILTIN << 1 | LIS_TAG_TYPE_ARY << 4;
    arr.data.array = malloc(sizeof(lis_array));
    arr.data.array->size = 0;
    arr.data.array->body = NULL;
    return arr;
}

lis_obj make_string() {
    lis_obj str;
    str.tags = LIS_TAG3_BUILTIN << 1 | LIS_TAG_TYPE_STR << 4;
    str.data.str = malloc(sizeof(lis_string));
    str.data.str->size = 0;
    str.data.str->body = NULL;
    return str;
}

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
            return obj;

        case LIS_TAG_TYPE_CONS:
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

void print(lis_obj * obj) {
    if (LIS_TAG3(obj) == LIS_TAG3_INT) {
        printf("%d", obj->data.num);
    } else if (LIS_TAG3(obj) == LIS_TAG3_CHAR) {
        printf("#\\%c", obj->data.ch);
    } else if (LIS_TAG3(obj) == LIS_TAG3_SPECIAL) {
        if (LIS_TAG_TYPE(obj) == LIS_TAG_TYPE_T) {
            printf("t");
        } else if (LIS_TAG_TYPE(obj) == LIS_TAG_TYPE_NIL) {
            printf("nil");
        } else {
            printf("unknown special value: %d\n", obj->tags);
        }
    } else if (LIS_TAG3(obj) == LIS_TAG3_BUILTIN) {
        // built-in types
        switch (LIS_TAG_TYPE(obj)) {
        case LIS_TAG_TYPE_ARY:
            printf("#(");
            for (size_t i=0; i < obj->data.array->size; i++) {
                print(&obj->data.array->body[i]);
                if (i+1 < obj->data.array->size) {
                    printf(" ");
                }
            }
            printf(")");
            break;

        case LIS_TAG_TYPE_STR:
            printf("\"");
            for (size_t i=0; i < obj->data.str->size; i++) {
                printf("%c", obj->data.str->body[i]);
            }
            printf("\"");
            break;

        case LIS_TAG_TYPE_TS:
        case LIS_TAG_TYPE_SYM:
            print(obj->data.sym->name);
            break;

        case LIS_TAG_TYPE_CONS:
        case LIS_TAG_TYPE_ENV:
        case LIS_TAG_TYPE_FN:
        case LIS_TAG_TYPE_CLS:
        case LIS_TAG_TYPE_PKG:
            break;
        }
    } else {
        printf("unknown object: %d\n", obj->tags);
    }
}
