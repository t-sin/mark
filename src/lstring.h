#ifndef __lis_string
#define __lis_string

#include "obj.h"

lis_obj * to_lstring_from_cstr(const char * cstr, size_t cstr_size);

bool string_equal(lis_string * s1, lis_string * s2);

#endif
