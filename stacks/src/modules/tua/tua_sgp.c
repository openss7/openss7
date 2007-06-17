/*****************************************************************************

 @(#) $RCSfile: tua_sgp.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/06/17 01:56:36 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2002  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@dallas.net>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2007/06/17 01:56:36 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: tua_sgp.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/06/17 01:56:36 $"

static char const ident[] =
    "$RCSfile: tua_sgp.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/06/17 01:56:36 $";

/*
 *  UA_RKMM_REG_REQ	0x1
 *  -------------------------------------------------------------------------
 */
static int tua_recv_reg_req(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  UA_RKMM_REG_RSP	0x2
 *  -------------------------------------------------------------------------
 */
static int tua_recv_reg_rsp(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  UA_RKMM_DEREG_REQ	0x3
 *  -------------------------------------------------------------------------
 */
static int tua_recv_dereg_req(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  UA_RKMM_DEREG_RSP	0x4
 *  -------------------------------------------------------------------------
 */
static int tua_recv_dereg_rsp(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  TUA_TDHM_UNI	0x0
 *  -------------------------------------------------------------------------
 */
static int tua_recv_uni(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  TUA_TDHM_BEG	0x1
 *  -------------------------------------------------------------------------
 */
static int tua_recv_beg(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  TUA_TDHM_CON	0x2
 *  -------------------------------------------------------------------------
 */
static int tua_recv_con(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  TUA_TDHM_END	0x3
 *  -------------------------------------------------------------------------
 */
static int tua_recv_end(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  TUA_TDHM_U_ABT	0x4
 *  -------------------------------------------------------------------------
 */
static int tua_recv_u_abt(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  TUA_TDHM_P_ABT	0x5
 *  -------------------------------------------------------------------------
 */
static int tua_recv_p_abt(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  TUA_TDHM_NOT	0x6
 *  -------------------------------------------------------------------------
 */
static int tua_recv_not(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  TUA_TCHM_IVK	0x0
 *  -------------------------------------------------------------------------
 */
static int tua_recv_ivk(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  TUA_TCHM_RES	0x1
 *  -------------------------------------------------------------------------
 */
static int tua_recv_res(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  TUA_TCHM_U_ERR	0x2
 *  -------------------------------------------------------------------------
 */
static int tua_recv_u_err(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  TUA_TCHM_REJ	0x3
 *  -------------------------------------------------------------------------
 */
static int tua_recv_rej(sgp_t * sgp, mblk_t * msg)
{
	(void) q;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  Common defined ASP/SGP management procedures.
 */
extern int (*ua_mgmt[]) (sgp_t *, mblk_t *);
extern int (*ua_asps[]) (sgp_t *, mblk_t *);
extern int (*ua_aspt[]) (sgp_t *, mblk_t *);

static int (*tua_rkmm[]) (sgp_t *, mblk_t *) = {
	NULL,			/* (reserved) 0x0 */
	    tua_recv_reg_req,	/* UA_RKMM_REG_REQ 0x1 */
	    tua_recv_reg_rsp,	/* UA_RKMM_REG_RSP 0x2 */
	    tua_recv_dereg_req,	/* UA_RKMM_DEREG_REQ 0x3 */
	    tua_recv_dereg_rsp	/* UA_RKMM_DEREG_RSP 0x4 */
};
static int (*tua_tdhm[]) (sgp_t *, mblk_t *) = {
	tua_recv_uni,		/* TUA_TDHM_UNI 0x0 */
	    tua_recv_beg,	/* TUA_TDHM_BEG 0x1 */
	    tua_recv_con,	/* TUA_TDHM_CON 0x2 */
	    tua_recv_end,	/* TUA_TDHM_END 0x3 */
	    tua_recv_u_abt,	/* TUA_TDHM_U_ABT 0x4 */
	    tua_recv_p_abt,	/* TUA_TDHM_P_ABT 0x5 */
	    tua_recv_not	/* TUA_TDHM_NOT 0x6 */
};
static int (*tua_tchm[]) (sgp_t *, mblk_t *) = {
	tua_recv_ivk,		/* TUA_TCHM_IVK 0x0 */
	    tua_recv_res,	/* TUA_TCHM_RES 0x1 */
	    tua_recv_u_err,	/* TUA_TCHM_U_ERR 0x2 */
	    tua_recv_rej	/* TUA_TCHM_REJ 0x3 */
};

static struct msg_class msg_decode[] = {
	{ua_mgmt, UA_MGMT_LAST},	/* UA_CLASS_MGMT 0x0 */
	{NULL, 0},		/* UA_CLASS_XFER 0x1 */
	{NULL, 0},		/* UA_CLASS_SNMM 0x2 */
	{ua_asps, UA_ASPS_LAST},	/* UA_CLASS_ASPS 0x3 */
	{ua_aspt, UA_ASPT_LAST},	/* UA_CLASS_ASPT 0x4 */
	{NULL, 0},		/* UA_CLASS_Q921 0x5 */
	{NULL, 0},		/* UA_CLASS_MAUP 0x6 */
	{NULL, 0},		/* UA_CLASS_CNLS 0x7 */
	{NULL, 0},		/* UA_CLASS_CONS 0x8 */
	{tua_rkmm, UA_RKMM_LAST},	/* UA_CLASS_RKMM 0x9 */
	{tua_tdhm, TUA_TDHM},	/* UA_CLASS_TDHM 0xa */
	{tua_tchm TUA_TCHM}	/* UA_CLASS_TCHM 0xb */
};

int tua_recv_msg(sgp_t * sgp, mblk_t * msg)
{
	return ua_recv_msg(q, msg, msg_decode);
}
