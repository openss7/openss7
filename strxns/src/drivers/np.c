/*****************************************************************************

 @(#) $RCSfile: np.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2006/05/08 11:26:13 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/05/08 11:26:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: np.c,v $
 Revision 0.9.2.6  2006/05/08 11:26:13  brian
 - post inc problem and working through test cases

 Revision 0.9.2.5  2006/05/08 08:16:43  brian
 - module_text_address, hash alloc changes

 Revision 0.9.2.4  2006/04/22 01:08:03  brian
 - working up NP driver

 Revision 0.9.2.3  2006/04/18 22:19:37  brian
 - working up np driver

 Revision 0.9.2.2  2006/04/18 18:00:45  brian
 - working up DL and NP drivers

 Revision 0.9.2.1  2006/04/12 20:36:03  brian
 - added some experimental drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: np.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2006/05/08 11:26:13 $"

static char const ident[] =
    "$RCSfile: np.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2006/05/08 11:26:13 $";

/*
 *  This multiplexing driver is a master device driver for Network Provider streams presenting a
 *  Network Provider Interface (NPI Revision 2.0.0) Service Interface at the upper boundary.  It
 *  collects a wide range of NPI drivers into a single device hierarchy using the Linux device
 *  independent packet layer.
 *  
 *  DL streams presenting the Data Link Provider Interface (DLPI) Service Interface can be linked
 *  under the multiplexing driver at the lower multiplex.  Rather than directly providing
 *  connections to the network layer, most of these drivers will marshall packets to and from the
 *  Linux device independent packet layer.  As such, it is not necessary to link any DLPI streams
 *  for the network provider to be of use.  Network provider streams can access any Linux packet
 *  system network device.
 *
 *  Because CDI streams are only of several types: CD_HDLC, CD_BISYNC, CD_LAN, the link layer
 *  protocol (LLC, LAPB, etc) needs to be identified.
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>

#define n_tst_bit(nr,addr)	test_bit(nr,addr)
#define n_set_bit(nr,addr)	__set_bit(nr,addr)
#define n_clr_bit(nr,addr)	__clear_bit(nr,addr)

#include <linux/interrupt.h>

#include <sys/npi.h>

#include "np.h"

#define NP_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define NP_EXTRA	"Part of the OpenSS7 stack for Linux Fast-STREAMS"
#define NP_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define NP_REVISION	"OpenSS7 $RCSfile: np.c,v $ $Name:  $ ($Revision: 0.9.2.6 $) $Date: 2006/05/08 11:26:13 $"
#define NP_DEVICE	"SVR 4.2 STREAMS NPI Network Provider"
#define NP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define NP_LICENSE	"GPL"
#define NP_BANNER	NP_DESCRIP	"\n" \
			NP_EXTRA	"\n" \
			NP_COPYRIGHT	"\n" \
			NP_DEVICE	"\n" \
			NP_CONTACT
#define NP_SPLASH	NP_DESCRIP	"\n" \
			NP_REVISION

#ifdef LINUX
MODULE_AUTHOR(NP_CONTACT);
MODULE_DESCRIPTION(NP_DESCRIP);
MODULE_SUPPORTED_DEVICE(NP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(NP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-np_ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifdef LFS
#define NP_IP_DRV_ID	CONFIG_STREAMS_NP_IP_MODID
#define NP_IP_DRV_NAME	CONFIG_STREAMS_NP_IP_NAME
#define NP_IP_CMAJORS	CONFIG_STREAMS_NP_IP_NMAJOR
#define NP_IP_CMAJOR_0	CONFIG_STREAMS_NP_IP_MAJOR
#define NP_IP_UNITS	CONFIG_STREAMS_NP_IP_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_NP_MODID));
MODULE_ALIAS("streams-driver-np_ip");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_NP_MAJOR));
MODULE_ALIAS("/dev/streams/np_ip");
MODULE_ALIAS("/dev/streams/np_ip/*");
MODULE_ALIAS("/dev/streams/clone/np_ip");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(NP_IP_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(NP_IP_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(NP_IP_CMAJOR_0) "-0");
MODULE_ALIAS("char-major-" __stringify(NP_IP_CMAJOR_0) "-" __stringify(NP_IP_CMINOR));
MODULE_ALIAS("/dev/np_ip");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  STREAMS Definitions
 *  ===================
 */

#define DRV_ID		NP_IP_DRV_ID
#define CMAJORS		NP_IP_CMAJORS
#define CMAJOR_0	NP_IP_CMAJOR_0
#define UNITS		NP_IP_UNITS
#ifdef MODULE
#define DRV_BANNER	NP_IP_BANNER
#else				/* MODULE */
#define DRV_BANNER	NP_IP_SPLASH
#endif				/* MODULE */

STATIC struct module_info np_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = 0,			/* Min packet size accepted */
	.mi_maxpsz = INFPSZ,		/* Max packet size acceptd */
	.mi_hiwat = 1 << 15,		/* Hi water mark */
	.mi_hiwat = 1 << 10,		/* Lo water mark */
};

STATIC struct module_stat np_mstat = {
};

/* Upper multiplex is a N provider following the NPI. */

STATIC streamscall int np_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int np_qclose(queue_t *, int, cred_t *);

STATIC struct qinit np_rinit = {
	.qi_putp = &ss7_oput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_osrv,		/* Read service procedure */
	.qi_qopen = &np_qopen,		/* Each open */
	.qi_qclose = &np_qclose,	/* Last close */
	.qi_minfo = &np_minfo,		/* Module information */
	.qi_mstat = &np_mstat,		/* Module statistics */
};

STATIC struct qinit np_winit = {
	.qi_putp = &ss7_iput,		/* Read put procedure (message from below) */
	.qi_srvp = &ss7_isrv,		/* Read service procedure */
	.qi_minfo = &np_minfo,		/* Module information */
	.qi_mstat = &np_mstat,		/* Module statistics */
};

STATIC struct module_info dl_minfo = {
	.mi_idnum = DRV_ID,
	.mi_idname = DRV_NAME "-mux",
	.mi_minpsz = 0,
	.mi_maxpsz = INFPSZ,
	.mi_hiwat = 1 << 15,
	.mi_lowat = 1 << 10,
};

STATIC struct module_stat dl_mstat = {
};

STATIC struct qinit dl_rinit = {
	.qi_putp = &ss7_iput,
	.qi_srvp = &ss7_iput,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_mstat,
};

STATIC struct qinit dl_winit = {
	.qi_putp = &ss7_oput,
	.qi_srvp = &ss7_oput,
	.qi_minfo = &dl_minfo,
	.qi_mstat = &dl_mstat,
};

STATIC struct streamtab np_info = {
	.st_rdinit = &np_rinit,		/* Upper read queue */
	.st_wrinit = &np_winit,		/* Upper write queue */
	.st_muxrinit = &dl_rinit,
	.st_muxwinit = &dl_winit,
};

/*
 *  Addressing:
 *
 *  NSAPs (Protocol IDs) are IP protocol numbers.  NSAP addresses consist of a port number and a
 *  list of IP addreses.  If the port number is zero, any port number is used.  Initially it is only
 *  supporting IPv4.
 *
 *  There are two types of providers: connectionless and connection oriented.
 *
 *  - Connectionless providers will start delivering packets after the bind.
 *
 *  - When the NS provider is bound to multiple protocol ids, or bound or connected to multiple
 *    addresses, data will be delivered as N_DATA_IND primitives that contain the protocol id index,
 *    destination address index, and source addresses index in the DATA_xfer_flags as the highest
 *    order byte, next highest order byte and so on.  An index of 0 indicates the first bound
 *    protocol id, source address or destination address.  In this way, the high order 3 bytes of
 *    the DATA_xfer_flags are coded all zeros in the non-multiple case.
 *
 *    The NS user is also permitted to send N_DATA_REQ primitives that contain the protocol id,
 *    destination address, and source addresses, similarly encoded in the DATA_xfer_flags.  Invalid
 *    indexes will error the stream.
 *
 *  - Connection oriented provider bound as listening will start delivering packets as connection
 *    indications after the bind.  The user can either reject these with a disconnect request or can
 *    accept them with a connection response, with a slight difference from normal NPI: the
 *    responding address in the connection response is the list of peer addresses to which to
 *    establish a connection connect rather than the local responding address.
 *
 *    If the connection is accepted on the listening stream, no further connection indications will
 *    be delivered.  If accepted on another stream, further connection indications (belonging to
 *    annother association) will be delivered.
 *
 *  - Connection oriented providers bound as non-listening will deliver packets only after a
 *    successful connection establishment operation.  Connection establishment does not doe anything
 *    except to provide a port and list of addresses from which the provider is to deliver packets.
 *    This can be useful for RTP and for establishing endpoint communication with SCTP.
 *
 *  - Before bind, the provider will report both N_CLNS and N_CONS.  When bound with bind flags
 *    equal to N_CLNS in the second least significant BIND_flags byte, the provider will be
 *    connectionless.  When bound with bind flags equal to N_CONS in the second least significant
 *    BIND_flags byte, the provider will be connection-oriented.
 */

enum {
	NP_MINOR_OTHER = 0,
	NP_MINOR_ETH,
	NP_MINOR_LAPB,
	NP_MINOR_LAPD,
	NP_MINOR_LAPF,
	NP_MINOR_IP,
	NP_MINOR_X25,
	NP_MINOR_CLNL,
	NP_MINOR_CONS,
	NP_MINOR_FREE,
};

typedef streamscall int (*np_event_t) (queue_t *, struct np *, union ne_event *);
typedef streamscall int (*dl_event_t) (queue_t *, struct dl *, union dl_event *);

STATIC kmem_cache_t *np_priv_cachep;
STATIC kmem_cache_t *np_link_cachep;
STATIC kmem_cache_t *np_prot_cachep;

struct np {
	STR_DECLARATION (struct np);	/* Stream declaration */
	N_info_ack_t np_info;		/* information structure */
	np_event_t np_event;		/* event interface */
	bufq_t np_conq;			/* connection indication queue */
	uint np_coninds;		/* outstanding connection indications */
	bufq_t np_datq;			/* data acknowledgement indications */
	uint np_datinds;		/* outstanding data indications */
	uint_t BIND_flags;		/* bind flags */
	u_char ADDR_buffer[64];		/* bind addresses */
	uint_t ADDR_length;		/* bind addresses length */
	uint_t CONN_flags;		/* connection flags */
	u_char DEST_buffer[64];		/* connection addresses */
	uint_t DEST_length;		/* connection addresses length */
	u_char QOS_buffer[64];		/* quality of service */
	uint_t QOS_length;		/* quality of service length */
	u_char QOS_range_buffer[64];	/* quality of service range */
	uint_t QOS_range_length;	/* quality of service range length */
	u_char PROTOID_buffer[8];	/* bound protocol ids */
	uint_t PROTOID_length;		/* bound protocol ids length */
};

struct dl {
	STR_DECLARATION (struct dl);	/* Stream declaration */
	struct dl_info_ack dl_info;	/* information structure */
	dl_event_t dl_event;		/* event interface */
	bufq_t dl_conq;			/* connection indication queue */
	uint dl_coninds;		/* outstanding connection indications */
	bufq_t dl_datq;			/* data acknowledgement indications */
	uint dl_datinds;		/* outstanding data indications */
};

/*
 *  Hashing is a single hash table entered up to four times.  First with specific local and remote
 *  nselectors, then with wildcard local and specific remote nselectors, then with specific local
 *  and wildcard remote nselectors and finally wildcard local and wildcard remote nselectors.  Hash
 *  functions consist of a hash of the stream type, protocol id, local_nselector and remote
 *  nselector by simply adding all four and then adding the bytes together.
 */

STATIC INLINE int
np_prot_hashfn(uint type, uint prot, uint loc, uint rem)
{
	uint result;

	result = type + prot + loc + rem;
	result += (result >> 16);
	result += (result >> 8);
	result &= (NP_HASH_SIZE - 1);
	return (result);
}

struct np_prot {
	struct np_prot *next;		/* list linkage */
	struct np_prot **prev;		/* list linkage */
	struct np *np;			/* owner linage */
	struct np_prot *np_next;	/* owner list linkage */
	struct np_prot **np_prev;	/* owner list linkage */
	uint type;			/* stream type */
	uint protoid;			/* protocol id */
	uint loc_nselector;		/* local NSAP selector */
	uint rem_nselector;		/* remote NSAP selector */
};

/* counts of stream type binds */
struct np_type {
	atomic_t npt_count;
	uint npt_initialized;
};

struct np_type np_types[NP_MINOR_FREE] = {
	[NP_MINOR_OTHER] = {ATOMIC_SET(0), 0},
	[NP_MINOR_ETH] = {ATOMIC_SET(0), 0},
	[NP_MINOR_LAPB] = {ATOMIC_SET(0), 0},
	[NP_MINOR_LAPD] = {ATOMIC_SET(0), 0},
	[NP_MINOR_LAPF] = {ATOMIC_SET(0), 0},
	[NP_MINOR_IP] = {ATOMIC_SET(0), 0},
	[NP_MINOR_X25] = {ATOMIC_SET(0), 0},
	[NP_MINOR_CLNL] = {ATOMIC_SET(0), 0},
	[NP_MINOR_CONS] = {ATOMIC_SET(0), 0},
};

/* should really page align this somewhere */
struct np_prot np_hash[NP_HASH_SIZE];

STATIC INLINE struct np *
np_get(struct np *np)
{
	if (np)
		atomic_inc(&np->refcnt);
	return (np);
}
STATIC INLINE void
np_put(struct np *np)
{
	if (np && atomic_dec_and_test(&np->refcnt))
		kmem_cache_free(np_priv_cachep, np);
}
STATIC INLINE void
np_release(struct np **npp)
{
	np_put(xchg(npp, NULL));
}
STATIC INLINE struct np *
np_alloc(void)
{
	struct np *np;

	if ((np = kmem_cache_alloc(np_priv_cachep, SLAB_ATOMIC))) {
		bzero(np, sizeof(*np));
		atomic_set(&np->refcnt, 1);
		spin_lock_init(&np->lock);	/* "np-lock" */
		np->priv_put = &np_put;
		np->priv_get = &np_get;
	}
	return (np);
}

STATIC INLINE struct dl *
dl_get(struct dl *dl)
{
	if (dl)
		atomic_inc(&dl->refcnt);
	return (dl);
}
STATIC INLINE void
dl_put(struct dl *dl)
{
	if (dl && atomic_dec_and_test(&dl->refcnt))
		kmem_cache_free(np_link_cachep, dl);
}
STATIC INLINE void
dl_release(struct dl **dlp)
{
	dl_put(xchg(dlp, NULL));
}
STATIC INLINE void
dl_alloc(void)
{
	struct dl *dl;

	if ((dl = kmem_cache_alloc(np_link_cachep, SLAB_ATOMIC))) {
		bzero(dl, sizeof(*dl));
		atomic_set(&dl->refcnt, 1);
		spin_lock_init(&dl->lock);	/* "dl-lock" */
		dl->priv_put = &dl_put;
		dl->priv_get = &dl_get;
	}
	return (dl);
}

/*
 *  Some notes on logging.  This driver use the strlog(9) facility so that the
 *  driver can be debugged and traced in a production environment.  Things
 *  that are trace logged are:
 */
#define NP_LOG_NS_ERROR		0	/* issued errors */
#define NP_LOG_NSU_PRIM		1	/* NS user issued primitives */
#define NP_LOG_NSP_PRIM		2	/* NS prov issued primitives */
#define NP_LOG_NS_STATE		3	/* NS interface state changes */
#define NP_LOG_NS_TIMER		4	/* NS prov timer set, clear and expiry */

/* primitive names */
STATIC const char *
np_prim_name(np_ulong prim)
{
	switch (prim) {
	/* *INDENT-OFF* */
	case N_CONN_REQ:	return ("N_CONN_REQ");
	case N_CONN_RES:	return ("N_CONN_RES");
	case N_DISCON_REQ:	return ("N_DISCON_REQ");
	case N_DATA_REQ:	return ("N_DATA_REQ");
	case N_EXDATA_REQ:	return ("N_EXDATA_REQ");
	case N_INFO_REQ:	return ("N_INFO_REQ");
	case N_BIND_REQ:	return ("N_BIND_REQ");
	case N_UNBIND_REQ:	return ("N_UNBIND_REQ");
	case N_UNITDATA_REQ:	return ("N_UNITDATA_REQ");
	case N_OPTMGMT_REQ:	return ("N_OPTMGMT_REQ");
	case N_CONN_IND:	return ("N_CONN_IND");
	case N_CONN_CON:	return ("N_CONN_CON");
	case N_DISCON_IND:	return ("N_DISCON_IND");
	case N_DATA_IND:	return ("N_DATA_IND");
	case N_EXDATA_IND:	return ("N_EXDATA_IND");
	case N_INFO_ACK:	return ("N_INFO_ACK");
	case N_BIND_ACK:	return ("N_BIND_ACK");
	case N_ERROR_ACK:	return ("N_ERROR_ACK");
	case N_OK_ACK:		return ("N_OK_ACK");
	case N_UNITDATA_IND:	return ("N_UNITDATA_IND");
	case N_UDERROR_IND:	return ("N_UDERROR_IND");
	case N_DATACK_REQ:	return ("N_DATACK_REQ");
	case N_DATACK_IND:	return ("N_DATACK_IND");
	case N_RESET_REQ:	return ("N_RESET_REQ");
	case N_RESET_IND:	return ("N_RESET_IND");
	case N_RESET_RES:	return ("N_RESET_RES");
	case N_RESET_CON:	return ("N_RESET_CON");
	default:		return ("N_???");
	/* *INDENT-ON* */
	}
}

/* state names */
STATIC const char *
np_state_name(np_ulong state)
{
	switch (state) {
	/* *INDENT-OFF* */
	case NS_UNBND:		return("NS_UNBND");
	case NS_WACK_BREQ:	return("NS_WACK_BREQ");
	case NS_WACK_UREQ:	return("NS_WACK_UREQ");
	case NS_IDLE:		return("NS_IDLE");
	case NS_WACK_OPTREQ:	return("NS_WACK_OPTREQ");
	case NS_WACK_RRES:	return("NS_WACK_RRES");
	case NS_WCON_CREQ:	return("NS_WCON_CREQ");
	case NS_WRES_CIND:	return("NS_WRES_CIND");
	case NS_WACK_CRES:	return("NS_WACK_CRES");
	case NS_DATA_XFER:	return("NS_DATA_XFER");
	case NS_WCON_RREQ:	return("NS_WCON_RREQ");
	case NS_WRES_RIND:	return("NS_WRES_RIND");
	case NS_WACK_DREQ6:	return("NS_WACK_DREQ6");
	case NS_WACK_DREQ7:	return("NS_WACK_DREQ7");
	case NS_WACK_DREQ9:	return("NS_WACK_DREQ9");
	case NS_WACK_DREQ10:	return("NS_WACK_DREQ10");
	case NS_WACK_DREQ11:	return("NS_WACK_DREQ11");
	case NS_NOSTATES:	return("NS_NOSTATES");
	default:		return("NS_???");
	/* *INDENT-ON* */
	}
}

/* error names */
STATIC const char *
np_error_name(nl_long error)
{
	switch (error) {
	/* *INDENT-OFF* */
	case NBADADDR:		return ("NBADADDR");
	case NBADOPT:		return ("NBADOPT");
	case NACCESS:		return ("NACCESS");
	case NNOADDR:		return ("NNOADDR");
	case NOUTSTATE:		return ("NOUTSTATE");
	case NBADSEQ:		return ("NBADSEQ");
	case NSYSERR:		return ("NSYSERR");
	case NBADDATA:		return ("NBADDATA");
	case NBADFLAG:		return ("NBADFLAG");
	case NNOTSUPPORT:	return ("NNOTSUPPORT");
	case NBOUND:		return ("NBOUND");
	case NBADQOSPARAM:	return ("NBADQOSPARAM");
	case NBADQOSTYPE:	return ("NBADQOSTYPE");
	case NBADTOKEN:		return ("NBADTOKEN");
	case NNOPROTOID:	return ("NNOPROTOID");
	case -ENOBUFS:		return ("ENOBUFS");
	case -ENOMEM:		return ("ENOMEM");
	case -EAGAIN:		return ("EAGAIN");
	case -EOPNOTSUPP:	return ("EOPNOTSUPP");
	case -EINVAL:		return ("EINVAL");
	case -EFAULT:		return ("EFAULT");
	case -EPROTO:		return ("EPROTO");
	default:		return ("NERROR");
	/* *INDENT-ON* */
	}
}

