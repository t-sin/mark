#include <uchar.h>

#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "print.h"
#include "special_form.h"

lis_obj * lis_sf_quote(lis_obj * obj, lis_obj * args) {
    return args;
}

lis_obj * lis_sf_function(lis_obj * genv, lis_obj * args) {
    if (LIS_TAG3(args) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(args) != LIS_TAG_TYPE_SYM) {
        lis_stream * stream = genv->data.env->env.global->stream_stderr->data.stream;
        print(stream, genv, args);
        stream_write_string(stream, LSTR(U" is not a symbol."));
        stream_flush(stream);
        return NULL;
    }
    if (args->data.sym->fn == NULL) {
        return genv->data.env->env.global->symbol_nil;
    } else {
        return args->data.sym->fn;
    }
}
