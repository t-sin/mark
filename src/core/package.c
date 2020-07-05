#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "obj.h"
#include "lstring.h"
#include "lerror.h"
#include "list.h"
#include "eval.h"

#include "package.h"

bool _table_string_eq(void * a, void * b) {
    if (a == NULL || b == NULL) {
        return false;
    }

    return _string_equal(LIS_STR((lis_obj *)a), LIS_STR((lis_obj *)b));
}

size_t _hash_string(void * str_obj, size_t hash_size) {
    lis_obj * str = (lis_obj *)str_obj;
    uintptr_t v = 0;
    for (size_t i=0; i<LIS_STR(str)->size; i++) {
        v = (v + LIS_STR(str)->body[i]) % hash_size;
    }
    return v;
}

lis_obj * _package_make_package(lis_obj * genv, lis_obj * name) {
    if (LIS_TAG_BASE(name) != LIS_TAG_BASE_BUILTIN ||
        !(LIS_TAG_TYPE(name) == LIS_TAG_TYPE_STR ||
          LIS_TAG_TYPE(name) == LIS_TAG_TYPE_SYM)) {
        not_string_error(genv, name);
        return NULL;
    }

    lis_obj * name_str;
    if (LIS_TAG_TYPE(name) == LIS_TAG_TYPE_STR) {
        name_str = name;
    } else {
        name_str = LIS_SYM(name)->name;
    }

    _table * pkgtable = LIS_GENV(genv)->package_table;
    _table_entry * res = _table_find(pkgtable, (void *)name_str);

    if (res != NULL) {
        return (lis_obj *)res->value;
    }

    lis_obj * pkg = _make_package(name_str);
    LIS_PKG(pkg)->uselist = LIS_NIL(genv);
    _table_add(pkgtable, (void *)name_str, (void *)pkg);

    return pkg;
}

lis_obj * _package_list_all_packages(lis_obj * genv) {
    lis_obj * pkg_list = LIS_NIL(genv);
    _table * table = LIS_GENV(genv)->package_table;

    for (size_t i=0; i<table->size; i++) {
        _table_entry * e = table->array + i;
        if (e->key != NULL) {
            while (e != NULL) {
                pkg_list = _list_cons(genv, (lis_obj *)e->value, pkg_list);
                e = e->next;
            }
        }
    }

    return pkg_list;
}

lis_obj * _package_in_package(lis_obj * genv, lis_obj * name) {
    if (LIS_TAG_BASE(name) != LIS_TAG_BASE_BUILTIN ||
        !(LIS_TAG_TYPE(name) == LIS_TAG_TYPE_STR ||
          LIS_TAG_TYPE(name) == LIS_TAG_TYPE_SYM)) {
        not_string_error(genv, name);
        return NULL;
    }

    _table_entry * res;
    if (LIS_TAG_TYPE(name) == LIS_TAG_TYPE_STR) {
        res = _table_find(LIS_GENV(genv)->package_table, (void *)name);
    } else {
        res = _table_find(LIS_GENV(genv)->package_table, (void *)LIS_SYM(name)->name);
    }

    if (res == NULL) {
        // some error
    }

    lis_obj * pkg = (lis_obj *)res->value;
    LIS_GENV(genv)->current_package = pkg;
    return pkg;
}

lis_obj * _package_find_package(lis_obj * genv, lis_obj * name) {
    if (LIS_TAG_BASE(name) != LIS_TAG_BASE_BUILTIN ||
        !(LIS_TAG_TYPE(name) == LIS_TAG_TYPE_STR ||
          LIS_TAG_TYPE(name) == LIS_TAG_TYPE_SYM)) {
        not_string_error(genv, name);
        return NULL;
    }

    _table_entry * res;
    if (LIS_TAG_TYPE(name) == LIS_TAG_TYPE_STR) {
        res = _table_find(LIS_GENV(genv)->package_table, (void *)name);
    } else {
        res = _table_find(LIS_GENV(genv)->package_table, (void *)LIS_SYM(name)->name);
    }

    if (res == NULL) {
        return LIS_NIL(genv);
    }

    lis_obj * pkg = (lis_obj *)res->value;
    return pkg;
}