/* error strings */
STATIC const char *
np_error_string(nl_long error)
{
	switch (error) {
	/* *INDENT-OFF* */
	case NBADADDR:		return ("address incorrect format or contains illegal information");
	case NBADOPT:		return ("options incorrect format or contain illegal information");
	case NACCESS:		return ("user did not have proper permissions");
	case NNOADDR:		return ("provider could not allocate address");
	case NOUTSTATE:		return ("would place interface out of state");
	case NBADSEQ:		return ("sequence number in primitive was incorrect/illegal");
	case NSYSERR:		return ("unix system error occurred");
	case NBADDATA:		return ("user data outside supported range");
	case NBADFLAG:		return ("flags were illegal/incorrect");
	case NNOTSUPPORT:	return ("primitive type not supported");
	case NBOUND:		return ("illegal second attempt to bind listener or default listener");
	case NBADQOSPARAM:	return ("qos values specified outside supported range");
	case NBADQOSTYPE:	return ("qos structure type specified is not supported");
	case NBADTOKEN:		return ("token used is not associated with an open stream");
	case NNOPROTOID:	return ("protocol id could not be allocated");
	case -ENOBUFS:		return ("message block allocation would block");
	case -ENOMEM:		return ("memory allocation would block");
	case -EAGAIN:		return ("temporary failure, retry");
	case -EBUSY:		return ("flow control encountered");
	case -EOPNOTSUPP:	return ("primitive or operation not supported");
	case -EINVAL:		return ("invalid request");
	case -EFAULT:		return ("software error");
	case -EPROTO:		return ("protocol error");
	/* *INDENT-ON* */
	}
}

/* 
 * STATE CHANGES
 */
STATIC INLINE fastcall np_ulong
np_get_state(struct np *np)
{
	return np->np_info.CURRENT_state;
}
STATIC INLINE fastcall np_ulong
np_get_statef(struct np * np)
{
	return (1 << np_get_state(np));
}
STATIC INLINE fastcall void
np_set_state(struct np *np, np_ulong state)
{
	strlog(DRV_ID, np->u.dev.cminor, NP_LOG_NS_STATE, SL_TRACE,
	       "state transition %s -> %s",
	       np_state_name(np->np_info.CURRENT_state), np_state_name(state));
	np->np_info.CURRENT_state = state;
}

/*
 *  Some notes on logging.  This driver use the strlog(9) facility so that the
 *  driver can be debugged and traced in a production environment.  Things
 *  that are trace logged are:
 */
#define DL_LOG_DLS_ERROR	0	/* issued errors */
#define DL_LOG_DLSU_PRIM	1	/* DLS user issued primitives */
#define DL_LOG_DLSP_PRIM	2	/* DLS prov issued primitives */
#define DL_LOG_DLS_STATE	3	/* DLS interface state changes */
#define DL_LOG_DLS_TIMER	4	/* DLS user timer set, clear and expiry */

STATIC const char *
dl_prim_name(dl_ulong prim)
{
	switch (prim) {
	/* *INDENT-OFF* */
	case DL_INFO_REQ:		return ("DL_INFO_REQ");
	case DL_INFO_ACK:		return ("DL_INFO_ACK");
	case DL_ATTACH_REQ:		return ("DL_ATTACH_REQ");
	case DL_DETACH_REQ:		return ("DL_DETACH_REQ");
	case DL_BIND_REQ:		return ("DL_BIND_REQ");
	case DL_BIND_ACK:		return ("DL_BIND_ACK");
	case DL_UNBIND_REQ:		return ("DL_UNBIND_REQ");
	case DL_OK_ACK:			return ("DL_OK_ACK");
	case DL_ERROR_ACK:		return ("DL_ERROR_ACK");
	case DL_SUBS_BIND_REQ:		return ("DL_SUBS_BIND_REQ");
	case DL_SUBS_BIND_ACK:		return ("DL_SUBS_BIND_ACK");
	case DL_SUBS_UNBIND_REQ:	return ("DL_SUBS_UNBIND_REQ");
	case DL_ENABMULTI_REQ:		return ("DL_ENABMULTI_REQ");
	case DL_DISABMULTI_REQ:		return ("DL_DISABMULTI_REQ");
	case DL_PROMISCON_REQ:		return ("DL_PROMISCON_REQ");
	case DL_PROMISCOFF_REQ:		return ("DL_PROMISCOFF_REQ");
	case DL_UNITDATA_REQ:		return ("DL_UNITDATA_REQ");
	case DL_UNITDATA_IND:		return ("DL_UNITDATA_IND");
	case DL_UDERROR_IND:		return ("DL_UDERROR_IND");
	case DL_UDQOS_REQ:		return ("DL_UDQOS_REQ");
	case DL_CONNECT_REQ:		return ("DL_CONNECT_REQ");
	case DL_CONNECT_IND:		return ("DL_CONNECT_IND");
	case DL_CONNECT_RES:		return ("DL_CONNECT_RES");
	case DL_CONNECT_CON:		return ("DL_CONNECT_CON");
	case DL_TOKEN_REQ:		return ("DL_TOKEN_REQ");
	case DL_TOKEN_ACK:		return ("DL_TOKEN_ACK");
	case DL_DISCONNECT_REQ:		return ("DL_DISCONNECT_REQ");
	case DL_DISCONNECT_IND:		return ("DL_DISCONNECT_IND");
	case DL_RESET_REQ:		return ("DL_RESET_REQ");
	case DL_RESET_IND:		return ("DL_RESET_IND");
	case DL_RESET_RES:		return ("DL_RESET_RES");
	case DL_RESET_CON:		return ("DL_RESET_CON");
	case DL_DATA_ACK_REQ:		return ("DL_DATA_ACK_REQ");
	case DL_DATA_ACK_IND:		return ("DL_DATA_ACK_IND");
	case DL_DATA_ACK_STATUS_IND:	return ("DL_DATA_ACK_STATUS_IND");
	case DL_REPLY_REQ:		return ("DL_REPLY_REQ");
	case DL_REPLY_IND:		return ("DL_REPLY_IND");
	case DL_REPLY_STATUS_IND:	return ("DL_REPLY_STATUS_IND");
	case DL_REPLY_UPDATE_REQ:	return ("DL_REPLY_UPDATE_REQ");
	case DL_REPLY_UPDATE_STATUS_IND:return ("DL_REPLY_UPDATE_STATUS_IND");
	case DL_XID_REQ:		return ("DL_XID_REQ");
	case DL_XID_IND:		return ("DL_XID_IND");
	case DL_XID_RES:		return ("DL_XID_RES");
	case DL_XID_CON:		return ("DL_XID_CON");
	case DL_TEST_REQ:		return ("DL_TEST_REQ");
	case DL_TEST_IND:		return ("DL_TEST_IND");
	case DL_TEST_RES:		return ("DL_TEST_RES");
	case DL_TEST_CON:		return ("DL_TEST_CON");
	case DL_PHYS_ADDR_REQ:		return ("DL_PHYS_ADDR_REQ");
	case DL_PHYS_ADDR_ACK:		return ("DL_PHYS_ADDR_ACK");
	case DL_SET_PHYS_ADDR_REQ:	return ("DL_SET_PHYS_ADDR_REQ");
	case DL_GET_STATISTICS_REQ:	return ("DL_GET_STATISTICS_REQ");
	case DL_GET_STATISTICS_ACK:	return ("DL_GET_STATISTICS_ACK");
	default:			return ("DL_???");
	/* *INDENT-ON* */
	}
}

/* state names */
STATIC const char *
dl_state_name(dl_ulong state)
{
	switch (state) {
	/* *INDENT-OFF* */
	case DL_UNATTACHED:		return ("DL_UNATTACHED");
	case DL_ATTACH_PENDING:		return ("DL_ATTACH_PENDING");
	case DL_DETACH_PENDING:		return ("DL_DETACH_PENDING");
	case DL_UNBOUND:		return ("DL_UNBOUND");
	case DL_BIND_PENDING:		return ("DL_BIND_PENDING");
	case DL_UNBIND_PENDING:		return ("DL_UNBIND_PENDING");
	case DL_IDLE:			return ("DL_IDLE");
	case DL_UDQOS_PENDING:		return ("DL_UDQOS_PENDING");
	case DL_OUTCON_PENDING:		return ("DL_OUTCON_PENDING");
	case DL_INCON_PENDING:		return ("DL_INCON_PENDING");
	case DL_CONN_RES_PENDING:	return ("DL_CONN_RES_PENDING");
	case DL_DATAXFER:		return ("DL_DATAXFER");
	case DL_USER_RESET_PENDING:	return ("DL_USER_RESET_PENDING");
	case DL_PROV_RESET_PENDING:	return ("DL_PROV_RESET_PENDING");
	case DL_RESET_RES_PENDING:	return ("DL_RESET_RES_PENDING");
	case DL_DISCON8_PENDING:	return ("DL_DISCON8_PENDING");
	case DL_DISCON9_PENDING:	return ("DL_DISCON9_PENDING");
	case DL_DISCON11_PENDING:	return ("DL_DISCON11_PENDING");
	case DL_DISCON12_PENDING:	return ("DL_DISCON12_PENDING");
	case DL_DISCON13_PENDING:	return ("DL_DISCON13_PENDING");
	case DL_SUBS_BIND_PND:		return ("DL_SUBS_BIND_PND");
	case DL_SUBS_UNBIND_PND:	return ("DL_SUBS_UNBIND_PND");
	default:			return ("DL_???");
	/* *INDENT-ON* */
	}
}

/* error names */
STATIC const char *
dl_error_name(dl_long error)
{
	switch (error) {
	/* *INDENT-OFF* */
	case DL_ACCESS:		return ("DL_ACCESS");
	case DL_BADADDR:	return ("DL_BADADDR");
	case DL_BADCORR:	return ("DL_BADCORR");
	case DL_BADDATA:	return ("DL_BADDATA");
	case DL_BADPPA:		return ("DL_BADPPA");
	case DL_BADPRIM:	return ("DL_BADPRIM");
	case DL_BADQOSPARAM:	return ("DL_BADQOSPARAM");
	case DL_BADQOSTYPE:	return ("DL_BADQOSTYPE");
	case DL_BADSAP:		return ("DL_BADSAP");
	case DL_BADTOKEN:	return ("DL_BADTOKEN");
	case DL_BOUND:		return ("DL_BOUND");
	case DL_INITFAILED:	return ("DL_INITFAILED");
	case DL_NOADDR:		return ("DL_NOADDR");
	case DL_NOTINIT:	return ("DL_NOTINIT");
	case DL_OUTSTATE:	return ("DL_OUTSTATE");
	case DL_SYSERR:		return ("DL_SYSERR");
	case DL_UNSUPPORTED:	return ("DL_UNSUPPORTED");
	case DL_UNDELIVERABLE:	return ("DL_UNDELIVERABLE");
	case DL_NOTSUPPORTED:	return ("DL_NOTSUPPORTED");
	case DL_TOOMANY:	return ("DL_TOOMANY");
	case DL_NOTENAB:	return ("DL_NOTENAB");
	case DL_BUSY:		return ("DL_BUSY");
	case DL_NOAUTO:		return ("DL_NOAUTO");
	case DL_NOXIDAUTO:	return ("DL_NOXIDAUTO");
	case DL_NOTESTAUTO:	return ("DL_NOTESTAUTO");
	case DL_XIDAUTO:	return ("DL_XIDAUTO");
	case DL_TESTAUTO:	return ("DL_TESTAUTO");
	case DL_PENDING:	return ("DL_PENDING");
	case -ENOBUFS:		return ("ENOBUFS");
	case -ENOMEM:		return ("ENOMEM");
	case -EBUSY:		return ("EBUSY");
	case -EAGAIN:		return ("EAGAIN");
	case -EOPNOTSUPP:	return ("EOPNOTSUPP");
	case -EINVAL:		return ("EINVAL");
	case -EFAULT:		return ("EFAULT");
	case -EPROTO:		return ("EPROTO");
	default:		return "(unknown)";
	/* *INDENT-ON* */
	}
}

/* error strings */
STATIC const char *
dl_error_string(dl_long error)
{
	switch (error) {
	/* *INDENT-OFF* */
	case DL_ACCESS:		return ("improper permissions for request");
	case DL_BADADDR:	return ("dlsap addr in improper format or invalid");
	case DL_BADCORR:	return ("seq number not from outstand dl_conn_ind");
	case DL_BADDATA:	return ("user data exceeded provider limit");
	case DL_BADPPA:		return ("specified ppa was invalid");
	case DL_BADPRIM:	return ("primitive received not known by provider");
	case DL_BADQOSPARAM:	return ("qos parameters contained invalid values");
	case DL_BADQOSTYPE:	return ("qos structure type is unknown/unsupported");
	case DL_BADSAP:		return ("bad lsap selector");
	case DL_BADTOKEN:	return ("token used not an active stream");
	case DL_BOUND:		return ("attempted second bind with dl_max_conind");
	case DL_INITFAILED:	return ("physical link initialization failed");
	case DL_NOADDR:		return ("provider couldn't allocate alt. address");
	case DL_NOTINIT:	return ("physical link not initialized");
	case DL_OUTSTATE:	return ("primitive issued in improper state");
	case DL_SYSERR:		return ("unix system error occurred");
	case DL_UNSUPPORTED:	return ("requested serv. not supplied by provider");
	case DL_UNDELIVERABLE:	return ("previous data unit could not be delivered");
	case DL_NOTSUPPORTED:	return ("primitive is known but not supported");
	case DL_TOOMANY:	return ("limit exceeded");
	case DL_NOTENAB:	return ("promiscuous mode not enabled");
	case DL_BUSY:		return ("other streams for ppa in post-attached state");
	case DL_NOAUTO:		return ("automatic handling of xid and test response not supported.");
	case DL_NOXIDAUTO:	return ("automatic handling of xid not supported");
	case DL_NOTESTAUTO:	return ("automatic handling of test not supported");
	case DL_XIDAUTO:	return ("automatic handling of xid response");
	case DL_TESTAUTO:	return ("automatic handling of test response");
	case DL_PENDING:	return ("pending outstanding connect indications");
	case -ENOBUFS:		return ("message block allocation would block");
	case -ENOMEM:		return ("memory allocation would block");
	case -EAGAIN:		return ("temporary failure, retry");
	case -EBUSY:		return ("flow control encountered");
	case -EOPNOTSUPP:	return ("primitive or operation not supported");
	case -EINVAL:		return ("invalid request");
	case -EFAULT:		return ("software error");
	case -EPROTO:		return ("protocol error");
	default:		return "(unknown)";
	/* *INDENT-ON* */
	}
}

STATIC INLINE fastcall dl_ulong
dl_get_state(struct dl *dl)
{
	return dl->dl_info.dl_current_state;
}
STATIC INLINE fastcall dl_ulong
dl_get_statef(struct dl * dl)
{
	return (1 << dl_get_state(np));
}
STATIC INLINE fastcall void
dl_set_state(struct dl *dl, dl_ulong state)
{
	strlog(DRV_ID, dl->u.mux.index, DL_LOG_DLS_STATE, SL_TRACE,
	       "state transitions %s -> %s",
	       dl_state_name(dl->dl_info.dl_current_state), dl_state_name(state));
	dl->dl_info.dl_current_state = state;
}

/*
 *  NS PROVIDER PRIMITIVES SENT UPSTREAM
 *  ====================================
 */

