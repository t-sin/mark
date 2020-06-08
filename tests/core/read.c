#include <assert.h>
#include <string.h>

#include "../../src/util/table.h"

#include "../../src/core/obj.h"
#include "../../src/core/lstring.h"
#include "../../src/core/basic_stream.h"
#include "../../src/core/package.h"
#include "../../src/core/read.h"
#include "../../src/core/print.h"

#define define_symbol(sym_name, pkg, name) \
    lis_obj * sym_name = _make_symbol(LSTR(name)); \
    sym_name->data.sym->constant_p = true; \
    sym_name->data.sym->package = (pkg); \
    assert(add_symbol((pkg), sym_name) != NULL);

void init_symbols(lis_global_env * genv) {
    define_symbol(sym_nil, genv->current_package, U"nil");
    sym_nil->data.sym->value = sym_nil;
    genv->symbol_nil = sym_nil;

    define_symbol(sym_t, genv->current_package, U"t");
    sym_t->data.sym->value = sym_t;
    genv->symbol_t = sym_t;

    define_symbol(sym_optional, genv->current_package, U"&optional");
    genv->symbol_optional = sym_optional;

    define_symbol(sym_key, genv->current_package, U"&key");
    genv->symbol_key = sym_key;

    define_symbol(sym_rest, genv->current_package, U"&rest");
    genv->symbol_rest = sym_rest;
}

lis_obj * init_minimal_global_env() {
    lis_global_env * genv;
    genv = (lis_global_env *)malloc(sizeof(lis_global_env));
    memset(genv, 0, sizeof(lis_global_env));
    genv->package_table = _make_table(256);
    genv->package_table->hash_fn = _hash_string;
    genv->package_table->eq_fn = _table_string_eq;

    lis_obj * genv_obj = _make_env(LIS_ENV_GLOBAL);
    genv_obj->data.env->env.global = genv;

    lis_obj * pkg_lis = _package_make_package(genv_obj, LSTR(U"lis"));
    genv->current_package = pkg_lis;
    genv->lis_package = pkg_lis;

    init_symbols(genv);
    LIS_PKG(pkg_lis)->uselist = genv->symbol_nil;

    lis_obj * pkg_keyword = _package_make_package(genv_obj, LSTR(U"keyword"));
    genv->keyword_package = pkg_keyword;

    return genv_obj;
}

void test_empty_input() {
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result == NULL);
}

void test_nil() {
    lis_obj * input = LSTR(U"nil");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
}

int main() {
    test_empty_input();
    test_nil();
    return 0;
}
