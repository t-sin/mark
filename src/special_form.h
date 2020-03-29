#ifndef __lis_special_form
#define __lis_special_form

#include "obj.h"

lis_obj * lis_sf_quote(lis_obj * genv, lis_obj * args);
lis_obj * lis_sf_function(lis_obj * genv, lis_obj * args);
lis_obj * lis_sf_setq(lis_obj * genv, lis_obj * args);

#endif
