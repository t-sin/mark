#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "table.h"

#include <stdio.h>

size_t _hash(void * ptr, size_t hash_size) {
    uintptr_t v = (uintptr_t)ptr;
    return v % hash_size;
}

bool _table_ptr_eq(void * a, void * b) {
    return a == b;
}

_table * _make_table(size_t size) {
    _table * table = (_table *)malloc(sizeof(_table));
    table->size = size;
    table->array = (_table_entry *)malloc(sizeof(_table_entry) * size);
    memset(table->array, 0, sizeof(_table_entry) * size);
    table->num = 0;
    table->hash_fn = _hash;
    table->eq_fn = _table_ptr_eq;
    return table;
}

void _free_table(_table * table) {
    free(table->array);
    free(table);
}

_table_entry * _table_find(_table * table, void * key) {
    size_t pos = table->hash_fn(key, table->size);
    _table_entry * entry = table->array + pos;

    while (entry != NULL) {
        if (table->eq_fn(entry->key, key)) {
            return entry;
        }
        entry = entry->next;
    }

    return NULL;
}

_table_entry * _entry_set(_table_entry * array, size_t size, void * key, void * value,
                          size_t (* hash_fn)(void *, size_t), bool (* eq_fn)(void *, void *)) {
    size_t pos = hash_fn(key, size);
    _table_entry * base = array + pos;
    _table_entry * entry = base;
    _table_entry * prev = NULL;

    if (entry->key == NULL) {
        entry->key = key;
        entry->value = value;
        return entry;
    }

    while (entry != NULL) {
        if (eq_fn(entry->key, key)) {
            entry->value = value;
            return entry;  // existed is overwritten
        }
        prev = entry;
        entry = entry->next;
    }

    entry = (_table_entry *)malloc(sizeof(_table_entry));
    prev->next = entry;
    entry->key = key;
    entry->value = value;

    return NULL;  // created new
}

void _table_rehash(_table * table, size_t new_size) {
    _table_entry * new_array = (_table_entry *)malloc(sizeof(_table_entry) * new_size);
    memset(new_array, 0, sizeof(_table_entry) * new_size);

    for (size_t i=0; i<table->size; i++) {
        _table_entry * e = table->array + i;
        if (e->key != NULL) {
            while (e != NULL) {
                _entry_set(new_array, new_size, e->key, e->value, table->hash_fn, table->eq_fn);
                e = e->next;
            }
        }
    }

    free(table->array);
    table->array = new_array;
    table->size = new_size;
}

void _table_add(_table * table, void * key, void * value) {
    _entry_set(table->array, table->size, key, value, table->hash_fn, table->eq_fn);
    table->num++;

    if (table->num / (float)table->size > 0.6) {
        _table_rehash(table, table->size * 2);
    }
}

void _table_remove(_table * table, void * key) {
    size_t pos = table->hash_fn(key, table->size);
    _table_entry * base = table->array + pos;
    _table_entry * entry = base;
    _table_entry * prev = NULL;

    while (entry != NULL) {
        if (table->eq_fn(entry->key, key)) {
            if (prev != NULL) {
                prev->next = entry->next;
            }

            if (entry != base) {
                free(entry);
            }

            table->num--;
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}

_table * _copy_table(_table * table) {
    _table * new_table = (_table *)malloc(sizeof(_table));
    new_table->size = table->size;
    new_table->array = (_table_entry *)malloc(sizeof(_table_entry) * new_table->size);
    new_table->num = table->num;
    new_table->hash_fn = table->hash_fn;
    new_table->eq_fn = table->eq_fn;

     for (size_t i=0; i<table->size; i++) {
        _table_entry * e = table->array + i;
        if (e->key != NULL) {
            while (e != NULL) {
                _entry_set(new_table->array, new_table->size, e->key, e->value,
                           table->hash_fn, table->eq_fn);
                e = e->next;
            }
        }
    }

    return new_table;
}
