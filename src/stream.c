#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stream.h"

lis_stream make_stream(stream_direction_type dir, stream_element_type etype, size_t buf_size) {
    lis_stream stream;
    stream.direction = dir;
    stream.element_type = etype;
    stream.buffer_size = buf_size;
    stream.buffer_array_size = 1;

    uint8_t * buffer = (uint8_t *)malloc(sizeof(uint8_t *) * 1);
    memset(buffer, NULL, sizeof(uint8_t) * 1);
    uint8_t * buffer1 = (uint8_t *)malloc(sizeof(uint8_t) * buf_size);
    memset(buffer1, 0, sizeof(uint8_t) * buf_size);
    buffer[0] = buffer1;
    stream.buffer = buffer;

    return stream;
}
