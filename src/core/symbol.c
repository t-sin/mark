#include <stdbool.h>

#include "obj.h"
#include "lstring.h"
#include "list.h"
#include "eval.h"

#include "symbol.h"

bool _symbol_symbolp(lis_obj * genv, lis_obj * obj) {
    if (LIS_TAG_BASE(obj) == LIS_TAG_BASE_BUILTIN &&
        LIS_TAG_TYPE(obj) == LIS_TAG_TYPE_SYM) {
        return true;
    } else {
        return false;
    }
}

lis_obj * lisp_symbolp(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"symbolp"))) {
        return NULL;
    }

    if (_symbol_symbolp(genv, _list_nth(genv, INT(0), args))) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}
