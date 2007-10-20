
#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/sequence.c,v 9.0 1992/06/16 12:44:54 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/sequence.c,v 9.0 1992/06/16 12:44:54 isode Rel $
 *
 *
 * $Log: sequence.c,v $
 * Revision 9.0  1992/06/16  12:44:54  isode
 * Release 8.0
 *
 */


#include "general.h" 
#include <sys/types.h>
#include "sequence.h"

void add_seq (seq, str)
     str_seq *seq;
     char *str;
{
  str_seq curr;
  
  if (*seq) {
    for (curr = *seq; curr->next; curr = curr->next) ;
    curr->next = (struct str_seq *) malloc (sizeof (struct str_seq));
    curr = curr->next;
  } else {
    curr = (struct str_seq *) malloc (sizeof (struct str_seq));
    *seq = curr;
  }

  curr->dname = (char *) malloc ((unsigned int) (strlen(str) + 1));
  (void) strcpy(curr->dname, str);
  
  curr->next = 0;
}
	          
char *get_from_seq (seq_num, seq_ptr)
     int seq_num;
     str_seq seq_ptr;
{
  for (; seq_num > 1 && seq_ptr; seq_ptr = seq_ptr->next, seq_num--) {}
  if (seq_ptr) return seq_ptr->dname;
  else return 0;
}

void free_seq (seq_ptr)
     str_seq seq_ptr;
{
  str_seq next_seq;
  
  while (seq_ptr) {
    free((char *) (seq_ptr->dname));
    next_seq = seq_ptr->next;
    free((char *) seq_ptr);
    seq_ptr = next_seq;
  }
}
