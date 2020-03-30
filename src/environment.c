#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "environment.h"
#include "package.h"
#include "eval.h"
#include "special_form.h"
#include "list.h"

void init_special_forms(lis_global_env * genv) {
    lis_obj * sym_quote = _make_symbol(LSTR(U"quote"));
    sym_quote->data.sym->package = genv->current_package;
    sym_quote->data.sym->fn = _make_raw_function(lis_sf_quote);
    sym_quote->data.sym->fn->data.fn->type = LIS_FUNC_SPECIAL_FORM;
    assert(add_symbol(genv->current_package, sym_quote) != NULL);

    lis_obj * sym_function = _make_symbol(LSTR(U"function"));
    sym_function->data.sym->package = genv->current_package;
    sym_function->data.sym->fn = _make_raw_function(lis_sf_function);
    sym_function->data.sym->fn->data.fn->type = LIS_FUNC_SPECIAL_FORM;
    assert(add_symbol(genv->current_package, sym_function) != NULL);

    lis_obj * sym_setq = _make_symbol(LSTR(U"setq"));
    sym_setq->data.sym->package = genv->current_package;
    sym_setq->data.sym->fn = _make_raw_function(lis_sf_setq);
    sym_setq->data.sym->fn->data.fn->type = LIS_FUNC_SPECIAL_FORM;
    assert(add_symbol(genv->current_package, sym_setq) != NULL);

    // let
    // flet
    // if
    // progn
}

void init_functions(lis_global_env * genv) {
    // lists
    lis_obj * sym_list_length = _make_symbol(LSTR(U"length"));
    sym_list_length->data.sym->package = genv->current_package;
    sym_list_length->data.sym->fn = _make_raw_function(list_length);
    sym_list_length->data.sym->fn->data.fn->type = LIS_FUNC_NORMAL;
    assert(add_symbol(genv->current_package, sym_list_length) != NULL);

    lis_obj * sym_list_nth = _make_symbol(LSTR(U"nth"));
    sym_list_nth->data.sym->package = genv->current_package;
    sym_list_nth->data.sym->fn = _make_raw_function(list_nth);
    sym_list_nth->data.sym->fn->data.fn->type = LIS_FUNC_NORMAL;
    assert(add_symbol(genv->current_package, sym_list_nth) != NULL);

    lis_obj * sym_list_first = _make_symbol(LSTR(U"first"));
    sym_list_first->data.sym->package = genv->current_package;
    sym_list_first->data.sym->fn = _make_raw_function(list_first);
    sym_list_first->data.sym->fn->data.fn->type = LIS_FUNC_NORMAL;
    assert(add_symbol(genv->current_package, sym_list_first) != NULL);

    lis_obj * sym_list_second = _make_symbol(LSTR(U"second"));
    sym_list_second->data.sym->package = genv->current_package;
    sym_list_second->data.sym->fn = _make_raw_function(list_second);
    sym_list_second->data.sym->fn->data.fn->type = LIS_FUNC_NORMAL;
    assert(add_symbol(genv->current_package, sym_list_second) != NULL);

    lis_obj * sym_list_third = _make_symbol(LSTR(U"third"));
    sym_list_third->data.sym->package = genv->current_package;
    sym_list_third->data.sym->fn = _make_raw_function(list_third);
    sym_list_third->data.sym->fn->data.fn->type = LIS_FUNC_NORMAL;
    assert(add_symbol(genv->current_package, sym_list_third) != NULL);

    lis_obj * sym_list_fourth = _make_symbol(LSTR(U"fourth"));
    sym_list_fourth->data.sym->package = genv->current_package;
    sym_list_fourth->data.sym->fn = _make_raw_function(list_fourth);
    sym_list_fourth->data.sym->fn->data.fn->type = LIS_FUNC_NORMAL;
    assert(add_symbol(genv->current_package, sym_list_fourth) != NULL);

    lis_obj * sym_list_fifth = _make_symbol(LSTR(U"fifth"));
    sym_list_fifth->data.sym->package = genv->current_package;
    sym_list_fifth->data.sym->fn = _make_raw_function(list_fifth);
    sym_list_fifth->data.sym->fn->data.fn->type = LIS_FUNC_NORMAL;
    assert(add_symbol(genv->current_package, sym_list_fifth) != NULL);
}

