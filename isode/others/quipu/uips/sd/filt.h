/*
 * $Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/filt.h,v 9.0 1992/06/16 12:45:08 isode Rel $
 */

#ifndef FILT
#define FILT

#include "quipu/ds_search.h"

typedef struct stroid_list {
  int fi_type;
  char *stroid;
  char *name;
} filt_item;

typedef struct filter_struct {
  int flt_type;
  union pod_ftype {
    filt_item item;
    struct filter_struct *sub_filt;
  } fu_cont;
  struct filter_struct *next;
} filt_struct;

void make_type();
filt_struct *make_item_filter();
filt_struct *link_filters();
filt_struct *make_parent_filter();
Filter make_filter();
Filter make_attr_filter();
void free_filt();

#endif 
