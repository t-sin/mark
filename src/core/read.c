#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "lstring.h"
#include "list.h"
#include "basic_stream.h"
#include "package.h"
#include "eval.h"

#include "read.h"


void reader_error(lis_obj * genv, lis_obj * str) {
    lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
    stream_write_string(buffer, str);
    LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
}

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

bool is_colon(lis_char ch) {
    return ch == ':';
}

bool is_single_quote(lis_char ch) {
    return ch == '\'';
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

lis_obj * read(lis_obj * genv, lis_stream * stream);

lis_obj * read_integer(lis_obj * genv, lis_stream * stream) {
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

            lis_obj * integer = INT(atoi(s));
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

lis_obj * read_character(lis_obj * genv, lis_stream * stream) {
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
                reader_error(genv, LSTR(U"unexpected EOF while reading character."));
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

lis_obj * read_array(lis_obj * genv, lis_stream * stream) {
    reader_error(genv, LSTR(U"array is not implemented."));
    return NULL;
}

lis_obj * read_sharpmacro(lis_obj * genv, lis_stream * stream) {
    lis_char ch;
    if (!stream_peek_char(stream, &ch)) {
        reader_error(genv, LSTR(U"unexpected EOF while reading second character of sharp macro."));
        return NULL;
    }

    switch (ch) {
    case '\\':
        stream_read_char(stream, &ch);
        return read_character(genv, stream);
    case '(':
        stream_read_char(stream, &ch);
        return read_array(genv, stream);
    default:
        stream_read_char(stream, &ch);
        reader_error(genv, LSTR(U"unknown sharp macro character."));
        return NULL;
    }
}

lis_obj * read_string(lis_obj * genv, lis_stream * stream) {
    lis_char ch;
    size_t size = 0;
    lis_stream * buffer = make_lis_stream(100, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    while (true) {
        if (!stream_peek_char(stream, &ch) || is_string_delimiter(ch)) {
            stream_read_char(stream, &ch);

            lis_obj * str = _make_string();
            LIS_STR(str)->size = size;
            LIS_STR(str)->body = (lis_char *)malloc(sizeof(lis_char) * size);

            for (int i=0; i<size; i++) {
                stream_read_char(buffer, &ch);
                LIS_STR(str)->body[i] = ch;
            }

            return str;

        } else {
            stream_read_char(stream, &ch);
            stream_write_char(buffer, ch);
            size++;
        }
    }

    reader_error(genv, LSTR(U"unexpected EOF while reading string."));
    return NULL;
}

lis_obj * read_symbol(lis_obj * genv, lis_stream * stream, lis_obj * pkg) {
    lis_char ch;
    size_t size = 0;
    lis_stream * buffer = make_lis_stream(1000, LIS_STREAM_INOUT, LIS_STREAM_TEXT);

    while (true) {
        if (!stream_peek_char(stream, &ch) ||
            is_whitespace(ch) ||
            is_newline(ch) ||
            is_string_delimiter(ch) ||
            is_cons_delimiters(ch)) {

            if (size == 0) {
                reader_error(genv, LSTR(U"symbol with zero-length name."));
                return NULL;
            }

            lis_obj * name = _make_string();
            LIS_STR(name)->size = size;
            LIS_STR(name)->body = (lis_char *)malloc(sizeof(lis_char) * size);
            for (int i=0; i<size; i++) {
                stream_read_char(buffer, &ch);
                LIS_STR(name)->body[i] = ch;
            }

            lis_obj * sym;
            intern(genv, pkg, name, &sym);

            return sym;

        } else {
            stream_read_char(stream, &ch);
            stream_write_char(buffer, ch);
            size++;
        }
    }

    reader_error(genv, LSTR(U"unexpected EOF while reading symbol."));
    return NULL;
}

lis_obj * read_cons(lis_obj * genv, lis_stream * stream) {
    lis_obj * head = _make_cons();
    lis_obj * current = head;
    lis_obj * prev_current = NULL;
    lis_char ch;

    while (true) {
        if (stream_peek_char(stream, &ch) && is_cons_close_delimiter(ch)) {
            stream_read_char(stream, &ch);
            if (current != head) {
                LIS_CONS(prev_current)->cdr = LIS_NIL(genv);
            } else {
                head = LIS_NIL(genv);
            }
            break;

        } else {
            lis_obj * car = read(genv, stream);
            if (car == NULL) {
                reader_error(genv, LSTR(U"car of read_cons() is NULL!"));
                return NULL;
            }

            LIS_CONS(current)->car = car;
            skip_whitespaces(stream);

            if (stream_peek_char(stream, &ch) && is_cons_delimiter(ch)) {
                // cons
                stream_read_char(stream, &ch);
                LIS_CONS(current)->cdr = read(genv, stream);

                skip_whitespaces(stream);
                stream_peek_char(stream, &ch);
                if (!is_cons_close_delimiter(ch)) {
                    reader_error(genv, LSTR(U"cons?"));
                    return NULL;
                } else {
                    stream_read_char(stream, &ch);
                    break;
                }

            } else {
                // list
                lis_obj * cdr = _make_cons();
                LIS_CONS(current)->cdr = cdr;
                prev_current = current;
                current = cdr;
            }
        }
    }

    return head;
}

lis_obj * read(lis_obj * genv, lis_stream * stream) {
    if (skip_whitespaces(stream) == EOF) {
        return NULL;
    }

    lis_obj * obj;
    lis_char ch;
    bool ret = stream_peek_char(stream, &ch);

    if (!ret) {
        obj = NULL;

    } else if (is_numeric(ch)) {
        obj = read_integer(genv, stream);

    } else if (is_sharpmacro(ch)) {
        stream_read_char(stream, &ch);
        obj = read_sharpmacro(genv, stream);

    } else if (is_string_delimiter(ch)) {
        stream_read_char(stream, &ch);
        obj = read_string(genv, stream);

    } else if (is_cons_open_delimiter(ch)) {
        stream_read_char(stream, &ch);
        obj = read_cons(genv, stream);

    } else if (is_colon(ch)) {
        stream_read_char(stream, &ch);
        obj = read_symbol(genv, stream, LIS_GENV(genv)->keyword_package);

    } else if (is_single_quote(ch)) {
        stream_read_char(stream, &ch);
        lis_obj * quoted = read(genv, stream);
        if (quoted == NULL) {
            return quoted;
        }
        lis_obj * quote;
        intern(genv, LIS_GENV(genv)->lis_package, LSTR(U"quote"), &quote);
        lis_obj * list = _list_cons(genv, quote, _list_cons(genv, quoted, LIS_NIL(genv)));
        obj = list;

    } else {
        obj = read_symbol(genv, stream, LIS_GENV(genv)->current_package);
    }

    return obj;
}

lis_obj * lisp_read(lis_obj * genv, lis_obj * args) {
    lis_obj * obj = _list_nth(genv, INT(0), args);
    lis_stream * stream;

    if (obj == LIS_NIL(genv)) {
        stream = LIS_STREAM(LIS_GENV(genv)->stream_stdin);
    } else {
        stream = LIS_STREAM(obj);
    }

    return read(genv, stream);
}
