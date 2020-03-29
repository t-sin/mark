#ifndef __lis_string
#define __lis_string

#include <uchar.h>

#include "obj.h"

// requirements
// - char32_t __lstr;
// - lis_obj * __str;
#define LSTR(ustr) u2lstring(ustr, sizeof(ustr) / sizeof(ustr[0]))

lis_obj * u2lstring(const char32_t * ustr, size_t ustr_size);

bool string_equal(lis_string * s1, lis_string * s2);

#endif
