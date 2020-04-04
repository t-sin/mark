#ifndef __lis_read
#define __lis_read

#include "obj.h"
#include "stream.h"

int skip_whitespaces(lis_stream * stream);
lis_obj * read(lis_stream * stream, lis_obj * genv);

#endif
