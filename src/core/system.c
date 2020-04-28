#include <stdbool.h>

#include "obj.h"
#include "lstring.h"
#include "lerror.h"
#include "basic_stream.h"
#include "list.h"
#include "read.h"
#include "eval.h"

bool load(lis_obj * genv, lis_string * filename) {
    lis_stream * input = stream_open(filename, LIS_STREAM_IN, LIS_STREAM_TEXT);
    lis_obj * obj;

    if (input == NULL) {
        return false;
    }

    while ((obj = read(genv, input)) != NULL) {
        eval(genv, NULL, obj);
    }

    return true;
}

lis_obj * lisp_load(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"load"))) {
        return NULL;
    }

    lis_obj * filename = _list_nth(genv, INT(0), args);

    if (LIS_TAG_BASE(filename) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(filename) != LIS_TAG_TYPE_STR) {
        not_string_error(genv, filename);
        return NULL;
    }

    if (!load(genv, LIS_STR(filename))) {
        return LIS_NIL;
    } else {
        return filename;
    }
}
