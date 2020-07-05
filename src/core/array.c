#include "obj.h"
#include "lstring.h"
#include "list.h"
#include "eval.h"
#include "array.h"

bool _array_arrayp(lis_obj * genv, lis_obj * obj) {
    if (LIS_TAG_BASE(obj) == LIS_TAG_BASE_BUILTIN &&
        LIS_TAG_TYPE(obj) == LIS_TAG_TYPE_ARY) {
        return true;
    } else {
        return false;
    }
}

lis_obj * lisp_arrayp(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"arrayp"))) {
        return NULL;
    }

    if (_array_arrayp(genv, _list_nth(genv, INT(0), args))) {
        return LIS_T(genv);
    } else {
        return LIS_NIL(genv);
    }
}

lis_obj * _array_make_array(lis_obj * genv, lis_obj * dimlis) {
    return NULL;
}

lis_obj * lisp_make_array(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"make-array"))) {
        return NULL;
    }
    return NULL;
}

lis_obj * _array_aref(lis_obj * genv, lis_obj * subscripts) {
    return NULL;
}

lis_obj * lisp_aref(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"aref"))) {
        return NULL;
    }
    return NULL;
}
