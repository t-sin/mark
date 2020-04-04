#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "../src/util/utf8.h"

void test_encode_and_decode() {
    char expect[] = u8"abc ABC αβγλω あいうえお 😀😁😂";
    int32_t codepoints[256];
    char actual[256];

    utf8_decoding_state * state = make_utf8_decoding_state();
    utf8_decoding_status status;

    // decoding
    int i = 0;
    int cplen = 0;
    for (i=0; i<sizeof(expect); i++) {
        int32_t cp;
        status = utf8_decode_byte(state, expect[i], &cp);
        if (status == UTF8_DECODED) {
            codepoints[cplen++] = cp;
            reset_utf8_decoding_state(state);
        }
    }

    assert(status == UTF8_DECODED);

    // encoding
    int j = 0;
    uint8_t bytes[4];
    for (i=0; i<cplen; i++) {
        int len = utf8_encode_codepoint(codepoints[i], bytes);
        assert(len > 0);
        for (int k=0; k<len; k++) {
            actual[j+k] = bytes[k];
        }
        j += len;
    }

    assert(strlen(expect) == strlen(actual));
    assert(strcmp(expect, actual) == 0);
}

int main() {
    test_encode_and_decode();
    return 0;
}
