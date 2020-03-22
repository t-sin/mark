#ifndef __lis_package
#define __lis_package

#include "obj.h"

lis_obj * add_symbol(lis_package * package, lis_obj * symbol);
lis_obj * find_symbol(lis_package * package, lis_obj * symbol);

lis_obj * intern(lis_package * package, lis_obj * name);

#endif
