#include <stdbool.h>
#include <stdint.h>

typedef enum stream_direction_type {
    LIS_STREAM_IN, LIS_STREAM_OUTPUT, LIS_STREAM_INOUT,
} stream_direction_type;

typedef enum stream_element_type {
    LIS_STREAM_CHAR, LIS_STREAM_BINARY,
} stream_element_type;

typedef struct stream_pos {
    size_t array_idx;
    size_t elem_idx;
} stream_pos;

typedef struct lis_stream {
    stream_direction_type direction;
    stream_element_type element_type;
    uint8_t ** buffer;
    size_t buffer_size;
    size_t buffer_array_size;
    stream_pos head;
    stream_pos tail;
    bool readable;
} lis_stream;

lis_stream make_stream(stream_direction_type dir, stream_element_type etype, size_t buf_size);

bool stream_listen_p(lis_stream * stream);
uint8_t stream_read_elem(lis_stream * stream);
bool stream_write_elem(lis_stream * stream, uint8_t elem);
bool stream_unread_elem(lis_stream * stream, uint8_t elem);
void stream_close(lis_stream * stream);
