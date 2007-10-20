/* conn_abort.c - abort association */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/conn_abort.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/conn_abort.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: conn_abort.c,v $
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


/* LINTLIBRARY */

#include "quipu/dsap.h"
#include "quipu/util.h"
#include "quipu/connection.h"
#include "tpkt.h"

extern  LLog    * log_dsap;

struct connection	* conn_alloc();
void			  conn_free();
void			  ds_log ();

net_send_abort(conn)
register        struct connection       * conn;
{
    int				  result;
    struct DSAPindication      di_s;
    struct DSAPindication      *di = &di_s;

    DLOG(log_dsap, LLOG_NOTICE, ("D-ABORT.REQUEST: <%d>", conn->cn_ad));

    result = DUAbortRequest(conn->cn_ad, di);

    if (result != OK)
	force_close (conn->cn_ad,di);	/* Need a transport close really */

    conn->cn_state = CN_FAILED;
    conn->cn_ad = NOTOK;
}

force_close (fd, di)
int fd;
struct DSAPindication	* di;
{
struct tsapblk *tb;
int level = LLOG_EXCEPTIONS;

	if (fd == NOTOK)
	    return;

	if (di)
		ds_log (&di->di_abort,"ABORT",fd);

	/* Something has gone wrong on this fd, and all attempts
	   at a protocol reject have failed.
	   Rip out the connection the best you can.
	   If there is not a tblk - TLI could be a problem.
	*/

	if ((tb = findtblk (fd)) != (struct tsapblk *) NULL) {
		if ((*tb -> tb_closefnx) (tb -> tb_fd) == NOTOK) {
		        if (errno == EBADF) level = LLOG_NOTICE;
			LLOG (log_dsap, level,
			      ("force_close failed %d (1): %d",
			       tb -> tb_fd, errno ));

			if ( errno != EBADF && close (fd) == NOTOK) {
			    if (errno == EBADF) level = LLOG_NOTICE;
			    LLOG (log_dsap, level,
				  ("force_close failed %d (2): %d",
				   fd, errno ));
			}
		}
		tb -> tb_fd = NOTOK;

	} else if (close (fd) == NOTOK) {
	         if (errno == EBADF) level = LLOG_NOTICE;
		 LLOG (log_dsap, level,
		       ("force_close failed %d (3): %d", fd, errno ));
	     }
}

/* ARGSUSED */

void	  ds_log (da, str, fd)
struct DSAPabort	* da;
char			* str;
int 			fd;
{
char buffer [BUFSIZ];
char * source;
char * reason;

	switch (da->da_source) {
	    case DA_USER:	source = "user"; break;
	    case DA_PROVIDER:	source = "provider"; break;
	    case DA_LOCAL:	source = "local"; break;
	    default:		source = "?"; break;
	}

	switch (da->da_reason) {
	    case DA_NO_REASON:	reason = "No Reason"; 		break;
	    case DA_RO_BIND:	reason = "RO BIND"; 		break;
	    case DA_ROS:	reason = "ROS"; 		break;
	    case DA_ARG_ENC:	reason = "encoding argument"; 	break;
	    case DA_RES_ENC:	reason = "encoding result"; 	break;
	    case DA_ERR_ENC:	reason = "encoding error"; 	break;
	    case DA_ARG_DEC:	reason = "decoding argument"; 	break;
	    case DA_RES_DEC:	reason = "decoding result"; 	break;
	    case DA_ERR_DEC:	reason = "decoding error"; 	break;
	    case DA_APP_CONTEXT:
		reason = "Unexpected application context"; 	break;
	    case DA_PCDL:
		reason = "Unacceptable presentation context list"; break;
	    default:		reason = "?"; 			break;
	    }

	if (da->da_cc)	   
		(void) sprintf (buffer,"DSAP %s abort (%d), %s (%s): %s",
			source, fd, str, reason, da->da_data);
	else
		(void) sprintf (buffer,"DSAP %s abort (%d), %s (%s)",
			source, fd, str, reason);
			
	LLOG (log_dsap, LLOG_EXCEPTIONS, ("%s", buffer));
	
}

