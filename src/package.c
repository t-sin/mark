#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"

lis_obj * add_symbol(lis_package * package, lis_obj * symbol) {
    assert(LIS_TAG3(symbol) == LIS_TAG3_BUILTIN);
    assert(LIS_TAG_TYPE(symbol) == LIS_TAG_TYPE_SYM);

    if (package->num > package->size) {
        size_t new_size = package->size * 2;
        lis_obj ** new_symbols = (lis_obj *)malloc(sizeof(lis_obj) * new_size);
        memset(new_symbols, 0, new_size);

        for (int i=0; i<package->num; i++) {
            new_symbols[i] = package->symbols[i];
        }
        free(package->symbols);
        package->symbols = new_symbols;
        package->size = new_size;
    }

    size_t i = 0;
    while (i<package->size) {
        if (package->symbols[i] == NULL) {
            break;
        }
        i++;
    }

    package->symbols[i] = symbol;
    package->num++;

    return symbol;
}

lis_obj * intern(lis_package * package, lis_string * name) {
}
