#include <stdio.h>

#include "obj.h"
#include "print.h"
#include "stream.h"
#include "runtime.h"

void print_string(lis_stream * stream, lis_obj * str) {
    for (size_t i=0; i < str->data.str->size; i++) {
        stream_write_char(stream, str->data.str->body[i]);
    }
}

void print_cons(lis_stream * stream, lis_runtime * runtime, lis_obj * car, lis_obj * cdr) {
    print(stream, runtime, car);
    if (LIS_TAG3(cdr) == LIS_TAG3_BUILTIN && LIS_TAG_TYPE(cdr) == LIS_TAG_TYPE_CONS) {
        stream_write_char(stream, ' ');
        print_cons(stream, runtime, cdr->data.cons->car, cdr->data.cons->cdr);
    } else if (cdr == runtime->symbol_nil) {
        ;
    } else {
        stream_write_char(stream, ' ');
        stream_write_char(stream, '.');
        stream_write_char(stream, ' ');
        print(stream, runtime, cdr);
    }
}

#define BUF_SIZE 256
void print(lis_stream * stream, lis_runtime * runtime, lis_obj * obj) {
    if (obj == NULL) {
        printf("NULL!!\n");
        return;
    }

    char buf[BUF_SIZE];

    if (LIS_TAG3(obj) == LIS_TAG3_INT) {
        snprintf(buf, BUF_SIZE, "%d", obj->data.num);
        for (int i=0; i<BUF_SIZE; i++) {
            if (buf[i] == '\0') return;
            stream_write_char(stream, buf[i]);
        }

    } else if (LIS_TAG3(obj) == LIS_TAG3_CHAR) {
        stream_write_char(stream, '#');
        stream_write_char(stream, '\\');
        stream_write_char(stream, obj->data.ch);

    } else if (LIS_TAG3(obj) == LIS_TAG3_BUILTIN) {
        // built-in types
        switch (LIS_TAG_TYPE(obj)) {
        case LIS_TAG_TYPE_ARY:
            stream_write_char(stream, '#');
            stream_write_char(stream, '(');
            for (size_t i=0; i < obj->data.array->size; i++) {
                print(stream, runtime, &obj->data.array->body[i]);
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
            print_string(stream, obj->data.sym->name);
            break;

        case LIS_TAG_TYPE_CONS:
            stream_write_char(stream, '(');
            print_cons(stream, runtime, obj->data.cons->car, obj->data.cons->cdr);
            stream_write_char(stream, ')');
            break;

        case LIS_TAG_TYPE_ENV:
        case LIS_TAG_TYPE_FN:
        case LIS_TAG_TYPE_CLS:
        case LIS_TAG_TYPE_PKG:
            break;
        }
    } else {
        printf("unknown object: %x\n", obj->tags);
    }
}
