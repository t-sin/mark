#include <uchar.h>

#include "obj.h"
#include "stream.h"
#include "print.h"
#include "special_form.h"

lis_obj * lis_sf_quote(lis_obj * obj, lis_obj * args) {
    return args;
}

lis_obj * lis_sf_function(lis_obj * genv, lis_obj * args) {
    if (LIS_TAG3(args) != LIS_TAG3_BUILTIN ||
        LIS_TAG_TYPE(args) != LIS_TAG_TYPE_SYM) {
        return NULL;
    }
    if (args->data.sym->fn == NULL) {
        return genv->data.env->env.global->symbol_nil;
    } else {
        return args->data.sym->fn;
    }
}
