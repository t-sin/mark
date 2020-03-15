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
    printf("(byte: %x) ", byte);

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

    printf("cp = %x ", state->cp);
    if (state->status == UTF8_DECODED) {
        *out = state->cp;
    }
    return state->status;
}

bool utf8_encode_codepoint(lis_char * cp, lis_byte * out) {
    return false;
}

void main() {
    utf8_decoding_state * state = make_utf8_decoding_state();

    char s[256];
    fgets(s, 255, stdin);
    for (int i=0; i<255; i++) {
        lis_char cp;

        printf("%x ", s[i]);

        utf8_decoding_status status = utf8_decode_byte(state, s[i], &cp);
        if (status == UTF8_DECODED) {
            printf("%d\n", cp);
            reset_utf8_decoding_state(state);
        }

        if (s[i] == 0) break;
    }
    return;
}
