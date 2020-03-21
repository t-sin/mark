#ifndef __lis_eval
#define __lis_eval

#include "obj.h"

lis_obj * apply(lis_obj * obj, lis_obj * args);
lis_obj * eval(lis_obj * obj);

#endif