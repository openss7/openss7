/*****************************************************************************

 @(#) $Id: sockio.h,v 1.1.2.2 2010-11-28 14:21:49 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2010-11-28 14:21:49 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sockio.h,v $
 Revision 1.1.2.2  2010-11-28 14:21:49  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:25:38  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __SYS_SOCKIO_H__
#define __SYS_SOCKIO_H__

/* This file can be processed with doxygen(1). */

#define SOCKIO ('s'<<8)

#define SIOCHIWAT	(SOCKIO|0)	/* set hi watermark */
#define SIOGHIWAT	(SOCKIO|1)	/* get hi watermark */
#define SIOCLOWAT	(SOCKIO|2)	/* set lo watermark */
#define SIOGLOWAT	(SOCKIO|3)	/* get lo watermark */

/* Linux sometimes defines these three... */

#ifndef SIOCATMARK
#define SIOCATMARK	(SOCKIO|7)	/* at oob mark */
#endif				/* SIOCATMARK */
#ifndef SIOCSPGRP
#define SIOCSPGRP	(SOCKIO|8)	/* set process group */
#endif				/* SIOCSPGRP */
#ifndef SIOCGPGRP
#define SIOCGPGRP	(SOCKIO|9)	/* get process group */
#endif				/* SIOCGPGRP */

#define SIOCPROTO	(SOCKIO|51)	/* link proto */
#define SIOCGETNAME	(SOCKIO|52)	/* getsockname */
#define SIOCGETPEER	(SOCKIO|53)	/* getpeername */
#define IF_UNITSEL	(SOCKIO|54)	/* set unit number */
#define SIOCXPROTO	(SOCKIO|55)	/* empty proto table */

#endif				/* __SYS_SOCKIO_H__ */
