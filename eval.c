#include <stdio.h>
#include <stdlib.h>

#include "obj.h"
#include "eval.h"

lis_obj make_nil() {
    lis_obj nil;
    nil.tags = 0x05 << 1 | 0x00 << 4;
    return nil;
}

lis_obj make_t() {
    lis_obj t;
    t.tags = 0x05 << 1 | 0x02 << 4;
    return t;
}


lis_obj make_int(lis_int n) {
    lis_obj num;
    num.tags = 0x01 << 1;
    num.data.num = n;
    return num;
}

lis_obj make_char(lis_int ch) {
    lis_obj c;
    c.tags = 0x03 << 1;
    c.data.ch = ch;
    return c;
}

lis_obj make_array() {
    lis_obj arr;
    arr.tags = 0x06 << 1;
    arr.data.array = malloc(sizeof(lis_array));
    arr.data.array->size = 0;
    arr.data.array->arr = NULL;
    return arr;
}

lis_obj * eval(lis_obj * obj) {
    if (LIS_TAG3(obj) == 0x01) {
        return obj;  // integer
    } else if (LIS_TAG3(obj) == 0x03) {
        return obj;  // char
    } else if (LIS_TAG3(obj) == 0x05) {
        if (LIS_TAG_TYPE(obj) == 0x02) {
            return obj;  // t
        } else if (LIS_TAG_TYPE(obj) == 0x00) {
            return obj;  // nil
        }
    } else if (LIS_TAG3(obj) == 0x06) {
        // built-in types
        switch (LIS_TAG_TYPE(obj)) {
        case 0x00:  // array
            return obj;
        case 0x01:  // timestamp
        case 0x02:  // symbol
        case 0x03:  // cons
        case 0x04:  // environment
        case 0x05:  // function
        case 0x06:  // closure
        case 0x07:  // package
            break;
        }
    } else {
        return obj;
    }
}

void print(lis_obj * obj) {
    if (LIS_TAG3(obj) == 0x01) {
        printf("%d", obj->data.num);
    } else if (LIS_TAG3(obj) == 0x03) {
        printf("#\\%c", obj->data.ch);
    } else if (LIS_TAG3(obj) == 0x05) {
        if (LIS_TAG_TYPE(obj) == 0x02) {
            printf("t");
        } else if (LIS_TAG_TYPE(obj) == 0x00) {
            printf("nil");
        } else {
            printf("unknown special value: %d\n", obj->tags);
        }
    } else if (LIS_TAG3(obj) == 0x06) {
        // built-in types
        switch (LIS_TAG_TYPE(obj)) {
        case 0x00:  // array
            printf("#(");
            for (size_t i=0; i < obj->data.array->size; i++) {
                print(&obj->data.array->arr[i]);
                if (i+1 < obj->data.array->size) {
                    printf(" ");
                }
            }
            printf(")");
            break;
        case 0x01:  // timestamp
        case 0x02:  // symbol
        case 0x03:  // cons
        case 0x04:  // environment
        case 0x05:  // function
        case 0x06:  // closure
        case 0x07:  // package
            break;
        }
    } else {
        printf("unknown object: %d\n", obj->tags);
    }
}
