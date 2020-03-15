#ifndef __utf8
#define __utf8

#include <stdint.h>

typedef enum utf8_decoding_status {
    UTF8_DECODING,
    UTF8_DECODED,
    UTF8_INVALID_BYTE,
} utf8_decoding_status;

typedef struct utf8_decoding_state {
    utf8_decoding_status status;
    uint8_t bytes[4];
    int32_t cp;
    size_t n;
    size_t seq_len;
} utf8_decoding_state;

utf8_decoding_state * make_utf8_decoding_state();
void reset_utf8_decoding_state(utf8_decoding_state * state);

utf8_decoding_status utf8_decode_byte(utf8_decoding_state * state, uint8_t byte, int32_t * out);
int utf8_encode_codepoint(int32_t cp, uint8_t * out);

#endif
