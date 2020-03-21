#include <stdlib.h>

#include "obj.h"
#include "lstring.h"
#include "runtime.h"

lis_runtime * init_runtime() {
    lis_runtime * runtime;
    runtime = (lis_runtime *)malloc(sizeof(lis_runtime));
    memset(runtime, 0, sizeof(lis_runtime));

    // runtime->package_table = (lis_package_table *)malloc(sizeof(lis_package_table));
    // runtime->package_table = 

    char name_cstr[] = u8"lis";
    lis_string * pkgname = to_string_from_cstr(name_cstr, sizeof(name_cstr));
    lis_obj * pkg_lis = make_package(pkgname);
    runtime->current_package = pkg_lis;

    return runtime;
}
