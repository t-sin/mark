#ifndef __lis_runtime
#define __lis_runtime

typedef struct lis_package_table {
    lis_package * packages;
    size_t size;
} lis_package_table;

lis_package_table * package_table_init();
void package_table_add(lis_package * package);
void package_table_remove(lis_package * package);

typedef struct lis_runtime {
    // lis_readtable * readtable;
    lis_package_table * package_table;
    lis_obj * current_package;
    lis_obj * keyword_package;
    lis_symbol * symbol_t;
    lis_symbol * symbol_nil;
} lis_runtime;

lis_runtime * init_runtime();

#endif
