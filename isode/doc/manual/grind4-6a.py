-- personnel record pretty-printer

PERSONNEL DEFINITIONS	::=

%{
#define DEBUG

#include <stdio.h>


#define ps_adios(ps, f) \
	adios (NULLCP, "%s: %s", (f), ps_error ((ps) -> ps_errno))


static char *myname = "prpp";


void	adios ();


static char *givenName;
static char *initial;
static char *familyName;
static char *Title;
static char *Date;


static char *months[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

...
