#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "lerror.h"
#include "list.h"
#include "eval.h"
#include "print.h"

#include "sequence.h"

lis_obj * _seq_copy_seq(lis_obj * genv, lis_obj * seq) {
    if (seq == LIS_GENV(genv)->symbol_nil) {
        return seq;

    } else if (LIS_TAG_BASE(seq) == LIS_TAG_BASE_BUILTIN) {
        switch (LIS_TAG_TYPE(seq)) {
        case LIS_TAG_TYPE_ARY:
            return NULL;

        case LIS_TAG_TYPE_STR:
            return NULL;

        case LIS_TAG_TYPE_CONS:
            return NULL;

        default:
            not_seq_error(genv, seq);
            return NULL;
        }

    } else {
        // user defined sequences??
        not_seq_error(genv, seq);
        return NULL;
    }
}

lis_obj * lisp_copy_seq(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"copy-seq"))) {
        return NULL;
    }

    return _seq_copy_seq(genv, _list_nth(genv, _make_int(0), args));
}

lis_obj * _seq_length(lis_obj * genv, lis_obj * seq) {
    if (seq == LIS_GENV(genv)->symbol_nil) {
        return _make_int(0);

    } else if (LIS_TAG_BASE(seq) == LIS_TAG_BASE_BUILTIN) {
        switch (LIS_TAG_TYPE(seq)) {
        case LIS_TAG_TYPE_ARY:
            return NULL;

        case LIS_TAG_TYPE_STR:
            return _string_length(genv, seq);

        case LIS_TAG_TYPE_CONS:
            return _list_length(genv, seq);

        default:
            not_seq_error(genv, seq);
            return NULL;
        }

    } else {
        // user defined sequences??
        not_seq_error(genv, seq);
        return NULL;
    }
}

lis_obj * lisp_length(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"length"))) {
        return NULL;
    }

    return _seq_length(genv, _list_nth(genv, _make_int(0), args));
}

lis_obj * _seq_elt(lis_obj * genv, lis_obj * seq, lis_obj * n) {
    if (seq == LIS_GENV(genv)->symbol_nil) {
        return seq;

    } else if (LIS_TAG_BASE(seq) == LIS_TAG_BASE_BUILTIN) {
        switch (LIS_TAG_TYPE(seq)) {
        case LIS_TAG_TYPE_ARY:
            return NULL;

        case LIS_TAG_TYPE_STR:
            return _string_char(genv, seq, n);

        case LIS_TAG_TYPE_CONS:
            return _list_nth(genv, n, seq);

        default:
            not_seq_error(genv, seq);
            return NULL;
        }

    } else {
        // user defined sequences??
        not_seq_error(genv, seq);
        return NULL;
    }
}

lis_obj * lisp_elt(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"elt"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _seq_elt(genv, arg1, arg2);
}

lis_obj * _seq_reverse(lis_obj * genv, lis_obj * seq) {
   if (seq == LIS_GENV(genv)->symbol_nil) {
        return _make_int(0);

    } else if (LIS_TAG_BASE(seq) == LIS_TAG_BASE_BUILTIN) {
        switch (LIS_TAG_TYPE(seq)) {
        case LIS_TAG_TYPE_ARY:
            return NULL;

        case LIS_TAG_TYPE_STR:
            return NULL;

        case LIS_TAG_TYPE_CONS:
            return _list_reverse(genv, seq);

        default:
            not_seq_error(genv, seq);
            return NULL;
        }

    } else {
        // user defined sequences??
        not_seq_error(genv, seq);
        return NULL;
    }
}

lis_obj * lisp_reverse(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"reverse"))) {
        return NULL;
    }

    return _seq_reverse(genv, _list_nth(genv, _make_int(0), args));
}
