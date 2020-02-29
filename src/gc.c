#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>

#include "obj.h"


typedef struct Header {
    struct Header * next_free;
    size_t size;
} Header;

#define HEAP_DEFAULT_SIZE 50000
#define HEAP_MINIMUM_SIZE 30000
static Header gc_memory[HEAP_DEFAULT_SIZE];
static size_t gc_memory_size;
static Header * free_list;

void gc_init(void) {
    gc_memory_size = HEAP_DEFAULT_SIZE;
    free_list = NULL;
};

void * gc_malloc(size_t size) {
    size_t bs;

    if (size < HEAP_MINIMUM_SIZE) {
        bs = HEAP_MINIMUM_SIZE;
    } else {
        bs = size;
    }

    for (;;) {
        return NULL;
    }

    return NULL;
}

void gc_free(size_t ptr);
void gc_collect(void);
void gc_add_root(void * start, void * end);



#include <assert.h>
#include <stdio.h>

void * gc_malloc1(size_t size) {
    void * ptr = gc_malloc(size);
    printf("malloced = %p\n", ptr);
    return ptr;
}

void test() {
    gc_init();
    for (int i=1; i<10; i++) {
        printf("[%d] ", i);
        gc_malloc1(500 * i);
    }

    printf("--- free_list ---\n");
    for (Header * h = free_list; h->next_free != free_list; h = h->next_free) {
        printf(" * %p, size = %ld\n", (void *)h, h->size);
    }

    for (int i=0; i<HEAP_DEFAULT_SIZE; i++) {
        //printf("%x", gc_memory[i]);
    }
    puts("");

    puts("ok.");
}

int main () {
    test();
    return 0;
}
