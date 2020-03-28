#ifndef __lis_package
#define __lis_package

#include "obj.h"

typedef enum package_intern_status {
    PKG_INTERNAL, PKG_EXTERNAL, PKG_INHERITED
} package_intern_status;

lis_obj * add_symbol(lis_obj * package, lis_obj * symbol);

package_intern_status intern(lis_obj * package, lis_obj * name, lis_obj ** sym);


lis_package_table * package_table_init();
void package_table_add(lis_package * package);
void package_table_remove(lis_package * package);


#endif
