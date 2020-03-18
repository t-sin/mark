#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "stream.h"

bool is_newline(lis_char ch) {
    return ch == '\n';
}

bool is_whitespace(lis_char ch) {
    return ch == ' ' || ch == '\t' || ch == '\n';
}

static const char NUMERIC_CHARS[] = "0123456789";
bool is_numeric(lis_char ch) {
    return strchr(NUMERIC_CHARS, ch) != NULL;
}

int skip_whitespaces(lis_stream * stream) {
    lis_char ch;
    int n = 0;
    bool is_comment = false;

    while (true) {
        if (!stream_peek_char(stream, &ch)) {
            return EOF;
        }

        if (!is_comment && ch == ';') {
            is_comment = true;
            stream_read_char(stream, &ch);

        } else if (!is_comment && is_whitespace(ch)) {
            n++;
            stream_read_char(stream, &ch);

        } else if (is_comment && is_newline(ch)) {
            n++;
            stream_read_char(stream, &ch);
            return n;

        } else {
            return n;
        }
    }
}

lis_obj * read_integer(lis_stream * stream) {
    lis_char ch;
    size_t size = 0;
    lis_stream * buffer = make_lis_stream(10, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    while (true) {
        if (!stream_peek_char(stream, &ch) || !is_numeric(ch)) {
            char * s = (char *)malloc(sizeof(char) * size + 1);
            memset(s, 0, size);

            for (int i=0; i<size; i++) {
                stream_read_char(buffer, &ch);
                s[i] = ch;
            }

            lis_obj * integer = make_int(atoi(s));
            return integer;

        } else {
            stream_read_char(stream, &ch);
            stream_write_char(buffer, ch);
            size++;
        }
    }
}

lis_obj * read_string(lis_stream * stream) {
    lis_char ch;
    size_t size = 0;
    lis_stream * buffer = make_lis_stream(100, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    while (true) {
        if (!stream_peek_char(stream, &ch) || ch == '"') {
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

lis_obj * read_symbol(lis_stream * stream) {
    lis_char ch;
    size_t size = 0;
    lis_stream * buffer = make_lis_stream(1000, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    while (true) {
        if (!stream_peek_char(stream, &ch) ||
            is_whitespace(ch) ||
            is_newline(ch)) {

            lis_obj * name = make_string();
            lis_obj * sym = make_symbol(name);
            name->data.str->size = size;
            name->data.str->body = (lis_char *)malloc(sizeof(lis_char) * size);

            for (int i=0; i<size; i++) {
                stream_read_char(buffer, &ch);
                name->data.str->body[i] = ch;
            }

            return sym;

        } else {
            stream_read_char(stream, &ch);
            stream_write_char(buffer, ch);
            size++;
        }
    }

    return NULL;
}

lis_obj * read(lis_stream * stream) {
    if (skip_whitespaces(stream) == EOF) {
        return NULL;
    }

    lis_obj * obj;
    lis_char ch;

    if (!stream_peek_char(stream, &ch)) {
        obj = NULL;

    } else if (is_numeric(ch)) {
        obj = read_integer(stream);

    } else if (ch == '"') {
        stream_read_char(stream, &ch);
        obj = read_string(stream);

    } else {
        obj = read_symbol(stream);
    }

    return obj;
}
