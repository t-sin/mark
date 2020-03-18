#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "../src/byte_stream.h"

void test_make_stream() {
    _stream * stream;
    stream = _make_stream(10);
    assert(stream->buffer_size == 10);

    stream = _make_stream(5);
    assert(stream->buffer_size == 5);
}

void test_count_filled() {
    _stream * stream = _make_stream(10);
    uint8_t b;

    _stream_write_byte(stream, 0);
    _stream_write_byte(stream, 1);
    _stream_write_byte(stream, 2);

    assert(_stream_filled(stream) == 3);

    _stream_read_byte(stream, &b);
    _stream_read_byte(stream, &b);

    assert(_stream_filled(stream) == 1);
}

void test_write_once() {
    _stream * stream = _make_stream(3);
    bool ret;

    assert(stream->head == 0);
    assert(stream->tail == 0);

    ret = _stream_write_byte(stream, 42);

    assert(ret == true);
    assert(stream->head == 1);
    assert(stream->tail == 0);
}

void test_peek_empty_stream() {
    _stream * stream = _make_stream(3);
    bool ret;
    uint8_t b;

    assert(stream->head == 0);
    assert(stream->tail == 0);

    ret = _stream_peek_byte(stream, &b, 0);

    assert(ret == false);
    assert(stream->head == 0);
    assert(stream->tail == 0);
}

void test_read_empty_stream() {
    _stream * stream = _make_stream(3);
    bool ret;
    uint8_t b;

    assert(stream->head == 0);
    assert(stream->tail == 0);

    ret = _stream_read_byte(stream, &b);

    assert(ret == false);
    assert(stream->head == 0);
    assert(stream->tail == 0);
}

void test_peek_once() {
    _stream * stream = _make_stream(3);
    bool ret;
    uint8_t b;

    ret = _stream_write_byte(stream, 42);

    assert(ret == true);
    assert(stream->head == 1);
    assert(stream->tail == 0);

    _stream_peek_byte(stream, &b, 0);

    assert(ret == true);
    assert(stream->head == 1);
    assert(stream->tail == 0);
}

void test_read_once() {
    _stream * stream = _make_stream(3);
    bool ret;
    uint8_t b;

    ret = _stream_write_byte(stream, 42);

    assert(ret == true);
    assert(stream->head == 1);
    assert(stream->tail == 0);

    _stream_read_byte(stream, &b);

    assert(ret == true);
    assert(b == 42);
    assert(stream->head == 1);
    assert(stream->tail == 1);
}

void test_peek() {
    _stream * stream = _make_stream(5);
    bool ret;
    uint8_t b;

    _stream_write_byte(stream, 42);
    _stream_write_byte(stream, 43);

    ret = _stream_peek_byte(stream, &b, 0);
    assert(ret == true);
    assert(b == 42);

    ret = _stream_peek_byte(stream, &b, 1);
    assert(ret == true);
    assert(b == 43);

    ret = _stream_peek_byte(stream, &b, 2);
    assert(ret == false);

    _stream_write_byte(stream, 44);

    ret = _stream_peek_byte(stream, &b, 2);
    assert(ret == true);
    assert(b == 44);
}

void test_ring_wraps_pointer_of_buffer_pointer() {
    _stream * stream = _make_stream(3);
    bool ret;
    uint8_t b;

    _stream_write_byte(stream, 1);
    _stream_read_byte(stream, &b);
    _stream_write_byte(stream, 2);
    _stream_read_byte(stream, &b);

    ret = _stream_write_byte(stream, 3);
    assert(ret == true);
    assert(stream->head == 0);
    assert(stream->tail == 2);

    ret = _stream_read_byte(stream, &b);
    assert(ret == true);
    assert(stream->head == 0);
    assert(stream->tail == 0);
    assert(b == 3);

    ret = _stream_write_byte(stream, 4);
    assert(ret == true);
    assert(stream->head == 1);
    assert(stream->tail == 0);

    ret = _stream_read_byte(stream, &b);
    assert(ret == true);
    assert(stream->head == 1);
    assert(stream->tail == 1);
    assert(b == 4);
}

void test_buffer_extention() {  // or the case when buffer is fulfilled
    _stream * stream = _make_stream(5);
    bool ret;

    assert(stream->buffer_size == 5);

    ret = _stream_write_byte(stream, 1);
    assert(ret == true);
    assert(stream->buffer_size == 5);

    _stream_write_byte(stream, 2);
    assert(ret == true);
    assert(stream->buffer_size == 10);

    _stream_write_byte(stream, 3);
    assert(ret == true);
    assert(stream->buffer_size == 10);
}

