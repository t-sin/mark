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

lis_obj * lisp_integerp(lis_obj * genv, lis_obj * args);
lis_obj * lisp_zerop(lis_obj * genv, lis_obj * args);
lis_obj * lisp_plusp(lis_obj * genv, lis_obj * args);
lis_obj * lisp_minusp(lis_obj * genv, lis_obj * args);
lis_obj * lisp_equal(lis_obj * genv, lis_obj * args);
lis_obj * lisp_add(lis_obj * genv, lis_obj * args);
lis_obj * lisp_sub(lis_obj * genv, lis_obj * args);
lis_obj * lisp_mul(lis_obj * genv, lis_obj * args);
lis_obj * lisp_div(lis_obj * genv, lis_obj * args);

#endif
