#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/util.c,v 9.0 1992/06/16 12:45:27 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/util.c,v 9.0 1992/06/16 12:45:27 isode Rel $ */

/*
 * $Log: util.c,v $
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

#include "types.h"
#include "util.h"
#include "quipu/util.h"

char *copy_string(string)
     char *string;
{
  char *new_string;

  if (string == NULLCP) return NULLCP;

  new_string = (char *) smalloc(strlen(string) + 1);
  (void) strcpy(new_string, string);

  return new_string;
}

void friendlify(name, fname)
     char *name, *fname;
{
  register char *start, *end;
  char save;

  *fname = '\0';

  end = name;
  while (!isnull(*end)) end++;
  while (end > name && isspace(*(end - 1))) end--;
  
  while (1)
    {
      start = end;
      while (*start != '=') start--;
      start++;
      
      save = *end;
      *end = '\0';

      while (isspace(*start)) start++;

      if (!isnull(*fname))
	(void) strcat(fname, ", ");

      (void) strcat(fname, start);

      *end = save;

      end = start;
      while (end > name && *end != '@' && *end != '\n')
	end--;

      if (end == name)
	break;
      
      end--;
      while(isspace(*end)) end--;
      end++;
    }
}
