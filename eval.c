#include <stdio.h>

#include "obj.h"
#include "eval.h"

lis_obj make_t() {
    lis_obj t;
    t.tags = 0x05 << 1;
    return t;
}

lis_obj make_nil() {
    lis_obj nil;
    nil.tags = 0x04 << 1;
    return nil;
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

lis_obj * eval(lis_obj * obj) {
    if (LIS_TAG3(obj) == 0x01) {
        return obj;  // integer
    } else if (LIS_TAG3(obj) == 0x03) {
        return obj;  // char
    } else if (LIS_TAG3(obj) & 0x04) {
        if (LIS_TAG1(obj)) {
            return obj;  // t
        } else {
            return obj;  // nil
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
    } else if (LIS_TAG3(obj) & 0x04) {
        if (LIS_TAG1(obj)) {
            printf("t");
        } else {
            printf("nil");
        }
    }
}
