#include <stdbool.h>
#include <stdlib.h>
#include <uchar.h>

#include "obj.h"
#include "lstring.h"

lis_obj * u2lstring(const char32_t * ustr, size_t ustr_size) {
    lis_obj * str = _make_string();
    lis_string * _str = (lis_string *)malloc(sizeof(lis_string));
    size_t size = 0;
    lis_char * body = (lis_char *)malloc(sizeof(lis_char) * ustr_size);
    
    for (int i=0; i<ustr_size; i++) {
        body[i] = ustr[i];
        size++;
    }

    _str->body = body;
    _str->size = size - 1;
    str->data.str = _str;

    return str;
}

bool string_equal(lis_string * s1, lis_string * s2) {
    if (s1->size != s2->size) {
        return false;
    }

    for (int i=0; i<s1->size; i++) {
        if (s1->body[i] != s2->body[i]) {
            return false;
        }
    }

    return true;
}
