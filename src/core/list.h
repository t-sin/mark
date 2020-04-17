#ifndef __lis_list
#define __lis_list

#include "obj.h"

bool _list_atom(lis_obj * genv, lis_obj * obj);
bool _list_consp(lis_obj * genv, lis_obj * cons);
bool _list_listp(lis_obj * genv, lis_obj * cons);

lis_obj * lisp_atom(lis_obj * genv, lis_obj * args);
lis_obj * lisp_consp(lis_obj * genv, lis_obj * args);
lis_obj * lisp_listp(lis_obj * genv, lis_obj * args);

lis_obj * _list_cons(lis_obj * genv, lis_obj * a, lis_obj * b);
lis_obj * _list_car(lis_obj * genv, lis_obj * cons);
lis_obj * _list_cdr(lis_obj * genv, lis_obj * cons);
lis_obj * _list_rplaca(lis_obj * genv, lis_obj * cons, lis_obj * obj);
lis_obj * _list_rplacd(lis_obj * genv, lis_obj * cons, lis_obj * obj);

lis_obj * lisp_cons(lis_obj * genv, lis_obj * args);
lis_obj * lisp_car(lis_obj * genv, lis_obj * args);
lis_obj * lisp_cdr(lis_obj * genv, lis_obj * args);
lis_obj * lisp_rplaca(lis_obj * genv, lis_obj * args);
lis_obj * lisp_rplacd(lis_obj * genv, lis_obj * args);

lis_obj * _list_length(lis_obj * genv, lis_obj * list);
lis_obj * _list_nth(lis_obj * genv, lis_obj * n, lis_obj * list);
lis_obj * lisp_nth(lis_obj * genv, lis_obj * args);

lis_obj * lisp_first(lis_obj * genv, lis_obj * args);
lis_obj * lisp_second(lis_obj * genv, lis_obj * args);
lis_obj * lisp_third(lis_obj * genv, lis_obj * args);
lis_obj * lisp_fourth(lis_obj * genv, lis_obj * args);
lis_obj * lisp_fifth(lis_obj * genv, lis_obj * args);

lis_obj * lisp_list(lis_obj * genv, lis_obj * args);

lis_obj * _list_reverse(lis_obj * genv, lis_obj * list);

#endif
