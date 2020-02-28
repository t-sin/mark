#include <stdint.h>

/* tags
 * 
 * least significant bit 'g' is a GC-bit
 * it is imeddiate value if the second least bit is 1
 * 
 * 0000000g => pointer
 * 0000001g => int
 * 0000011g => char
 * xxx0101g => special values
 * xxxx100g => built-in types
 * 0000110g => other types
 * special values:
 * 
 * 0000101g => nil
 * 0010101g => t
 *
 * heap-allocated data
 * 
 * 0000110g => array
 * 0001110g => timestamp
 * 0010110g => symbol
 * 0011110g => cons
 * 0100110g => environment
 * 0101110g=> function
 * 0110110g => closure
 * 0111110g => pakcage
 * 1000110g => other classes?
 */

typedef uint8_t lis_byte;
typedef int32_t lis_int;
typedef int32_t lis_char;
typedef int32_t lis_ptr;

typedef struct {
    lis_byte tags;
} lis_cell;


#define LIS_GC_TAG(cell) ((cell).tags & 0x01)
#define LIS_GC_MARKEDP(cell) (LIS_GC_TAG(cell) == 1)
#define LIS_GC_FLIP(cell) (cell).tags ^= 0x01

#define LIS_TAG(cell) ((cell).tags >> 1)
#define LIS_TAG1(o) (LIS_TAG(o) & 0x01)
#define LIS_TAG2(o) (LIS_TAG(o) & 0x03)
#define LIS_TAG3(o) (LIS_TAG(o) & 0x07)
#define LIS_TAG7(o) (LIS_TAG(o) & 0x7f)

typedef struct {
    lis_cell * arr;
} lis_array;

typedef struct {
    lis_int year;
    lis_byte month;
    lis_byte day;
    lis_byte hour;
    lis_byte minute;
    lis_byte second;
    lis_byte day_of_week;
} lis_tstamp; 

typedef struct {
    lis_cell * name;
} lis_symbol;

typedef struct {
    lis_cell * car;
    lis_cell * cdr;
} lis_cons;

typedef struct env{
    struct env * parent;
    lis_cell * syms;
    lis_cell * vals;
} lis_env;

typedef struct {
} lis_function;

typedef struct {
    lis_function fn;
    lis_env env;
} lis_closure;

typedef struct {
} lis_package;
