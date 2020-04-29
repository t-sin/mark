#ifndef __lis_obj
#define __lis_obj

#include <stdio.h>
#include <stdint.h>

#include "../util/byte_stream.h"
#include "../util/utf8.h"
#include "../util/table.h"

/* tags
 * 
 * least significant bit 'g' is a GC-bit
 * second least bit 'm' is not used but reserved
 * 
 * 000000mg => int
 * 000001mg => char
 * xxxx10mg => built-in types
 * 0000xxmg => multiple value
 *
 * built-in types
 * 
 * 000010mg => array
 * 000110mg => string
 * 001010mg => timestamp
 * 001110mg => symbol
 * 010010mg => cons
 * 010110mg => environment
 * 011010mg => function
 * 011110mg => closure
 * 100010mg => pakcage
 * 100110mg => stream
 * 101010mg => error
 * 101110mg => multiple value
 */

#define LIS_TAG_BASE_INT      0x00
#define LIS_TAG_BASE_CHAR     0x01
#define LIS_TAG_BASE_BUILTIN  0x02

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
#define LIS_TAG_TYPE_ERR  0x10
#define LIS_TAG_TYPE_MVAL 0x0b


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
struct lis_error;

typedef struct lis_obj {
    lis_byte tags;
    union {
        lis_int num;
        lis_char ch;
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
        struct lis_error * err;
        struct lis_obj * mval;
    } data;
} lis_obj;

#define LIS_GC_TAG(o) ((o)->tags & 0x01)
#define LIS_GC_MARKEDP(o) (LIS_GC_TAG(o) == 1)
#define LIS_GC_FLIP(o) (o)->tags ^= 0x01

#define LIS_TAG(o) ((o)->tags >> 2)
#define LIS_TAG_BASE(o) (LIS_TAG(o) & 0x03)
#define LIS_TAG_TYPE(o) (LIS_TAG(o) >> 2)

#define LIS_CH(obj) (obj)->data.ch
#define LIS_INT(obj) (obj)->data.num

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
    bool exported_p;
    lis_obj * package;
    lis_obj * fn;
} lis_symbol;

#define LIS_SYM(obj) ((obj)->data.sym)

typedef struct lis_cons {
    lis_obj * car;
    lis_obj * cdr;
} lis_cons;

#define LIS_CONS(obj) ((obj)->data.cons)

typedef struct lis_package {
    lis_obj * name;
    lis_obj ** symbols;
    lis_obj * uselist;
    size_t num;
    size_t size;
} lis_package;

#define LIS_PKG(obj) ((obj)->data.pkg)

typedef enum env_type {
    LIS_ENV_GLOBAL, LIS_ENV_DYNAMIC, LIS_ENV_LEXICAL,
} env_type;

typedef struct lis_global_env {
    _table * package_table;
    lis_obj * current_package;
    lis_obj * keyword_package;
    lis_obj * lis_package;

    lis_obj * error;

    lis_obj * symbol_t;
    lis_obj * symbol_nil;
    lis_obj * symbol_optional;
    lis_obj * symbol_key;
    lis_obj * symbol_rest;

    lis_obj * stream_stdin;
    lis_obj * stream_stdout;
    lis_obj * stream_stderr;
} lis_global_env;

#define LIS_NIL LIS_GENV(genv)->symbol_nil

typedef struct lis_dynamic_env {
} lis_dynamic_env;

typedef struct lis_lexical_env {
    _table * var;
    _table * fn;
} lis_lexical_env;

typedef struct lis_env {
    env_type type;
    lis_obj * parent;
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
    LIS_FUNC_RAW, LIS_FUNC_NORMAL,LIS_FUNC_SPECIAL_FORM, LIS_FUNC_MACRO_RAW, LIS_FUNC_MACRO
} lis_function_type;

typedef struct lis_arg {
    lis_obj * name;
    lis_obj * default_value;
    struct lis_arg * next;
} lis_arg;

typedef struct lis_lambdalist {
    lis_obj * required;
    lis_arg * optional;
    _table * keyword;
    lis_obj * rest;
} lis_lambdalist;

typedef struct lis_function {
    lis_function_type type;
    lis_lambdalist * lambdalist;
    union {
        lis_obj * lisp;
        lis_obj * (* raw)(lis_obj *, lis_obj *);
        lis_obj * (* sf)(lis_obj *, lis_obj *, lis_obj *);
    } body;
} lis_function;

#define LIS_FN(obj) ((obj)->data.fn)

typedef struct lis_closure {
    lis_obj * fn;
    lis_obj * env;
} lis_closure;

#define LIS_CLS(obj) ((obj)->data.cls)

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

typedef struct lis_error {
    lis_obj * message;
} lis_error;

#define LIS_ERR(obj) ((obj)->data.err)

#define LIS_MVAL(obj) ((obj)->data.mval)

lis_obj * _make_int(lis_int n);
lis_obj * _make_char(lis_int ch);
lis_obj * _make_array();
lis_obj * _make_string();
lis_obj * _make_symbol(lis_obj * name);
lis_obj * _make_cons();
lis_obj * _make_package(lis_obj * name);
lis_obj * _make_env();
lis_obj * _make_lexical_env();
lis_obj * _make_raw_function();
lis_obj * _make_lisp_function(lis_lambdalist * lambdalist, lis_obj * body);
lis_obj * _make_lisp_closure(lis_lambdalist * lambdalist, lis_obj * body, lis_obj * lenv);
lis_obj * _make_lis_stream(lis_stream * s);
lis_obj * _make_error(lis_obj * msg);
lis_obj * _make_multiple_value();

#define INT(integer) _make_int(integer)

#endif
