/* text2spkt.c - read/write a SPDU thru a debug filter */

#ifndef	lint
static char *rcsid = "$Header: /f/iso/ssap/RCS/text2spkt.c,v 5.0 88/07/21 14:58:21 mrose Rel $";
#endif

/* 
 * $Header: /f/iso/ssap/RCS/text2spkt.c,v 5.0 88/07/21 14:58:21 mrose Rel $
 *
 *
 * $Log$
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

#include <stdio.h>
#include "spkt.h"


#define	sprintc(v,b)	sprintb ((int) (v), (b))

/*  */

#define	SPDU_TYPE(e)	fprintf (fp, "%sCODE/ %s\n", rw, e)

#define	DMASK	"\020\01RELEASE\02USER\03PROTOCOL\04UNKNOWN"
#define	EMASK	"\020\01BEGIN\02END"
#define	OMASK	"\020\01EXTD"
#define	RMASK \
    "\020\01HALFDUPLEX\02DUPLEX\03EXPEDITED\04MINORSYNC\05MAJORSYNC\06RESYNC\07ACTIVITY\010NEGOTIATED\011CAPABILITY\012EXCEPTIONS\013TYPEDATA"
#define	SMASK	"\020\01NOEND"
#define	TMASK	"\020\01DATA\03SYNC\05ACTIVITY\07RELEASE"
#define	YMASK	"\020\01NOEXPLICIT"



