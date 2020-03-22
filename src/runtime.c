#include <assert.h>
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
    lis_obj * sym_nil = _make_symbol(nilname);
    sym_nil->data.sym->constant_p = true;
    sym_nil->data.sym->package = pkg_lis;
    sym_nil->data.sym->value = sym_nil;
    assert(add_symbol(pkg_lis, sym_nil) != NULL);
    runtime->symbol_nil = sym_nil;

    char t_cstr[] = u8"t";
    lis_obj * tname = to_lstring_from_cstr(t_cstr, sizeof(t_cstr));
    lis_obj * sym_t = _make_symbol(tname);
    sym_t->data.sym->constant_p = true;
    sym_t->data.sym->package = pkg_lis;
    sym_t->data.sym->value = sym_t;
    assert(add_symbol(pkg_lis, sym_t) != NULL);
    runtime->symbol_t = sym_t;

    return runtime;
}
