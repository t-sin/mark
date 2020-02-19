#include <stdalign.h>
#include <stdio.h>
#include <stdlib.h>

#include "obj.h"


typedef struct Header {
    struct Header * next;
    size_t size;
} Header;

#define HEAP_DEFAULT_SIZE 50000
#define HEAP_MINIMUM_SIZE 1000
static size_t alignment = alignof(Header);
static unsigned char * gc_memory;
static size_t gc_memory_size;
static Header * free_list;

void gc_init(void) {
    gc_memory_size = HEAP_DEFAULT_SIZE;
    gc_memory = malloc(gc_memory_size);  // くっそぉぉぉぉぉ

    Header * header = (Header *)gc_memory;
    header->next = header;
    header->size = 0;
    free_list = header;
};

void * gc_malloc(size_t size) {
    size_t bs;

    if (size < HEAP_MINIMUM_SIZE) {
        bs = HEAP_MINIMUM_SIZE;
    } else {
        bs = size;
    }

    if (free_list == free_list->next) {
        Header * header = (Header *)&gc_memory[HEAP_DEFAULT_SIZE - 1 - sizeof(Header)];
        header->size = bs;
        header->next = free_list;
        free_list->next = header;
        return (void *)header - bs;
    }

    Header * p;
    for (p = free_list; p->next != free_list; p = p->next) {
        void * start = p + 1;  // block start
        void * end = p->next - 1;  // block end
//        printf("%p, %p, %p\n", start, end, p);
        // checks if exists a space between current header and next block
        if ((sizeof(Header) + bs) < (end - p->size - start)) {
            // もっとまんなかへんに位置とるとよい？
            Header * nh = p->next - 1;
            nh->next = p->next;
            nh->size = bs;
            p->next = nh;
            return (void *)(nh) - bs;
        }
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
    for (Header * h = free_list; h->next != free_list; h = h->next) {
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
