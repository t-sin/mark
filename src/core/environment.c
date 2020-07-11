#include <setjmp.h>

#include "../util/table.h"
#include "obj.h"

#include "environment.h"

bool find_from_lexical_env(lis_obj * lenv, lis_obj * name) {
    if (lenv == NULL) {
        return NULL;
    }

    if (_table_find(LIS_LENV(lenv)->var, (void *)name) == NULL) {
        return find_from_lexical_env(LIS_ENV(lenv)->parent, name);
    }

    return true;
}

lis_obj * get_lexical_value(lis_obj * lenv, lis_obj * name) {
    if (lenv == NULL) {
        return NULL;
    }

    _table_entry * entry = _table_find(LIS_LENV(lenv)->var, (void *)name);

    if (entry == NULL) {
        return get_lexical_value(LIS_ENV(lenv)->parent, name);
    }

    return (lis_obj *)entry->value;
}

void set_lexical_value(lis_obj * lenv, lis_obj * name, lis_obj * val) {
    _table_add(LIS_LENV(lenv)->var, (void *)name, (void *)val);
}

void find_and_set_lexical_value(lis_obj * lenv, lis_obj * name, lis_obj * val) {
    if (lenv == NULL) {
        return;
    }

    _table_entry * entry = _table_find(LIS_LENV(lenv)->var, (void *)name);

    if (entry != NULL) {
        _table_add(LIS_LENV(lenv)->var, (void *)name, (void *)val);
    } else {
        set_lexical_value(LIS_ENV(lenv)->parent, name, val);
    }
}

jmp_buf * get_block(lis_obj * lenv, lis_obj * name) {
    if (lenv == NULL) {
        return NULL;
    }

    _table_entry * entry = _table_find(LIS_LENV(lenv)->btags, (void *)name);

    if (entry == NULL) {
        return get_block(LIS_ENV(lenv)->parent, name);
    } else {
        return (jmp_buf *)entry->value;
    }
}
