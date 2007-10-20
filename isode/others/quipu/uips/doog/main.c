#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/RCS/main.c,v 9.0 1992/06/16 12:45:18 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/RCS/main.c,v 9.0 1992/06/16 12:45:18 isode Rel $ */

/*
 * $Log: main.c,v $
 * Revision 9.0  1992/06/16  12:45:18  isode
 * Release 8.0
 *
 */

#include "interact.h"
#include "query.h"
#include "initialize.h"

#include "quipu/util.h"

void exit();

static void read_doog_args();

QBool testing = FALSE;

int main(argc, argv)
     int argc;
     char *argv[];
{
  (void) fprintf(stderr, "Binding to directory.....\n");

  read_doog_args(argc, argv);
  init_query_engine(argc, argv);
  initialize();
  if (bind_to_ds(argc, argv, SIMPLE_AUTH) == FALSE) exit(1);
  
  interact();
  
  return 0;
}

static void read_doog_args(argc, argv)
     int argc;
     char *argv[];
{
  char *arg;
  int count;

  for (count = 0, arg = argv[count];
       count < argc;
       count++, arg = argv[count])
    if (*arg == '-')
      switch (*++arg)
	{
	case 't':
	case 'T':
	  if (lexequ(arg, "test") == 0) testing = TRUE;
	  break;
	}
}
      

  
