#ifndef __lis_environment
#define __lis_environment

#include "obj.h"

#define NUMBER_OF_LIS_SPECIAL_FORM 1
typedef enum lis_special_form {
    LIS_SPECIAL_FORM_QUOTE,
} lis_special_form;

typedef struct lis_global_environment {
    lis_package_table * package_table;
    lis_obj * current_package;
    lis_obj * keyword_package;
    lis_obj * symbol_t;
    lis_obj * symbol_nil;
    lis_obj ** special_forms;
} lis_global_environment;

lis_obj * init_global_env();

typedef struct lis_dynamic_environment {
} lis_dynamic_environment;

typedef struct lis_lexical_environment {
} lis_lexical_environment;

#endif
