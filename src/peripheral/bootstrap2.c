#include "../core/obj.h"
#include "../core/lstring.h"
#include "../core/system.h"

#include "bootstrap2.h"

void init_runtime(lis_obj * genv) {
    load(genv, LIS_STR(LSTR(U"./src/peripheral/test.lisp")));
}
