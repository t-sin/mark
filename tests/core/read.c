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
    LIS_SYM(sym_name)->constant_p = true; \
    LIS_SYM(sym_name)->package = (pkg); \
    assert(add_symbol((pkg), sym_name) != NULL);

void init_symbols(lis_global_env * genv) {
    define_symbol(sym_nil, genv->current_package, U"nil");
    LIS_SYM(sym_nil)->value = sym_nil;
    genv->symbol_nil = sym_nil;

    define_symbol(sym_t, genv->current_package, U"t");
    LIS_SYM(sym_t)->value = sym_t;
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
    LIS_GENV(genv_obj) = genv;

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

void test_whitespaces() {
    lis_obj * input = LSTR(U"   ");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result == NULL);
}

void test_number_one_digit() {
    lis_obj * input = LSTR(U"1");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_INT);
    assert(LIS_INT(result) == 1);
}

void test_number_two_digits() {
    lis_obj * input = LSTR(U"98");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_INT);
    assert(LIS_INT(result) == 98);
}

void test_empty_string() {
    lis_obj * input = LSTR(U"\"\"");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_STR);
    assert(LIS_STR(result)->size == 0);
}

void test_string() {
    lis_obj * input = LSTR(U"\"abcde123\"");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_STR);
    assert(LIS_STR(result)->size == 8);
    assert(LIS_STR(result)->body[0] == 'a');
    assert(LIS_STR(result)->body[1] == 'b');
    assert(LIS_STR(result)->body[7] == '3');
}

void test_symbol_ends_with_eof() {
    lis_obj * input = LSTR(U"symbol");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    lis_obj * expected_name = LSTR(U"symbol");

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_SYM);
    assert(_string_equal(LIS_STR(expected_name), LIS_STR(LIS_SYM(result)->name)));
}

void test_symbol_ends_with_space() {
    lis_obj * input = LSTR(U"symbol ");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    lis_obj * expected_name = LSTR(U"symbol");

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_SYM);
    assert(_string_equal(LIS_STR(expected_name), LIS_STR(LIS_SYM(result)->name)));
}

void test_symbol_ends_with_paren() {
    lis_obj * input = LSTR(U"symbol(");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    lis_obj * expected_name = LSTR(U"symbol");

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_SYM);
    assert(_string_equal(LIS_STR(expected_name), LIS_STR(LIS_SYM(result)->name)));
}

void test_symbol_ends_with_newline() {
    lis_obj * input = LSTR(U"symbol\n");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    lis_obj * expected_name = LSTR(U"symbol");

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_SYM);
    assert(_string_equal(LIS_STR(expected_name), LIS_STR(LIS_SYM(result)->name)));
}

void test_nil() {
    lis_obj * input = LSTR(U"nil");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(result == LIS_NIL(genv));
}

void test_empty_list() {
    lis_obj * input = LSTR(U"()");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(result == LIS_NIL(genv));
}

void test_list_without_nest_with_one_element() {
    lis_obj * input = LSTR(U"(1)");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_CONS);
    assert(LIS_CONS(result)->car != NULL);
    assert(LIS_TAG_BASE(LIS_CONS(result)->car) == LIS_TAG_BASE_INT);
    assert(LIS_INT(LIS_CONS(result)->car) == 1);
    assert(LIS_CONS(result)->cdr == LIS_NIL(genv));
}

void test_list_without_nest_with_one_element_2() {
    lis_obj * input = LSTR(U"(1 )");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_CONS);

    assert(LIS_CONS(result)->car != NULL);
    assert(LIS_TAG_BASE(LIS_CONS(result)->car) == LIS_TAG_BASE_INT);
    assert(LIS_INT(LIS_CONS(result)->car) == 1);

    assert(LIS_CONS(result)->cdr == LIS_NIL(genv));
}

