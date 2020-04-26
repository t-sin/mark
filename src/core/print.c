#include <assert.h>
#include <stdio.h>
#include <uchar.h>

#include "obj.h"
#include "lstring.h"
#include "list.h"
#include "basic_stream.h"
#include "eval.h"

#include "print.h"

void print_string(lis_stream * stream, lis_obj * str) {
    for (size_t i=0; i < str->data.str->size; i++) {
        stream_write_char(stream, str->data.str->body[i]);
    }
}

void print_cons(lis_obj * genv, lis_obj * car, lis_obj * cdr, lis_stream * stream) {
    print(genv, car, stream);
    if (LIS_TAG_BASE(cdr) == LIS_TAG_BASE_BUILTIN && LIS_TAG_TYPE(cdr) == LIS_TAG_TYPE_CONS) {
        stream_write_char(stream, ' ');
        print_cons(genv, cdr->data.cons->car, cdr->data.cons->cdr, stream);
    } else if (cdr == genv->data.env->env.global->symbol_nil) {
        ;
    } else {
        stream_write_char(stream, ' ');
        stream_write_char(stream, '.');
        stream_write_char(stream, ' ');
        print(genv, cdr, stream);
    }
}

#define BUF_SIZE 256
void print(lis_obj * genv, lis_obj * obj, lis_stream * stream) {
    assert(LIS_TAG_BASE(genv) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(genv) == LIS_TAG_TYPE_ENV);
    assert(genv->data.env->type == LIS_ENV_GLOBAL);

    if (obj == NULL) {
        return;
    }

    char buf[BUF_SIZE];

    if (LIS_TAG_BASE(obj) == LIS_TAG_BASE_INT) {
        snprintf(buf, BUF_SIZE, "%d", obj->data.num);
        for (int i=0; i<BUF_SIZE; i++) {
            if (buf[i] == '\0') return;
            stream_write_char(stream, buf[i]);
        }

    } else if (LIS_TAG_BASE(obj) == LIS_TAG_BASE_CHAR) {
        stream_write_char(stream, '#');
        stream_write_char(stream, '\\');
        stream_write_char(stream, obj->data.ch);

    } else if (LIS_TAG_BASE(obj) == LIS_TAG_BASE_BUILTIN) {
        // built-in types
        switch (LIS_TAG_TYPE(obj)) {
        case LIS_TAG_TYPE_ARY:
            stream_write_char(stream, '#');
            stream_write_char(stream, '(');
            for (size_t i=0; i < obj->data.array->size; i++) {
                print(genv, &obj->data.array->body[i], stream);
                if (i+1 < obj->data.array->size) {
                    stream_write_char(stream, ' ');
                }
            }
            stream_write_char(stream, ')');
            break;

        case LIS_TAG_TYPE_STR:
            stream_write_char(stream, '"');
            print_string(stream, obj);
            stream_write_char(stream, '"');
            break;

        case LIS_TAG_TYPE_TS:
        case LIS_TAG_TYPE_SYM:
            if (LIS_SYM(obj)->package == LIS_GENV(genv)->keyword_package) {
                stream_write_char(stream, ':');
            }
            stream_write_string(stream, LIS_SYM(obj)->name);
            break;

        case LIS_TAG_TYPE_CONS:
            stream_write_char(stream, '(');
            print_cons(genv, obj->data.cons->car, obj->data.cons->cdr, stream);
            stream_write_char(stream, ')');
            break;

        case LIS_TAG_TYPE_ENV:
            stream_write_string(stream, LSTR(U"#<FN: "));
            if (obj->data.env->type == LIS_ENV_GLOBAL) {
                stream_write_string(stream, LSTR(U"global "));
            } else if (obj->data.env->type == LIS_ENV_DYNAMIC) {
                stream_write_string(stream, LSTR(U"dynamic "));
            } else if (obj->data.env->type == LIS_ENV_LEXICAL) {
                stream_write_string(stream, LSTR(U"lexical "));
            }

            snprintf(buf, BUF_SIZE, "%u", obj->data.num);
            for (int i=0; i<BUF_SIZE; i++) {
                if (buf[i] == '\0') break;
                stream_write_char(stream, buf[i]);
            }

            stream_write_char(stream, '>');
            break;

        case LIS_TAG_TYPE_FN:
            stream_write_string(stream, LSTR(U"#<FN: "));

            if (obj->data.fn->type == LIS_FUNC_MACRO) {
                stream_write_string(stream, LSTR(U"macro "));
            }
            if (obj->data.fn->type == LIS_FUNC_SPECIAL_FORM) {
                stream_write_string(stream, LSTR(U"special "));
            }

            snprintf(buf, BUF_SIZE, "%u", obj->data.num);
            for (int i=0; i<BUF_SIZE; i++) {
                if (buf[i] == '\0') break;
                stream_write_char(stream, buf[i]);
            }
            stream_write_char(stream, '>');
            break;

        case LIS_TAG_TYPE_CLS:
            stream_write_string(stream, LSTR(U"#<CLOSURE:"));

            snprintf(buf, BUF_SIZE, "%u", obj->data.num);
            for (int i=0; i<BUF_SIZE; i++) {
                if (buf[i] == '\0') break;
                stream_write_char(stream, buf[i]);
            }
            stream_write_char(stream, '>');
            break;

        case LIS_TAG_TYPE_PKG:
            stream_write_string(stream, LSTR(U"#<PACKAGE:"));
            stream_write_string(stream, LIS_PKG(obj)->name);
            stream_write_char(stream, '>');
            break;

        case LIS_TAG_TYPE_STRM:
            stream_write_string(stream, LSTR(U"#<STREAM:"));
            snprintf(buf, BUF_SIZE, "%p", (void *)obj);
            for (int i=0; i<BUF_SIZE; i++) {
                if (buf[i] == '\0') break;
                stream_write_char(stream, buf[i]);
            }
            stream_write_char(stream, '>');
            break;

        case LIS_TAG_TYPE_MVAL:
            print(genv, _list_car(genv, LIS_MVAL(obj)), stream);
            break;
        }
    } else {
        printf("unknown object: %x\n", obj->tags);
    }
}

void print_mval(lis_obj * genv, lis_obj * obj, lis_stream * stream) {
    if (LIS_TAG_BASE(obj) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(obj) != LIS_TAG_TYPE_MVAL) {
        print(genv, obj, stream);
        return;
    }

    lis_obj * rest = LIS_MVAL(obj);
    while (rest != LIS_NIL) {
        print(genv, _list_car(genv, rest), stream);
        rest = _list_cdr(genv, rest);
        if (rest != LIS_NIL) stream_write_char(stream, '\n');
    }
}

lis_obj * lisp_print(lis_obj * genv, lis_obj * args) {
    if (!check_argge(genv, args, 1, LSTR(U"print"))) {
        return NULL;
    }

    lis_obj * obj = _list_nth(genv, _make_int(0), args);
    lis_obj * stream_obj = _list_nth(genv, _make_int(1), args);
    lis_stream * stream;

    if (stream_obj == LIS_GENV(genv)->symbol_nil) {
        stream = LIS_STREAM(LIS_GENV(genv)->stream_stdout);
    } else {
        stream = LIS_STREAM(stream_obj);
    }

    print(genv, obj, stream);
    stream_write_char(stream, '\n');
    stream_flush(stream);
    return LIS_GENV(genv)->symbol_nil;
}
