/*****************************************************************************

 @(#) $Id: sc.h,v 0.9.2.13 2006/03/03 10:57:11 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>

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

 Last Modified $Date: 2006/03/03 10:57:11 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sc.h,v $
 Revision 0.9.2.13  2006/03/03 10:57:11  brian
 - 32-bit compatibility support, updates for release

 Revision 0.9.2.12  2006/02/20 10:59:20  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ifndef __SYS_STREAMS_SC_H__
#define __SYS_STREAMS_SC_H__

#ident "@(#) $RCSfile: sc.h,v $ $Name:  $($Revision: 0.9.2.13 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef __SYS_SC_H__
#warning "Do no include sys/streams/sc.h directly, include sys/sc.h instead."
#endif

#define SC_IOC_MAGIC 'C'
#define SC_IOC_LIST ((SC_IOC_MAGIC << 8) | 0x01)

#define SCI_FOO 1
#define SCI_BAR 2

struct foo {
	int foo;
};

struct bar {
	int bar;
};

struct sc_module_info {
	ushort mi_idnum;		/* module id number */
	char mi_idname[FMNAMESZ + 1];	/* module name */
	ssize_t mi_minpsz;		/* min packet size accepted */
	ssize_t mi_maxpsz;		/* max packet size accepted */
	size_t mi_hiwat;		/* hi water mark */
	size_t mi_lowat;		/* lo water mark */
};

typedef struct sc_module_stat {
	long ms_pcnt;			/* calls to qi_putp() */
	long ms_scnt;			/* calls to qi_srvp() */
	long ms_ocnt;			/* calls to qi_qopen() */
	long ms_ccnt;			/* calls to qi_qclose() */
	long ms_acnt;			/* calls to qi_qadmin() */
	void *ms_xptr;			/* module private stats */
	short ms_xsize;			/* len of private stats */
	uint ms_flags;			/* bool stats -- for future use */
} sc_module_stat_t;

struct sc_mlist {
	long major;
	char name[FMNAMESZ + 1];
	struct sc_module_info mi[4];
	struct sc_module_stat ms[4];
};

struct sc_list {
	int sc_nmods;
	struct sc_mlist *sc_mlist;
};

#ifdef __LP64__
struct sc_module_info32 {
	u_int16_t mi_idnum;		/* module id number */
	char mi_idname[FMNAMESZ + 1];	/* module name */
	int32_t mi_minpsz;		/* min packet size accepted */
	int32_t mi_maxpsz;		/* max packet size accepted */
	u_int32_t mi_hiwat;		/* hi water mark */
	u_int32_t mi_lowat;		/* lo water mark */
};

struct sc_module_stat32 {
	int32_t ms_pcnt;		/* calls to qi_putp() */
	int32_t ms_scnt;		/* calls to qi_srvp() */
	int32_t ms_ocnt;		/* calls to qi_qopen() */
	int32_t ms_ccnt;		/* calls to qi_qclose() */
	int32_t ms_acnt;		/* calls to qi_qadmin() */
	u_int32_t ms_xptr;		/* module private stats */
	int16_t ms_xsize;		/* len of private stats */
	u_int32_t ms_flags;		/* bool stats -- for future use */
};

struct sc_mlist32 {
	int32_t major;
	char name[FMNAMESZ + 1];
	struct sc_module_info32 mi[4];
	struct sc_module_stat32 ms[4];
};

struct sc_list32 {
	int32_t sc_nmods;
	u_int32_t sc_mlist;
};
#endif				/* __LP64__ */

#endif				/* __SYS_STREAMS_SC_H__ */
