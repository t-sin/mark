#ifndef __lis_list
#define __lis_list

#include "obj.h"

lis_obj * _list_length(lis_obj * genv, lis_obj * list);
lis_obj * list_length(lis_obj * genv, lis_obj * args);

lis_obj * first(lis_obj * genv, lis_obj * list);
lis_obj * second(lis_obj * genv, lis_obj * list);
lis_obj * third(lis_obj * genv, lis_obj * list);
lis_obj * fourth(lis_obj * genv, lis_obj * list);
lis_obj * fifth(lis_obj * genv, lis_obj * list);

#endif
