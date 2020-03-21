#include <stdlib.h>

#include "utf8.h"
#include "obj.h"
#include "lstring.h"

lis_string * to_lstring_from_cstr(const char * cstr, size_t cstr_size) {
    lis_string * str = (lis_string *)malloc(sizeof(lis_string));
    size_t size = 0;
    lis_char ch;
    utf8_decoding_state * state = make_utf8_decoding_state();
    lis_char * body = (lis_char *)malloc(sizeof(lis_char) * cstr_size);
    
    for (int i=0; i<cstr_size; i++) {
        utf8_decoding_status status = utf8_decode_byte(state, cstr[i], &ch);
        if (status == UTF8_DECODED) {
            body[size++] = ch;
        } else if (status == UTF8_DECODING) {
            continue;
        } else {
            return NULL;
        }
    }

    str->body = body;
    str->size = size - 1;

    return str;
}