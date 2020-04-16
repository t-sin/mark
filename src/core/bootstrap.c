#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#include "../util/table.h"

#include "obj.h"
#include "lstring.h"
#include "basic_stream.h"
#include "list.h"
#include "array.h"
#include "arithmetic.h"
#include "sequence.h"
#include "package.h"
#include "read.h"
#include "print.h"
#include "eval.h"
#include "data.h"
#include "stream.h"

#include "bootstrap.h"

#define define_special_form(opname, symname, cfnname) \
    lis_obj * symname = _make_symbol(LSTR(opname)); \
    symname->data.sym->package = genv->current_package; \
    symname->data.sym->fn = _make_raw_function(cfnname); \
    symname->data.sym->fn->data.fn->type = LIS_FUNC_SPECIAL_FORM; \
    assert(add_symbol(genv->current_package, symname) != NULL)


void init_special_forms(lis_global_env * genv) {
    define_special_form(U"quote", sym_quote, lis_sf_quote);
    define_special_form(U"function", sym_function, lis_sf_function);
    define_special_form(U"setq", sym_setq, lis_sf_setq);
    define_special_form(U"progn", sym_progn, lis_sf_progn);
    define_special_form(U"if", sym_if, lis_sf_if);
    define_special_form(U"let", sym_let, lis_sf_let);
    define_special_form(U"lambda", sym_lambda, lis_sf_lambda);
    define_special_form(U"multiple-value-call", sym_multiple_value_call,
                        lis_sf_multiple_value_call);

    // flet
}

#define define_builtin_function(opname, symname, cfnname) \
    lis_obj * symname = _make_symbol(LSTR(opname)); \
    symname->data.sym->package = genv->current_package; \
    symname->data.sym->fn = _make_raw_function(cfnname); \
    symname->data.sym->fn->data.fn->type = LIS_FUNC_RAW; \
    assert(add_symbol(genv->current_package, symname) != NULL)


void init_functions(lis_global_env * genv) {
    // lists
    define_builtin_function(U"atom", sym_atom, lisp_atom);
    define_builtin_function(U"consp", sym_consp, lisp_consp);
    define_builtin_function(U"listp", sym_listp, lisp_listp);
    define_builtin_function(U"nth", sym_nth, lisp_nth);
    define_builtin_function(U"cons", sym_cons, lisp_cons);
    define_builtin_function(U"list", sym_list, lisp_list);
    define_builtin_function(U"car", sym_car, lisp_car);
    define_builtin_function(U"cdr", sym_cdr, lisp_cdr);
    define_builtin_function(U"first", sym_first, lisp_first);
    define_builtin_function(U"second", sym_second, lisp_second);
    define_builtin_function(U"third", sym_third, lisp_third);
    define_builtin_function(U"fourth", sym_fourth, lisp_fourth);
    define_builtin_function(U"fifth", sym_fifth, lisp_fifth);

    // integers
    define_builtin_function(U"integerp", sym_integerp, lisp_integerp);
    define_builtin_function(U"zerop", sym_zerop, lisp_zerop);
    define_builtin_function(U"plusp", sym_plusp, lisp_plusp);
    define_builtin_function(U"minusp", sym_minusp, lisp_minusp);
    define_builtin_function(U"=", sym_equal, lisp_equal);
    define_builtin_function(U"+", sym_add, lisp_add);
    define_builtin_function(U"-", sym_sub, lisp_sub);
    define_builtin_function(U"*", sym_mul, lisp_mul);
    define_builtin_function(U"/", sym_div, lisp_div);

    // strings
    define_builtin_function(U"string=", sym_string_equal, lisp_string_equal);
    define_builtin_function(U"stringp", sym_stringp, lisp_stringp);

    // arrays
    define_builtin_function(U"arrayp", sym_arrayp, lisp_arrayp);
    // define_builtin_function(U"make_array", sym_make_array, lisp_make_array);
    // define_builtin_function(U"aref", sym_aref, lisp_aref);

    // sequences
    define_builtin_function(U"copy-seq", sym_copy_seq, lisp_copy_seq);
    define_builtin_function(U"length", sym_length, lisp_length);
    define_builtin_function(U"elt", sym_elt, lisp_elt);
    define_builtin_function(U"reverse", sym_reverse, lisp_reverse);

    // packages
    define_builtin_function(U"list-all-packages", sym_list_all_packages, lisp_list_all_packages);
    define_builtin_function(U"make-package", sym_make_package, lisp_make_package);
    define_builtin_function(U"in-package", sym_in_package, lisp_in_package);
    define_builtin_function(U"intern", sym_intern, lisp_intern);

    // streams
    define_builtin_function(U"listen", sym_listen, lisp_listen);
    define_builtin_function(U"force-output", sym_force_output, lisp_force_output);

    // eval
    define_builtin_function(U"apply", sym_apply, lisp_apply);
    define_builtin_function(U"eval", sym_eval, lisp_eval);
    // read
    define_builtin_function(U"read", sym_read, lisp_read);
    // print
    define_builtin_function(U"print", sym_print, lisp_print);

    // data
    define_builtin_function(U"values", sym_values, lisp_values);
}

#define define_standard_stream(sname, dir, type, symname, symstream, symsym) \
    lis_obj * symname = LSTR(sname); \
    lis_obj * symstream = _make_lis_stream(make_lis_stream(1024, dir, type)); \
    symstream->data.stream->name = symname; \
    lis_obj * symsym = _make_symbol(symname); \
    symsym->data.sym->constant_p = true; \
    symsym->data.sym->dynamic_p = true; \
    symsym->data.sym->package = genv->current_package; \
    symsym->data.sym->value = symstream; \
    assert(add_symbol(genv->current_package, symsym) != NULL)

void init_streams(lis_global_env * genv) {
    define_standard_stream(U"*stdin*", LIS_STREAM_IN, LIS_STREAM_TEXT,
                           name_stdin, stream_stdin, sym_stdin);
    stream_stdin->data.stream->fin = stdin;
    genv->stream_stdin = stream_stdin;

    define_standard_stream(U"*stdout*", LIS_STREAM_INOUT, LIS_STREAM_TEXT,
                           name_stdout, stream_stdout, sym_stdout);
    stream_stdout->data.stream->fout = stdout;
    genv->stream_stdout = stream_stdout;

    define_standard_stream(U"*stderr*", LIS_STREAM_INOUT, LIS_STREAM_TEXT,
                           name_stderr, stream_stderr, sym_stderr);
    stream_stderr->data.stream->fout = stderr;
    genv->stream_stderr = stream_stderr;
}

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

lis_obj * init_global_env() {
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

    lis_obj * pkg_keyword = _package_make_package(genv_obj, LSTR(U"keyword"));
    genv->keyword_package = pkg_keyword;

    init_symbols(genv);
    init_streams(genv);
    init_special_forms(genv);
    init_functions(genv);

    return genv_obj;
}
