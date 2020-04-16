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

lis_obj * lisp_values(lis_obj * genv, lis_obj * args);

#endif
