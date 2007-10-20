/* conn_finish.c - deal with request to finish the association */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/conn_finish.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/RCS/conn_finish.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: conn_finish.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
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


/* LINTLIBRARY */

#include "quipu/dsap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

extern  LLog    * log_dsap;

/* ARGSUSED */
conn_finish(conn, df)
struct connection	* conn;
struct DSAPfinish	* df;
{
    int			  result;
    struct oper_act	* on;
    extern time_t	  conn_timeout, timenow;
    struct DSAPindication	  di_s;
    struct DSAPindication	* di = &(di_s);

    DLOG(log_dsap, LLOG_TRACE, ("conn_finish()"));

    /* Can release be negotiated? */
    if (conn->cn_start.cs_ds.ds_start.acs_start.ps_srequirements & SR_NEGOTIATED)
    {
	/* Should release be rejected? */
        for(on=conn->cn_operlist; on!=NULLOPER; on=on->on_next_conn)
	    if (on->on_state == ON_CHAINED)
		break;

        if (on != NULLOPER)
	{
	    /*
	    * See if oper has had time to complete
	    * if so remote DSA has probably lost the operation (never !!!)
	    * else reject the release
	    */

	    if ( timenow - conn->cn_last_used < conn_timeout)
	    {
		result = DUnBindReject (conn->cn_ad, ACS_REJECT,
			    ACR_NOTFINISHED, di);

		if (result != OK)
		{
		    result = DUAbortRequest (conn->cn_ad, di);
		    if (result != OK)
			    force_close (conn->cn_ad, di);

		    do_ds_unbind(conn);
		    conn_extract(conn);
	    	} 
	        return;
	    }
	}
    }

    result = DUnBindAccept (conn->cn_ad, di);
    if (result != OK) {
	    result = DUAbortRequest (conn->cn_ad, di);
	    if (result != OK)
		    force_close (conn->cn_ad, di);
    }
    do_ds_unbind(conn);
    conn_extract(conn);

}

conn_rel_abort (conn)
struct connection       * conn;
{
    struct DSAPindication      di_s;
    struct DSAPindication      *di = &di_s;

#ifdef notanymore  /* Just get rid of it! */
	if (!conn->cn_initiator)
		return;
#endif

	LLOG(log_dsap, LLOG_NOTICE, ("conn_rel_abort %d",conn->cn_ad));

	if ( DUAbortRequest (conn->cn_ad, di) != OK)
		force_close (conn->cn_ad, di);
}

