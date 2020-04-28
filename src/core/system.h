#ifndef __lis_system_construction
#define __lis_system_construction

#include "obj.h"

lis_obj * load(lis_obj * genv, lis_string * filename);

lis_obj * lisp_load(lis_obj * genv, lis_obj * args);

#endif
