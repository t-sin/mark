#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "lstring.h"
#include "package.h"
#include "runtime.h"


void intern_special_form_names(lis_runtime * runtime) {
    lis_obj ** special_forms = (lis_obj *)malloc(sizeof(lis_obj *) * NUMBER_OF_LIS_SPECIAL_FORM);
    runtime->special_forms = special_forms;

    char quote_cstr[] = u8"quote";
    lis_obj * quote_name = to_lstring_from_cstr(quote_cstr, sizeof(quote_cstr));
    lis_obj * sym_quote = _make_symbol(quote_name);
    sym_quote->data.sym->package = runtime->current_package;
    sym_quote->data.sym->value = sym_quote;
    assert(add_symbol(runtime->current_package, sym_quote) != NULL);
    runtime->special_forms[LIS_SPECIAL_FORM_QUOTE] = sym_quote;
}

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

    intern_special_form_names(runtime);

    return runtime;
}
