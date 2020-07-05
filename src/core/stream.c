#include "obj.h"
#include "lerror.h"
#include "lstring.h"
#include "list.h"
#include "package.h"
#include "eval.h"
#include "print.h"
#include "basic_stream.h"

#include "stream.h"

lis_obj * lisp_open(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 3, LSTR(U"open"))) {
        return NULL;
    }

    lis_obj * pathname = _list_nth(genv, INT(0), args);
    lis_obj * direction = _list_nth(genv, INT(1), args);
    lis_obj * element_type = _list_nth(genv, INT(2), args);

    if (LIS_TAG_BASE(pathname) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(pathname) != LIS_TAG_TYPE_STR) {
        not_string_error(genv, pathname);
        return NULL;
    }

    lis_obj * sym;
    lis_stream_direction_type dir;
    lis_stream_element_type etype;

    if (intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"input"), &sym), direction == sym) {
        dir = LIS_STREAM_IN;
    } else if (intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"output"), &sym), direction == sym) {
        dir = LIS_STREAM_OUT;
    } else {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        stream_write_string(buffer, LSTR(U":direction should be whether :input or :output"));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"text"), &sym),
        element_type == sym) {
        etype = LIS_STREAM_TEXT;
    } else if (intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"binary"), &sym),
               element_type == sym) {
        etype = LIS_STREAM_BINARY;
    } else {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        stream_write_string(buffer, LSTR(U":element-type should be whether :text or :binary"));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    lis_stream * stream = stream_open(LIS_STR(pathname), dir, etype);
    if (stream == NULL) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        stream_write_string(buffer, LSTR(U"cannot open file: "));
        print(genv, pathname, buffer);
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    return _make_lis_stream(stream);
}

lis_obj * lisp_close(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"close"))) {
        return NULL;
    }

    lis_obj * stream_obj = _list_nth(genv, INT(0), args);

    if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
        not_stream_error(genv, stream_obj);
        return NULL;
    }

    stream_close(LIS_STREAM(stream_obj));
    return LIS_NIL(genv);
}

lis_obj * lisp_listen(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"listen"))) {
        return NULL;
    }

    lis_obj * stream_obj = _list_nth(genv, INT(0), args);

    if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
        not_stream_error(genv, stream_obj);
        return NULL;
    }

    if (stream_listen_p(LIS_STREAM(stream_obj))) {
        return LIS_T(genv);
    } else {
        return LIS_NIL(genv);
    }
}

lis_obj * lisp_force_output(lis_obj * genv, lis_obj * args) {
    lis_obj * stream_obj = _list_nth(genv, INT(0), args);
    lis_obj * stream = LIS_GENV(genv)->stream_stdout;

    if (stream_obj != LIS_NIL(genv)) {
        if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_INTERNAL ||
            LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
            not_stream_error(genv, stream_obj);
            return NULL;
        }
        stream = stream_obj;
    }

    stream_flush(LIS_STREAM(stream));
    return LIS_NIL(genv);
}

lis_obj * lisp_peek_byte(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"peek-byte"))) {
        return NULL;
    }
    
    lis_obj * stream_obj = _list_nth(genv, INT(0), args);
    if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
        not_stream_error(genv, stream_obj);
        return NULL;
    }

    if (LIS_STREAM(stream_obj)->stream == NULL) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"stream is already closed."));
        return NULL;
    }

    lis_byte byte;
    if (!stream_peek_byte(LIS_STREAM(stream_obj), &byte, 1)) {
        lis_obj * sym;
        intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"eof"), &sym);
        return sym;
    } else {
        return _make_int(byte);
    }
}

lis_obj * lisp_read_byte(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"peek-byte"))) {
        return NULL;
    }

    lis_obj * stream_obj = _list_nth(genv, INT(0), args);
    if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
        not_stream_error(genv, stream_obj);
        return NULL;
    }

    if (LIS_STREAM(stream_obj)->stream == NULL) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"stream is already closed."));
        return NULL;
    }

    lis_byte byte;
    if (!stream_read_byte(LIS_STREAM(stream_obj), &byte)) {
        lis_obj * sym;
        intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"eof"), &sym);
        return sym;
    } else {
        return _make_int(byte);
    }
}

