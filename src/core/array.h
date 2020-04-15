#ifndef __lis_array
#define __lis_array

#include "obj.h"
#include "list.h"
#include "array.h"

bool _array_arrayp(lis_obj * genv, lis_obj * obj);
lis_obj * lisp_arrayp(lis_obj * genv, lis_obj * args);

lis_obj * _array_make_array(lis_obj * genv, lis_obj * dimlis);
lis_obj * lisp_make_array(lis_obj * genv, lis_obj * args);

lis_obj * _array_aref(lis_obj * genv, lis_obj * subscripts);
lis_obj * lisp_aref(lis_obj * genv, lis_obj * args);

#endif