lis_obj * _package_use_package(lis_obj * genv, lis_obj * pkg, lis_obj * to_use) {
    if (LIS_TAG_BASE(pkg) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(pkg) != LIS_TAG_TYPE_PKG) {
        not_package_error(genv, pkg);
        return NULL;
    }

    if (LIS_TAG_BASE(to_use) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(to_use) != LIS_TAG_TYPE_PKG) {
        not_package_error(genv, to_use);
        return NULL;
    }

    if (pkg == to_use) {
        return to_use;
    }

    lis_obj * rest = LIS_PKG(pkg)->uselist;

    if (rest == LIS_NIL(genv)) {
        LIS_PKG(pkg)->uselist = _list_cons(genv, to_use, LIS_NIL(genv));
    }

    while (rest != LIS_NIL(genv)) {
        if (_list_car(genv, rest) == to_use) {
            return to_use;
        }

        lis_obj * next = _list_cdr(genv, rest);
        if (next == LIS_NIL(genv)) {
            LIS_CONS(rest)->cdr = _list_cons(genv, to_use, LIS_NIL(genv));
        }
        rest = next;
    }

    return to_use;
}

lis_obj * _package_package_use_list(lis_obj * genv, lis_obj * pkg) {
    if (LIS_TAG_BASE(pkg) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(pkg) != LIS_TAG_TYPE_PKG) {
        not_package_error(genv, pkg);
        return NULL;
    }

    return LIS_PKG(pkg)->uselist;
}

lis_obj * lisp_make_package(lis_obj * genv, lis_obj * args) {
    // this does not have `use` and `nicknames` because of there's no optional args feature...
    if (!check_arglen(genv, args, 1, LSTR(U"make-package"))) {
        return NULL;
    }
    return _package_make_package(genv, _list_nth(genv, INT(0), args));
}

lis_obj * lisp_list_all_packages(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 0, LSTR(U"list-all-packages"))) {
        return NULL;
    }
    return _package_list_all_packages(genv);
}

lis_obj * lisp_in_package(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"in-package"))) {
        return NULL;
    }
    return _package_in_package(genv, _list_nth(genv, INT(0), args));
}

lis_obj * lisp_find_package(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"find-package"))) {
        return NULL;
    }
    return _package_find_package(genv, _list_nth(genv, INT(0), args));
}

lis_obj * add_symbol(lis_obj * package, lis_obj * symbol) {
    assert(LIS_TAG_BASE(package) == LIS_TAG_BASE_INTERNAL);
    assert(LIS_TAG_TYPE(package) == LIS_TAG_TYPE_PKG);

    assert(LIS_TAG_BASE(symbol) == LIS_TAG_BASE_BUILTIN);
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

package_intern_status find_symbol(lis_obj * genv, lis_obj * package, lis_obj * name, lis_obj ** sym) {
    assert(LIS_TAG_BASE(package) == LIS_TAG_BASE_INTERNAL);
    assert(LIS_TAG_TYPE(package) == LIS_TAG_TYPE_PKG);

    assert(LIS_TAG_BASE(name) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(name) == LIS_TAG_TYPE_STR);

    lis_package * pkg = package->data.pkg;

    for (int i=0; i<pkg->num; i++) {
        if (pkg->symbols[i] != NULL &&
            _string_equal(LIS_STR(LIS_SYM(pkg->symbols[i])->name), LIS_STR(name))) {
            *sym = pkg->symbols[i];
            return PKG_INTERNAL;
        }
    }

    lis_obj * rest = pkg->uselist;
    while (rest != LIS_NIL(genv)) {
        package_intern_status status = find_symbol(genv, _list_car(genv, rest), name, sym);
        if (status == PKG_INTERNAL || status == PKG_INHERITED) {
            return PKG_INHERITED;
        }
        rest = _list_cdr(genv, rest);
    }

    *sym = LIS_NIL(genv);
    return PKG_NOT_FOUND;
}

package_intern_status intern(lis_obj * genv, lis_obj * package, lis_obj * name, lis_obj ** sym) {
    assert(LIS_TAG_BASE(package) == LIS_TAG_BASE_INTERNAL);
    assert(LIS_TAG_TYPE(package) == LIS_TAG_TYPE_PKG);

    assert(LIS_TAG_BASE(name) == LIS_TAG_BASE_BUILTIN);
    assert(LIS_TAG_TYPE(name) == LIS_TAG_TYPE_STR);

    lis_obj * s;

    package_intern_status status = find_symbol(genv, package, name, &s);

    if (status == PKG_INTERNAL ||
        status == PKG_EXTERNAL ||
        status == PKG_INHERITED) {
        *sym = s;
        return status;
    }

    *sym = _make_symbol(name);
    LIS_SYM(*sym)->package = package;
    add_symbol(package, *sym);
    return PKG_INTERNAL;
}

bool import(lis_obj * genv, lis_obj * sym, lis_obj * pkg) {
    if (LIS_TAG_BASE(sym) != LIS_TAG_BASE_BUILTIN ||
        LIS_TAG_TYPE(sym) != LIS_TAG_TYPE_SYM) {
        not_symbol_error(genv, sym);
        return NULL;
    }

    if (LIS_TAG_BASE(pkg) != LIS_TAG_BASE_INTERNAL ||
        LIS_TAG_TYPE(pkg) != LIS_TAG_TYPE_PKG) {
        not_package_error(genv, pkg);
        return NULL;
    }

    lis_obj * result;
    package_intern_status status = find_symbol(genv, pkg, LIS_SYM(sym)->name, &result);
    if (status == PKG_NOT_FOUND || status == PKG_INHERITED) {
        add_symbol(pkg, sym);
        return true;
    } else {
        return false;
    }
}

lis_obj * status_to_keyword(lis_obj * genv, package_intern_status status) {
    lis_obj * key;
    switch (status) {
    case PKG_INTERNAL:
        intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"internal"), &key);
        break;
    case PKG_EXTERNAL:
        intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"external"), &key);
        break;
    case PKG_INHERITED:
        intern(genv, LIS_GENV(genv)->keyword_package, LSTR(U"inherited"), &key);
        break;
    default:
        key = LIS_NIL(genv);
    }

    return key;
}

