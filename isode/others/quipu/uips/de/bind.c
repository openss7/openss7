/* template.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/de/RCS/bind.c,v 9.0 1992/06/16 12:45:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/bind.c,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: bind.c,v $
 * Revision 9.0  1992/06/16  12:45:59  isode
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


/*****************************************************************************

  bind.c -

*****************************************************************************/

#include <stdio.h>
#include "bind.h"
#include "util.h"
#include "quipu/common.h"
#include "quipu/bind.h"
#include "quipu/dap2.h"
#include "logger.h"

extern char *dsa_address;
extern char *backup_dsa_address;
extern char *username;
extern char callingDteNumber[];

extern int boundToDSA;
extern int deLogLevel;

extern LLog *de_log;

/* this is not the way to do it */
extern int dsap_ad;

struct DAPconnect         dc;
struct DAPindication      di;

static int bindres;
static int triedBackup = FALSE;
static int bindStarted = FALSE;
static int boundOnce = FALSE;

int tryBackup()
{

  if (triedBackup == TRUE)
    return NOTOK;
  if (backup_dsa_address == NULLCP)
    return NOTOK;
  else
  {
    free(dsa_address);
    dsa_address = copy_string(backup_dsa_address);
    triedBackup = TRUE;
    return OK;
  }
}

/*
 * bind_to_ds - Bind to directory
 *
 */
int init_bind_to_ds(assoc)
int * assoc;
{
struct ds_bind_arg bind_arg;
struct PSAPaddr             *addr;
void exit();

  if (dsa_address == NULLCP)
  {
    (void) fprintf(stderr, "No dsa address has been configured in dsaptailor or detailor\n\n");
    exit(-1);
  }

  if (triedBackup == FALSE)
    if (boundOnce == FALSE)
      (void)printf("Connecting to the Directory - wait just a moment please ...\n");

  bind_arg.dba_version = DBA_VERSION_V1988;
  bind_arg.dba_auth_type = DBA_AUTH_SIMPLE;

  bind_arg.dba_dn = str2dn(username);

  bind_arg.dba_passwd[0] = '\0';
  bind_arg.dba_passwd_len = 0;

try_bind:

  if ((addr = str2paddr (dsa_address)) == NULLPA) {
    (void) fprintf(stderr, "DSA address format problem\n");
    exit(-1);
  }
  bindres = DapAsynBindRequest(addr, &bind_arg, &dc, &di, ROS_ASYNC);
  if (bindres == NOTOK)
  {
    (void) fprintf(stderr, "\n\nDirectory server temporarily unavailable\n\n");
    if (tryBackup == OK)
    {
      (void) fprintf(stderr, "Trying another server ...\n\n");
      goto try_bind;
    }
    return NOTOK;
  }
  *assoc = dc.dc_sd;
  dsap_ad = dc.dc_sd;
  return OK;
}


int wait_bind_to_ds(assoc, wantToBlock)
int assoc;
int wantToBlock;
{
  struct PSAPindication   pi_s;
  struct PSAPindication   * pi = &(pi_s);
  int     nfds, nevents;
  fd_set  rfds;
  fd_set  wfds;


  while ((bindres == CONNECTING_1) || (bindres == CONNECTING_2)) 
  {

    FD_ZERO (&rfds);
    FD_ZERO (&wfds);

    if ((bindres == CONNECTING_2) && (wantToBlock == FALSE))
      return CONNECTING_2;

    if (bindres == CONNECTING_1) 
    {
      if (PSelectMask (assoc, &wfds, &nfds, pi) == NOTOK) 
      {
         (void) fprintf(stderr, "PSelectMask (write) failed\n");
         return NOTOK;
      }
    }

    if (bindres == CONNECTING_2) 
    {
      if (PSelectMask (assoc, &rfds, &nfds, pi) == NOTOK) 
      {
        (void) fprintf(stderr, "PSelectMask (read) failed\n");
        return NOTOK;
      }
    }

    nevents = xselect (nfds, &rfds, &wfds, NULLFD, NOTOK);
  
    if (nevents == NOTOK) 
    {
      (void) fprintf(stderr, "xselect failed\n");
      return NOTOK;
    } 
    else 
    {
      /*
       * Hack coming up: nevents is number of events, we only asked for
       * one event, so unless there are no events we go ahead without
       * checking those horrid fd things, which would be safer.
       */
       if (nevents != OK) 
         bindres = DapAsynBindRetry(assoc, 0, &dc, &di);
    }
  
    if (bindres == DONE)
    {
      if (dc.dc_result != DC_RESULT)
      {
        if ((dc.dc_un.dc_bind_err.dbe_type == DBE_TYPE_SECURITY) &&
	    (dc.dc_un.dc_bind_err.dbe_value == DSE_SC_INVALIDCREDENTIALS))
          (void)fprintf(stderr, "\nBind error - check the username in the detailor file\n");
        else
	  (void)fprintf(stderr, "\nBind error - type %d, code %d\n", 
	    dc.dc_un.dc_bind_err.dbe_type, dc.dc_un.dc_bind_err.dbe_value);
        return NOTOK;
      }
      boundToDSA = TRUE;
      boundOnce = TRUE;
      if (deLogLevel)
        (void) ll_log (de_log, LLOG_NOTICE, NULLCP, "Bound: %s", callingDteNumber);
      return OK;
    }
  }
  if (tryBackup() == OK)
    if (init_bind_to_ds(&assoc) == OK)
      return OK;
  (void)fprintf(stderr, "\nCouldn't bind to the DSA - probably something wrong with the DSA address\n\n");
  return NOTOK; /* exit this way if can't talk to access point(s) */
} /* bind_to_ds */

int
rebind()
{
  if (boundToDSA == FALSE)
    return(de_bind(TRUE));
  else
    return OK;
}

int 
de_bind(wantToBlock)
int wantToBlock;
{
static int assoc;

  if (bindStarted == FALSE)
  {
    if (init_bind_to_ds(&assoc) != OK)
      return NOTOK;
    bindStarted = TRUE;
    if (wantToBlock == FALSE)
      return OK;
  }
  if (wait_bind_to_ds(assoc, wantToBlock) == NOTOK) /* don`t block */
    return NOTOK;
  return OK;
}

de_unbind()
{
  if (deLogLevel)
    (void) ll_log (de_log, LLOG_NOTICE, NULLCP, "Unbind:");
  (void) ds_unbind();
  boundToDSA = FALSE;
  bindStarted = FALSE;
}