void	spkt2text (fp, s, read)
register FILE	*fp;
register struct ssapkt *s;
int	read;
{
    char   *rw = read ? "<--- " : "---> ";

    fprintf (fp, "%s(: dump of SPDU 0x%x, errno=0x%x mask=0x%x%s\n",
	    rw, s, s -> s_errno, s -> s_mask,
	    s -> s_mask & SMASK_SPDU_EXPD ? " (expedited)" : "");

    fprintf (fp, "%sLI/ %d\n", rw, s -> s_li);

    switch (s -> s_code) {
	case SPDU_CN: 
	case SPDU_AC: 
	    SPDU_TYPE (s -> s_code == SPDU_CN ? "CONNECT" : "ACCEPT");
	    if (s -> s_mask & SMASK_CN_REF)
		type_ref (fp, rw, &s -> s_cn_reference);
	    if (s -> s_mask & SMASK_CN_OPT)
		type_bits (fp, rw, "OPTIONS", s -> s_options, CR_OPT_MASK,
			OMASK);
	    if (s -> s_mask & SMASK_CN_TSDU)
		type_tsdu (fp, rw, s -> s_tsdu_init, s -> s_tsdu_resp);
	    if (s -> s_mask & SMASK_CN_VRSN)
		type_vrsn (fp, rw, s -> s_cn_version);
	    if (s -> s_mask & SMASK_CN_ISN)
		type_ssn (fp, rw, "ISN", s -> s_isn);
	    if (s -> s_mask & SMASK_CN_SET)
		type_settings (fp, rw, s -> s_settings);
	    if (s -> s_code == SPDU_AC && (s -> s_mask & SMASK_AC_TOKEN))
		type_bits (fp, rw, "TOKENS", s -> s_ac_token, -1, TMASK);
	    if (s -> s_mask & SMASK_CN_REQ)
		type_bits (fp, rw, "REQUIREMENTS", s -> s_cn_require,
			-1, RMASK);
	    if (s -> s_mask & SMASK_CN_CALLING)
		type_id (fp, "CALLING", rw, s -> s_calling, s -> s_callinglen);
	    if (s -> s_mask & SMASK_CN_CALLED)
		type_id (fp, "CALLED", rw, s -> s_called, s -> s_calledlen);
	    break;

#ifdef HULA
	case SPDU_UD: 
	    SPDU_TYPE ("UNITDATA");
	    if (s -> s_mask & SMASK_UD_VERSION)
		type_vrsn (fp, rw, s -> s_ud_version);
	    if (s -> s_mask & SMASK_UD_CALLING)
		type_id (fp, "CALLING", rw, s -> s_ud_calling,
					    s -> s_ud_callinglen);
	    if (s -> s_mask & SMASK_UD_CALLED)
		type_id (fp, "CALLED", rw, s -> s_ud_called, 
				           s -> s_ud_calledlen);
	    break;
#endif

	case SPDU_RF: 
	    SPDU_TYPE ("REFUSE");
	    if (s -> s_mask & SMASK_RF_REF)
		type_ref (fp, rw, &s -> s_rf_reference);
	    if (s -> s_mask & SMASK_RF_DISC)
		type_bits (fp, rw, "DISCONNECT", s -> s_rf_disconnect,
			RF_DISC_MASK, DMASK);
	    if (s -> s_mask & SMASK_RF_REQ)
		type_bits (fp, rw, "REQUIREMENTS", s -> s_rf_require,
			-1, RMASK);
	    if (s -> s_mask & SMASK_RF_VRSN)
		type_vrsn (fp, rw, s -> s_rf_version);
	    if (s -> s_rlen > 0) {
		type_reason (fp, rw, *s -> s_rdata & 0xff);
		if (s -> s_rlen > 1)
		    type_data (fp, "REASON", rw, s -> s_rlen - 1,
			    s -> s_rdata + 1);
	    }
	    break;

	case SPDU_FN: 
	    SPDU_TYPE ("FINISH");
	    if (s -> s_mask & SMASK_FN_DISC)
		type_bits (fp, rw, "DISCONNECT", s -> s_fn_disconnect,
			FN_DISC_MASK, DMASK);
	    break;

	case SPDU_DN: 
	    SPDU_TYPE ("DISCONNECT");
	    break;

	case SPDU_NF: 
	    SPDU_TYPE ("NOT FINISHED");
	    break;

	case SPDU_AB: 
#ifdef	notdef
	case SPDU_AI: 		/* aka SPDU_AB */
#endif
	    if (s -> s_mask & SMASK_SPDU_AB) {
		SPDU_TYPE ("ABORT");
		if (s -> s_mask & SMASK_AB_DISC)
		    type_bits (fp, rw, "DISCONNECT", s -> s_ab_disconnect,
			    AB_DISC_MASK, DMASK);
		if (s -> s_mask & SMASK_AB_REFL)
		    type_data (fp, "REFLECT", rw, sizeof s -> s_reflect,
			    (char *) s -> s_reflect);
		break;
	    }
	    SPDU_TYPE ("ACTIVITY INTERRUPT");
	    if (s -> s_mask & SMASK_AI_REASON)
		type_error (fp, rw, s -> s_ai_reason);
	    break;

	case SPDU_AA: 
#ifdef	notdef
	case SPDU_AIA: 		/* aka SPDU_AA */
#endif
	    if (s -> s_mask & SMASK_SPDU_AA)
		SPDU_TYPE ("ABORT ACCEPT");
	    else
		SPDU_TYPE ("ACTIVITY INTERRUPT ACK");
	    break;

	case SPDU_GT: 
#ifdef	notdef
	case SPDU_DT: 		/* aka SPDU_GT */
#endif
	    if (s -> s_mask & SMASK_SPDU_GT) {
		SPDU_TYPE ("GIVE TOKENS");
		if (s -> s_mask & SMASK_GT_TOKEN)
		    type_bits (fp, rw, "TOKENS", s -> s_gt_token, -1, TMASK);
	    }
	    else
		SPDU_TYPE ("DATA TRANSFER");
	    break;

	case SPDU_EX: 
	    SPDU_TYPE ("EXPEDITED");
	    break;

	case SPDU_TD: 
	    SPDU_TYPE ("TYPED DATA");
	    break;

	case SPDU_CD: 
	    SPDU_TYPE ("CAPABILITY DATA");
	    break;

	case SPDU_CDA: 
	    SPDU_TYPE ("CAPABILITY DATA ACK");
	    break;

	case SPDU_PT: 
	    SPDU_TYPE ("PLEASE TOKENS");
	    if (s -> s_mask & SMASK_PT_TOKEN)
		type_bits (fp, rw, "TOKENS", s -> s_pt_token, -1, TMASK);
	    break;

	case SPDU_GTC: 
	    SPDU_TYPE ("GIVE TOKENS CONFIRM");
	    break;

	case SPDU_GTA: 
	    SPDU_TYPE ("GIVE TOKENS ACK");
	    break;

	case SPDU_MIP: 
	    SPDU_TYPE ("MINOR SYNCHRONIZATION POINT");
	    if (s -> s_mask & SMASK_MIP_SYNC)
		type_bits (fp, rw, "SYNC", s -> s_mip_sync, MIP_SYNC_MASK,
			YMASK);
	    if (s -> s_mask & SMASK_MIP_SERIAL)
		type_ssn (fp, rw, "SSN", s -> s_mip_serial);
	    break;

	case SPDU_MIA: 
	    SPDU_TYPE ("MINOR SYNC ACK");
	    if (s -> s_mask & SMASK_MIA_SERIAL)
		type_ssn (fp, rw, "SSN", s -> s_mia_serial);
	    break;

	case SPDU_MAP: 
#ifdef	notdef
	case SPDU_AE: 		/* aka SPDU_MAP */
#endif
	    if ((s -> s_mask & SMASK_MAP_SYNC)
		    && (s -> s_map_sync & MAP_SYNC_NOEND)) {
		SPDU_TYPE ("MAJOR SYNCHRONIZATION POINT");
		type_bits (fp, rw, "SYNC", s -> s_map_sync, MAP_SYNC_MASK,
			SMASK);
	    }
	    else
		SPDU_TYPE ("ACTIVITY END");
	    if (s -> s_mask & SMASK_MAP_SERIAL)
		type_ssn (fp, rw, "SSN", s -> s_map_serial);
	    break;

	case SPDU_MAA: 
#ifdef	notdef
	case SPDU_AEA: 		/* aka SPDU_MAA */
#endif
	    SPDU_TYPE ("MAJOR SYNC/ACTIVITY END ACK");
	    if (s -> s_mask & SMASK_MAA_SERIAL)
		type_ssn (fp, rw, "SSN", s -> s_maa_serial);
	    break;

	case SPDU_RS: 
	    SPDU_TYPE ("RESYNCHRONIZE");
	    if (s -> s_mask & SMASK_RS_SET)
		type_settings (fp, rw, s -> s_rs_settings);
	    if (s -> s_mask & SMASK_RS_TYPE)
		type_resync (fp, rw, s -> s_rs_type);
	    if (s -> s_mask & SMASK_RS_SSN)
		type_ssn (fp, rw, "RSN", s -> s_rs_serial);
	    break;

	case SPDU_RA: 
	    SPDU_TYPE ("RESYNCHRONIZE ACK");
	    if (s -> s_mask & SMASK_RA_SET)
		type_settings (fp, rw, s -> s_ra_settings);
	    if (s -> s_mask & SMASK_RA_SSN)
		type_ssn (fp, rw, "RSN", s -> s_ra_serial);
	    break;

	case SPDU_PR: 
	    SPDU_TYPE ("PREPARE");
	    type_prepare (fp, rw, s -> s_pr_type);
	    break;

	case SPDU_ER: 
	    SPDU_TYPE ("EXCEPTION REPORT");
	    break;

	case SPDU_ED: 
	    SPDU_TYPE ("EXCEPTION DATA");
	    if (s -> s_mask & SMASK_ED_REASON)
		type_error (fp, rw, s -> s_ed_reason);
	    break;

	case SPDU_AS: 
	    SPDU_TYPE ("ACTIVITY START");
	    if (s -> s_mask & SMASK_AS_ID) {
		fprintf (fp, "%s", rw);
		type_info (fp, "ID/ %d", (int) s -> s_as_id.sd_len,
			s -> s_as_id.sd_data);
		fprintf (fp, "\n");
	    }
	    break;

	case SPDU_AR: 
	    SPDU_TYPE ("ACTIVITY RESUME");
	    if (s -> s_mask & SMASK_AR_REF)
		type_ref (fp, rw, &s -> s_ar_reference);
	    if (s -> s_mask & SMASK_AR_OID) {
		fprintf (fp, "%s", rw);
		type_info (fp, "OLD ID/ %d", (int) s -> s_ar_oid.sd_len,
			s -> s_ar_oid.sd_data);
		fprintf (fp, "\n");
	    }
	    if (s -> s_mask & SMASK_AR_SSN)
		type_ssn (fp, rw, "SSN", s -> s_ar_serial);
	    if (s -> s_mask & SMASK_AR_ID) {
		fprintf (fp, "%s", rw);
		type_info (fp, "ID/ %d", (int) s -> s_ar_id.sd_len,
			s -> s_ar_id.sd_data);
		fprintf (fp, "\n");
	    }
	    break;

	case SPDU_AD: 
	    SPDU_TYPE ("ACTIVITY DISCARD");
	    if (s -> s_mask & SMASK_AD_REASON)
		type_error (fp, rw, s -> s_ad_reason);
	    break;

	case SPDU_ADA: 
	    SPDU_TYPE ("ACTIVITY DISCARD ACK");
	    break;

	default: 
	    fprintf (fp, "%sCODE/ 0x%x\n", rw, s -> s_code);
	    break;
    }

    if (s -> s_mask & SMASK_ENCLOSE)
	type_bits (fp, rw, "ENCLOSURE", s -> s_enclose, ENCL_MASK, EMASK);

    if (s -> s_udata)
	if (s -> s_code == SPDU_ER)
	    type_data (fp, "REFLECT", rw, s -> s_ulen, s -> s_udata);
	else
	    if (s -> s_mask & SMASK_UDATA_PGI)
		type_data (fp, "USER", rw, s -> s_ulen, s -> s_udata);
	    else {
		fprintf (fp, "%sUSER INFO/ ", rw);
		type_info (fp, "%d", s -> s_ulen, s -> s_udata);
		fprintf (fp, "\n");
	    }
    fprintf (fp, "%s)\n", rw);
}

