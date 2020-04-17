#include "obj.h"
#include "lstring.h"
#include "basic_stream.h"
#include "print.h"

#include "lerror.h"

void not_string_error(lis_obj * genv, lis_obj * obj) {
    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    print(genv, obj, buffer);
    stream_write_string(buffer, LSTR(U" is not an string."));
    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
}

void not_int_error(lis_obj * genv, lis_obj * obj) {
    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    print(genv, obj, buffer);
    stream_write_string(buffer, LSTR(U" is not an integer."));
    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
}

void not_list_error(lis_obj * genv, lis_obj * obj) {
    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    print(genv, obj, buffer);
    stream_write_string(buffer, LSTR(U" is not a list."));
    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
}

void not_seq_error(lis_obj * genv, lis_obj * obj) {
    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    print(genv, obj, buffer);
    stream_write_string(buffer, LSTR(U" is not a sequence."));
    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
}

void not_stream_error(lis_obj * genv, lis_obj * obj) {
    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    print(genv, obj, buffer);
    stream_write_string(buffer, LSTR(U" is not a stream."));
    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
}

void not_package_error(lis_obj * genv, lis_obj * obj) {
    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    print(genv, obj, buffer);
    stream_write_string(buffer, LSTR(U" is not a package."));
    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
}

void not_symbol_error(lis_obj * genv, lis_obj * obj) {
    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    print(genv, obj, buffer);
    stream_write_string(buffer, LSTR(U" is not a symbol."));
    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
}

void not_function_error(lis_obj * genv, lis_obj * obj) {
    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    print(genv, obj, buffer);
    stream_write_string(buffer, LSTR(U" is not a function."));
    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
}
