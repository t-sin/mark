#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "arithmetic.h"
#include "print.h"
#include "eval.h"

lis_obj * _list_length(lis_obj * genv, lis_obj * list);
lis_obj * list_length(lis_obj * genv, lis_obj * args);

lis_obj * _list_nth(lis_obj * genv, lis_obj * n, lis_obj * list);
lis_obj * list_nth(lis_obj * genv, lis_obj * args);

bool _list_atom(lis_obj * genv, lis_obj * obj) {
    if (LIS_TAG3(obj) == LIS_TAG3_BUILTIN &&
        LIS_TAG_TYPE(obj) == LIS_TAG_TYPE_CONS) {
        return false;
    } else {
        return true;
    }
}

lis_obj * list_atom(lis_obj * genv, lis_obj * args) {
    lis_obj * arg = _list_nth(genv, _make_int(0), args);
    if (_list_atom(genv, arg)) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

bool _list_consp(lis_obj * genv, lis_obj * cons) {
    if (LIS_TAG3(cons) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(cons) != LIS_TAG_TYPE_CONS) {
        return false;
    } else {
        return true;
    }
}

lis_obj * list_consp(lis_obj * genv, lis_obj * args) {
    lis_obj * arg = _list_nth(genv, _make_int(0), args);
    if (_list_consp(genv, arg)) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

bool _list_listp(lis_obj * genv, lis_obj * obj) {
    if (obj == LIS_GENV(genv)->symbol_nil) {
        return true;
    }

    if (!_list_consp(genv, obj)) {
        return false;
    }

    return _list_listp(genv, LIS_CONS(obj)->cdr);
}

lis_obj * list_listp(lis_obj * genv, lis_obj * args) {
    lis_obj * arg = _list_nth(genv, _make_int(0), args);
    if(_list_listp(genv, arg)) {
        return LIS_GENV(genv)->symbol_t;
    } else {
        return LIS_GENV(genv)->symbol_nil;
    }
}

lis_obj * _list_length(lis_obj * genv, lis_obj * list) {
    if (list == LIS_GENV(genv)->symbol_nil) {
        return _make_int(0);
    } else if (LIS_TAG3(list) != LIS_TAG3_BUILTIN ||
               LIS_TAG_TYPE(list) != LIS_TAG_TYPE_CONS) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, list, buffer);
        stream_write_string(buffer, LSTR(U" is not a list."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    } else {
        return _int_add(genv, _list_length(genv, LIS_CONS(list)->cdr), _make_int(1));
    }
}

lis_obj * list_length(lis_obj * genv, lis_obj * args) {
    if (_list_length(genv, args)->data.num == 1) {
        return _list_length(genv, LIS_CONS(args)->car);
    } else {
        LIS_GENV(genv)->error = _make_error(LSTR(U"wrong number of args for `length`."));
        return NULL;
    }
}

lis_obj * _list_nth(lis_obj * genv, lis_obj * n, lis_obj * list) {
    if (LIS_TAG3(n) != LIS_TAG3_INT) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, n, buffer);
        stream_write_string(buffer, LSTR(U" is not an integer."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }

    if (list == LIS_GENV(genv)->symbol_nil) {
        return LIS_GENV(genv)->symbol_nil;

    } else if (LIS_TAG3(list) != LIS_TAG3_BUILTIN ||
               LIS_TAG_TYPE(list) != LIS_TAG_TYPE_CONS) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, list, buffer);
        stream_write_string(buffer, LSTR(U" is not a list."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;

    } else {
        lis_obj * ret = list;
        for (int i=0; i<n->data.num; i++) {
            ret = LIS_CONS(ret)->cdr;
        }
        return LIS_CONS(ret)->car;
    }
}

lis_obj * list_nth(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"nth"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _list_nth(genv, arg1, arg2);
}

lis_obj * _list_cons(lis_obj * genv, lis_obj * a, lis_obj * b) {
    lis_obj * cons = _make_cons();
    LIS_CONS(cons)->car = a;
    LIS_CONS(cons)->cdr = b;
    return cons;
}

lis_obj * list_cons(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"cons"))) {
        return NULL;
    }

    lis_obj * arg1 = _list_nth(genv, _make_int(0), args);
    lis_obj * arg2 = _list_nth(genv, _make_int(1), args);
    return _list_cons(genv, arg1, arg2);
}

lis_obj * _list_car(lis_obj * genv, lis_obj * cons) {
    if (LIS_TAG3(cons) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(cons) != LIS_TAG_TYPE_CONS) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, cons, buffer);
        stream_write_string(buffer, LSTR(U" is not a cons."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }
    return LIS_CONS(cons)->car;
}

lis_obj * list_car(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"car"))) {
        return NULL;
    }

    return _list_car(genv, _list_nth(genv, _make_int(0), args));
}

lis_obj * _list_cdr(lis_obj * genv, lis_obj * cons) {
    if (LIS_TAG3(cons) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(cons) != LIS_TAG_TYPE_CONS) {
        lis_stream * buffer = make_lis_stream(1024, LIS_STREAM_INOUT, LIS_STREAM_TEXT);
        print(genv, cons, buffer);
        stream_write_string(buffer, LSTR(U" is not a cons."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(buffer));
        return NULL;
    }
    return LIS_CONS(cons)->cdr;
}

lis_obj * list_cdr(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"cdr"))) {
        return NULL;
    }

    return _list_cdr(genv, _list_nth(genv, _make_int(0), args));
}

lis_obj * list_first(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"first"))) {
        return NULL;
    }

    return _list_nth(genv, _make_int(0), LIS_CONS(args)->car);
}

lis_obj * list_second(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"second"))) {
        return NULL;
    }

    return _list_nth(genv, _make_int(1), LIS_CONS(args)->car);
}

lis_obj * list_third(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"third"))) {
        return NULL;
    }

    return _list_nth(genv, _make_int(2), LIS_CONS(args)->car);
}

lis_obj * list_fourth(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"fourth"))) {
        return NULL;
    }

    return _list_nth(genv, _make_int(3), LIS_CONS(args)->car);
}

lis_obj * list_fifth(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"fifth"))) {
        return NULL;
    }

    return _list_nth(genv, _make_int(4), LIS_CONS(args)->car);
}
