#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "stream.h"
#include "runtime.h"

bool is_cons_open_delimiter(lis_char ch) {
    return ch == '(';
}

bool is_cons_close_delimiter(lis_char ch) {
    return ch == ')';
}

bool is_cons_delimiter(lis_char ch) {
    return ch == '.';
}

bool is_cons_delimiters(lis_char ch) {
    return (is_cons_delimiter(ch) ||
            is_cons_open_delimiter(ch) ||
            is_cons_close_delimiter(ch));
}

bool is_string_delimiter(lis_char ch) {
    return ch == '"';
}

bool is_sharpmacro(lis_char ch) {
    return ch == '#';
}

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

lis_obj * read(lis_stream * stream, lis_runtime * runtime);

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

            lis_obj * integer = _make_int(atoi(s));
            return integer;

        } else {
            stream_read_char(stream, &ch);
            stream_write_char(buffer, ch);
            size++;
        }
    }
}

#define NUMBER_OF_CHAR_NAMES 3
const char * CHAR_NAME_TABLE[NUMBER_OF_CHAR_NAMES][2] = {
    {"tab", "\t"},
    {"newline", "\n"},
    {"space", " "},
};

int get_char_from_name(lis_char * name, size_t size) {
    for (int n=0; n<NUMBER_OF_CHAR_NAMES; n++) {
        bool flag_acc = true;

        for (int i=0; i<size; i++) {
            char ch = CHAR_NAME_TABLE[n][0][i];
            flag_acc &= (ch == name[i]);
            if (!flag_acc || ch == '\0') {
                break;
            }
        }

        if (flag_acc) {
            return CHAR_NAME_TABLE[n][1][0];
        }
    }
    return EOF;
}

lis_obj * read_character(lis_stream * stream) {
    lis_char ch;
    size_t size = 0;
    lis_stream * buffer = make_lis_stream(10, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    while (true) {
        if (!stream_peek_char(stream, &ch) ||
            is_whitespace(ch) ||
            is_string_delimiter(ch) ||
            is_cons_delimiters(ch)) {

            if (size == 1) {
                stream_read_char(buffer, &ch);

            } else {
                lis_char * name = (lis_char *)malloc(sizeof(lis_char) * size);
                for (int i=0; i<size; i++) {
                    stream_read_char(buffer, &ch);
                    name[i] = ch;
                }

                ch = get_char_from_name(name, size);
            }

            if (ch == EOF) {
                return NULL;
            } else {
                return _make_char(ch);
            }

        } else {
            stream_read_char(stream, &ch);
            stream_write_char(buffer, ch);
            size++;
        }
    }
}

lis_obj * read_array(lis_stream * stream) {
    return NULL;
}

lis_obj * read_sharpmacro(lis_stream * stream) {
    lis_char ch;
    if (!stream_peek_char(stream, &ch)) {
        return NULL;
    }

    switch (ch) {
    case '\\':
        stream_read_char(stream, &ch);
        return read_character(stream);
    case '(':
        stream_read_char(stream, &ch);
        return read_array(stream);
    default:
        stream_read_char(stream, &ch);
        return NULL;
    }
}

lis_obj * read_string(lis_stream * stream) {
    lis_char ch;
    size_t size = 0;
    lis_stream * buffer = make_lis_stream(100, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    while (true) {
        if (!stream_peek_char(stream, &ch) || is_string_delimiter(ch)) {
            stream_read_char(stream, &ch);

            lis_obj * str = _make_string();
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

lis_obj * read_symbol(lis_stream * stream, lis_runtime * runtime) {
    lis_char ch;
    size_t size = 0;
    lis_stream * buffer = make_lis_stream(1000, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    while (true) {
        if (!stream_peek_char(stream, &ch) ||
            is_whitespace(ch) ||
            is_newline(ch) ||
            is_string_delimiter(ch) ||
            is_cons_delimiters(ch)) {

            lis_obj * name = _make_string();
            // intern
            lis_obj * sym = _make_symbol(name);
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

lis_obj * read_cons(lis_stream * stream, lis_runtime * runtime) {
    lis_obj * head = _make_cons();
    lis_obj * current = head;
    lis_obj * prev_current = NULL;
    lis_char ch;

    while (true) {
        if (stream_peek_char(stream, &ch) && is_cons_close_delimiter(ch)) {
            stream_read_char(stream, &ch);
            if (current != head) {
                prev_current->data.cons->cdr = _make_nil();
            } else {
                head = _make_nil();
            }
            break;

        } else {
            current->data.cons->car = read(stream, runtime);
            skip_whitespaces(stream);

            if (stream_peek_char(stream, &ch) && is_cons_delimiter(ch)) {
                // cons
                stream_read_char(stream, &ch);
                current->data.cons->cdr = read(stream, runtime);

                skip_whitespaces(stream);
                stream_peek_char(stream, &ch);
                if (!is_cons_close_delimiter(ch)) {
                    return NULL;
                } else {
                    stream_read_char(stream, &ch);
                    break;
                }

            } else {
                // list
                lis_obj * cdr = _make_cons();
                current->data.cons->cdr = cdr;
                prev_current = current;
                current = cdr;
            }
        }
    }

    return head;
}

lis_obj * read(lis_stream * stream, lis_runtime * runtime) {
    if (skip_whitespaces(stream) == EOF) {
        return NULL;
    }

    lis_obj * obj;
    lis_char ch;
    bool ret = stream_peek_char(stream, &ch);

    if (!ret) {
        obj = NULL;

    } else if (is_numeric(ch)) {
        obj = read_integer(stream);

    } else if (is_sharpmacro(ch)) {
        stream_read_char(stream, &ch);
        obj = read_sharpmacro(stream);

    } else if (is_string_delimiter(ch)) {
        stream_read_char(stream, &ch);
        obj = read_string(stream);

    } else if (is_cons_open_delimiter(ch)) {
        stream_read_char(stream, &ch);
        obj = read_cons(stream, runtime);

    } else {
        obj = read_symbol(stream, runtime);
    }

    return obj;
}
