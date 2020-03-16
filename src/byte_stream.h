#ifndef __byte_stream
#define __byte_stream

#include <stdbool.h>
#include <stdint.h>

typedef struct _stream {
    uint8_t * buffer;
    size_t buffer_size;
    size_t head;
    size_t tail;
    bool unreadable;
} _stream;

_stream * _make_stream(size_t buf_size);

size_t _stream_filled(_stream * stream);

bool _stream_listen_p(_stream * stream);
void _stream_extend_buffer(_stream * stream);
void _stream_close(_stream * stream);

bool _stream_peek_byte(_stream * stream, uint8_t * out, size_t n);
bool _stream_read_byte(_stream * stream, uint8_t * out);
bool _stream_unread_byte(_stream * stream, uint8_t byte);
bool _stream_write_byte(_stream * stream, uint8_t elem);

#endif
