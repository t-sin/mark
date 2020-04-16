#include "obj.h"
#include "lerror.h"
#include "lstring.h"
#include "list.h"
#include "eval.h"
#include "basic_stream.h"

#include "stream.h"

lis_obj * lisp_listen(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"listen"))) {
        return NULL;
    }

    lis_obj * stream_obj = _list_nth(genv, INT(0), args);

    if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
        not_stream_error(genv, stream_obj);
        return NULL;
    }

    if (stream_listen_p(LIS_STREAM(stream_obj))) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * lisp_force_output(lis_obj * genv, lis_obj * args) {
    lis_obj * stream_obj = _list_nth(genv, INT(0), args);
    lis_obj * stream = LIS_GENV(genv)->stream_stdout;

    if (stream_obj != LIS_GENV(genv)->symbol_nil) {
        if (LIS_TAG_BASE(stream_obj) != LIS_TAG_BASE_BUILTIN ||
            LIS_TAG_TYPE(stream_obj) != LIS_TAG_TYPE_STRM) {
            not_stream_error(genv, stream_obj);
            return NULL;
        }
        stream = stream_obj;
    }

    stream_listen_p(LIS_STREAM(stream));
    return LIS_GENV(genv)->symbol_nil;
}

// lis_obj * lisp_peek_byte(lis_obj * genv, lis_obj * args) {
    
// }

// lis_obj * lisp_read_byte(lis_obj * genv, lis_obj * args) {}
// lis_obj * lisp_write_byte(lis_obj * genv, lis_obj * args) {}

// lis_obj * lisp_peek_char(lis_obj * genv, lis_obj * args) {}
// lis_obj * lisp_read_char(lis_obj * genv, lis_obj * args) {}
// lis_obj * lisp_unread_char(lis_obj * genv, lis_obj * args) {}
// lis_obj * lisp_write_char(lis_obj * genv, lis_obj * args) {}

// lis_obj * lisp_write_string(lis_obj * genv, lis_obj * args) {}
