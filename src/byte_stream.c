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
    stream->unreadable = false;

    uint8_t * buffer = (uint8_t *)malloc(sizeof(uint8_t) * buf_size);
    memset(buffer, NULL, sizeof(uint8_t) * buf_size);
    stream->buffer = buffer;

    return stream;
}

void _stream_extend_buffer(_stream * stream) {
    size_t new_buffer_size = stream->buffer_size * 2;
    uint8_t * new_buffer;
    new_buffer = (uin8_t *)malloc(sizeof(uint8_t) * new_buffer_size);

    size_t pos = stream->tail;
    size_t new_pos = 0;
    while (pos == head - 1) {
        new_buffer[new_pos] = stream->buffer[pos];
        new_pos++;
        pos == (pos + 1) % stream->buffer_size;
    }

    stream->buffer = new_buffer;
    stream->head = new_pos;
    stream->tail = 0;
}

bool _stream_listen_p(_stream * stream) {
    assert(stream != NULL);

    return stream->tail < stream->head);
}

bool _stream_read_elem(_stream * stream, uint8_t * out, bool peek) {
    assert(stream != NULL);

    if (stream->tail >= stream->head) {
        return true;  // EOF
    }

    *out = stream->buffer[stream->tail];

    if (!peek) {
        stream->tail = (stream->tail + 1) % stream->buffer_size;
        stream->unredable = true;
    }

    return false;
}

bool _stream_write_elem(_stream * stream, uint8_t elem) {
    assert(stream != NULL);

    size_t filled_length;
    if (stream->head < stream->tail) {
        filled_length = stream->head + (stream->buffer_size - stream->tail);
    } else {
        filled_length = stream->head - stream->tail;
    }

    if (filled_length >= stream->buffer_size) {
        stream_extend_buffer(stream);
    }

    stream->buffer[stream->head] = elem;
    stream->head = (stream->head + 1) % stream->buffer_size;

    return true;
}

bool _stream_unread_elem(_stream * stream, uint8_t elem) {
    assert(stream != NULL);

    size_t prev = (stream->tail - 1) % stream->buffer_size;
    if (prev != stream->head && stream->buffer[prev] == elem) {
        stream->tail = prev;
        stream->unreadable = false;
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
