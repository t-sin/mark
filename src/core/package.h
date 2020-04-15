#ifndef __lis_package
#define __lis_package

#include "obj.h"

bool _table_string_eq(void * a, void * b);
size_t _hash_string(void * str_obj, size_t hash_size);

lis_obj * _package_make_package(lis_obj * genv, lis_obj * name_str);
lis_obj * _package_list_all_packages(lis_obj * genv);
lis_obj * _package_in_package(lis_obj * genv, lis_obj * name);

lis_obj * lisp_make_package(lis_obj * genv, lis_obj * args);
lis_obj * lisp_list_all_packages(lis_obj * genv, lis_obj * args);
lis_obj * lisp_in_package(lis_obj * genv, lis_obj * args);

typedef enum package_intern_status {
    PKG_INTERNAL, PKG_EXTERNAL, PKG_INHERITED
} package_intern_status;

lis_obj * add_symbol(lis_obj * package, lis_obj * symbol);

package_intern_status intern(lis_obj * package, lis_obj * name, lis_obj ** sym);
lis_obj * lisp_intern(lis_obj * genv, lis_obj * args);

#endif
