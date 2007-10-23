...

static struct dispatch dispatches[] = {
/* real user-supplied routines go here... */

    "help", 0, do_help, NULLIFP, NULLIFP, NULLIFP,
	"print this information",
    "quit", 0, do_quit, NULLIFP, NULLIFP, NULLIFP,
	"terminate the association and exit",

    NULL
};


/* ARGSUSED */

static int  do_help (sd, ds, args, dummy)
int	sd;
register struct dispatch *ds;
char  **args;
caddr_t *dummy;
{
    printf ("\nCommands are:\n");
    for (ds = dispatches; ds -> ds_name; ds++)
	printf ("%s\t%s\n", ds -> ds_name, ds -> ds_help);

    return NOTOK;
}


/* ARGSUSED */

static int  do_quit (sd, ds, args, dummy)
int	sd;
struct dispatch *ds;
char  **args;
caddr_t *dummy;
{
    struct AcSAPrelease acrs;
    register struct AcSAPrelease   *acr = &acrs;
    struct AcSAPindication  acis;
    register struct AcSAPindication *aci = &acis;
    register struct AcSAPabort *aca = &aci -> aci_abort;

    if (AcRelRequest (sd, ACF_NORMAL, NULLPEP, 0, NOTOK, acr, aci) == NOTOK)
	acs_adios (aca, "A-RELEASE.REQUEST");

    if (!acr -> acr_affirmative) {
	(void) AcUAbortRequest (sd, NULLPEP, 0, aci);
	adios (NULLCP, "release rejected by peer: %d", acr -> acr_reason);
    }

    ACRFREE (acr);

    exit (0);
}
