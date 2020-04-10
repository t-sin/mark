#include <uchar.h>

#include "obj.h"
#include "lstring.h"
#include "stream.h"
#include "eval.h"
#include "print.h"
#include "list.h"
#include "special_form.h"

lis_obj * lis_sf_quote(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"quote"))) {
        return NULL;
    }
    return args->data.cons->car;
}

lis_obj * lis_sf_function(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    lis_obj * sym = args->data.cons->car;
    if (LIS_TAG3(sym) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(sym) != LIS_TAG_TYPE_SYM) {
        lis_stream * stream = genv->data.env->env.global->stream_stderr->data.stream;
        print(genv, sym, stream);
        stream_write_string(stream, LSTR(U" is not a symbol."));
        LIS_GENV(genv)->error = _make_error(stream_output_to_string(stream));
        return NULL;
    }
    if (sym->data.sym->fn == NULL) {
        return genv->data.env->env.global->symbol_nil;
    } else {
        return sym->data.sym->fn;
    }
}

lis_obj * lis_sf_setq(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (!check_arglen(genv, args, 2, LSTR(U"setq"))) {
        return NULL;
    }

    lis_obj * sym = _list_nth(genv, _make_int(0), args);
    lis_obj * val = eval(genv, lenv, _list_nth(genv, _make_int(1), args));
    sym->data.sym->value = val;
    return val;
}

lis_obj * lis_sf_progn(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    lis_obj * ret = LIS_GENV(genv)->symbol_nil;

    lis_obj * rest = args;
    while (rest != LIS_GENV(genv)->symbol_nil) {
        ret = eval(genv, lenv, _list_car(genv, rest));
        rest = _list_cdr(genv, rest);
    }

    return ret;
}

lis_obj * lis_sf_if(lis_obj * genv, lis_obj * lenv, lis_obj * args) {
    if (!check_arglen(genv, args, 3, LSTR(U"if"))) {
        return NULL;
    }

    lis_obj * condition = eval(genv, lenv, _list_nth(genv, _make_int(0), args));
    if (condition != LIS_GENV(genv)->symbol_nil) {
        return eval(genv, lenv, _list_nth(genv, _make_int(1), args));
    } else {
        return eval(genv, lenv, _list_nth(genv, _make_int(2), args));
    }
}
    }
}
