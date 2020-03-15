#ifndef __utf8
#define __utf8

#include "obj.h"

typedef enum utf8_decoding_status {
    UTF8_DECODING,
    UTF8_DECODED,
    UTF8_INVALID_BYTE,
} utf8_decoding_status;

typedef struct utf8_decoding_state {
    utf8_decoding_status status;
    lis_byte bytes[4];
    lis_char cp;
    size_t n;
    size_t seq_len;
} utf8_decoding_state;

utf8_decoding_state * make_utf8_decoding_state();
void reset_utf8_decoding_state(utf8_decoding_state * state);

utf8_decoding_status utf8_decode_byte(utf8_decoding_state * state, lis_byte byte, lis_char * out);
bool utf8_encode_codepoint(lis_char * cp, lis_byte * out);

#endif
