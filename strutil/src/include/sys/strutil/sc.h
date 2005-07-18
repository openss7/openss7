/*****************************************************************************

 @(#) $Id: sc.h,v 0.9.2.9 2005/07/18 12:38:48 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/07/18 12:38:48 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_STREAMS_SC_H__
#define __SYS_STREAMS_SC_H__

#ident "@(#) $RCSfile: sc.h,v $ $Name:  $($Revision: 0.9.2.9 $) $Date: 2005/07/18 12:38:48 $"

#ifndef __SYS_SC_H__
#warn "Do no include sys/streams/sc.h directly, include sys/sc.h instead."
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

#ifndef __KERNEL__
#undef  module_info
#define module_info smodule_info
struct module_info {
	ushort mi_idnum;		/* module id number */
	char *mi_idname;		/* module name */
	ssize_t mi_minpsz;		/* min packet size accepted *//* OSF/Mac OT: long */
	ssize_t mi_maxpsz;		/* max packet size accepted *//* OSF/Mac OT: long */
	size_t mi_hiwat;		/* hi water mark *//* OSF/Mac OT: ulong */
	size_t mi_lowat;		/* lo water mark *//* OSF/Mac OT: ulong */
};

typedef struct module_stat {
	long ms_pcnt;			/* calls to qi_putp() */
	long ms_scnt;			/* calls to qi_srvp() */
	long ms_ocnt;			/* calls to qi_qopen() */
	long ms_ccnt;			/* calls to qi_qclose() */
	long ms_acnt;			/* calls to qi_qadmin() */
	void *ms_xptr;			/* module private stats */
	short ms_xsize;			/* len of private stats */
	uint ms_flags;			/* bool stats -- for future use */
} module_stat_t;
#endif

struct sc_mlist {
	int major;
	struct module_info mi;
	struct module_stat ms;
};

struct sc_list {
	int sc_nmods;
	struct sc_mlist *sc_mlist;
};

#endif				/* __SYS_STREAMS_SC_H__ */
