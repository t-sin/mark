#include "../util/table.h"
#include "obj.h"

#include "environment.h"

lis_obj * get_lexical_value(lis_obj * lenv, lis_obj * name) {
    if (lenv == NULL) {
        return NULL;
    }

    _table_entry * entry = _table_find(LIS_LENV(lenv)->var, (void *)name);

    if (entry == NULL) {
        return LIS_ENV(lenv)->parent;
    }

    return (lis_obj *)entry->value;
}

void set_lexical_value(lis_obj * lenv, lis_obj * name, lis_obj * val) {
    _table_add(LIS_LENV(lenv)->var, (void *)name, (void *)val);
}
