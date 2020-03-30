#ifndef __lis_list
#define __lis_list

#include "obj.h"

lis_obj * _list_length(lis_obj * genv, lis_obj * list);
lis_obj * list_length(lis_obj * genv, lis_obj * args);

lis_obj * _list_nth(lis_obj * genv, lis_obj * n, lis_obj * list);
lis_obj * list_nth(lis_obj * genv, lis_obj * args);

lis_obj * list_first(lis_obj * genv, lis_obj * args);
lis_obj * list_second(lis_obj * genv, lis_obj * args);
lis_obj * list_third(lis_obj * genv, lis_obj * args);
lis_obj * list_fourth(lis_obj * genv, lis_obj * args);
lis_obj * list_fifth(lis_obj * genv, lis_obj * args);

#endif
