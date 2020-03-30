#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "print.h"
#include "arithmetic.h"

lis_obj * list_length(lis_obj * genv, lis_obj * list) {
    if (list == genv->data.env->env.global->symbol_nil) {
        return _make_int(0);
    } else if (LIS_TAG3(list) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(list) != LIS_TAG_TYPE_CONS) {
        lis_stream * stream = genv->data.env->env.global->stream_stderr->data.stream;
        stream_write_string(stream, LSTR(U" is not a list."));
        stream_flush(stream);
        print(genv, list, buffer);
        return NULL;
    } else {
        return int_add(genv, list_length(genv, list->data.cons->cdr), _make_int(1));
    }
}
