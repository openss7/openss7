/*
 * $Header: /xtel/isode/isode/others/quipu/uips/sd/RCS/sequence.h,v 9.0 1992/06/16 12:45:08 isode Rel $
 */

/* This file contains code to implement the list storage facilities
 * in the modified widget program (renamed SD 5/1/90).
 */

/*    This file was written by Damanjit Mahl @ Brunel University
 *    as part of the modifications made to 
 *    the Quipu X.500 widget interface written by Paul Sharpe
 *    at GEC Research, Hirst Research Centre.
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

#ifndef STRINGSEQ
#define STRINGSEQ

#include "general.h"

typedef struct string_seq {
  char *dname;
  unsigned s_strlen;
  int seq_num;
  struct string_seq *next;
} string_seq, *str_seq;

#define NULLDS ((str_seq) 0)
char *get_from_seq ();
void free_seq(), add_seq();

#endif
