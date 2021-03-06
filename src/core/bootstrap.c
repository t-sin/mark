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
#include "symbol.h"
#include "read.h"
#include "print.h"
#include "eval.h"
#include "data.h"
#include "stream.h"
#include "system.h"

#include "bootstrap.h"

#define define_special_form(opname, symname, cfnname) \
    lis_obj * symname = _make_symbol(LSTR(opname)); \
    LIS_SYM(symname)->package = genv->current_package; \
    LIS_SYM(symname)->fn = _make_raw_function(cfnname); \
    LIS_FN(LIS_SYM(symname)->fn)->type = LIS_FUNC_SPECIAL_FORM; \
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
    define_special_form(U"%fset", sym_fset, lis_sf_fset);
    define_special_form(U"block", sym_block, lis_sf_block);
    define_special_form(U"return-from", sym_return_from, lis_sf_return_from);
    define_special_form(U"unwind-protect", sym_unwind_protect, lis_sf_unwind_protect);
    // flet
}

#define define_macro(opname, symname, cfnname) \
    lis_obj * symname = _make_symbol(LSTR(opname)); \
    LIS_SYM(symname)->package = genv->current_package; \
    LIS_SYM(symname)->fn = _make_raw_function(cfnname); \
    LIS_FN(LIS_SYM(symname)->fn)->type = LIS_FUNC_MACRO_RAW; \
    assert(add_symbol(genv->current_package, symname) != NULL)

void init_macros(lis_global_env * genv) {
    define_macro(U"defun", sym_defun, lis_macro_defun);
}

#define define_builtin_function(opname, symname, cfnname) \
    lis_obj * symname = _make_symbol(LSTR(opname)); \
    LIS_SYM(symname)->package = genv->current_package; \
    LIS_SYM(symname)->fn = _make_raw_function(cfnname); \
    LIS_FN(LIS_SYM(symname)->fn)->type = LIS_FUNC_RAW; \
    assert(add_symbol(genv->current_package, symname) != NULL)


void init_functions(lis_global_env * genv) {
    // lists
    define_builtin_function(U"atom", sym_atom, lisp_atom);
    define_builtin_function(U"null", sym_null, lisp_null);
    define_builtin_function(U"consp", sym_consp, lisp_consp);
    define_builtin_function(U"listp", sym_listp, lisp_listp);
    define_builtin_function(U"nth", sym_nth, lisp_nth);
    define_builtin_function(U"cons", sym_cons, lisp_cons);
    define_builtin_function(U"list", sym_list, lisp_list);
    define_builtin_function(U"car", sym_car, lisp_car);
    define_builtin_function(U"cdr", sym_cdr, lisp_cdr);
    define_builtin_function(U"rplaca", sym_rplaca, lisp_rplaca);
    define_builtin_function(U"rplacd", sym_rplacd, lisp_rplacd);
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
    define_builtin_function(U"find-package", sym_find_package, lisp_find_package);
    define_builtin_function(U"find-symbol", sym_find_symbol, lisp_find_symbol);
    define_builtin_function(U"intern", sym_intern, lisp_intern);
    define_builtin_function(U"use-package", sym_use_package, lisp_use_package);
    define_builtin_function(U"package-use-list", sym_package_use_list, lisp_package_use_list);
    define_builtin_function(U"import", sym_import, lisp_import);

    // symbols
    define_builtin_function(U"symbolp", sym_symbolp, lisp_symbolp);
    define_builtin_function(U"symbol-name", sym_symbol_name, lisp_symbol_name);
    define_builtin_function(U"symbol-value", sym_symbol_value, lisp_symbol_value);
    define_builtin_function(U"symbol-function", sym_symbol_function, lisp_symbol_function);
    define_builtin_function(U"symbol-package", sym_symbol_package, lisp_symbol_package);

    // streams
    define_builtin_function(U"listen", sym_listen, lisp_listen);
    define_builtin_function(U"force-output", sym_force_output, lisp_force_output);
    define_builtin_function(U"open", sym_open, lisp_open);
    define_builtin_function(U"close", sym_close, lisp_close);
    define_builtin_function(U"peek-byte", sym_peek_byte, lisp_peek_byte);
    define_builtin_function(U"read-byte", sym_read_byte, lisp_read_byte);
    define_builtin_function(U"write-byte", sym_write_byte, lisp_write_byte);
    define_builtin_function(U"peek-char", sym_peek_char, lisp_peek_char);
    define_builtin_function(U"read-char", sym_read_char, lisp_read_char);
    define_builtin_function(U"unread-char", sym_unread_char, lisp_unread_char);
    define_builtin_function(U"write-char", sym_write_char, lisp_write_char);

    // eval
    define_builtin_function(U"apply", sym_apply, lisp_apply);
    define_builtin_function(U"eval", sym_eval, lisp_eval);
    define_builtin_function(U"macroexpand-1", sym_macroexpand_1, lisp_macroexpand_1);

    // read
    define_builtin_function(U"read", sym_read, lisp_read);
    // print
    define_builtin_function(U"print", sym_print, lisp_print);

    // data
    define_builtin_function(U"eq", sym_eq, lisp_eq);
    define_builtin_function(U"values", sym_values, lisp_values);

    // system construction
    define_builtin_function(U"load", sym_load, lisp_load);
}

#define define_standard_stream(sname, dir, type, symname, symstream, symsym) \
    lis_obj * symname = LSTR(sname); \
    lis_obj * symstream = _make_lis_stream(make_lis_stream(1024, dir, type)); \
    LIS_STREAM(symstream)->name = symname; \
    lis_obj * symsym = _make_symbol(symname); \
    LIS_SYM(symsym)->constant_p = true; \
    LIS_SYM(symsym)->dynamic_p = true; \
    LIS_SYM(symsym)->package = genv->current_package; \
    LIS_SYM(symsym)->value = symstream; \
    assert(add_symbol(genv->current_package, symsym) != NULL)

void init_streams(lis_global_env * genv) {
    define_standard_stream(U"*stdin*", LIS_STREAM_IN, LIS_STREAM_TEXT,
                           name_stdin, stream_stdin, sym_stdin);
    LIS_STREAM(stream_stdin)->fin = stdin;
    genv->stream_stdin = stream_stdin;

    define_standard_stream(U"*stdout*", LIS_STREAM_INOUT, LIS_STREAM_TEXT,
                           name_stdout, stream_stdout, sym_stdout);
    LIS_STREAM(stream_stdout)->fout = stdout;
    genv->stream_stdout = stream_stdout;

    define_standard_stream(U"*stderr*", LIS_STREAM_INOUT, LIS_STREAM_TEXT,
                           name_stderr, stream_stderr, sym_stderr);
    LIS_STREAM(stream_stderr)->fout = stderr;
    genv->stream_stderr = stream_stderr;
}

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

lis_obj * init_global_env() {
    lis_global_env * genv;
    genv = (lis_global_env *)malloc(sizeof(lis_global_env));
    genv->error = NULL;
    genv->jump_tag = NULL;
    genv->return_from_value = NULL;
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

    init_streams(genv);
    init_special_forms(genv);
    init_macros(genv);
    init_functions(genv);

    return genv_obj;
}
