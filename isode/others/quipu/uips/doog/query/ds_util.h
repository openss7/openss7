/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ds_util.h,v 9.0 1992/06/16 12:45:27 isode Rel $ */

#ifndef _query_ds_util_h_
#define _query_ds_util_h_

#include "types.h"
#include "util.h"
#include "error.h"

void qy_dn_print();
char *qy_dn2str();
QBool qy_in_hierarchy();

QE_error_code make_filter_items(), make_typed_filter_items();
QBool is_good_match();

char *get_entry_type_name();

#endif
