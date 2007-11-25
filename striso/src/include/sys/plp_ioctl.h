/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __SYS_PLP_IOCTL_H__
#define __SYS_PLP_IOCTL_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

#include <linux/ioctl.h>

#define X25_IOC_MAGIC 'x'

#define X25_OBJ_TYPE_DF		 0	/* default */
#define X25_OBJ_TYPE_XP		 1	/* X.25 PLP user */
#define X25_OBJ_TYPE_CN		 2	/* X.25 connection */
#define X25_OBJ_TYPE_RT		 3	/* X.25 route (static, CI, RI) */
#define X25_OBJ_TYPE_AJ		 4	/* X.25 adjacent (SNARE, L1 IS, L2 IS, IDRP GW) */
#define X25_OBJ_TYPE_DT		 5	/* X.25 destination (ES, etc) */
#define X25_OBJ_TYPE_DS		 6	/* X.25 DLSAP */
#define X25_OBJ_TYPE_DL		 7	/* X.25 LAPB or LLC Datalink */

#ifdef __KERNEL__
typedef mblk_t *x25_timer_t;
#else				/* __KERNEL__ */
typedef unsigned long x25_timer_t;
#endif				/* __KERNEL__ */

/*
 * Notes:
 *
 * There is precisely one communications entity and connection-oriented state
 * machine reprsented by the DF object.  The entity can be an ES, SNARE, L1
 * IS, L2 IS or IDRP GW, or combinations thereof.
 *
 * Each entity has a collection of routes in a routing table, the entries of
 * which are represented by RT objects.  Each route can be a static route
 * (allocated by management), a configuration information (CI) route obtained
 * from a SNARE, IS or GW, a redirection information (RI) route obtained from
 * a SNARE or IS.  Timers are associated with dynamic routes.  Upper level
 * modules or user programs establish routes using M_CTL messages or
 * input-output controls.  All routes are referenced against a DLSAP and a
 * datalink.  Timers are issued against the datalink Stream linked beneath
 * the multiplexing driver.
 *
 * All adjacent access to subnetworks are referenced to a DLSAP and a
 * datalink.  The adjacent entity is a SNARE, L1 IS, L2 IS, or IDRP GW.
 * These systems are either statically allocated or discovered using dynamic
 * procedures.  All adjacent systems are referenced against a DLSAP and
 * datalink.  Any timers used with regard to adjacent systems are referenced
 * against the datalink Stream.  Adjacent systems represent non-terminal leaf
 * nodes in the routing table.  Adjacent systems on LANs have a MAC address
 * and DLSAP.  Adjacent systems on point-to-point links are the system at the
 * other end of the link.
 *
 * Each explicit destination system is represented by a DT object.  This is
 * typically an local or remote end-system (ES).  End systems can be allocated
 * statically or dynamically.  All end systems are referenced against a
 * DLSAP and datalink.  Any timers used with regard to end systems are
 * referenced against the datalink Stream.  End systems represent terminal
 * leaf nodes in the routing table.  For LAN stations, they are addressed with
 * a MAC address and DLSAP.  For LAPB stations they are addresses with an
 * X.121 address.
 *
 * Connection objects (CN) represent an X.25 PLP VC.  They are referenced
 * against a DLSPA and a datalink.  They also are referenced against an
 * X.25 PLP user stream.  Timers for upper boundary procedures are referenced
 * against the user stream; those for the lower boundary against the
 * datalink.
 *
 * X.25 PLP user streams are represented by XP objects.
 *
 * Datalinks are either LAPB or XOT point-to-point or LLC2 LAN datalinks,
 * represented by the DL object.  Each datalink contains one or more
 * DLSAPs. LAPB and XOT datalinks have only one DLSAP object per
 * interface, whereas LAN datalinks have multiple DLSAP objects.  DLSAP
 * objects are dynamic (created by the state machines) whereas datalinks are
 * static.  DLSAP configuration information is provisioned against the
 * datalink.  Timers related to DLSAP objects are referenced against the
 * datalink stream.
 *
 * X.283 an ISO 10589 object classes are:
 *
 * system
 * +--> networkSubsystem (subsystem)
 *      +--> networkEntity (communicationsEntity)
 *      |    +--> cLNS (clProtocolMachine)
 *      |    |    +--> linkage
 *      |    |         +--> adjacency
 *      |    |         +--> reachableAddress
 *      |    +--> cONS (coProtocolMachine)
 *      |         +--> linkage
 *      |         |    +--> adjacency
 *      |         |    +--> reachableAddress
 *      |         +--> networkConnection (singlePeerConnection)
 *      |              (points to nSAP)
 *      +--> nSAP (sap2)
 *      +--> x25PLE[IVMO] --> x25PLE-DTE, x25PLE-DCE
 *           (points to sN-SAP associated with linkage)
 *           (points to sN-ServiceProvider associated with linkage)
 *           +--> permanetVirtualCiruit --> pVC-DTE, pVC-DCE
 *           +--> virtualCall --> vC-DTE, vC-DCE
 *                +--> dSeriesCounts
 *
 */

/*
 * Interface options
 */
typedef struct x25_opt_conf_if {
} x25_opt_conf_if_t;
/*
 * Datalink options
 */
typedef struct x25_opt_conf_dl {
} x25_opt_conf_dl_t;
/*
 * Destination options
 */
typedef struct x25_opt_conf_dt {
} x25_opt_conf_dt_t;
/*
 * Adjacent options
 */
typedef struct x25_opt_conf_aj {
} x25_opt_conf_aj_t;
/*
 * Route options
 */
typedef struct x25_opt_conf_rt {
} x25_opt_conf_rt_t;
/*
 * Connection options
 */
typedef struct x25_opt_conf_cn {
} x25_opt_conf_cn_t;
/*
 * User options
 */
typedef struct x25_opt_conf_xp {
} x25_opt_conf_xp_t;
/*
 * Default options
 */
typedef struct x25_opt_conf_df {
} x25_opt_conf_df_t;


#endif				/* __SYS_PLP_IOCTL_H__ */
