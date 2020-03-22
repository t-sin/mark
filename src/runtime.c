#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "lstring.h"
#include "package.h"
#include "runtime.h"

lis_runtime * init_runtime() {
    lis_runtime * runtime;
    runtime = (lis_runtime *)malloc(sizeof(lis_runtime));
    memset(runtime, 0, sizeof(lis_runtime));

    // runtime->package_table = (lis_package_table *)malloc(sizeof(lis_package_table));
    // runtime->package_table = 

    char name_cstr[] = u8"lis";
    lis_obj * pkgname = to_lstring_from_cstr(name_cstr, sizeof(name_cstr));
    lis_obj * pkg_lis = _make_package(pkgname);
    runtime->current_package = pkg_lis;

    char nil_cstr[] = u8"nil";
    lis_obj * nilname = to_lstring_from_cstr(nil_cstr, sizeof(nil_cstr));
    lis_obj * nil_sym = _make_symbol(nilname);
    nil_sym->data.sym->constant_p = true;
    nil_sym->data.sym->package = pkg_lis;
    add_symbol(pkg_lis, nil_sym);

    return runtime;
}
