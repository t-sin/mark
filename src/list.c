#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "print.h"
#include "arithmetic.h"

lis_obj * _list_length(lis_obj * genv, lis_obj * list) {
    if (list == LIS_GENV(genv)->symbol_nil) {
        return _make_int(0);
    } else if (LIS_TAG3(list) != LIS_TAG3_BUILTIN ||
               LIS_TAG_TYPE(list) != LIS_TAG_TYPE_CONS) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_OUT, LIS_STREAM_TEXT);
        print(genv, list, buffer);
        stream_write_string(buffer, LSTR(U" is not a list."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    } else {
        return int_add(genv, _list_length(genv, LIS_CONS(list)->cdr), _make_int(1));
    }
}

lis_obj * list_length(lis_obj * genv, lis_obj * args) {
    if (_list_length(genv, args)->data.num == 1) {
        return _list_length(genv, LIS_CONS(args)->car);
    } else {
        LIS_GENV(genv)->error = _make_error(LSTR(U"wrong number of args for `length`."));
        return NULL;
    }
}

lis_obj * first(lis_obj * genv, lis_obj * list) {}
lis_obj * second(lis_obj * genv, lis_obj * list) {}
lis_obj * third(lis_obj * genv, lis_obj * list) {}
lis_obj * fourth(lis_obj * genv, lis_obj * list) {}
lis_obj * fifth(lis_obj * genv, lis_obj * list) {}