/*  */

static	type_id (fp, type, rw, selector, len)
FILE   *fp;
char   *type,
       *rw;
char   *selector;
int	len;
{
    char    buffer[BUFSIZ];

    buffer[explode (buffer, (u_char *) selector, len)] = NULL;

    fprintf (fp, "%s%s/ %d/\"%s\"\n", rw, type, len, buffer);
}


static	type_ssn (fp, rw, what, ssn)
FILE   *fp;
char   *rw,
       *what;
u_long	ssn;
{
    fprintf (fp, "%s%s/ %d\n", rw, what, ssn);
}


static	type_bits (fp, rw, s, bits, mask, t)
FILE   *fp;
char   *rw,
       *s,
       *t;
u_char  bits,
	mask;
{
    fprintf (fp, "%s%s/ %s", rw, s, sprintc (bits & mask, t));
    if (bits & ~mask)
	fprintf (fp, ": illegal use of %s", sprintc (bits & ~mask, t));
    fprintf (fp, "\n");
}


#define dotoken(requires,shift,bit,type) \
{ \
    token = (settings >> shift) & ST_MASK; \
    fprintf (fp, " %s:%s", type, token == ST_INIT_VALUE ? "initiator" \
	: token == ST_RESP_VALUE ? "responder" \
	: token == ST_CALL_VALUE ? "choice" \
	: "reserved"); \
}

