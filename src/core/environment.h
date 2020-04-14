#ifndef __lis_environment
#define __lis_environment

#include "obj.h"

lis_obj * make_lexical_env(lis_obj * parent);
lis_obj * get_lexical_value(lis_obj * lenv, lis_obj * name);
void set_lexical_value(lis_obj * lenv, lis_obj * name, lis_obj * val);
void find_and_set_lexical_value(lis_obj * lenv, lis_obj * name, lis_obj * val);

#endif
