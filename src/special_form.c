#include <uchar.h>

#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "eval.h"
#include "print.h"
#include "list.h"
#include "special_form.h"

lis_obj * lis_sf_quote(lis_obj * obj, lis_obj * args) {
    return args->data.cons->car;
}

lis_obj * lis_sf_function(lis_obj * genv, lis_obj * args) {
    lis_obj * sym = args->data.cons->car;
    if (LIS_TAG3(sym) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(sym) != LIS_TAG_TYPE_SYM) {
        lis_stream * stream = genv->data.env->env.global->stream_stderr->data.stream;
        print(genv, sym, stream);
        stream_write_string(stream, LSTR(U" is not a symbol."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(stream));
        return NULL;
    }
    if (sym->data.sym->fn == NULL) {
        return genv->data.env->env.global->symbol_nil;
    } else {
        return sym->data.sym->fn;
    }
}

lis_obj * lis_sf_setq(lis_obj * genv, lis_obj * args) {
    lis_obj * len = _list_length(genv, args);
    if (len == NULL ||
        LIS_TAG3(len) != LIS_TAG3_INT ||
        len->data.num != 2) {
        lis_stream * stream = genv->data.env->env.global->stream_stderr->data.stream;
        stream_write_string(stream, LSTR(U"wrong number of args to setq"));
        print(genv, args, stream);
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(stream));
        return NULL;
    }

    lis_obj * sym = _list_nth(genv, _make_int(0), args);
    lis_obj * val = eval(genv, _list_nth(genv, _make_int(1), args));
    sym->data.sym->value = val;
    return val;
}

lis_obj * lis_sf_progn(lis_obj * genv, lis_obj * args) {
    lis_obj * ret = LIS_GENV(genv)->symbol_nil;

    lis_obj * rest = args;
    while (rest != LIS_GENV(genv)->symbol_nil) {
        ret = eval(genv, _list_car(genv, rest));
        rest = _list_cdr(genv, rest);
    }

    return ret;
}
