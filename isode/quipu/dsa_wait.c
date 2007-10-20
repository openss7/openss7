/* dsa_wait.c - accept and process events listened for */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/dsa_wait.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/dsa_wait.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: dsa_wait.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include "rosap.h"
#include "tsap.h"

#ifdef QUIPU_CONSOLE
#include "psap.h"             /* Brought in by SPT for the UTCTime stuff... */
#endif /* QUIPU_CONSOLE */

#include "quipu/util.h"
#include "quipu/connection.h"
#include <signal.h>

extern unsigned watchdog_time;
extern unsigned watchdog_delta;

#ifdef QUIPU_CONSOLE
AV_Sequence open_call_avs ;   /* SPT: This sequence holds all the 
				 current open calls. */
extern PS opt ;
static void print_connlist() ;
       void opening_analyse() ;
static void connecting_analyse() ;
void closing_analyse() ;

#endif /* QUIPU_CONSOLE */

extern LLog * log_dsap;
extern LLog * tsap_log;

time_t	timenow;

dsa_wait(secs)
int	  secs;
{
    int                         vecp = 0;
    char                        *vec[4];
    fd_set                      iads;
    fd_set                      wads;
    int                         nads;
    struct TSAPdisconnect       td_s;
    struct TSAPdisconnect       *td = &td_s;
    struct connection		* cn;
    struct connection		* next_cn;
    char			ibuffer[BUFSIZ];
    char			*ibp;
    char			wbuffer[BUFSIZ];
    char			*wbp;
    SFD 			attempt_restart();
    int				newfd;
    int				result = NOTOK;

    nads = 0;
    FD_ZERO(&iads);
    FD_ZERO(&wads);
    ibp = ibuffer;
    wbp = wbuffer;

#ifdef QUIPU_CONSOLE
#ifdef DEBUG
    DLOG(log_dsap, LLOG_DEBUG, ("dsa_wait connections:"));
    conn_list_log(connlist);
#endif /* DEBUG */
#endif /* QUIPU_CONSOLE */

    for(cn=connlist; cn != NULLCONN; cn=cn->cn_next)
    {
	if (cn->cn_state == CN_CONNECTING1)
	{
	    if (cn->cn_ad > 0)
		    FD_SET(cn->cn_ad, &wads);
	    else
		SLOG (tsap_log, LLOG_EXCEPTIONS, NULLCP,
		      ("fd=%d for connection block(1)", cn -> cn_ad));
	    (void) sprintf(wbp, ", %d.", cn->cn_ad);
	    wbp += (strlen(wbp) - 1);
	}
	else
	{
	    if (cn->cn_ad > 0)
		    FD_SET(cn->cn_ad, &iads);
	    else
		SLOG (tsap_log, LLOG_EXCEPTIONS, NULLCP,
		      ("fd=%d for connection block(2)", cn -> cn_ad));
	    (void) sprintf(ibp, ", %d.", cn->cn_ad);
	    ibp += (strlen(ibp) - 1);
	}

	if(cn->cn_ad >= nads)
	    nads = cn->cn_ad + 1;
    }

    if(ibp == ibuffer)
    {
	DLOG (log_dsap, LLOG_DEBUG, ("Listening for new associations"));
    }
    else
    {
	LLOG (log_dsap, LLOG_TRACE, ("Listening on ads: %s", (ibuffer+1)));
    }

    if(wbp == wbuffer)
    {
	DLOG (log_dsap, LLOG_DEBUG, ("Not making new associations"));
    }
    else
    {
	LLOG (log_dsap, LLOG_TRACE, ("Making ads: %s", (wbuffer+1)));
    }

    DLOG (log_dsap, LLOG_TRACE, ("secs: %d; nads: %d; iads 0x%x, wads 0x%x", 
	secs, nads, iads.fds_bits[0], wads.fds_bits[0]));

    if (secs != NOTOK) {
	/* if secs == NOTOK we want to block, otherwise set watchdog, but
	   beware of setting watchdog off accidentally !
	*/
	if (secs > (watchdog_time - watchdog_delta))
		watch_dog_aux ("TNetAccept (long)",(unsigned)secs + watchdog_delta);
	else
		watch_dog ("TNetAccept");
    }

    if(TNetAcceptAux(&vecp, vec, &newfd, NULLTA, nads, &iads, &wads, NULLFD, secs, td) == NOTOK)
    {
	watch_dog_reset();

	td_log (td, "TNetAccept");

/*
 *	if (td -> td_reason == DR_PROTOCOL || td -> td_reason == DR_NETWORK) 
 */
		return NOTOK;
/*
 *	attempt_restart (NOTOK);
 * 	exit (0);			*/ /* should not be reached */

    }
    watch_dog_reset();

    (void) time (&timenow);

    if (vecp > 0) {
	conn_pre_init (newfd,vecp,vec);
	result = OK;
    } else if (newfd != NOTOK) {
	warn_conn_init (newfd);
	result = OK;
    }

    for(cn = connlist; cn != NULLCONN; cn = next_cn)
    {
	next_cn = cn->cn_next;

	switch(cn->cn_state)
	{
	case CN_CONNECTING1:
	    DLOG(log_dsap, LLOG_TRACE, ("Checking %d", cn->cn_ad));
	    if(FD_ISSET(cn->cn_ad, &wads))
	    {
	        DLOG(log_dsap, LLOG_DEBUG, ("Polling %d", cn->cn_ad));

#ifdef QUIPU_CONSOLE
		/* We must analyse first before calling conn_retry */
		/* as there is a disasterous cn_free otherwise! */
		connecting_analyse(cn) ;
#endif /* QUIPU_CONSOLE */

	        conn_retry(cn,0);
		result = OK;
	    }
	break;

	case CN_CONNECTING2:
	    DLOG (log_dsap, LLOG_TRACE, ("Checking %d (2)", cn ->cn_ad));
	    if (FD_ISSET(cn->cn_ad, &iads))
	    {
		DLOG(log_dsap, LLOG_DEBUG, ("Polling %d (2)", cn->cn_ad));

#ifdef QUIPU_CONSOLE
		/* We must analyse first before calling conn_retry */
		/* as there is a disasterous cn_free otherwise! */
		connecting_analyse(cn) ;
#endif /* QUIPU_CONSOLE */

		conn_retry(cn,0);
		result = OK;
	    }
	break;

	case CN_OPEN:
	    if (FD_ISSET (cn->cn_ad, &iads))
	    {
		DLOG (log_dsap,LLOG_DEBUG,( "Activity on association: %d", cn->cn_ad));
		conn_dispatch(cn);
		result = OK;
	    } /* if there is work on this connection */
	break;

	case CN_CLOSING:
	    if (FD_ISSET (cn->cn_ad, &iads)) {

#ifdef QUIPU_CONSOLE
	        closing_analyse(cn) ;
#endif /* QUIPU_CONSOLE */

		(void) conn_release_retry(cn);
		result = OK;
	    }
	break;

	case CN_OPENING:
	    if (FD_ISSET (cn->cn_ad, &iads)) {
		conn_init(cn);
		result = OK;
	    }
	break;

	case CN_PRE_OPENING:
	    if (FD_ISSET (cn->cn_ad, &iads))
	    {
		LLOG (log_dsap,LLOG_EXCEPTIONS,
		("Unexpected activity on pre-open association %d ... aborting",
		      cn->cn_ad));
		force_close(cn->cn_ad, (struct DSAPindication *)NULL);
		conn_extract(cn);
		result = OK;
	    } /* if there is work on this connection */
	break;

	case CN_INDICATED:
	    if (FD_ISSET (cn->cn_ad, &iads))
	    {
		if(cn->cn_start.cs_bind_compare == NULLOPER)
		{
		    LLOG(log_dsap, LLOG_EXCEPTIONS, ("cn_state = INDICATED but no bind_compare operation"));
		}
		else
		{
		    cn->cn_start.cs_bind_compare->on_bind_compare = NULLCONN;
		}
	    }
	    /* FALL THROUGH */
	
	default:
	    if (FD_ISSET (cn->cn_ad, &iads))
	    {
		LLOG (log_dsap,LLOG_EXCEPTIONS,( "Unexpected activity on association %d ... aborting",cn->cn_ad));
#ifdef DEBUG
		conn_log(cn,LLOG_EXCEPTIONS);
#endif
		net_send_abort(cn);
		conn_extract(cn);
		result = OK;
	    } /* if there is work on this connection */
	break;
	}
    } /* for each connection */

    return result;

} /* dsa_wait */



