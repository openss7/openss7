/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/bind.h,v 9.0 1992/06/16 12:45:27 isode Rel $ */

#ifndef _query_bind_h_
#define _query_bind_h_

#include "types.h"

typedef enum
{
  EXTERNAL_AUTH,
  STRONG_AUTH,
  PROTECTED_AUTH,
  SIMPLE_AUTH
} auth_bind_type;

QBool bind_to_ds();
int get_association_descriptor();

#endif _query_bind_h_
