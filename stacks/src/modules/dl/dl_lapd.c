/*****************************************************************************

 @(#) $RCSfile: dl_lapd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/04/14 10:33:05 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/04/14 10:33:05 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: dl_lapd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/04/14 10:33:05 $"

static char const ident[] = "$RCSfile: dl_lapd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/04/14 10:33:05 $";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/dki.h>
#include <sys/ddi.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <sys/cdi.h>
#include <sys/cdi_hdlc.h>
#include <sys/dlpi.h>
#include <sys/dlpi_lapd.h>
#include <ss7/lapd_ioctl.h>
#include <ss7/hdlc_ioctl.h>

#include "debug.h"
#include "bufq.h"
#include "priv.h"
#include "lock.h"
#include "queue.h"
#include "allocb.h"
#include "timer.h"

#define LAPD_DESCRIP	"LAPD Data Link (DL-LAPD) STREAMS MULTIPLEXING DRIVER ($Revision: 0.9.2.1 $)"
#define LAPD_REVISION	"OpenSS7 $RCSfile: dl_lapd.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/04/14 10:33:05 $"
#define LAPD_COPYRIGHT	"Copyright (c) 1997-2004  OpenSS7 Corporation.  All Rights Reserved."
#define LAPD_DEVICE	"OpenSS7 CDI Devices."
#define LAPD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LAPD_LICENSE	"GPL"
#define LAPD_BANNER	LAPD_DESCRIP	"\n" \
			LAPD_REVISION	"\n" \
			LAPD_COPYRIGHT	"\n" \
			LAPD_DEVICE	"\n" \
			LAPD_CONTACT

MODULE_AUTHOR(LAPD_CONTACT);
MODULE_DESCRIPTION(LAPD_DESCRIP);
MODULE_SUPPORTED_DEVICE(LAPD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LAPD_LICENSE);
#endif

#define DL_LAPD_CMINORS 255

#define MODULE_STATIC STATIC

/*
 *  =========================================================================
 *
 *  STREAMS Definitions
 *
 *  =========================================================================
 */

STATIC struct module_info dl_winfo = {
	mi_idnum:DL_LAPD_DRV_ID,	/* Module ID number */
	mi_idname:DL_LAPD_DRV_NAME,	/* Module ID name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1 << 15),		/* Hi water mark */
	mi_lowat:(1 << 10),		/* Lo water mark */
};

STATIC struct module_info dl_rinfo = {
	mi_idnum:DL_LAPD_DRV_ID,	/* Module ID number */
	mi_idname:DL_LAPD_DRV_NAME,	/* Module ID name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1 << 15),		/* Hi water mark */
	mi_lowat:(1 << 10),		/* Lo water mark */
};

STATIC struct module_info cd_winfo = {
	mi_idnum:DL_LAPD_DRV_ID,	/* Module ID number */
	mi_idname:DL_LAPD_DRV_NAME,	/* Module ID name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1 << 15),		/* Hi water mark */
	mi_lowat:(1 << 10),		/* Lo water mark */
};

STATIC struct module_info cd_rinfo = {
	mi_idnum:DL_LAPD_DRV_ID,	/* Module ID number */
	mi_idname:DL_LAPD_DRV_NAME,	/* Module ID name */
	mi_minpsz:(1),			/* Min packet size accepted */
	mi_maxpsz:INFPSZ,		/* Max packet size accepted */
	mi_hiwat:(1 << 15),		/* Hi water mark */
	mi_lowat:(1 << 10),		/* Lo water mark */
};

STATIC int dl_open(queue_t *, dev_t *, int, int, cred_t *);
STATIC int dl_close(queue_t *, int, cred_t *);

STATIC struct qinit dl_rinit = {
	qi_putp:ss7_oput,		/* Read put (message from below) */
	qi_srvp:ss7_osrv,		/* Read queue service */
	qi_qopen:dl_open,		/* Each open */
	qi_qclose:dl_close,		/* Last close */
	qi_minfo:&dl_rinfo,		/* Information */
};

STATIC struct qinit dl_winit = {
	qi_putp:ss7_iput,		/* Write put (message from above) */
	qi_srvp:ss7_isrv,		/* Write queue service */
	qi_minfo:&dl_winfo,		/* Information */
};

STATIC struct qinit cd_rinit = {
	qi_putp:ss7_iput,		/* Read put (message from below) */
	qi_srvp:ss7_isrv,		/* Read queue service */
	qi_minfo:&cd_rinfo,		/* Information */
};

STATIC struct qinit cd_winit = {
	qi_putp:ss7_oput,		/* Write put (message from above) */
	qi_srvp:ss7_osrv,		/* Write queue service */
	qi_minfo:&cd_winfo,		/* Information */
};

MODULE_STATIC struct streamtab lapd_info = {
	st_rdinit:&dl_rinit,		/* Upper read queue */
	st_wrinit:&dl_winit,		/* Upper write queue */
	st_muxrinit:&cd_rinit,		/* Lower read queue */
	st_muxwinit:&cd_winit,		/* Lower write queue */
};

STATIC int dl_r_prim(queue_t *, mblk_t *);
STATIC int dl_w_prim(queue_t *, mblk_t *);

STATIC int cd_r_prim(queue_t *, mblk_t *);
STATIC int cd_w_prim(queue_t *, mblk_t *);

/*
 *  =========================================================================
 *
 *  Private structures
 *
 *  =========================================================================
 */

/*
   forward declarations 
 */
struct df;				/* Default structure */
struct dl;				/* DL User */
struct cd;				/* CD provider */

/*
   default 
 */
struct df {
	lis_spin_lock_t lock;		/* master list lock */
	SLIST_HEAD (dl, dl);		/* list of DL structures */
	SLIST_HEAD (cd, cd);		/* list of CD structures */
	struct lapd_proto proto;	/* default protocol variant and options */
	struct lapd_timers_df timers;	/* default timers */
	struct lapd_option_df config;	/* default configuration */
	struct lapd_statem_df statem;	/* default state */
	struct lapd_stats_df statsp;	/* default statistics periods */
	struct lapd_stats_df stamp;	/* default statistics timestamps */
	struct lapd_stats_df stats;	/* default statistics */
	struct lapd_notify_df notify;	/* default notifications */
};

STATIC struct df master;
STATIC INLINE struct df *
df_lookup(ulong id)
{
	if (id)
		return (NULL);
	return (&master);
}

/*
 *  DL - DL User
 *  -----------------------------------
 */
struct dl {
	STR_DECLARATION (struct dl);	/* stream declaration */
	SLIST_LINKAGE (cd, dl, bind);	/* CD provider bind hash linkage */
	SLIST_LINKAGE (cd, dl, conn);	/* CD provider conn hash linkage */
	SLIST_LINKAGE (cd, dl, list);	/* CD provider list hash linkage */
	SLIST_LINKAGE (cd, dl, mgmt);	/* CD provider mgmt hash linkage */
	SLIST_LINKAGE (cd, dl, wait);	/* CD provider wait list linkage */
	SLIST_LINKAGE (cd, dl, teim);	/* CD provider teim list linkage */
	SLIST_LINKAGE (cd, dl, cd);	/* CD provider attached list linkage */
	struct lapd_addr dlc;		/* DL data link connection address */
	bufq_t conq;			/* connection queue */
	bufq_t sndq;			/* data for transmission */
	bufq_t rtxq;			/* data for retransmission */
	bufq_t rcvq;			/* data that was received */
	bufq_t ackq;			/* data awaiting acknowledgement */
	ulong conind;			/* max number of connection indications */
	ulong va;			/* next I frame acknowledged */
	ulong vs;			/* next I frame sent */
	ulong vr;			/* next I frame expected */
	ulong rc;			/* retransmission count */
	ulong ref;			/* reference for TEI management */
	dl_info_ack_t info;		/* DL information */
	struct lapd_proto proto;	/* DL protocol variant and options */
	struct lapd_timers_dl timers;	/* DL timers */
	struct lapd_option_dl config;	/* DL configuration */
	struct lapd_statem_dl statem;	/* DL state */
	struct lapd_stats_dl statsp;	/* DL statistics periods */
	struct lapd_stats_dl stamp;	/* DL statistics timestamps */
	struct lapd_stats_dl stats;	/* DL statistics */
	struct lapd_notify_dl notify;	/* DL notifications */
};
#define DL_PRIV(__q) ((struct dl *)(__q)->q_ptr)

#define DLF_AUTO_XID		0x00000001
#define DLF_AUTO_TEST		0x00000002
#define DLF_TEI_ASSIGNED	0x00000004

#define LAPD_TEI_UNASSIGNED		0
#define LAPD_WAIT_TEI_ASSIGN		1
#define LAPD_WAIT_TEI_ESTABLISH		2
#define LAPD_TEI_ASSIGNED		3
#define LAPD_WAIT_ESTABLISH		4
#define LAPD_WAIT_REESTABLISH		5
#define LAPD_PEND_RELEASE		6
#define LAPD_WAIT_RELEASE		7
#define LAPD_ESTABLISHED		8
#define LAPD_TIME_RECOVERY		9

STATIC INLINE struct dl *dl_lookup(ulong);
STATIC INLINE struct dl *dl_get(struct dl *);
STATIC INLINE void dl_put(struct dl *);
STATIC INLINE ulong dl_get_id(ulong);

STATIC struct dl *dl_alloc_priv(queue_t *, struct dl **, dev_t *, cred_t *, int);
STATIC void dl_free_priv(struct dl *);

STATIC INLINE ulong dl_get_state(struct dl *);
STATIC INLINE ulong dl_set_state(struct dl *, ulong);

#define DL_BIND_SIZE		(1<<4)
#define DL_CONN_SIZE		(1<<4)
#define DL_LIST_SIZE		(1<<4)

#define DL_BIND_HASHMASK	(DL_BIND_SIZE-1)
#define DL_CONN_HASHMASK	(DL_CONN_SIZE-1)
#define DL_LIST_HASHMASK	(DL_LIST_SIZE-1)

#define DLF_UNATTACHED		(1<<DL_UNATTACHED)
#define DLF_ATTACH_PENDING	(1<<DL_ATTACH_PENDING)
#define DLF_DETACH_PENDING	(1<<DL_DETACH_PENDING)
#define DLF_UNBOUND		(1<<DL_UNBOUND)
#define DLF_BIND_PENDING	(1<<DL_BIND_PENDING)
#define DLF_UNBIND_PENDING	(1<<DL_UNBIND_PENDING)
#define DLF_IDLE		(1<<DL_IDLE)
#define DLF_UDQOS_PENDING	(1<<DL_UDQOS_PENDING)
#define DLF_OUTCON_PENDING	(1<<DL_OUTCON_PENDING)
#define DLF_INCON_PENDING	(1<<DL_INCON_PENDING)
#define DLF_CONN_RES_PENDING	(1<<DL_CONN_RES_PENDING)
#define DLF_DATAXFER		(1<<DL_DATAXFER)
#define DLF_USER_RESET_PENDING	(1<<DL_USER_RESET_PENDING)
#define DLF_PROV_RESET_PENDING	(1<<DL_PROV_RESET_PENDING)
#define DLF_RESET_RES_PENDING	(1<<DL_RESET_RES_PENDING)
#define DLF_DISCON8_PENDING	(1<<DL_DISCON8_PENDING)
#define DLF_DISCON9_PENDING	(1<<DL_DISCON9_PENDING)
#define DLF_DISCON11_PENDING	(1<<DL_DISCON11_PENDING)
#define DLF_DISCON12_PENDING	(1<<DL_DISCON12_PENDING)
#define DLF_DISCON13_PENDING	(1<<DL_DISCON13_PENDING)
#define DLF_SUBS_BIND_PND	(1<<DL_SUBS_BIND_PND)
#define DLF_SUBS_UNBIND_PND	(1<<DL_SUBS_UNBIND_PND)

/*
 *  CD - CD Provider
 *  -----------------------------------
 */
struct cd {
	STR_DECLARATION (struct cd);	/* stream declaration */
	SLIST_HASH (dl, bind, DL_BIND_SIZE);	/* bind hash */
	SLIST_HASH (dl, conn, DL_CONN_SIZE);	/* conn hash */
	SLIST_HASH (dl, list, DL_LIST_SIZE);	/* list hash */
	struct dl *mgmt;		/* connection management stream */
	SLIST_HEAD (dl, wait);		/* DLs waiting on CD state change */
	SLIST_HEAD (dl, teim);		/* DLs performing TEI management */
	SLIST_HEAD (dl, dl);		/* CD attached DL list */
	ulong ppa;			/* CD physical point of attachment */
	ulong mode;			/* user/ntwk/both mode */
	cd_info_ack_t info;		/* CD information */
	struct lapd_proto proto;	/* CD protocol variant and options */
	struct lapd_timers_cd timers;	/* CD timers */
	struct lapd_option_cd config;	/* CD configuration */
	struct lapd_statem_cd statem;	/* CD state */
	struct lapd_stats_cd statsp;	/* CD statistics periods */
	struct lapd_stats_cd stamp;	/* CD statistics timestamps */
	struct lapd_stats_cd stats;	/* CD statistics */
	struct lapd_notify_cd notify;	/* CD notifications */
};
#define CD_PRIV(__q) ((struct cd *)(__q)->q_ptr)

STATIC INLINE struct cd *cd_lookup(ulong);
STATIC INLINE struct cd *cd_get(struct cd *);
STATIC INLINE void cd_put(struct cd *);
STATIC INLINE ulong cd_get_id(ulong);

STATIC struct cd *dl_alloc_link(queue_t *, struct cd **, ulong, cred_t *);
STATIC void dl_free_link(struct cd *);

STATIC INLINE ulong cd_get_state(struct cd *);
STATIC INLINE ulong cd_set_state(struct cd *, ulong);

#define CDF_UNATTACHED		(1<<CD_UNATTACHED)
#define CDF_UNUSABLE		(1<<CD_UNUSABLE)
#define CDF_DISABLED		(1<<CD_DISABLED)
#define CDF_ENABLE_PENDING	(1<<CD_ENABLE_PENDING)
#define CDF_ENABLED		(1<<CD_ENABLED)
#define CDF_READ_ACTIVE		(1<<CD_READ_ACTIVE)
#define CDF_INPUT_ALLOWED	(1<<CD_INPUT_ALLOWED)
#define CDF_DISABLE_PENDING	(1<<CD_DISABLE_PENDING)
#define CDF_OUTPUT_ACTIVE	(1<<CD_OUTPUT_ACTIVE)
#define	CDF_XRAY		(1<<CD_XRAY)

/*
 *  =========================================================================
 *
 *  STATE TRANSISIONS
 *
 *  =========================================================================
 */
#ifdef _DEBUG
STATIC INLINE const char *
dl_state_name(ulong state)
{
	switch (state) {
	case DL_UNATTACHED:
		return ("DL_UNATTACHED");
	case DL_ATTACH_PENDING:
		return ("DL_ATTACH_PENDING");
	case DL_DETACH_PENDING:
		return ("DL_DETACH_PENDING");
	case DL_UNBOUND:
		return ("DL_UNBOUND");
	case DL_BIND_PENDING:
		return ("DL_BIND_PENDING");
	case DL_UNBIND_PENDING:
		return ("DL_UNBIND_PENDING");
	case DL_IDLE:
		return ("DL_IDLE");
	case DL_UDQOS_PENDING:
		return ("DL_UDQOS_PENDING");
	case DL_OUTCON_PENDING:
		return ("DL_OUTCON_PENDING");
	case DL_INCON_PENDING:
		return ("DL_INCON_PENDING");
	case DL_CONN_RES_PENDING:
		return ("DL_CONN_RES_PENDING");
	case DL_DATAXFER:
		return ("DL_DATAXFER");
	case DL_USER_RESET_PENDING:
		return ("DL_USER_RESET_PENDING");
	case DL_PROV_RESET_PENDING:
		return ("DL_PROV_RESET_PENDING");
	case DL_RESET_RES_PENDING:
		return ("DL_RESET_RES_PENDING");
	case DL_DISCON8_PENDING:
		return ("DL_DISCON8_PENDING");
	case DL_DISCON9_PENDING:
		return ("DL_DISCON9_PENDING");
	case DL_DISCON11_PENDING:
		return ("DL_DISCON11_PENDING");
	case DL_DISCON12_PENDING:
		return ("DL_DISCON12_PENDING");
	case DL_DISCON13_PENDING:
		return ("DL_DISCON13_PENDING");
	case DL_SUBS_BIND_PND:
		return ("DL_SUBS_BIND_PND");
	case DL_SUBS_UNBIND_PND:
		return ("DL_SUBS_UNBIND_PND");
	case -1UL:
		return ("DL_UNUSABLE");
	default:
		swerr();
		return ("DL_????");
	}
}
#endif
STATIC INLINE ulong
dl_get_state(struct dl *dl)
{
	return (dl->i_state);
}
STATIC INLINE ulong
dl_set_state(struct dl *dl, ulong newstate)
{
	ulong oldstate = dl_get_state(dl);
	(void) oldstate;
	dl->info.dl_current_state = dl->i_state = newstate;
	printd(("%s: %p: %s <- %s\n", DL_LAPD_DRV_NAME, dl, dl_state_name(newstate),
		dl_state_name(oldstate)));
	return (newstate);
}

#ifdef _DEBUG
STATIC INLINE const char *
cd_state_name(ulong state)
{
	switch (state) {
	case CD_UNATTACHED:
		return ("CD_UNATTACHED");
	case CD_UNUSABLE:
		return ("CD_UNUSABLE");
	case CD_DISABLED:
		return ("CD_DISABLED");
	case CD_ENABLE_PENDING:
		return ("CD_ENABLE_PENDING");
	case CD_ENABLED:
		return ("CD_ENABLED");
	case CD_READ_ACTIVE:
		return ("CD_READ_ACTIVE");
	case CD_INPUT_ALLOWED:
		return ("CD_INPUT_ALLOWED");
	case CD_DISABLE_PENDING:
		return ("CD_DISABLE_PENDING");
	case CD_OUTPUT_ACTIVE:
		return ("CD_OUTPUT_ACTIVE");
	case CD_XRAY:
		return ("CD_XRAY");
	default:
		swerr();
		return ("CD_????");
	}
}
#endif
STATIC INLINE ulong
cd_get_state(struct cd *cd)
{
	return (cd->i_state);
}
STATIC INLINE ulong
cd_set_state(struct cd *cd, ulong newstate)
{
	ulong oldstate = cd_get_state(cd);
	(void) oldstate;
	cd->info.cd_state = cd->i_state = newstate;
	printd(("%s: %p: %s <- %s\n", DL_LAPD_DRV_NAME, cd, cd_state_name(newstate),
		cd_state_name(oldstate)));
	return (newstate);
}

/*
 *  =========================================================================
 *
 *  FUNCTIONS
 *
 *  =========================================================================
 */
STATIC void
dl_teim_link(struct dl *dl, struct cd *cd)
{
	ensure(dl, return);
	unless(dl->teim.cd, return);
	if ((dl->teim.next = cd->teim.list))
		dl->teim.next->teim.prev = &dl->teim.next;
	dl->teim.prev = &cd->teim.list;
	dl->teim.cd = cd_get(cd);
	cd->teim.list = dl_get(dl);
	cd->teim.numb++;
}
STATIC void
dl_teim_unlink(struct dl *dl)
{
	ensure(dl->teim.cd, return);
	if ((*dl->teim.prev = dl->teim.next))
		dl->teim.next->teim.prev = dl->teim.prev;
	dl->teim.next = NULL;
	dl->teim.prev = &dl->teim.next;
	dl->teim.cd->teim.numb--;
	cd_put(xchg(&dl->teim.cd, NULL));
	dl_put(dl);
}
STATIC void
dl_wait_link(struct dl *dl, struct cd *cd)
{
	ensure(dl, return);
	unless(dl->wait.cd, return);
	if ((dl->wait.next = cd->wait.list))
		dl->wait.next->wait.prev = &dl->wait.next;
	dl->wait.prev = &cd->wait.list;
	dl->wait.cd = cd_get(cd);
	cd->wait.list = dl_get(dl);
	cd->wait.numb++;
}
STATIC void
dl_wait_unlink(struct dl *dl)
{
	ensure(dl->wait.cd, return);
	if ((*dl->wait.prev = dl->wait.next))
		dl->wait.next->wait.prev = dl->wait.prev;
	dl->wait.next = NULL;
	dl->wait.prev = &dl->wait.next;
	dl->wait.cd->wait.numb--;
	cd_put(xchg(&dl->wait.cd, NULL));
	dl_put(dl);
}

