SalaryDefs DEFINITIONS ::=

%{
#include <stdio.h>

char   *myname;

static struct salary_record {
    char   *name;
    int     salary;
}	salary;

#define PEPYPARM struct salary_record *


main (argc, argv)
int     argc;
char  **argv;
{
    PE	    pe;

    myname = argv[0];

    salary.name = argv[1];
    salary.salary = atoi (argv[2]);
    build_SalaryDefs_Salary (&pe, 1, 0, NULLCP, &salary);

    salary.name = NULL;
    salary.salary = 0;
    unbuild_SalaryDefs_Salary (pe, 1, NULLIP, NULLVP, &salary);

    exit (0);
}
%}

...
