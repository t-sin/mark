#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "../../src/util/table.h"

void test_find_from_empty_table() {
    _table * table = _make_table(10);

    assert(table->size == 10);
    assert(table->array != NULL);
    assert(table->num == 0);

    assert(_table_find(table, (void *)100) == NULL);
}

void test_remove_from_empty_table() {
    _table * table = _make_table(10);

    _table_remove(table, (void *)0);
    _table_remove(table, (void *)100);
}

void test_add_once() {
    _table * table = _make_table(10);

    assert(table->num == 0);
    _table_add(table, (void *)100, (void *)42);
    assert(table->num == 1);
}

void test_remove_once() {
    _table * table = _make_table(10);
    _table_entry * e;

    _table_add(table, (void *)100, (void *)42);
    assert(table->num == 1);

    e = _table_find(table, (void *)100);
    assert(e != NULL);
    assert(e->value == (void *)42);

    _table_remove(table, (void *)100);
    assert(table->num == 0);
}

void test_add_and_remove() {
    _table * table = _make_table(10);
    _table_entry * e;

    _table_add(table, (void *)100, (void *)42);
    _table_add(table, (void *)101, (void *)43);
    assert(table->num == 3);

    e = _table_find(table, (void *)101);
    assert(e != NULL);
    assert(e->value == (void *)43);

    _table_remove(table, (void *)101);
    assert(table->num == 2);

    e = _table_find(table, (void *)101);
    assert(e == NULL);

    _table_add(table, (void *)102, (void *)44);
    assert(table->num == 2);

    e = _table_find(table, (void *)102);
    assert(e != NULL);
    assert(e->value == (void *)45);

    _table_remove(table, (void *)100);
    _table_remove(table, (void *)101);
    _table_remove(table, (void *)102);

    assert(table->num == 0);
}

void test_hash_collision() {
    _table * table = _make_table(10);
    _table_entry * e;

    _table_add(table, (void *)100, (void *)42);
    _table_add(table, (void *)110, (void *)43);

    assert(table->num == 2);

    e = _table_find(table, (void *)100);
    assert(e != NULL);
    assert(e->value == (void *)42);

    e = _table_find(table, (void *)110);
    assert(e != NULL);
    assert(e->value == (void *)43);
}

void test_rehash() {
    _table * table = _make_table(10);
    _table_entry * e;

    _table_add(table, (void *)100, (void *)42);
    _table_add(table, (void *)101, (void *)43);
    _table_add(table, (void *)102, (void *)44);
    _table_add(table, (void *)103, (void *)45);
    _table_add(table, (void *)104, (void *)46);

    assert(table->num == 5);
    assert(table->size == 10);

    _table_add(table, (void *)105, (void *)47);

    assert(table->num == 6);
    assert(table->size == 20);

    for (size_t i=0; i<=5; i++) {
        e = _table_find(table, (void *)(100 + i));
        assert(e != NULL);
        assert(e->value == (void *)(42 + i));
    }
}

int main() {
    test_find_from_empty_table();
    test_remove_from_empty_table();

    test_add_once();
    test_remove_once();

    test_hash_collision();
    test_rehash();

    return 0;
}
