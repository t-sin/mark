#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "obj.h"

struct Header {
    unsigned char flags;
    size_t size;
    struct Header * next;
};

struct Heap {
    struct Header * head;
    size_t size;
};

#define HEADER_SIZE ((size_t) sizeof(struct Header))
#define POINTER_SIZE ((size_t) sizeof(void *))

#define HEAP_LIMIT 10000
#define GC_ALLOC 0x1
#define GC_MARKED 0x2

struct Header * free_list;

static struct Header gc_heaps[HEAP_LIMIT];

size_t gc_malloc(size_t size) {}

void gc_free(size_t ptr) {}

void gc_init(void) {}

void gc_collect(void);
void gc_add_root(void * start, void * end);

void test_simple_malloc_and_free() {}

void test() {
    LisObj obj;
    obj.tag = 0x07;
    printf("gc_tag = %d\n", LIS_GC_TAG(obj));
    printf("tag1 = %d\n", LIS_TAG1(obj));
    printf("tag2 = %d\n", LIS_TAG2(obj));
    printf("tag3 = %d\n", LIS_TAG3(obj));

    gc_init();
    puts("ok.");
}

int main () {
    test();
    return 0;
}
