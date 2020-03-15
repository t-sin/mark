#include<stdio.h>

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "utf8.h"

utf8_decoding_state * make_utf8_decoding_state() {
    utf8_decoding_state * state;
    state = (utf8_decoding_state *)malloc(sizeof(utf8_decoding_state));
    memset(state, 0, sizeof(utf8_decoding_state));
    return state;
}

void reset_utf8_decoding_state(utf8_decoding_state * state) {
    memset(state, 0, sizeof(utf8_decoding_state));
}

void decode_first_byte(utf8_decoding_state * state) {
    lis_byte byte = state->bytes[state->n];

    if (byte >> 5 == 0b110) {          // 2-bytes sequence
        state->cp = byte & 0b11111;
        state->status = UTF8_DECODING;
        state->seq_len = 2;
    } else if (byte >> 4 == 0b1110) {  // 3-bytes sequence
        state->cp = byte & 0b1111;
        state->status = UTF8_DECODING;
        state->seq_len = 3;
    } else if (byte >> 3 == 0b11110) { // 4-bytes sequence
        state->cp = byte & 0b111;
        state->status = UTF8_DECODING;
        state->seq_len = 4;
    } else if (byte < 0x7f) {          // ascii
        state->cp = byte;
        state->status = UTF8_DECODED;
        state->seq_len = 1;
        return;
    } else {
        state->status = UTF8_INVALID_BYTE;
        return;
    }

    state->n++;
}

void decode_successor_byte(utf8_decoding_state * state) {
    lis_byte byte = state->bytes[state->n];

    if (byte >> 6 == 0b10) {
        state->cp = state->cp << 6 | byte & 0x3f;
        if (state->n + 1 == state->seq_len) {
            state->status = UTF8_DECODED;
        } else {
            state->n++;
        }
    } else {
        state->status = UTF8_INVALID_BYTE;
    }
}

utf8_decoding_status utf8_decode_byte(utf8_decoding_state * state, lis_byte byte, lis_char * out) {
    assert(state->n >= 0 && state->n < 4);

    state->bytes[state->n] = byte;
    switch (state->n) {
    case 0:
        decode_first_byte(state);
        break;
    default:
        decode_successor_byte(state);
        break;
    }

    if (state->status == UTF8_DECODED) {
        *out = state->cp;
    }
    return state->status;
}

int utf8_encode_codepoint(lis_char cp, lis_byte * out) {
    if (cp < 0 || cp > 0x10FFFF) {
        return 0;
    }

    if (cp <= 0x007F) {
        out[0] = (lis_byte)cp;
        return 1;
    } else if (cp >= 0x0080 && cp < 0x07FF) {
        out[0] = cp >> 6 | 0b11000000;
        out[1] = cp & 0b00111111 | 0b10000000;
        return 2;
    } else if (cp >=0x0800 && cp < 0xFFFF) {
        out[0] = cp >> (6 + 6) | 0b11100000;
        out[1] = cp >> 6 & 0b00111111 | 0b10000000;
        out[2] = cp & 0b00111111 | 0b10000000;
        return 3;
    } else if (cp >= 0x010000 && cp < 0x10FFFF) {
        out[0] = cp >> (6 + 6 + 6) | 0b11110000;
        out[1] = cp >> (6 + 6) & 0b00111111 | 0b10000000;
        out[2] = cp >> 6 & 0b00111111 | 0b10000000;
        out[3] = cp & 0b00111111 | 0b10000000;
        return 4;
    }
}

void main() {
    utf8_decoding_state * state = make_utf8_decoding_state();

    char s[256];
    lis_byte c[4];
    lis_char cp;

    fgets(s, 255, stdin);
    for (int i=0; i<255; i++) {

        utf8_decoding_status status = utf8_decode_byte(state, s[i], &cp);
        if (status == UTF8_DECODED) {
            printf("cp=%x ", cp);
            reset_utf8_decoding_state(state);

            int len = utf8_encode_codepoint(cp, c);
            for (int j=0; j<len; j++) {
                printf("%c", c[j]);
            }
            printf("\n");
        }

        if (s[i] == 0) break;
    }
    return;
}