#ifndef __lis_eval
#define __lis_eval

#include "obj.h"

bool check_arglen(lis_obj * genv, lis_obj * args, int len, lis_obj * opname);
bool check_argeven(lis_obj * genv, lis_obj * args, lis_obj * opname);

bool validate_lambdalist(lis_obj * genv, lis_obj * lambdalist);
lis_obj * bind_lambdalist(lis_obj * fn, lis_obj * args);

lis_obj * apply(lis_obj * genv, lis_obj * fn, lis_obj * args);
lis_obj * eval(lis_obj * genv, lis_obj * lenv, lis_obj * obj);

lis_obj * eval_apply(lis_obj * genv, lis_obj * args);
lis_obj * eval_eval(lis_obj * genv, lis_obj * args);

#endif