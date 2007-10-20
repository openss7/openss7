/*
 * $Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/sequence.h,v 9.0 1992/06/16 12:44:54 isode Rel $
 */

#ifndef SEQ
#define SEQ

#include "general.h"
#include "defs.h"

typedef struct str_seq {
  char *dname;
  struct str_seq *next;
} strSeq, *str_seq;

#define NULLDS ((str_seq) 0)
char *get_from_seq();
void add_seq();
void free_seq();
#endif