#ifdef QUIPU_CONSOLE
static void
connecting_analyse(cn)
struct connection *cn ;
{
/* SPT: The sub parts to build an openCall attribute type. */
  AttributeType oc_att ;
  AttributeValue oc_av ;
  AV_Sequence tmp_avs ;
  struct quipu_call * open_call ;
  struct quipu_call * tmp_oc ;
  UTCtime ut ;

  /* First scan through the open_call_avs to see if this connection */
  /* has already been made. If so, then do not add it again. */
  /* Criteria: Same call number, not finished, same DN as a check. */
  
  tmp_avs = open_call_avs ;
  while(tmp_avs != NULLAV)
  {
    tmp_oc = ((struct quipu_call *)tmp_avs->avseq_av.av_struct) ;
    if (( tmp_oc->assoc_id == cn->cn_ad) &&
	( tmp_oc->finish_time == (char *) 0))
    {
      if ( dn_cmp(tmp_oc->usersDN, cn->cn_dn) == 0)
      {
	fprintf(stderr, "Double Success!\n") ;
	return ;
      }
      fprintf(stderr, "Single Success!\n") ;
      return ;
    }
    tmp_avs = tmp_avs->avseq_next ;
  }

  oc_att = AttrT_new("openCall") ;

#ifdef SPT_DEBUG
  fprintf(stderr, "Ping!!! should have added a connecting_analyse call!\n") ;
#endif
  
