#include <stdint.h>

/* tags
 * 
 * least significant bit 'g' is a GC-bit
 * 
 * 0000000g => pointer
 * 0000001g => int
 * 0000011g => char
 * xxx0101g => special values
 * xxxx111g => built-in types
 * 0000100g => other types
 * special values:
 * 
 * 0000101g => nil
 * 0010101g => t
 *
 * heap-allocated data
 * 
 * 0001111g => symbol
 * 0010111g => cons
 * 0011111g => function
 * 0100111g => closure
 * 0101111g => pakcage
 * 0110111g => timestamp
 * 0111111g => other classes?
 *
 */

typedef uint8_t lis_byte;
typedef int32_t lis_int;
typedef int32_t lis_char;
typedef uint32_t lis_tname;

typedef struct {
} lis_env;

typedef struct {
} lis_func;

typedef struct {
 lis_func f;
 lis_env env;
} lis_clos;

typedef union {
 lis_byte tags;
 lis_char ch;
 lis_int num;
 lis_tname type;
} lis_cell;

#define LIS_GC_TAG(cell) (cell.tags & 0x01)
#define LIS_GC_MARKEDP(cell) (LIS_GC_TAG(cell) == 1)
#define LIS_GC_FLIP(cell) cell.tags ^= 0x01

#define LIS_TAG(cell) (cell.tags >> 1)
#define LIS_TAG1(o) (LIS_TAG(o) & 0x01)
#define LIS_TAG2(o) (LIS_TAG(o) & 0x03)
#define LIS_TAG3(o) (LIS_TAG(o) & 0x07)
#define LIS_TAG7(o) (LIS_TAG(o) & 0x7f)