lis_obj * lisp_write_byte(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"write-char"))) {
        return NULL;
    }

    lis_obj * byte_obj = _list_nth(genv, INT(0), args);
    lis_obj * stream_obj = _list_nth(genv, INT(1), args);

    if (LIS_TAG_BASE(byte_obj) != LIS_TAG_BASE_INT) {
        not_int_error(genv, byte_obj);
        return NULL;
    }

    if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
        not_stream_error(genv, stream_obj);
        return NULL;
    }

    if (!stream_write_byte(LIS_STREAM(stream_obj), LIS_INT(byte_obj))) {
        return byte_obj;
    } else {
        return LIS_NIL(genv);
    }
}

lis_obj * lisp_peek_char(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"peek-char"))) {
        return NULL;
    }

    lis_obj * stream_obj = _list_nth(genv, INT(0), args);
    if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
        not_stream_error(genv, stream_obj);
        return NULL;
    }

    if (LIS_STREAM(stream_obj)->stream == NULL) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"stream is already closed."));
        return NULL;
    }

    lis_char ch;
    if (!stream_peek_char(LIS_STREAM(stream_obj), &ch)) {
        lis_obj * sym;
        intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"eof"), &sym);
        return sym;
    } else {
        return _make_char(ch);
    }
}

lis_obj * lisp_read_char(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"read-char"))) {
        return NULL;
    }

    lis_obj * stream_obj = _list_nth(genv, INT(0), args);
    if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
        not_stream_error(genv, stream_obj);
        return NULL;
    }

    if (LIS_STREAM(stream_obj)->stream == NULL) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"stream is already closed."));
        return NULL;
    }

    lis_char ch;
    if (!stream_read_char(LIS_STREAM(stream_obj), &ch)) {
        lis_obj * sym;
        intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"eof"), &sym);
        return sym;
    } else {
        return _make_char(ch);
    }
}


lis_obj * lisp_unread_char(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"unread-char"))) {
        return NULL;
    }

    lis_obj * stream_obj = _list_nth(genv, INT(0), args);
    lis_obj * ch_obj = _list_nth(genv, INT(1), args);

    if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
        not_stream_error(genv, stream_obj);
        return NULL;
    }

    if (LIS_TAG_BASE(ch_obj) != LIS_TAG_BASE_CHAR) {
        not_char_error(genv, ch_obj);
        return NULL;
    }

    if (LIS_STREAM(stream_obj)->stream == NULL) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"stream is already closed."));
        return NULL;
    }

    if (!stream_unread_char(LIS_STREAM(stream_obj), LIS_CH(ch_obj))) {
        return LIS_NIL(genv);
    } else {
        return ch_obj;
    }
}

lis_obj * lisp_write_char(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"unread-char"))) {
        return NULL;
    }

    lis_obj * stream_obj = _list_nth(genv, INT(0), args);
    lis_obj * ch_obj = _list_nth(genv, INT(1), args);

    if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
        not_stream_error(genv, stream_obj);
        return NULL;
    }

    if (LIS_TAG_BASE(ch_obj) != LIS_TAG_BASE_CHAR) {
        not_char_error(genv, ch_obj);
        return NULL;
    }

    if (LIS_STREAM(stream_obj)->stream == NULL) {
        LIS_GENV(genv)->error = _make_error(LSTR(U"stream is already closed."));
        return NULL;
    }

    if (!stream_write_char(LIS_STREAM(stream_obj), LIS_CH(ch_obj))) {
        lis_obj * sym;
        intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"eof"), &sym);
        return sym;
    } else {
        return ch_obj;
    }
}

// lis_obj * lisp_write_string(lis_obj * genv, lis_obj * args) {}
