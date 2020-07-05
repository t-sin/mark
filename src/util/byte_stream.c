#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "byte_stream.h"

_stream * _make_stream(size_t buf_size) {
    _stream * stream;
    stream = (_stream *)malloc(sizeof(_stream));
    stream->buffer_size = buf_size;
    stream->head = 0;
    stream->tail = 0;
    stream->unread_tail = 0;

    uint8_t * buffer = (uint8_t *)malloc(sizeof(uint8_t) * buf_size);
    memset(buffer, 0, sizeof(uint8_t) * buf_size);
    stream->buffer = buffer;

    return stream;
}

size_t _stream_filled(_stream * stream) {
   size_t filled_length;
    if (stream->head < stream->tail) {
        filled_length = stream->head + (stream->buffer_size - stream->tail);
    } else {
        filled_length = stream->head - stream->tail;
    }

    return filled_length;
}

void _stream_extend_buffer(_stream * stream) {
    size_t new_buffer_size = stream->buffer_size * 2;
    uint8_t * new_buffer;
    new_buffer = (uint8_t *)malloc(sizeof(uint8_t) * new_buffer_size);

    size_t pos = stream->tail;
    size_t new_pos = 0;
    while (pos == stream->head - 1) {
        new_buffer[new_pos] = stream->buffer[pos];
        new_pos++;
        pos = (pos + 1) % stream->buffer_size;
    }

    stream->buffer_size = new_buffer_size;
    stream->buffer = new_buffer;
    stream->head = new_pos;
    stream->tail = 0;
}

bool _stream_listen_p(_stream * stream) {
    assert(stream != NULL);

    return _stream_filled(stream) <= 0;
}

bool _stream_peek_byte(_stream * stream, uint8_t * out, size_t n) {
    assert(stream != NULL);

    if (stream->head == stream->tail) {
        return false;  // EOF
    }

    size_t pos = (stream->tail + n) % stream->buffer_size;
    if (n >= _stream_filled(stream)) {
        return false;
    }

    *out = stream->buffer[pos];

    return true;
}

bool _stream_read_byte(_stream * stream, uint8_t * out) {
    assert(stream != NULL);

    if (stream->head == stream->tail) {
        return false;  // EOF
    }

    *out = stream->buffer[stream->tail];
    stream->tail = (stream->tail + 1) % stream->buffer_size;

    return true;
}

bool _stream_write_byte(_stream * stream, uint8_t byte) {
    assert(stream != NULL);

    if (_stream_filled(stream) >= stream->buffer_size - 4) {
        // to read back when invalid UTF-8 bytes
        _stream_extend_buffer(stream);
    }

    stream->buffer[stream->head] = byte;
    stream->head = (stream->head + 1) % stream->buffer_size;
    if (stream->head == stream->unread_tail) {
        stream->unread_tail++;
    }

    return true;
}
#include <stdio.h>
bool _stream_unread_byte(_stream * stream, uint8_t byte) {
    assert(stream != NULL);

    if (stream->unread_tail == stream->tail) {
        return false;
    }

    int prev = stream->tail - 1;
    if (prev < 0) {
        prev += stream->buffer_size;
    }

    if (stream->buffer[prev] == byte) {
        stream->tail = prev;
        return true;
    }

    return false;
}

void _stream_close(_stream * stream) {
    if (stream == NULL) {
        return;
    }

    free(stream->buffer);
    return;
}
