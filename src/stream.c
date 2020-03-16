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

bool stream_read_byte(lis_stream * stream, lis_byte * out) {
    if (stream->direction != LIS_STREAM_IN &&
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    if (stream->element_type != LIS_STREAM_BINARY) {
        return false;
    }

    if (stream->fin != NULL &&
        _stream_filled(stream->stream) <= 0) {
        for (int i=0; i<stream->stream->buffer_size/2; i++) {
            int ch = fgetc(stream->fin);
            if (ch == EOF) {
                break;
            }
            _stream_write_byte(stream->stream, (lis_byte)ch);
        }
    }

    return _stream_read_byte(stream->stream, out, false);
}

bool stream_write_byte(lis_stream * stream, lis_byte b) {
    if (stream->direction != LIS_STREAM_OUT &&
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    if (stream->element_type != LIS_STREAM_BINARY) {
        return false;
    }

    if (stream->fout != NULL &&
        _stream_filled(stream->stream) > stream->stream->buffer_size/2) {
        lis_byte b;
        while (true) {
            if (!_stream_read_byte(stream->stream, &b, false)) {
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

}

bool stream_read_char(lis_stream * stream, lis_char * out) {
    if (stream->direction != LIS_STREAM_IN &&
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    if (stream->element_type != LIS_STREAM_TEXT) {
        return false;
    }

}

bool stream_unread_char(lis_stream * stream, uint8_t elem) {
    if (stream->direction != LIS_STREAM_IN &&
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    if (stream->element_type != LIS_STREAM_TEXT) {
        return false;
    }
}

bool stream_write_char(lis_stream * stream, lis_char ch) {
    if (stream->direction != LIS_STREAM_IN ||
        stream->direction != LIS_STREAM_INOUT) {
        return false;
    }

    if (stream->element_type != LIS_STREAM_TEXT) {
        return false;
    }
}
