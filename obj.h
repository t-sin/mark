#include <stdint.h>

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
 * 0001101g => timestamp
 * 0010101g => symbol
 * 0011101g => cons
 * 0100101g => environment
 * 0101101g => function
 * 0110101g => closure
 * 0111101g => pakcage
 * 1000101g => stream
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
#define LIS_TAG_TYPE_TS   0x01
#define LIS_TAG_TYPE_SYM  0x02
#define LIS_TAG_TYPE_CONS 0x03
#define LIS_TAG_TYPE_ENV  0x04
#define LIS_TAG_TYPE_FN   0x05
#define LIS_TAG_TYPE_CLS  0x06
#define LIS_TAG_TYPE_PKG  0x07
#define LIS_TAG_TYPE_STRM  0x08

typedef uint8_t lis_byte;
typedef int32_t lis_int;
typedef int32_t lis_char;
typedef int32_t lis_ptr;

struct lis_array;
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
  struct lis_tstamp * ts;
  struct lis_symbol * sym;
  struct lis_cons * cons;
  struct lis_env * env;
  struct lis_function * fn;
  struct lis_closure * cls;
  struct lis_package * pkg;
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
 lis_byte tags;
 lis_obj * arr;
 lis_int size;
} lis_array;

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
