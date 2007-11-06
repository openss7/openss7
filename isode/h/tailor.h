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

#ifndef __ISODE_TAILOR_H__
#define __ISODE_TAILOR_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* tailor.h - ISODE tailoring */

/* 
 * Header: /xtel/isode/isode/h/RCS/tailor.h,v 9.0 1992/06/16 12:17:57 isode Rel
 *
 *
 * Log: tailor.h,v
 * Revision 9.0  1992/06/16  12:17:57  isode
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

#ifndef	_TAILOR_
#define	_TAILOR_

#ifndef	_LOGGER_
#include "logger.h"
#endif

/* SYSTEM AREAS */

extern char *isodename;
extern char *isodebinpath;
extern char *isodesbinpath;
extern char *isodetcpath;
extern char *isodelogpath;

/* LOGGING */

extern LLog *compat_log, _compat_log;
extern LLog *addr_log, _addr_log;
extern LLog *tsap_log, _tsap_log;
extern LLog *ssap_log, _ssap_log;
extern LLog *psap_log, _psap_log;
extern LLog *psap2_log, _psap2_log;
extern LLog *acsap_log, _acsap_log;
extern LLog *rtsap_log, _rtsap_log;
extern LLog *rosap_log, _rosap_log;

extern int _tailor_logging_only;

/* TRANSPORT-SWITCH */

extern int ts_stacks;

#define	TS_NONE	0x00
#define	TS_TCP	0x01
#define	TS_X25	0x02
#define	TS_TP4	0x08
#define TS_X2584 0x10
#define	TS_ALL	0xff

struct ts_interim {
	char *ts_name;			/* community name, also MACRO name */
	char *ts_value;			/* .. MACRO value */

	int ts_subnet;			/* internal key */
	int ts_syntax;			/* same values as na_stack */

	char ts_prefix[20];		/* NSAP prefix */
	int ts_length;			/* .. and length */
};
extern struct ts_interim ts_interim[];

extern int ts_communities[];
extern int ts_comm_nsap_default;
extern int ts_comm_x25_default;
extern int ts_comm_tcp_default;

extern int nsap_default_stack;		/* NAS_CONS or NAS_CLNS */

extern char *tsb_addresses[];
extern int tsb_communities[];

extern char *tsb_default_address;

/* X.25 */

#ifdef	X25
extern char *x25_local_dte;
extern char *x25_local_pid;

extern char x25_intl_zero;
extern char x25_strip_dnic;
extern char *x25_dnic_prefix[];

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

extern LLog *x25_log, _x25_log;

#ifdef	CAMTEC_CCL
extern char x25_outgoing_port;
#endif

#ifdef  CCUR_X25
extern int x25_communities[];
extern char *x25_lines[];
extern char *x25_default_line;
#endif

#ifdef ULTRIX_X25
extern char *x25_default_template;
extern char *x25_default_filter;
extern char *x25_default_class;
#endif
#endif

/* SESSION */

extern int ses_ab_timer;
extern int ses_dn_timer;
extern int ses_rf_timer;

/* USER-FRIENDLY NAMESERVICE */

extern char ns_enabled;
extern char *ns_address;

/* ROUTINES */

void isodetailor();
int isodesetvar();
void isodexport();
char *isodesetailor();

#define	isodefile(file,ispgm) \
	_isodefile ((ispgm) ? isodesbinpath : isodetcpath, (file))

char *_isodefile();

char *getlocalhost();

#endif

#endif				/* __ISODE_TAILOR_H__ */
