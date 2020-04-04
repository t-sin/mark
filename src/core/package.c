#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "lstring.h"
#include "package.h"

lis_obj * add_symbol(lis_obj * package, lis_obj * symbol) {
    assert(LIS_TAG3(package) == LIS_TAG3_BUILTIN);
    assert(LIS_TAG_TYPE(package) == LIS_TAG_TYPE_PKG);

    assert(LIS_TAG3(symbol) == LIS_TAG3_BUILTIN);
    assert(LIS_TAG_TYPE(symbol) == LIS_TAG_TYPE_SYM);

    lis_package * pkg = package->data.pkg;

    if (pkg->num > pkg->size) {
        size_t new_size = pkg->size * 2;
        lis_obj ** new_symbols = (lis_obj **)malloc(sizeof(lis_obj *) * new_size);
        memset(new_symbols, 0, new_size);

        for (int i=0; i<pkg->num; i++) {
            new_symbols[i] = pkg->symbols[i];
        }
        free(pkg->symbols);
        pkg->symbols = new_symbols;
        pkg->size = new_size;
    }

    size_t i = 0;
    while (i<pkg->size) {
        if (pkg->symbols[i] == NULL) {
            break;
        }
        i++;
    }

    pkg->symbols[i] = symbol;
    pkg->num++;

    return symbol;
}

package_intern_status intern(lis_obj * package, lis_obj * name, lis_obj ** sym) {
    assert(LIS_TAG3(package) == LIS_TAG3_BUILTIN);
    assert(LIS_TAG_TYPE(package) == LIS_TAG_TYPE_PKG);

    assert(LIS_TAG3(name) == LIS_TAG3_BUILTIN);
    assert(LIS_TAG_TYPE(name) == LIS_TAG_TYPE_STR);

    lis_package * pkg = package->data.pkg;

    for (int i=0; i<pkg->num; i++) {
        if (pkg->symbols[i] != NULL &&
            _string_equal(pkg->symbols[i]->data.sym->name->data.str, name->data.str)) {
            *sym = pkg->symbols[i];
            return PKG_INTERNAL;  // TODO: INTERNAL, EXTERNAL or INHERITED
        }
    }

    *sym = _make_symbol(name);
    add_symbol(package, *sym);
    return PKG_INTERNAL;
}
