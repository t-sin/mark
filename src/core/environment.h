#ifndef __lis_environment
#define __lis_environment

#include <setjmp.h>

#include "obj.h"

bool find_from_lexical_env(lis_obj * lenv, lis_obj * name);
lis_obj * get_lexical_value(lis_obj * lenv, lis_obj * name);

void set_lexical_value(lis_obj * lenv, lis_obj * name, lis_obj * val);
void find_and_set_lexical_value(lis_obj * lenv, lis_obj * name, lis_obj * val);

jmp_buf * get_block(lis_obj * lenv, lis_obj * name);

#endif
