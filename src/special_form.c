#include <uchar.h>

#include "obj.h"
#include "lstring.h"
#include "stream.h"
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
        stream_flush(stream);
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
        stream_flush(stream);
        return NULL;
    }

    lis_obj * sym = args->data.cons->car;                 // TODO: (first args)
    lis_obj * val = args->data.cons->cdr->data.cons->car;  // TODO: (second args)
    sym->data.sym->value = val;
    return val;
}
