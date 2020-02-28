#include <stdint.h>

/* tags
 * 
 * least significant bit 'g' is a GC-bit
 * it is imeddiate value if the second least bit is 1
 * 
 * 0000000g => pointer
 * 0000001g => int
 * 0000011g => char
 * xxxx101g => special values
 * xxxx100g => built-in types
 * 0000101g => other types
 *
 * special values:
 * 
 * 0000101g => nil
 * 0010101g => t
 *
 * built-in types
 * 
 * 0000100g => array
 * 0001100g => timestamp
 * 0010100g => symbol
 * 0011100g => cons
 * 0100100g => environment
 * 0101100g => function
 * 0110100g => closure
 * 0111100g => pakcage
 *
 * other types
 * 1000101g => other classes?
 */

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
