#ifndef __lis_sequence
#define __lis_sequence

#include "obj.h"

// seq_copy_seq

lis_obj * _seq_length(lis_obj * genv, lis_obj * seq);
lis_obj * seq_length(lis_obj * genv, lis_obj * args);

lis_obj * _seq_elt(lis_obj * genv, lis_obj * seq, lis_obj * n);
lis_obj * seq_elt(lis_obj * genv, lis_obj * args);

// seq_subseq
// seq_concatenate

// seq_map
// seq_reduce

// seq_reverse

// seq_sort

// seq_find
// seq_find_if
// seq_position
// seq_position_if
// seq_remove
// seq_remove_if

// seq_remove_duplicate

#endif
