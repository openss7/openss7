/*****************************************************************************

 @(#) $Id: strsad.h,v 0.9.2.10 2006/12/18 10:08:59 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/12/18 10:08:59 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: strsad.h,v $
 Revision 0.9.2.10  2006/12/18 10:08:59  brian
 - updated headers for release

 *****************************************************************************/

#ifndef __LOCAL_STRSAD_H__
#define __LOCAL_STRSAD_H__

#ident "@(#) $RCSfile: strsad.h,v $ $Name:  $($Revision: 0.9.2.10 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#ifndef BIG_STATIC_STH
#define BIG_STATIC_STH
#endif

// defined in sys/strconf.h
//extern int autopush_add(struct strapush *sap);
//extern int autopush_del(struct strapush *sap);
//extern int autopush_vml(struct str_mlist *smp, int nmods);
//extern struct strapush *autopush_find(dev_t dev);
//extern int apush_set(struct strapush *sap);
//extern int apush_get(struct strapush *sap);
//extern int apush_vml(struct str_list *slp);

BIG_STATIC_STH int streams_fastcall autopush(struct stdata *sd, struct cdevsw *cdev, dev_t *devp, int oflag, int sflag,
		    cred_t *crp);

#endif				/* __LOCAL_STRSAD_H__ */
