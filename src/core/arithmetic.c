#include "obj.h"
#include "lerror.h"
#include "lstring.h"
#include "basic_stream.h"
#include "list.h"
#include "print.h"
#include "eval.h"

#include "arithmetic.h"

lis_obj * lisp_integerp(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"integerp"))) {
        return NULL;
    }

    lis_obj * arg = _list_nth(genv, _make_int(0), args);
    if (LIS_TAG_BASE(arg) == LIS_TAG_BASE_INT) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * _int_zerop(lis_obj * genv, lis_obj * num) {
    if (LIS_TAG_BASE(num) != LIS_TAG_BASE_INT) {
        not_int_error(genv, num);
        return NULL;
    }

    if (num->data.num == 0) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * lisp_zerop(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"zerop"))) {
        return NULL;
    }

    return _int_zerop(genv, _list_nth(genv, _make_int(0), args));
}

lis_obj * _int_plusp(lis_obj * genv, lis_obj * num) {
    if (LIS_TAG_BASE(num) != LIS_TAG_BASE_INT) {
        not_int_error(genv, num);
        return NULL;
    }

    if (num->data.num > 0) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * lisp_plusp(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"plusp"))) {
        return NULL;
    }

    return _int_plusp(genv, _list_nth(genv, _make_int(0), args));
}

lis_obj * _int_minusp(lis_obj * genv, lis_obj * num) {
    if (LIS_TAG_BASE(num) != LIS_TAG_BASE_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, num, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (num->data.num < 0) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * lisp_minusp(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"minusp"))) {
        return NULL;
    }

    return _int_minusp(genv, _list_nth(genv, _make_int(0), args));
}

lis_obj * _int_equal(lis_obj * genv, lis_obj * a, lis_obj * b) {
    if (LIS_TAG_BASE(a) != LIS_TAG_BASE_INT) {
        not_int_error(genv, a);
        return NULL;
    }

    if (LIS_TAG_BASE(b) != LIS_TAG_BASE_INT) {
        not_int_error(genv, b);
        return NULL;
    }

    if (a->data.num == b->data.num) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * lisp_equal(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"="))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _int_equal(genv, arg1, arg2);
}

lis_obj * _int_add(lis_obj * genv, lis_obj * a, lis_obj * b) {
    if (LIS_TAG_BASE(a) != LIS_TAG_BASE_INT) {
        not_int_error(genv, a);
        return NULL;
    }

    if (LIS_TAG_BASE(b) != LIS_TAG_BASE_INT) {
        not_int_error(genv, b);
        return NULL;
    }

    return _make_int(a->data.num + b->data.num);
}

lis_obj * lisp_add(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"+"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _int_add(genv, arg1, arg2);
}

lis_obj * _int_sub(lis_obj * genv, lis_obj * a, lis_obj * b) {
    if (LIS_TAG_BASE(a) != LIS_TAG_BASE_INT) {
        not_int_error(genv, a);
        return NULL;
    }

    if (LIS_TAG_BASE(b) != LIS_TAG_BASE_INT) {
        not_int_error(genv, b);
        return NULL;
    }

    return _make_int(a->data.num - b->data.num);
}

lis_obj * lisp_sub(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"-"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _int_sub(genv, arg1, arg2);
}

lis_obj * _int_mul(lis_obj * genv, lis_obj * a, lis_obj * b) {
    if (LIS_TAG_BASE(a) != LIS_TAG_BASE_INT) {
        not_int_error(genv, a);
        return NULL;
    }

    if (LIS_TAG_BASE(b) != LIS_TAG_BASE_INT) {
        not_int_error(genv, b);
        return NULL;
    }

    return _make_int(a->data.num * b->data.num);
}

lis_obj * lisp_mul(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"*"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _int_mul(genv, arg1, arg2);
}

lis_obj * _int_div(lis_obj * genv, lis_obj * a, lis_obj * b) {
    if (LIS_TAG_BASE(a) != LIS_TAG_BASE_INT) {
        not_int_error(genv, a);
        return NULL;
    }

    if (LIS_TAG_BASE(b) != LIS_TAG_BASE_INT) {
        not_int_error(genv, b);
        return NULL;
    }

    if (b->data.num == 0) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"division by zero."));
        return NULL;
    }

    return _make_int(a->data.num / b->data.num);
}

lis_obj * lisp_div(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"/"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _int_div(genv, arg1, arg2);
}