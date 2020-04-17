#include <stdbool.h>

#include "obj.h"
#include "lstring.h"
#include "lerror.h"
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

lis_obj * _symbol_symbol_name(lis_obj * genv, lis_obj * sym) {
    if (!_symbol_symbolp(genv, sym)) {
        not_symbol_error(genv, sym);
        return NULL;
    }

    return LIS_SYM(sym)->name;
}

lis_obj * _symbol_symbol_value(lis_obj * genv, lis_obj * sym) {
    if (!_symbol_symbolp(genv, sym)) {
        not_symbol_error(genv, sym);
        return NULL;
    }

    return LIS_SYM(sym)->value;
}

lis_obj * _symbol_symbol_function(lis_obj * genv, lis_obj * sym) {
    if (!_symbol_symbolp(genv, sym)) {
        not_symbol_error(genv, sym);
        return NULL;
    }

    return LIS_SYM(sym)->fn;
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

lis_obj * lisp_symbol_name(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"symbol-name"))) {
        return NULL;
    }

    return _symbol_symbol_name(genv, _list_nth(genv, INT(0), args));
}

lis_obj * lisp_symbol_value(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"symbol-value"))) {
        return NULL;
    }

    return _symbol_symbol_value(genv, _list_nth(genv, INT(0), args));
}

lis_obj * lisp_symbol_function(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"symbol-function"))) {
        return NULL;
    }

    return _symbol_symbol_function(genv, _list_nth(genv, INT(0), args));
}
