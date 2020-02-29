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

lis_obj make_symbol(lis_obj * name) {
    lis_obj sym;
    sym.tags = LIS_TAG3_BUILTIN << 1 | LIS_TAG_TYPE_SYM << 4;
    sym.data.sym = malloc(sizeof(lis_symbol));
    sym.data.sym->name = name;
    return sym;
}

lis_obj make_cons() {
    lis_obj cons;
    cons.tags = LIS_TAG3_BUILTIN << 1 | LIS_TAG_TYPE_CONS << 4;
    cons.data.cons = malloc(sizeof(lis_cons));
    cons.data.cons->car = NULL;
    cons.data.cons->cdr = NULL;
    return cons;
}

lis_obj * eval_function(lis_obj * fn, lis_obj * args) {
    return fn;
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
