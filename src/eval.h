#ifndef __lis_eval
#define __lis_eval

#include "obj.h"
#include "runtime.h"

lis_obj * apply(lis_obj * obj, lis_obj * args);
lis_obj * eval(lis_runtime * runtime, lis_obj * obj);

#endif