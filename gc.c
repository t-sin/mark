#include<assert.h>
#include<stdio.h>
#include<stdlib.h>

struct Heap {
    unsigned char flags;
    size_t size;
    void * ptr;
    size_t next;
};

#define HEAP_LIMIT 10000
#define GC_ALLOC 0x1
#define GC_MARKED 0x2

static struct Heap gc_heaps[HEAP_LIMIT];
static size_t gc_last = 0;

size_t find_heap(size_t pos) {
    for (size_t p=pos; p<HEAP_LIMIT; p++) {
        if ((gc_heaps[p].flags & GC_ALLOC)) {
            continue;
        } else {
            return p;
        }
    }
    printf("GC: heap exausted.\n");
    assert(0);
}

size_t gc_malloc(size_t size) {
    size_t pos = find_heap(gc_last);
    void * ptr = malloc(size);
    assert(ptr != NULL);

    gc_heaps[pos].flags = GC_ALLOC;
    gc_heaps[pos].size = size;
    gc_heaps[pos].ptr = ptr;
    gc_heaps[pos].next = -1;

    if (gc_last == -1) {
        gc_last = pos;
    } else {
        gc_heaps[gc_last].next = pos;
        gc_last = pos;
    }

    return pos;
}

void gc_free(size_t ptr) {
    size_t target = 0;
    size_t rest = gc_heaps[ptr].next;

    for (; target<HEAP_LIMIT; target++) {
        if (gc_heaps[target].next == ptr) {
            break;
        }
    }

    assert(target < HEAP_LIMIT);

    gc_heaps[target].next = rest;

    gc_heaps[ptr].flags ^= GC_ALLOC;
    gc_heaps[ptr].next = -1;
}

void gc_init(void) {
    for (int i=0; i<HEAP_LIMIT; i++) {
        gc_heaps[i].flags = 0;
        gc_heaps[i].size = 0;
        gc_heaps[i].ptr = NULL;
        gc_heaps[i].next = -1;
    }
}

void gc_collect(void);
void gc_add_root(void * start, void * end);

void test_simple_malloc_and_free() {
    assert(gc_malloc(1) == 0);
    assert(gc_malloc(2) == 1);
    assert(gc_malloc(3) == 2);
    assert(gc_malloc(4) == 3);

    assert(gc_heaps[3].flags == GC_ALLOC);
    assert(gc_heaps[3].size == 4);
    assert(gc_heaps[3].next == -1);

    gc_free(3);

    assert(gc_heaps[2].next == -1);
    assert(gc_heaps[3].flags == 0);
    assert(gc_heaps[3].next == -1);

    gc_free(1);

    assert(gc_heaps[0].next == 2);
}

void test() {
    gc_init();
    test_simple_malloc_and_free();
    puts("ok.");
}

int main () {
    test();
    return 0;
}
