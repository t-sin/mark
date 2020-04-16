#include <stdlib.h>

#include "../util/table.h"
#include "obj.h"

lis_obj * _make_int(lis_int n) {
    lis_obj * num = (lis_obj *)malloc(sizeof(lis_obj));
    num->tags = LIS_TAG_BASE_INT << 2;
    num->data.num = n;
    return num;
}

lis_obj * _make_char(lis_int ch) {
    lis_obj * c = (lis_obj *)malloc(sizeof(lis_obj));
    c->tags = LIS_TAG_BASE_CHAR << 2;
    c->data.ch = ch;
    return c;
}

lis_obj * _make_array() {
    lis_obj * arr = (lis_obj *)malloc(sizeof(lis_obj));
    arr->tags = LIS_TAG_BASE_BUILTIN << 2 | LIS_TAG_TYPE_ARY << 4;
    arr->data.array = malloc(sizeof(lis_array));
    arr->data.array->size = 0;
    arr->data.array->body = NULL;
    return arr;
}

lis_obj * _make_string() {
    lis_obj * str = (lis_obj *)malloc(sizeof(lis_obj));
    str->tags = LIS_TAG_BASE_BUILTIN << 2 | LIS_TAG_TYPE_STR << 4;
    str->data.str = malloc(sizeof(lis_string));
    str->data.str->size = 0;
    str->data.str->body = NULL;
    return str;
}

lis_obj * _make_symbol(lis_obj * name) {
    lis_obj * sym = (lis_obj *)malloc(sizeof(lis_obj));
    sym->tags = LIS_TAG_BASE_BUILTIN << 2 | LIS_TAG_TYPE_SYM << 4;
    sym->data.sym = malloc(sizeof(lis_symbol));
    sym->data.sym->name = name;
    sym->data.sym->constant_p = false;
    sym->data.sym->dynamic_p = false;
    return sym;
}

lis_obj * _make_cons() {
    lis_obj * cons = (lis_obj *)malloc(sizeof(lis_obj));
    cons->tags = LIS_TAG_BASE_BUILTIN << 2 | LIS_TAG_TYPE_CONS << 4;
    cons->data.cons = malloc(sizeof(lis_cons));
    cons->data.cons->car = NULL;
    cons->data.cons->cdr = NULL;
    return cons;
}

#define DEFAULT_PACKAGE_LEN 256
lis_obj * _make_package(lis_obj * name) {
    lis_obj * pkg = (lis_obj *)malloc(sizeof(lis_obj));
    pkg->tags = LIS_TAG_BASE_BUILTIN << 2 | LIS_TAG_TYPE_PKG << 4;
    pkg->data.pkg = (lis_package *)malloc(sizeof(lis_package));
    pkg->data.pkg->name = name;
    pkg->data.pkg->num = 0;
    pkg->data.pkg->size = DEFAULT_PACKAGE_LEN;
    pkg->data.pkg->symbols = (lis_obj **)malloc(sizeof(lis_obj) * DEFAULT_PACKAGE_LEN);
    return pkg;
}

lis_obj * _make_env(env_type type) {
    lis_obj * env = (lis_obj *)malloc(sizeof(lis_obj));
    env->tags = LIS_TAG_BASE_BUILTIN << 2 | LIS_TAG_TYPE_ENV << 4;
    env->data.env = (lis_env *)malloc(sizeof(lis_env));
    env->data.env->parent = NULL;
    env->data.env = (lis_env *)malloc(sizeof(lis_env));
    env->data.env->type = type;
    return env;
}

lis_obj * _make_lexical_env() {
    lis_obj * env = _make_env(LIS_ENV_LEXICAL);
    LIS_LENV(env) = (lis_lexical_env *)malloc(sizeof(lis_lexical_env));
    LIS_LENV(env)->var = _make_table(256);
    LIS_LENV(env)->fn = _make_table(256);
    return env;
}

lis_obj * _make_raw_function(lis_obj * (*raw_fn)(lis_obj *, lis_obj *)) {
    lis_obj * fn = (lis_obj *)malloc(sizeof(lis_obj));
    fn->tags = LIS_TAG_BASE_BUILTIN << 2 | LIS_TAG_TYPE_FN << 4;
    fn->data.fn = (lis_function *)malloc(sizeof(lis_function));
    fn->data.fn->type = LIS_FUNC_RAW;
    fn->data.fn->lambdalist = NULL;
    fn->data.fn->body.raw = raw_fn;
    return fn;
}

lis_obj * _make_lisp_function(lis_lambdalist * lambdalist, lis_obj * body) {
    lis_obj * fn = (lis_obj *)malloc(sizeof(lis_obj));
    fn->tags = LIS_TAG_BASE_BUILTIN << 2 | LIS_TAG_TYPE_FN << 4;
    fn->data.fn = (lis_function *)malloc(sizeof(lis_function));
    fn->data.fn->type = LIS_FUNC_NORMAL;
    fn->data.fn->lambdalist = lambdalist;
    fn->data.fn->body.lisp = body;
    return fn;
}

lis_obj * _make_lisp_closure(lis_lambdalist * lambdalist, lis_obj * body, lis_obj * lenv) {
    lis_obj * cls = (lis_obj *)malloc(sizeof(lis_obj));
    cls->tags = LIS_TAG_BASE_BUILTIN << 2 | LIS_TAG_TYPE_CLS << 4;
    cls->data.cls = (lis_closure *)malloc(sizeof(lis_closure));
    cls->data.cls->fn = _make_lisp_function(lambdalist, body);
    cls->data.cls->env = lenv;
    return cls;
}

lis_obj * _make_lis_stream(lis_stream * s) {
    lis_obj * stream = (lis_obj *)malloc(sizeof(lis_obj));
    stream->tags = LIS_TAG_BASE_BUILTIN << 2 | LIS_TAG_TYPE_STRM << 4;
    stream->data.stream = s;
    return stream;
}

lis_obj * _make_error(lis_obj * msg) {
    lis_obj * err = (lis_obj *)malloc(sizeof(lis_obj));
    err->tags = LIS_TAG_BASE_BUILTIN << 2 | LIS_TAG_TYPE_ERR << 4;
    err->data.err = (lis_error *)malloc(sizeof(lis_error));
    err->data.err->message = msg;
    return err;
}
