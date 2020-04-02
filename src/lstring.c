#include <stdbool.h>
#include <stdlib.h>
#include <uchar.h>

#include "obj.h"
#include "stream.h"
#include "list.h"
#include "print.h"
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

bool _string_equal(lis_string * s1, lis_string * s2) {
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

lis_obj * string_equal(lis_obj * genv, lis_obj * args) {
    if (_list_length(genv, args)->data.num != 2) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"wrong number of args for `string=`."));
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    if (LIS_TAG3(arg1) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(arg1) != LIS_TAG_TYPE_STR) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, arg1, buffer);
        stream_write_string(buffer, LSTR(U" is not an string."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (LIS_TAG3(arg2) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(arg2) != LIS_TAG_TYPE_STR) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, arg2, buffer);
        stream_write_string(buffer, LSTR(U" is not an string."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (_string_equal(arg1->data.str, arg2->data.str)) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * stringp(lis_obj * genv, lis_obj * args) {
    lis_obj * arg = _list_nth(genv, _make_int(0), args);
    if (LIS_TAG3(arg) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(arg) != LIS_TAG_TYPE_STR) {
        return LIS_GENV(genv)->symbol_nil;
    } else {
        return LIS_GENV(genv)->symbol_t;
    }
}