static	type_settings (fp, rw, settings)
FILE   *fp;
char   *rw;
u_char  settings;
{
    int     token;

    fprintf (fp, "%sSETTINGS/", rw);
    dotokens ();
    fprintf (fp, "\n");
}

#undef	dotoken


static	type_tsdu (fp, rw, init, resp)
FILE   *fp;
char   *rw;
u_short	init,
	resp;
{
    fprintf (fp, "%sTSDU/ INITIATOR: %d, RESPONDER: %d\n",
	    rw, init, resp);
}


static	type_ref (fp, rw, ref)
FILE   *fp;
char   *rw;
struct SSAPref *ref;
{
    fprintf (fp, "%sREFERENCE/", rw);
    if (ref -> sr_vlen)
	type_info (fp, "<CALLING %d", (int) ref -> sr_calling_len,
		ref -> sr_calling);
    else
	type_info (fp, " <USER %d", (int) ref -> sr_ulen, ref -> sr_udata);
    type_info (fp, ", COMMON %d", (int) ref -> sr_clen, ref -> sr_cdata);
    type_info (fp, ", ADDITIONAL %d", (int) ref -> sr_alen, ref -> sr_adata);
    if (ref -> sr_vlen)
	type_info (fp, ", CALLED %d", (int) ref -> sr_called_len,
		ref -> sr_called);
    fprintf (fp, ">\n");
}


