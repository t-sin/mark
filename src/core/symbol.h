#ifndef __lis_symbol
#define __lis_symbol

#include <stdbool.h>

#include "obj.h"

bool _symbol_symbolp(lis_obj * genv, lis_obj * obj);
lis_obj * _symbol_symbol_name(lis_obj * genv, lis_obj * sym);
lis_obj * _symbol_symbol_value(lis_obj * genv, lis_obj * sym);
lis_obj * _symbol_symbol_function(lis_obj * genv, lis_obj * sym);

lis_obj * lisp_symbolp(lis_obj * genv, lis_obj * args);
lis_obj * lisp_symbol_name(lis_obj * genv, lis_obj * args);
lis_obj * lisp_symbol_value(lis_obj * genv, lis_obj * args);
lis_obj * lisp_symbol_function(lis_obj * genv, lis_obj * args);

#endif