void test_list_without_nest_with_two_element() {
    lis_obj * input = LSTR(U"(1 2)");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_CONS);

    assert(LIS_CONS(result)->car != NULL);
    assert(LIS_TAG_BASE(LIS_CONS(result)->car) == LIS_TAG_BASE_INT);
    assert(LIS_INT(LIS_CONS(result)->car) == 1);

    assert(LIS_CONS(result)->cdr != NULL);
    assert(LIS_TAG_BASE(LIS_CONS(result)->cdr) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(LIS_CONS(result)->cdr) == LIS_TAG_TYPE_CONS);

    lis_obj * second_cons = LIS_CONS(result)->cdr;

    assert(LIS_CONS(second_cons)->car != NULL);
    assert(LIS_TAG_BASE(LIS_CONS(second_cons)->car) == LIS_TAG_BASE_INT);
    assert(LIS_INT(LIS_CONS(second_cons)->car) == 2);

    assert(LIS_CONS(second_cons)->cdr == LIS_NIL(genv));
}

void test_list_with_one_nest() {
    lis_obj * input = LSTR(U"(1 (2))");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();
    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_CONS);

    assert(LIS_CONS(result)->car != NULL);
    assert(LIS_TAG_BASE(LIS_CONS(result)->car) == LIS_TAG_BASE_INT);
    assert(LIS_INT(LIS_CONS(result)->car) == 1);

    assert(LIS_CONS(result)->cdr != NULL);
    assert(LIS_TAG_BASE(LIS_CONS(result)->cdr) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(LIS_CONS(result)->cdr) == LIS_TAG_TYPE_CONS);

    lis_obj * second_cons = LIS_CONS(result)->cdr;

    assert(LIS_CONS(second_cons)->car != NULL);
    assert(LIS_TAG_BASE(LIS_CONS(second_cons)->car) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(LIS_CONS(second_cons)->car) == LIS_TAG_TYPE_CONS);

    lis_obj * third_cons = LIS_CONS(second_cons)->car;

    assert(LIS_CONS(third_cons)->car != NULL);
    assert(LIS_TAG_BASE(LIS_CONS(third_cons)->car) == LIS_TAG_BASE_INT);
    assert(LIS_INT(LIS_CONS(third_cons)->car) == 2);

    assert(LIS_CONS(third_cons)->cdr == LIS_NIL(genv));

    assert(LIS_CONS(second_cons)->cdr == LIS_NIL(genv));
}

void test_newline_separated_expressions() {
    lis_obj * input = LSTR(U"(1)\n(2)");
    lis_stream * input_stream = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    assert(stream_write_string(input_stream, input));
    lis_obj * genv = init_minimal_global_env();

    lis_obj * result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_CONS);

    assert(LIS_CONS(result)->car != NULL);
    assert(LIS_TAG_BASE(LIS_CONS(result)->car) == LIS_TAG_BASE_INT);
    assert(LIS_INT(LIS_CONS(result)->car) == 1);

    assert(LIS_CONS(result)->cdr == LIS_NIL(genv));

    result = read(genv, input_stream);

    assert(result != NULL);
    assert(LIS_TAG_BASE(result) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(result) == LIS_TAG_TYPE_CONS);

    assert(LIS_CONS(result)->car != NULL);
    assert(LIS_TAG_BASE(LIS_CONS(result)->car) == LIS_TAG_BASE_INT);
    assert(LIS_INT(LIS_CONS(result)->car) == 2);

    assert(LIS_CONS(result)->cdr == LIS_NIL(genv));
}


int main() {
    test_empty_input();
    test_whitespaces();

    test_number_one_digit();
    test_number_two_digits();

    test_empty_string();
    test_string();

    test_symbol_ends_with_eof();
    test_symbol_ends_with_space();
    test_symbol_ends_with_paren();

    test_nil();
    test_empty_list();

    test_list_without_nest_with_one_element();
    test_list_without_nest_with_one_element_2();
    test_list_without_nest_with_two_element();

    test_list_with_one_nest();

    test_newline_separated_expressions();

    return 0;
}