#if 0
STATIC void
dl_mgmt_link(struct dl *dl, struct cd *cd)
{
	unless(cd->mgmt, return);
	if ((dl->mgmt.next = cd->mgmt))
		dl->mgmt.next->mgmt.prev = &dl->mgmt.next;
	dl->mgmt.prev = &cd->mgmt;
	dl->mgmt.cd = cd_get(cd);
	cd->mgmt = dl_get(dl);
}
#endif
STATIC void
dl_mgmt_unlink(struct dl *dl)
{
	ensure(dl->mgmt.cd, return);
	if ((*dl->mgmt.prev = dl->mgmt.next))
		dl->mgmt.next->mgmt.prev = dl->mgmt.prev;
	dl->mgmt.next = NULL;
	dl->mgmt.prev = &dl->mgmt.next;
	cd_put(xchg(&dl->mgmt.cd, NULL));
	dl_put(dl);
}
STATIC void
dl_atta_link(struct dl *dl, struct cd *cd)
{
	unless(dl->cd.cd, return);
	if ((dl->cd.next = cd->dl.list))
		dl->cd.next->cd.prev = &dl->cd.next;
	dl->cd.prev = &cd->dl.list;
	dl->cd.cd = cd_get(cd);
	cd->dl.list = dl_get(dl);
	cd->dl.numb++;
}
STATIC void
dl_atta_unlink(struct dl *dl)
{
	ensure(dl->cd.cd, return);
	if ((*dl->cd.prev = dl->cd.next))
		dl->cd.next->cd.prev = dl->cd.prev;
	dl->cd.next = NULL;
	dl->cd.prev = &dl->cd.next;
	dl->cd.cd->dl.numb--;
	cd_put(xchg(&dl->cd.cd, NULL));
	dl_put(dl);
}
STATIC void
dl_bind_link(struct dl *dl, struct cd *cd)
{
	int i;
	ensure(dl, return);
	ensure(cd, return);
	unless(dl->bind.cd, return);
	i = ((dl->dlc.dl_sap + dl->dlc.dl_tei) & DL_BIND_HASHMASK);
	if ((dl->bind.next = cd->bind.hash[i]))
		dl->bind.next->bind.prev = &dl->bind.next;
	dl->bind.prev = &cd->bind.hash[i];
	dl->bind.cd = cd_get(cd);
	cd->bind.hash[i] = dl_get(dl);
	cd->bind.numb++;
}
STATIC void
dl_bind_unlink(struct dl *dl)
{
	ensure(dl->bind.cd, return);
	if ((*dl->bind.prev = dl->bind.next))
		dl->bind.next->bind.prev = dl->bind.prev;
	dl->bind.next = NULL;
	dl->bind.prev = &dl->bind.next;
	dl->bind.cd->bind.numb--;
	cd_put(xchg(&dl->bind.cd, NULL));
	dl_put(dl);
}
STATIC void
dl_conn_link(struct dl *dl, struct cd *cd)
{
	int i;
	ensure(dl, return);
	ensure(cd, return);
	unless(dl->conn.cd, return);
	i = ((dl->dlc.dl_sap + dl->dlc.dl_tei) & DL_CONN_HASHMASK);
	if ((dl->conn.next = cd->conn.hash[i]))
		dl->conn.next->conn.prev = &dl->conn.next;
	dl->conn.prev = &cd->conn.hash[i];
	dl->conn.cd = cd_get(cd);
	cd->conn.hash[i] = dl_get(dl);
	cd->conn.numb++;
}
STATIC void
dl_conn_unlink(struct dl *dl)
{
	ensure(dl->conn.cd, return);
	if ((*dl->conn.prev = dl->conn.next))
		dl->conn.next->conn.prev = dl->conn.prev;
	dl->conn.next = NULL;
	dl->conn.prev = &dl->conn.next;
	dl->conn.cd->conn.numb--;
	cd_put(xchg(&dl->conn.cd, NULL));
	dl_put(dl);
}
STATIC void
dl_list_link(struct dl *dl, struct cd *cd)
{
	int i;
	ensure(dl, return);
	ensure(cd, return);
	unless(dl->list.cd, return);
	i = ((dl->dlc.dl_sap + dl->dlc.dl_tei) & DL_LIST_HASHMASK);
	if ((dl->list.next = cd->list.hash[i]))
		dl->list.next->list.prev = &dl->list.next;
	dl->list.prev = &cd->list.hash[i];
	dl->list.cd = cd_get(cd);
	cd->list.hash[i] = dl_get(dl);
	cd->list.numb++;
}
STATIC void
dl_list_unlink(struct dl *dl)
{
	ensure(dl->list.cd, return);
	if ((*dl->list.prev = dl->list.next))
		dl->list.next->list.prev = dl->list.prev;
	dl->list.next = NULL;
	dl->list.prev = &dl->list.next;
	dl->list.cd->list.numb--;
	cd_put(xchg(&dl->list.cd, NULL));
	dl_put(dl);
}
STATIC INLINE void
dl_mast_link(struct dl *dl, struct dl **dlp)
{
	if ((dl->next = *dlp))
		dl->next->prev = &dl->next;
	dl->prev = dlp;
	*dlp = dl_get(dl);
	master.dl.numb++;
}
STATIC INLINE void
dl_mast_unlink(struct dl *dl)
{
	if ((*dl->prev = dl->next))
		dl->next->prev = dl->prev;
	dl->next = NULL;
	dl->prev = &dl->next;
	dl_put(dl);
	master.dl.numb--;
}
STATIC INLINE void
cd_mast_link(struct cd *cd, struct cd **cdp)
{
	if ((cd->next = *cdp))
		cd->next->prev = &cd->next;
	cd->prev = cdp;
	*cdp = cd_get(cd);
	master.cd.numb++;
}
STATIC INLINE void
cd_mast_unlink(struct cd *cd)
{
	if ((*cd->prev = cd->next))
		cd->next->prev = cd->prev;
	cd->next = NULL;
	cd->prev = &cd->next;
	cd_put(cd);
	master.cd.numb--;
}

/*
 *  =========================================================================
 *
 *  TIMERS
 *
 *  =========================================================================
 */
enum { tall, t200, t201, t202, t203 };

/*
 *  DL timers
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DL_LAPD_DRV_NAME, dl, t200, config.timers);
SS7_DECLARE_TIMER(DL_LAPD_DRV_NAME, dl, t201, config.timers);
SS7_DECLARE_TIMER(DL_LAPD_DRV_NAME, dl, t202, config.timers);
SS7_DECLARE_TIMER(DL_LAPD_DRV_NAME, dl, t203, config.timers);
STATIC INLINE void
__dl_timer_stop(struct dl *dl, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/*
		   fall through 
		 */
	case t200:
		dl_stop_timer_t200(dl);
		if (single)
			break;
		/*
		   fall through 
		 */
	case t201:
		dl_stop_timer_t201(dl);
		if (single)
			break;
		/*
		   fall through 
		 */
	case t202:
		dl_stop_timer_t202(dl);
		if (single)
			break;
		/*
		   fall through 
		 */
	case t203:
		dl_stop_timer_t203(dl);
		if (single)
			break;
		/*
		   fall through 
		 */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
dl_timer_stop(struct dl *dl, const uint t)
{
	psw_t flags;
	lis_spin_lock_irqsave(&dl->lock, &flags);
	{
		__dl_timer_stop(dl, t);
	}
	lis_spin_unlock_irqrestore(&dl->lock, &flags);
}
STATIC INLINE void
dl_timer_start(struct dl *dl, const uint t)
{
	psw_t flags;
	lis_spin_lock_irqsave(&dl->lock, &flags);
	{
		__dl_timer_stop(dl, t);
		switch (t) {
		case t200:
			dl_start_timer_t200(dl);
			break;
		case t201:
			dl_start_timer_t201(dl);
			break;
		case t202:
			dl_start_timer_t202(dl);
			break;
		case t203:
			dl_start_timer_t203(dl);
			break;
		default:
			swerr();
			break;
		}
	}
	lis_spin_unlock_irqrestore(&dl->lock, &flags);
}

/*
 *  CD timers
 *  -------------------------------------------------------------------------
 */
SS7_DECLARE_TIMER(DL_LAPD_DRV_NAME, cd, t201, config.timers);
SS7_DECLARE_TIMER(DL_LAPD_DRV_NAME, cd, t202, config.timers);
STATIC INLINE void
__cd_timer_stop(struct cd *cd, const uint t)
{
	int single = 1;
	switch (t) {
	case tall:
		single = 0;
		/*
		   fall through 
		 */
	case t201:
		cd_stop_timer_t201(cd);
		if (single)
			break;
		/*
		   fall through 
		 */
	case t202:
		cd_stop_timer_t202(cd);
		if (single)
			break;
		/*
		   fall through 
		 */
		break;
	default:
		swerr();
		break;
	}
}
STATIC INLINE void
cd_timer_stop(struct cd *cd, const uint t)
{
	psw_t flags;
	lis_spin_lock_irqsave(&cd->lock, &flags);
	{
		__cd_timer_stop(cd, t);
	}
	lis_spin_unlock_irqrestore(&cd->lock, &flags);
}
STATIC INLINE void
cd_timer_start(struct cd *cd, const uint t)
{
	psw_t flags;
	lis_spin_lock_irqsave(&cd->lock, &flags);
	{
		__cd_timer_stop(cd, t);
		switch (t) {
		case t201:
			cd_start_timer_t201(cd);
			break;
		case t202:
			cd_start_timer_t202(cd);
			break;
		default:
			swerr();
			break;
		}
	}
	lis_spin_unlock_irqrestore(&cd->lock, &flags);
}

/*
 *  =========================================================================
 *
 *  PRIMITIVES
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Primitive sent to upstream
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  M_HANGUP
 *  -----------------------------------
 */
