#ifndef __lis_obj
#define __lis_obj

#include <stdio.h>
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
 * xxxx100g => special values (NOT USED)
 * xxxx101g => built-in types
 * 0000110g => other types
 *
 * // special values (NOT USED):
 * // 
 * // 1111100g => nil
 * // 1110100g => t
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
#define LIS_TAG3_SPECIAL  0x05  // NOT USED
#define LIS_TAG3_BUILTIN  0x04
#define LIS_TAG3_OTHER    0x06

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

#define LIS_STR(obj) ((obj)->data.str)

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
 bool constant_p;
 bool dynamic_p;
 lis_obj * package;
 lis_obj * fn;
} lis_symbol;

typedef struct lis_cons {
 lis_obj * car;
 lis_obj * cdr;
} lis_cons;

#define LIS_CONS(obj) ((obj)->data.cons)

typedef struct lis_package {
 lis_obj * name;
 lis_obj ** symbols;
 size_t num;
 size_t size;
} lis_package;

typedef struct lis_package_table {
    lis_package * packages;
    size_t size;
} lis_package_table;

typedef enum env_type {
    LIS_ENV_GLOBAL, LIS_ENV_DYNAMIC, LIS_ENV_LEXICAL,
} env_type;

typedef struct lis_global_env {
    lis_package_table * package_table;
    lis_obj * current_package;
    lis_obj * keyword_package;
    lis_obj * symbol_t;
    lis_obj * symbol_nil;
    lis_obj * stream_stdin;
    lis_obj * stream_stdout;
    lis_obj * stream_stderr;
} lis_global_env;

typedef struct lis_dynamic_env {
} lis_dynamic_env;

typedef struct lis_lexical_env {
} lis_lexical_env;

typedef struct lis_env {
 env_type type;
 struct lis_env * parent;
 union {
  lis_global_env * global;
  lis_dynamic_env * dynamic;
  lis_lexical_env * lexical;
 } env;
} lis_env;

#define LIS_ENV(obj) ((obj)->data.env)
#define LIS_GENV(obj) ((obj)->data.env->env.global)
#define LIS_DENV(obj) ((obj)->data.env->env.dynamic)
#define LIS_LENV(obj) ((obj)->data.env->env.lexical)

typedef enum lis_function_type {
 LIS_FUNC_NORMAL, LIS_FUNC_SPECIAL_FORM, LIS_FUNC_MACRO
} lis_function_type;

typedef struct lis_function {
 lis_function_type type;
 lis_obj * lambdalist;
 lis_obj * body;
 lis_obj * (* raw_body)(lis_obj *, lis_obj *);
} lis_function;

#define LIS_FN(obj) ((obj)->data.fn)

typedef struct lis_closure {
 lis_function fn;
 lis_env env;
} lis_closure;

typedef enum lis_stream_direction_type {
    LIS_STREAM_IN, LIS_STREAM_OUT, LIS_STREAM_INOUT,
} lis_stream_direction_type;

typedef enum lis_stream_element_type {
    LIS_STREAM_TEXT, LIS_STREAM_BINARY,
} lis_stream_element_type;

struct utf8_decoding_state;

typedef struct lis_stream {
    lis_obj * name;
    _stream * stream;
    FILE * fin;
    FILE * fout;
    struct utf8_decoding_state * decode_state;
    lis_stream_direction_type direction;
    lis_stream_element_type element_type;
} lis_stream;

#define LIS_STREAM(obj) ((obj->data.stream))


lis_obj * _make_int(lis_int n);
lis_obj * _make_char(lis_int ch);
lis_obj * _make_array();
lis_obj * _make_string();
lis_obj * _make_symbol(lis_obj * name);
lis_obj * _make_cons();
lis_obj * _make_package(lis_obj * name);
lis_obj * _make_env();
lis_obj * _make_raw_function();
lis_obj * _make_lis_stream(lis_stream * s);

#endif
