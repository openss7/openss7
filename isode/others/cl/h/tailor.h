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

#ifndef __CL_H_TAILOR_H__
#define __CL_H_TAILOR_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* tailor.h - ISODE tailoring */

/* 
 * Header: /f/iso/h/RCS/tailor.h,v 5.0 88/07/21 14:39:37 mrose Rel
 *
 *
 * Log
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

#ifndef	_TAILOR_
#define	_TAILOR_

extern char *isodename;
extern char *isodepath;

extern char *isodeconfig;
extern int isodeconlen;

#ifndef	BSD42
extern char *isodelog;
#endif

#define	ISODELOG_NONE		0
#define	ISODELOG_EXCEPTIONS	(1 << 0)	/* log exceptions */
#define	ISODELOG_PDUS		(1 << 1)	/* trace pdus */
#define	ISODELOG_DEBUG		(1 << 2)	/* full debugging */
#define	ISODELOG_MASK		"\020\01EXCEPTIONS\02PDUS\03DEBUG"

void xsprintf(), _xsprintf();

extern int compatlevel;
extern char *compatfile;

extern int addrlevel;
extern char *addrfile;

extern int tsaplevel;
extern char *tsapfile;

extern int ssaplevel;
extern char *ssapfile;

extern int psaplevel;
extern char *psapfile;

extern int psap2level;
extern char *psap2file;

extern int acsaplevel;
extern char *acsapfile;

extern int rtsaplevel;
extern char *rtsapfile;

extern int rosaplevel;
extern char *rosapfile;

#ifdef	X25
#ifdef	SUN_X25
extern u_char reverse_charge;
extern u_short recvpktsize;
extern u_short sendpktsize;
extern u_char recvwndsize;
extern u_char sendwndsize;
extern u_char recvthruput;
extern u_char sendthruput;
extern u_char cug_req;
extern u_char cug_index;
extern u_char fast_select_type;
extern u_char rpoa_req;
extern u_short rpoa;
#endif

#if	defined(CAMTEC) || defined(CAMTEC_CCL)
extern char *x25_lsubaddr;
extern char x25_lsubprefix;
extern int x25_lsublen;
extern int x25_dtelen;
extern int x25_start_range;
extern int x25_end_range;
extern char x25_outgoing_port;
extern int x25_port;
#endif

#if	defined(UBC_X25) || defined(CAMTEC) || defined(CAMTEC_CCL) || defined(SUN_X25)
extern char *x25_local_janet_dte;
extern char *x25_local_pss_dte;
#endif

extern char *x25_default_listen;
extern char *x25_default_pid;
#endif

#ifdef	BRIDGE_X25
extern char *x25_bridge_host;

extern char *x25_bridge_addr;

extern char *x25_bridge_discrim;

extern char *x25_bridge_listen;
extern char *x25_bridge_pid;
#endif

#if defined(BRIDGE_X25) || defined(X25)
extern u_short x25_bridge_port;
#endif

#ifdef CONS
#ifdef CAMTEC_CCL
extern char *cons_local_address;
extern char cons_is_ybts;
extern char *cons_listen_str;
#endif
#endif

#ifdef	DSAP
extern char dua_enabled;
extern char *dua_address;
#endif

void isodetailor();
char *isodefile();

char *getlocalhost();

#endif

#endif				/* __CL_H_TAILOR_H__ */