STATIC int
m_hangup(queue_t *q, struct dl *dl)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 0, BPRI_MED))) {
		mp->b_datap->db_type = M_HANGUP;
		printd(("%s: %p: <- M_HANGUP\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  M_ERROR
 *  -----------------------------------
 */
STATIC int
m_error(queue_t *q, struct dl *dl, uint8_t rerr, uint8_t werr)
{
	mblk_t *mp;
	if ((mp = ss7_allocb(q, 2, BPRI_MED))) {
		mp->b_datap->db_type = M_ERROR;
		*mp->b_wptr++ = rerr;
		*mp->b_wptr++ = werr;
		printd(("%s: %p: <- M_ERROR\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  M_HANGUP All
 *  -----------------------------------
 */
STATIC int
m_hangup_all(queue_t *q, struct cd *cd)
{
	struct dl *dl;
	int err;
	fixme(("re-write this function\n"));
	/*
	   this function should also detach the dl 
	 */
	for (dl = cd->dl.list; dl; dl = dl->cd.next)
		if ((err = m_hangup(q, dl)))
			return (err);
	return (QR_DONE);
}

/*
 *  M_ERROR All
 *  -----------------------------------
 */
STATIC int
m_error_all(queue_t *q, struct cd *cd, uint8_t rerr, uint8_t werr)
{
	struct dl *dl;
	int err;
	fixme(("re-write this function\n"));
	/*
	   this function should also detach the dl 
	 */
	for (dl = cd->dl.list; dl; dl = dl->cd.next)
		if ((err = m_error(q, dl, rerr, werr)))
			return (err);
	return (QR_DONE);
}

/*
 *  DL_INFO_ACK                 0x03
 *  -----------------------------------
 */
STATIC INLINE int
dl_info_ack(queue_t *q, struct dl *dl)
{
	mblk_t *mp;
	dl_info_ack_t *p;
	size_t alen, blen = 0;
	caddr_t aptr, bptr = NULL;
	if (dl_get_state(dl) > DL_UNATTACHED) {
		alen = sizeof(dl->dlc);
		aptr = (caddr_t) (&dl->dlc);
	} else {
		alen = 0;
		aptr = NULL;
	}
	if ((mp = ss7_allocb(q, sizeof(*p) + alen + blen, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		*p = dl->info;
		p->dl_primitive = DL_INFO_ACK;
		p->dl_addr_length = alen;
		p->dl_addr_offset = alen ? sizeof(*p) : 0;
		p->dl_brdcst_addr_length = blen;
		p->dl_brdcst_addr_offset = blen ? sizeof(*p) + alen : 0;
		if (alen) {
			bcopy(aptr, mp->b_wptr, alen);
			mp->b_wptr += alen;
		}
		todo(("should return broadcast address\n"));
		if (blen) {
			bcopy(bptr, mp->b_wptr, blen);
			mp->b_wptr += blen;
		}
		printd(("%s: %p: <- DL_INFO_ACK\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_BIND_ACK                 0x04
 *  -----------------------------------
 */
STATIC INLINE int
dl_bind_ack(queue_t *q, struct dl *dl, struct cd *cd)
{
	mblk_t *mp;
	dl_bind_ack_t *p;
	size_t add_len = 0;
	caddr_t add_ptr = NULL;
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_BIND_ACK;
		p->dl_sap = dl->dlc.dl_sap;
		p->dl_addr_length = add_len;
		p->dl_addr_offset = add_len ? sizeof(*p) : 0;
		p->dl_max_conind = dl->conind;
		p->dl_xidtest_flg =
		    ((dl->flags & DLF_AUTO_XID) ? DL_AUTO_XID : 0) | ((dl->
								       flags & DLF_AUTO_TEST) ?
								      DL_AUTO_TEST : 0);
		if (add_len) {
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		if (!dl->bind.cd)
			dl_bind_link(dl, cd);
		if (!dl->list.cd && dl->conind)
			dl_list_link(dl, cd);
		if (dl->wait.cd)
			dl_wait_unlink(dl);
		dl->state = LAPD_TEI_UNASSIGNED;
		dl_set_state(dl, DL_IDLE);
		printd(("%s: %p: <- DL_BIND_ACK\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_SUBS_BIND_ACK            0x1c
 *  -----------------------------------
 */
STATIC INLINE int
dl_subs_bind_ack(queue_t *q, struct dl *dl, uchar tei)
{
	mblk_t *mp;
	dl_subs_bind_ack_t *p;
	size_t sap_len = sizeof(tei);
	caddr_t sap_ptr = (caddr_t) &tei;
	if ((mp = ss7_allocb(q, sizeof(*p) + sap_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_SUBS_BIND_ACK;
		p->dl_subs_sap_length = sap_len;
		p->dl_subs_sap_offset = sap_len ? sizeof(*p) : 0;
		if (sap_len) {
			bcopy(sap_ptr, mp->b_wptr, sap_len);
			mp->b_wptr += sap_len;
		}
		dl_bind_unlink(dl);
		if (dl->conind)
			dl_list_unlink(dl);
		dl->dlc.dl_tei = tei;
		dl_bind_link(dl, dl->cd.cd);
		if (dl->conind)
			dl_list_link(dl, dl->cd.cd);
		printd(("%s: %p: <- DL_SUBS_BIND_ACK\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_OK_ACK                   0x06
 *  -----------------------------------
 */
STATIC INLINE int send_UA_res(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf);
STATIC INLINE int send_DM_res(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf);
STATIC INLINE int send_DISC_cmd(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf);
STATIC INLINE int
dl_ok_ack(queue_t *q, struct dl *dl, ulong prim, struct cd *cd, struct dl *ap, mblk_t *cp)
{
	mblk_t *mp;
	dl_ok_ack_t *p;
	int err;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_OK_ACK;
		p->dl_correct_primitive = prim;
		if (dl->wait.cd)
			dl_wait_unlink(dl);	/* remove from wait list */
		switch (dl_get_state(dl)) {
		case DL_ATTACH_PENDING:
			if (!dl->cd.cd)
				dl_atta_link(dl, cd);	/* add dl to cd attach list */
			dl->state = LAPD_TEI_UNASSIGNED;
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_DETACH_PENDING:
			if (dl->cd.cd)
				dl_atta_unlink(dl);	/* remove from cd attach list */
			dl->state = LAPD_TEI_UNASSIGNED;
			dl_set_state(dl, DL_UNATTACHED);
			break;
		case DL_UNBIND_PENDING:
			if (dl->bind.cd)
				dl_bind_unlink(dl);	/* remove from bind hash */
			if (dl->list.cd)
				dl_list_unlink(dl);	/* remove from list hash */
			dl->state = LAPD_TEI_UNASSIGNED;
			dl_set_state(dl, DL_UNBOUND);
			break;
		case DL_CONN_RES_PENDING:
			ensure(ap, ap = dl);
			if (cp) {
				uchar sapi = (cp->b_rptr[0] >> 2) & 0x3f;
				uchar tei = (cp->b_rptr[1] >> 1) & 0x7f;
				uchar pf = (cp->b_rptr[3] & 0x01);
				if ((err = send_UA_res(q, cd, sapi, tei, pf)) < 0) {
					freemsg(mp);
					return (err);
				}
				dl_timer_start(ap, t203);
				bufq_unlink(&dl->conq, cp);
				freemsg(cp);
			}
			if (ap != dl) {
				/*
				   auto attach 
				 */
				if (dl->cd.cd != ap->cd.cd) {
					dl_atta_unlink(ap);
					dl_atta_link(ap, dl->cd.cd);
				}
				/*
				   auto bind 
				 */
				if (dl->dlc.dl_sap != ap->dlc.dl_sap
				    || dl->dlc.dl_tei != ap->dlc.dl_tei) {
					dl_bind_unlink(ap);
					ap->dlc = dl->dlc;
					dl_bind_link(ap, ap->cd.cd);
				}
				if (dl->conq.q_msgs)
					dl_set_state(dl, DL_INCON_PENDING);
				else
					dl_set_state(dl, DL_IDLE);
			}
			/*
			   move to data transfer state 
			 */
			dl->state = LAPD_ESTABLISHED;
			dl_set_state(ap, DL_DATAXFER);
			/*
			   place in connection hashes 
			 */
			dl_conn_link(ap, ap->cd.cd);
			ap->vs = ap->vr = ap->va = 0;
			break;
		case DL_RESET_RES_PENDING:
			dl->state = LAPD_ESTABLISHED;
			dl_set_state(dl, DL_DATAXFER);
			break;
		case DL_DISCON9_PENDING:
			if (cp) {
				uchar sapi = (cp->b_rptr[0] >> 2) & 0x3f;
				uchar tei = (cp->b_rptr[1] >> 1) & 0x7f;
				uchar pf = (cp->b_rptr[3] & 0x01);
				if ((err = send_DM_res(q, cd, sapi, tei, pf)) < 0) {
					freemsg(mp);
					return (err);
				}
				bufq_unlink(&dl->conq, cp);
				freemsg(cp);
			}
			dl->state = LAPD_TEI_ASSIGNED;
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_DISCON8_PENDING:
		case DL_DISCON11_PENDING:
		case DL_DISCON12_PENDING:
		case DL_DISCON13_PENDING:
			if ((1 << cd_get_state(cd)) &
			    (CDF_ENABLED | CDF_READ_ACTIVE | CDF_INPUT_ALLOWED | CDF_OUTPUT_ACTIVE))
			{
				/*
				   layer 1 active 
				 */
				uchar sapi = dl->dlc.dl_sap;
				uchar tei = dl->dlc.dl_tei;
				uchar pf = 1;
				if ((err = send_DISC_cmd(q, cd, sapi, tei, pf))) {
					freemsg(mp);
					return (err);
				}
			}
			dl->state = LAPD_WAIT_RELEASE;
			dl_set_state(dl, DL_IDLE);
			break;
		case DL_SUBS_UNBIND_REQ:
			/*
			   just jam tei 
			 */
			dl_bind_unlink(dl);
			if (dl->conind)
				dl_list_unlink(dl);
			dl->dlc.dl_tei = -1;
			dl_bind_link(dl, dl->cd.cd);
			if (dl->conind)
				dl_list_link(dl, dl->cd.cd);
			dl->state = LAPD_TEI_UNASSIGNED;
			dl_set_state(dl, DL_IDLE);
			break;
		default:
			/*
			   happens for promicon, promiscoff, enabmulti, disabmulti 
			 */
			/*
			   remain in the same state 
			 */
			break;
		}
		printd(("%s: %p: <- DL_OK_ACK\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_ERROR_ACK                0x05
 *  -----------------------------------
 */
STATIC INLINE int
dl_error_ack(queue_t *q, struct dl *dl, ulong prim, ulong error, ulong reason)
{
	mblk_t *mp;
	dl_error_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_ERROR_ACK;
		p->dl_error_primitive = prim;
		p->dl_errno = error;
		p->dl_unix_errno = reason;
		if (dl->wait.cd)
			dl_wait_unlink(dl);	/* remove from wait list */
		/*
		   restore previous state 
		 */
		dl_set_state(dl, dl->i_oldstate);
		printd(("%s: %p: <- DL_ERROR_ACK\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_CONNECT_IND              0x0e
 *  -----------------------------------
 */
STATIC INLINE int
dl_connect_ind(queue_t *q, struct dl *dl, mblk_t *cp, size_t cda_len, caddr_t cda_ptr,
	       size_t cga_len, caddr_t cga_ptr)
{
	mblk_t *mp;
	dl_connect_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + cda_len + cga_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_CONNECT_IND;
		p->dl_correlation = (typeof(p->dl_correlation)) cp;
		p->dl_called_addr_length = cda_len;
		p->dl_called_addr_offset = cda_len ? sizeof(*p) : 0;
		p->dl_calling_addr_length = cga_len;
		p->dl_calling_addr_offset = cga_len ? sizeof(*p) + cda_len : 0;
		p->dl_qos_length = 0;
		p->dl_qos_offset = 0;
		p->dl_growth = 0;
		if (cda_len) {
			bcopy(cda_ptr, mp->b_wptr, cda_len);
			mp->b_wptr += cda_len;
		}
		if (cga_len) {
			bcopy(cga_ptr, mp->b_wptr, cga_len);
			mp->b_wptr += cga_len;
		}
		bufq_queue(&dl->conq, cp);
		dl_set_state(dl, DL_INCON_PENDING);
		printd(("%s: %p: <- DL_CONNECT_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_ABSORBED);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_CONNECT_CON              0x10
 *  -----------------------------------
 */
STATIC INLINE int
dl_connect_con(queue_t *q, struct dl *dl, size_t res_len, caddr_t res_ptr)
{
	mblk_t *mp;
	dl_connect_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + res_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_CONNECT_CON;
		p->dl_resp_addr_length = res_len;
		p->dl_resp_addr_offset = res_len ? sizeof(*p) : 0;
		p->dl_qos_length = 0;
		p->dl_qos_offset = 0;
		p->dl_growth = 0;
		if (res_len) {
			bcopy(res_ptr, mp->b_wptr, res_len);
			mp->b_wptr += res_len;
		}
		dl->state = LAPD_ESTABLISHED;
		dl_set_state(dl, DL_DATAXFER);
		dl_timer_start(dl, t200);
		dl_timer_start(dl, t203);
		dl->vs = dl->vr = dl->va = 0;
		printd(("%s: %p: <- DL_CONNECT_CON\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_TOKEN_ACK                0x12
 *  -----------------------------------
 */
STATIC INLINE int
dl_token_ack(queue_t *q, struct dl *dl)
{
	mblk_t *mp;
	dl_token_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_TOKEN_ACK;
		p->dl_token = dl->id;
		printd(("%s: %p: <- DL_TOKEN_ACK\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_DISCONNECT_IND           0x14
 *  -----------------------------------
 */
STATIC INLINE int
dl_disconnect_ind(queue_t *q, struct dl *dl, ulong orig, ulong reason, mblk_t *cp)
{
	mblk_t *mp;
	dl_disconnect_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_DISCONNECT_IND;
		p->dl_originator = orig;
		p->dl_reason = reason;
		p->dl_correlation = (ulong) cp;
		if (cp) {
			bufq_unlink(&dl->conq, cp);
			if (dl->conq.q_msgs)
				dl_set_state(dl, DL_INCON_PENDING);
			else
				dl_set_state(dl, DL_IDLE);
		} else {
			dl->state = LAPD_TEI_ASSIGNED;
			dl_set_state(dl, DL_IDLE);
		}
		printd(("%s: %p: <- DL_DISCONNECT_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_RESET_IND                0x18
 *  -----------------------------------
 */
STATIC INLINE int
dl_reset_ind(queue_t *q, struct dl *dl, ulong orig, ulong reason)
{
	mblk_t *mp;
	dl_reset_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_RESET_IND;
		p->dl_originator = orig;
		p->dl_reason = reason;
		dl_set_state(dl, DL_PROV_RESET_PENDING);
		printd(("%s: %p: <- DL_RESET_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_RESET_CON                0x1a
 *  -----------------------------------
 */
STATIC INLINE int
dl_reset_con(queue_t *q, struct dl *dl)
{
	mblk_t *mp;
	dl_reset_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_RESET_CON;
		dl->state = LAPD_ESTABLISHED;
		dl_set_state(dl, DL_DATAXFER);
		printd(("%s: %p: <- DL_RESET_CON\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_UNITDATA_IND             0x08
 *  -----------------------------------
 */
STATIC INLINE int
dl_unitdata_ind(queue_t *q, struct dl *dl, mblk_t *dp, size_t dst_len, caddr_t dst_ptr,
		size_t src_len, caddr_t src_ptr, ulong flag)
{
	mblk_t *mp;
	dl_unitdata_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_UNITDATA_IND;
		p->dl_dest_addr_length = dst_len;
		p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src_len;
		p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
		p->dl_group_address = flag;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		if (src_len) {
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- DL_UNITDATA_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_UDERROR_IND              0x09
 *  -----------------------------------
 */
STATIC INLINE int
dl_uderror_ind(queue_t *q, struct dl *dl, ulong errno, ulong reason, size_t dst_len,
	       caddr_t dst_ptr, mblk_t *dp)
{
	mblk_t *mp;
	dl_uderror_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_UDERROR_IND;
		p->dl_dest_addr_length = dst_len;
		p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		p->dl_unix_errno = reason;
		p->dl_errno = errno;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- DL_UDERROR_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_TEST_IND                 0x2e
 *  -----------------------------------
 */
STATIC INLINE int
dl_test_ind(queue_t *q, struct dl *dl, ulong flag, size_t dst_len, caddr_t dst_ptr, size_t src_len,
	    caddr_t src_ptr, mblk_t *dp)
{
	mblk_t *mp;
	dl_test_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_TEST_IND;
		p->dl_flag = flag;
		p->dl_dest_addr_length = dst_len;
		p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src_len;
		p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		if (src_len) {
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- DL_TEST_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_TEST_CON                 0x30
 *  -----------------------------------
 */
STATIC INLINE int
dl_test_con(queue_t *q, struct dl *dl, ulong flag, size_t dst_len, caddr_t dst_ptr, size_t src_len,
	    caddr_t src_ptr, mblk_t *dp)
{
	mblk_t *mp;
	dl_test_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_TEST_CON;
		p->dl_flag = flag;
		p->dl_dest_addr_length = dst_len;
		p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src_len;
		p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		if (src_len) {
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- DL_TEST_CON\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_XID_IND                  0x2a
 *  -----------------------------------
 */
STATIC INLINE int
dl_xid_ind(queue_t *q, struct dl *dl, ulong flag, size_t dst_len, caddr_t dst_ptr, size_t src_len,
	   caddr_t src_ptr, mblk_t *dp)
{
	mblk_t *mp;
	dl_xid_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_XID_IND;
		p->dl_flag = flag;
		p->dl_dest_addr_length = dst_len;
		p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src_len;
		p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		if (src_len) {
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- DL_XID_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_XID_CON                  0x2c
 *  -----------------------------------
 */
STATIC INLINE int
dl_xid_con(queue_t *q, struct dl *dl, ulong flag, size_t dst_len, caddr_t dst_ptr, size_t src_len,
	   caddr_t src_ptr, mblk_t *dp)
{
	mblk_t *mp;
	dl_xid_con_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_XID_CON;
		p->dl_flag = flag;
		p->dl_dest_addr_length = dst_len;
		p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src_len;
		p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		if (src_len) {
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- DL_XID_CON\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_DATA_ACK_IND             0x22
 *  -----------------------------------
 */
STATIC INLINE int
dl_data_ack_ind(queue_t *q, struct dl *dl, size_t dst_len, caddr_t dst_ptr, size_t src_len,
		caddr_t src_ptr, ulong prio, ulong sclass, mblk_t *dp)
{
	mblk_t *mp;
	dl_data_ack_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_DATA_ACK_IND;
		p->dl_dest_addr_length = dst_len;
		p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src_len;
		p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
		p->dl_priority = prio;
		p->dl_service_class = sclass;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		if (src_len) {
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- DL_DATA_ACK_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_DATA_ACK_STATUS_IND      0x23
 *  -----------------------------------
 */
STATIC INLINE int
dl_data_ack_status_ind(queue_t *q, struct dl *dl, ulong corr, ulong status)
{
	mblk_t *mp;
	dl_data_ack_status_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_DATA_ACK_STATUS_IND;
		p->dl_correlation = corr;
		p->dl_status = status;
		printd(("%s: %p: <- DL_DATA_ACK_STATUS_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_REPLY_IND                0x25
 *  -----------------------------------
 */
STATIC INLINE int
dl_reply_ind(queue_t *q, struct dl *dl, size_t dst_len, caddr_t dst_ptr, size_t src_len,
	     caddr_t src_ptr, ulong prio, ulong sclass, mblk_t *dp)
{
	mblk_t *mp;
	dl_reply_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len + src_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_REPLY_IND;
		p->dl_dest_addr_length = dst_len;
		p->dl_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		p->dl_src_addr_length = src_len;
		p->dl_src_addr_offset = src_len ? sizeof(*p) + dst_len : 0;
		p->dl_priority = prio;
		p->dl_service_class = sclass;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		if (src_len) {
			bcopy(src_ptr, mp->b_wptr, src_len);
			mp->b_wptr += src_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- DL_REPLY_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_REPLY_STATUS_IND         0x26
 *  -----------------------------------
 */
STATIC INLINE int
dl_reply_status_ind(queue_t *q, struct dl *dl, ulong corr, ulong status)
{
	mblk_t *mp;
	dl_reply_status_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_REPLY_STATUS_IND;
		p->dl_correlation = corr;
		p->dl_status = status;
		printd(("%s: %p: <- DL_REPLY_STATUS_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_REPLY_UPDATE_STATUS_IND  0x28
 *  -----------------------------------
 */
STATIC INLINE int
dl_reply_update_status_ind(queue_t *q, struct dl *dl, ulong corr, ulong status)
{
	mblk_t *mp;
	dl_reply_update_status_ind_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_REPLY_UPDATE_STATUS_IND;
		p->dl_correlation = corr;
		p->dl_status = status;
		printd(("%s: %p: <- DL_REPLY_UPDATE_STATUS_IND\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_PHYS_ADDR_ACK            0x32
 *  -----------------------------------
 */
STATIC INLINE int
dl_phys_addr_ack(queue_t *q, struct dl *dl, size_t add_len, caddr_t add_ptr)
{
	mblk_t *mp;
	dl_phys_addr_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + add_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_PHYS_ADDR_ACK;
		p->dl_addr_length = add_len;
		p->dl_addr_offset = add_len ? sizeof(*p) : 0;
		if (add_len) {
			bcopy(add_ptr, mp->b_wptr, add_len);
			mp->b_wptr += add_len;
		}
		printd(("%s: %p: <- DL_PHYS_ADDR_ACK\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  DL_GET_STATISTICS_ACK       0x35
 *  -----------------------------------
 */
STATIC INLINE int
dl_get_statistics_ack(queue_t *q, struct dl *dl, size_t sta_len, caddr_t sta_ptr)
{
	mblk_t *mp;
	dl_get_statistics_ack_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + sta_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->dl_primitive = DL_GET_STATISTICS_ACK;
		p->dl_stat_length = sta_len;
		p->dl_stat_offset = sta_len ? sizeof(*p) : 0;
		if (sta_len) {
			bcopy(sta_ptr, mp->b_wptr, sta_len);
			mp->b_wptr += sta_len;
		}
		printd(("%s: %p: <- DL_GET_STATISTICS_ACK\n", DL_LAPD_DRV_NAME, dl));
		putnext(dl->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitive sent to downstream
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  CD_INFO_REQ                 0x00 - Information request
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_info_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_info_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_INFO_REQ;
		printd(("%s: %p: <- CD_INFO_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_ATTACH_REQ               0x02 - Attach a PPA
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_attach_req(queue_t *q, struct cd *cd, struct dl *dl)
{
	mblk_t *mp;
	cd_attach_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_ATTACH_REQ;
		p->cd_ppa = cd->ppa;
		dl_wait_link(dl, cd);	/* put the dl into the wait list */
		printd(("%s: %p: <- CD_ATTACH_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_DETACH_REQ               0x03 - Detach a PPA
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_detach_req(queue_t *q, struct cd *cd, struct dl *dl)
{
	mblk_t *mp;
	cd_detach_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_DETACH_REQ;
		dl_wait_link(dl, cd);	/* put the dl into the wait list */
		printd(("%s: %p: <- CD_DETACH_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_ENABLE_REQ               0x04 - Prepare a device
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_enable_req(queue_t *q, struct cd *cd, struct dl *dl)
{
	mblk_t *mp;
	cd_enable_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_ENABLE_REQ;
		p->cd_dial_type = CD_NODIAL;
		p->cd_dial_length = 0;
		p->cd_dial_offset = 0;
		cd_set_state(cd, CD_ENABLE_PENDING);
		dl_wait_link(dl, cd);	/* put the dl into the wait list */
		printd(("%s: %p: <- CD_ENABLE_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_DISABLE_REQ              0x05 - Disable a device
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_disable_req(queue_t *q, struct cd *cd, struct dl *dl, ulong disposal)
{
	mblk_t *mp;
	cd_disable_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_DISABLE_REQ;
		p->cd_disposal = disposal;
		cd_set_state(cd, CD_DISABLE_PENDING);
		dl_wait_link(dl, cd);	/* put the dl into the wait list */
		printd(("%s: %p: <- CD_DISABLE_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_ALLOW_INPUT_REQ          0x0b - Allow input
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_allow_input_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_allow_input_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_ALLOW_INPUT_REQ;
		cd_set_state(cd, CD_INPUT_ALLOWED);
		printd(("%s: %p: <- CD_ALLOW_INPUT_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_READ_REQ                 0x0c - Wait-for-input request
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_read_req(queue_t *q, struct cd *cd, ulong msec)
{
	mblk_t *mp;
	cd_read_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_READ_REQ;
		p->cd_msec = msec;
		cd_set_state(cd, CD_READ_ACTIVE);
		printd(("%s: %p: <- CD_READ_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_UNITDATA_REQ             0x0d - Data send request
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_unitdata_req(queue_t *q, struct cd *cd, ulong atype, ulong prio, size_t dst_len, caddr_t dst_ptr,
		mblk_t *dp)
{
	mblk_t *mp;
	cd_unitdata_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_UNITDATA_REQ;
		p->cd_addr_type = atype;
		p->cd_priority = prio;
		p->cd_dest_addr_length = dst_len;
		p->cd_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- CD_UNITDATA_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_WRITE_READ_REQ           0x0e - Write/read request
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_write_read_req(queue_t *q, struct cd *cd, ulong atype, ulong prio, size_t dst_len,
		  caddr_t dst_ptr, mblk_t *dp, ulong msec)
{
	mblk_t *mp;
	cd_write_read_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p) + dst_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_WRITE_READ_REQ;
		p->cd_unitdata_req.cd_primitive = CD_UNITDATA_REQ;
		p->cd_unitdata_req.cd_addr_type = atype;
		p->cd_unitdata_req.cd_priority = prio;
		p->cd_unitdata_req.cd_dest_addr_length = dst_len;
		p->cd_unitdata_req.cd_dest_addr_offset = dst_len ? sizeof(*p) : 0;
		p->cd_read_req.cd_primitive = CD_READ_REQ;
		p->cd_read_req.cd_msec = msec;
		if (dst_len) {
			bcopy(dst_ptr, mp->b_wptr, dst_len);
			mp->b_wptr += dst_len;
		}
		mp->b_cont = dp;
		printd(("%s: %p: <- CD_WRITE_READ_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_HALT_INPUT_REQ           0x11 - Halt input
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_halt_input_req(queue_t *q, struct cd *cd, ulong disposal)
{
	mblk_t *mp;
	cd_halt_input_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_HALT_INPUT_REQ;
		p->cd_disposal = disposal;
		cd_set_state(cd, CD_ENABLED);
		printd(("%s: %p: <- CD_HALT_INPUT_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_ABORT_OUTPUT_REQ         0x12 - Abort output
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_abort_output_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_abort_output_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_ABORT_OUTPUT_REQ;
		printd(("%s: %p: <- CD_ABORT_OUTPUT_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

#if 0
/*
 *  CD_MUX_NAME_REQ             0x13 - get mux name (Gcom)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_mux_name_req(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_mux_name_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_MUX_NAME_REQ;
		printd(("%s: %p: <- CD_MUX_NAME_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}
#endif

/*
 *  CD_MODEM_SIG_REQ            0x15 - Assert modem signals (Gcom)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_modem_sig_req(queue_t *q, struct cd *cd, ulong sigs)
{
	mblk_t *mp;
	cd_modem_sig_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_MODEM_SIG_REQ;
		p->cd_sigs = sigs;
		printd(("%s: %p: <- CD_MODEM_SIG_REQ\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  CD_MODEM_SIG_POLL           0x17 - requests a CD_MODEM_SIG_IND (Gcom)
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_modem_sig_poll(queue_t *q, struct cd *cd)
{
	mblk_t *mp;
	cd_modem_sig_poll_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->cd_primitive = CD_MODEM_SIG_POLL;
		printd(("%s: %p: <- CD_MODEM_SIG_POLL\n", DL_LAPD_DRV_NAME, cd));
		putnext(cd->oq, mp);
		return (QR_DONE);
	}
	rare();
	return (-ENOBUFS);
}

/*
 *  =========================================================================
 *
 *  STATE MACHINES
 *
 *  =========================================================================
 */

/*
 *  -------------------------------------------------------------------------
 *
 *  Build Message Functions
 *
 *  -------------------------------------------------------------------------
 */

STATIC INLINE void
build_addr(mblk_t *mp, uint sapi, uint cr, uint tei)
{
	*mp->b_wptr++ = (sapi << 2) | (cr << 1) | (0x00);
	*mp->b_wptr++ = (tei << 1) | (0x01);
}
STATIC INLINE void
build_I_header(mblk_t *mp, uint sapi, uint cr, uint tei, uint pf, uint nr, uint ns)
{
	build_addr(mp, sapi, cr, tei);
	*mp->b_wptr++ = ns << 1;
	*mp->b_wptr++ = (nr << 1) | pf;
}
STATIC INLINE void
build_S_header(mblk_t *mp, uint sapi, uint cr, uint tei, uint pf, uint nr, uint s)
{
	build_addr(mp, sapi, cr, tei);
	*mp->b_wptr++ = (s << 2) | 0x01;
	*mp->b_wptr++ = (nr << 1) | pf;
}
STATIC INLINE void
build_U_header(mblk_t *mp, uint sapi, uint cr, uint tei, uint pf, uint m)
{
	build_addr(mp, sapi, cr, tei);
	*mp->b_wptr++ = (m << 2) | (pf << 4) | 0x03;
}

STATIC mblk_t *
build_msg(queue_t *q, ulong prio, size_t len)
{
	mblk_t *mp;
	cd_unitdata_req_t *p;
	if ((mp = ss7_allocb(q, sizeof(*p), BPRI_MED))) {
		mblk_t *bp;
		if ((bp = ss7_allocb(q, len, BPRI_MED))) {
			mp->b_datap->db_type = M_PROTO;
			mp->b_band = prio;
			p = (typeof(p)) mp->b_wptr++;
			bzero(p, sizeof(*p));
			p->cd_primitive = CD_UNITDATA_REQ;
			p->cd_priority = prio;
			p->cd_addr_type = CD_IMPLICIT;
			p->cd_dest_addr_length = 0;
			p->cd_dest_addr_offset = 0;
			mp->b_cont = bp;
			bp->b_datap->db_type = M_DATA;
			mp->b_band = prio;
			return (QR_DONE);
		}
		freemsg(mp);
	}
	return (NULL);

}

/*
 *  SEND I (Information)
 *  -----------------------------------
 */
STATIC INLINE int
send_I_cmd(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf, uint nr, uint ns, mblk_t *dp)
{
	/*
	   a dl entity receiving a I with the P bit set to 1 shall set the F bit to 1 in the next
	   RR, RNR, REJ (or FRMR or DM in LAPB) frame it transmits 
	 */
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 4))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_I_header(mp->b_cont, sapi, cr, tei, pf, nr, ns);
			linkb(mp, dp);
			putnext(cd->oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND RR (Receive Ready) command
 *  -----------------------------------
 */
STATIC INLINE int
send_RR_cmd(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	/*
	   a dl entity receiving a RR with the P bit set to 1 shall set the F bit to 1 in the next
	   RR, RNR, REJ frame it transmits 
	 */
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 4))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_S_header(mp->b_cont, sapi, cr, tei, pf, nr, 0x00);
			putnext(cd->oq, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND RR (Receive Ready) response
 *  -----------------------------------
 */
STATIC INLINE int
send_RR_res(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 4))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 1 : 0;
			build_S_header(mp->b_cont, sapi, cr, tei, pf, nr, 0x00);
			putnext(cd->oq, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND RNR (Receive Not Ready) command
 *  -----------------------------------
 */
STATIC INLINE int
send_RNR_cmd(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	/*
	   a dl entity receiving a RNR with the P bit set to 1 shall set the F bit to 1 in the next 
	   RR, RNR, REJ frame it transmits 
	 */
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 4))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_S_header(mp->b_cont, sapi, cr, tei, pf, nr, 0x01);
			putnext(cd->oq, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND RNR (Receive Not Ready) response
 *  -----------------------------------
 */
STATIC INLINE int
send_RNR_res(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 4))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 1 : 0;
			build_S_header(mp->b_cont, sapi, cr, tei, pf, nr, 0x01);
			putnext(cd->oq, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND REJ (Reject) command
 *  -----------------------------------
 */
STATIC INLINE int
send_REJ_cmd(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	/*
	   a dl entity receiving a REJ with the P bit set to 1 shall set the F bit to 1 in the next 
	   RR, RNR, REJ frame it transmits 
	 */
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 4))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_S_header(mp->b_cont, sapi, cr, tei, pf, nr, 0x02);
			putnext(cd->oq, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND REJ (Reject) response
 *  -----------------------------------
 */
STATIC INLINE int
send_REJ_res(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 4))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 1 : 0;
			build_S_header(mp->b_cont, sapi, cr, tei, pf, nr, 0x02);
			putnext(cd->oq, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND UI (Unnumbered Information) command
 *  -----------------------------------
 */
STATIC INLINE int
send_UI_cmd(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf, mblk_t *dp)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 3))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			/*
			   the P bit shall be set to zero 
			 */
			assure(pf == 0);
			build_U_header(mp->b_cont, sapi, cr, tei, pf, 0x00);
			linkb(mp, dp);
			putnext(cd->oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND DM (Disconnected Mode) response
 *  -----------------------------------
 */
STATIC INLINE int
send_DM_res(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 3))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 1 : 0;
			build_U_header(mp->b_cont, sapi, cr, tei, pf, 0x03);
			putnext(cd->oq, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND DISC (Disconnect) command
 *  -----------------------------------
 */
STATIC INLINE int
send_DISC_cmd(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf)
{
	/*
	   a dl entity receiving a DISC with the P bit set to 1 shall set the F bit to 1 in the
	   next UA or DM frame it transmits 
	 */
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 3))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_U_header(mp->b_cont, sapi, cr, tei, pf, 0x10);
			putnext(cd->oq, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND UA (Unnumbered Acknowledgeemnt) response
 *  -----------------------------------
 */
STATIC INLINE int
send_UA_res(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 3))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 1 : 0;
			build_U_header(mp->b_cont, sapi, cr, tei, pf, 0x18);
			putnext(cd->oq, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND SABME (Set Asynchronous Balanced Mode Extended) command
 *  -----------------------------------
 */
STATIC INLINE int
send_SABME_cmd(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf)
{
	/*
	   a dl entity receiving a SABME with the P bit set to 1 shall set the F bit to 1 in the
	   next UA or DM frame it transmits 
	 */
	/*
	   no information field is permitted with a SAMBE 
	 */
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 3))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_U_header(mp->b_cont, sapi, cr, tei, pf, 0x1b);
			putnext(cd->oq, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND FRMR (Frame Reject) response
 *  -----------------------------------
 */
STATIC INLINE int
send_FRMR_res(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf, uchar *ctl, uint vs, uint vr,
	      uint oldcr, uint flags)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 3))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 1 : 0;
			build_U_header(mp->b_cont, sapi, cr, tei, pf, 0x21);
			*mp->b_wptr++ = ctl[0];
			*mp->b_wptr++ = ctl[1];
			*mp->b_wptr++ = (vs << 1) | 0x00;
			*mp->b_wptr++ = (vr << 1) | oldcr;
			*mp->b_wptr++ = flags;
			putnext(cd->oq, mp);
			return (QR_DONE);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND XID (Exchange Identification) command
 *  -----------------------------------
 */
STATIC INLINE int
send_XID_cmd(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf, mblk_t *dp)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 3))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_U_header(mp->b_cont, sapi, cr, tei, pf, 0x2b);
			linkb(mp, dp);
			putnext(cd->oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  SEND XID (Exchange Identification) response
 *  -----------------------------------
 */
STATIC INLINE int
send_XID_res(queue_t *q, struct cd *cd, uint sapi, uint tei, uint pf, mblk_t *dp)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 3))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 1 : 0;
			build_U_header(mp->b_cont, sapi, cr, tei, pf, 0x2b);
			linkb(mp, dp);
			putnext(cd->oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

#define MEI			0x0f
#define TEI_ANY			127
#define TEI_ALL			127
#define TEI_MGMT_SAPI		63
#define TEI_MGMT_TEI		127
#define LAPD_TEI_MT_I_REQ	0x01	/* Identity request (user to network) */
#define LAPD_TEI_MT_I_ACK	0x02	/* Identity assigned (network to user) */
#define LAPD_TEI_MT_I_REJ	0x03	/* Identity denied (network to user) */
#define LAPD_TEI_MT_C_REQ	0x04	/* Identity check request (network to user) */
#define LAPD_TEI_MT_C_RES	0x05	/* Identity check response (user to network) */
#define LAPD_TEI_MT_I_RMV	0x06	/* Identity remove (network to user) */
#define LAPD_TEI_MT_I_VER	0x07	/* Identity verify (user to network) */

STATIC INLINE int
send_TEI_i_req(queue_t *q, struct cd *cd, ushort ref, uchar ai)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 8))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_U_header(mp->b_cont, TEI_MGMT_SAPI, cr, TEI_MGMT_TEI, 0, 0x00);
			*mp->b_cont->b_wptr++ = MEI;
			*mp->b_cont->b_wptr++ = ref;
			*mp->b_cont->b_wptr++ = ref >> 8;
			*mp->b_cont->b_wptr++ = LAPD_TEI_MT_I_REQ;
			*mp->b_cont->b_wptr++ = (ai << 1) | 0x01;
			putnext(cd->oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
STATIC INLINE int
send_TEI_i_ack(queue_t *q, struct cd *cd, ushort ref, uchar ai)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 8))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_U_header(mp->b_cont, TEI_MGMT_SAPI, cr, TEI_MGMT_TEI, 0, 0x00);
			*mp->b_cont->b_wptr++ = MEI;
			*mp->b_cont->b_wptr++ = ref;
			*mp->b_cont->b_wptr++ = ref >> 8;
			*mp->b_cont->b_wptr++ = LAPD_TEI_MT_I_ACK;
			*mp->b_cont->b_wptr++ = (ai << 1) | 0x01;
			putnext(cd->oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
STATIC INLINE int
send_TEI_i_rej(queue_t *q, struct cd *cd, ushort ref, uchar ai)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 8))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_U_header(mp->b_cont, TEI_MGMT_SAPI, cr, TEI_MGMT_TEI, 0, 0x00);
			*mp->b_cont->b_wptr++ = MEI;
			*mp->b_cont->b_wptr++ = ref;
			*mp->b_cont->b_wptr++ = ref >> 8;
			*mp->b_cont->b_wptr++ = LAPD_TEI_MT_I_REJ;
			*mp->b_cont->b_wptr++ = (ai << 1) | 0x01;
			putnext(cd->oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
STATIC INLINE int
send_TEI_c_req(queue_t *q, struct cd *cd, uchar ai)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 8))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_U_header(mp->b_cont, TEI_MGMT_SAPI, cr, TEI_MGMT_TEI, 0, 0x00);
			*mp->b_cont->b_wptr++ = MEI;
			*mp->b_cont->b_wptr++ = 0;
			*mp->b_cont->b_wptr++ = 0;
			*mp->b_cont->b_wptr++ = LAPD_TEI_MT_C_REQ;
			*mp->b_cont->b_wptr++ = (ai << 1) | 0x01;
			putnext(cd->oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
STATIC INLINE int
send_TEI_c_res(queue_t *q, struct cd *cd, ushort ref, uchar *ai_ptr, size_t ai_len)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 7 + ai_len))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_U_header(mp->b_cont, TEI_MGMT_SAPI, cr, TEI_MGMT_TEI, 0, 0x00);
			*mp->b_cont->b_wptr++ = MEI;
			*mp->b_cont->b_wptr++ = ref;
			*mp->b_cont->b_wptr++ = ref >> 8;
			*mp->b_cont->b_wptr++ = LAPD_TEI_MT_C_RES;
			while (ai_len--)
				*mp->b_cont->b_wptr++ = ((*ai_ptr++) << 1) | (ai_len ? 0 : 1);
			putnext(cd->oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
STATIC INLINE int
send_TEI_i_rmv(queue_t *q, struct cd *cd, uchar ai)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 8))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_U_header(mp->b_cont, TEI_MGMT_SAPI, cr, TEI_MGMT_TEI, 0, 0x00);
			*mp->b_cont->b_wptr++ = MEI;
			*mp->b_cont->b_wptr++ = 0;
			*mp->b_cont->b_wptr++ = 0;
			*mp->b_cont->b_wptr++ = LAPD_TEI_MT_I_RMV;
			*mp->b_cont->b_wptr++ = (ai << 1) | 0x01;
			putnext(cd->oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}
STATIC INLINE int
send_TEI_i_ver(queue_t *q, struct cd *cd, uchar ai)
{
	if (canputnext(cd->oq)) {
		mblk_t *mp;
		if ((mp = build_msg(q, 0, 8))) {
			uint cr = (cd->mode != CD_MODE_NTWK) ? 0 : 1;
			build_U_header(mp->b_cont, TEI_MGMT_SAPI, cr, TEI_MGMT_TEI, 0, 0x00);
			*mp->b_cont->b_wptr++ = MEI;
			*mp->b_cont->b_wptr++ = 0;
			*mp->b_cont->b_wptr++ = 0;
			*mp->b_cont->b_wptr++ = LAPD_TEI_MT_I_VER;
			*mp->b_cont->b_wptr++ = (ai << 1) | 0x01;
			putnext(cd->oq, mp);
			return (QR_ABSORBED);
		}
		return (-ENOBUFS);
	}
	return (-EBUSY);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Receive Message Functions
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  RECV I (Information)
 *  -----------------------------------
 */
STATIC int
recv_I_cmd(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf, uint nr, uint ns)
{
	/*
	   a dl entity receiving a I with the P bit set to 1 shall set the F bit to 1 in the next
	   RR, RNR, REJ (or FRMR or DM in LAPB) frame it transmits 
	 */
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV RR (Receive Ready) command
 *  -----------------------------------
 */
STATIC int
recv_RR_cmd(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	/*
	   a dl entity receiving a RR with the P bit set to 1 shall set the F bit to 1 in the next
	   RR, RNR, REJ frame it transmits 
	 */
	if (pf) {
	} else {
	}
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV RR (Receive Ready) response
 *  -----------------------------------
 */
STATIC int
recv_RR_res(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	if (pf) {
	} else {
	}
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV RNR (Receive Not Ready) command
 *  -----------------------------------
 */
STATIC int
recv_RNR_cmd(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	/*
	   a dl entity receiving a RNR with the P bit set to 1 shall set the F bit to 1 in the next 
	   RR, RNR, REJ frame it transmits 
	 */
	if (pf) {
	} else {
	}
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV RNR (Receive Not Ready) response
 *  -----------------------------------
 */
STATIC int
recv_RNR_res(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	if (pf) {
	} else {
	}
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV REJ (Reject) command
 *  -----------------------------------
 */
STATIC int
recv_REJ_cmd(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	/*
	   a dl entity receiving a REJ with the P bit set to 1 shall set the F bit to 1 in the next 
	   RR, RNR, REJ frame it transmits 
	 */
	if (pf) {
	} else {
	}
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV REJ (Reject) response
 *  -----------------------------------
 */
STATIC int
recv_REJ_res(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf, uint nr)
{
	if (pf) {
	} else {
	}
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV UI (Unnumbered Information) command
 *  -----------------------------------
 */
STATIC int
recv_UI_cmd(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf)
{
	/*
	   the P bit shall be set to zero 
	 */
	assure(pf == 0);
	if (sapi == 63 && tei == 127 && mp->b_rptr[3] == 0x0f) {
		struct dl *dl;
		ulong ref;
		uchar ai;
		int err;
		ref = mp->b_rptr[5];
		ref <<= 8;
		ref |= mp->b_rptr[4];
		ai = (mp->b_rptr[7] >> 1) & 0x7f;
		switch (mp->b_rptr[6]) {
		case LAPD_TEI_MT_I_REQ:	/* Identity request (user to network) */
			if (cd->mode == CD_MODE_USER)
				goto discard;
			if (ai == 127) {
				/*
				   pick any TEI 
				 */
			} else {
				/*
				   requested TEI 
				 */
			}
			break;
		case LAPD_TEI_MT_I_ACK:	/* Identity assigned (network to user) */
			if (cd->mode == CD_MODE_NTWK)
				goto discard;
			if (ai < 64 || ai > 126)
				goto discard;
			/*
			   find requesting data link 
			 */
			for (dl = cd->teim.list; dl && dl->ref != ref; dl = dl->teim.next) ;
			if (!dl)
				goto discard;
			/*
			   accept assigned value 
			 */
			switch (dl->state) {
			case LAPD_WAIT_TEI_ASSIGN:	/* was connectionless */
				dl_timer_stop(dl, t202);
				switch (dl_get_state(dl)) {
				case DL_SUBS_BIND_PND:
					if ((err = dl_subs_bind_ack(q, dl, ai)))
						return (err);
					/*
					   fall through 
					 */
				case DL_IDLE:
					dl->dlc.dl_tei = ai;
					if (dl->bind.cd) {
						dl_bind_unlink(dl);
						dl_bind_link(dl, cd);
					}
					dl->state = LAPD_TEI_ASSIGNED;
					dl_teim_unlink(dl);
					return (QR_DONE);
				}
				goto discard;
			case LAPD_WAIT_TEI_ESTABLISH:	/* was connection oriented */
				dl_timer_stop(dl, t202);
				switch (dl_get_state(dl)) {
				case DL_OUTCON_PENDING:
					if ((err = send_SABME_cmd(q, cd, dl->dlc.dl_sap, ai, 1)))
						return (err);
					dl->dlc.dl_tei = ai;
					if (dl->bind.cd) {
						dl_bind_unlink(dl);
						dl_bind_link(dl, cd);
					}
					if (dl->list.cd) {
						dl_list_unlink(dl);
						dl_list_link(dl, cd);
					}
					dl->state = LAPD_TEI_ASSIGNED;
					dl_teim_unlink(dl);
					dl->rc = 0;
					dl_timer_start(dl, t200);
					dl_conn_link(dl, cd);
					dl->state = LAPD_WAIT_ESTABLISH;
					dl_set_state(dl, DL_OUTCON_PENDING);
					return (QR_DONE);
				}
				goto discard;
			default:
				goto discard;
			}
			break;
		case LAPD_TEI_MT_I_REJ:	/* Identity denied (network to user) */
			if (cd->mode == CD_MODE_NTWK)
				goto discard;
			/*
			   find requesting data link 
			 */
			/*
			   ai doesn't matter really 
			 */
			break;
		case LAPD_TEI_MT_C_REQ:	/* Identity check request (network to user) */
			if (cd->mode == CD_MODE_NTWK)
				goto discard;
			/*
			   no reference 
			 */
			break;
		case LAPD_TEI_MT_C_RES:	/* Identity check response (user to network) */
			if (cd->mode == CD_MODE_USER)
				goto discard;
			break;
		case LAPD_TEI_MT_I_RMV:	/* Identity remove (network to user) */
			if (cd->mode == CD_MODE_NTWK)
				goto discard;
			/*
			   no reference 
			 */
			break;
		case LAPD_TEI_MT_I_VER:	/* Identity verify (user to network) */
			if (cd->mode == CD_MODE_USER)
				goto discard;
			/*
			   no reference 
			 */
			break;
		default:
			goto discard;
		}
	} else {
	}
	fixme(("Write this function!\n"));
	return (-EIO);
      discard:
	return (QR_DONE);
}

/*
 *  RECV DM (Disconnected Mode) response
 *  -----------------------------------
 */
STATIC int
recv_DM_res(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf)
{
	struct dl *dl;
	/*
	   Upon reception of a DM response with the F bit set to 1, the originator of the SABME
	   command shall indicate this to layer 3 by means of a DL-RELEASE indication primitive,
	   and reset timer T200. It shall the enter the TEI-assigned state.  DM responses with the
	   F bit set to zero shall be ignored in this case. 
	 */
	if (!pf)
		goto ignore;
	for (dl = cd->conn.hash[((sapi + tei) & DL_CONN_HASHMASK)];
	     dl && (dl->dlc.dl_sap != sapi || dl->dlc.dl_tei != tei); dl = dl->conn.next) ;
	if (!dl)
		goto unexpected;
	switch (dl->state) {
	case LAPD_TEI_UNASSIGNED:
		fixme(("Write this procedure\n"));
		break;
	case LAPD_WAIT_TEI_ASSIGN:
		fixme(("Write this procedure\n"));
		break;
	case LAPD_TEI_ASSIGNED:
		/*
		   5.5.4 While in the TEI-assigned state, on receipt of an unsolicited DM response
		   with the F bit set to 0, the data link layer entity shall, if it is able to,
		   initiate the esnablishment procedures by the transmission of a SABME (see
		   5.5.1.2).  Otherwise, the DM shall be ignored 
		 */
		if (!pf)
			goto ignore;
		goto unexpected;
	case LAPD_WAIT_ESTABLISH:
		dl_timer_stop(dl, t200);
		return dl_disconnect_ind(q, dl, DL_USER, DL_DISC_UNSPECIFIED, NULL);
	case LAPD_WAIT_RELEASE:
		fixme(("Write this procedure\n"));
		break;
	case LAPD_ESTABLISHED:
		fixme(("Write this procedure\n"));
		break;
	case LAPD_TIME_RECOVERY:
		fixme(("Write this procedure\n"));
		break;
	default:
		swerr();
		break;
	}
      unexpected:
	fixme(("Write this function!\n"));
	return (-EIO);
      ignore:
	return (QR_DONE);
}

/*
 *  RECV DISC (Disconnect) command
 *  -----------------------------------
 */
STATIC int
recv_DISC_cmd(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf)
{
	struct dl *dl;
	/*
	   a dl entity receiving a DISC with the P bit set to 1 shall set the F bit to 1 in the
	   next UA or DM frame it transmits 
	 */
	for (dl = cd->conn.hash[((sapi + tei) & DL_CONN_HASHMASK)];
	     dl && (dl->dlc.dl_sap != sapi || dl->dlc.dl_tei != tei); dl = dl->conn.next) ;
	if (!dl)
		goto unexpected;
	switch (dl->state) {
	case LAPD_TEI_UNASSIGNED:
		fixme(("Write this procedure\n"));
		break;
	case LAPD_WAIT_TEI_ASSIGN:
		fixme(("Write this procedure\n"));
		break;
	case LAPD_TEI_ASSIGNED:
		/*
		   5.5.4 While in the TEI-assigned state, the receipt of a DISC command shall
		   result in the transmission of a DM response with the F bit set to the value of
		   the received P bit 
		 */
		return send_DM_res(q, cd, sapi, tei, pf);
	case LAPD_WAIT_ESTABLISH:
		fixme(("Write this procedure\n"));
		break;
	case LAPD_WAIT_RELEASE:
		fixme(("Write this procedure\n"));
		break;
	case LAPD_ESTABLISHED:
		fixme(("Write this procedure\n"));
		break;
	case LAPD_TIME_RECOVERY:
		fixme(("Write this procedure\n"));
		break;
	default:
		swerr();
		break;
	}
      unexpected:
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV UA (Unnumbered Acknowledgeemnt) response
 *  -----------------------------------
 */
STATIC int
recv_UA_res(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf)
{
	struct dl *dl;
	/*
	   upon reception of a UA response with the F-bit set to 1, the originator of the SABME
	   command shall: reset timer T200; start timer T203, if implemented; set V(S), V(R) and
	   V(A) to 0; and enter the multiple-frame-established state and inform layer 3 using the
	   DL-ESTABLISH confirm primitive. 
	 */
	for (dl = cd->conn.hash[((sapi + tei) & DL_CONN_HASHMASK)];
	     dl && (dl->dlc.dl_sap != sapi || dl->dlc.dl_tei != tei); dl = dl->conn.next) ;
	if (!dl)
		goto unexpected;
	if (dl_get_state(dl) == DL_OUTCON_PENDING) {
		struct lapd_addr a = { dl_sap:sapi, dl_tei:tei, };
		size_t alen = sizeof(a);
		caddr_t aptr = (caddr_t) &a;
		return dl_connect_con(q, dl, alen, aptr);
	}
      unexpected:
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV SABME (Set Asynchronous Balanced Mode Extended) command
 *  -----------------------------------
 */
STATIC int
recv_SABME_cmd(queue_t *q, mblk_t *mp, struct cd *cd, uchar sapi, uchar tei, uint pf)
{
	struct dl *dl;
	/*
	   a dl entity receiving a SABME with the P bit set to 1 shall set the F bit to 1 in the
	   next UA or DM frame it transmits 
	 */
	/*
	   no information field is permitted with a SAMBE 
	 */
	if (msgdsize(mp) > 3)
		return (-EINVAL);
	for (dl = cd->conn.hash[((sapi + tei) & DL_CONN_HASHMASK)];
	     dl && (dl->dlc.dl_sap != sapi || dl->dlc.dl_tei != tei); dl = dl->conn.next) ;
	if (dl)
		goto discard;	/* dl already in the connected state, this could be late SABME,
				   discard it */
	for (dl = cd->list.hash[((sapi + tei) & DL_LIST_HASHMASK)];
	     dl && (dl->dlc.dl_sap != sapi || dl->dlc.dl_tei != tei); dl = dl->list.next) ;
	if (!dl)
		dl = cd->mgmt;
	if (dl && ((1 << dl_get_state(dl)) & (DLF_IDLE | DLF_INCON_PENDING))) {
		/*
		   dl in listening state 
		 */
		mblk_t *cp;
		for (cp = dl->conq.q_head;
		     cp && (((cp->b_rptr[0] >> 2) & 0x3f) != sapi
			    || ((cp->b_rptr[1] >> 1) & 0x7f) != tei); cp = cp->b_next) ;
		if (cp)
			goto discard;	/* already processing connection indication, discard
					   duplicate */
		if (dl->conq.q_msgs < dl->conind) {
			struct lapd_addr a = { dl_sap:sapi, dl_tei:tei, };
			size_t alen = sizeof(a);
			caddr_t aptr = (caddr_t) &a;
			return dl_connect_ind(q, dl, mp, alen, aptr, alen, aptr);
		}
	}
	/*
	   If the data link layer entity is unable to enter the multiple-frame-established state,
	   it shall respond to the SABME command with a DM response with the F bit set to the same
	   binary value as the P bit in the received SABME command. 
	 */
	return send_DM_res(q, cd, sapi, tei, pf);
      discard:
	return (QR_DONE);
}

/*
 *  RECV FRMR (Frame Reject) response
 *  -----------------------------------
 */
STATIC int
recv_FRMR_res(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf)
{
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV XID (Exchange Identification) command
 *  -----------------------------------
 */
STATIC int
recv_XID_cmd(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf)
{
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV XID (Exchange Identification) response
 *  -----------------------------------
 */
STATIC int
recv_XID_res(queue_t *q, mblk_t *mp, struct cd *cd, uint sapi, uint tei, uint pf)
{
	fixme(("Write this function!\n"));
	return (-EIO);
}

/*
 *  RECV MSG - Receive a LAPD message
 *  -----------------------------------
 */
STATIC int
recv_msg(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	size_t dlen = msgdsize(mp);
	int err;
	uint sapi, cr, tei, ctl, ns, nr, s, m, pf;
	(void) recv_msg;
	if (dlen < 3)
		goto discard;
	if (mp->b_cont && mp->b_wptr < mp->b_rptr + FASTBUF
	    && (err = ss7_pullupmsg(q, mp, FASTBUF)))
		return (err);
	sapi = mp->b_rptr[0];
	if (sapi & 0x01)	/* invlaid one-octet address */
		goto discard;
	cr = (sapi & 0x02) ? 1 : 0;
	if (cd->mode != CD_MODE_NTWK)
		cr ^= 0x1;
	sapi >>= 2;
	sapi &= 0x3f;
	tei = mp->b_rptr[1];
	if (!(tei & 0x01))	/* invalid three+ octet address */
		goto discard;
	tei >>= 1;
	tei &= 0x7f;
	ctl = mp->b_rptr[2];
	switch (ctl & 0x03) {
	case 0x00:
	case 0x02:		/* I format */
	{
		if (dlen < 4)
			goto discard;
		ns = (mp->b_rptr[2] >> 1) & 0x7f;
		nr = (mp->b_rptr[3] >> 1) & 0x7f;
		pf = (mp->b_rptr[3] & 0x1);
		if (cr)
			return recv_I_cmd(q, mp, cd, sapi, tei, pf, nr, ns);
		else
			goto reject;
		break;
	}
	case 0x01:		/* S format */
	{
		if (dlen < 4)
			goto discard;
		s = (mp->b_rptr[2] >> 2) & 0x03;
		nr = (mp->b_rptr[3] >> 1) & 0x7f;
		pf = (mp->b_rptr[3] & 0x1);
		switch (s) {
		case 0x00:	/* RR (receive ready) */
			if (cr)
				return recv_RR_cmd(q, mp, cd, sapi, tei, pf, nr);
			else
				return recv_RR_res(q, mp, cd, sapi, tei, pf, nr);
			break;
		case 0x01:	/* RNR (receive not ready) */
			if (cr)
				return recv_RNR_cmd(q, mp, cd, sapi, tei, pf, nr);
			else
				return recv_RNR_res(q, mp, cd, sapi, tei, pf, nr);
			break;
		case 0x02:	/* REJ (reject) */
			if (cr)
				return recv_REJ_cmd(q, mp, cd, sapi, tei, pf, nr);
			else
				return recv_REJ_res(q, mp, cd, sapi, tei, pf, nr);
			break;
		case 0x03:
			goto reject;
		}
		break;
	}
	case 0x03:		/* U format */
	{
		m = (mp->b_rptr[2] >> 2) & 0x3b;
		pf = (mp->b_rptr[2] >> 2) & 0x1;
		switch (m) {
		case 0x00:	/* UI (unnumbered information) */
			if (cr)
				return recv_UI_cmd(q, mp, cd, sapi, tei, pf);
			else
				goto reject;
			break;
		case 0x03:	/* DM (disconnected mode) */
			if (cr)
				goto reject;
			else
				return recv_DM_res(q, mp, cd, sapi, tei, pf);
			break;
		case 0x10:	/* DISC (disconnect) */
			if (cr)
				return recv_DISC_cmd(q, mp, cd, sapi, tei, pf);
			else
				goto reject;
			break;
		case 0x18:	/* UA (unnumbered acknowledgement) */
			if (cr)
				goto reject;
			else
				return recv_UA_res(q, mp, cd, sapi, tei, pf);
			break;
		case 0x1b:	/* SABME */
			if (cr)
				return recv_SABME_cmd(q, mp, cd, sapi, tei, pf);
			else
				goto reject;
			break;
		case 0x21:	/* FRMR (frame reject) */
			if (dlen < 8)
				goto discard;
			if (cr)
				goto reject;
			else
				return recv_FRMR_res(q, mp, cd, sapi, tei, pf);
			break;
		case 0x2b:	/* XID (Exchange identification) */
			if (cr)
				return recv_XID_cmd(q, mp, cd, sapi, tei, pf);
			else
				return recv_XID_res(q, mp, cd, sapi, tei, pf);
			break;
		}
		break;
	}
	}
      discard:
	return (QR_DONE);
      reject:
	return (-ENXIO);
}

/*
 *  =========================================================================
 *
 *  PRIVATE STRUCTURE ALLOCATION AND DEALLOCATION
 *
 *  =========================================================================
 */
STATIC kmem_cache_t *dl_priv_cachep = NULL;
STATIC kmem_cache_t *dl_link_cachep = NULL;

STATIC void
lapd_term_caches(void)
{
	if (dl_priv_cachep) {
		if (kmem_cache_destroy(dl_priv_cachep))
			cmn_err(CE_WARN, "%s: did not destroy dl_priv_cachep", DL_LAPD_DRV_NAME);
		else {
			printd(("%s: destroyed dl_priv_cachep\n", DL_LAPD_DRV_NAME));
			dl_priv_cachep = NULL;
		}
	}
	if (dl_link_cachep) {
		if (kmem_cache_destroy(dl_link_cachep))
			cmn_err(CE_WARN, "%s: did not destroy dl_link_cachep", DL_LAPD_DRV_NAME);
		else {
			printd(("%s: destroyed dl_link_cachep\n", DL_LAPD_DRV_NAME));
			dl_link_cachep = NULL;
		}
	}
	return;
}
STATIC int
lapd_init_caches(void)
{
	if (!dl_priv_cachep
	    && !(dl_priv_cachep =
		 kmem_cache_create("dl_priv_cachep", sizeof(struct dl), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate dl_priv_cachep", DL_LAPD_DRV_NAME);
		lapd_term_caches();
		return (-ENOMEM);
	} else
		printd(("%s: initialized dl priv structure cache\n", DL_LAPD_DRV_NAME));
	if (!dl_link_cachep
	    && !(dl_link_cachep =
		 kmem_cache_create("dl_link_cachep", sizeof(struct cd), 0, SLAB_HWCACHE_ALIGN, NULL,
				   NULL))) {
		cmn_err(CE_PANIC, "%s: did not allocate dl_link_cachep", DL_LAPD_DRV_NAME);
		lapd_term_caches();
		return (-ENOMEM);
	} else
		printd(("%s: initialized dl link structure cache\n", DL_LAPD_DRV_NAME));
	return (0);
}

/*
 *  DL PRIV structure allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC INLINE struct dl *
__dl_lookup(ulong id)
{
	struct dl *dl;
	for (dl = master.dl.list; dl && dl->id != id; dl = dl->next) ;
	return (dl);
}
STATIC INLINE struct dl *
dl_lookup(ulong id)
{
	struct dl *dl;
	psw_t flags;
	lis_spin_lock_irqsave(&master.lock, &flags);
	dl = __dl_lookup(id);
	lis_spin_unlock_irqrestore(&master.lock, &flags);
	return (dl);
}
STATIC INLINE struct dl *
dl_get(struct dl *dl)
{
	assure(dl);
	if (dl)
		atomic_inc(&dl->refcnt);
	return (dl);
}
STATIC INLINE void
dl_put(struct dl *dl)
{
	assure(dl);
	if (dl && atomic_dec_and_test(&dl->refcnt)) {
		kmem_cache_free(dl_priv_cachep, dl);
		printd(("%s: %p: deallocated dl priv structure\n", DL_LAPD_DRV_NAME, dl));
	}
}
STATIC INLINE ulong
dl_get_id(ulong id)
{
	if (!id) {
		static ulong sequence = 0;
		psw_t flags;
		lis_spin_lock_irqsave(&master.lock, &flags);
		while (__dl_lookup(++sequence)) ;
		id = sequence;
		lis_spin_unlock_irqrestore(&master.lock, &flags);
	}
	return (id);
}
STATIC struct dl *
dl_alloc_priv(queue_t *q, struct dl **dlp, dev_t *devp, cred_t *crp, int style)
{
	struct dl *dl;
	ushort cmajor = getmajor(*devp);
	ushort cminor = getminor(*devp);
	printd(("%s: create dl dev = %d:%d\n", DL_LAPD_DRV_NAME, cmajor, cminor));
	if ((dl = kmem_cache_alloc(dl_priv_cachep, SLAB_ATOMIC))) {
		bzero(dl, sizeof(*dl));
		dl_get(dl);	/* first get */
		dl->u.dev.cmajor = cmajor;
		dl->u.dev.cminor = cminor;
		dl->cred = *crp;
		lis_spin_lock_init(&dl->qlock, "dl-queue-lock");
		(dl->oq = RD(q))->q_ptr = dl_get(dl);
		(dl->iq = WR(q))->q_ptr = dl_get(dl);
		dl->o_prim = &dl_r_prim;
		dl->i_prim = &dl_w_prim;
		dl->o_wakeup = NULL;
		dl->i_wakeup = NULL;
		dl->i_state = DL_UNATTACHED;
		dl->i_style = DL_STYLE2;
		dl->i_version = DL_CURRENT_VERSION;
		lis_spin_lock_init(&dl->lock, "dl-lock");
		/*
		   place in master list - open holds the master list lock 
		 */
		dl_mast_link(dl, dlp);
		bufq_init(&dl->conq);
		dl->id = dl_get_id(0);
		/*
		   fill out info ack defaults 
		 */
		dl->info.dl_primitive = DL_INFO_ACK;
		dl->info.dl_max_sdu = 256;
		dl->info.dl_min_sdu = 3;
		dl->info.dl_addr_length = 0;
		dl->info.dl_mac_type = DL_ISDN;
		dl->info.dl_reserved = 0;
		dl->info.dl_current_state = DL_UNATTACHED;
		dl->info.dl_sap_length = 0;
		dl->info.dl_service_mode = (DL_CLDLS | DL_CODLS);
		dl->info.dl_qos_length = 0;
		dl->info.dl_qos_offset = 0;
		dl->info.dl_qos_range_length = 0;
		dl->info.dl_qos_range_offset = 0;
		dl->info.dl_provider_style = DL_STYLE2;
		dl->info.dl_addr_offset = 0;
		dl->info.dl_version = DL_CURRENT_VERSION;
		dl->info.dl_brdcst_addr_length = 0;
		dl->info.dl_brdcst_addr_offset = 0;
		dl->info.dl_growth = 0;
		fixme(("allocate option defaults"));
	} else
		ptrace(("%s: ERROR: failed to allocated dl priv structure\n", DL_LAPD_DRV_NAME));
	return (dl);
}
STATIC void
dl_free_priv(struct dl *dl)
{
	psw_t flags;
	ensure(dl, return);
	printd(("%s: %p: free dl dev = %d:%d\n", DL_LAPD_DRV_NAME, dl, dl->u.dev.cmajor,
		dl->u.dev.cminor));
	lis_spin_lock_irqsave(&dl->lock, &flags);
	{
		ss7_unbufcall((struct str *) dl);
		flushq(dl->oq, FLUSHALL);
		flushq(dl->iq, FLUSHALL);
		bufq_purge(&dl->conq);
		/*
		   remove from wait list 
		 */
		if (dl->wait.cd)
			dl_wait_unlink(dl);
		/*
		   remove from teim list 
		 */
		if (dl->teim.cd)
			dl_teim_unlink(dl);
		/*
		   remove from bind hashes 
		 */
		if (dl->bind.cd)
			dl_bind_unlink(dl);
		/*
		   remove from conn hashes 
		 */
		if (dl->conn.cd)
			dl_conn_unlink(dl);
		/*
		   remove from list hashes 
		 */
		if (dl->list.cd)
			dl_list_unlink(dl);
		/*
		   remove form mgmt hashes 
		 */
		if (dl->mgmt.cd)
			dl_mgmt_unlink(dl);
		/*
		   remove from attachment list 
		 */
		if (dl->cd.cd)
			dl_atta_unlink(dl);
		/*
		   remove from master list 
		 */
		dl_mast_unlink(dl);
		/*
		   remove from queues 
		 */
		dl_put(xchg(&dl->oq->q_ptr, NULL));
		dl_put(xchg(&dl->iq->q_ptr, NULL));
		dl->oq = NULL;
		dl->iq = NULL;
	}
	lis_spin_unlock_irqrestore(&dl->lock, &flags);
	dl_put(dl);		/* final put */
	return;
}

/*
 *  DL LINK structure allocation and deallocation
 *  -------------------------------------------------------------------------
 */
STATIC INLINE struct cd *
__cd_lookup(ulong id)
{
	struct cd *cd;
	for (cd = master.cd.list; cd && cd->id != id; cd = cd->next) ;
	return (cd);
}
STATIC INLINE struct cd *
cd_lookup(ulong id)
{
	struct cd *cd = NULL;
	if (id) {
		psw_t flags;
		lis_spin_lock_irqsave(&master.lock, &flags);
		cd = __cd_lookup(id);
		lis_spin_unlock_irqrestore(&master.lock, &flags);
	}
	return (cd);
}
STATIC INLINE struct cd *
cd_get(struct cd *cd)
{
	assure(cd);
	if (cd)
		atomic_inc(&cd->refcnt);
	return (cd);
}
STATIC INLINE void
cd_put(struct cd *cd)
{
	assure(cd);
	if (cd && atomic_dec_and_test(&cd->refcnt)) {
		kmem_cache_free(dl_link_cachep, cd);
		printd(("%s: %p: deallocated dl link structure\n", DL_LAPD_DRV_NAME, cd));
	}
}
STATIC INLINE ulong
cd_get_id(ulong id)
{
	if (!id) {
		static ulong sequence = 0;
		psw_t flags;
		lis_spin_lock_irqsave(&master.lock, &flags);
		while (__cd_lookup(++sequence)) ;
		id = sequence;
		lis_spin_unlock_irqrestore(&master.lock, &flags);
	}
	return (id);
}
STATIC struct cd *
dl_alloc_link(queue_t *q, struct cd **cdp, ulong index, cred_t *crp)
{
	struct cd *cd;
	printd(("%s: create cd mux = %lu\n", DL_LAPD_DRV_NAME, index));
	if ((cd = kmem_cache_alloc(dl_link_cachep, SLAB_ATOMIC))) {
		bzero(cd, sizeof(*cd));
		cd_get(cd);	/* first get */
		cd->u.mux.index = index;
		cd->cred = *crp;
		lis_spin_lock_init(&cd->qlock, "cd-queue-lock");
		(cd->iq = RD(q))->q_ptr = cd_get(cd);
		(cd->oq = WR(q))->q_ptr = cd_get(cd);
		cd->o_prim = cd_w_prim;
		cd->i_prim = cd_r_prim;
		cd->o_wakeup = NULL;
		cd->i_wakeup = NULL;
		cd->i_state = CD_UNATTACHED;
		cd->i_style = CD_STYLE2;
		cd->i_version = 1;
		lis_spin_lock_init(&cd->lock, "cd-lock");
		/*
		   place in master list - link holds the master list lock 
		 */
		cd_mast_link(cd, cdp);
		/*
		   fill out info ack defaults 
		 */
		cd->info.cd_primitive = CD_INFO_ACK;
		cd->info.cd_state = CD_UNATTACHED;
		cd->info.cd_max_sdu = 256;
		cd->info.cd_min_sdu = 3;
		cd->info.cd_class = CD_HDLC;
		cd->info.cd_duplex = CD_FULLDUPLEX;
		cd->info.cd_output_style = CD_UNACKEDOUTPUT;
		cd->info.cd_features = (CD_CANREAD | CD_AUTOALLOW);
		cd->info.cd_addr_length = 0;
		cd->info.cd_ppa_style = CD_STYLE2;
		fixme(("allocate option defaults"));
	} else
		ptrace(("%s: ERROR: failed to allocate dl link structure\n", DL_LAPD_DRV_NAME));
	return (cd);
}
STATIC void
dl_free_link(struct cd *cd)
{
	psw_t flags;
	ensure(cd, return);
	printd(("%s: %p: free cd mux = %lu\n", DL_LAPD_DRV_NAME, cd, cd->u.mux.index));
	lis_spin_lock_irqsave(&cd->lock, &flags);
	{
		int i;
		struct dl *dl;
		ss7_unbufcall((struct str *) cd);
		flushq(cd->oq, FLUSHALL);
		flushq(cd->iq, FLUSHALL);
		/*
		   empty wait list 
		 */
		while ((dl = cd->wait.list))
			dl_wait_unlink(dl);
		/*
		   empty teim list 
		 */
		while ((dl = cd->teim.list))
			dl_teim_unlink(dl);
		/*
		   empty conn hash 
		 */
		for (i = 0; i < DL_CONN_SIZE; i++)
			while ((dl = cd->conn.hash[i]))
				dl_conn_unlink(dl);
		/*
		   empty list hash 
		 */
		for (i = 0; i < DL_LIST_SIZE; i++)
			while ((dl = cd->list.hash[i]))
				dl_list_unlink(dl);
		/*
		   empty bind hash 
		 */
		for (i = 0; i < DL_BIND_SIZE; i++)
			while ((dl = cd->bind.hash[i]))
				dl_bind_unlink(dl);
		/*
		   empty connection management list 
		 */
		if ((dl = cd->mgmt)) {
			dl->state = LAPD_TEI_UNASSIGNED;
			dl_set_state(dl, -1);
			dl_mgmt_unlink(dl);
		}
		/*
		   empty attach list 
		 */
		while ((dl = cd->dl.list))
			dl_atta_unlink(dl);
		/*
		   remove from master list 
		 */
		cd_mast_unlink(cd);
		/*
		   remove from queues 
		 */
		cd_put(xchg(&cd->oq->q_ptr, NULL));
		cd_put(xchg(&cd->iq->q_ptr, NULL));
		cd->oq = NULL;
		cd->iq = NULL;
	}
	lis_spin_unlock_irqrestore(&cd->lock, &flags);
	cd_put(cd);		/* final put */
	return;
}

/*
 *  =========================================================================
 *
 *  EVENTS
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  Timeouts
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  T200 Timeout
 *  -----------------------------------
 *  5.5.1.3 ... If timer T200 expires before the UA or DM response with the F
 *  bit set to 1 is received, the data link layer entity shall: restransmit
 *  the SABME command; restart timer T200; and increment the retransmission
 *  counter.  After retransmission of theh SABME command N200 times, the data
 *  link layer entity shall indicate this to layer 3 and the connection
 *  management entity by means of the DL-RELEASE indication and MDL-ERROR
 *  indication primitives, respectively, and enter the TEI-assigned state,
 *  after discarding all outstanding DL-DATA request primitives and all I
 *  frames in queue.  The valud of N200 is defined in 5.9.2.
 */
STATIC int
dl_t200_timeout(struct dl *dl)
{
	int err;
	switch (dl->state) {
	case LAPD_WAIT_ESTABLISH:
		switch (dl_get_state(dl)) {
		case DL_OUTCON_PENDING:
			ensure(dl->cd.cd, return (-EIO));
			if (dl->rc < dl->cd.cd->config.n200) {
				if ((err =
				     send_SABME_cmd(NULL, dl->cd.cd, dl->dlc.dl_sap, dl->dlc.dl_tei,
						    1)) < 0)
					return (err);
				dl_timer_start(dl, t200);
				dl->rc++;
				return (QR_DONE);
			}
			return dl_disconnect_ind(NULL, dl, DL_PROVIDER, DL_CONREJ_DEST_UNKNOWN,
						 NULL);
		default:
			swerr();
			break;
		}
		break;
	case LAPD_WAIT_RELEASE:
		switch (dl_get_state(dl)) {
		case DL_DISCON11_PENDING:
		case DL_DISCON12_PENDING:
		case DL_DISCON13_PENDING:
			ensure(dl->cd.cd, return (-EIO));
			if (dl->rc < dl->cd.cd->config.n200) {
				if ((err =
				     send_DISC_cmd(NULL, dl->cd.cd, dl->dlc.dl_sap, dl->dlc.dl_tei,
						   1)) < 0)
					return (err);
				dl_timer_start(dl, t200);
				dl->rc++;
				return (QR_DONE);
			}
			dl->state = LAPD_TEI_ASSIGNED;
			return (QR_DONE);	/* LAPD actually sends a DL-RELEASE confirm */
		default:
			swerr();
			break;
		}
		break;
	}
	fixme(("Write this function\n"));
	return (-EIO);
}

/*
 *  T201 Timeout
 *  -----------------------------------
 */
STATIC int
dl_t201_timeout(struct dl *dl)
{
	fixme(("Write this function\n"));
	return (-EIO);
}
STATIC int
cd_t201_timeout(struct cd *cd)
{
	fixme(("Write this function\n"));
	return (-EIO);
}

/*
 *  T202 Timeout
 *  -----------------------------------
 */
STATIC int
dl_t202_timeout(struct dl *dl)
{
	fixme(("Write this function\n"));
	return (-EIO);
}
STATIC int
cd_t202_timeout(struct cd *cd)
{
	fixme(("Write this function\n"));
	return (-EIO);
}

/*
 *  T203 Timeout
 *  -----------------------------------
 */
STATIC int
dl_t203_timeout(struct dl *dl)
{
	fixme(("Write this function\n"));
	return (-EIO);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitive recv from upstream
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  DL_DATA_REQ (M_DATA)
 *  -----------------------------------
 */
STATIC int
dl_data_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	size_t dlen = msgdsize(mp);
	if (dlen < dl->info.dl_min_sdu || dlen > dl->info.dl_max_sdu)
		goto eproto;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
		goto discard;
	case DL_DATAXFER:
	case DL_USER_RESET_PENDING:
	case DL_PROV_RESET_PENDING:
	case DL_RESET_RES_PENDING:
		bufq_queue(&dl->sndq, mp);
		return (QR_ABSORBED);
	default:
		goto eproto;
	}
      eproto:
	return m_error(q, dl, EPROTO, EPROTO);
      discard:
	return (QR_DONE);
}

/*
 *  DL_INFO_REQ                 0x00
 *  -----------------------------------
 */
STATIC int
dl_info_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_info_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	return dl_info_ack(q, dl);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_ATTACH_REQ               0x0b
 *  -----------------------------------
 *  This DL multiplexor is meant to work with the OpenSS7 CD multiplexor.  Each CD multiplexor
 *  (STYLE2) stream linked below the DL module has access to all PPAs on the system.  Each CD
 *  STYLE1 stream has a PPA assigned when the device is linked.  When an attach is requested,
 *  we first look for a matching STYLE1 or STYLE2 (with assigned CDI PPA) lower stream.  If one
 *  is found then we attached to the found stream.  LAPD DL PPAs are identifier values
 *  associated with a linked lower CD stream.
 */
STATIC int
dl_attach_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	struct cd *cd;
	dl_attach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	if (dl_get_state(dl) != DL_UNATTACHED)
		goto outstate;
	if (!(cd = cd_lookup(p->dl_ppa)) || cd->info.cd_class == CD_NODEV)
		goto badppa;
	if (dl->cd.cd)
		goto eio;
	dl_set_state(dl, DL_ATTACH_PENDING);
	if (cd_get_state(cd) != CD_UNATTACHED)
		return dl_ok_ack(q, dl, p->dl_primitive, cd, NULL, NULL);
	/*
	   when the first DL attaches we will attach the comm device if it is in the unattached
	   state (implicitly a style 2 comm device) 
	 */
	if (!cd->wait.list)
		/*
		   nobody attaching yet 
		 */
		return cd_attach_req(q, cd, dl);
	dl_wait_link(dl, cd);	/* add to wait list */
	return (QR_DONE);
      eio:
	return m_error(q, dl, EIO, EIO);
      badppa:
	return dl_error_ack(q, dl, p->dl_primitive, DL_BADPPA, ENXIO);
      outstate:
	return dl_error_ack(q, dl, p->dl_primitive, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_DETACH_REQ               0x0c
 *  -----------------------------------
 */
STATIC int
dl_detach_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	struct cd *cd;
	dl_detach_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	if (!(cd = dl->cd.cd))
		goto eio;
	dl_set_state(dl, DL_DETACH_PENDING);
	if (cd->dl.numb > 1 || cd->info.cd_ppa_style == CD_STYLE1)
		/*
		   we are not the last 
		 */
		return dl_ok_ack(q, dl, p->dl_primitive, cd, NULL, NULL);
	return cd_detach_req(q, cd, dl);
      eio:
	return m_error(q, dl, EIO, EIO);
      outstate:
	return dl_error_ack(q, dl, p->dl_primitive, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_BIND_REQ                 0x01
 *  -----------------------------------
 */
STATIC int
dl_bind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q), *d2;
	struct cd *cd = dl->cd.cd;
	int hash;
	dl_bind_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	if (dl_get_state(dl) != DL_UNBOUND)
		goto outstate;
	dl_set_state(dl, DL_BIND_PENDING);
	if (!cd || dl->bind.cd)
		goto eio;
	switch (cd_get_state(cd)) {
	case CD_UNUSABLE:
	case CD_UNATTACHED:
		goto initfailed;
	}
	switch (p->dl_service_mode) {
	case DL_CODLS:
	case DL_CLDLS:
		break;
	case DL_ACLDLS:
		goto unsupported;
	}
	if (p->dl_conn_mgmt) {
		if (cd->mgmt)
			goto bound;
		if (p->dl_service_mode != DL_CODLS)
			goto unsupported;
	} else {
		switch (dl->info.dl_mac_type) {
		case DL_ISDN:
			fixme(("check SAPI\n"));
			break;
			goto badaddr;
		case DL_X25:
			fixme(("check SAPI\n"));
			break;
			goto badaddr;
		case DL_FRAME:
			fixme(("check SAPI\n"));
			break;
			goto badaddr;
#ifdef DL_MTP2
		case DL_MTP2:
			fixme(("check SAPI\n"));
			break;
			goto badaddr;
#endif
#ifdef DL_GSMA
		case DL_GSMA:
			fixme(("check SAPI\n"));
			break;
			goto badaddr;
#endif
#ifdef DL_V5
		case DL_V5:
			fixme(("check SAPI\n"));
			break;
			goto badaddr;
#endif
#ifdef DL_GR303
		case DL_GR303:
			fixme(("check SAPI\n"));
			break;
			goto badaddr;
#endif
		default:
			goto enodev;
		}
		hash = ((dl->dlc.dl_sap + dl->dlc.dl_tei) & DL_BIND_HASHMASK);
		for (d2 = cd->bind.hash[hash]; d2; d2 = d2->bind.next)
			if (d2->dlc.dl_sap == p->dl_sap && d2->dlc.dl_tei == -1U)
				goto bound;
	}
	dl->dlc.dl_sap = p->dl_sap;
	dl->dlc.dl_tei = -1;
	if (p->dl_xidtest_flg & DL_AUTO_XID)
		dl->flags |= DLF_AUTO_XID;
	if (p->dl_xidtest_flg & DL_AUTO_TEST)
		dl->flags |= DLF_AUTO_TEST;
	dl->info.dl_service_mode = p->dl_service_mode;
	if (cd_get_state(cd) != CD_DISABLED) {
		dl->info.dl_min_sdu = cd->info.cd_min_sdu;
		dl->info.dl_max_sdu = cd->info.cd_max_sdu;
		return dl_bind_ack(q, dl, cd);
	}
	if (!cd->wait.list)
		/*
		   nobody binding yet 
		 */
		return cd_enable_req(q, cd, dl);
	dl_wait_link(dl, cd);	/* add to wait list */
	return (QR_DONE);
      badaddr:
	return dl_error_ack(q, dl, p->dl_primitive, DL_BADADDR, ENXIO);
      bound:
	return dl_error_ack(q, dl, p->dl_primitive, DL_BOUND, EADDRINUSE);
      unsupported:
	return dl_error_ack(q, dl, p->dl_primitive, DL_UNSUPPORTED, EINVAL);
      initfailed:
	return dl_error_ack(q, dl, p->dl_primitive, DL_INITFAILED, EIO);
      eio:
	return m_error(q, dl, EIO, EIO);
      enodev:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, ENODEV);
      outstate:
	return dl_error_ack(q, dl, p->dl_primitive, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_UNBIND_REQ               0x02
 *  -----------------------------------
 */
STATIC int
dl_unbind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	struct cd *cd;
	dl_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (!(cd = dl->cd.cd))
		goto eio;
	dl_set_state(dl, DL_UNBIND_PENDING);
	if (cd->bind.numb > 1)
		/*
		   we are not the last bound stream 
		 */
		return dl_ok_ack(q, dl, p->dl_primitive, cd, NULL, NULL);
	return cd_disable_req(q, cd, dl, CD_WAIT);
      eio:
	return m_error(q, dl, EIO, EIO);
      outstate:
	return dl_error_ack(q, dl, p->dl_primitive, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_SUBS_BIND_REQ            0x1b
 *  -----------------------------------
 */
STATIC int
dl_subs_bind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_subs_bind_req_t *p = (typeof(p)) mp->b_rptr;
	uchar tei;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (p->dl_subs_sap_length == 0)
		goto noaddr;
	if (p->dl_subs_sap_length < sizeof(dl->dlc.dl_tei))
		goto badaddr;
	if (mp->b_wptr > mp->b_rptr + p->dl_subs_sap_offset + p->dl_subs_sap_length)
		goto badaddr;
	switch (p->dl_subs_bind_class) {
	case DL_PEER_BIND:
		goto unsupported;
	case DL_HIERARCHICAL_BIND:
		break;
	default:
		goto unsupported;
	}
	if (dl->dlc.dl_tei != -1U)
		goto toomany;
	bcopy(mp->b_rptr + p->dl_subs_sap_offset, &tei, sizeof(tei));
	if (dl->conind) {
		struct dl *d2;
		int slot = ((dl->dlc.dl_sap + tei) & DL_BIND_HASHMASK);
		for (d2 = dl->cd.cd->list.hash[slot]; d2; d2 = d2->list.next)
			if (d2->dlc.dl_sap == dl->dlc.dl_sap && d2->dlc.dl_tei == tei)
				goto bound;
	}
	dl_set_state(dl, DL_SUBS_BIND_PND);
	return dl_subs_bind_ack(q, dl, tei);
#if 0
      access:
	return dl_error_ack(q, dl, p->dl_primitive, DL_ACCESS, EPERM);
#endif
      bound:
	return dl_error_ack(q, dl, p->dl_primitive, DL_BOUND, EINVAL);
      noaddr:
	return dl_error_ack(q, dl, p->dl_primitive, DL_NOADDR, EINVAL);
      badaddr:
	return dl_error_ack(q, dl, p->dl_primitive, DL_BADADDR, EINVAL);
      outstate:
	return dl_error_ack(q, dl, p->dl_primitive, DL_OUTSTATE, EPROTO);
      toomany:
	return dl_error_ack(q, dl, p->dl_primitive, DL_TOOMANY, ENXIO);
      unsupported:
	return dl_error_ack(q, dl, p->dl_primitive, DL_UNSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_SUBS_UNBIND_REQ          0x15
 *  -----------------------------------
 */
STATIC int
dl_subs_unbind_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_subs_unbind_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	dl_set_state(dl, DL_SUBS_UNBIND_PND);
	goto notsupported;
	return dl_ok_ack(q, dl, p->dl_primitive, dl->cd.cd, NULL, NULL);
      notsupported:
	return dl_error_ack(q, dl, p->dl_primitive, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_ENABMULTI_REQ            0x1d
 *  -----------------------------------
 */
STATIC int
dl_enabmulti_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_enabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
	return dl_ok_ack(q, dl, p->dl_primitive, dl->cd.cd, NULL, NULL);
      notsupported:
	return dl_error_ack(q, dl, p->dl_primitive, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_DISABMULTI_REQ           0x1e
 *  -----------------------------------
 */
STATIC int
dl_disabmulti_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_disabmulti_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
	return dl_ok_ack(q, dl, p->dl_primitive, dl->cd.cd, NULL, NULL);
      notsupported:
	return dl_error_ack(q, dl, p->dl_primitive, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_PROMISCON_REQ            0x1f
 *  -----------------------------------
 */
STATIC int
dl_promiscon_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_promiscon_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
	return dl_ok_ack(q, dl, p->dl_primitive, dl->cd.cd, NULL, NULL);
      notsupported:
	return dl_error_ack(q, dl, p->dl_primitive, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_PROMISCOFF_REQ           0x20
 *  -----------------------------------
 */
STATIC int
dl_promiscoff_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_promiscoff_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	goto notsupported;
	return dl_ok_ack(q, dl, p->dl_primitive, dl->cd.cd, NULL, NULL);
      notsupported:
	return dl_error_ack(q, dl, p->dl_primitive, DL_NOTSUPPORTED, EOPNOTSUPP);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_CONNECT_REQ              0x0d
 *  -----------------------------------
 *  If the DLSAP destination address is not provided, the TEI used in
 *  SUBS_BIND will be used.  If there was no SUBS_BIND, a TEI will be aquired
 *  from the peer and assigned.  If the TEI is provided and it is 127 (the
 *  broadcast address), the stream must be opened with root credentials.
 */
STATIC int
dl_connect_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_connect_req_t *p = (typeof(p)) mp->b_rptr;
	struct cd *cd = dl->cd.cd;
	uchar tei;
	int err;
	if (dl_get_state(dl) != DL_IDLE)
		goto outstate;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	if (p->dl_qos_length)
		goto badqostype;
	ensure(cd, goto eio);
	if (p->dl_dest_addr_length) {
		if (mp->b_wptr > mp->b_rptr + p->dl_dest_addr_length + p->dl_dest_addr_offset)
			goto badaddr;
		if (p->dl_dest_addr_length < sizeof(dl->dlc.dl_tei))
			goto badaddr;
		if (dl->dlc.dl_tei != -1U)
			goto badaddr;	/* a tei is already assigned */
		bcopy(mp->b_rptr + p->dl_dest_addr_offset, &tei, sizeof(tei));
		if (tei > 127)
			goto badaddr;
		if (tei == 127 && dl->cred.cr_uid != 0)
			goto access;
	} else if ((tei = dl->dlc.dl_tei) == -1U) {
		todo(("request assignment of a TEI\n"));
		goto badaddr;	/* for now */
	}
	switch (dl->state) {
	case LAPD_TEI_UNASSIGNED:
		dl->ref = (jiffies ^ (ulong) &q);
		dl->ref ^= dl->ref >> 16;
		dl->ref &= 0x0000ffff;
		/*
		   initiate TEI assignment 
		 */
		if ((err = send_TEI_i_req(q, cd, dl->ref, TEI_ANY)))
			return (err);
		dl_teim_link(dl, cd);
		dl_timer_start(dl, t202);
		dl->state = LAPD_WAIT_TEI_ESTABLISH;
		dl_set_state(dl, DL_OUTCON_PENDING);
		break;
	case LAPD_WAIT_TEI_ASSIGN:
		/*
		   wait for assignment to complete 
		 */
		dl->state = LAPD_WAIT_TEI_ESTABLISH;
		dl_set_state(dl, DL_OUTCON_PENDING);
		break;
	case LAPD_TEI_ASSIGNED:
		if ((err = send_SABME_cmd(q, cd, dl->dlc.dl_sap, tei, 1)))
			return (err);
		dl->rc = 0;
		dl_timer_start(dl, t200);
		dl_conn_link(dl, cd);
		dl->state = LAPD_WAIT_ESTABLISH;
		dl_set_state(dl, DL_OUTCON_PENDING);
		break;
	case LAPD_WAIT_REESTABLISH:
		/*
		   DISC, I QUEUE 
		 */
		dl->state = LAPD_WAIT_ESTABLISH;
		dl_set_state(dl, DL_OUTCON_PENDING);
		break;
	default:
		swerr();
		goto eio;
	}
	return (QR_DONE);
      access:
	return dl_error_ack(q, dl, p->dl_primitive, DL_ACCESS, EPERM);
      badqostype:
	return dl_error_ack(q, dl, p->dl_primitive, DL_BADQOSTYPE, EINVAL);
      outstate:
	return dl_error_ack(q, dl, p->dl_primitive, DL_OUTSTATE, EPROTO);
      badaddr:
	return dl_error_ack(q, dl, p->dl_primitive, DL_BADADDR, EINVAL);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
      eio:
	return m_error(q, dl, EIO, EIO);
}

/*
 *  DL_CONNECT_RES              0x0f
 *  -----------------------------------
 */
STATIC int
dl_connect_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_connect_res_t *p = (typeof(p)) mp->b_rptr;
	struct dl *ap = dl;
	mblk_t *cp;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	if (dl_get_state(dl) != DL_INCON_PENDING)
		goto outstate;
	if (p->dl_qos_length)
		goto badqostype;
	if (p->dl_resp_token && !(ap = dl_lookup(p->dl_resp_token)))
		goto badtoken;
	if (ap == dl) {
		if (dl->conq.q_msgs > 1)
			goto pending;
	} else {
		if ((1 << dl_get_state(ap)) & ~(DLF_UNATTACHED | DLF_UNBOUND | DLF_IDLE))
			goto badtoken;
		if (ap->conind)
			goto badtoken;
	}
	if (!p->dl_correlation)
		goto badcorr;
	for (cp = dl->conq.q_head; cp && (p->dl_correlation != (typeof(p->dl_correlation)) cp);
	     cp = cp->b_next) ;
	if (!cp)
		goto badcorr;
	dl_set_state(dl, DL_CONN_RES_PENDING);
	return dl_ok_ack(q, dl, p->dl_primitive, dl->cd.cd, ap, cp);
      badcorr:
	return dl_error_ack(q, dl, p->dl_primitive, DL_BADCORR, EINVAL);
      pending:
	return dl_error_ack(q, dl, p->dl_primitive, DL_PENDING, EINVAL);
      badtoken:
	return dl_error_ack(q, dl, p->dl_primitive, DL_BADTOKEN, EINVAL);
      badqostype:
	return dl_error_ack(q, dl, p->dl_primitive, DL_BADQOSTYPE, EINVAL);
      outstate:
	return dl_error_ack(q, dl, p->dl_primitive, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_TOKEN_REQ                0x11
 *  -----------------------------------
 */
STATIC int
dl_token_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_token_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_DISCONNECT_REQ           0x13
 *  -----------------------------------
 */
STATIC int
dl_disconnect_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	struct cd *cd = dl->cd.cd;
	dl_disconnect_req_t *p = (typeof(p)) mp->b_rptr;
	mblk_t *cp = NULL;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (dl_get_state(dl)) {
	case DL_OUTCON_PENDING:
		dl_set_state(dl, DL_DISCON8_PENDING);
		break;
	case DL_INCON_PENDING:
		if (!p->dl_correlation)
			goto badcorr;
		for (cp = dl->conq.q_head;
		     cp && (p->dl_correlation != (typeof(p->dl_correlation)) cp); cp = cp->b_next) ;
		if (!cp)
			goto badcorr;
		dl_set_state(dl, DL_DISCON9_PENDING);
		break;
	case DL_DATAXFER:
		dl_set_state(dl, DL_DISCON11_PENDING);
		break;
	case DL_USER_RESET_PENDING:
		dl_set_state(dl, DL_DISCON12_PENDING);
		break;
	case DL_PROV_RESET_PENDING:
		dl_set_state(dl, DL_DISCON13_PENDING);
		break;
	case DL_IDLE:
		if (dl->state == LAPD_TEI_ASSIGNED)
			break;
		/*
		   fall through 
		 */
	default:
		goto outstate;
	}
	return dl_ok_ack(q, dl, p->dl_primitive, cd, NULL, cp);
      badcorr:
	return dl_error_ack(q, dl, p->dl_primitive, DL_BADCORR, EINVAL);
      outstate:
	return dl_error_ack(q, dl, p->dl_primitive, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_RESET_REQ                0x17
 *  -----------------------------------
 */
STATIC int
dl_reset_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reset_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_RESET_RES                0x19
 *  -----------------------------------
 */
STATIC int
dl_reset_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reset_res_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	dl_set_state(dl, DL_RESET_RES_PENDING);
	return dl_ok_ack(q, dl, p->dl_primitive, dl->cd.cd, NULL, NULL);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_UNITDATA_REQ             0x07
 *  -----------------------------------
 */
STATIC int
dl_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_unitdata_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
	switch (dl_get_state(dl)) {
	case DL_IDLE:
		switch (dl->state) {
		case LAPD_TEI_UNASSIGNED:
			/*
			   initiate assignment procedures 
			 */
			dl->state = LAPD_WAIT_TEI_ASSIGN;
			return (-EAGAIN);
		case LAPD_WAIT_TEI_ASSIGN:
			/*
			   wait more for assignment 
			 */
			return (-EAGAIN);
		case LAPD_WAIT_TEI_ESTABLISH:
		case LAPD_TEI_ASSIGNED:
		case LAPD_WAIT_ESTABLISH:
		case LAPD_WAIT_REESTABLISH:
		case LAPD_PEND_RELEASE:
		case LAPD_WAIT_RELEASE:
		case LAPD_ESTABLISHED:
		case LAPD_TIME_RECOVERY:
		default:
			break;
		}
		return (QR_DONE);
	default:
		goto outstate;
	}
      outstate:
	return dl_error_ack(q, dl, p->dl_primitive, DL_OUTSTATE, EPROTO);
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_UDQOS_REQ                0x0a
 *  -----------------------------------
 */
STATIC int
dl_udqos_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_udqos_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_TEST_REQ                 0x2d
 *  -----------------------------------
 */
STATIC int
dl_test_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_test_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_TEST_RES                 0x2f
 *  -----------------------------------
 */
STATIC int
dl_test_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_test_res_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_XID_REQ                  0x29
 *  -----------------------------------
 */
STATIC int
dl_xid_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_xid_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_XID_RES                  0x2b
 *  -----------------------------------
 */
STATIC int
dl_xid_res(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_xid_res_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_DATA_ACK_REQ             0x21
 *  -----------------------------------
 */
STATIC int
dl_data_ack_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_data_ack_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_REPLY_REQ                0x24
 *  -----------------------------------
 */
STATIC int
dl_reply_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reply_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_REPLY_UPDATE_REQ         0x27
 *  -----------------------------------
 */
STATIC int
dl_reply_update_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_reply_update_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_PHYS_ADDR_REQ            0x31
 *  -----------------------------------
 */
STATIC int
dl_phys_addr_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_SET_PHYS_ADDR_REQ        0x33
 *  -----------------------------------
 */
STATIC int
dl_set_phys_addr_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_set_phys_addr_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  DL_GET_STATISTICS_REQ       0x34
 *  -----------------------------------
 */
STATIC int
dl_get_statistics_req(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_get_statistics_req_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto badprim;
      badprim:
	return dl_error_ack(q, dl, p->dl_primitive, DL_SYSERR, EMSGSIZE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Primitive recv from downstream
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  CD_DATA_IND (M_DATA)
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_data_ind(queue_t *q, mblk_t *mp)
{
	swerr();
	return (-EFAULT);
}

/*
 *  CD_INFO_ACK                 0x01 - Information acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_info_ack(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_info_ack_t *p = (typeof(p)) mp->b_rptr;
	struct dl *dl;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	cd->info = *p;		/* just adopt the state */
	/*
	   this is just a educated guess really, but here it goes 
	 */
	switch (cd_set_state(cd, p->cd_state)) {
	case CD_UNATTACHED:
	case CD_DISABLED:
		/*
		   initial info request on link 
		 */
		break;
	case CD_ENABLED:
	case CD_INPUT_ALLOWED:
	case CD_READ_ACTIVE:
	case CD_OUTPUT_ACTIVE:
		for (dl = cd->wait.list; dl; dl = dl->wait.next)
			if (dl_get_state(dl) == DL_BIND_PENDING)
				if ((err = dl_bind_ack(q, dl, cd)))
					return (err);
		return (QR_DONE);
	}
	return (QR_DONE);
      eio:
	swerr();
	return m_error_all(q, cd, EIO, EIO);
}

/*
 *  CD_OK_ACK                   0x06 - Success acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_ok_ack(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_ok_ack_t *p = (typeof(p)) mp->b_rptr;
	struct dl *dl;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	cd_set_state(cd, p->cd_state);
	switch (p->cd_correct_primitive) {
	case CD_ATTACH_REQ:
		for (dl = cd->wait.list; dl; dl = dl->wait.next)
			if (dl_get_state(dl) == DL_ATTACH_PENDING)
				if ((err = dl_ok_ack(q, dl, DL_ATTACH_REQ, cd, NULL, NULL)))
					return (err);
		return (QR_DONE);
	case CD_DETACH_REQ:
		for (dl = cd->wait.list; dl; dl = dl->wait.next)
			if (dl_get_state(dl) == DL_DETACH_PENDING)
				if ((err = dl_ok_ack(q, dl, DL_DETACH_REQ, cd, NULL, NULL)))
					return (err);
		return (QR_DONE);
	}
	return (QR_DONE);
      eio:
	swerr();
	return m_error_all(q, cd, EIO, EIO);
}

/*
 *  CD_ERROR_ACK                0x07 - Error acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_error_ack(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_error_ack_t *p = (typeof(p)) mp->b_rptr;
	struct dl *dl;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	switch (p->cd_error_primitive) {
	case CD_ATTACH_REQ:
		cd_set_state(cd, p->cd_state);
		for (dl = cd->wait.list; dl; dl = dl->wait.next)
			if (dl_get_state(dl) == DL_ATTACH_PENDING)
				if ((err = dl_error_ack(q, dl, DL_ATTACH_REQ, DL_SYSERR, EIO)))
					return (err);
		break;
	case CD_DETACH_REQ:
		cd_set_state(cd, p->cd_state);
		for (dl = cd->wait.list; dl; dl = dl->wait.next)
			if (dl_get_state(dl) == DL_DETACH_PENDING)
				if ((err = dl_error_ack(q, dl, DL_DETACH_REQ, DL_SYSERR, EIO)))
					return (err);
		break;
	case CD_ENABLE_REQ:
		cd_set_state(cd, p->cd_state);
		for (dl = cd->wait.list; dl; dl = dl->wait.next)
			if (dl_get_state(dl) == DL_BIND_PENDING)
				if ((err = dl_error_ack(q, dl, DL_BIND_REQ, DL_INITFAILED, EIO)))
					return (err);
		break;
	case CD_DISABLE_REQ:
		cd_set_state(cd, p->cd_state);
		for (dl = cd->wait.list; dl; dl = dl->wait.next)
			if (dl_get_state(dl) == DL_UNBIND_PENDING)
				if ((err = dl_error_ack(q, dl, DL_UNBIND_REQ, DL_SYSERR, EIO)))
					return (err);
		break;
	}
	return (QR_DONE);
      eio:
	swerr();
	return m_error_all(q, cd, EIO, EIO);
}

/*
 *  CD_ENABLE_CON               0x08 - Enable confirmation
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_enable_con(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_enable_con_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	cd_set_state(cd, p->cd_state);
	/*
	   turn around immediate info request 
	 */
	mp->b_datap->db_type = M_PCPROTO;
	mp->b_wptr = mp->b_rptr;
	*(ulong *) mp->b_wptr++ = CD_INFO_REQ;
	qreply(q, mp);
	return (QR_ABSORBED);
      eio:
	swerr();
	return m_error_all(q, cd, EIO, EIO);
}

/*
 *  CD_DISABLE_CON              0x09 - Disable confirmation
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_disable_con(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_disable_con_t *p = (typeof(p)) mp->b_rptr;
	struct dl *dl;
	int err;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	if (p->cd_state != CD_DISABLED)
		goto eio;
	if (cd_get_state(cd) != CD_DISABLE_PENDING)
		goto eio;
	cd_set_state(cd, p->cd_state);
	for (dl = cd->wait.list; dl; dl = dl->wait.next)
		if (dl_get_state(dl) == DL_UNBIND_PENDING)
			if ((err = dl_ok_ack(q, dl, DL_UNBIND_REQ, cd, NULL, NULL)))
				return (err);
	return (QR_DONE);
      eio:
	swerr();
	return m_error_all(q, cd, EIO, EIO);
}

/*
 *  CD_ERROR_IND                0x0a - Error indication
 *  -------------------------------------------------------------------------
 *  This is a particularly ugly primitive.  It can occur at any time and can
 *  change the state to any state.
 */
STATIC int
cd_error_ind(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_error_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	cd_set_state(cd, p->cd_state);
	if (cd->i_oldstate == p->cd_state)
		goto nochange;
	if (p->cd_state == CD_UNUSABLE) {
		assure(p->cd_errno == CD_FATALERR);
		goto eio;	/* fatal error */
	}
	/*
	   can't switch to pending, unattached or xray state 
	 */
	switch (p->cd_state) {
	case CD_ENABLE_PENDING:
	case CD_DISABLE_PENDING:
	case CD_UNATTACHED:
	case CD_XRAY:
		goto eio;
	}
	/*
	   these required error ack not error ind 
	 */
	switch (cd->i_oldstate) {
	case CD_UNATTACHED:
	case CD_DISABLED:
	case CD_DISABLE_PENDING:
	case CD_ENABLE_PENDING:
		goto eio;
	}
	switch (cd->i_oldstate) {
	case CD_UNUSABLE:
		if (p->cd_state != CD_DISABLED)
			goto eio;
		assure(p->cd_errno == CD_EVENT);
		/*
		   handle autonomous recovery 
		 */
		return m_error_all(q, cd, 0, 0);
	case CD_ENABLED:
	case CD_READ_ACTIVE:
	case CD_INPUT_ALLOWED:
	case CD_OUTPUT_ACTIVE:
		if (p->cd_state != CD_DISABLED)
			break;
		assure(p->cd_errno == CD_DISC);
		/*
		   handle autonomous disable 
		 */
		return m_hangup_all(q, cd);
	default:
		goto eio;
	}
      nochange:
	return (QR_DONE);
      eio:
	swerr();
	return m_error_all(q, cd, EIO, EIO);
}

/*
 *  CD_UNITDATA_ACK             0x0f - Data send acknowledgement
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_unitdata_ack(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_unitdata_ack_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	fixme(("input to state machine\n"));
      eio:
	swerr();
	return m_error_all(q, cd, EIO, EIO);
}

/*
 *  CD_UNITDATA_IND             0x10 - Data receive indication
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_unitdata_ind(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_unitdata_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	fixme(("input to state machine\n"));
      eio:
	swerr();
	return m_error_all(q, cd, EIO, EIO);
}

/*
 *  CD_BAD_FRAME_IND            0x14 - frame w/error (Gcom extension)
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_bad_frame_ind(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_bad_frame_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	fixme(("input to state machine\n"));
      eio:
	swerr();
	return m_error_all(q, cd, EIO, EIO);
}

/*
 *  CD_MODEM_SIG_IND            0x16 - Report modem signal state (Gcom)
 *  -------------------------------------------------------------------------
 */
STATIC int
cd_modem_sig_ind(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	cd_modem_sig_ind_t *p = (typeof(p)) mp->b_rptr;
	if (mp->b_wptr > mp->b_rptr + sizeof(*p))
		goto eio;
	fixme(("input to state machine\n"));
      eio:
	swerr();
	return m_error_all(q, cd, EIO, EIO);
}

/*
 *  =========================================================================
 *
 *  IO CONTROLS
 *
 *  =========================================================================
 */

/*
 *  LAPD_IOCGOPTIONS
 *  -----------------------------------
 */
STATIC INLINE int
lapd_iocgoptions(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_option_dl *opt = (typeof(opt)) (hdr + 1);
		if (!dl)
			return (-EINVAL);
		if ((size -= sizeof(*opt)) < 0)
			return (-EFAULT);
		*opt = dl->config;
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_option_cd *opt = (typeof(opt)) (hdr + 1);
		if (!cd)
			return (-EINVAL);
		if ((size -= sizeof(*opt)) < 0)
			return (-EFAULT);
		*opt = cd->config;
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCSOPTIONS
 *  -----------------------------------
 */
STATIC INLINE int
lapd_iocsoptions(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_option_dl *opt = (typeof(opt)) (hdr + 1);
		if (!dl)
			return (-EINVAL);
		if ((size -= sizeof(*opt)) < 0)
			return (-EFAULT);
		fixme(("check options first!\n"));
		dl->config = *opt;
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_option_cd *opt = (typeof(opt)) (hdr + 1);
		if (!cd)
			return (-EINVAL);
		if ((size -= sizeof(*opt)) < 0)
			return (-EFAULT);
		fixme(("check options first!\n"));
		cd->config = *opt;
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCXCONFIG Common configuration
 *  -----------------------------------
 */
STATIC INLINE int
lapd_iocxconfig(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size, int force, int test)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_config_dl *cnf = (typeof(cnf)) (hdr + 1);
		if ((size -= sizeof(*cnf)) < 0)
			return (-EFAULT);
		switch (hdr->cmd) {
		case LAPD_GET:
		{
			if (size < sizeof(*hdr))
				return (-EFAULT);
			if (hdr->id) {
				/*
				   get specific object 
				 */
				if (!dl)
					return (-ENXIO);
				hdr->type = LAPD_OBJ_TYPE_DL;
				hdr->id = dl->id;
				hdr->cmd = LAPD_GET;
				cnf->proto = dl->proto;
				cnf->dlc = dl->dlc;
				cnf->ppa = dl->cd.cd ? dl->cd.cd->id : 0;
				hdr = (typeof(hdr)) (cnf + 1);
			} else {
				/*
				   get list of all objects 
				 */
				for (dl = master.dl.list;
				     dl && size >= sizeof(*hdr) + sizeof(*cnf) + sizeof(*hdr);
				     dl = dl->next, size -= sizeof(*hdr) + sizeof(*cnf), hdr =
				     (typeof(hdr)) (cnf + 1), cnf = (typeof(cnf)) (hdr + 1)) {
					hdr->type = LAPD_OBJ_TYPE_DL;
					hdr->id = dl->id;
					cnf->proto = dl->proto;
					cnf->dlc = dl->dlc;
					cnf->ppa = dl->cd.cd ? dl->cd.cd->id : 0;
				}
			}
			/*
			   terminate list with zero object type 
			 */
			hdr->type = 0;
			hdr->id = 0;
			hdr->cmd = 0;
			return (QR_DONE);
		}
		case LAPD_ADD:
		{
			if (dl)
				return (-EEXIST);
			/*
			   DL objects normally add with open(2), but we might support permanent
			   data links 
			 */
			return (-EOPNOTSUPP);
		}
		case LAPD_CHA:
		{
			if (!dl)
				return (-ENXIO);
			/*
			   DL objects normally cha with bind, but we might support permanent data
			   links 
			 */
			return (-EOPNOTSUPP);
		}
		case LAPD_DEL:
		{
			if (!dl)
				return (-ENXIO);
			/*
			   DL objects normally del with close(2), but we might support permanent
			   data links 
			 */
			return (-EOPNOTSUPP);
		}
		}
		break;
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_config_cd *cnf = (typeof(cnf)) (hdr + 1);
		if ((size -= sizeof(*cnf)) < 0)
			return (-EFAULT);
		switch (hdr->cmd) {
		case LAPD_GET:
		{
			if (size < sizeof(*hdr))
				return (-EFAULT);
			if (hdr->id) {
				struct dl *dl;
				struct lapd_config_dl *cdl;
				/*
				   get specific object 
				 */
				if (!cd)
					return (-ENXIO);
				hdr->type = LAPD_OBJ_TYPE_CD;
				hdr->id = cd->id;
				hdr->cmd = LAPD_GET;
				cnf->proto = cd->proto;
				cnf->ppa = cd->ppa;
				hdr = (typeof(hdr)) (cnf + 1);
				cdl = (typeof(cdl)) (hdr + 1);
				/*
				   list all attached data links 
				 */
				for (dl = cd->dl.list;
				     dl && size >= sizeof(*hdr) + sizeof(*cdl) + sizeof(*hdr);
				     dl = dl->next, size -= sizeof(*hdr) + sizeof(*cdl), hdr =
				     (typeof(hdr)) (cdl + 1), cdl = (typeof(cdl)) (hdr + 1)) {
					hdr->type = LAPD_OBJ_TYPE_DL;
					hdr->id = dl->id;
					hdr->cmd = LAPD_GET;
					cdl->proto = dl->proto;
					cdl->dlc = dl->dlc;
					cdl->ppa = dl->cd.cd ? dl->cd.cd->id : 0;
				}
			} else {
				/*
				   get list of all objects 
				 */
				for (cd = master.cd.list;
				     cd && size >= sizeof(*hdr) + sizeof(*cnf) + sizeof(*hdr);
				     cd = cd->next, size -= sizeof(*hdr) + sizeof(*cnf), hdr =
				     (typeof(hdr)) (cnf + 1), cnf = (typeof(cnf)) (hdr + 1)) {
					hdr->type = LAPD_OBJ_TYPE_CD;
					hdr->id = cd->id;
					cnf->proto = cd->proto;
					cnf->ppa = cd->ppa;
				}
			}
			/*
			   terminate list with zero object type 
			 */
			hdr->type = 0;
			hdr->id = 0;
			hdr->cmd = 0;
			return (QR_DONE);
		}
		case LAPD_ADD:
		{
			if (cd)
				return (-EEXIST);
			for (cd = master.cd.list; cd; cd = cd->next) {
				if (cd->u.mux.index == cnf->muxid) {
					if (cd->id)
						return (-EEXIST);
					break;
				}
			}
			if (!force) {
				/*
				   nothing to skip 
				 */
			}
			if (!test) {
				hdr->id = cd->id = cd_get_id(hdr->id);
				cd->proto = cnf->proto;
				cd->ppa = cnf->ppa;
				fixme(("Actually add the object.\n"));
			}
			return (QR_DONE);
		}
		case LAPD_CHA:
		{
			if (!cd)
				return (-ENXIO);
			if (!force) {
				/*
				   we have attached data links 
				 */
				if (cd->dl.list)
					return (-EBUSY);
			}
			if (!test) {
				cd->proto = cnf->proto;
				cd->ppa = cnf->ppa;
				fixme(("Actually cha the object.\n"));
			}
			return (QR_DONE);
		}
		case LAPD_DEL:
		{
			if (!cd)
				return (-ENXIO);
			if (!force) {
				fixme(("Check if CD is active first.\n"));
			}
			if (!test) {
				fixme(("Actually del the object.\n"));
			}
			return (QR_DONE);
		}
		}
		break;
	}
	case LAPD_OBJ_TYPE_DF:
	{
		struct df *df = df_lookup(hdr->id);
		struct lapd_config_df *cnf = (typeof(cnf)) (hdr + 1);
		if ((size -= sizeof(*cnf)) < 0)
			return (-EFAULT);
		switch (hdr->cmd) {
		case LAPD_GET:
		{
			if (size < sizeof(*hdr))
				return (-EFAULT);
			if (!df)
				return (-ENXIO);
			fixme(("Get configuration\n"));
			return (QR_DONE);
		}
		case LAPD_ADD:
		{
			if (df)
				return (-EEXIST);
			/*
			   The default object always exists. 
			 */
			return (-EEXIST);
		}
		case LAPD_CHA:
		{
			if (!df)
				return (-ENXIO);
			if (!force) {
				/*
				   nothing to skip 
				 */
			}
			if (!test) {
				df->proto = cnf->proto;
				fixme(("Actually cha the object.\n"));
			}
			return (QR_DONE);
		}
		case LAPD_DEL:
		{
			if (!df)
				return (-ENXIO);
			/*
			   Can't delete the default object. 
			 */
			return (-EOPNOTSUPP);
		}
		}
		break;
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCGCONFIG
 *  -----------------------------------
 */
STATIC INLINE int
lapd_iocgconfig(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->cmd) {
	case LAPD_GET:
		return lapd_iocxconfig(q, dl, hdr, size, 0, 0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCSCONFIG
 *  -----------------------------------
 */
STATIC INLINE int
lapd_iocsconfig(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->cmd) {
	case LAPD_ADD:
	case LAPD_CHA:
	case LAPD_DEL:
		return lapd_iocxconfig(q, dl, hdr, size, 0, 0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCTCONFIG
 *  -----------------------------------
 */
STATIC INLINE int
lapd_ioctconfig(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->cmd) {
	case LAPD_ADD:
	case LAPD_CHA:
	case LAPD_DEL:
		return lapd_iocxconfig(q, dl, hdr, size, 1, 0);
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCCCONFIG
 *  -----------------------------------
 */
STATIC INLINE int
lapd_ioccconfig(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->cmd) {
	case LAPD_ADD:
	case LAPD_CHA:
	case LAPD_DEL:
		return lapd_iocxconfig(q, dl, hdr, size, 0, 1);
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCGSTATEM
 *  -----------------------------------
 */
STATIC INLINE int
lapd_iocgstatem(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_statem_dl *sta = (typeof(sta)) (hdr + 1);
		if (!dl)
			return (-EINVAL);
		if ((size -= sizeof(*sta)) < 0)
			return (-EFAULT);
		*sta = dl->statem;
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_statem_cd *sta = (typeof(sta)) (hdr + 1);
		if (!cd)
			return (-EINVAL);
		if ((size -= sizeof(*sta)) < 0)
			return (-EFAULT);
		*sta = cd->statem;
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCCMRESET
 *  -----------------------------------
 */
STATIC INLINE int
lapd_ioccmreset(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_statem_dl *sta = (typeof(sta)) (hdr + 1);
		if (!dl)
			return (-EINVAL);
		if ((size -= sizeof(*sta)) < 0)
			return (-EFAULT);
		*sta = dl->statem;
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_statem_cd *sta = (typeof(sta)) (hdr + 1);
		if (!cd)
			return (-EINVAL);
		if ((size -= sizeof(*sta)) < 0)
			return (-EFAULT);
		*sta = cd->statem;
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCGSTATSP
 *  -----------------------------------
 */
STATIC INLINE int
lapd_iocgstatsp(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_stats_dl *per = (typeof(per)) (hdr + 1);
		if (!dl)
			return (-EINVAL);
		if ((size -= sizeof(*per)) < 0)
			return (-EFAULT);
		*per = dl->statsp;
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_stats_cd *per = (typeof(per)) (hdr + 1);
		if (!cd)
			return (-EINVAL);
		if ((size -= sizeof(*per)) < 0)
			return (-EFAULT);
		*per = cd->statsp;
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCSSTATSP
 *  -----------------------------------
 */
STATIC INLINE int
lapd_iocsstatsp(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_stats_dl *per = (typeof(per)) (hdr + 1);
		if (!dl)
			return (-EINVAL);
		if ((size -= sizeof(*per)) < 0)
			return (-EFAULT);
		*per = dl->statsp;
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_stats_cd *per = (typeof(per)) (hdr + 1);
		if (!cd)
			return (-EINVAL);
		if ((size -= sizeof(*per)) < 0)
			return (-EFAULT);
		*per = cd->statsp;
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCGSTATS
 *  -----------------------------------
 */
STATIC INLINE int
lapd_iocgstats(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_stats_dl *sta = (typeof(sta)) (hdr + 1);
		if (!dl)
			return (-EINVAL);
		if ((size -= sizeof(*sta)) < 0)
			return (-EFAULT);
		*sta = dl->stats;
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_stats_cd *sta = (typeof(sta)) (hdr + 1);
		if (!cd)
			return (-EINVAL);
		if ((size -= sizeof(*sta)) < 0)
			return (-EFAULT);
		*sta = cd->stats;
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCCSTATS
 *  -----------------------------------
 */
STATIC INLINE int
lapd_ioccstats(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_stats_dl *sta = (typeof(sta)) (hdr + 1);
		if (!dl)
			return (-EINVAL);
		if ((size -= sizeof(*sta)) < 0)
			return (-EFAULT);
		*sta = dl->stats;
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_stats_cd *sta = (typeof(sta)) (hdr + 1);
		if (!cd)
			return (-EINVAL);
		if ((size -= sizeof(*sta)) < 0)
			return (-EFAULT);
		*sta = cd->stats;
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCGNOTIFY
 *  -----------------------------------
 */
STATIC INLINE int
lapd_iocgnotify(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_notify_dl *not = (typeof(not)) (hdr + 1);
		if (!dl)
			return (-EINVAL);
		if ((size -= sizeof(*not)) < 0)
			return (-EFAULT);
		*not = dl->notify;
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_notify_cd *not = (typeof(not)) (hdr + 1);
		if (!cd)
			return (-EINVAL);
		if ((size -= sizeof(*not)) < 0)
			return (-EFAULT);
		*not = cd->notify;
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCSNOTIFY
 *  -----------------------------------
 */
STATIC INLINE int
lapd_iocsnotify(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_notify_dl *not = (typeof(not)) (hdr + 1);
		if (!dl)
			return (-EINVAL);
		if ((size -= sizeof(*not)) < 0)
			return (-EFAULT);
		*not = dl->notify;
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_notify_cd *not = (typeof(not)) (hdr + 1);
		if (!cd)
			return (-EINVAL);
		if ((size -= sizeof(*not)) < 0)
			return (-EFAULT);
		*not = cd->notify;
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCCNOTIFY
 *  -----------------------------------
 */
STATIC INLINE int
lapd_ioccnotify(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		struct lapd_notify_dl *not = (typeof(not)) (hdr + 1);
		if (!dl)
			return (-EINVAL);
		if ((size -= sizeof(*not)) < 0)
			return (-EFAULT);
		*not = dl->notify;
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		struct lapd_notify_cd *not = (typeof(not)) (hdr + 1);
		if (!cd)
			return (-EINVAL);
		if ((size -= sizeof(*not)) < 0)
			return (-EFAULT);
		*not = cd->notify;
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCCMGMT
 *  -----------------------------------
 */
STATIC INLINE int
lapd_ioccmgmt(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		if (!dl)
			return (-EINVAL);
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		if (!cd)
			return (-EINVAL);
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  LAPD_IOCCPASS
 *  -----------------------------------
 */
STATIC INLINE int
lapd_ioccpass(queue_t *q, struct dl *dl, struct lapd_iochdr *hdr, int size)
{
	switch (hdr->type) {
	case LAPD_OBJ_TYPE_DL:
	{
		struct dl *dl = dl_lookup(hdr->id);
		if (!dl)
			return (-EINVAL);
		return (QR_DONE);
	}
	case LAPD_OBJ_TYPE_CD:
	{
		struct cd *cd = cd_lookup(hdr->id);
		if (!cd)
			return (-EINVAL);
		return (QR_DONE);
	}
	}
	rare();
	return (-EINVAL);
}

/*
 *  =========================================================================
 *
 *  STREAMS MESSAGE HANDLING
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  M_IOCTL, M_IOCDATA, M_IOCACK, M_IOCNAK, M_COPYIN, M_COPYOUT  Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
dl_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	struct iocblk *iocp = (struct iocblk *) mp->b_rptr;
	void *arg = mp->b_cont ? mp->b_cont->b_rptr : NULL;
	int cmd = iocp->ioc_cmd, count = iocp->ioc_count;
	int type = _IOC_TYPE(cmd), nr = _IOC_NR(cmd), size = _IOC_SIZE(cmd);
	int ret = 0;
	psw_t flags;
	(void) dl;
	switch (type) {
	case _IOC_TYPE(__SID):
	{
		struct linkblk *lb;
		struct cd *cd, **cdp;
		if (!(lb = arg)) {
			swerr();
			ret = (-EINVAL);
			break;
		}
		switch (nr) {
		case _IOC_NR(I_PLINK):
			ptrace(("%s: %p: -> I_PLINK\n", DL_LAPD_DRV_NAME, dl));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PLINK\n",
					DL_LAPD_DRV_NAME, dl));
				ret = (-EPERM);
				break;
			}
		case _IOC_NR(I_LINK):
			ptrace(("%s: %p: -> I_LINK\n", DL_LAPD_DRV_NAME, dl));
			MOD_INC_USE_COUNT;	/* keep module from unloading */
			lis_spin_lock_irqsave(&master.lock, &flags);
			{
				/*
				   place in list in ascending index order 
				 */
				for (cdp = &master.cd.list;
				     *cdp && (*cdp)->u.mux.index < lb->l_index;
				     cdp = &(*cdp)->next) ;
				if ((cd =
				     dl_alloc_link(lb->l_qbot, cdp, lb->l_index, iocp->ioc_cr))) {
					lis_spin_unlock_irqrestore(&master.lock, &flags);
					break;
				}
				MOD_DEC_USE_COUNT;
				ret = (-ENOMEM);
			}
			lis_spin_unlock_irqrestore(&master.lock, &flags);
			break;
		case _IOC_NR(I_PUNLINK):
			ptrace(("%s: %p: -> I_PUNLINK\n", DL_LAPD_DRV_NAME, dl));
			if (iocp->ioc_cr->cr_uid != 0) {
				ptrace(("%s: %p: ERROR: Non-root attempt to I_PUNLINK\n",
					DL_LAPD_DRV_NAME, dl));
				ret = (-EPERM);
				break;
			}
		case _IOC_NR(I_UNLINK):
			ptrace(("%s: %p: -> I_UNLINK\n", DL_LAPD_DRV_NAME, dl));
			lis_spin_lock_irqsave(&master.lock, &flags);
			{
				for (cd = master.cd.list; cd; cd = cd->next)
					if (cd->u.mux.index == lb->l_index)
						break;
				if (!cd) {
					ret = (-EINVAL);
					lis_spin_unlock_irqrestore(&master.lock, &flags);
					break;
				}
				dl_free_link(cd);
				MOD_DEC_USE_COUNT;
			}
			lis_spin_unlock_irqrestore(&master.lock, &flags);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported STREAMS ioctl %c, %d\n",
				DL_LAPD_DRV_NAME, dl, (char) type, nr));
			ret = (-EOPNOTSUPP);
			break;
		}
		break;
	}
	case LAPD_IOC_MAGIC:
	{
		struct lapd_iochdr *hdr;
		if (!(hdr = arg) || count < size || (count -= sizeof(*hdr)) < 0) {
			ret = (-EFAULT);
			break;
		}
		switch (nr) {
		case _IOC_NR(LAPD_IOCGOPTIONS):
			printd(("%s: %p: -> LAPD_IOCGOPTIONS\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_iocgoptions(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCSOPTIONS):
			printd(("%s: %p: -> LAPD_IOCSOPTIONS\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_iocsoptions(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCGCONFIG):
			printd(("%s: %p: -> LAPD_IOCGCONFIG\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_iocgconfig(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCSCONFIG):
			printd(("%s: %p: -> LAPD_IOCSCONFIG\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_iocsconfig(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCTCONFIG):
			printd(("%s: %p: -> LAPD_IOCTCONFIG\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_ioctconfig(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCCCONFIG):
			printd(("%s: %p: -> LAPD_IOCCCONFIG\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_ioccconfig(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCGSTATEM):
			printd(("%s: %p: -> LAPD_IOCGSTATEM\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_iocgstatem(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCCMRESET):
			printd(("%s: %p: -> LAPD_IOCCMRESET\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_ioccmreset(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCGSTATSP):
			printd(("%s: %p: -> LAPD_IOCGSTATSP\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_iocgstatsp(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCSSTATSP):
			printd(("%s: %p: -> LAPD_IOCSSTATSP\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_iocsstatsp(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCGSTATS):
			printd(("%s: %p: -> LAPD_IOCGSTATS\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_iocgstats(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCCSTATS):
			printd(("%s: %p: -> LAPD_IOCCSTATS\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_ioccstats(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCGNOTIFY):
			printd(("%s: %p: -> LAPD_IOCGNOTIFY\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_iocgnotify(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCSNOTIFY):
			printd(("%s: %p: -> LAPD_IOCSNOTIFY\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_iocsnotify(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCCNOTIFY):
			printd(("%s: %p: -> LAPD_IOCCNOTIFY\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_ioccnotify(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCCMGMT):
			printd(("%s: %p: -> LAPD_IOCCMGMT\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_ioccmgmt(q, dl, hdr, count);
			break;
		case _IOC_NR(LAPD_IOCCPASS):
			printd(("%s: %p: -> LAPD_IOCCPASS\n", DL_LAPD_DRV_NAME, dl));
			ret = lapd_ioccpass(q, dl, hdr, count);
			break;
		default:
			ptrace(("%s: %p: ERROR: Unsupported DL ioctl %c, %d\n", DL_LAPD_DRV_NAME,
				dl, (char) type, nr));
			ret = (-EOPNOTSUPP);
			break;
		}
		break;
	}
	case HDLC_IOC_MAGIC:
	{
		struct cd *cd;
		if (!(cd = dl->cd.cd)) {
			/*
			   not attached to ppa 
			 */
			ret = (-ENXIO);
			break;
		}
		if (!canputnext(cd->oq)) {
			ret = (-EAGAIN);
			break;
		}
		putnext(cd->oq, mp);
		return (QR_ABSORBED);
	}
	default:
		ptrace(("%s: %p: ERROR: Unsuported DL ioctl %c, %d\n", DL_LAPD_DRV_NAME, dl,
			(char) type, nr));
		ret = (-EOPNOTSUPP);
		break;
	}
	if (ret == 0) {
		mp->b_datap->db_type = M_IOCACK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = 0;
	} else if (ret < 0) {
		mp->b_datap->db_type = M_IOCNAK;
		iocp->ioc_error = -ret;
		iocp->ioc_rval = -1U;
	} else
		return (ret);
	qreply(q, mp);
	return (QR_ABSORBED);
}
STATIC INLINE int
dl_w_iocdata(queue_t *q, mblk_t *mp)
{
	fixme(("write this function\n"));
	return (-EFAULT);
}
STATIC INLINE int
cd_r_iocack(queue_t *q, mblk_t *mp)
{
	fixme(("write this function\n"));
	return (-EFAULT);
}
STATIC INLINE int
cd_r_iocnak(queue_t *q, mblk_t *mp)
{
	fixme(("write this function\n"));
	return (-EFAULT);
}
STATIC INLINE int
cd_r_copyin(queue_t *q, mblk_t *mp)
{
	fixme(("write this function\n"));
	return (-EFAULT);
}
STATIC INLINE int
cd_r_copyout(queue_t *q, mblk_t *mp)
{
	fixme(("write this function\n"));
	return (-EFAULT);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_PROTO, M_PCPROTO Message Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_w_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	ulong prim;
	int rtn;
	dl->i_oldstate = dl_get_state(dl);
	if (mp->b_wptr > mp->b_rptr + sizeof(prim))
		return (-EMSGSIZE);
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case DL_INFO_REQ:
		printd(("%s: %p: -> DL_INFO_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_info_req(q, mp);
		break;
	case DL_ATTACH_REQ:
		printd(("%s: %p: -> DL_ATTACH_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_attach_req(q, mp);
		break;
	case DL_DETACH_REQ:
		printd(("%s: %p: -> DL_DETACH_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_detach_req(q, mp);
		break;
	case DL_BIND_REQ:
		printd(("%s: %p: -> DL_BIND_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_bind_req(q, mp);
		break;
	case DL_UNBIND_REQ:
		printd(("%s: %p: -> DL_UNBIND_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_unbind_req(q, mp);
		break;
	case DL_SUBS_BIND_REQ:
		printd(("%s: %p: -> DL_SUBS_BIND_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_subs_bind_req(q, mp);
		break;
	case DL_SUBS_UNBIND_REQ:
		printd(("%s: %p: -> DL_SUBS_UNBIND_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_subs_unbind_req(q, mp);
		break;
	case DL_ENABMULTI_REQ:
		printd(("%s: %p: -> DL_ENABMULTI_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_enabmulti_req(q, mp);
		break;
	case DL_DISABMULTI_REQ:
		printd(("%s: %p: -> DL_DISABMULTI_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_disabmulti_req(q, mp);
		break;
	case DL_PROMISCON_REQ:
		printd(("%s: %p: -> DL_PROMISCON_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_promiscon_req(q, mp);
		break;
	case DL_PROMISCOFF_REQ:
		printd(("%s: %p: -> DL_PROMISCOFF_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_promiscoff_req(q, mp);
		break;
	case DL_CONNECT_REQ:
		printd(("%s: %p: -> DL_CONNECT_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_connect_req(q, mp);
		break;
	case DL_CONNECT_RES:
		printd(("%s: %p: -> DL_CONNECT_RES\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_connect_res(q, mp);
		break;
	case DL_TOKEN_REQ:
		printd(("%s: %p: -> DL_TOKEN_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_token_req(q, mp);
		break;
	case DL_DISCONNECT_REQ:
		printd(("%s: %p: -> DL_DISCONNECT_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_disconnect_req(q, mp);
		break;
	case DL_RESET_REQ:
		printd(("%s: %p: -> DL_RESET_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_reset_req(q, mp);
		break;
	case DL_RESET_RES:
		printd(("%s: %p: -> DL_RESET_RES\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_reset_res(q, mp);
		break;
	case DL_UNITDATA_REQ:
		printd(("%s: %p: -> DL_UNITDATA_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_unitdata_req(q, mp);
		break;
	case DL_UDQOS_REQ:
		printd(("%s: %p: -> DL_UDQOS_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_udqos_req(q, mp);
		break;
	case DL_TEST_REQ:
		printd(("%s: %p: -> DL_TEST_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_test_req(q, mp);
		break;
	case DL_TEST_RES:
		printd(("%s: %p: -> DL_TEST_RES\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_test_res(q, mp);
		break;
	case DL_XID_REQ:
		printd(("%s: %p: -> DL_XID_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_xid_req(q, mp);
		break;
	case DL_XID_RES:
		printd(("%s: %p: -> DL_XID_RES\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_xid_res(q, mp);
		break;
	case DL_DATA_ACK_REQ:
		printd(("%s: %p: -> DL_DATA_ACK_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_data_ack_req(q, mp);
		break;
	case DL_REPLY_REQ:
		printd(("%s: %p: -> DL_REPLY_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_reply_req(q, mp);
		break;
	case DL_REPLY_UPDATE_REQ:
		printd(("%s: %p: -> DL_REPLY_UPDATE_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_reply_update_req(q, mp);
		break;
	case DL_PHYS_ADDR_REQ:
		printd(("%s: %p: -> DL_PHYS_ADD_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_phys_addr_req(q, mp);
		break;
	case DL_SET_PHYS_ADDR_REQ:
		printd(("%s: %p: -> DL_SET_PHYS_ADDR_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_set_phys_addr_req(q, mp);
		break;
	case DL_GET_STATISTICS_REQ:
		printd(("%s: %p: -> DL_GET_STATISTICS_REQ\n", DL_LAPD_DRV_NAME, dl));
		rtn = dl_get_statistics_req(q, mp);
		break;
	default:
		printd(("%s: %p: -> DL_?? %lu ??\n", DL_LAPD_DRV_NAME, dl, prim));
		rtn = dl_error_ack(q, dl, prim, DL_BADPRIM, EOPNOTSUPP);
		break;
	}
	if (rtn < 0)
		dl_set_state(dl, dl->i_oldstate);
	return (rtn);
}
STATIC int
cd_r_proto(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	ulong prim;
	int rtn;
	cd->i_oldstate = cd_get_state(cd);
	if (mp->b_wptr > mp->b_rptr + sizeof(prim))
		return (-EMSGSIZE);
	switch ((prim = *(ulong *) mp->b_rptr)) {
	case CD_INFO_ACK:
		printd(("%s: %p: CD_INFO_ACK <-\n", DL_LAPD_DRV_NAME, cd));
		rtn = cd_info_ack(q, mp);
		break;
	case CD_OK_ACK:
		printd(("%s: %p: CD_OK_ACK <-\n", DL_LAPD_DRV_NAME, cd));
		rtn = cd_ok_ack(q, mp);
		break;
	case CD_ERROR_ACK:
		printd(("%s: %p: CD_ERROR_ACK <-\n", DL_LAPD_DRV_NAME, cd));
		rtn = cd_error_ack(q, mp);
		break;
	case CD_ENABLE_CON:
		printd(("%s: %p: CD_ENABLE_CON <-\n", DL_LAPD_DRV_NAME, cd));
		rtn = cd_enable_con(q, mp);
		break;
	case CD_DISABLE_CON:
		printd(("%s: %p: CD_DISABLE_CON <-\n", DL_LAPD_DRV_NAME, cd));
		rtn = cd_disable_con(q, mp);
		break;
	case CD_ERROR_IND:
		printd(("%s: %p: CD_ERROR_IND <-\n", DL_LAPD_DRV_NAME, cd));
		rtn = cd_error_ind(q, mp);
		break;
	case CD_UNITDATA_ACK:
		printd(("%s: %p: CD_UNITDATA_ACK <-\n", DL_LAPD_DRV_NAME, cd));
		rtn = cd_unitdata_ack(q, mp);
		break;
	case CD_UNITDATA_IND:
		printd(("%s: %p: CD_UNITDATA_IND <-\n", DL_LAPD_DRV_NAME, cd));
		rtn = cd_unitdata_ind(q, mp);
		break;
	case CD_BAD_FRAME_IND:
		printd(("%s: %p: CD_BAD_FRAME_IND <-\n", DL_LAPD_DRV_NAME, cd));
		rtn = cd_bad_frame_ind(q, mp);
		break;
	case CD_MODEM_SIG_IND:
		printd(("%s: %p: CD_MODEM_SIG_IND <-\n", DL_LAPD_DRV_NAME, cd));
		rtn = cd_modem_sig_ind(q, mp);
		break;
	default:
		pswerr(("%s: %p: CD_?? %lu ?? <-\n", DL_LAPD_DRV_NAME, cd, prim));
		rtn = m_hangup_all(q, cd);
		break;
	}
	if (rtn < 0)
		cd_set_state(cd, cd->i_oldstate);
	return (rtn);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_DATA Message Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
dl_w_data(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	(void) dl;
	printd(("%s: %p: -> M_DATA [%d]\n", DL_LAPD_DRV_NAME, dl, msgdsize(mp)));
	return dl_data_req(q, mp);
}
STATIC INLINE int
cd_r_data(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	(void) cd;
	printd(("%s: %p: M_DATA [%d] <-\n", DL_LAPD_DRV_NAME, cd, msgdsize(mp)));
	return cd_data_ind(q, mp);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  M_ERROR, M_HANGUP, Message Handling
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE int
cd_r_error(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	printd(("%s: %p: M_ERROR <-\n", DL_LAPD_DRV_NAME, cd));
	return m_error_all(q, cd, ((uchar *) mp->b_rptr)[0], ((uchar *) mp->b_rptr++)[1]);
}
STATIC INLINE int
cd_r_hangup(queue_t *q, mblk_t *mp)
{
	struct cd *cd = CD_PRIV(q);
	printd(("%s: %p: M_HANGUP <-\n", DL_LAPD_DRV_NAME, cd));
	return m_hangup_all(q, cd);
}

/*
 *  =========================================================================
 *
 *  PUT AND SRV
 *
 *  =========================================================================
 */
STATIC int
dl_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC int
dl_w_prim(queue_t *q, mblk_t *mp)
{
	/*
	   Fast Path 
	 */
	if (mp->b_datap->db_type == M_DATA)
		return dl_w_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return dl_w_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return dl_w_proto(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	case M_IOCTL:
		return dl_w_ioctl(q, mp);
	case M_IOCDATA:
		return dl_w_iocdata(q, mp);
	}
	swerr();
	return (-EOPNOTSUPP);
}
STATIC int
cd_r_prim(queue_t *q, mblk_t *mp)
{
	/*
	   Fast Path 
	 */
	if (mp->b_datap->db_type == M_DATA)
		return cd_r_data(q, mp);
	switch (mp->b_datap->db_type) {
	case M_DATA:
		return cd_r_data(q, mp);
	case M_PROTO:
	case M_PCPROTO:
		return cd_r_proto(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	case M_IOCACK:
		return cd_r_iocnak(q, mp);
	case M_IOCNAK:
		return cd_r_iocack(q, mp);
	case M_COPYIN:
		return cd_r_copyin(q, mp);
	case M_COPYOUT:
		return cd_r_copyout(q, mp);
	case M_ERROR:
		return cd_r_error(q, mp);
	case M_HANGUP:
		return cd_r_hangup(q, mp);
	}
	swerr();
	return (-EIO);
}
STATIC int
cd_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	}
	return (QR_PASSFLOW);
}

/*
 *  =========================================================================
 *
 *  OPEN AND CLOSE
 *
 *  =========================================================================
 */
STATIC int lapd_majors[DL_LAPD_CMAJORS] = {
	DL_LAPD_CMAJOR_0,
};

/*
 *  Open
 *  -------------------------------------------------------------------------
 *  This open function automatically allocates major device numbers up to
 *  DL_LAPD_CMAJORS device numbers maximum.  Majors will be deallocated when the
 *  module is unloaded.  This ensures that the driver only uses one major
 *  device number when less that about 250 devices are not needed.
 */
STATIC int
dl_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
{
	psw_t flags;
	int mindex = 0;
	ushort cmajor = getmajor(*devp);
	ushort cminor = getminor(*devp);
	ushort bminor = cminor;
	struct dl *dl, **dlp = &master.dl.list;
	MOD_INC_USE_COUNT;
	if (q->q_ptr != NULL) {
		MOD_DEC_USE_COUNT;
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DL_LAPD_DRV_NAME));
		MOD_DEC_USE_COUNT;
		return (EIO);
	}
	if ((cmajor && cmajor != lapd_majors[0]) || cminor >= 1) {
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	/*
	   allocate a new device 
	 */
	cminor = 1;
	lis_spin_lock_irqsave(&master.lock, &flags);
	for (dlp = &master.dl.list; *dlp; dlp = &(*dlp)->next) {
		ushort dmajor = (*dlp)->u.dev.cmajor;
		ushort dminor = (*dlp)->u.dev.cminor;
		if (!dmajor)
			continue;	/* skip headless opens */
		if (!cmajor || cmajor != dmajor || cminor < dminor)
			break;
		if (cminor > dminor || ++cminor < DL_LAPD_CMINORS)
			continue;
		if (++mindex >= DL_LAPD_CMAJORS)
			break;
		if (!(cmajor = lapd_majors[mindex])) {
			if ((cmajor =
			     lis_register_strdev(lapd_majors[mindex], &lapd_info, DL_LAPD_CMINORS,
						 DL_LAPD_DRV_NAME)) <= 0)
				break;
			lapd_majors[mindex] = cmajor;
			LIS_DEVFLAGS(cmajor) |= LIS_MODFLG_CLONE;
		}
		cminor = 0;
	}
	if (mindex >= DL_LAPD_CMAJORS || !lapd_majors[mindex]) {
		ptrace(("%s: ERROR: no device numbers available\n", DL_LAPD_DRV_NAME));
		lis_spin_unlock_irqrestore(&master.lock, &flags);
		MOD_DEC_USE_COUNT;
		return (ENXIO);
	}
	printd(("%s: opened chanracter device %d:%d\n", DL_LAPD_DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!(dl = dl_alloc_priv(q, dlp, devp, crp, bminor))) {
		ptrace(("%s: ERROR: no memory\n", DL_LAPD_DRV_NAME));
		lis_spin_unlock_irqrestore(&master.lock, &flags);
		MOD_DEC_USE_COUNT;
		return (ENOMEM);
	}
	lis_spin_unlock_irqrestore(&master.lock, &flags);
	return (0);
}

/*
 *  Close
 *  -------------------------------------------------------------------------
 */
STATIC int
dl_close(queue_t *q, int flag, cred_t *crp)
{
	struct dl *dl = DL_PRIV(q);
	psw_t flags;
	(void) flag;
	(void) crp;
	printd(("%s: closing character device %d:%d\n", DL_LAPD_DRV_NAME, dl->u.dev.cmajor,
		dl->u.dev.cminor));
	lis_spin_lock_irqsave(&master.lock, &flags);
	dl_free_priv(dl);
	lis_spin_unlock_irqrestore(&master.lock, &flags);
	MOD_DEC_USE_COUNT;
	return (0);
}

/*
 *  =========================================================================
 *
 *  LIS MODULE INITIALIZATION
 *
 *  =========================================================================
 */
STATIC int lapd_initialized = 0;
MODULE_STATIC void
lapd_terminate(void)
{
	int rtn, mindex;
	for (mindex = 0; mindex < DL_LAPD_CMAJORS; mindex++) {
		if (lapd_majors[mindex]) {
			if ((rtn = lis_unregister_strdev(lapd_majors[mindex])))
				cmn_err(CE_PANIC, "%s: Cannot unregister major %d\n",
					DL_LAPD_DRV_NAME, lapd_majors[mindex]);
			if (mindex)
				lapd_majors[mindex] = 0;
		}
	}
	lapd_term_caches();
	lapd_initialized = 0;
	return;
}
MODULE_STATIC void
lapd_init(void)
{
	int rtn, mindex = 0;
	unless(lapd_initialized, return);
	cmn_err(CE_NOTE, LAPD_BANNER);	/* console splash */
	if ((rtn = lapd_init_caches())) {
		cmn_err(CE_PANIC, "%s: ERROR; Could no allocate caches", DL_LAPD_DRV_NAME);
		goto error;
	}
	if ((rtn =
	     lis_register_strdev(lapd_majors[mindex], &lapd_info, DL_LAPD_CMINORS,
				 DL_LAPD_DRV_NAME)) <= 0) {
		cmn_err(CE_PANIC, "%s: ERROR: Cannot register 1'st major %d", DL_LAPD_DRV_NAME,
			lapd_majors[mindex]);
		goto error;
	}
#if 0
	if (!lapd_majors[mindex]) {
		dev_t dev = makedevice(LIS_CLONE, rtn);
		/*
		   we have auto allocated a major, so we have to create a clone device to access
		   the major 
		 */
		if ((rtn = lis_unlink("/dev/dl-lapd") < 0)) {
			cmn_err(CE_PANIC, "%s: ERROR: Cannot unlink /dev/dl-lapd\n",
				DL_LAPD_DRV_NAME);
			goto error;
		}
		if ((rtn = lis_mknod("/dev/dl-lapd", S_IFCHR | 0666, dev) < 0)) {
			cmn_err(CE_PANIC, "%s: ERROR: Cannot create /dev/dl-lapd\n ",
				DL_LAPD_DRV_NAME);
			goto error;
		}
	}
#endif
	lapd_majors[mindex] = rtn;
	LIS_DEVFLAGS(rtn) |= LIS_MODFLG_CLONE;
	lis_spin_lock_init(&master.lock, "dl-open-list-lock");
	lapd_initialized = 1;
	return;
      error:
	lapd_terminate();
	lapd_initialized = rtn;
	return;
}

/*
 *  =========================================================================
 *
 *  LINUX MODULE INITIALIZATION
 *
 *  =========================================================================
 */
int
init_module(void)
{
	lapd_init();
	if (lapd_initialized < 0)
		return lapd_initialized;
	return (0);
}

void
cleanup_module(void)
{
	lapd_terminate();
}
