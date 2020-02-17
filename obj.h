#include<stdint.h>

typedef intptr_t LisWord;
typedef uint8_t LisByte;

typedef struct lis_obj {
  LisByte tag;
} LisObj;

/* tags
 * 
 * least significant bit 'g' is a GC-bit
 * 
 * 0000000g => pointer
 * 0000001g => int
 * 0000011g => char
 * xxx0101g => special values
 * xxx0111g => any heap-allocated data
 * 
 * special values:
 * 
 * 0000101g => nil
 * 0010101g => t
 *
 * heap-allocated data
 * 
 * 0010111g => symbol
 * 0100111g => cons
 * 0110111g => function
 * 1000111g => closure
 * 1010111g => pakcage
 * 1100111g => timestamp
 * 1110111g => other classes?
 * 
 */

#define LIS_OBJ(obj) ((LisObj)(obj))
#define LIS_WORD(obj) ((LisWord)(obj))

#define LIS_GC_TAG(obj) (LIS_OBJ(obj).tag & 0x01)
#define LIS_GC_MARKEDP(obj) (LIS_GC_TAG(obj) == 1)
#define LIS_GC_FLIP(obj) (obj.tag ^= 0x01)

#define LIS_TAG1(o) ((LIS_OBJ(obj).tag & (0x01 << 1)) >> 1)
#define LIS_TAG2(o) ((LIS_OBJ(obj).tag & (0x03 << 1)) >> 1)
#define LIS_TAG3(o) ((LIS_OBJ(obj).tag & (0x07 << 1)) >> 1)
#define LIS_TAG7(o) ((LIS_OBJ(obj).tag & (0x7f << 1)) >> 1)