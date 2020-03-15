#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "../src/byte_stream.h"
#include "../src/obj.h"
#include "../src/stream.h"

void test_read_write_bytes() {
    lis_stream * stream = make_lis_stream(5, LIS_STREAM_IN, LIS_STREAM_BINARY);
    bool ret;
    lis_byte b;

    ret = stream_read_byte(stream, &b);
    assert(ret == false);

    _stream_write_byte(stream->stream, 42);
    _stream_write_byte(stream->stream, 52);

    ret = stream_read_byte(stream, &b);
    assert(ret == true);
    assert(b == 42);

    _stream_write_byte(stream->stream, 62);

    ret = stream_read_byte(stream, &b);
    assert(ret == true);
    assert(b == 52);

    ret = stream_read_byte(stream, &b);
    assert(ret == true);
    assert(b == 62);

    ret = stream_read_byte(stream, &b);
    assert(ret == false);

    _stream_write_byte(stream->stream, 72);

    ret = stream_read_byte(stream, &b);
    assert(ret == true);
    assert(b == 72);
}

int main() {
    test_read_write_bytes();
    return 0;
}
