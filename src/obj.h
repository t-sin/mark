#ifndef __lis_obj
#define __lis_obj

#include <stdint.h>

#include "byte_stream.h"
#include "utf8.h"

/* tags
 * 
 * least significant bit 'g' is a GC-bit
 * it is imeddiate value if the second least bit is 1
 * 
 * 0000000g => pointer
 * 0000001g => int
 * 0000011g => char
 * xxxx100g => special values
 * xxxx101g => built-in types
 * 0000110g => other types
 *
 * special values:
 * 
 * 1111100g => nil
 * 1110100g => t
 *
 * built-in types
 * 
 * 0000101g => array
 * 0001101g => string
 * 0010101g => timestamp
 * 0011101g => symbol
 * 0100101g => cons
 * 0101101g => environment
 * 0110101g => function
 * 0111101g => closure
 * 1000101g => pakcage
 * 1001101g => stream
 *
 * other types
 * 1000110g => other classes?
 */

#define LIS_TAG3_PTR      0x00
#define LIS_TAG3_INT      0x01
#define LIS_TAG3_CHAR     0x03
#define LIS_TAG3_SPECIAL  0x05
#define LIS_TAG3_BUILTIN  0x04
#define LIS_TAG3_OTHER    0x06

#define LIS_TAG_TYPE_NIL ~0x00
#define LIS_TAG_TYPE_T   ~0x01

#define LIS_TAG_TYPE_ARY  0x00
#define LIS_TAG_TYPE_STR  0x01
#define LIS_TAG_TYPE_TS   0x02
#define LIS_TAG_TYPE_SYM  0x03
#define LIS_TAG_TYPE_CONS 0x04
#define LIS_TAG_TYPE_ENV  0x05
#define LIS_TAG_TYPE_FN   0x06
#define LIS_TAG_TYPE_CLS  0x07
#define LIS_TAG_TYPE_PKG  0x08
#define LIS_TAG_TYPE_STRM 0x09

typedef uint8_t lis_byte;
typedef int32_t lis_int;
typedef int32_t lis_char;
typedef int32_t lis_ptr;

struct lis_array;
struct lis_string;
struct lis_tstamp;
struct lis_symbol;
struct lis_cons;
struct lis_env;
struct lis_function;
struct lis_closure;
struct lis_package;

typedef struct {
 lis_byte tags;
 union {
  lis_int num;
  lis_char ch;
  lis_ptr ptr;
  struct lis_array * array;
  struct lis_string * str;
  struct lis_tstamp * ts;
  struct lis_symbol * sym;
  struct lis_cons * cons;
  struct lis_env * env;
  struct lis_function * fn;
  struct lis_closure * cls;
  struct lis_package * pkg;
  struct lis_stream * stream;
 } data;
} lis_obj;

#define LIS_GC_TAG(o) ((o)->tags & 0x01)
#define LIS_GC_MARKEDP(o) (LIS_GC_TAG(o) == 1)
#define LIS_GC_FLIP(o) (o)->tags ^= 0x01

#define LIS_TAG(o) ((o)->tags >> 1)
#define LIS_TAG1(o) (LIS_TAG(o) & 0x01)
#define LIS_TAG2(o) (LIS_TAG(o) & 0x03)
#define LIS_TAG3(o) (LIS_TAG(o) & 0x07)
#define LIS_TAG7(o) (LIS_TAG(o) & 0x7f)

#define LIS_TAG_TYPE(o) (LIS_TAG(o) >> 3)

typedef struct lis_array {
 lis_obj * body;
 lis_int size;
} lis_array;

typedef struct lis_string {
 lis_char * body;
 lis_int size;
} lis_string;

typedef struct lis_tstamp {
 lis_int year;
 lis_byte month;
 lis_byte day;
 lis_byte hour;
 lis_byte minute;
 lis_byte second;
 lis_byte day_of_week;
} lis_tstamp; 

typedef struct lis_symbol {
 lis_obj * name;
 lis_obj * value;
 struct lis_package * package;
// lis_function * fn;
} lis_symbol;

typedef struct lis_cons {
 lis_obj * car;
 lis_obj * cdr;
} lis_cons;

typedef struct lis_env {
 struct env * parent;
 lis_obj * syms;
 lis_obj * vals;
} lis_env;

typedef struct lis_function {
} lis_function;

typedef struct lis_closure {
 lis_function fn;
 lis_env env;
} lis_closure;

typedef struct lis_package {
} lis_package;

typedef enum lis_stream_direction_type {
    LIS_STREAM_IN, LIS_STREAM_OUTPUT, LIS_STREAM_INOUT,
} lis_stream_direction_type;

typedef enum lis_stream_element_type {
    LIS_STREAM_TEXT, LIS_STREAM_BINARY,
} lis_stream_element_type;

struct utf8_decoding_state;

typedef struct lis_stream {
    _stream stream;
    struct utf8_decoding_state * decode_state;
    lis_stream_direction_type direction;
    lis_stream_element_type element_type;
} lis_stream;

#endif