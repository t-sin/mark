#ifndef __lis_arithmatic
#define __lis_arithmatic

#include "obj.h"

lis_obj * _int_zerop(lis_obj * genv, lis_obj * num);
lis_obj * _int_plusp(lis_obj * genv, lis_obj * num);
lis_obj * _int_minusp(lis_obj * genv, lis_obj * num);
lis_obj * _int_equal(lis_obj * genv, lis_obj * a, lis_obj * b);
lis_obj * _int_add(lis_obj * genv, lis_obj * a, lis_obj * b);
lis_obj * _int_sub(lis_obj * genv, lis_obj * a, lis_obj * b);
lis_obj * _int_mul(lis_obj * genv, lis_obj * a, lis_obj * b);
lis_obj * _int_div(lis_obj * genv, lis_obj * a, lis_obj * b);

lis_obj * int_integerp(lis_obj * genv, lis_obj * args);
lis_obj * int_zerop(lis_obj * genv, lis_obj * args);
lis_obj * int_plusp(lis_obj * genv, lis_obj * args);
lis_obj * int_minusp(lis_obj * genv, lis_obj * args);
lis_obj * int_equal(lis_obj * genv, lis_obj * args);
lis_obj * int_add(lis_obj * genv, lis_obj * args);
lis_obj * int_sub(lis_obj * genv, lis_obj * args);
lis_obj * int_mul(lis_obj * genv, lis_obj * args);
lis_obj * int_div(lis_obj * genv, lis_obj * args);

#endif
