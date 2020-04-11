#ifndef __lis_package
#define __lis_package

#include "obj.h"

lis_obj * _package_make_package(lis_obj * genv, lis_obj * name_str);

typedef enum package_intern_status {
    PKG_INTERNAL, PKG_EXTERNAL, PKG_INHERITED
} package_intern_status;

lis_obj * add_symbol(lis_obj * package, lis_obj * symbol);

package_intern_status intern(lis_obj * package, lis_obj * name, lis_obj ** sym);


#endif
