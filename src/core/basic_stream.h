#ifndef __lis_basic_stream
#define __lis_basic_stream

#include <stdbool.h>
#include <stdint.h>

#include "../util/byte_stream.h"
#include "obj.h"

lis_stream * make_lis_stream(size_t buf_size,
                             lis_stream_direction_type direction,
                             lis_stream_element_type etype);

bool stream_peek_byte(lis_stream * stream, lis_byte * out, size_t n);
bool stream_read_byte(lis_stream * stream, lis_byte * out);
bool stream_write_byte(lis_stream * stream, lis_byte b);

bool stream_peek_char(lis_stream * stream, lis_char * out);
bool stream_read_char(lis_stream * stream, lis_char * out);
bool stream_unread_char(lis_stream * stream, lis_char ch);
bool stream_write_char(lis_stream * stream, lis_char ch);

bool stream_write_string(lis_stream * stream, lis_obj * str);
lis_obj * stream_output_to_string(lis_stream * stream);

bool stream_listen_p(lis_stream * stream);
void stream_flush(lis_stream * stream);

#endif
