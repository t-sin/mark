#ifndef __lis_symbol
#define __lis_symbol

#include <stdbool.h>

#include "obj.h"

bool _symbol_symbolp(lis_obj * genv, lis_obj * obj);

lis_obj * lisp_symbolp(lis_obj * genv, lis_obj * args);

#endif
