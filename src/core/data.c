#include "obj.h"
#include "list.h"

#include "data.h"

lis_obj * lisp_values(lis_obj * genv, lis_obj * args) {
    if (args == LIS_NIL) {
        return LIS_NIL;
    }

    lis_obj * rest = args;
    lis_obj * list = LIS_NIL;
    while (rest != LIS_GENV(genv)->symbol_nil) {
        list = _list_cons(genv, _list_car(genv, rest), list);
        rest = _list_cdr(genv, rest);
    }

    lis_obj * mv = _make_multiple_value();
    LIS_MVAL(mv) = _list_reverse(genv, list);
    return mv;
}