void init_streams(lis_global_env * genv) {
    lis_obj * stdin_name = LSTR(U"*stdin*");
    lis_stream * _stream_stdin = make_lis_stream(1024, LIS_STREAM_IN, LIS_STREAM_TEXT);
    _stream_stdin->name = stdin_name;
    _stream_stdin->fin = stdin;
    lis_obj * stream_stdin = _make_lis_stream(_stream_stdin);
    lis_obj * sym_stdin = _make_symbol(stdin_name);
    sym_stdin->data.sym->constant_p = true;
    sym_stdin->data.sym->dynamic_p = true;
    sym_stdin->data.sym->package = genv->current_package;
    sym_stdin->data.sym->value = stream_stdin;
    assert(add_symbol(genv->current_package, sym_stdin) != NULL);
    genv->stream_stdin = stream_stdin;

    lis_obj * stdout_name = LSTR(U"*stdout*");
    lis_stream * _stream_stdout = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    _stream_stdout->name = stdout_name;
    _stream_stdout->fout = stdout;
    lis_obj * stream_stdout = _make_lis_stream(_stream_stdout);
    lis_obj * sym_stdout = _make_symbol(stdout_name);
    sym_stdout->data.sym->constant_p = true;
    sym_stdout->data.sym->dynamic_p = true;
    sym_stdout->data.sym->package = genv->current_package;
    sym_stdout->data.sym->value = stream_stdout;
    assert(add_symbol(genv->current_package, sym_stdout) != NULL);
    genv->stream_stdout = stream_stdout;

    lis_obj * stderr_name = LSTR(U"*stderr*");
    lis_stream * _stream_stderr = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    _stream_stderr->name = stderr_name;
    _stream_stderr->fout = stderr;
    lis_obj * stream_stderr = _make_lis_stream(_stream_stderr);
    lis_obj * sym_stderr = _make_symbol(stderr_name);
    sym_stderr->data.sym->constant_p = true;
    sym_stderr->data.sym->dynamic_p = true;
    sym_stderr->data.sym->package = genv->current_package;
    sym_stderr->data.sym->value = stream_stderr;
    assert(add_symbol(genv->current_package, sym_stderr) != NULL);
    genv->stream_stderr = stream_stderr;
}

lis_obj * init_global_env() {
    lis_global_env * genv;
    genv = (lis_global_env *)malloc(sizeof(lis_global_env));
    memset(genv, 0, sizeof(lis_global_env));

    lis_obj * pkgname = LSTR(U"lis");
    lis_obj * pkg_lis = _make_package(pkgname);
    genv->current_package = pkg_lis;

    lis_obj * kwd_pkgname = LSTR(U"keyword");
    lis_obj * pkg_keyword = _make_package(kwd_pkgname);
    genv->keyword_package = pkg_keyword;

    lis_obj * nilname = LSTR(U"nil");
    lis_obj * sym_nil = _make_symbol(nilname);
    sym_nil->data.sym->constant_p = true;
    sym_nil->data.sym->package = pkg_lis;
    sym_nil->data.sym->value = sym_nil;
    assert(add_symbol(pkg_lis, sym_nil) != NULL);
    genv->symbol_nil = sym_nil;

    lis_obj * tname = LSTR(U"t");
    lis_obj * sym_t = _make_symbol(tname);
    sym_t->data.sym->constant_p = true;
    sym_t->data.sym->package = pkg_lis;
    sym_t->data.sym->value = sym_t;
    assert(add_symbol(pkg_lis, sym_t) != NULL);
    genv->symbol_t = sym_t;

    init_streams(genv);
    init_special_forms(genv);
    init_functions(genv);

    lis_obj * env = _make_env(LIS_ENV_GLOBAL);
    env->data.env->env.global = genv;
    return env;
}
