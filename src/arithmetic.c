#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "list.h"
#include "print.h"
#include "eval.h"

lis_obj * int_integerp(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"integerp"))) {
        return NULL;
    }

    lis_obj * arg = _list_nth(genv, _make_int(0), args);
    if (LIS_TAG3(arg) == LIS_TAG3_INT) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * _int_zerop(lis_obj * genv, lis_obj * num) {
    if (LIS_TAG3(num) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, num, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (num->data.num == 0) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * int_zerop(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"zerop"))) {
        return NULL;
    }

    return _int_zerop(genv, _list_nth(genv, _make_int(0), args));
}

lis_obj * _int_plusp(lis_obj * genv, lis_obj * num) {
    if (LIS_TAG3(num) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, num, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (num->data.num > 0) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * int_plusp(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"plusp"))) {
        return NULL;
    }

    return _int_plusp(genv, _list_nth(genv, _make_int(0), args));
}

lis_obj * _int_minusp(lis_obj * genv, lis_obj * num) {
    if (LIS_TAG3(num) != LIS_TAG3_INT) {
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

lis_obj * int_minusp(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"minusp"))) {
        return NULL;
    }

    return _int_minusp(genv, _list_nth(genv, _make_int(0), args));
}

lis_obj * _int_equal(lis_obj * genv, lis_obj * a, lis_obj * b) {
    if (LIS_TAG3(a) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, a, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (LIS_TAG3(b) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, b, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (a->data.num == b->data.num) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * int_equal(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"="))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _int_equal(genv, arg1, arg2);
}

lis_obj * _int_add(lis_obj * genv, lis_obj * a, lis_obj * b) {
    if (LIS_TAG3(a) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, a, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (LIS_TAG3(b) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, b, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    return _make_int(a->data.num + b->data.num);
}

lis_obj * int_add(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"+"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _int_add(genv, arg1, arg2);
}

lis_obj * _int_sub(lis_obj * genv, lis_obj * a, lis_obj * b) {
    if (LIS_TAG3(a) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, a, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (LIS_TAG3(b) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_OUT, LIS_STREAM_TEXT);
        print(genv, b, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    return _make_int(a->data.num - b->data.num);
}

lis_obj * int_sub(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"-"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _int_sub(genv, arg1, arg2);
}

lis_obj * _int_mul(lis_obj * genv, lis_obj * a, lis_obj * b) {
    if (LIS_TAG3(a) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, a, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (LIS_TAG3(b) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_OUT, LIS_STREAM_TEXT);
        print(genv, b, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    return _make_int(a->data.num * b->data.num);
}

lis_obj * int_mul(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"*"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _int_mul(genv, arg1, arg2);
}

lis_obj * _int_div(lis_obj * genv, lis_obj * a, lis_obj * b) {
    if (LIS_TAG3(a) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, a, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (LIS_TAG3(b) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_OUT, LIS_STREAM_TEXT);
        print(genv, b, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (b->data.num == 0) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"division by zero."));
        return NULL;
    }

    return _make_int(a->data.num / b->data.num);
}

lis_obj * int_div(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"/"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _int_div(genv, arg1, arg2);
}