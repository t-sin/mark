#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "lstring.h"
#include "environment.h"
#include "package.h"
#include "eval.h"


void init_special_forms(lis_global_env * genv) {
    lis_obj ** special_forms = (lis_obj **)malloc(sizeof(lis_obj *) * NUMBER_OF_LIS_SPECIAL_FORM);
    genv->special_forms = special_forms;

    char quote_cstr[] = u8"quote";
    lis_obj * quote_name = to_lstring_from_cstr(quote_cstr, sizeof(quote_cstr));
    lis_obj * sym_quote = _make_symbol(quote_name);
    sym_quote->data.sym->package = genv->current_package;
    sym_quote->data.sym->fn = _make_raw_function(eval_quote);
    sym_quote->data.sym->fn->data.fn->type = LIS_FUNC_SPECIAL_FORM;
    sym_quote->data.sym->fn->data.fn->raw_body = eval_quote;
    assert(add_symbol(genv->current_package, sym_quote) != NULL);
    genv->special_forms[LIS_SPECIAL_FORM_QUOTE] = sym_quote;
}

lis_obj * init_global_env() {
    lis_global_env * genv;
    genv = (lis_global_env *)malloc(sizeof(lis_global_env));
    memset(genv, 0, sizeof(lis_global_env));

    char name_cstr[] = u8"lis";
    lis_obj * pkgname = to_lstring_from_cstr(name_cstr, sizeof(name_cstr));
    lis_obj * pkg_lis = _make_package(pkgname);
    genv->current_package = pkg_lis;

    char nil_cstr[] = u8"nil";
    lis_obj * nilname = to_lstring_from_cstr(nil_cstr, sizeof(nil_cstr));
    lis_obj * sym_nil = _make_symbol(nilname);
    sym_nil->data.sym->constant_p = true;
    sym_nil->data.sym->package = pkg_lis;
    sym_nil->data.sym->value = sym_nil;
    assert(add_symbol(pkg_lis, sym_nil) != NULL);
    genv->symbol_nil = sym_nil;

    char t_cstr[] = u8"t";
    lis_obj * tname = to_lstring_from_cstr(t_cstr, sizeof(t_cstr));
    lis_obj * sym_t = _make_symbol(tname);
    sym_t->data.sym->constant_p = true;
    sym_t->data.sym->package = pkg_lis;
    sym_t->data.sym->value = sym_t;
    assert(add_symbol(pkg_lis, sym_t) != NULL);
    genv->symbol_t = sym_t;

    init_special_forms(genv);

    lis_obj * env = _make_env(LIS_ENV_GLOBAL);
    env->data.env->env.global = genv;
    return env;
}