lis_obj * lisp_intern(lis_obj * genv, lis_obj * args) {
    if (!check_argge(genv, args, 1, LSTR(U"intern"))) {
        return NULL;
    }

    lis_obj * name = _list_nth(genv, INT(0), args);
    lis_obj * pkg = _list_nth(genv, INT(1), args);
    lis_obj * ret;

    if (pkg == LIS_NIL(genv)) {
        pkg = LIS_GENV(genv)->current_package;
    } else if (LIS_TAG_BASE(pkg) != LIS_TAG_BASE_INTERNAL ||
               LIS_TAG_TYPE(pkg) != LIS_TAG_TYPE_PKG) {
        not_package_error(genv, pkg);
        return NULL;
    }

    package_intern_status status = intern(genv, pkg, name, &ret);
    lis_obj * key = status_to_keyword(genv, status);
    lis_obj * mv = _make_multiple_value();
    LIS_MVAL(mv) = _list_cons(genv, ret, _list_cons(genv, key, LIS_NIL(genv)));
    return mv;
}

lis_obj * lisp_use_package(lis_obj * genv, lis_obj * args) {
    if (!check_argge(genv, args, 1, LSTR(U"use-package"))) {
        return NULL;
    }

    lis_obj * to_use = _package_find_package(genv, _list_nth(genv, INT(0), args));
    lis_obj * pkg = _list_nth(genv, INT(1), args);

    if (pkg == LIS_NIL(genv)) {
        pkg = LIS_GENV(genv)->current_package;
    }

    return _package_use_package(genv, pkg, to_use);
}

lis_obj * lisp_package_use_list(lis_obj * genv, lis_obj * args) {
    if (!check_arglen(genv, args, 1, LSTR(U"package-use-list"))) {
        return NULL;
    }

    return _package_package_use_list(genv, _list_nth(genv, INT(0), args));
}

lis_obj * lisp_find_symbol(lis_obj * genv, lis_obj * args) {
    if (!check_argge(genv, args, 1, LSTR(U"find-symbol"))) {
        return NULL;
    }

    lis_obj * name = _list_nth(genv, INT(0), args);
    lis_obj * pkg = _list_nth(genv, INT(1), args);
    
    if (LIS_TAG_BASE(name) != LIS_TAG_BASE_BUILTIN ||
        !(LIS_TAG_TYPE(name) == LIS_TAG_TYPE_STR ||
          LIS_TAG_TYPE(name) == LIS_TAG_TYPE_SYM)) {
        not_string_error(genv, name);
        return NULL;
    }

    if (pkg == LIS_NIL(genv)) {
        pkg = LIS_GENV(genv)->current_package;
    }

    lis_obj * ret;
    package_intern_status status = find_symbol(genv, pkg, name, &ret);
    lis_obj * key = status_to_keyword(genv, status);
    lis_obj * mv = _make_multiple_value();
    LIS_MVAL(mv) = _list_cons(genv, ret, _list_cons(genv, key, LIS_NIL(genv)));
    return mv;
}

lis_obj * lisp_import(lis_obj * genv, lis_obj * args) {
    if (!check_argge(genv, args, 2, LSTR(U"import"))) {
        return NULL;
    }

    lis_obj * sym = _list_nth(genv, INT(0), args);
    lis_obj * pkg = _list_nth(genv, INT(1), args);

    if (import(genv, sym, pkg)) {
        return LIS_T(genv);
    } else {
        return LIS_NIL(genv);
    }
}
