#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "../src/util/byte_stream.h"
#include "../src/core/obj.h"
#include "../src/core/stream.h"

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

void test_peek_and_read_char() {
    lis_stream * stream = make_lis_stream(20, LIS_STREAM_IN, LIS_STREAM_TEXT);
    bool ret;
    lis_char cp;

    // prepare
    char s[] = u8"abcあいう";
    for (int i=0; i<sizeof(s)-1; i++) {  // avoid null character
        _stream_write_byte(stream->stream, s[i]);
    }

    ret = stream_peek_char(stream, &cp);
    assert(ret == true);
    assert(cp == 'a');

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == 'a');

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == 'b');

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == 'c');

    ret = stream_peek_char(stream, &cp);
    assert(ret == true);
    assert(cp == 0x3042);  // あ

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == 0x3042);  // あ

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == 0x3044);  // い

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == 0x3046);  // う

    ret = stream_peek_char(stream, &cp);
    assert(ret == false);
}

void test_read_and_write_chars() {
    lis_stream * stream = make_lis_stream(20, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    bool ret;
    lis_char cp;

    ret = stream_write_char(stream, 'a');
    assert(ret == true);

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == 'a');

    int32_t ch_a = 0x3042;  // あ
    ret = stream_write_char(stream, ch_a);
    assert(ret == true);

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == ch_a);

    ret = stream_write_char(stream, '1');
    ret = stream_write_char(stream, '2');
    ret = stream_write_char(stream, '3');

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == '1');

    ret = stream_write_char(stream, '4');

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == '2');

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == '3');

    ret = stream_read_char(stream, &cp);
    assert(ret == true);
    assert(cp == '4');

    ret = stream_read_char(stream, &cp);
    assert(ret == false);
}

void test_unread_char() {
    lis_stream * stream = make_lis_stream(20, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    bool ret;
    lis_char cp;

    stream_write_char(stream, '1');
    stream_write_char(stream, '2');
    stream_write_char(stream, '3');
    stream_write_char(stream, '4');

    ret = stream_unread_char(stream, '1');
    assert(ret == false);

    stream_read_char(stream, &cp);  // 1
    stream_read_char(stream, &cp);  // 2
    ret = stream_read_char(stream, &cp);  // 3
    assert(ret == true);
    assert(cp == '3');

    ret = stream_unread_char(stream, '3');
    assert(ret == true);
    ret = stream_unread_char(stream, '2');
    assert(ret == true);
    ret = stream_unread_char(stream, '1');
    assert(ret == true);
    ret = stream_unread_char(stream, '4');
    assert(ret == false);
}

int main() {
    test_read_write_bytes();
    test_peek_and_read_char();
    test_read_and_write_chars();
    test_unread_char();
    return 0;
}
