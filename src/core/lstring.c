#include <stdbool.h>
#include <stdlib.h>
#include <uchar.h>

#include "obj.h"
#include "lerror.h"
#include "stream.h"
#include "list.h"
#include "print.h"
#include "eval.h"

#include "lstring.h"


lis_obj * u2lstring(const char32_t * ustr, size_t ustr_size) {
    lis_obj * str = _make_string();
    lis_string * _str = (lis_string *)malloc(sizeof(lis_string));
    lis_char * body = (lis_char *)malloc(sizeof(lis_char) * ustr_size);

    for (int i=0; i<ustr_size-1; i++) {
        body[i] = ustr[i];
    }

    _str->body = body;
    _str->size = ustr_size - 1;
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

lis_obj * lisp_string_equal(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"string="))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, INT(0), args);
    lis_obj * arg2 = _list_nth(genv, INT(1), args);
    if (LIS_TAG_BASE(arg1) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(arg1) != LIS_TAG_TYPE_STR) {
        not_string_error(genv, arg1);
        return NULL;
    }

    if (LIS_TAG_BASE(arg2) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(arg2) != LIS_TAG_TYPE_STR) {
        not_string_error(genv, arg2);
        return NULL;
    }

    if (_string_equal(arg1->data.str, arg2->data.str)) {
        return LIS_T(genv);
    } else {
        return LIS_NIL(genv);
    }
}

lis_obj * lisp_stringp(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"stringp"))) {
        return NULL;
    }

    lis_obj * arg = _list_nth(genv, INT(0), args);
    if (LIS_TAG_BASE(arg) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(arg) != LIS_TAG_TYPE_STR) {
        return LIS_NIL(genv);
    } else {
        return LIS_T(genv);
    }
}

lis_obj * _string_length(lis_obj * genv, lis_obj * str) {
    if (LIS_TAG_BASE(str) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(str) != LIS_TAG_TYPE_STR) {
        not_string_error(genv, str);
        return NULL;
    }

    return INT(LIS_STR(str)->size);
}

lis_obj * _string_char(lis_obj * genv, lis_obj * str, lis_obj * n) {
    if (LIS_TAG_BASE(str) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(str) != LIS_TAG_TYPE_STR) {
        not_string_error(genv, str);
        return NULL;
    }

    if (LIS_TAG_BASE(str) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(str) != LIS_TAG_TYPE_STR) {
        not_int_error(genv, n);
        return NULL;
    }

    lis_int pos = n->data.num;
    if (pos < 0 || pos >= LIS_STR(str)->size) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"out of range"));
        return NULL;
    }

    return _make_char(LIS_STR(str)->body[pos]);
}