void test_read_after_buffer_extention() {
    _stream * stream = _make_stream(5);
    bool ret;
    uint8_t b;

    _stream_write_byte(stream, 1);
    _stream_write_byte(stream, 2);
    _stream_write_byte(stream, 3);
    _stream_write_byte(stream, 4);

    ret = _stream_read_byte(stream, &b);
    assert(ret == true);
    assert(b = 1);

    ret = _stream_read_byte(stream, &b);
    assert(ret == true);
    assert(b = 2);

    ret = _stream_read_byte(stream, &b);
    assert(ret == true);
    assert(b = 3);

    ret = _stream_read_byte(stream, &b);
    assert(ret == true);
    assert(b = 4);
}

void test_unread_empty_stream() {
    _stream * stream = _make_stream(5);
    bool ret;

    ret = _stream_unread_byte(stream, 0);
    assert(ret == false);
}

void test_unread_once() {
    _stream * stream = _make_stream(5);
    bool ret;
    uint8_t b;

    _stream_write_byte(stream, 42);

    assert(stream->head == 1);
    assert(stream->tail == 0);

    ret = _stream_read_byte(stream, &b);
    assert(ret == true);
    assert(stream->head == 1);
    assert(stream->tail == 1);
    assert(stream->unread_tail == 0);

    ret = _stream_unread_byte(stream, 40);
    assert(ret == false);
    assert(stream->head == 1);
    assert(stream->tail == 1);
    assert(stream->unread_tail == 0);

    ret = _stream_unread_byte(stream, 42);
    assert(ret == true);
    assert(stream->head == 1);
    assert(stream->tail == 0);
    assert(stream->unread_tail == 0);
}

void test_unread() {
    _stream * stream = _make_stream(5);
    bool ret;
    uint8_t b;

    _stream_write_byte(stream, 42);
    _stream_read_byte(stream, &b);

    assert(stream->head == 1);
    assert(stream->tail == 1);
    assert(stream->unread_tail == 0);  // no moves

    _stream_write_byte(stream, 52);
    _stream_read_byte(stream, &b);

    assert(stream->head == 2);
    assert(stream->tail == 2);
    assert(stream->unread_tail == 0);

    _stream_write_byte(stream, 62);
    _stream_read_byte(stream, &b);

    assert(stream->head == 3);
    assert(stream->tail == 3);
    assert(stream->unread_tail == 0);

    _stream_write_byte(stream, 72);
    _stream_read_byte(stream, &b);

    assert(stream->head == 4);
    assert(stream->tail == 4);
    assert(stream->unread_tail == 0);

    _stream_write_byte(stream, 82);
    _stream_read_byte(stream, &b);

    assert(stream->head == 0);
    assert(stream->tail == 0);
    assert(stream->unread_tail == 1);  // move by writing ring buffer

    // can unread because unread_tail is not overwritten by head
    ret = _stream_unread_byte(stream, 82);
    assert(ret == true);
    assert(stream->head == 0);
    assert(stream->tail == 4);
    assert(stream->unread_tail == 1);

    ret = _stream_unread_byte(stream, 72);
    assert(ret == true);
    assert(stream->head == 0);
    assert(stream->tail == 3);
    assert(stream->unread_tail == 1);

    ret = _stream_unread_byte(stream, 62);
    assert(ret == true);
    assert(stream->head == 0);
    assert(stream->tail == 2);
    assert(stream->unread_tail == 1);

    ret = _stream_unread_byte(stream, 52);
    assert(ret == true);
    assert(stream->head == 0);
    assert(stream->tail == 1);
    assert(stream->unread_tail == 1);

    // cannot unread because unread_tail is overwritten by head
    ret = _stream_unread_byte(stream, 42);
    assert(ret == false);
    assert(stream->head == 0);
    assert(stream->tail == 1);
    assert(stream->unread_tail == 1);
}

int main() {
    test_make_stream();
    test_count_filled();

    test_peek_empty_stream();
    test_read_empty_stream();

    test_write_once();
    test_peek_once();
    test_read_once();
    test_peek();

    test_ring_wraps_pointer_of_buffer_pointer();
    test_buffer_extention();
    test_read_after_buffer_extention();

    test_unread_empty_stream();
    test_unread_once();
    test_unread();

    return 0;
}
