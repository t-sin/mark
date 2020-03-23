#ifndef __lis_runtime
#define __lis_runtime

#define NUMBER_OF_LIS_SPECIAL_FORM 1
typedef enum lis_special_form {
    LIS_SPECIAL_FORM_QUOTE,
} lis_special_form;

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
    lis_obj * symbol_t;
    lis_obj * symbol_nil;
    lis_obj ** special_forms;
} lis_runtime;

lis_runtime * init_runtime();

#endif
