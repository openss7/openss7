/* security.c - Check security parameters */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/security.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/security.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: security.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include "logger.h"
#include "quipu/ds_error.h"
#include "quipu/commonarg.h"
#include "pepsy.h"
#include "quipu/AF-types.h"
#include "osisec-stub.h"

extern LLog *log_dsap;
#ifndef NO_STATS
extern LLog *log_stat;
#endif

#define adios(a, b) fatal(-1, b)

char *new_version();

/* 
 * Function pointers to security routines
 */

struct SecurityServices *dsap_security = (struct SecurityServices *) 0;

/*
 * Check security parameters - return 0 or the number of the security error.
 */

/* ARGSUSED */
int check_security_parms(data, type, module, sp, sig, nameptr)
caddr_t data;
int type;
modtyp *module;
struct security_parms *sp;
struct signature *sig;
DN *nameptr;
{
long time_now;
long time_then;
long delta;

  /* If parameters are present, they must be valid */

  if (sp != (struct security_parms *) 0)
  {
    if (sp->sp_time != NULLCP)
    {
      (void) time(&time_now);
      time_then = gtime(ut2tm(str2utct(sp->sp_time, strlen(sp->sp_time))));
      delta = time_now - time_then;
    }
    else
      delta = 0L;

#ifndef NO_STATS
    DLOG(log_dsap, LLOG_NOTICE, 
	("Delay=%D s, protection%s requested, certificate%s present",
		delta, 
		(sp->sp_target == '\0') ? " not" : "",
		(sp->sp_path == (struct certificate_list *) 0) ? " not" : "" 
		));
   /* NB : must use "" rather than NULLCP for the above to work. */
#endif
   }

/* If no signature is provided, nothing else to do */

  if (sig == (struct signature *) 0)
	return (0);

#ifndef NO_STATS
    DLOG(log_dsap, LLOG_NOTICE, ("Operation is signed"));
#endif

/* Policy : signed messages must have security parameters present. */
  if (sp == (struct security_parms *) 0)
  {
#ifndef NO_STATS
    DLOG(log_dsap, LLOG_EXCEPTIONS, ("No security parameters present"));
#endif
    return (DSE_SC_INVALIDCREDENTIALS);
  }

/* Policy: signed messages must have a time-stamp. */
  if (sp->sp_time == NULLCP)
  {
#ifndef NO_STATS
    DLOG(log_dsap, LLOG_EXCEPTIONS, ("No time-stamp present"));
#endif
    return (DSE_SC_INVALIDCREDENTIALS);
  }

/* Policy: a certification path must be provided. */
  if (sp->sp_path == (struct certificate_list *) 0)
  {
#ifndef NO_STATS
    DLOG(log_dsap, LLOG_EXCEPTIONS, ("No certification path"));
#endif
    return (DSE_SC_INVALIDCREDENTIALS);
  }

  if (dsap_security->serv_ckpath)
      return (dsap_security->serv_ckpath)(data, type, module, 
	sp->sp_path, sig, nameptr);
  else
     return (DSE_SV_UNAVAILABLE);
}