  /* Fill out the open_call structure before merging it in. */
  open_call = (struct quipu_call *) calloc (1, sizeof(struct quipu_call)) ;
  switch (cn->cn_ctx)
  {
    case(DS_CTX_X500_DAP):
    {
      open_call->protocol = PROTOCOL_DAP ;
      break ;
    }
    case(DS_CTX_X500_DSP):
    {
      open_call->protocol = PROTOCOL_DSP ;
      break ;
    }
    case(DS_CTX_QUIPU_DSP):
    {
      open_call->protocol = PROTOCOL_QUIPUDSP ;
      break ;
    }
    case(DS_CTX_INTERNET_DSP):
    {
      open_call->protocol = PROTOCOL_INTERNET_DSP ;
      break ;
    }
    default:
    {
      open_call->protocol = -1 ;
      break ;
    }
  }
  open_call->assoc_id = cn->cn_ad ;
  open_call->authorizationLevel = (struct auth_level *) malloc (sizeof (struct auth_level)) ;
  open_call->initiated_by_dsa = cn->cn_initiator ;
  open_call->usersDN = dn_cpy(cn->cn_dn);
  open_call->net_address = strdup(pa2str(cn->cn_start.cs_ds.ds_start.acs_start.ps_calling)) ;
  (void) time(&timenow) ;
  tm2ut(gmtime(&timenow), &ut) ;
  open_call->start_time = strdup(utct2str(&ut)) ;
  open_call->finish_time = (char *) 0 ;

  switch (cn->cn_authen)
  {
    case(DBA_AUTH_NONE):
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_NONE ;
      break ;
    }
    case(DBA_AUTH_SIMPLE):
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_SIMPLE ;
      break ;
    }
    case(DBA_AUTH_STRONG):
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_STRONG ;
      break ;
    }
    case(DBA_AUTH_PROTECTED):
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_PROTECTED_SIMPLE ;
      break ;
    }
    case(DBA_AUTH_EXTERNAL):
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_IDENTIFIED ;
      break ;
    }
    default:
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_NONE ;
    }
  }
  
  /* Now merge in the "struct quipu_call" with the rest in open_call_avs */
  oc_av = AttrV_alloc() ;
  oc_av->av_syntax = oc_att->oa_syntax ;
  oc_av->av_struct = (caddr_t) open_call ;
  if (open_call_avs == NULLAV)
  {
    open_call_avs = avs_comp_new(oc_av) ;
  }
  else
  {
    avs_merge(open_call_avs, avs_comp_new(oc_av)) ;
  }
}

void
opening_analyse(cn)
struct connection *cn ;
{
/* SPT: The sub parts to build an openCall attribute type. */
  AttributeType oc_att ;
  AttributeValue oc_av ;
  AV_Sequence tmp_avs ;
  struct quipu_call * open_call ;
  struct quipu_call * tmp_oc ;
  UTCtime ut ;

  tmp_avs = open_call_avs ;

  while (tmp_avs != NULLAV)
  {
    tmp_oc = (struct quipu_call *)(tmp_avs->avseq_av.av_struct) ;
    if ((tmp_oc->assoc_id == cn->cn_ad) &&
	(tmp_oc->finish_time == (char *) 0))
    {
#ifdef SPT_DEBUG
  fprintf(stderr, "Attention. A call is being added when assoc is still open. Why?\n") ;
#endif
      return ;
    }
    tmp_avs = tmp_avs->avseq_next ;
  }

  oc_att = AttrT_new("openCall") ;
#ifdef SPT_DEBUG
  fprintf(stderr, "Ping!!! should have added a opening_analyse call!\n") ;
#endif
  
