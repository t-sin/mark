#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "list.h"
#include "eval.h"
#include "print.h"

#include "sequence.h"

void type_error(lis_obj * genv, lis_obj * obj) {
    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    print(genv, obj, buffer);
    stream_write_string(buffer, LSTR(U" is not a sequence."));
    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
}

lis_obj * _seq_length(lis_obj * genv, lis_obj * seq) {
    if (seq == LIS_GENV(genv)->symbol_nil) {
        return _make_int(0);

    } else if (LIS_TAG3(seq) == LIS_TAG3_BUILTIN) {
        switch (LIS_TAG_TYPE(seq)) {
        case LIS_TAG_TYPE_ARY:
            return NULL;

        case LIS_TAG_TYPE_STR:
            return NULL;

        case LIS_TAG_TYPE_CONS:
            return _list_length(genv, seq);

        default:
            type_error(genv, seq);
            return NULL;
        }

    } else {
        // user defined sequences??
        type_error(genv, seq);
        return NULL;
    }
}

lis_obj * seq_length(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"length"))) {
        return NULL;
    }

    return _seq_length(genv, _list_nth(genv, _make_int(0), args));
}

lis_obj * _seq_elt(lis_obj * genv, lis_obj * seq, lis_obj * n) {
    if (seq == LIS_GENV(genv)->symbol_nil) {
        return seq;

    } else if (LIS_TAG3(seq) == LIS_TAG3_BUILTIN) {
        switch (LIS_TAG_TYPE(seq)) {
        case LIS_TAG_TYPE_ARY:
            return NULL;

        case LIS_TAG_TYPE_STR:
            return NULL;

        case LIS_TAG_TYPE_CONS:
            return _list_nth(genv, n, seq);

        default:
            type_error(genv, seq);
            return NULL;
        }

    } else {
        // user defined sequences??
        type_error(genv, seq);
        return NULL;
    }
}

lis_obj * seq_elt(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"elt"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _seq_elt(genv, arg1, arg2);
}
