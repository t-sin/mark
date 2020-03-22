#ifndef __lis_package
#define __lis_package

#include "obj.h"

typedef enum package_intern_status {
    PKG_INTERNAL, PKG_EXTERNAL, PKG_INHERITED
} package_intern_status;

lis_obj * add_symbol(lis_obj * package, lis_obj * symbol);

package_intern_status intern(lis_obj * package, lis_obj * name, lis_obj ** sym);

#endif