  /* Fill out the open_call structure before merging it in. */
  open_call = (struct quipu_call *) calloc (1, sizeof(struct quipu_call)) ;
  switch (cn->cn_ctx)
  {
    case(DS_CTX_X500_DAP):
    {
      open_call->protocol = PROTOCOL_DAP ;
      break ;
    }
    case(DS_CTX_X500_DSP):
    {
      open_call->protocol = PROTOCOL_DSP ;
      break ;
    }
    case(DS_CTX_QUIPU_DSP):
    {
      open_call->protocol = PROTOCOL_QUIPUDSP ;
      break ;
    }
    case(DS_CTX_INTERNET_DSP):
    {
      open_call->protocol = PROTOCOL_INTERNET_DSP ;
      break ;
    }
    default:
    {
      open_call->protocol = -1 ;
      break ;
    }
  }
  open_call->assoc_id = cn->cn_ad ;
  open_call->authorizationLevel = (struct auth_level *) malloc (sizeof (struct auth_level)) ;
  open_call->initiated_by_dsa = cn->cn_initiator ;
  open_call->usersDN = dn_cpy(cn->cn_dn);
  open_call->net_address = strdup(pa2str(cn->cn_start.cs_ds.ds_start.acs_start.ps_calling)) ;
  tm2ut(gmtime(&timenow), &ut) ;
  open_call->start_time = strdup(utct2str(&ut)) ;
  open_call->finish_time = (char *) 0 ;

  switch (cn->cn_authen)
  {
    case(DBA_AUTH_NONE):
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_NONE ;
      break ;
    }
    case(DBA_AUTH_SIMPLE):
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_SIMPLE ;
      break ;
    }
    case(DBA_AUTH_STRONG):
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_STRONG ;
      break ;
    }
    case(DBA_AUTH_PROTECTED):
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_PROTECTED_SIMPLE ;
      break ;
    }
    case(DBA_AUTH_EXTERNAL):
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_IDENTIFIED ;
      break ;
    }
    default:
    {
      open_call->authorizationLevel->parm = AUTHLEVEL_NONE ;
    }
  }
  
  /* Now merge in the "struct quipu_call" with the rest in open_call_avs */
  oc_av = AttrV_alloc() ;
  oc_av->av_syntax = oc_att->oa_syntax ;
  oc_av->av_struct = (caddr_t) open_call ;
  if (open_call_avs == NULLAV)
  {
    open_call_avs = avs_comp_new(oc_av) ;
  }
  else
  {
    avs_merge(open_call_avs, avs_comp_new(oc_av)) ;
  }
}

  
void
closing_analyse(cn)
struct connection *cn ;
{
  AV_Sequence tmp_avs ;
  int success = FALSE ;
  UTCtime ut ;

  /* Move along our structure until we come to one with */
  /* no finish time and the appropriate association number */

#ifdef SPT_DEBUG
  fprintf(stderr, "Ping! We have a closing_analysis to do. Port %d, %d..\n",
	  cn->cn_start.cs_ds.ds_sd, cn->cn_ad) ;
#endif

  tmp_avs = open_call_avs ;

  if (tmp_avs == (AV_Sequence) 0)
  {
#ifdef SPT_DEBUG
    fprintf(stderr, "dsa_wait.c: Hey, got an closing analyse with no calls present. Error!\n") ;
#endif
    return ;
  }

  while (tmp_avs != (AV_Sequence) 0)
  {
    if ((((struct quipu_call *)tmp_avs->avseq_av.av_struct)->assoc_id == cn->cn_start.cs_ds.ds_sd) &&
	(((struct quipu_call *)tmp_avs->avseq_av.av_struct)->finish_time == (char *) 0))
    {
      /* Same Assoc Number and the connection has not been finished! Match? */
      /* Now fill out the finish time */
      tm2ut(gmtime(&timenow), &ut) ;
      ((struct quipu_call *)tmp_avs->avseq_av.av_struct)->finish_time = strdup(utct2str(&ut)) ;
      success = TRUE ;
    }	 
    tmp_avs = tmp_avs->avseq_next ;
  }

  tmp_avs = open_call_avs ;
  while (tmp_avs != (AV_Sequence) 0)
  {
    if ((((struct quipu_call *)tmp_avs->avseq_av.av_struct)->assoc_id == cn->cn_ad) &&
	(((struct quipu_call *)tmp_avs->avseq_av.av_struct)->finish_time == (char *) 0))
    {
      /* Same Assoc Number and the connection has not been finished! Match? */
      /* Now fill out the finish time */
      tm2ut(gmtime(&timenow), &ut) ;
      ((struct quipu_call *)tmp_avs->avseq_av.av_struct)->finish_time = strdup(utct2str(&ut)) ;
      success = TRUE ;
    }	 
    tmp_avs = tmp_avs->avseq_next ;
  }
  if (success == TRUE)
    return ;

#ifdef SPT_DEBUG
    fprintf(stderr, "We have no closing match here. Odd. Check...\n") ;
#endif
}

static void
print_connlist(conn)
struct connection * conn ;
{
  fprintf(stderr, "***** SPT: ctx %d\n", conn->cn_ctx) ;
}
#endif /* QUIPU_CONSOLE */
