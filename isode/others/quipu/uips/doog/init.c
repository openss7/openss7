#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/RCS/init.c,v 9.0 1992/06/16 12:45:18 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/RCS/init.c,v 9.0 1992/06/16 12:45:18 isode Rel $ */

/*
 * $Log: init.c,v $
 * Revision 9.0  1992/06/16  12:45:18  isode
 * Release 8.0
 *
 */

#include "quipu/util.h"
#include "tailor.h"

#include "query.h"
#include "initialize.h"
#include "util.h"
#include "config.parse.h"

#ifndef NO_STATS
extern LLog    *log_stat;
#endif

extern char username[], userpassword[];

QCardinal config_line_num = 0;
char config_file_name[LINESIZE];
FILE *config_file;

static void read_quipurc();

void exit();

void initialize()
{
  char *homeconfig = "/.doogrc";
  char *sysconfig = "doogrc";
  extern QBool testing;
  char *getenv();

#ifndef NO_STATS
  ll_hdinit (log_stat, "doog");
#endif

  if (testing == FALSE)
    {
      (void) strcpy(config_file_name, getenv("HOME"));
      (void) strcat(config_file_name, homeconfig);
      
      if ((config_file = fopen(config_file_name, "r")) == (FILE *) NULL)
	{
	  (void) strcpy(config_file_name, isodefile(sysconfig, 0));
	  
	  if ((config_file = fopen(config_file_name, "r")) == (FILE *) NULL)
	    {
	      (void) fprintf(stderr,
			     "Cannot find `doogrc' tailor file. Exiting.\n");
	      exit (1);
	    }
	}
    }
  else
    {
      (void) strcpy(config_file_name, "./");
      (void) strcat(config_file_name, sysconfig);
      
      if ((config_file = fopen(config_file_name, "r")) == (FILE *) NULL)
	{
	  (void) fprintf(stderr, "Cannot find `doogrc' tailor file.\n");
	  (void) fprintf(stderr,
		      "To test, run `doog -test' in the source directory.\n");

	  exit(1);
	}
    }
  
  (void) parseConfig();
  (void) fclose(config_file);

  read_quipurc();
}

static void read_quipurc()
{
  char quipurc[LINESIZE];
  char *quipurc_name = "/.quipurc";
  char *SkipSpace(), *TidyString();
  char *p, *part1, *part2;
  char line[LINESIZE];
  FILE *qfile;

  (void) strcpy(quipurc, getenv("HOME"));
  (void) strcat(quipurc, quipurc_name);

  if ((qfile = fopen(quipurc, "r")) == NULL) return;

  while (fgets (line, LINESIZE, qfile) != 0)
    {
      p = SkipSpace(line);

      if ((*p == '#') || (*p == '\0'))
	continue;  /* ignore comments and blanks */

      part1 = p;
      if ((part2 = index (p,':')) == NULLCP)
	continue; /* ignore it */
      
      *part2++ = '\0';
      part2 = TidyString(part2);
      
      if ((lexequ(part1, "username") == 0) && *username == '\0')
	(void) strcpy (username, part2);
      else if ((lexequ(part1, "password") == 0) && *userpassword == '\0')
	(void) strcpy (userpassword, part2);
      else if (lexequ (part1, "dsap") == 0)
	(void) tai_string (part2);
      else if (lexequ (part1, "isode") == 0)
	{
	  char *split;
	  if ((split = index (part2,' ')) != NULLCP)
	    {
	      *split++ = 0;
	      (void) isodesetvar (part2, split, 0);
	    }
	}
      else if (lexequ(part1, "service") == 0)
	new_service (part2);
    }
  isodexport(NULLCP);
  (void) fclose(qfile);
}
