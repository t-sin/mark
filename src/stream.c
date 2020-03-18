#include <stdlib.h>
#include <string.h>

#include "byte_stream.h"
#include "utf8.h"
#include "obj.h"
#include "stream.h"

lis_stream * make_lis_stream(size_t buf_size,
                             lis_stream_direction_type direction,
                             lis_stream_element_type etype) {
    lis_stream * stream;
    stream = (lis_stream *)malloc(sizeof(lis_stream));
    stream->stream = _make_stream(buf_size);
    stream->fin = NULL;
    stream->fout = NULL;
    stream->direction = direction;
    stream->element_type = etype;

    if (etype == LIS_STREAM_TEXT) {
        stream->decode_state = make_utf8_decoding_state();
    } else {
        stream->decode_state = NULL;
    }

    return stream;
}

void stream_fill_buffer(lis_stream * stream) {
    if (stream->fin == NULL) {
        return;
    }

    if (_stream_filled(stream->stream) > 0) {
        return;
    }

    for (int i=0; i<stream->stream->buffer_size/2; i++) {
        int ch = fgetc(stream->fin);
        if (ch == EOF) {
            break;
        } else if (ch == '\n') {
            _stream_write_byte(stream->stream, (lis_byte)ch);
            break;
        }

        _stream_write_byte(stream->stream, (lis_byte)ch);
    }
}

bool stream_peek_byte(lis_stream * stream, lis_byte * out, size_t n) {
    if (stream->direction != LIS_STREAM_IN &&
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    stream_fill_buffer(stream);
    return _stream_peek_byte(stream->stream, out, n);
}

bool stream_read_byte(lis_stream * stream, lis_byte * out) {
    if (stream->direction != LIS_STREAM_IN &&
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    stream_fill_buffer(stream);
    return _stream_read_byte(stream->stream, out);
}

bool stream_write_byte(lis_stream * stream, lis_byte b) {
    if (stream->direction != LIS_STREAM_OUT &&
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    if (stream->fout != NULL &&
        _stream_filled(stream->stream) > stream->stream->buffer_size/2) {
        lis_byte b;
        while (true) {
            if (!_stream_read_byte(stream->stream, &b)) {
                break;
            }
            fputc((char)b, stream->fout);
        }
    }

    return _stream_write_byte(stream->stream, b);
}

bool stream_peek_char(lis_stream * stream, lis_char * out) {
    if (stream->direction != LIS_STREAM_IN &&
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    if (stream->element_type != LIS_STREAM_TEXT) {
        return false;
    }

    lis_byte b;
    lis_char cp;
    size_t peek_n = 0;

    while (true) {
        if (!stream_peek_byte(stream, &b, peek_n)) {
            return false;
        }

        utf8_decoding_status status = utf8_decode_byte(stream->decode_state, b, &cp);
        if (status == UTF8_INVALID_BYTE) {
            return false;
        } else if (status == UTF8_DECODED) {
            reset_utf8_decoding_state(stream->decode_state);
            *out = cp;
            return true;
        }

        peek_n++;
    }
}

bool stream_read_char(lis_stream * stream, lis_char * out) {
    if (stream->direction != LIS_STREAM_IN &&
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    if (stream->element_type != LIS_STREAM_TEXT) {
        return false;
    }

    lis_byte b;
    lis_char cp;
    size_t peek_n = 0;

    while (true) {
        if (!stream_peek_byte(stream, &b, peek_n++)) {
            return false;
        }

        utf8_decoding_status status = utf8_decode_byte(stream->decode_state, b, &cp);
        if (status == UTF8_INVALID_BYTE) {
            return false;
        } else if (status == UTF8_DECODED) {
            *out = cp;
            reset_utf8_decoding_state(stream->decode_state);
            for (; peek_n>0; peek_n--) {
                _stream_read_byte(stream->stream, &b);
            }
            return true;
        }
    }
}

bool stream_unread_char(lis_stream * stream, lis_char ch) {
    if (stream->direction != LIS_STREAM_IN &&
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    if (stream->element_type != LIS_STREAM_TEXT) {
        return false;
    }

    uint8_t bytes[4];
    int len = utf8_encode_codepoint(ch, bytes);

    if (len == 0) {
        return false;
    }

    int i;
    lis_byte _b;
    for (i=0; i<len; i++) {
        if (!_stream_unread_byte(stream->stream, bytes[i])) {
            for (; i>0; i--) {
                _stream_read_byte(stream->stream, &_b);
            }
            return false;
        }
    }

    return true;
}

bool stream_write_char(lis_stream * stream, lis_char ch) {
    if (stream->direction != LIS_STREAM_IN &&
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    if (stream->element_type != LIS_STREAM_TEXT) {
        return false;
    }

    uint8_t bytes[4];
    int len = utf8_encode_codepoint(ch, bytes);

    if (len == 0) {
        return false;
    }


    for (int i=0; i<len; i++) {
        if (!stream_write_byte(stream, bytes[i])) {
            return false;
        }
    }

    return true;
}
