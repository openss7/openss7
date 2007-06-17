/*****************************************************************************

 @(#) $RCSfile: sua_asp.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:42 $

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

 Last Modified $Date: 2006/10/17 11:55:42 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sua_asp.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:42 $"

static char const ident[] =
    "$RCSfile: sua_asp.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/10/17 11:55:42 $";

#define __NO_VERSION__

#include <sys/os7/compat.h>

#include "../../include/ss7/mtp.h"
#include "../mtp/mtp_prov.h"

#include "sua_data.h"
#include "sua_msg.h"

#include "../ua/ua_asp.h"

/*
 *  =========================================================================
 *
 *  SUA Peer --> SUA (ASP) Received Messages
 *
 *  =========================================================================
 */
/*
 *  SUA_CLNS_CLDT   ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_clns_cldt(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CLNS_CLDR   ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_clns_cldr(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CONS_CORE   ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_cons_core(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CONS_COAK   ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_cons_coak(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CONS_COREF  ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_cons_coref(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CONS_RELRE  ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_cons_relre(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CONS_RELCO  ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_cons_relco(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CONS_RESCO  ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_cons_resco(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CONS_RESRE  ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_cons_resre(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CONS_CODT   ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_cons_codt(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CONS_CODA   ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_cons_coda(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CONS_COERR  ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_cons_coerr(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  SUA_CONS_COIT   ( AS --> SG, SG --> AS, SP --> SP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_cons_coit(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  UA_SNMM_DUNA
 *  -------------------------------------------------------------------------
 */
static int sua_recv_duna(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  UA_SNMM_DAVA
 *  -------------------------------------------------------------------------
 */
static int sua_recv_dava(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  UA_SNMM_DAUD
 *  -------------------------------------------------------------------------
 */
static int sua_recv_daud(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  UA_SNMM_SCON
 *  -------------------------------------------------------------------------
 */
static int sua_recv_scon(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  UA_SNMM_DUPU
 *  -------------------------------------------------------------------------
 */
static int sua_recv_dupu(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  UA_SNMM_DRST
 *  -------------------------------------------------------------------------
 */
static int sua_recv_drst(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  UA_RKMM_REG_RSP ( SGP --> ASP, IPSP --> IPSP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_reg_rsp(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  UA_RKMM_DEREG_RSP ( SGP --> ASP, IPSP --> IPSP )
 *  -------------------------------------------------------------------------
 */
static int sua_recv_dereg_rsp(asp_t * asp, mblk_t * msg)
{
	(void) asp;
	(void) msg;
	return (-EOPNOTSUPP);
}

/*
 *  Common defined ASP/SGP management procedures.
 */
extern int (*ua_asp_mgmt[]) (asp_t *, mblk_t *);
extern int (*ua_asp_asps[]) (asp_t *, mblk_t *);
extern int (*ua_asp_aspt[]) (asp_t *, mblk_t *);

static int (*sua_snmm[]) (asp_t *, mblk_t *) = {
	NULL,			/* (reserved) 0x0 */
	    sua_recv_duna,	/* UA_SNMM_DUNA 0x1 */
	    sua_recv_dava,	/* UA_SNMM_DAVA 0x2 */
	    sua_recv_daud,	/* UA_SNMM_DAUD 0x3 */
	    sua_recv_scon,	/* UA_SNMM_SCON 0x4 */
	    sua_recv_dupu,	/* UA_SNMM_DUPU 0x5 */
	    sua_recv_drst	/* UA_SNMM_DRST 0x6 */
};

static int (*sua_cnls[]) (asp_t * asp, mblk_t *) = {
	NULL,			/* (reserved) 0x0 */
	    sua_recv_clns_cldt,	/* SUA_CNLS_CLDT 0x1 */
	    sua_recv_clns_cldr,	/* SUA_CNLS_CLDR 0x2 */
};

static int (*sua_cons[]) (asp_t * asp, mblk_t *) = {
	NULL,			/* (reserved) 0x0 */
	    sua_recv_cons_core,	/* SUA_CONS_CORE 0x1 */
	    sua_recv_cons_coak,	/* SUA_CONS_COAK 0x2 */
	    sua_recv_cons_coref,	/* SUA_CONS_COREF 0x3 */
	    sua_recv_cons_relre,	/* SUA_CONS_RELRE 0x4 */
	    sua_recv_cons_relco,	/* SUA_CONS_RELCO 0x5 */
	    sua_recv_cons_resco,	/* SUA_CONS_RESCO 0x6 */
	    sua_recv_cons_resre,	/* SUA_CONS_RESRE 0x7 */
	    sua_recv_cons_codt,	/* SUA_CONS_CODT 0x8 */
	    sua_recv_cons_coda,	/* SUA_CONS_CODA 0x9 */
	    sua_recv_cons_coerr,	/* SUA_CONS_COERR 0xa */
	    sua_recv_cons_coit	/* SUA_CONS_COIT 0xb */
};

static int (*sua_rkmm[]) (asp_t *, mblk_t *) = {
	NULL,			/* (reserved) 0x0 */
	    NULL,		/* UA_RKMM_REG_REQ 0x1 */
	    sua_recv_reg_rsp,	/* UA_RKMM_REG_RSP 0x2 */
	    NULL,		/* UA_RKMM_DEREG_REQ 0x3 */
	    sua_recv_dereg_rsp	/* UA_RKMM_DEREG_RSP 0x4 */
};

static struct msg_class msg_decode[] = {
	{ua_asp_mgmt, UA_MGMT_LAST},	/* UA_CLASS_MGMT 0x0 */
	{NULL, 0},		/* UA_CLASS_XFER 0x1 */
	{sua_snmm, UA_SNMM_LAST},	/* UA_CLASS_SNMM 0x2 */
	{ua_asp_asps, UA_ASPS_LAST},	/* UA_CLASS_ASPS 0x3 */
	{ua_asp_aspt, UA_ASPT_LAST},	/* UA_CLASS_ASPT 0x4 */
	{NULL, 0},		/* UA_CLASS_Q921 0x5 */
	{NULL, 0},		/* UA_CLASS_MAUP 0x6 */
	{sua_cnls, SUA_CNLS_LAST},	/* UA_CLASS_CNLS 0x7 */
	{sua_cons, SUA_CONS_LAST},	/* UA_CLASS_CONS 0x8 */
	{sua_rkmm, UA_RKMM_LAST},	/* UA_CLASS_RKMM 0x9 */
	{NULL, 0},		/* UA_CLASS_TDHM 0xa */
	{NULL, 0}		/* UA_CLASS_TCHM 0xb */
};

int sua_recv_msg(asp_t * asp, mblk_t * msg)
{
	return ua_recv_msg(q, msg, msg_decode);
}
