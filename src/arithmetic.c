#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "print.h"

lis_obj * int_add(lis_obj * genv, lis_obj * a, lis_obj * b) {
    if (LIS_TAG3(a) != LIS_TAG3_INT) {
        lis_stream * stream = genv->data.env->env.global->stream_stderr->data.stream;
        print(stream, genv, b);
        stream_write_string(stream, LSTR(U" is not an int."));
        stream_flush(stream);
        return NULL;
    }

    if (LIS_TAG3(b) != LIS_TAG3_INT) {
        lis_stream * stream = genv->data.env->env.global->stream_stderr->data.stream;
        print(stream, genv, b);
        stream_write_string(stream, LSTR(U" is not an int."));
        stream_flush(stream);
        return NULL;
    }

    return _make_int(a->data.num + b->data.num);
}