STATIC fastcall int
n_reply_ack(struct np *np, np_ulong PRIM_type, np_long error, mblk_t *mp)
{
	union N_primitives *p = (typeof(p)) mp->b_rptr;

	if (likely(error == 0)) {
		mp->b_datap->db_type = M_PCPROTO;
		mp->b_band = 0;
		p->ok_ack.PRIM_type = N_OK_ACK;
		p->ok_ack.CORRECT_prim = PRIM_type;
		mp->b_wptr = mp->b_rptr + sizeof(p->ok_ack);
		mp->b_cont = NULL;
		strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_OK_ACK");
		putnext(np->oq, mp);
		return (QR_ABSORBED);
	}
	/* Note: don't ever call this with a positive queue return code other than QR_DONE (which
	   is really zero), because those codes overlap with provider specific error codes.  The
	   ones to really watch out for are QR_ABSORBED and QR_STRIP. */
	switch (error) {
	case -ENOBUFS:
	case -ENOMEM:
	case -EAGAIN:
	case -EBUSY:
		/* These do not generate an error ack, but get returned directly */
		return (error);
	case -EPROTO:
	case -EMSGSIZE:
	case -EFAULT:
		break;
	}
	mp->b_datap->db_type = M_PCPROTO;
	mp->b_band = 0;
	p->error_ack.PRIM_type = N_ERROR_ACK;
	p->error_ack.ERROR_prim = PRIM_type;
	p->error_ack.NPI_error = error > 0 ? error : NSYSERR;
	p->error_ack.UNIX_error = error < 0 ? -error : 0;
	if (mp->b_cont)
		freemsg(XCHG(&mp->b_cont, NULL));
	np_set_state(np, np->i_oldstate);
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_ERROR_ACK");
	putnext(np->oq, mp);
	return (QR_ABSORBED);
}
STATIC fastcall int
n_reply_err(struct np *np, np_ulong PRIM_type, np_long error, mblk_t *mp)
{
	if (likely(error == 0))
		return (QR_DONE);
	switch (error) {
	case -ENOBUFS:
	case -ENOMEM:
	case -EAGAIN:
	case -EBUSY:
		/* These do not generate an error, but get returned directly */
		return (error);
	case -EPROTO:
	case -EMSGSIZE:
	case -EFAULT:
		break;
	}
	mp->b_datap->db_type = M_ERROR;
	mp->b_band = 0;
	mp->b_wptr = mp->b_rptr;
	*mp->b_wptr++ = EPROTO;
	*mp->b_wptr++ = EPROTO;
	if (mp->b_cont)
		freemsg(XCHG(&mp->b_cont, NULL));
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- M_ERROR");
	putnext(np->oq, mp);
	return (QR_ABSORBED);
}
STATIC fastcall int
ne_conn_ind(struct np *np, struct ne_conn_ind *ep)
{
	N_conn_ind_t *p;
	mblk_t *mp;
	np_ulong DEST_length = ep->DEST_length;
	np_ulong SRC_length = ep->SRC_length;
	np_ulong QOS_length = ep->QOS_length;
	size_t size = sizeof(*p) + DEST_length + SRC_length + QOS_length;

	if (unlikely((mp = ss7_allocb(np->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += size;
	p->PRIM_type = N_CONN_IND;
	p->DEST_length = DEST_length;
	p->DEST_offset = DEST_length ? sizeof(*p) : 0;
	p->SRC_length = SRC_length;
	p->SRC_offset = SRC_length ? sizeof(*p) + DEST_length : 0;
	p->SEQ_number = (np_ulong) (long) ep->SEQ_number;
	p->CONN_flags = ep->CONN_flags;
	p->QOS_length = QOS_length;
	p->QOS_offset = QOS_length ? sizeof(*p) + QOS_length : 0;
	if (likely(DEST_length))
		bcopy(ep->DEST_buffer, mp->b_rptr + p->DEST_offset, DEST_length);
	if (likely(SRC_length))
		bcopy(ep->SRC_buffer, mp->b_rptr + p->SRC_offset, SRC_length);
	if (likely(QOS_length))
		bcopy(ep->QOS_buffer, mp->b_rptr + p->QOS_offset, QOS_length);
	mp->b_cont = ep->DATA_blocks;
	bufq_queue(&np->np_conq, ep->SEQ_number);
	np_set_state(np, NS_WRES_CIND);
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_CONN_IND");
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC fastcall int
ne_conn_con(struct np *np, struct ne_conn_con *ep)
{
	N_conn_con_t *p;
	mblk_t *mp;
	np_ulong RES_length = ep->RES_length;
	np_ulong QOS_length = ep->QOS_length;
	size_t size = sizeof(*p) + RES_length + QOS_length;

	if (unlikely((mp = ss7_allocb(np->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += size;
	p->PRIM_type = N_CONN_CON;
	p->RES_length = RES_length;
	p->RES_offset = RES_length ? sizeof(*p) : 0;
	p->CONN_flags = ep->CONN_flags;
	p->QOS_length = QOS_length;
	p->QOS_offset = QOS_length ? sizeof(*p) + RES_length : 0;
	if (likely(RES_length))
		bcopy(RES_buffer, mp->b_rptr + p->RES_offset, RES_length);
	if (likely(QOS_length))
		bcopy(QOS_buffer, mp->b_rptr + p->QOS_offset, QOS_length);
	mp->b_cont = ep->DATA_blocks;
	np_set_state(np, NS_DATA_XFER);
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_CONN_CON");
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC fastcall int
ne_discon_ind(struct np *np, struct ne_discon_ind *ep)
{
	N_discon_ind_t *p;
	mblk_t *mp;
	np_ulong RES_length = ep->RES_length;
	size_t size = sizeof(*p) + RES_length;

	if (unlikely((mp = ss7_allocb(np->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += size;
	p->PRIM_type = N_DISCON_IND;
	p->DISCON_orig = ep->DISCON_orig;
	p->DISCON_reason = ep->DISCON_reason;
	p->RES_length = RES_length;
	p->RES_offset = RES_length ? sizeof(*p) : 0;
	p->SEQ_number = (np_ulong) (long) ep->SEQ_number;
	if (likely(RES_length))
		bcopy(RES_buffer, mp->b_rptr + p->RES_offset, RES_length);
	mp->b_cont = ep->DATA_blocks;
	if (ep->SEQ_number) {
		bufq_dequeue(&np->np_conq, ep->SEQ_number);
		freemsg(ep->SEQ_number);
	}
	if (bufq_length(&np->np_conq) < 1)
		np_set_state(np, NS_IDLE);
	else
		np_set_state(np, NS_WRES_CIND);
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_DISCON_IND");
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC fastcall int
ne_data_ind(struct np *np, struct ne_data_ind *ep)
{
	N_data_ind_t *p;
	mblk_t *mp;

	if (unlikely((mp = ss7_allocb(np->oq, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(np->oq)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATA_IND;
	p->DATA_xfer_flags = ep->DATA_xfer_flags;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = ep->DATA_blocks;
	if (ep->DATA_xfer_flags & RC_FLAG)
		np->np_datinds++;
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_DATA_IND");
	putnext(np->oq, mp);
	return (0);
	freeb(mp);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
STATIC fastcall int
ne_exdata_ind(struct np *np, struct ne_exdata_ind *ep)
{
	N_exdata_ind_t *p;
	mblk_t *mp;

	if (unlikely((mp = ss7_allocb(np->oq, sizeof(*p), BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!bcanputnext(np->oq, 1)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 1;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_EXDATA_IND;
	mp->b_wptr += sizeof(*p);
	mp->b_cont = ep->DATA_blocks;
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_EXDATA_IND");
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
STATIC fastcall int
ne_unitdata_ind(struct np *np, struct ne_unitdata_ind *ep)
{
	N_unitdata_ind_t *p;
	mblk_t *mp;
	np_ulong SRC_length = ep->SRC_length;
	np_ulong DEST_length = ep->DEST_length;
	size_t size = sizeof(*p) + SRC_length + DEST_length;

	if (unlikely((mp = ss7_allocb(np->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(np->oq)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += size;
	p->PRIM_type = N_UNITDATA_IND;
	p->SRC_length = SRC_length;
	p->SRC_offset = SRC_length ? sizeof(*p) : 0;
	p->DEST_length = dlen;
	p->DEST_offset = dlen ? sizeof(*p) + SRC_length : 0;
	p->ERROR_type = ERROR_type;
	if (SRC_length)
		bcopy(ep->SRC_buffer, mp->b_rptr + p->SRC_offset, SRC_length);
	if (DEST_length)
		bcopy(ep->DEST_buffer, mp->b_rptr + p->DEST_offset, DEST_length);
	mp->b_cont = ep->DATA_blocks;
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_UNITDATA_IND");
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
STATIC fastcall int
ne_uderror_ind(struct np *np, struct ne_uderror_ind *ep)
{
	N_uderror_ind_t *p;
	mblk_t *mp;
	np_ulong DEST_length = ep->DEST_length;
	size_t size = sizeof(*p) + DEST_length;

	if (unlikely((mp = ss7_allocb(np->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(np->oq)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	mp->b_wptr += size;
	p->PRIM_type = N_UDERROR_IND;
	p->DEST_length = DEST_length;
	p->DEST_offset = DEST_length ? sizeof(*p) : 0;
	p->RESERVED_field = 0;
	p->ERROR_type = ep->ERROR_type;
	if (DEST_length)
		bcopy(ep->DEST_buffer, mp->b_rptr + p->DEST_offset, DEST_length);
	mp->b_cont = ep->DATA_blocks;
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_UDERROR_IND");
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
STATIC fastcall int
ne_datack_ind(struct np *np, struct ne_datack_ind *ep)
{
	N_datack_ind_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(np->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(np->oq)))
		mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_DATACK_IND;
	mp->b_wptr += sizeof(*p);
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_DATACK_IND");
	putnext(np->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
STATIC fastcall int
ne_reset_ind(struct np *np, struct ne_reset_ind *ep)
{
	N_reset_ind_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(np->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_IND;
	p->RESET_orig = ep->RESET_orig;
	p->RESET_reason = ep->RESET_reason;
	mp->b_wptr += sizeof(*p);
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_RESET_IND");
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC fastcall int
ne_reset_con(struct np *np, struct ne_reset_con *ep)
{
	N_reset_con_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(np->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->PRIM_type = N_RESET_CON;
	mp->b_wptr += sizeof(*p);
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_RESET_CON");
	putnext(np->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  DLS USER PRIMITIVES SENT DOWNSTREAM
 *  ===================================
 */

STATIC int
dle_info_req(struct dl *dl, struct dle_info_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_INFO_REQ;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_INFO_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_attach_req(struct dl *dl, struct dle_attach_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_ATTACH_REQ;
	p->dl_ppa = ep->dl_ppa;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, DL_ATTACH_PENDING);
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_ATTACH_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_detach_req(struct dl *dl, struct dle_detach_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_DETACH_REQ;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, DL_DETACH_PENDING);
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_DETACH_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_bind_req(struct dl *dl, struct dle_bind_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_BIND_REQ;
	p->dl_sap = ep->dl_sap;
	p->dl_max_conind = ep->dl_max_conind;
	p->dl_service_mode = ep->dl_service_mode;
	p->dl_conn_mgmt = ep->dl_conn_mgmt;
	p->dl_xidtest_flg = ep->dl_xidtest_flg;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, DL_BIND_PENDING);
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_BIND_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_unbind_req(struct dl *dl, struct dle_unbind_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_UNBIND_REQ;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, DL_UNBIND_PENDING);
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_UNBIND_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_subs_bind_req(struct dl *dl, struct dle_subs_bind_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_subs_sap_length = ep->dl_subs_sap_length;
	size_t size = sizeof(*p) + dl_subs_sap_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_SUBS_BIND_REQ;
	p->dl_subs_sap_offset = dl_subs_sap_length ? sizeof(*p) : 0;
	p->dl_subs_sap_length = dl_subs_sap_length;
	p->dl_subs_bind_class = ep->dl_subs_bind_class;
	mp->b_wptr += sizeof(*p);
	if (dl_subs_sap_length) {
		bcopy(ep->dl_subs_sap_buffer, mp->b_wptr, dl_subs_sap_length);
		mp->b_wptr += dl_subs_sap_length;
	}
	dl_set_state(dl, DL_SUBS_BIND_PND);
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_SUBS_BIND_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_subs_unbind_req(struct dl *dl, struct dle_subs_unbind_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_subs_sap_length = ep->dl_subs_sap_length;
	size_t size = sizeof(*p) + dl_subs_sap_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_SUBS_UNBIND_REQ;
	p->dl_subs_sap_offset = dl_subs_sap_length ? sizeof(*p) : 0;
	p->dl_subs_sap_length = dl_subs_sap_length;
	mp->b_wptr += sizeof(*p);
	if (dl_subs_sap_length) {
		bcopy(ep->dl_subs_sap_buffer, mp->b_wptr, dl_subs_sap_length);
		mp->b_wptr += dl_subs_sap_length;
	}
	dl_set_state(dl, DL_SUBS_UNBIND_PND);
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_SUBS_UNBIND_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_enabmulti_req(struct dl *dl, struct dle_enabmulti_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_addr_length = ep->dl_addr_length;
	size_t size = sizeof(*p) + dl_addr_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_ENABMULTI_REQ;
	p->dl_addr_offset = dl_addr_length ? sizeof(*p) : 0;
	p->dl_addr_length = dl_addr_length;
	mp->b_wptr += sizeof(*p);
	if (dl_addr_length) {
		bcopy(ep->dl_addr_buffer, mp->b_wptr, dl_addr_length);
		mp->b_wptr += dl_addr_length;
	}
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_ENABMULTI_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_disabmulti_req(struct dl *dl, struct dle_disabmulti_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_addr_length = ep->dl_addr_length;
	size_t size = sizeof(*p) + dl_addr_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_DISABMULTI_REQ;
	p->dl_addr_offset = dl_addr_length ? sizeof(*p) : 0;
	p->dl_addr_length = dl_addr_length;
	mp->b_wptr += sizeof(*p);
	if (dl_addr_length) {
		bcopy(ep->dl_addr_buffer, mp->b_wptr, dl_addr_length);
		mp->b_wptr += dl_addr_length;
	}
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_DISABMULTI_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_promiscon_req(struct dl *dl, struct dle_promiscon_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_PROMISCON_REQ;
	p->dl_level = ep->dl_level;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_PROMISCON_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_promiscoff_req(struct dl *dl, struct dle_promiscoff_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_PROMISCOFF_REQ;
	p->dl_level = ep->dl_level;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_PROMISCOFF_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_unitdata_req(struct dl *dl, struct dle_unitdata_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = ep->dl_dest_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(dl->oq)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_UNITDATA_REQ;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_priority = ep->dl_priority;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(ep->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	mp->b_cont = ep->dl_data_blocks;
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_UNITDATA_REQ");
	putnext(dl->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_udqos_req(struct dl *dl, struct dle_udqos_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_qos_length = ep->dl_qos_length;
	size_t size = sizeof(*p) + dl_qos_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_UDQOS_REQ;
	p->dl_qos_offset = dl_qos_length ? sizeof(*p) : 0;
	p->dl_qos_length = dl_qos_length;
	mp->b_wptr += sizeof(*p);
	if (dl_qos_length) {
		bcopy(ep->dl_qos_buffer, mp->b_wptr, dl_qos_length);
		mp->b_wptr += dl_qos_length;
	}
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_UDQOS_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_connect_req(struct dl *dl, struct dle_connect_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = ep->dl_dest_addr_length;
	dl_ulong dl_qos_length = ep->dl_qos_length;
	size_t size = sizeof(*p) + dl_dest_addr_length = dl_qos_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_CONNECT_REQ;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_qos_offset = dl_qos_length ? sizeof(*p) + dl_dest_addr_length : 0;
	p->dl_qos_length = dl_qos_length;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(ep->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	if (dl_qos_length) {
		bcopy(ep->dl_qos_buffer, mp->b_wptr, dl_qos_length);
		mp->b_wptr += dl_qos_length;
	}
	dl_set_state(dl, DL_OUTCON_PENDING);
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_CONNECT_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_connect_res(struct dl *dl, struct dle_connect_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_qos_length = ep->dl_qos_length;
	size_t size = sizeof(*p) + dl_qos_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_CONNECT_RES;
	p->dl_correlation = (dl_ulong) (long) ep->dl_correlation;
	p->dl_resp_token = (dl_ulong) (long) ep->dl_resp_token;
	p->dl_qos_length = dl_qos_length;
	p->dl_qos_offset = dl_qos_length ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	if (dl_qos_length) {
		bcopy(ep->dl_qos_buffer, mp->b_wptr, dl_qos_length);
		mp->b_wptr += dl_qos_length;
	}
	dl_set_state(dl, DL_CONN_RES_PENDING);
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_CONNECT_RES");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_token_req(struct dl *dl, struct dle_token_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_TOKEN_REQ;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_TOKEN_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_disconnect_req(struct dl *dl, struct dle_disconnect_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_DISCONNECT_REQ;
	p->dl_reason = ep->dl_reason;
	p->dl_correlation = (dl_ulong) (long) ep->dl_correlation;
	mp->b_wptr += sizeof(*p);
	switch (dl_get_state(dl)) {
	case DL_OUTCON_PENDING:
		dl_set_state(dl, DL_DISCON8_PENDING);
		break;
	case DL_INCON_PENDING:
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
	}
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_DISCONNECT_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_reset_req(struct dl *dl, struct dle_reset_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_RESET_REQ;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, DL_USER_RESET_PENDING);
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_RESET_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_reset_res(struct dl *dl, struct dle_reset_res *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_RESET_RES;
	mp->b_wptr += sizeof(*p);
	dl_set_state(dl, DL_RESET_RES_PENDING);
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_RESET_RES");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_data_ack_req(struct dl *dl, struct dle_data_ack_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = ep->dl_dest_addr_length;
	dl_ulong dl_src_addr_length = ep->dl_src_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length + dl_src_addr_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	if (unlikely(!canputnext(dl->oq)))
		goto ebusy;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_DATA_ACK_REQ;
	p->dl_correlation = ep->dl_correlation;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_src_addr_length = dl_src_addr_length;
	p->dl_src_addr_offset = dl_src_addr_length ? sizeof(*p) + dl_dest_addr_length : 0;
	p->dl_priority = ep->dl_priority;
	p->dl_service_class = ep->dl_service_class;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(ep->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	if (dl_src_addr_length) {
		bcopy(ep->dl_src_addr_buffer, mp->b_wptr, dl_src_addr_length);
		mp->b_wptr += dl_src_addr_length;
	}
	mp->b_cont = ep->dl_data_blocks;
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_DATA_ACK_REQ");
	putnext(dl->oq, mp);
	return (0);
      ebusy:
	freeb(mp);
	return (-EBUSY);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_reply_req(struct dl *dl, struct dle_reply_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = ep->dl_dest_addr_length;
	dl_ulong dl_src_addr_length = ep->dl_src_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length + dl_src_addr_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_REPLY_REQ;
	p->dl_correlation = ep->dl_correlation;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	p->dl_src_addr_length = dl_src_addr_length;
	p->dl_src_addr_offset = dl_src_addr_length ? sizeof(*p) + dl_dest_addr_length : 0;
	p->dl_priority = ep->dl_priority;
	p->dl_service_class = ep->dl_service_class;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(ep->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	if (dl_src_addr_length) {
		bcopy(ep->dl_src_addr_buffer, mp->b_wptr, dl_src_addr_length);
		mp->b_wptr += dl_src_addr_length;
	}
	mp->b_cont = ep->dl_data_blocks;
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_REPLY_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_reply_update_req(struct dl *dl, struct dle_reply_update_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_src_addr_length = ep->dl_src_addr_length;
	size_t size = sizeof(*p) + dl_src_addr_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_REPLY_UPDATE_REQ;
	p->dl_correlation = ep->dl_correlation;
	p->dl_src_addr_length = dl_src_addr_length;
	p->dl_src_addr_offset = dl_src_addr_length ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	if (dl_src_addr_length) {
		bcopy(ep->dl_src_addr_buffer, mp->b_wptr, dl_src_addr_length);
		mp->b_wptr += dl_src_addr_length;
	}
	mp->b_cont = ep->dl_data_blocks;
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_REPLY_UPDATE_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_xid_req(struct dl *dl, dle_xid_req * ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = ep->dl_dest_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_XID_REQ;
	p->dl_flag = ep->dl_flag;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(ep->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	mp->b_cont = ep->dl_data_blocks;
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_XID_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_xid_res(struct dl *dl, dle_xid_res * ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = ep->dl_dest_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_XID_RES;
	p->dl_flag = ep->dl_flag;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(ep->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	mp->b_cont = ep->dl_data_blocks;
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_XID_RES");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_test_req(struct dl *dl, struct dle_test_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = ep->dl_dest_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_TEST_REQ;
	p->dl_flag = ep->dl_flag;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(ep->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	mp->b_cont = ep->dl_data_blocks;
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_TEST_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_test_res(struct dl *dl, struct dle_test_res *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_dest_addr_length = ep->dl_dest_addr_length;
	size_t size = sizeof(*p) + dl_dest_addr_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_TEST_RES;
	p->dl_flag = ep->dl_flag;
	p->dl_dest_addr_length = dl_dest_addr_length;
	p->dl_dest_addr_offset = dl_dest_addr_length ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	if (dl_dest_addr_length) {
		bcopy(ep->dl_dest_addr_buffer, mp->b_wptr, dl_dest_addr_length);
		mp->b_wptr += dl_dest_addr_length;
	}
	mp->b_cont = ep->dl_data_blocks;
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_TEST_RES");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_phys_addr_req(struct dl *dl, struct dle_phys_addr_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	size_t size = sizeof(*p);

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_PHYS_ADDR_REQ;
	p->dl_addr_type = ep->dl_addr_type;
	mp->b_wptr += sizeof(*p);
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_PHYS_ADDR_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}
STATIC int
dle_set_phys_addr_req(struct dl *dl, struct dle_set_phys_addr_req *ep)
{
	dl_info_req_t *p;
	mblk_t *mp;
	dl_ulong dl_addr_length = ep->dl_addr_length;
	size_t size = sizeof(*p) + dl_addr_length;

	if (unlikely((mp = ss7_allocb(dl->oq, size, BPRI_MED)) == NULL))
		goto enobufs;
	mp->b_datap->db_type = M_PROTO;
	p = (typeof(p)) mp->b_wptr;
	p->dl_primitive = DL_SET_PHYS_ADDR_REQ;
	p->dl_addr_length = dl_addr_length;
	p->dl_addr_offset = dl_addr_length ? sizeof(*p) : 0;
	mp->b_wptr += sizeof(*p);
	if (dl_addr_length) {
		bcopy(ep->dl_addr_buffer, mp->b_wptr, dl_addr_length);
		mp->b_wptr += dl_addr_length;
	}
	dl_set_state(dl, dl_get_state(dl));
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSU_PRIM, SL_TRACE, "-> DL_SET_PHYS_ADDR_REQ");
	putnext(dl->oq, mp);
	return (0);
      enobufs:
	return (-ENOBUFS);
}

/*
 *  CORE NETWORK AND PACKET LAYER INTERFACE
 *  =======================================
 */

/*
 *  Core packet layer interface
 *  ---------------------------
 *  When a DL stream is linked under the multiplexing driver, we interrogate the DLS provider and
 *  create a new network device of the appropriate type.  The device should not process its own
 *  MPoxx and SNAP headers on received frames unless it presents an LLC link protocol (i.e. instead
 *  of a MAC protocol).  The device is responsible for adding its own MPoxx and SNAP headers on
 *  transmitted frames, unless it presents an LLC link protocol (i.e. instead of a MAC protocol).
 *
 *  MAC protocols are DL_ETHER, DL_HDLC, DL_CHAR, DL_ATM, DL_ASYNC.  LLC protocols are DL_CSMACD,
 *  DL_TPB, DL_TPR, DL_METRO, DL_ETH_CSMA, DL_IPATM, DL_X25, DL_ISDN, DL_FRAME, DL_MPFRAME,
 *  DL_IPX25.  Some of these are LLC/MPE (SNAP or MPoxx).
 *
 *  In general, we want to process raw LLC packets and will provide LAPB (X.25 and Q.921) and LAPF
 *  (Q.922) procedures here as required in the network portion.  Rather than use the Linux X.25
 *  subsystem (which is sparsely implemented and missing on most production kernels), we provide
 *  X.25, Q.921, and Q.922 LAPB/LAPF procedures here, as well as 802.2 LLC2, and allow other protcol
 *  stacks to register against network selectors on these interfaces.  This permits X.25, Q.921 and
 *  Q.922 over Ethernet and 802.2 devices.  We also need to provide interfaces for ARP (neighbor
 *  discovery subsystem in Linux), and provision for X.121 addressing and routing.
 *
 *  Correspondences from mac_type to network device type:
 *
 *  Real ARP:
 *
 *  DL_CMSACD/DL_TPB/DL_TPR	ARPHRD_IEEE802
 *  DL_TPR			ARPHRD_IEEE802/ARPHRD_IEEE802_TR
 *  DL_FRAME			ARPHRD_DLCI/ARPHRD_FRAD
 *  DL_ATM			ARPHRD_ATM
 *
 *  Proxy ARP:
 *
 *  DL_X25			ARPHRD_X25/ARPHDR_LAPB/ARPHRD_HWX25
 *  DL_FDDI			ARPHRD_FDDI
 *  DL_FC			ARPHRD_FCPP/ARPHRD_FCAL/ARPHRD_FCPL/ARPHRD_FCFABRIC
 *  DL_HIPPI			ARPHRD_HIPPI
 *  DL_HDLC			ARPHRD_HDLC/ARPHRD_RAWHDLC
 *  DL_LOOP			ARPHRD_LOOPBACK
 */

/*
 *  CORE NEWTORK LAYER INTERFACE
 *  ============================
 *
 *  When binding, use arp cache to determine network interface from NSAP.  Use NSAP selector to
 *  multiplex stream from NSAP.  This is easy for connectionless, and even acknowledged
 *  connectionless.  It is more difficult for connection oriented, as we must maintain a connection
 *  table.  For the most part we don't care about LLC, SNAP headers, or MPoX, MPoFR, MPoA
 *  multiprotocol or SNAP headers as long as an appropriate pseudo-netdevice has been defined for
 *  the Linux packet subsystem.
 */

/*
 *  Packet Interface
 *  ----------------
 *  This is the low level packet interface for receiving packets.   It registers packets under the
 *  Linux packet handler by real or pseudo EtherTypes (e.g. ETH_P_IP).  It is possible to register
 *  for ETH_P_ALL and receive all packets for examination, but that is probably more than we need.
 *  Connection, Listen, and Bind hashes are checked in that order.  Hashing is performed at the
 *  first level on Network selectors, and at the second level on network addresses.  Connection
 *  hashes are Local-NSel/Remote-NSel, Listen hashes are on Local-NSel, Bind hashes are also on
 *  Local-NSel, so it is possible to only have two hashes, or even just one.  For most traditional
 *  IP protocols, the NSel is port number.  For encapsulated protocols, we allow the encapsulation
 *  to be handled by the Linux packet layer, or provide a normal Linux packet handler to handle
 *  deencapsulation.  For example, for ISO-grams (CLNL) over UDP port 147, we provide a non-binding
 *  packet handler that de-encapsulates UDP port 147 and presents them to the CLNS packet handler
 *  as ETH_P_CLNS (not defined by the Linux kernel but we can use one of the psudeo EtherTypes such
 *  as the original DIX code 0xFE).  Similarly, ISO-grams (CLNL) on IP protocol 80, can be
 *  deencapsulated and placed back on the wheel as ETH_P_CLNL.  Then the CLNL driver registers for
 *  ETH_P_CLNL and performs selectors from there.  The NP_IP provider is intended for traditional
 *  UDP/TCP/SCTP protocols.
 *
 *  Taking this approach, it is possible to have an NS provider specialized to each of the
 *  EtherTypes (DIX or non-DIX) provided by the Linux kernel and any other DIX or non-DIX types that
 *  we provide here.  The current EtherType list for Linux (2.6.15) is:
 *
 *  #define ETH_P_LOOP		0x0060	// Ethernet Loopback packet
 *  #define ETH_P_PUP		0x0200	// Xerox PUP packet
 *  #define ETH_P_PUPAT		0x0201	// Xerox PUP Addr Trans packet
 *  #define ETH_P_IP		0x0800	// Internet Protocol packet
 *  #define ETH_P_X25		0x0805	// CCITT X.25
 *  #define ETH_P_ARP		0x0806	// Address Resolution packet
 *  #define ETH_P_BPQ		0x08FF	// G8BPQ AX.25 Ethernet Packet [ NOT AN OFFICIALLY REGISTERED ID ]
 *  #define ETH_P_IEEEPUP	0x0a00	// Xerox IEEE802.3 PUP packet
 *  #define ETH_P_IEEEPUPAT	0x0a01	// Xerox IEEE802.3 PUP Addr Trans packet
 *  #define ETH_P_DEC		0x6000	// DEC Assigned proto
 *  #define ETH_P_DNA_DL	0x6001	// DEC DNA Dump/Load
 *  #define ETH_P_DNA_RC	0x6002	// DEC DNA Remote Console
 *  #define ETH_P_DNA_RT	0x6003	// DEC DNA Routing
 *  #define ETH_P_LAT		0x6004	// DEC LAT
 *  #define ETH_P_DIAG		0x6005	// DEC Diagnostics
 *  #define ETH_P_CUST		0x6006	// DEC Customer use
 *  #define ETH_P_SCA		0x6007	// DEC Systems Comms Arch
 *  #define ETH_P_RARP		0x8035	// Reverse Addr Res packet
 *  #define ETH_P_ATALK		0x809B	// Appletalk DDP
 *  #define ETH_P_AARP		0x80F3	// Appletalk AARP
 *  #define ETH_P_8021Q		0x8100	// 802.1Q VLAN Extended Header
 *  #define ETH_P_IPX		0x8137	// IPX over DIX
 *  #define ETH_P_IPV6		0x86DD	// IPv6 over bluebook
 *  #define ETH_P_WCCP		0x883E	// Web-cache coordination protocol defined in draft-wilson-wrec-wccp-v2-00.txt
 *  #define ETH_P_PPP_DISC	0x8863	// PPPoE discovery messages
 *  #define ETH_P_PPP_SES	0x8864	// PPPoE session messages
 *  #define ETH_P_MPLS_UC	0x8847	// MPLS Unicast traffic
 *  #define ETH_P_MPLS_MC	0x8848	// MPLS Multicast traffic
 *  #define ETH_P_ATMMPOA	0x884c	// MultiProtocol Over ATM
 *  #define ETH_P_ATMFATE	0x8884	// Frame-based ATM Transport over Ethernet
 *  #define ETH_P_AOE		0x88A2	// ATA over Ethernet
 *
 *  // Non DIX types. Won't clash for 1500 types.
 *
 *  #define ETH_P_802_3		0x0001	// Dummy type for 802.3 frames
 *  #define ETH_P_AX25		0x0002	// Dummy protocol id for AX.25
 *  #define ETH_P_ALL		0x0003	// Every packet (be careful!!!)
 *  #define ETH_P_802_2		0x0004	// 802.2 frames
 *  #define ETH_P_SNAP		0x0005	// Internal only
 *  #define ETH_P_DDCMP		0x0006	// DEC DDCMP: Internal only
 *  #define ETH_P_WAN_PPP	0x0007	// Dummy type for WAN PPP frames
 *  #define ETH_P_PPP_MP	0x0008	// Dummy type for PPP MP frames
 *  #define ETH_P_LOCALTALK	0x0009	// Localtalk pseudo type
 *  #define ETH_P_PPPTALK	0x0010	// Dummy type for Atalk over PPP
 *  #define ETH_P_TR_802_2	0x0011	// 802.2 frames
 *  #define ETH_P_MOBITEX	0x0015	// Mobitex (kaz@cafe.net)
 *  #define ETH_P_CONTROL	0x0016	// Card specific control frames
 *  #define ETH_P_IRDA		0x0017	// Linux-IrDA
 *  #define ETH_P_ECONET	0x0018	// Acorn Econet
 *  #define ETH_P_HDLC		0x0019	// HDLC frames
 *  #define ETH_P_ARCNET	0x001A	// 1A for ArcNet :-)
 *
 *  It is probably worth noting that the Linux EtherNet, LLC and SNAP handlers (and even GRE) do not
 *  rely on a definition in this list.  Whatever is present in the EtherType field or the SNAP
 *  header will be used as the packet type.  Therefore, any true DIX types, not present in this list
 *  can still be used.  Of primary interest to us are CLNL (DIX 0x00FE, which is only usable on
 *  Ethernet only or SNAP on 802.2 because it conflicts with the 1500 byte length field, but is also
 *  defined for GRE) and X.75 Internet (DIX 0x0801 which is combined Ethernet/802.2 usable) and X.25
 *  ISO 8881 (DIX 0x0805 as defined as ETH_P_X25 above).
 *
 *  Then there is the LLC LSAP definitions (which are the address fields of the LLC header) and
 *  consist of one byte.  They are as follows:
 *
 *  #define LSAP_NULL	0x00	// Null LSAP
 *  #define LSAP_IMGMT	0x02	// Individual LLC Sublayer Management
 *  #define LSAP_GMGMT	0x03	// Group LLC Sublayer Management
 *  #define LSAP_PTSNA	0x04	// SNA Path Control
 *  #define LSAP_DODIP	0x06	// DOD IP (Internet Protocol)
 *  #define LSAP_PWLAN	0x0E	// PROWAY-LAN
 *  #define LSAP_RS511	0x4E	// EIA-RS 511
 *  #define LSAP_ISIIP	0x5E	// ISI IP (Experimental IP)
 *  #define LSAP_PWLA2	0x8E	// PROWAY-LAN
 *  #define LSAP_SNAP	0xAA	// SNAP
 *  #define LSAP_IPX	0xE0	// IPX
 *  #define LSAP_CLNS	0xFE	// ISO CLNS IS 8473
 *  #define LSAP_DSAP	0xFF	// Global DSAP
 *
 *  Guess what: IEEE specifies that, where possible, LSAP assignments and ISO 9577 (X.263)
 *  assignments will be the same.
 *
 *  The only ones of interest above are IP (0x06), SNAP (0xAA) and CLNS (0xFE).  Again, nothing has
 *  to be defined in this list for the Linux LLC layer to deliver to any protocol handler registered
 *  against one of these numbers.  SNAP (0xAA) and IPX (0xE0) are handled by Linux.   Interesting,
 *  but DOD IP (0x06) is not handled by Linux.
 *
 *  For MPoxx (MultiProtocol over X.25, MPoX, MultiProtocol over Frame Relay, MPoFR, MultiProtocol
 *  over ATM, MPoA) there are a different set of protocol encapsulation identifiers as follows:
 *
 *  #define NLPID_BX25	0xC5	// Blacker X.25
 *  #define NLPID_ISOIP	0xCD	// ISO-IP
 *  #define NLPID_IP	0xCC	// Internet Protocol Datagram
 *  #define NLPID_SNAP	0x80	// IEEE Subnetwork Access Protocol
 *  #define NLPID_CLNP	0x81	// ISO/IEC 8473 (also ESIS and ISIS)
 *  #define NLPID_ESIS	0x82	// ISO/IEC 9542
 *  #define NLPID_ISIS	0x83	// ISO/IEC ISIS
 *  #define NLPID_Q933	0x08	// CCITT Q.933 (Frame Relay)
 *  #define NLPID_PAD	0x00	// Null Protocol, protocol selection on frame by frame basis
 *
 *  These, I discovered, are actually defined by X.263|ISO/IEC 9577 IPI (Initial Protocol
 *  Identifier) and SPI (Subsequent Protocol Identifier) and the following list are defined by that
 *  standard:
 *
 *  0x00    Null Network Layer (see X.233|ISO/IEC 8473-1)
 *  0x01    T.70 Minimum Network layer functionality
 *  0x03    X.633 (Network layer fast-byte protocol)
 *  0x08    Q.931,Q.932,Q.933,X.36,ISO/IEC 11572,ISO/IEC 11582
 *  0x0A    Q.2119
 *  0x09    Q.2931 (Broadband ISDN Signalling Protocol)
 *  0x44    G.764
 *  0x80    IEEE SNAP
 *  0x81    X.233 | ISO/IEC 8473-1 (exclusing the inactive subset)
 *  0x82    ISO/IEC 9542
 *  0x83    ISO/IEC 10589
 *  0x84    ISO/IEC 8878 Annex A (SPI only)
 *  0x85    ISO/IEC 10747 (not used, coverd by X.233 | ISO/IEC 8473-1 IPI)
 *  0x8A    ISO/IEC 10030
 *  0x8B    X.273 | ISO/IEC 11577
 *  0xA0    X.37 (SPI only)
 *  0xA1    X.39 (SPI only)
 *  0xA2    X.634 (SPI only)
 *  0xA4    X.48 and X.49 (SPI only)
 *  0xA8    X.37 (encapsulation of frame relay)
 *  0xB0    Data compression protocol (first octet of decompressed packet is another IPI)
 *  0xCC    Annex C (IP - Internet Protocol)
 *  0xCF    Annex C (SPI only) (PPP - Point to Point Protocol)
 *  0x8F    Private Network layer protocols
 *
 *  These are define under Linux for wanrouter; however, only IP and SNAP are handled.
 */

/*
 *  STATE MACHINES for Individual Provider Types
 *  --------------------------------------------
 */

/*
 * NP_OTHER
 * --------
 */

/* Information */

STATIC N_info_ack_t np_other_info = {
	.PRIM_type = N_INFO_ACK,
	.NSDU_size = QOS_UNKNOWN,
	.ENSDU_size = QOS_UNKNOWN,
	.CDATA_size = QOS_UNKNOWN,
	.DDATA_size = QOS_UNKNOWN,
	.ADDR_size = QOS_UNKNOWN,
	.OPTIONS_flags = 0,
	.NIDU_size = 4096,
	.SERV_type = (N_CONS | N_CLNS),
	.CURRENT_state = NS_UNBND,
	.PROVIDER_type = N_SUBNET,
	.NODU_size = 4096,
	.NPI_version = N_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
np_other_event(queue_t *q, struct np *np, union ne_event *ep)
{
	return (NNOTSUPPORT);
}

/*
 * NP_ETH
 * --------
 */

/* Information */

STATIC N_info_ack_t np_eth_info = {
	.PRIM_type = N_INFO_ACK,
	.NSDU_size = 1492,
	.ENSDU_size = QOS_UNKNOWN,
	.CDATA_size = QOS_UNKNOWN,
	.DDATA_size = QOS_UNKNOWN,
	.ADDR_size = 4,			/* XXX */
	.OPTIONS_flags = 0,
	.NIDU_size = 1492,
	.SERV_type = N_CLNS,
	.CURRENT_state = NS_UNBND,
	.PROVIDER_type = N_SNICFP,
	.NODU_size = 1492,
	.NPI_version = N_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
np_eth_event(queue_t *q, struct np *np, union ne_event *ep)
{
	return (NNOTSUPPORT);
}

/*
 * NP_LAPB
 * --------
 */

/* Information */

STATIC N_info_ack_t np_lapb_info = {
	.PRIM_type = N_INFO_ACK,
	.NSDU_size = QOS_UNKNOWN,
	.ENSDU_size = QOS_UNKNOWN,
	.CDATA_size = QOS_UNKNOWN,
	.DDATA_size = QOS_UNKNOWN,
	.ADDR_size = QOS_UNKNOWN,
	.OPTIONS_flags = 0,
	.NIDU_size = 4096,
	.SERV_type = (N_CONS | N_CLNS),
	.CURRENT_state = NS_UNBND,
	.PROVIDER_type = N_SUBNET,
	.NODU_size = 4096,
	.NPI_version = N_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
np_lapb_event(queue_t *q, struct np *np, union ne_event *ep)
{
	return (NNOTSUPPORT);
}

/*
 * NP_LAPD
 * --------
 */

/* Information */

STATIC N_info_ack_t np_lapd_info = {
	.PRIM_type = N_INFO_ACK,
	.NSDU_size = QOS_UNKNOWN,
	.ENSDU_size = QOS_UNKNOWN,
	.CDATA_size = QOS_UNKNOWN,
	.DDATA_size = QOS_UNKNOWN,
	.ADDR_size = QOS_UNKNOWN,
	.OPTIONS_flags = 0,
	.NIDU_size = 4096,
	.SERV_type = (N_CONS | N_CLNS),
	.CURRENT_state = NS_UNBND,
	.PROVIDER_type = N_SUBNET,
	.NODU_size = 4096,
	.NPI_version = N_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
np_lapd_event(queue_t *q, struct np *np, union ne_event *ep)
{
	return (NNOTSUPPORT);
}

/*
 * NP_LAPF
 * --------
 */

/* Information */

STATIC N_info_ack_t np_lapf_info = {
	.PRIM_type = N_INFO_ACK,
	.NSDU_size = QOS_UNKNOWN,
	.ENSDU_size = QOS_UNKNOWN,
	.CDATA_size = QOS_UNKNOWN,
	.DDATA_size = QOS_UNKNOWN,
	.ADDR_size = QOS_UNKNOWN,
	.OPTIONS_flags = 0,
	.NIDU_size = 4096,
	.SERV_type = (N_CONS | N_CLNS),
	.CURRENT_state = NS_UNBND,
	.PROVIDER_type = N_SUBNET,
	.NODU_size = 4096,
	.NPI_version = N_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
np_lapf_event(queue_t *q, struct np *np, union ne_event *ep)
{
	return (NNOTSUPPORT);
}

/* ========================================================================== */
/* ========================================================================== */
/* ========================================================================== */

/*
 * NP_IP
 * --------
 */

/* Information */

STATIC N_info_ack_t np_ip_info = {
	.PRIM_type = N_INFO_ACK,
	.NSDU_size = 0x0000ffff,
	.ENSDU_size = QOS_UNKNOWN,
	.CDATA_size = QOS_UNKNOWN,
	.DDATA_size = QOS_UNKNOWN,
	.ADDR_size = QOS_UNKNOWN,
	.OPTIONS_flags = 0,
	.NIDU_size = 0x0000ffff,
	.SERV_type = N_CLNS,
	.CURRENT_state = NS_UNBND,
	.PROVIDER_type = N_SUBNET,
	.NODU_size = 576,
	.NPI_version = N_CURRENT_VERSION,
};

/* 
 * Packet Handling
 * ---------------
 * Notes on packet handling:  To avoid duplicating a lot of the Linux IP subsystem, the IP NS
 * provider does not hook into the Linux packet handler, but hooks at the inet_protocol (transport)
 * level.  The approach differs from 2.4 to 2.6 kernels as follows:
 */

STATIC int np_ip_v4_rcv(struct sk_buff *skb);
STATIC void np_ip_v4_err(struct sk_buff *skb, u32 info);

#ifdef HAVE_KTYPE_STRUCT_NET_PROTOCOL
struct inet_protocol {
	struct net_protocol proto;
	struct net_protocol *next;
	struct module *kmod;
};
#endif				/* HAVE_KTYPE_STRUCT_NET_PROTOCOL */

struct inet_protocol *np_ip_protos[256];

#if defined HAVE_KTYPE_STRUCT_INET_PROTOCOL
/*
 * Under 2.4, simply add the protocol to the network using an inet_protocol structure and the
 * inet_add_protocol() function.  In the handler, if the packet is for us, overwrite
 * skb->nh.iph->protocol with 255 before returning.  If the packet is not for us, free it without
 * alteration.  Simply use inet_del_protocol() to remove the handler.  We need to dynamically
 * allocate handlers.
 */
/**
 * np_ip_v4_steal - steal a socket buffer
 * @skb: socket buffer to steal
 *
 * In the packet handler, if the packet is for us, steal the packet by overwriting the protocol and
 * returning.  This is only done for normal packets and not error packets (which do not need to be
 * stolen).  In the 2.4 handler loop, ihp->protocol is examined on each iteration, permitting us to
 * steal the packet by overwritting the protocol number.
 */
STATIC void
np_ip_v4_steal(struct sk_buff *skb)
{
	skb->nh.iph->protocol = 255;
}
/**
 * np_ip_v4_rcv_next - pass a socket buffer to the next handler
 * @skb: socket buffer to pass
 *
 * In the packet handler, if the packet is not for us, pass it to the next handler by simply freeing
 * the cloned copy and returning.
 */
STATIC int
np_ip_v4_rcv_next(struct sk_buff *skb)
{
	kfree_skb(skb);
	return (0);
}
/**
 * np_ip_v4_err_next - pass a socket buffer to the next error handler
 * @skb: socket buffer to pass
 *
 * In the error packet handler, if the packet is not for us, pass it to the next handler by simply
 * returning.  Error packets are not cloned, so don't free it.
 */
STATIC int
np_ip_v4_err_next(struct sk_buff *skb, __u32 info)
{
	return (0);
}
/**
 * np_ip_init_proto - initialize interception of IP protocol packets
 * @proto: IP protocol number to intercept
 *
 * Under 2.4, simply add the protocol to the network using an inet_protocol structure and the
 * inet_add_protocol() function.
 */
STATIC int
np_ip_init_proto(unsigned char proto)
{
	struct inet_protocol *ip;

	if ((ip = np_ip_protos[proto]) != NULL)
		return (-EALREADY);	/* already initialized */
	if ((ip = np_ip_protos[proto] = kmalloc(sizeof(*ip), GFP_KERNEL)) == NULL)
		return (-ENOMEM);
	bzero(ip, sizeof(*ip));
	ip->protocol = proto;
	ip->name = "streams-np";
	ip->handler = &np_ip_v4_rcv;
	ip->err_handler = &n_ip_v4_err;
	ip->copy = 0;
	ip->next = NULL;
	inet_add_protocol(ip, proto);
	return (0);
}

/**
 * np_ip_init_proto - terminate interception of IP protocol packets
 * @proto: IP protocol number being intercepted
 *
 * Under 2.4, simply remove the protocol from the network using the inet_protocol structure and the
 * inet_del_protocol() function.
 */
STATIC int
np_ip_term_proto(unsigned char proto)
{
	struct inet_protocol *ip;

	if ((ip = np_ip_protos[proto]) == NULL)
		return (-EALREADY);	/* already terminated */
	inet_del_protocol(ip, proto);
	np_ip_protos[proto] = NULL;
	kfree(ip);
	return (0);
}
#elif defined HAVE_KTYPE_STRUCT_NET_PROTOCOL
/*
 * Under 2.6, attempt to use inet_add_protocol().  If it succeeds, handle packets as normal.  Remove
 * with inet_del_protocol().  When inet_add_protocol() fails, lock the list, check whether a module
 * owns the handler functions and, if so, try to increment the module count of each handler.  Then
 * replace the net_protocol structure with our own in the hashes and unlock.  In the packet handler,
 * if the packet is not for us, simply pass it to the next handler.  If the packet is for us, clone
 * it, free the original and work with the clone.
 */
/**
 * np_ip_v4_steal - steal a socket buffer
 * @skb: socket buffer to steal
 *
 * In the packet handler, if the packet is for us, steal the packet by simply not passing it to the
 * next handler.
 */
STATIC void
np_ip_v4_steal(struct sk_buff *skb)
{
}
/**
 * np_ip_v4_rcv_next - pass a socket buffer to the next handler
 * @skb - socket buffer to pass
 *
 * If the packet is not for us, pass it to the next handler.  If there is no next handler, free the
 * packet and return.  Note that we do not have to lock the hash because we own it and are also
 * holding a reference to any module owning the next handler.
 */
STATIC int
np_ip_v4_rcv_next(struct sk_buff *skb)
{
	struct inet_protocol *ip;

	if ((ip = np_ip_protos[skb->nh.iph->protocol]) != NULL && ip->next != NULL)
		return ip->next->proto.handler(skb);
	kfree_skb(skb);
	return (0);
}
/**
 * np_ip_v4_err_next - pass a socket buffer to the next error handler
 * @skb - socket buffer to pass
 *
 * Error packets are not cloned, so pass it to the next handler.  If there is no next handler,
 * simply return.
 */
STATIC int
np_ip_v4_err_next(struct sk_buff *skb, __u32 info)
{
	struct inet_protocol *ip;

	if ((ip = np_ip_protos[skb->nh.iph->protocol]) != NULL && ip->next != NULL)
		ip->next->proto.err_handler(skb, info);
	/* don't free for error handlers */
	return (0);
}
STATIC spinlock_t *inet_proto_lockp = (typeof(inet_proto_lockp)) HAVE_INET_PROTO_LOCK_ADDR;

#ifdef HAVE_MODULE_TEXT_ADDRESS_ADDR
#define module_text_address(__arg) ((typeof(&module_text_address))HAVE_MODULE_TEXT_ADDRESS_ADDR)((__arg))
#endif

/**
 * np_ip_init_proto - initialize interception of IP protocol packets
 * @proto: IP protocol number to intercept
 *
 * Under 2.6, first attempt to use inet_add_protocol().  If it succeeds, handle packets as normal.
 */
STATIC int
np_ip_init_proto(unsigned char proto)
{
	struct inet_protocol *ip;
	int hash = proto & (MAX_INET_PROTOS - 1);

	if ((ip = np_ip_protos[proto]) != NULL)
		return (-EALREADY);	/* already initialized */
	if ((ip = np_ip_protos[proto] = kmalloc(sizeof(*ip), GFP_ATOMIC)) == NULL)
		return (-ENOMEM);
	bzero(ip, sizeof(*ip));
#ifdef HAVE_KMEMB_STRUCT_NET_PROTOCOL_PROTO
	ip->proto.proto = proto;
#endif				/* HAVE_KMEMB_STRUCT_NET_PROTOCOL_PROTO */
	ip->proto.handler = &np_ip_v4_rcv;
	ip->proto.err_handler = &np_ip_v4_err;
	ip->proto.no_policy = 1;
	ip->next = NULL;
	ip->kmod = NULL;
	/* reduces to inet_add_protocol() if no protocol registered */
	spin_lock_bh(inet_proto_lockp);
	if ((ip->next = inet_protosp[hash]) != NULL) {
		if ((ip->kmod = module_text_address(ip->next))
		    && ip->kmod != THIS_MODULE) {
			if (!try_to_get_module(ip->kmod)) {
				spin_unlock_bh(inet_proto_lockp);
				kfree(ip);
				return (-EAGAIN);
			}
		}
	}
	inet_protosp[hash] = &ip->proto;
	spin_unlock_bh(inet_proto_lockp);
	synchronized_net();
	return (0);
}

/**
 * np_ip_init_proto - terminate interception of IP protocol packets
 * @proto: IP protocol number being intercepted
 *
 * Under 2.6, when there is no replacement, simply use inet_del_protocol().
 *
 */
STATIC int
np_ip_term_proto(unsigned char proto)
{
	struct inet_protocol *ip;
	int hash = proto & (MAX_INET_PROTOS - 1);

	if ((ip = np_ip_protos[proto]) == NULL)
		return (-EALREADY);	/* already terminated */
	/* reduces to inet_del_protocol() if no protocol was registered */
	spin_lock_bh(inet_proto_lockp);
	inet_protosp[hash] = ip->next;
	spin_unlock_bh(inet_proto_lockp);
	synchronized_net();
	if (ip->kmod && ip->kmod != THIS_MODULE)
		module_put(ip->kmod);
	np_ip_protos[proto] = NULL;
	kfree(ip);
	return (0);
}
#else
#error One of HAVE_KTYPE_STRUCT_INET_PROTOCOL or HAVE_KTYPE_STRUCT_NET_PROTOCOL must be defined.
#endif

STATIC void streamscall
np_ip_free(char *data)
{
	struct sk_buff *skb = (struct sk_buff *)data;
	kfree_skb(skb);
	return;
}

/**
 * np_ip_v4_rcv - handle an IPv4 received packet
 * @skb: socket buffer containing packet
 */
STATIC int
np_ip_v4_rcv(struct sk_buff *skb)
{
	struct np *np;
	struct iphdr *iph;
	struct udphdr *uh;		/* just for lookups */

	if (unlikely(!pskb_may_pull(skb, sizeof(struct updhdr))))
		goto too_small;
	if (unlikely(skb->pkt_type != PACKET_HOST))
		goto bad_pkt_type;
	rt = (struct rtable *) skb->dst;
	if (rt->rt_flags & (RTCF_BROADCAST | RTCF_MULTICAST))
		/* need to do something about broadcast and multicast */ ;
	/* we do the lookup before the checksum */
	iph = skb->nh.iph;
	uh = skb->h.uh;
	read_lock(&np_ip_lock);
	np = np_ip_lookup(iph->protocol, uh->dest, uh->source, iph->daddr, iph->saddr);
	if (unlikely(np == NULL))
		goto no_stream;
	np_ip_v4_steal(skb);	/* its ours now */
	if (unlikely(np->rq == NULL || !canput(np->rq)))
		goto flow_controlled;
	if (skb_is_nonlinear(skb) && unlikely(skb_linearize(skb, GFP_ATOMIC) != 0))
		goto linear_fail;
	{
		mblk_t *mp, *dp;
		size_t mlen = skb->len + (skb->data - skb->nh.raw);
		frtn_t fr = { &np_ip_free, (char *) skb };

		/* Create and queue a N_UNITDATA_IND message to the Stream's read queue for further 
		   (immediate or deferred) processing.  The Stream will convert this message into
		   an N_DATA_IND or N_UNITDATA_IND message and pass it along. */
		if (np->info.SERV_type == N_CLNS) {
			N_unitdata_ind_t *p;
			struct sockaddr_in *sin;

			mp = allocb(sizeof(*p) + (sizeof(*sin) << 1), BPRI_MED);
			if (unlikely(mp == NULL))
				goto no_buffers;
			dp = esballoc(skb->nh.raw, mlen, BPRI_MED, &fr);
			if (unlikely(dp == NULL))
				goto no_blocks;
			dp->b_wptr = dp->b_rptr + mlen;
			/* trim the ip header */
			dp->b_rptr = skb->h.raw;
			/* leave the udp-like header */
			skb->dev = NULL;
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UNITDATA_IND;
			p->DEST_length = sizeof(*sin);
			p->DEST_offset = sizeof(*p);
			p->SRC_length = sizeof(*sin);
			p->SRC_offset = sizeof(*p) + sizeof(*sin);
			p->ERROR_type = 0;
			mp->b_wptr += sizeof(*p);
			sin = (typeof(sin)) mp->b_wptr;
			sin->sin_family = AF_INET;
			sin->sin_port = uh->dest;
			sin->sin_addr.s_addr = iph->daddr;
			mp->b_wptr += sizeof(*sin);
			sin = (typeof(sin)) mp->b_wptr;
			sin->sin_family = AF_INET;
			sin->sin_port = uh->source;
			sin->sin_addr.s_addr = iph->saddr;
			mp->b_wptr += sizeof(*sin);
			/* strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_UNITDATA_IND"); */
		} else {
			N_data_ind_t *p;

			mp = allocb(sizeof(*p), BPRI_MED);
			if (unlikely(mp == NULL))
				goto no_buffers;
			dp = esballoc(skb->nh.raw, mlen, BPRI_MED, &fr);
			if (unlikely(dp == NULL))
				goto no_blocks;
			dp->b_wptr = dp->b_rptr + mlen;
			/* trim the ip header */
			dp->b_rptr = skb->h.raw;
			/* leave the udp-like header */
			skb->dev = NULL;
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_DATA_IND;
			p->DATA_xfer_flags = 0;
			mp->b_wptr += sizeof(*p);
			/* strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_DATA_IND"); */
		}
		mp->b_cont = dp;
		put(np->rq, mp);
		np_release(&np);
		read_unlock(&np_ip_lock);
		return (0);
	      no_blocks:
		freeb(mp);
	}
      no_buffers:
      linear_fail:
      flow_controlled:
	np_release(&np);
	read_unlock(&np_ip_lock);
	kfree_skb(skb);
	return (0);
      no_stream:
      bad_pkt_type:
      too_small:
	return np_ip_v4_rcv_next(skb);
}
/**
 * np_ip_v4_err - handle an ICMP received packet
 * @skb: socket buffer containing packet with headers pulled
 * @info: icmp error (unused)
 *
 * This function is a network protocol callback that is invoked when transport specific ICMP errors
 * are received.  The function looks up the Stream and, if found, wrap the packet in an M_ERROR
 * message and passes it to the read queue of the Stream.
 *
 * LOCKING: np_ip_lock protects the master list and protects from open, close, link and unlink.
 * np->qlock protects the state of the private structure.  np->refs protects the private structure
 * from being deallocated before locking.
 */
STATIC void
np_ip_v4_err(struct sk_buff *skb, __u32 info)
{
	struct np *np;
	struct udphdr *uh;
	struct iphdr *iph = (struct iphdr *) skb->data;
	size_t ihl;

#define ICMP_MIN_LENGTH 8
	if (skb->len < (ihl = iph->ihl << 2) + ICMP_MIN_LENGTH)
		goto drop;
	uh = (struct udphdr *) (skb->data + ihl);
	read_lock(&np_ip_lock);
	np = np_lookup(iph->protocol, uh->source, uh->dest, iph->saddr, iph->daddr);
	if (np == NULL)
		goto no_stream;
	if (np_get_state(np) == NS_UNBND)
		goto closed;
	if (unlikely(np->rq == NULL || !canput(np->rq)))
		goto flow_controlled;
	{
		mblk_t *mp;

		/* Create and queue a N_UDERROR_IND message to the Stream's read queue for further
		   (immediate or deferred) processing.  The Stream will convert this message into a 
		   N_RESET_IND or N_UDERROR_IND message and pass it along. */
		if (np->SERV_type == N_CLNS) {
			N_uderror_ind_t *p;
			struct sockaddr_in *sin;

			mp = allocb(sizeof(*p) + sizeof(*sin), BPRI_MED);
			if (unlikely(mp == NULL))
				goto no_buffers;
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_UDERROR_IND;
			p->DEST_length = sizeof(*sin);
			p->DEST_offset = sizeof(*p);
			p->ERROR_type = FIXME;
			mp->b_wptr += sizeof(*p);
			sin = (typeof(sin)) mp->b_wptr;
			sin->sin_family = AF_INET;
			sin->sin_port = uh->dest;
			sin->sin_addr.s_addr = iph->daddr;
			mp->b_wptr += sizeof(*sin);
			/* strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_UDERROR_IND"); */
		} else {
			N_reset_ind_t *p;

			mp = allocb(sizeof(*p), BPRI_MED);
			if (unlikely(mp == NULL))
				goto no_buffers;
			mp->b_datap->db_type = M_PROTO;
			p = (typeof(p)) mp->b_wptr;
			p->PRIM_type = N_RESET_IND;
			p->RESET_orig = N_PROVIDER;
			p->RESET_reason = FIXME;
			mp->b_wptr += sizeof(*p);
			np_set_state(np, NS_WRES_RIND);
			/* strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_RESET_IND"); */
		}
		put(np->rq, mp);
		np_release(&np);
		read_unlock(&np_ip_lock);
	      done:
		/* always pass the original to the next handler, if any */
		return np_ip_v4_err_next(skb, info);
	}
      no_buffers:
      no_clone:
      flow_controlled:
      closed:
      no_stream:
      drop:
	goto done;
}

STATIC unsigned char np_ip_8022_type = 0xCC;
STATIC unsigned char np_ip_snap_id[5] = { 0x00, 0x00, 0x00, 0x80, 0x00 };

/**
 * np_ip_pkt_rcv - receive packets
 */
STATIC int
np_ip_pkt_rcv(struct sk_buff *skb, struct net_device *dev, struct packet_type *pt,
	      struct net_device *orig_dev)
{
}

struct packet_type np_ip_dix_pkt_type = {
	.type = __constant_htons(ETH_P_IP),
	.func = &np_ip_pkt_rcv,
	.data = (void *) 1,		/* understand shared sk_buffs */
};

struct packet_type np_ip_8023_pkt_type = {
	.type = __constant_htons(ETH_P_802_3),
	.func = &np_ip_pkt_rcv,
	.data = (void *) 1,		/* understand shared sk_buffs */
};
STATIC void
np_ip_e2_datalink_header(struct datalink_proto *dl, struct sk_buff *skb, unsigned char *dest_node)
{
	struct net_device *dev;

	dev = skb->dev;
	skb->protocol = htons(ETH_P_IP);
	if (dev->hard_header)
		dev->hard_header(skb, dev, ETH_P_IP, dest_node, NULL, skb->len);
	return;
}
STATIC struct datalink_proto np_ip_e2_datalink_struct = {
	.type_len = 0,
	.string_name = "EthernetII",
	.header_len = 0,
	.rcvfunc = &np_ip_pkt_rcv,
	.datalink_header = &np_ip_e2_datalink_header,
}, *np_ip_e2_datalink = &np_ip_e2_datalink_struct;
STATIC void
np_ip_8023_datalink_header(struct datalink_proto *dl, struct sk_buff *skb, unsigned char *dest_node)
{
	struct net_device *dev;

	dev = skb->dev;
	skb->protocol = htons(ETH_P_IP);
	if (dev->hard_header)
		dev->hard_header(skb, dev, ETH_P_802_3, dest_node, NULL, skb->len);
	return;
}
STATIC struct datalink_proto np_ip_8023_datalink = {
	.type_len = 0,
	.string_name = "802.3",
	.header_length = 0,
	.rcvfunc = &np_ip_pkt_rcv,
	.datalink_header = &np_ip_8023_datalink_header,
}, *np_ip_8023_datalink = &np_ip_8023_datalink_struct;

STATIC struct datalink_proto *np_ip_8022_datalink;
STATIC struct datalink_proto *np_ip_snap_datalink;

/**
 * np_ip_init - initialize the IP NS provider
 */
STATIC int
np_ip_init(void)
{
	dev_add_pack(np_ip_dix_pkt_type);
	dev_add_pack(np_ip_8023_pkt_type);
	np_ip_8022_datalink = register_8022_client(np_ip_8022_type, np_ip_rcv);
	np_ip_snap_datalink = register_snap_client(np_ip_snap_id, np_ip_rcv);
}

/**
 * np_ip_term - terminate the IP NS provider
 */
STATIC void
np_ip_term(void)
{
	unregister_snap_client(np_ip_snap_id);
	unregister_8022_client(np_ip_8022_type);
	dev_remove_pack(&np_ip_8023_pkt_type);
	dev_remove_pack(&np_ip_dix_pkt_type);
}

/* Actions */

/**
 * np_ip_unbind - remove IP NS provider from bind hashes
 * @np: IP NS provider stream private structure
 */
STATIC void
np_ip_unbind(struct np *np)
{
	struct np_proto_bind **bpp = &np->bind, *bp;

	while ((bp = *bpp)) {
		if ((*bpp = bp->np_next))
			bp->np_next->np_prev = bpp;
		bp->np_next = NULL;
		bp->np_prev = &bp->np_next;
		np_ip_bind_hash_del(bp);
		bp->np = NULL;
		bp->protoid = 0;
		bp->loc_nselector = 0;
		bp->rem_nselector = 0;
		kmem_cache_free(np_proto_cachep, bp);
	}
	np->ADDR_length = 0;
	np->PROTOID_length = 0;
	np->CONIND_number = 0;
	np->BIND_flags = 0;
	return;
}

/**
 * np_ip_bind - add IP NS provider to bind hashes
 * @np: IP NS provider stream private structure
 */
STATIC int
np_ip_bind(struct np *np)
{
	int i;
	struct np_prot **bpp = &np->bind;

	for (i = 0; i < np->PROTOID_length; i++) {
		struct np_prot *bp;

		if (likely((bp = kmem_cache_alloc(np_proto_cachep, SLAB_ATOMIC)) != NULL)) {
			bzero(bp, sizeof(*bp));
			bp->np = np;
			bp->protoid = np->PROTOID_buffer[i];
			bp->loc_nselector = ((struct sockaddr_in *) np->ADDR_buffer)->sin_port;
			bp->rem_nselector = 0;
			if (np_ip_bind_hash_add(bp) == 0) {
				/* link it into our bind list */
				if ((bp->np_next = *bpp))
					bp->np_next->np_prev = &bp->np_next;
				bp->np_prev = bpp;
				*bpp = bp;
				continue;
			}
		}
		/* need to unbind them all */
		np_ip_unbind(np);
		return (-EAGAIN);
	}
	return (0);
}

/**
 * np_ip_discon - remove IP NS provider from connection hashes
 * @np: IP NS provider stream private structure
 */
STATIC void
np_ip_discon(struct np *np)
{
	struct np_prot **bpp = &np->conn, *bp;

	while ((bp = *bpp)) {
		if ((*bpp = bp->np_next))
			bp->np_next->np_prev = bpp;
		bp->np_next = NULL;
		bp->np_prev = &bp->np_next;
		np_ip_conn_hash_del(bp);
		bp->np = NULL;
		bp->protoid = 0;
		bp->loc_nselector = 0;
		bp->rem_nselector = 0;
		kmem_cache_free(np_proto_cachep, bp);
	}
	np->DEST_length = 0;
	np->CONN_flags = 0;
	return;
}

/**
 * np_ip_conn - add IP NS provider to connection hashes
 * @np: IP NS provider stream private structure
 */
STATIC int
np_ip_conn(struct np *np)
{
	int i;
	struct np_prot **bpp = &np->conn;

	for (i = 0; i < np->PROTOID_length; i++) {
		struct np_prot *bp;

		if (likely((bp = kmem_cache_alloc(np_proto_cachep, SLAB_ATOMIC)) != NULL)) {
			bzero(bp, sizeof(*bp));
			bp->np = np;
			bp->protoid = np->PROTOID_buffer[i];
			bp->loc_nselector = ((struct sockaddr_in *) np->ADDR_buffer)->sin_port;
			bp->rem_nselector = ((struct sockaddr_in *) np->DEST_buffer)->sin_port;
			if (np_ip_conn_bash_add(bp) == 0) {
				/* link it into our conn list */
				if ((bp->np_next = *bpp))
					bp->np_next->np_prev = &bp->np_next;
				bp->np_prev = bpp;
				*bpp = bp;
				continue;
			}
		}
		/* need to disconnect them all */
		np_ip_discon(np);
		return (-EAGAIN);
	}
	return (0);
}

/**
 * ne_ip_bind_req - process an IP bind event
 * @np: NS provider stream private structure
 * @ep: bind request event
 *
 * Protocol IDs consist of zero or more IP protocol numbers.  If protocol number 255 is used, all
 * protocols are intercepted.  If no protocol ids are supplied, it is assumed that UDP is requested
 * (protocol number 17).
 *
 * Addresses consist of a sockaddr structure.  The sa_family is AF_INET, followed by a 16-bit
 * network byte order port number and one or more 32-bit network byte order IPv4 addresses.  The
 * port number can be zero (0), indicating that any port will be used.  The first (and only) IPv4
 * address can be 0.0.0.0 indicating any IP address.  If no address is provided, any port and any
 * address is indicated.
 *
 * Note that when the first IP NS provider binds, the IP network tap will be installed.
 */
STATIC int
ne_ip_bind_req(struct np *np, struct ne_bind_req *ep)
{
	np_long NPI_error;
	struct sockaddr_in *sin = (struct sockaddr_in *) np->ADDR_buffer;

	np->BIND_flags = ep->BIND_flags;
	if (ep->ADDR_length == 0) {
		sin->sin_family = AF_INET;
		sin->sin_port = 0;
		sin->sin_addr.s_addr = 0;
		np->ADDR_length = sizeof(*sin);
		ep->ADDR_buffer = np->ADDR_buffer;
		ep->ADDR_length = np->ADDR_length;
	} else {
		NPI_error = NBADADDR;
		if (ep->ADDR_length < sizeof(struct sockaddr_in *))
			goto error;
		if (ep->ADDR_length > sizeof(np->ADDR_buffer))
			goto error;
		if (ep->ADDR_length / sizeof(uint32_t) < 2)
			goto error;
		if (ep->ADDR_length % sizeof(uint32_t) != 0)
			goto error;
		bcopy(ep->ADDR_buffer, np->ADDR_buffer, ep->ADDR_length);
		np->ADDR_length = ep->ADDR_length;
	}
	NPI_error = NBADADDR;
	if (sin->sin_port == 0 & ~(np->BIND_flags & (DEFAULT_LISTENER|TOKEN_REQUEST)))
		goto error;
	if (ep->PROTOID_length == 0) {
		uchar *proto = (uchar *) np->PROTOID_buffer;

		proto[0] = IPPROTO_UDP;
		np->PROTOID_length = sizeof(*proto);
		ep->PROTOID_buffer = np->PROTOID_buffer;
		ep->PROTOID_length = np->PROTOID_length;
	} else {
		NPI_error = NBADADDR;
		if (ep->PROTOID_length < sizeof(uchar))
			goto error;
		if (ep->PROTOID_length > sizeof(np->PROTOID_buffer))
			goto error;
		bcopy(ep->PROTOID_buffer, np->PROTOID_buffer, ep->PROTOID_length);
		np->PROTOID_length = ep->PROTOID_length;
	}
	np->info.SERV_type =
	    (ep->BIND_flags & (DEFAULT_LISTENER | TOKEN_REQUEST)) ? N_CONS : N_CLNS;
	/* now the address and protocol ids are in the np structure, do the bind */
	NPI_error = np_ip_bind(np);
      error:
	return (NPI_error);
}

/**
 * ne_ip_unbind_req - process an IP unbind event
 * @np: NS provider stream private structure
 * @ep: unbind request event
 *
 * Unbinding simply consists of removal from the bind hashes and zeroing of the appropriate address
 * and protocol id lengths.  Note that when the last IP NS provider unbinds, the IP network tap will
 * be removed.
 */
STATIC int
ne_ip_unbind_req(struct np *np, struct ne_unbind_req *ep)
{
	np_ip_unbind(np);
	return (0);
}
STATIC int
ne_ip_optmgmt_req(struct np *np, struct ne_optmgmt_req *ep)
{
}
STATIC int
ne_ip_conn_req(struct np *np, struct ne_conn_req *ep)
{
	np_long NPI_error;
	N_conn_res_t *p;

	NPI_error = NNOADDR;
	if (ep->DEST_length == 0)
		goto error;
	NPI_error = NBADADDR;
	if (ep->DEST_length < sizeof(struct sockaddr_in *))
		goto error;
	if (ep->DEST_length > sizeof(np->ADDR_buffer))
		goto error;
	if (ep->DEST_length / sizeof(uint32_t) < 2)
		goto error;
	if (ep->DEST_length % sizeof(uint32_t) != 0)
		goto error;
	bcopy(ep->DEST_buffer, np->DEST_buffer, ep->DEST_length);
	np->DEST_length = ep->DEST_length;
	NPI_error = -ENOBUFS;
	if ((mp = ss7_allocb(np->iq, sizeof(*p) + np->DEST_length + np->QOS_length))) {
		/* for each of the IP protocol numbers, we need to create a bind */
		if (unlikely((NPI_error = np_ip_conn(np)) != 0)) {
			freeb(mp);
			goto error;
		}
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p))mp->b_wptr;
		p->PRIM_type = N_CONN_CON;
		p->RES_length = np->DEST_length;
		p->RES_offset = np->DEST_length ? sizeof(*p) : 0;
		p->CONN_flags = ep->CONN_flags;
		p->QOS_length = np->QOS_length;
		p->QOS_offset = np->QOS_length ? sizeof(*p) + np->DEST_length : 0;
		mp->b_wptr += sizeof(*p);
		if (np->DEST_length) {
			bcopy(np->DEST_buffer, mp->b_wptr, np->DEST_length);
			mp->b_wptr += np->DEST_length;
		}
		if (np->QOS_length) {
			bcopy(np->QOS_buffer, mp->b_wptr, np->QOS_length);
			mp->b_wptr += np->QOS_length;
		}
		np_set_state(np, NS_DATA_XFER);
		strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_CONN_CON");
		putnext(np->oq, mp);
		return (0);
	}
      error:
	return (NPI_error);
}

/*
 *  A slight variation on NPI: we use the responding address as the connected address instead.  This
 *  permits support for multihoming.
 */
STATIC int
ne_ip_conn_res(struct np *np, struct ne_conn_res *ep)
{
	np_long NPI_error;
	struct np *ap = ep->TOKEN_value;

	if (ep->RES_length == 0) {
		/* Default to whatever address was in the connection indication */
	} else {
		NPI_error = NBADADDR;
		if (ep->RES_length < sizeof(struct sockaddr_in *))
			goto error;
		if (ep->RES_length > sizeof(np->DEST_buffer))
			goto error;
		if (ep->RES_length / sizeof(uint32_t) < 2)
			goto error;
		if (ep->RES_length % sizeof(uint32_t) != 0)
			goto error;
		bcopy(ep->RES_buffer, ap->DEST_buffer, ep->RES_length);
		ap->DEST_length = ep->RES_length;
	}
	/* for each of the IP protocol numbers, we need to create a bind */
	NPI_error = np_ip_conn(ap);
      error:
	return (NPI_error);
}
STATIC int
ne_ip_discon_req(struct np *np, struct ne_discon_req *ep)
{
	np_ip_discon(ap);
	return (0);
}
STATIC int
ne_ip_data_req(struct np *np, struct ne_data_req *ep)
{
}
STATIC int
ne_ip_exdata_req(struct np *np, struct ne_exdata_req *ep)
{
	return (NNOTSUPPORT);
}
STATIC int
ne_ip_reset_req(struct np *np, struct ne_reset_req *ep)
{
	return (NNOTSUPPORT);
}
STATIC int
ne_ip_reset_res(struct np *np, struct ne_reset_res *ep)
{
	return (NNOTSUPPORT);
}
STATIC int
ne_ip_datack_req(struct np *np, struct ne_datack_req *ep)
{
	return (NNOTSUPPORT);	/* would need LLC Type 3 */
}
STATIC int
ne_ip_unitdata_req(struct np *np, struct ne_unitdata_req *ep)
{
}

STATIC int streamscall
np_ip_event(queue_t *q, struct np *np, union ne_event *ep)
{
	switch (ep->EVENT) {
	/* *INDENT-OFF* */
	case NE_BIND_REQ:	return ne_ip_bind_req	 (np, &ep->bind_req);
	case NE_UNBIND_REQ:	return ne_ip_unbind_req	 (np, &ep->unbind_req);
	case NE_OPTMGMT_REQ:	return ne_ip_optmgmt_req (np, &ep->optmgmt_req);
	case NE_CONN_REQ:	return ne_ip_conn_req	 (np, &ep->conn_req);
	case NE_CONN_RES:	return ne_ip_conn_res	 (np, &ep->conn_res);
	case NE_DISCON_REQ:	return ne_ip_discon_req	 (np, &ep->discon_req);
	case NE_DATA_REQ:	return ne_ip_data_req	 (np, &ep->data_req);
	case NE_EXDATA_REQ:	return ne_ip_exdata_req	 (np, &ep->exdata_req);
	case NE_RESET_REQ:	return ne_ip_reset_req	 (np, &ep->reset_req);
	case NE_RESET_RES:	return ne_ip_reset_res	 (np, &ep->reset_res);
	case NE_DATACK_REQ:	return ne_ip_datack_req	 (np, &ep->datack_req);
	case NE_UNITDATA_REQ:	return ne_ip_unitdata_req(np, &ep->unitdata_req);

	case NE_CONN_IND:	return ne_conn_ind	 (np, &ep->conn_ind);
	case NE_CONN_CON:	return ne_conn_con	 (np, &ep->conn_con);
	case NE_DATA_IND:	return ne_data_ind	 (np, &ep->data_ind);
	case NE_EXDATA_IND:	return ne_exdata_ind	 (np, &ep->exdata_ind);
	case NE_DISCON_IND1:	return ne_discon_ind	 (np, &ep->discon_ind);
	case NE_DISCON_IND2:	return ne_discon_ind	 (np, &ep->discon_ind);
	case NE_DISCON_IND3:	return ne_discon_ind	 (np, &ep->discon_ind);
	case NE_DATACK_IND:	return ne_datack_ind	 (np, &ep->datack_ind);
	case NE_RESET_IND:	return ne_reset_ind	 (np, &ep->reset_ind);
	case NE_RESET_CON:	return ne_reset_con	 (np, &ep->reset_con);
	case NE_UNITDATA_IND:	return ne_unitdata_ind	 (np, &ep->unitdata_ind);
	case NE_UDERROR_IND:	return ne_uderror_ind	 (np, &ep->uderror_ind);
	/* *INDENT-ON* */
	case NE_BIND_ACK:
	case NE_ERROR_ACK:
	case NE_OK_ACK1:
	case NE_OK_ACK2:
	case NE_OK_ACK3:
	case NE_OK_ACK4:
	case NE_PASS_CON:
	default:
		swerr();
		return (NNOTSUPPORT);
	}
}

/*
 * NP_X25
 * ========
 */

/* Information */

STATIC N_info_ack_t np_x25_info = {
	.PRIM_type = N_INFO_ACK,
	.NSDU_size = 0x0000ffff,
	.ENSDU_size = 128,
	.CDATA_size = QOS_UNKNOWN,
	.DDATA_size = QOS_UNKNOWN,
	.ADDR_size = QOS_UNKNOWN,
	.OPTIONS_flags = 0,
	.NIDU_size = 128,
	.SERV_type = N_CONS,
	.CURRENT_state = NS_UNBND,
	.PROVIDER_type = N_SUBNET,
	.NODU_size = 128,
	.NPI_version = N_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
np_x25_event(queue_t *q, struct np *np, union ne_event *ep)
{
	return (NNOTSUPPORT);
}

/*
 * NP_CLNL
 * ========
 */

/* Information */

STATIC N_info_ack_t np_clnl_info = {
	.PRIM_type = N_INFO_ACK,
	.NSDU_size = 0x0000ffff,
	.ENSDU_size = 128,
	.CDATA_size = QOS_UNKNOWN,
	.DDATA_size = QOS_UNKNOWN,
	.ADDR_size = QOS_UNKNOWN,
	.OPTIONS_flags = 0,
	.NIDU_size = 128,
	.SERV_type = N_CONS,
	.CURRENT_state = NS_UNBND,
	.PROVIDER_type = N_SUBNET,
	.NODU_size = 128,
	.NPI_version = N_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
np_clnl_event(queue_t *q, struct np *np, union ne_event *ep)
{
	return (NNOTSUPPORT);
}

/*
 * NP_CONS
 * ========
 */

/* Information */

STATIC N_info_ack_t np_cons_info = {
	.PRIM_type = N_INFO_ACK,
	.NSDU_size = 0x0000ffff,
	.ENSDU_size = 128,
	.CDATA_size = QOS_UNKNOWN,
	.DDATA_size = QOS_UNKNOWN,
	.ADDR_size = QOS_UNKNOWN,
	.OPTIONS_flags = 0,
	.NIDU_size = 128,
	.SERV_type = N_CONS,
	.CURRENT_state = NS_UNBND,
	.PROVIDER_type = N_SUBNET,
	.NODU_size = 128,
	.NPI_version = N_CURRENT_VERSION,
};

/* Actions */

STATIC int streamscall
np_cons_event(queue_t *q, struct np *np, union ne_event *ep)
{
	return (NNOTSUPPORT);
}

/*
 *  NS USER PRIMITIVES SENT DOWNSTREAM
 *  ==================================
 */

STATIC INLINE int
n_conn_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	struct ne_conn_req ne = { NE_CONN_REQ, };
	int err;

	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + sizeof(p->conn_req))
		goto error;
	err = -EFAULT;
	if (p->type != N_CONN_REQ)
		goto error;
	err = NNOTSUPPORT;
	if (!(np->np_info.SERV_type & N_CONS))
		goto error;
	err = NOUTSTATE;
	if (np_get_state(np) != NS_IDLE)
		goto error;
	err = NBADADDR;
	if (mp->b_wptr < (ne.DEST_buffer = mp->b_rptr + p->DEST_offset)
	    + (ne.DEST_length = p->DEST_length))
		goto error;
	err = NBADOPT;
	if (mp->b_wptr < (ne.QOS_buffer = mp->b_rptr + p->QOS_offset)
	    + (ne.QOS_length = p->QOS_length))
		goto error;
	err = NNOADDR;
	if (ne.DEST_length == 0)
		goto error;
	err = NBADDATA;
	if ((ne.DATA_blocks = mp->b_cont)
	    && (ne.DATA_length = msgsize(ne.DATA_blocks)) > (np_long) np->np_info.CDATA_size)
		goto error;
	err = NBADFLAG;
	if ((ne.CONN_flags = p->CONN_flags) & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto error;
	np_set_state(np, NS_WCON_CREQ);
	if ((err = np->np_event(q, np, &ne)))
		goto error;
	return (QR_TRIMMED);
      error:
	return n_reply_ack(np, N_CONN_REQ, err, mp);
}

/**
 * n_conn_res - accept a specified NS connection indication on the requested stream
 * @q: write queue
 * @mp: N_CONN_RES message
 *
 * There is a slight variation on normal NPI Revision 2.0.0 semantics here: the Responding Address
 * is, here, the remote address(es) to which the connection is to be formed, rather than the
 * local address(es) with which the connection is established.  This is only necessary, however, for
 * multihomed operation, where the NS user is the only one aware of the other remote addresses from
 * the message.
 */
STATIC INLINE int
n_conn_res(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	struct ne_conn_res ne = { NE_CONN_RES, };
	int err;

	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + sizeof(p->conn_res))
		goto error;
	err = -EFAULT;
	if (p->type != N_CONN_RES)
		goto error;
	err = NNOTSUPPORT;
	if (!(np->np_info.SERV_type & N_CONS))
		goto error;
	err = NOUTSTATE;
	if (np_get_state(np) != NS_WRES_CIND)
		goto error;
	err = NBADADDR;
	if (mp->b_wptr < (ne.RES_buffer = mp->b_rptr + p->conn_res.RES_offset)
	    + (ne.RES_length = p->conn_res.RES_length))
		goto error;
	err = NBADOPT;
	if (mp->b_wptr < (ne.QOS_buffer = mp->b_rptr + p->conn_res.QOS_offset)
	    + (ne.QOS_length = p->conn_res.QOS_length))
		goto error;
	err = NBADDATA;
	if ((ne.DATA_blocks = mp->b_cont)
	    && (ne.DATA_length = msgsize(ne.DATA_blocks)) > np->np_info.CDATA_size)
		goto error;
	err = NBADTOKEN;
	if (p->conn_res.TOKEN_value && !(ne.TOKEN_value = np_find_token(p->conn_res.TOKEN_value)))
		goto error;
	if (ne.TOKEN_value == NULL)
		ne.TOKEN_value = np;
	else {
		/* FIXME: need to lock the accepting stream or fail -EAGAIN */
		/* perhaps these should just be NBADTOKEN */
		err = NNOTSUPPORT;	/* must be of the same service type */
		if (ne.TOKEN_value->SERV_type != N_CONS)
			goto error;
		err = NOUTSTATE;	/* must be bound and not connected */
		if (np_get_statef(ne.TOKEN_value) & (NS_UNBND | NS_WACK_BREQ | NS_WACK_UREQ))
			goto error;	/* note we could attempt to autobind instead */
		if (np_get_state(ne.TOKEN_value) != NS_IDLE)
			goto error;
	}
	err = NBADSEQ;
	if (!(ne.SEQ_number = np_find_sequence(p->conn_res.SEQ_number)))
		goto error;
	err = NBADFLAG;
	if ((ne.CONN_flags = p->conn_res.CONN_flags) & ~(REC_CONF_OPT | EX_DATA_OPT))
		goto error;
	if ((err = np->np_event(q, np, &ne)))
		goto error;
	bufq_dequeue(&np->np_conq, ne.SEQ_number);
	freemsg(ne.SEQ_number);
	if (np != ne.TOKEN_value) {
		np_set_state(ne.TOKEN_value, NS_DATA_XFER);
		/* FIXME: need to unlock the accepting stream */
		if (bufq_length(&np->np_conq) > 0)
			np_set_state(np, NS_WRES_CIND);
		else
			np_set_state(np, NS_IDLE);
	} else
		np_set_state(np, NS_DATA_XFER);
      error:
	return n_reply_ack(np, N_CONN_RES, err, mp);
}
STATIC INLINE int
n_discon_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	struct ne_discon_req ne = { NE_DISCON_REQ, };
	int err;

	err = NNOTSUPPORT;
	if (!(np->np_info.SERV_type & N_CONS))
		goto error;
	err = NOUTSTATE;
	if (!(np_get_statef(np)
	      & (NSF_WCON_CREQ | NSF_WRES_CIND | NSF_DATA_XFER | NSF_WCON_RREQ | NSF_WRES_RIND)))
		goto error;
	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + sizeof(p->discon_req))
		goto error;
	err = NBADADDR;
	if (mp->b_wptr < (ne.RES_buffer = mp->b_rptr + p->discon_req.RES_offset)
	    + (ne.RES_length = p->discon_req.RES_length))
		goto error;
	err = NBADSEQ;
	if (!(ne.SEQ_number = np_find_sequence(p->discon_req.SEQ_number)))
		goto error;
	ne.DISCON_reason = p->discon_req.DISCON_reason;
	if ((err = np->np_event(q, np, &ne)))
		goto error;
	if (ne.SEQ_number != NULL) {
		bufq_dequeue(&np->np_conq, ne.SEQ_number);
		freemsg(ne.SEQ_number);
		if (bufq_length(&np->np_conq) > 0)
			np_set_state(np, NS_WRES_CIND);
		else
			np_set_state(np, NS_IDLE);
	} else
		np_set_state(np, NS_IDLE);
      error:
	return n_reply_ack(np, N_DISCON_REQ, err, mp);
}
STATIC INLINE fastcall __hot_put int
n_data_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	struct ne_data_req ne = { NE_DATA_REQ, };
	dl_ulong statef;
	int err;

	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + sizeof(p->data_req))
		goto error;
	err = -EFAULT;
	if (p->type != N_DATA_REQ)
		goto error;
	err = NNOTSUPPORT;
	if (!(np->np_info.SERV_type & N_CONS))
		goto error;
	err = 0;
	if ((state = np_get_statef(np)) & (NSF_IDLE | NSF_WRES_RIND))
		goto error;
	err = NOUTSTATE;
	if (!(statef & NSF_DATA_XFER))
		goto error;
	err = NBADDATA;
	if (!(ne.DATA_blocks = mp->b_cont)
	    && (ne.DATA_length = msgsize(ne.DATA_blocks)) > np->np_info.NSDU_size)
		goto error;
	if (ne.DATA_length != 0 && ne.DATA_length > np->np_info.NIDU_size)
		goto error;
	err = NBADFLAG;
	if ((ne.DATA_xfer_flags = p->DATA_xfer_flags) & ~(N_MORE_DATA_FLAG | N_RC_FLAG))
		goto error;
	err = NBADFLAG;
	if ((ne.DATA_xfer_flags & (N_MORE_DATA_FLAG | N_RC_FLAG)) = (N_MORE_DATA_FLAG | N_RC_FLAG))
		goto error;
	if (err = np->np_event(q, np, &ne))
		goto error;
	return (QR_TRIMMED);
      error:
	return n_reply_err(np, N_DATA_REQ, err, mp);
}
STATIC INLINE fastcall __hot_put int
n_exdata_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	struct ne_exdata_req ne = { NE_EXDATA_REQ, };
	np_ulong statef;
	int err;

	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + sizeof(p->exdata_req))
		goto error;
	err = -EFAULT;
	if (p->type != N_EXDATA_REQ)
		goto error;
	err = NNOTSUPPORT;
	if (!(np->np_info.SERV_type & N_CONS))
		goto error;
	err = 0;
	if ((statef = np_get_statef(np)) & (NSF_IDLE | NSF_WRES_RIND))
		goto error;
	err = NOUTSTATE;
	if (!(statef & NSF_DATA_XFER))
		goto error;
	err = NBADDATA;
	if (!(ne.DATA_blocks = mp->b_cont)
	    && (ne.DATA_length = msgsize(ne.DATA_blocks)) > np->np_info.ENSDU_size)
		goto error;
	if (ne.DATA_length != 0 && ne.DATA_length > np->np_info.NIDU_size)
		goto error;
	if (err = np->np_event(q, np, &ne))
		goto error;
	return (QR_TRIMMED);
      error:
	return n_reply_err(np, N_EXDATA_REQ, err, mp);
}
STATIC INLINE int
n_info_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	int err;
	size_t size =
	    sizeof(p->info_ack) + np->ADDR_length + np->QOS_length + np->QOS_range_length +
	    np->PROTOID_length;

	err = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->info_req)))
		goto error;
	err = -EFAULT;
	if (unlikely(p->type != N_INFO_REQ))
		goto error;
	err = -ENOBUFS;
	if (!(mp = ss7_allocb(q, size, BPRI_MED)))
		goto error;
	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->b_rptr;
	mp->b_wptr += size;
	p->info_ack = np->np_info;
	p->info_ack.PRIM_type = N_INFO_ACK;
	p->info_ack.ADDR_length = np->ADDR_length;
	p->info_ack.ADDR_offset = np->ADDR_length ? sizeof(p->info_ack) : 0;
	p->info_ack.QOS_length = np->QOS_length;
	p->info_ack.QOS_offset = np->QOS_length ? sizeof(p->info_ack) + np->ADDR_length : 0;
	p->info_ack.QOS_range_length = np->QOS_range_length;
	p->info_ack.QOS_range_offset =
	    np->QOS_range_length ? sizeof(p->info_ack) + np->ADDR_length + np->QOS_length : 0;
	p->info_ack.PROTOID_length = np->PROTOID_length;
	p->info_ack.PROTOID_offset =
	    np->PROTOID_length ? sizeof(p->info_ack) + np->ADDR_length + np->QOS_length +
	    np->QOS_range_length : 0;
	if (np->ADDR_length)
		bcopy(np->ADDR_buffer, mp->b_rptr + p->info_ack.ADDR_offset, np->ADDR_length);
	if (np->QOS_length)
		bcopy(np->QOS_buffer, mp->b_rptr + p->info_ack.QOS_offset, np->QOS_length);
	if (np->QOS_range_length)
		bcopy(np->QOS_range_buffer, mp->b_rptr + p->info_ack.QOS_range_offset,
		      np->QOS_range_length);
	if (np->PROTOID_length)
		bcopy(np->PROTOID_buffer, mp->b_rptr + p->info_ack.PROTOID_offset,
		      np->PROTOID_length);
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_INFO_ACK");
	qreply(q, mp);
	return (QR_DONE);
      error:
	return n_error_ack(q, np, N_INFO_REQ, err, mp);
}

/**
 * n_bind_req - bind to a NS address
 * @q: write queue
 * @mp: the N_BIND_REQ message
 *
 * Because the NS provider can be either N_CONS or N_CLNS, as a feature we recognize N_CONS or
 * N_CLNS set in the second least significant byte of the BIND_flags member.  When not specified,
 * the service class defaults to a default based on which character device was opened.
 */
STATIC INLINE int
n_bind_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	struct ne_bind_req ne = { NE_BIND_REQ, };
	size_t size;
	mblk_t *bp = NULL;
	int err;

	err = NOUTSTATE;
	if (np_get_state(np) != NS_UNBND)
		goto error;
	err = -EMSGSIZE;
	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(p->bind_req)))
		goto error;
	err = -EFAULT;
	if (p->type != N_BIND_REQ)
		goto error;
	err = NBADADDR;
	if (unlikely(mp->b_wptr < (ne.ADDR_buffer = mp->b_rptr + p->bind_req.ADDR_offset)
		     + (ne.ADDR_length = p->bind_req.ADDR_length)))
		goto error;
	err = NBADADDR;
	if (unlikely(mp->b_wptr < (ne.PROTOID_buffer = mp->b_rptr + p->bind_req.PROTOID_offset)
		     + (ne.PROTOID_length = p->bind_req.PROTOID_length)))
		goto error;
	err = NNOADDR;
	if (unlikely(ne.ADDR_length == 0))
		goto error;
	err = NNOPROTOID;
	if (unlikely(ne.PROTOID_length == 0))
		goto error;
	err = NBADFLAG;
	if ((ne.BIND_flags = p->BIND_flags) & ~(DEFAULT_LISTENER | TOKEN_REQUEST | DEFAULT_DEST))
		goto error;
	ne.CONIND_number = p->bind_req.CONIND_number;
	np_set_state(np, NS_WACK_BREQ);
	if (unlikely((err = np->np_event(q, np, &ne)))) {
		np_set_state(np, NS_UNBND);
		goto error;
	}
	if (mp->b_cont)
		freemsg(XCHG(&mp->b_cont, NULL));
	size = sizeof(p->bind_ack) + ne.ADDR_length + ne.PROTOID_length;
	if (msgsize(mp) < size) {
		bp = mp;
		err = -ENOBUFS;
		if (!(mp = ss7_allocb(q, size, BPRI_MED))) {
			ne.EVENT = NE_UNBIND_REQ;
			np->np_event(q, np, &ne);
			mp = bp;
			goto error;
		}
	}
	mp->b_datap->db_type = M_PCPROTO;
	mp->b_band = 0;
	p = (typeof(p)) mp->b_rptr;
	p->bind_ack.PRIM_type = N_BIND_ACK;
	p->bind_ack.ADDR_length = ne.ADDR_length;
	p->bind_ack.ADDR_offset = ne.ADDR_length ? sizeof(p->bind_ack) : 0;
	p->bind_ack.CONIND_number = ne.CONIND_number;
	p->bind_ack.TOKEN_value = (np_ulong) (long) np;	/* always provide a token value */
	p->bind_ack.PROTOID_length = ne.PROTOID_length;
	p->bind_ack.PROTOID_offset = ne.PROTOID_length ? sizeof(p->bind_ack) + ne.ADDR_length : 0;
	mp->b_wptr = mp->b_rptr + size;
	if (ne.ADDR_length)
		bcopy(ne.ADDR_buffer, mp->b_rptr + p->bind_ack.ADDR_offset, ne.ADDR_length);
	if (ne.PROTOID_length)
		bcopy(ne.PROTOID_buffer, mp->b_rptr + p->bind_ack.PROTOID_offset,
		      ne.PROTOID_length);
	if (bp)
		freemsg(bp);
	np_set_state(np, NS_IDLE);
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_BIND_ACK");
	qreply(q, mp);
	return (QR_ABSORBED);
      error:
	return n_reply_ack(np, N_BIND_REQ, err, mp);
}
STATIC INLINE int
n_unbind_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	struct ne_unbind_req ne = { NE_UNBIND_REQ };
	int err;

	err = NOUTSTATE;
	if (np_get_state(np) != NS_IDLE)
		goto error;
	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + sizeof(p->unbind_req))
		goto error;
	err = -EFAULT;
	if (p->type != N_UNBIND_REQ)
		goto error;
	np_set_state(np, NS_WACK_UREQ);
	if ((err = np->np_event(q, np, &ne)) == 0)
		np_set_state(np, NS_UNBND);
      error:
	return n_reply_ack(np, N_UNBIND_REQ, err, mp);
}

/**
 * n_unitdata_req - send unit data
 * @q: write queue
 * @mp: N_UNITDATA_REQ message
 *
 * A slight difference to normal NPI as a feature: the first RESERVED_field is used to indicate the
 * index of the protocol id from/for which to send.  The second RESERVED_field is used to indicate
 * the index of the source address from which to send.  When bound to a single protocol id or source
 * address, the corresponding index is zero (0).
 */
STATIC INLINE int
n_unitdata_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	struct ne_unitdata_req ne = { NE_UNITDATA_REQ, };
	int err;

	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + sizeof(p->unidata_req))
		goto error;
	err = -EFAULT;
	if (p->type != N_UNITDATA_REQ)
		goto error;
	err = NNOTSUPPORT;
	if (!(np->np_info.SERV_type & N_CLNS))
		goto error;
	err = NOUTSTATE;
	if (np_get_state(np) != NS_IDLE)
		goto error;
	err = NBADADDR;
	if (mp->b_wptr < (ne.DEST_buffer = mp->b_rptr + p->DEST_offset)
	    + (ne.DEST_length = p->DEST_length))
		goto error;
	err = NNOADDR;
	if (ne.DEST_length == 0)
		goto error;
	err = NBADDATA;
	if (!(ne.DATA_blocks = mp->b_cont)
	    || (ne.DATA_length = msgsize(mp->b_cont)) > np->np_info.NSDU_size
	    || ne.DATA_length > np->np_info.NIDU_size)
		goto error;
	if ((err = np->np_event(q, np, &ne)))
		goto error;
	return (QR_TRIMMED);
      error:
	switch (err) {
	case 0:
		return (QR_DONE);
	case NOUTSTATE:
	case NBADDATA:
		/* fatal errors */
		return n_reply_err(np, N_UNITDATA_REQ, err, mp);
	default:
		break;
	}
	/* non-fatal errors */
	mp->b_datap->db_type = M_PROTO;
	mp->b_band = 0;
	p->uderror_ind.PRIM_type = N_UDERROR_IND;
	/* DEST_length and DEST_offset are the same */
	p->uderror_ind.ERROR_type = err;
	mp->b_wptr = mp->b_rptr + sizeof(p->uderror_ind);
	/* attached DATA_blocks is the same */
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSP_PRIM, SL_TRACE, "<- N_UDERROR_IND");
	qreply(q, mp);
	return (QR_ABSORBED);
}

/**
 * n_optmgmt_req - options management request
 * @q: write queue
 * @mp: the N_OPTMGMT_REQ message
 *
 * It might be an idea to define some provider-specific flags.  Some things could be performing or
 * not performing SAR, cooked and uncooked headers, and the like.  Another would be promiscuous mode
 * (processing packets with packet type == PACKET_OTHERHOST).
 */
STATIC INLINE int
n_optmgmt_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	struct ne_optmgmt_req ne = { NE_OPTMGMT_REQ, };
	int err;

	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + sizeof(p->optmgmt_req))
		goto error;
	err = -EFAULT;
	if (p->type != N_OPTMGMT_REQ)
		goto error;
	err = NBADOPT;
	if (mp->b_wtpr < (ne.QOS_buffer = mp->b_rptr + p->QOS_offset)
	    + (ne.QOS_length = p->QOS_length))
		goto error;
	if (ne.QOS_length < sizeof(np_ulong))
		goto error;
	err = NBADFLAG;
	if ((ne.OPTMGMT_flags = p->OPTMGMT_flags) & ~(DEFAULT_RC_SEL))
		goto error;
	err = np->np_event(q, np, &ne);
      error:
	return n_reply_ack(np, N_OPTMGMT_REQ, err, mp);
}
STATIC INLINE int
n_datack_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	union N_primitives *p = (typeof(p)) mp->b_rptr;
	struct ne_datack_req ne = { NE_DATACK_REQ, };
	np_ulong state;
	int err;

	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + sizeof(p->optmgmt_ack))
		goto error;
	err = -EFAULT;
	if (p->type != N_DATACK_REQ)
		goto error;
	err = NNOTSUPPORT;
	if (!(np->np_info.SERV_type & N_CONS))
		goto error;
	err = 0;
	if ((state = np_get_state(np)) == NS_IDLE)
		goto error;
	err = NOUTSTATE;
	if (state != NS_DATA_XFER)
		goto error;
	if ((err = np->np_event(q, np, &ne)))
		goto error;
	return (QR_TRIMMED);
      error:
	return n_reply_err(np, N_DATACK_REQ, err, mp);
}
STATIC INLINE int
n_reset_req(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_primitive *p = (typeof(p)) mp->b_rptr;
	struct ne_reset_req = { NE_RESET_REQ, };
	np_ulong state;
	int err;

	err = 0;
	if ((state = np_get_state(np)) == NS_IDLE)
		goto error;
	err = NOUTSTATE;
	if (state != NS_DATA_XFER)
		goto error;
	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + sizeof(p->datack_req))
		goto error;
	err = -EFAULT;
	if (p->type != N_RESET_REQ)
		goto error;
	ne.RESET_reason = p->RESET_reason;
	if ((err = np->np_event(q, np, &ne)))
		goto error;
	np_set_state(np, NS_WCON_RREQ);
      error:
	return n_reply_ack(np, N_RESET_REQ, err, mp);
}
STATIC INLINE int
n_reset_res(queue_t *q, mblk_t *mp)
{
	struct np *np = NP_PRIV(q);
	N_primitive *p = (typeof(p)) mp->b_rptr;
	struct ne_reset_res = { NE_RESET_RES, };
	np_ulong state;
	int err;

	if ((state = np_get_state(np)) == NS_IDLE)
		goto discard;
	err = NOUTSTATE;
	if (state != NS_WRES_RIND)
		goto error;
	err = -EMSGSIZE;
	if (mp->b_wptr < mp->b_rptr + sizeof(p->reset_res))
		goto error;
	err = -EFAULT;
	if (p->type != N_RESET_RES)
		goto error;
	np_set_state(np, NS_WACK_RRES);
	if (!(err = np->np_event(q, np, &ne)))
		ns_set_state(np, NS_DATA_XFER);
      error:
	return n_reply_ack(np, N_RESET_RES, err, mp);
      discard:
	return (QR_DONE);
}

/**
 * np_w_proto - process M_PROTO/M_PCPROTO messages on the write queue
 * @q: write queue
 * @mp: the M_PROTO/M_PCPROTO message
 */
STATIC INLINE fastcall __hot_put int
np_w_proto(queue_t *q, mblk_t *mp)
{
	int rtn = -EPROTO;
	np_long prim = 0;
	struct np *np = DL_PRIV(q);
	np_long oldstate = npi_get_state(np);

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(prim)))
		goto error;
	prim = *(nl_long *) mp->b_rptr;
	strlog(DRV_ID, np->u.dev.cminor, NS_LOG_NSU_PRIM, SL_TRACE, "%s ->", np_prim_name(prim));
	switch (prim) {
	/* *INDENT-OFF* */
	case N_CONN_REQ:	rtn = n_conn_req    (q, mp); break;
	case N_CONN_RES:	rtn = n_conn_res    (q, mp); break;
	case N_DISCON_REQ:	rtn = n_discon_req  (q, mp); break;
	case N_DATA_REQ:	rtn = n_data_req    (q, mp); break;
	case N_EXDATA_REQ:	rtn = n_exdata_req  (q, mp); break;
	case N_INFO_REQ:	rtn = n_info_req    (q, mp); break;
	case N_BIND_REQ:	rtn = n_bind_req    (q, mp); break;
	case N_UNBIND_REQ:	rtn = n_unbind_req  (q, mp); break;
	case N_UNITDATA_REQ:	rtn = n_unitdata_req(q, mp); break;
	case N_OPTMGMT_REQ:	rtn = n_optmgmt_req (q, mp); break;
	case N_DATACK_REQ:	rtn = n_datack_req  (q, mp); break;
	case N_RESET_REQ:	rtn = n_reset_req   (q, mp); break;
	case N_RESET_RES:	rtn = n_reset_res   (q, mp); break;
	/* *INDENT-ON* */
	case N_CONN_IND:
	case N_CONN_CON:
	case N_DISCON_IND:
	case N_DATA_IND:
	case N_EXDATA_IND:
	case N_INFO_ACK:
	case N_BIND_ACK:
	case N_ERROR_ACK:
	case N_OK_ACK:
	case N_UNITDATA_IND:
	case N_UDERROR_IND:
	case N_DATACK_IND:
	case N_RESET_IND:
	case N_RESET_CON:
		/* wrong direction */
		rtn = -EPROTO;
		break;
	default:
		/* unrecognized primitive */
		rtn = -EOPNOTSUPP;
		break;
	}
      error:
	if (unlikely(rtn < 0)) {
		seldom();
		npi_set_state(np, oldstate);
		/* The put and service procedure do not recognize all errors. Sometimes we return
		   an error to here just to restore the previous state. */
		switch (rtn) {
		case -EBUSY:	/* flow controlled */
		case -EAGAIN:	/* try again */
		case -ENOMEM:	/* could not allocate memory */
		case -ENOBUFS:	/* could not allocate an mblk */
		case -EOPNOTSUPP:	/* primitive not supported */
			return np_error_ack(q, prim, rtn);
		case -EPROTO:
			return np_error_reply(q, rtn);
		default:
			/* ignore all other errors */
			rtn = 0;
			break;
		}
	}
	return (rtn);
}
STATIC INLINE fastcall __hot_put int
np_w_data(queue_t *q, mblk_t *mp)
{
}
STATIC int
np_w_ctl(queue_t *q, mblk_t *mp)
{
}
STATIC int
np_w_ioctl(queue_t *q, mblk_t *mp)
{
}

/*
 *  DLS USER PRIMITIVES RECEIVED FROM DOWNSTREAM
 *  ============================================
 */

STATIC int
dl_info_ack(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_bind_ack(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_ok_ack(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_error_ack(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_subs_bind_ack(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_unitdata_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_uderror_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_connect_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_connect_con(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_token_ack(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_disconnect_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_reset_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_reset_con(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_data_ack_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_data_ack_status_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_reply_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_reply_status_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_reply_update_status_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_xid_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_xid_con(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_test_ind(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_test_con(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_phys_addr_ack(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}
STATIC int
dl_get_statistics_ack(queue_t *q, mblk_t *mp)
{
	return (NNOTSUPPORT);
}

STATIC INLINE fastcall __hot_in int
dl_r_proto(queue_t *q, mblk_t *mp)
{
	struct dl *dl = DL_PRIV(q);
	dl_ulong oldstate = dl_get_state(dl);
	dl_long prim;
	int rtn - EMSGSIZE;

	if (unlikely(mp->b_wptr < mp->b_rptr + sizeof(prim)))
		goto error;
	prim = *(dl_long *) mp->b_rptr;
	strlog(DRV_ID, dl->u.mux.index, DLS_LOG_DLSP_PRIM, SL_TRACE, "<- %s", dl_prim_name(prim));
	switch (prim) {
	/* *INDENT-OFF* */
	case DL_INFO_ACK:		rtn = dl_info_ack		(q, mp); break;
	case DL_BIND_ACK:		rtn = dl_bind_ack		(q, mp); break;
	case DL_OK_ACK:			rtn = dl_ok_ack			(q, mp); break;
	case DL_ERROR_ACK:		rtn = dl_error_ack		(q, mp); break;
	case DL_SUBS_BIND_ACK:		rtn = dl_subs_bind_ack		(q, mp); break;
	case DL_UNITDATA_IND:		rtn = dl_unitdata_ind		(q, mp); break;
	case DL_UDERROR_IND:		rtn = dl_uderror_ind		(q, mp); break;
	case DL_CONNECT_IND:		rtn = dl_connect_ind		(q, mp); break;
	case DL_CONNECT_CON:		rtn = dl_connect_con		(q, mp); break;
	case DL_TOKEN_ACK:		rtn = dl_token_ack		(q, mp); break;
	case DL_DISCONNECT_IND:		rtn = dl_disconnect_ind		(q, mp); break;
	case DL_RESET_IND:		rtn = dl_reset_ind		(q, mp); break;
	case DL_RESET_CON:		rtn = dl_reset_con		(q, mp); break;
	case DL_DATA_ACK_IND:		rtn = dl_data_ack_ind		(q, mp); break;
	case DL_DATA_ACK_STATUS_IND:	rtn = dl_data_ack_status_ind	(q, mp); break;
	case DL_REPLY_IND:		rtn = dl_reply_ind		(q, mp); break;
	case DL_REPLY_STATUS_IND:	rtn = dl_reply_status_ind	(q, mp); break;
	case DL_REPLY_UPDATE_STATUS_IND:rtn = dl_reply_update_status_ind(q, mp); break;
	case DL_XID_IND:		rtn = dl_xid_ind		(q, mp); break;
	case DL_XID_CON:		rtn = dl_xid_con		(q, mp); break;
	case DL_TEST_IND:		rtn = dl_test_ind		(q, mp); break;
	case DL_TEST_CON:		rtn = dl_test_con		(q, mp); break;
	case DL_PHYS_ADDR_ACK:		rtn = dl_phys_addr_ack		(q, mp); break;
	case DL_GET_STATISTICS_ACK:	rtn = dl_get_statistics_ack	(q, mp); break;
	/* *INDENT-ON* */
	case DL_INFO_REQ:
	case DL_ATTACH_REQ:
	case DL_DETACH_REQ:
	case DL_BIND_REQ:
	case DL_UNBIND_REQ:
	case DL_SUBS_BIND_REQ:
	case DL_SUBS_UNBIND_REQ:
	case DL_ENABMULTI_REQ:
	case DL_DISABMULTI_REQ:
	case DL_PROMISCON_REQ:
	case DL_PROMISCOFF_REQ:
	case DL_UNITDATA_REQ:
	case DL_UDQOS_REQ:
	case DL_CONNECT_REQ:
	case DL_CONNECT_RES:
	case DL_TOKEN_REQ:
	case DL_DISCONNECT_REQ:
	case DL_RESET_REQ:
	case DL_RESET_RES:
	case DL_DATA_ACK_REQ:
	case DL_REPLY_REQ:
	case DL_REPLY_UPDATE_REQ:
	case DL_XID_REQ:
	case DL_XID_RES:
	case DL_TEST_REQ:
	case DL_TEST_RES:
	case DL_PHYS_ADDR_REQ:
	case DL_SET_PHYS_ADDR_REQ:
	case DL_GET_STATISTICS_REQ:
		/* wrong direction */
		rtn = DL_UNSUPPORTED;
		break;
	default:
		/* not recognized */
		rtn = DL_BADPRIM;
		break;
	}
      error:
	if (unlikely(rtn < 0)) {
		seldom();
		dl_set_state(dl, oldstate);
		switch (rtn) {
		case -EBUSY:	/* flow controlled */
		case -EAGAIN:	/* try again */
		case -ENOMEM:	/* could not allocate memory */
		case -ENOBUFS:	/* could not allocate an mblk */
			return dl_error_ack(q, prim, rtn);
		case -EPROTO:
		case -EMSGSIZE:
		case -EOPNOTSUPP:
		case -EINVAL:
			return dl_error_ind(q, prim, rtn);
		default:
			/* ignore all other errors */
			rtn = 0;
			break;
		}
	}
	return (rtn);
}

STATIC INLINE fastcall __hot_in int
dl_r_data(queue_t *q, mblk_t *mp)
{
}
STATIC int
dl_r_ctl(queue_t *q, mblk_t *mp)
{
}
STATIC int
dl_r_ioctl(queue_t *q, mblk_t *mp)
{
}
STATIC int
dl_r_hangup(queue_t *q, mblk_t *mp)
{
}
STATIC int
dl_r_error(queue_t *q, mblk_t *mp)
{
}

/*
 *  Put and Service procedures
 *  ==========================
 */
STATIC streamscall __hot_put int
np_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_PROTO:
	case M_PCPROTO:
		return np_w_proto(q, mp);
	case M_DATA:
		return np_w_data(q, mp);
	case M_CTL:
	case M_PCCTL:
		return np_w_ctl(q, mp);
	case M_IOCTL:
	case M_IOCDATA:
		return np_w_ioctl(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	}
	swerr();
	return (QR_PASSFLOW);
}
STATIC streamscall __hot_get int
np_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_RSE:
	case M_PCRSE:
		return np_r_rse(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	return (QR_PASSFLOW);
}
STATIC streamscall __hot_out int
dl_w_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_type) {
	case M_RSE:
	case M_PCRSE:
		return dl_w_rse(q, mp);
	case M_FLUSH:
		return ss7_w_flush(q, mp);
	}
	return (QR_PASSFLOW);

}
STATIC streamscall __hot_in int
dl_r_prim(queue_t *q, mblk_t *mp)
{
	switch (mp->b_datap->db_dtype) {
	case M_PROTO:
	case M_PCPROTO:
		return dl_r_proto(q, mp);
	case M_DATA:
		return dl_r_data(q, mp);
	case M_CTL:
	case M_PCCTL:
		return dl_r_ctl(q, mp);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
		return dl_r_ioctl(q, mp);
	case M_HANGUP:
		return dl_r_hangup(q, mp);
	case M_ERROR:
		return dl_r_error(q, mp);
	case M_FLUSH:
		return ss7_r_flush(q, mp);
	}
	swerr();
	return (QR_PASSFLOW);
}

/*
 *  Private structure allocation, deallocation and cache
 *  ====================================================
 */
STATIC int
np_term_caches(void)
{
	err = 0;
	if (np_priv_cachep != NULL) {
		if (kmem_cache_destroy(np_priv_cachep)) {
			strlog(DRV_ID, 0, LOG_ERR, SL_ERROR | SL_CONSOLE,
			       "could not destroy np_priv_cachep");
			err = -EBUSY;
		} else {
			strlog(DRV_ID, 0, LOG_DEBUG, SL_TRACE, "destroyed np_priv_cachep");
		}
	}
	if (np_link_cachep != NULL) {
		if (kmem_cache_destroy(np_link_cachep)) {
			strlog(DRV_ID, 0, LOG_ERR, SL_ERROR | SL_CONSOLE,
			       "could not destroy np_link_cachep");
			err = -EBUSY;
		} else {
			strlog(DRV_ID, 0, LOG_DEBUG, SL_TRACE, "destroyed np_link_cachep");
		}
	}
	return (err);
}
STATIC int
np_init_caches(void)
{
	if (np_priv_cachep == NULL) {
		np_priv_cachep = kmem_cache_create("np_priv_cachep", sizeof(struct np), 0,
						   SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (np_priv_cachep == NULL) {
			strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
			       "canot allocate np_priv_cachep");
			np_term_caches();
			return (-ENOMEM);
		}
		strlog(DRV_ID, 0, LOG_DEBUG, SL_TRACE, "initialized driver priv structure cache");
	}
	if (np_link_cachep == NULL) {
		np_link_cachep = kmem_cache_create("np_link_cachep", sizeof(struct np), 0,
						   SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (np_link_cachep == NULL) {
			strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
			       "canot allocate np_link_cachep");
			np_term_caches();
			return (-ENOMEM);
		}
		strlog(DRV_ID, 0, LOG_DEBUG, SL_TRACE, "initialized driver link structure cache");
	}
	return (0);
}

/* Information */

STATIC N_info_ack_t *np_info[NP_MINOR_FREE] = {
	[NP_MINOR_OTHER] = &np_other_info,
	[NP_MINOR_ETH] = &np_eth_info,
	[NP_MINOR_LAPB] = &np_lapb_info,
	[NP_MINOR_LAPD] = &np_lapb_info,
	[NP_MINOR_LAPF] = &np_lapf_info,
	[NP_MINOR_IP] = &np_ip_info,
	[NP_MINOR_X25] = &np_x25_info,
	[NP_MINOR_CLNL] = &np_clnl_info,
	[NP_MINOR_CONS] = &np_cons_info,
};

STATIC np_event_t *np_event[NP_MINOR_FREE] = {
	[NP_MINOR_OTHER] = &np_other_event,
	[NP_MINOR_ETH] = &np_eth_event,
	[NP_MINOR_LAPB] = &np_lapb_event,
	[NP_MINOR_LAPD] = &np_lapb_event,
	[NP_MINOR_LAPF] = &np_lapf_event,
	[NP_MINOR_IP] = &np_ip_event,
	[NP_MINOR_X25] = &np_x25_event,
	[NP_MINOR_CLNL] = &np_clnl_event,
	[NP_MINOR_CONS] = &np_cons_event,
};

STATIC struct np *
np_alloc_priv(queue_t *q, struct np **npp, uint type, int sflag, dev_t *devp, cred_t *crp)
{
	struct np *np;

	if ((np = np_alloc())) {
		major_t cmajor = getmajor(*devp);
		minor_t cminor = getminor(*devp);

		/* np generic members */
		np->u.dev.cmajor = cmajor;
		np->u.dev.cminor = cminor;
		np->cred = *crp;
		(np->oq = RD(q))->q_ptr = np_get(np);
		(np->iq = WR(q))->q_ptr = np_get(np);
		np->i_prim = &np_w_prim;
		np->o_prim = &np_r_prim;
		np->i_wakeup = NULL;
		np->o_wakeup = NULL;
		np->type = type;
		spinlock_init(&np->qlock);
		np->i_version = N_CURRENT_VERSION;
		np->np_event = np_event[bminor];
		np->np_info = np_info[bminor];
		np->i_style = DL_STYLE2;
		np->i_state = np->i_oldstate = np->np_info.CURRENT_state = NS_UNBND;
		/* link into master list */
		if ((np->next = *npp))
			np->next->prev = &np->next;
		np->prev = npp;
		*npp = np_get(np);
	} else
		strlog(DRV_ID, getminor(*devp), LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "could not allocate driver private structure");
	return (np);
}
STATIC void
np_free_priv(queue_t *q)
{
	struct np *np = NP_PRIV(q);

	/* FIXME: Here we need to handle the disposition of the state of the Stream.  If the state
	   is not unattached, we need to take it there. */

	qprocsoff(q);
	strlog(DRV_ID, np->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "unlinking private structure: reference count = %d", atomic_read(&np->refcnt));
	ss7_unbufcall((str_t *) np);
	strlog(DRV_ID, np->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "removed bufcalls: reference count = %d", atomic_read(&np->refcnt));
	/* remove from master list */
	write_lock_bh(&master.lock);
	if ((*np->prev = np->next))
		np->next->prev = np->prev;
	np->next = NULL;
	np->prev = &np->next;
	write_unlock_bh(&master.lock);
	strlog(DRV_ID, np->u.dev.cminor, LOG_DEBUG, SL_TRACE,
	       "unlinked: reference count = %d", atomic_read(&np->refcnt));
	np_release(&np->oq->q_ptr);
	np->oq = NULL;
	np_release(&np->iq->q_ptr);
	np->iq = NULL;
	assure(atomic_read(&tp->refcnt) == 1);
	np_release(&np);
	return;
}

STATIC struct dl *
np_alloc_link(queue_t *q, struct dl **dlp, uint type, ulong index, cred_t *crp)
{
	struct dl *dl;

	if ((dl = dl_alloc())) {
		dl->u.mux.index = index;
		dl->cred = *crp;
		(dl->oq = WR(q))->q_ptr = dl_get(dl);
		(dl->iq = RD(q))->q_ptr = dl_get(dl);
		dl->i_prim = &dl_r_prim;
		dl->o_prim = &dl_w_prim;
		dp->i_wakeup = NULL;
		dp->o_wakeup = NULL;
		dl->type = type;
		spinlock_init(&dl->qlock);
		dl->i_version = DL_CURRENT_VERSION;
		dl->dl_event = dl_event[type];
		dl->dl_info = dl_info[type];
		dl->i_style = DL_STYLE2;
		dl->i_state = dl->i_oldstate = dl->dl_info.dl_current_state = DL_UNATTACHED;
		/* link into master list */
		if ((dl->next = *dlp))
			dl->next->prev = &dl->next;
		dl->prev = dlp;
		*dlp = dl_get(dl);
	} else
		strlog(DRV_ID, index, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "could not allocate driver linked structure");
	return (dl);
}
STATIC void
np_free_link(queue_t *q)
{
	struct dl *dl = DL_PRIV(q);

	qprocsoff(q);
	strlog(DRV_ID, dl->u.mux.index, LOG_DEBUG, SL_TRACE,
	       "unlinking linked structure: reference count = %d", atomic_read(&dl->refcnt));
	ss7_unbufcall((str_t *) dl);
	strlog(DRV_ID, dl->u.mux.index, LOG_DEBUG, SL_TRACE,
	       "removed bufcalls: reference count = %d", atomic_read(&dl->refcnt));
	/* remove from master list */
	write_lock_bh(&master.lock);
	if ((*dl->prev = dl->next))
		dl->next->prev = dl->prev;
	dl->next = NULL;
	dl->prev = &dl->next;
	write_unlock_bh(&master.lock);
	strlog(DRV_ID, dl->u.mux.index, LOG_DEBUG, SL_TRACE,
	       "unlinked: reference count = %d", atomic_read(&dl->refcnt));
	dl_release(&dl->oq->q_ptr);
	dl->oq = NULL;
	dl_release(&dl->iq->q_ptr);
	dl->iq = NULL;
	assure(atomic_read(&dl->refcnt) == 1);
	dl_release(&dl);
	return;
}

/*
 *  STREAMS Open and Close
 *  ======================
 */
STATIC streamscall int
np_open(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp), bminor = cminor;
	struct np *np, **npp;

	if (q->q_ptr != NULL)
		return (0);	/* already open */
	if (sflag == MODOPEN || WR(q)->q_next) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE, "cannot push as module");
		return (ENXIO);
	}
#ifdef LIS
	if (cmajor != CMAJOR_0 || cminor >= NP_MINOR_FREE) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "attempt to open device %d:%d directly", cmajor, cminor);
		return (ENXIO);
	}
#endif				/* LIS */
#ifdef LFS
	if (cmajor != DRV_ID || cminor >= NP_MINOR_FREE) {
		strlog(DRV_ID, cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "attempt to open device %d:%d directly", cmajor, cminor);
		return (ENXIO);
	}
#endif				/* LFS */
	if (sflag == CLONEOPEN || cminor < NP_MINOR_FREE) {
		strlog(DRV_ID, cminor, LOG_DEBUG, SL_TRACE, "clone open in effect");
		sflag = CLONEOPEN;
		cminor = NP_MINOR_FREE;
	}
	npp = &master.lnk.list;
	write_lock_bh(&master.lock);
	for (; *npp; npp = &(*npp)->next) {
		if (cmajor != (*npp)->u.dev.cmajor)
			break;
		if (cminor < (*npp)->u.dev.cminor)
			break;
		if (cminor == (*npp)->u.dev.cminor) {
			if (sflag != CLONEOPEN) {
				write_unlock_bh(&master.lock);
				strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
				       "opened device %d:%d in use!", cmajor, cminor);
				return (EAGAIN);
			}
			if (getminor(makedevice(cmajor, ++cminor)) != cminor) {
				cmajor = 0;
				break;
			}
		}
	}
	if (!cmajor) {
		write_unlock_bh(&master.lock);
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE, "no device numbers available");
		return (ENXIO);
	}
	cmn_err(CE_NOTE, "%s: opened character device %d:%d", DRV_NAME, cmajor, cminor);
	*devp = makedevice(cmajor, cminor);
	if (!(np = np_alloc_priv(q, npp, bminor, sflag, devp, crp))) {
		write_unlock_bh(&master.lock);
		return (ENOMEM);
	}
	write_unlock_bh(&master.lock);
	qprocson(q);
	return (0);
}

STATIC streamscall int
np_qclose(queue_t *q, int oflags, cred_t *crp)
{
	struct np *np = NP_PRIV(q);

	strlog(DRV_ID, np->u.dev.cminor, LOG_DEBUG, SL_TRACE, "closing character device");
#ifdef LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		strlog(DRV_ID, np->u.dev.cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "LiS double-close bug detected");
		goto quit;
	}
#if 0
	/* only for modules push on pipe ends */
	if (q->q_next == NULL) {
		strlog(DRV_ID, np->u.dev.cminor, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "LiS pipe bug: called with NULL q->q_next pointer");
		goto skip_pop;
	}
#endif
#endif				/* LIS */
	goto skip_pop;
      skip_pop:
	qprocsoff(q);
	np_free_priv(q);
	goto quit;
      quit:
	return (0);
}

/*
 *  Registration and initialization
 *  ===============================
 */
#ifdef LINUX
/*
 *  Linux Regsitration
 *  ------------------
 */

unsigned short modid = DRV_ID;

#ifndef module_parm
MODULE_PARM(modid, "h");
#else				/* module_parm */
module_param(modid, ushort, 0);
#endif				/* module_parm */
MODULE_PARM_DESC(modid, "Module ID number for NP driver (0 for allocation).");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else				/* module_param */
module_param(major, uint, 0);
#endif				/* module_param */
MODULE_PARM_DESC(major, "Major device number for NP driver (0 for allocation).");

#ifdef LFS
/*
 *  Linux Fast-STREAMS Registration
 *  -------------------------------
 */
STATIC struct cdevsw np_cdev = {
	.d_name = DRV_NAME,
	.d_str = &np_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR | S_IRUGO | S_IWUGO,
	.d_kmod = THIS_MODULE,
};

STATIC struct devnode np_node_eth = {
	.n_name = "eth",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
STATIC struct devnode np_node_lapb = {
	.n_name = "lapb",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
STATIC struct devnode np_node_lapd = {
	.n_name = "lapd",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
STATIC struct devnode np_node_lapf = {
	.n_name = "lapf",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
STATIC struct devnode np_node_ip = {
	.n_name = "ip",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
STATIC struct devnode np_node_x25 = {
	.n_name = "x25",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
STATIC struct devnode np_node_clnl = {
	.n_name = "clnl",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};
STATIC struct devnode np_node_cons = {
	.n_name = "cons",
	.n_flag = D_CLONE,		/* clone minor */
	.n_mode = S_IFCHR | S_IRUGO | S_IWUGO,
};

STATIC int
np_register_strdev(major_t major)
{
	int err;

	if ((err = regsister_strdev(&np_cdev, major)) < 0)
		return (err);
	return (0);
}
STATIC void
np_register_strnod(void)
{
	register_strnod(&np_cdev, &np_node_eth, NP_MINOR_ETH);
	register_strnod(&np_cdev, &np_node_lapb, NP_MINOR_LAPB);
	register_strnod(&np_cdev, &np_node_lapd, NP_MINOR_LAPD);
	register_strnod(&np_cdev, &np_node_lapf, NP_MINOR_LAPF);
	register_strnod(&np_cdev, &np_node_ip, NP_MINOR_IP);
	register_strnod(&np_cdev, &np_node_x25, NP_MINOR_X25);
	register_strnod(&np_cdev, &np_node_clnl, NP_MINOR_CLNL);
	register_strnod(&np_cdev, &np_node_cons, NP_MINOR_CONS);
}
STATIC void
np_unregister_strnod(void)
{
	unregister_strnod(&np_cdev, &np_node_cons, NP_MINOR_CONS);
	unregister_strnod(&np_cdev, &np_node_clnl, NP_MINOR_CLNL);
	unregister_strnod(&np_cdev, &np_node_x25, NP_MINOR_X25);
	unregister_strnod(&np_cdev, &np_node_ip, NP_MINOR_IP);
	unregister_strnod(&np_cdev, &np_node_lapf, NP_MINOR_LAPF);
	unregister_strnod(&np_cdev, &np_node_lapd, NP_MINOR_LAPD);
	unregister_strnod(&np_cdev, &np_node_lapb, NP_MINOR_LAPB);
	unregister_strnod(&np_cdev, &np_node_eth, NP_MINOR_ETH);
}
STATIC int
np_unregister_strdev(major_t major)
{
	if ((err = unregister_strdev(&np_cdev, major)) < 0)
		return (err);
	return (0);
}
#endif				/* LFS */

#ifdef LIS
/*
 *  Linux STREAMS Registation
 *  -------------------------
 */
STATIC int
np_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &np_info, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}
STATIC void
np_register_strnod(void)
{
}
STATIC void
np_unregister_strnod(void)
{
}
STATIC int
np_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}
#endif				/* LIS */

STATIC int np_initialized = 0;
STATIC void
np_init(void)
{
	int err;

	if (np_initialized != 0)
		return;
	cmn_err(CE_NOTE, DRV_BANNER);	/* console splash */
	if ((err = np_init_caches())) {
		np_initialized = err;
		return;
	}
	if ((err = np_register_strdev(major)) < 0) {
		strlog(DRV_ID, 0, LOG_WARNING, SL_WARN | SL_CONSOLE,
		       "cannot regsiter major %d", major);
		np_initialized = err;
		np_term_caches();
		return;
	}
	np_initialized = 1;
	if (major = 0 && err > 0) {
		major = err;
		np_initialized = 2;
	}
	np_register_strnod();
	return;
}
STATIC void
np_terminate(void)
{
	int err;

	if (np_initialized <= 0)
		return;
	np_unregister_strnod();
	if (major) {
		if ((err = np_unregister_strdev(major)) < 0)
			strlog(DRV_ID, 0, LOG_CRIT, SL_FATAL | SL_CONSOLE,
			       "cannot unregister major %d", major);
		major = 0;
	}
	np_term_caches();
	np_initialized = 0;
	return;
}

MODULE_STATIC int __init
np_module_init(void)
{
	np_init();
	if (np_initialized < 0)
		return np_initialized;
	return (0);
}

MODULE_STATIC void __exit
np_module_exit(void)
{
	return np_terminate();
}

module_init(np_module_init);
module_exit(np_module_exit);

#endif				/* LINUX */
