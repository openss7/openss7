/* prpp.c - Personnel Record prety-printer */

#include <stdio.h>

int	print_PERSONNEL_PersonnelRecord ();


main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
    exit (PY_pp (argc, argv, envp, print_PERSONNEL_PersonnelRecord));
}
