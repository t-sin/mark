#include <stdio.h>

#include "obj.h"
#include "print.h"

void print_string(lis_obj * str) {
    for (size_t i=0; i < str->data.str->size; i++) {
        printf("%c", str->data.str->body[i]);
    }
}

void print_cons(lis_obj * car, lis_obj * cdr) {
    print(car);
    if (LIS_TAG3(cdr) == LIS_TAG3_BUILTIN && LIS_TAG_TYPE(cdr) == LIS_TAG_TYPE_CONS) {
        printf(" ");
        print_cons(cdr->data.cons->car, cdr->data.cons->cdr);
    } else if (LIS_TAG3(cdr) == LIS_TAG3_SPECIAL && LIS_TAG_TYPE(cdr) == LIS_TAG_TYPE_NIL) {
        ;
    } else {
        printf(" . ");
        print(cdr);
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
            print_string(obj);
            printf("\"");
            break;

        case LIS_TAG_TYPE_TS:
        case LIS_TAG_TYPE_SYM:
            print_string(obj->data.sym->name);
            break;

        case LIS_TAG_TYPE_CONS:
            printf("(");
            print_cons(obj->data.cons->car, obj->data.cons->cdr);
            printf(")");
            break;

        case LIS_TAG_TYPE_ENV:
        case LIS_TAG_TYPE_FN:
        case LIS_TAG_TYPE_CLS:
        case LIS_TAG_TYPE_PKG:
            break;
        }
    } else {
        printf("unknown object: %x\n", obj->tags);
    }
}
