#ifndef __lis_data_and_control_flow
#define __lis_data_and_control_flow

#include "obj.h"

lis_obj * lis_sf_quote(lis_obj * genv, lis_obj * lenv, lis_obj * args);
lis_obj * lis_sf_function(lis_obj * genv, lis_obj * lenv, lis_obj * args);
lis_obj * lis_sf_setq(lis_obj * genv, lis_obj * lenv, lis_obj * args);
lis_obj * lis_sf_progn(lis_obj * genv, lis_obj * lenv, lis_obj * args);
lis_obj * lis_sf_if(lis_obj * genv, lis_obj * lenv, lis_obj * args);
lis_obj * lis_sf_let(lis_obj * genv, lis_obj * lenv, lis_obj * args);
lis_obj * lis_sf_lambda(lis_obj * genv, lis_obj * lenv, lis_obj * args);
lis_obj * lis_sf_multiple_value_call(lis_obj * genv, lis_obj * lenv, lis_obj * args);

lis_obj * lisp_eq(lis_obj * genv, lis_obj * args);

lis_obj * lisp_values(lis_obj * genv, lis_obj * args);

lis_obj * lis_sf_fset(lis_obj * genv, lis_obj * lenv, lis_obj * args);
lis_obj * lis_macro_defun(lis_obj * genv, lis_obj * args);

lis_obj * lis_sf_block(lis_obj * genv, lis_obj * lenv, lis_obj * args);
lis_obj * lis_sf_return_from(lis_obj * genv, lis_obj * lenv, lis_obj * args);

lis_obj * lis_sf_unwind_protect(lis_obj * genv, lis_obj * lenv, lis_obj * args);

#endif
