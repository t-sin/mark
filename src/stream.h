#ifndef __lis_stream
#define __lis_stream

#include <stdbool.h>
#include <stdint.h>

#include "byte_stream.h"
#include "obj.h"

lis_stream * make_lis_stream(size_t buf_size,
                             lis_stream_direction_type direction,
                             lis_stream_element_type etype);

bool stream_read_byte(lis_stream * stream, lis_byte * out);
bool stream_write_byte(lis_stream * stream, lis_byte b);

bool stream_peek_char(lis_stream * stream, lis_char * out);
bool stream_read_char(lis_stream * stream, lis_char * out);
bool stream_unread_char(lis_stream * stream, uint8_t elem);
bool stream_write_char(lis_stream * stream, lis_char ch);

void stream_flush(lis_stream * stream);

#endif
