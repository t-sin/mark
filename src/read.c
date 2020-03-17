#include <stdbool.h>
#include <stdlib.h>

#include "obj.h"
#include "stream.h"

bool is_newline(lis_char ch) {
    return ch == '\n';
}

bool is_whitespace(lis_char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n';
}

void skip_whitespaces(lis_stream * stream) {
    lis_char ch;
    bool is_comment = false;

    while (true) {
        if (!stream_peek_char(stream, &ch)) {
            return;
        }

        if (!is_comment && ch == ';') {
            is_comment = true;
            stream_read_char(stream, &ch);

        } else if (!is_comment && is_whitespace(ch)) {
            stream_read_char(stream, &ch);

        } else if (is_comment && is_newline(ch)) {
            stream_read_char(stream, &ch);
            return;

        } else {
            return;
        }
    }
}

lis_obj * read_string(lis_stream * stream) {
    lis_char ch;
    size_t size = 0;
    lis_stream * buffer = make_lis_stream(1000, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    while (true) {
        if (!stream_peek_char(stream, &ch)) {
            return NULL;
        }

        if (ch == '"') {
            stream_read_char(stream, &ch);

            lis_obj * str = make_string();
            str->data.str->size = size;
            str->data.str->body = (lis_char *)malloc(sizeof(lis_char) * size);

            for (int i=0; i<size; i++) {
                stream_read_char(buffer, &ch);
                str->data.str->body[i] = ch;
            }

            return str;

        } else {
            stream_read_char(stream, &ch);
            stream_write_char(buffer, ch);
            size++;
        }
    }

    return NULL;
}

lis_obj * read(lis_stream * stream) {
    skip_whitespaces(stream);

    lis_char ch;
    if (!stream_peek_char(stream, &ch)) {
        return NULL;
    }

    if (ch == '"') {
        stream_read_char(stream, &ch);
        return read_string(stream);
    } else {
        stream_read_char(stream, &ch);
        return NULL;
    }
}
