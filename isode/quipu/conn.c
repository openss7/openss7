/* conn.c - */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/conn.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/conn.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: conn.c,v $
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

#include "quipu/dsap.h"
#include "quipu/util.h"
#include "quipu/connection.h"

extern LLog * log_dsap;

struct connection	* conn_alloc()
{
    struct connection * conn_ret;

    conn_ret = (struct connection *) calloc(1,sizeof(struct connection));
    conn_ret->cn_op_id = 1;

    return(conn_ret);
}

conn_free(conn)
struct connection	* conn;
{
	DLOG(log_dsap, LLOG_TRACE, ("conn_free()"));

    if(conn->cn_dn != NULLDN)
    {
      dn_free(conn->cn_dn);
      conn->cn_dn = NULLDN ;
    }

    if (conn->cn_initiator)
    {
	conn_connect_free (&(conn->cn_connect));
    }
    else
    {
	conn_start_free (&(conn->cn_start));
    }

    check_getedb_ops(conn->cn_ad);

    free((char *)conn);
}

conn_connect_free (cc)
struct conn_connect	* cc;
{
    bind_arg_free (&(cc->cc_req));

    /* cc_dc should not be freed before calling conn_free() */
    DCFREE (&(cc->cc_dc));
}

conn_start_free (cs)
struct conn_start	* cs;
{
    if (cs->cs_svec[0])
	free (cs->cs_svec[0]);
    if (cs->cs_svec[1])
	free (cs->cs_svec[1]);
    if (cs->cs_svec[2])
	free (cs->cs_svec[2]);
    if (cs->cs_svec[3])
	free (cs->cs_svec[3]);

    bind_arg_free (&(cs->cs_res));	

    /* cs_ds should not be freed before calling conn_free() */
    DSFREE (&(cs->cs_ds));
}

conn_extract(conn)
struct connection	* conn;
{
    /*
    * Extract all the operations made on this connection, and all
    * the tasks (and their derivative operations) made on the connection;
    * then remove the connection from the list of active connections.
    */

    struct oper_act     * on;
    struct oper_act     * on_next;
    struct task_act     * tk;
    struct task_act     * tk_next;
    struct connection	* cn;
    struct connection	**cn_p;
    struct DSError	* err;

    DLOG (log_dsap,LLOG_TRACE, ("conn_extract"));

#ifdef QUIPU_CONSOLE
    /* SPT: Catch this close and indicate it on the open_calls */
    closing_analyse(conn) ;
#endif /* QUIPU_CONSOLE */

    if(conn == NULLCONN)
    {
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("Extracting NULLCONN!!!"));
	return;
    }

    cn_p = &(connlist);
    for(cn=connlist; cn!=NULLCONN; cn=cn->cn_next)
    {
	DLOG(log_dsap, LLOG_DEBUG, ("checking connlist"));
	if(cn == conn)
	    break;

	cn_p = &(cn->cn_next);
    }
    if(cn==NULLCONN)
    {
	LLOG(log_dsap, LLOG_EXCEPTIONS, ("conn_extract - connection not in connlist"));
	return;	
    }
    else
    {
	/* Cut connection loose from global list */
	DLOG(log_dsap, LLOG_DEBUG, ("Extracting conn from connlist"));
	(*cn_p) = cn->cn_next;
	conns_used--;
    }

    for(on=conn->cn_operlist; on!=NULLOPER; on=on_next)
    {
	on_next = on->on_next_conn;
	oper_fail_wakeup (on);
    }

    for(tk=conn->cn_tasklist; tk!=NULLTASK; tk=tk_next)
    {
	    tk_next = tk->tk_next;
	    err = &(tk->tk_resp.di_error.de_err);
	    if((err->dse_type != DSE_REFERRAL) && (err->dse_type != DSE_DSAREFERRAL))
	    {
		err->dse_type = DSE_SERVICEERROR;
		err->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNAVAILABLE;
	    }
	    task_error(tk);
	    task_extract(tk);
    }

    conn_free(conn);
}

conn_log(conn,level)
struct connection       * conn;
int level;
{
    struct oper_act     * oper;
    struct task_act     * task;
    char * cntxt;
    char * state;

    if(conn == NULLCONN)
    {
	LLOG (log_dsap,level, ("Connection: NULLCONN"));
	return;
    }

    switch (conn->cn_ctx) {
	case DS_CTX_X500_DAP:		cntxt = "DAP"; 	break;
	case DS_CTX_X500_DSP:		cntxt = "DSP"; 	break;
	case DS_CTX_QUIPU_DSP:		cntxt = "QDSP";	break;
	case DS_CTX_INTERNET_DSP: 	cntxt = "IDSP"; break;
	default:			cntxt = "?";	break;
    }

    switch(conn->cn_state)
    {
	case CN_INDICATED:	state = "INDICATED";	break;
	case CN_WAITING:	state = "WAITING";	break;
	case CN_CONNECTING1:	state = "CONNECTING 1";	break;
	case CN_CONNECTING2:	state = "CONNECTING 2";	break;
	case CN_OPEN:		state = "OPEN";		break;
	case CN_FAILED:		state = "FAIL";		break;
	case CN_CLOSING:	state = "CLOSING";	break;
	case CN_OPENING:	state = "OPENING";	break;
	case CN_PRE_OPENING:	state = "PRE OPENING";	break;
	default:
	    state = "?";
	    return;
    }

    LLOG (log_dsap,level, 
	  ("Connection %d, %s %s", conn->cn_ad, cntxt, state));

    if (conn->cn_initiator) 
	LLOG (log_dsap, level, ("To: %s", 
	    paddr2str(&conn->cn_addr,NULLNA)));
    else
        LLOG (log_dsap, level, ("From: %s", 
	    paddr2str(&conn->cn_start.cs_ds.ds_start.acs_start.ps_calling,
		NULLNA)));

#ifdef DEBUG
    if (conn->cn_tasklist != NULLTASK) {
	    DLOG (log_dsap,level, ("Tasks:"));
	    for(task=conn->cn_tasklist; task!=NULLTASK; task=task->tk_next)
		    task_log(task,level);
    }
    if (conn->cn_operlist != NULLOPER) {
	    DLOG (log_dsap,level, ("Opers:"));
	    for(oper=conn->cn_operlist; oper!=NULLOPER; 
		oper=oper->on_next_conn)
		    oper_log(oper,level);
    }
#endif
}

#ifdef DEBUG
conn_list_log(cn)
struct connection	* cn;
{
    struct connection	* cn_tmp;

    DLOG(log_dsap, LLOG_DEBUG, ("Connection List:"));
    for(cn_tmp=cn; cn_tmp!=NULLCONN; cn_tmp=cn_tmp->cn_next)
    {
	conn_log(cn_tmp, LLOG_DEBUG);
    }
    DLOG(log_dsap, LLOG_DEBUG, ("End of Connection List."));
}
#endif