static	type_vrsn (fp, rw, version)
FILE   *fp;
char   *rw;
u_char	version;
{
    fprintf (fp, "%sVERSION/ 0x%x\n", rw, version);
}


static	type_reason (fp, rw, reason)
FILE   *fp;
char   *rw;
int	reason;
{
    fprintf (fp, "%sREASON/ 0x%x: %s\n", rw, reason,
	    SErrString ((int) reason));
}


static	type_prepare (fp, rw, type)
FILE   *fp;
char   *rw;
u_char  type;
{
    fprintf (fp, "%sTYPE/ ", rw);
    switch (type) {
	case PR_MAA: 
	    fprintf (fp, "MAA");
	    break;
	case PR_RS: 
	    fprintf (fp, "RS");
	    break;
	case PR_RA: 
	    fprintf (fp, "RA");
	    break;
	case PR_AB: 
	    fprintf (fp, "AB");
	    break;
	default: 
	    fprintf (fp, "%d: illegal value", type);
	    break;
    }
    fprintf (fp, "\n");
}


static	type_error (fp, rw, reason)
FILE   *fp;
char   *rw;
u_char  reason;
{
    fprintf (fp, "%sREASON/ ", rw);
    switch (reason) {
	case SP_NOREASON: 
	    fprintf (fp, "No specific reason stated");
	    break;
	case SP_JEOPARDY: 
	    fprintf (fp, "User receiving ability jeopardized");
	    break;
	case SP_SEQUENCE: 
	    fprintf (fp, "User sequence error");
	    break;
	case SP_LOCAL: 
	    fprintf (fp, "Local SS-user error");
	    break;
	case SP_PROCEDURAL: 
	    fprintf (fp, "Unrecoverable procedural error");
	    break;
	case SP_DEMAND: 
	    fprintf (fp, "Demand data token");
	    break;
	default: 
	    fprintf (fp, "%d: illegal value", reason);
	    break;
    }
    fprintf (fp, "\n");
}


static	type_resync (fp, rw, type)
FILE   *fp;
char   *rw;
u_char  type;
{
    fprintf (fp, "%sTYPE/ ", rw);
    switch (type) {
	case SYNC_RESTART: 
	    fprintf (fp, "restart");
	    break;
	case SYNC_ABANDON: 
	    fprintf (fp, "abandon");
	    break;
	case SYNC_SET: 
	    fprintf (fp, "set");
	    break;
	default: 
	    fprintf (fp, "%d: illegal value", type);
	    break;
    }
    fprintf (fp, "\n");
}


static	type_data (fp, type, rw, len, data)
FILE   *fp;
char   *type,
       *rw,
       *data;
int	len;
{
    fprintf (fp, "%s%s DATA/ ", rw, type);
    type_info (fp, "%d", len, data);
    fprintf (fp, "\n");
}


static	type_info (fp, fmt, len, data)
FILE   *fp;
char   *fmt,
       *data;
int	len;
{
    char    buffer[BUFSIZ];

    fprintf (fp, fmt, len);
    if (0 < len && len < sizeof buffer / 2) {
	buffer[explode (buffer, (u_char *) data, len)] = NULL;
	fprintf (fp, " %s", buffer);
    }
}

/*  */

/* ARGSUSED */

void	text2spkt (s)
struct ssapkt *s;
{
    /* NOT YET IMPLEMENTED */
}
