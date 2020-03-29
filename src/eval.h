#ifndef __lis_eval
#define __lis_eval

#include "obj.h"
#include "environment.h"

lis_obj * eval_quote(lis_obj * genv, lis_obj * args);

lis_obj * apply(lis_obj * fn, lis_obj * args);
lis_obj * eval(lis_obj * genv, lis_obj * obj);

#endif