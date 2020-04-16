#ifndef __lis_stream
#define __lis_stream

#include "obj.h"

lis_obj * lisp_listen(lis_obj * genv, lis_obj * args);

lis_obj * lisp_force_output(lis_obj * genv, lis_obj * args);

lis_obj * lisp_peek_byte(lis_obj * genv, lis_obj * args);
lis_obj * lisp_read_byte(lis_obj * genv, lis_obj * args);
lis_obj * lisp_write_byte(lis_obj * genv, lis_obj * args);

lis_obj * lisp_peek_char(lis_obj * genv, lis_obj * args);
lis_obj * lisp_read_char(lis_obj * genv, lis_obj * args);
lis_obj * lisp_unread_char(lis_obj * genv, lis_obj * args);
lis_obj * lisp_write_char(lis_obj * genv, lis_obj * args);

lis_obj * lisp_write_string(lis_obj * genv, lis_obj * args);

#endif
