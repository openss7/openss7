/*****************************************************************************

 @(#) src/include/sys/sockio.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

#ifndef __SYS_SOCKIO_H__
#define __SYS_SOCKIO_H__

/* This file can be processed with doxygen(1). */

#ifdef _IOW

#define SIOCSHIWAT	_IOW('s', 0, int)
#define SIOCGHIWAT	_IOR('s', 1, int)
#define SIOCSLOWAT	_IOW('s', 2, int)
#define SIOCGLOWAT	_IOR('s', 3, int)

#ifndef SIOCATMARK
#define SIOCATMARK	_IOR('s', 7, int)
#endif				/* SIOCATMARK */
#ifndef SIOCSPGRP
#define SIOCSPGRP	_IOW('s', 8, int)
#endif				/* SIOCSPGRP */
#ifndef SIOCGPGRP
#define SIOCGPGRP	_IOR('s', 9, int)
#endif				/* SIOCGPGRP */

/* STREAMS based socket emulation */
#define SIOCPROTO	_IOW('s', 51, struct socknewproto) /* link proto */
#define SIOCGETNAME	_IOR('s', 52, struct sockaddr)	/* getsockname */
#define SIOCGETPEER	_IOR('s', 53, struct sockaddr)	/* getpeername */
#define IF_UNITSEL	_IOW('s', 54, int)	/* set unit number */
#define SIOCXPROTO	_IO('s', 55)	/* empty proto table */

#ifndef SIOCSIFADDR

#define SIOCSIFADDR	_IOW('i', 12, struct ifreq)
#define SIOCGIFADDR	_IOWR('i', 13, struct ifreq)
#define SIOCSIFDSTADDR	_IOW('i', 14, struct ifreq)
#define SIOCGIFDSTADDR	_IOWR('i', 15, struct ifreq)
#define SIOCSIFFLAGS	_IOW('i', 16, struct ifreq)
#define SIOCGIFFLAGS	_IOWR('i', 17, struct ifreq)
#define SIOCSIFMEM	_IOW('i', 18, struct ifreq)
#define SIOCGIFMEM	_IOWR('i', 19, struct ifreq)

#define O_SIOCGIFCONF	_IOWRN('i', 20, 8)

#define SIOCSIFMTU	_IOW('i', 21, struct ifreq)
#define SIOCGIFMTU	_IOWR('i', 22, struct ifreq)

/* from 4.3BSD */
#define SIOCGIFBRDADDR	_IOWR('i', 23, struct ifreq)
#define SIOCSIFBRDADDR	_IOW('i', 24, struct ifreq)
#define SIOCGIFNETMASK	_IOWR('i', 25, struct ifreq)
#define SIOCSIFNETMASK	_IOW('i', 26, struct ifreq)
#define SIOCGIFMETRIC	_IOWR('i', 27, struct ifreq)
#define SIOCSIFMETRIC	_IOW('i', 28, struct ifreq)

#if 0
#define SIOCSARP	_IOW('i', 30, struct arpreq)
#define SIOCGARP	_IOWR('i', 31, struct arpreq)
#define SIOCDARP	_IOW('i', 32, struct arpreq)
#endif

#define SIOCUPPER	_IOW('i', 40, struct ifreq)
#define SIOCLOWER	_IOW('i', 41, struct ifreq)
#define SIOCSETSYNC	_IOW('i', 44, struct ifreq)
#define SIOCGETSYNC	_IOWR('i', 45, struct ifreq)
#define SIOCSSDSTATS	_IOWR('i', 46, struct ifreq)
#define SIOCSSESTATS	_IOWR('i', 47, struct ifreq)

#define SIOCSPROMISC	_IOW('i', 48, int)

#define SIOCADDMULTI	_IOW('i', 49, struct ifreq)
#define SIOCDELMULTI	_IOW('i', 50, struct ifreq)

#define SIOCGIFHWADDR	_IOWR('i', 185, struct ifreq)

#endif				/* SIOCSIFADDR */

#define SIOCIFDETACH	_IOW('i', 56, struct ifreq)
#define SIOCGENPSTATS	_IOWR('i', 57, struct ifreq)
#define SIOCX25XMT	_IOWR('i', 59, struct ifreq)	/* start slp proc in x25if */
#define SIOCX25RCV	_IOWR('i', 60, struct ifreq)	/* start slp proc in x24if */
#define SIOCX25TBL	_IOWR('i', 61, struct ifreq)	/* xfer lun table to kernel */
#define SIOCSLGETREQ	_IOWR('i', 71, struct ifreq)	/* wait switched SLIP req */
#define SIOCSLSTAT	_IOWR('i', 72, struct ifreq)	/* pass SLIIP info to kernel */

#ifndef SIOCSIFNAME
#define SIOCSIFNAME	_IOW('i', 73, struct ifreq)	/* set interface name */
#endif
#define SIOCGENADDR	_IOWR('i', 85, struct ifreq)	/* get ethernet address */
#define SIOCGIFNUM	_IOR('i', 87, int)	/* get number of ifs */

#define SIOCGIFMUXID	_IOWR('i', 88, struct ifreq)
#define SIOCSIFMUXID	_IOW('i', 89, struct ifreq)

#ifndef SIOCGIFINDEX
#define SIOCGIFINDEX	_IOWR('i', 90, struct ifreq)
#endif
#define SIOCSIFINDEX	_IOW('i', 91, struct ifreq)
#ifndef SIOCGIFCONF
#define SIOCGIFCONF	_IOWRN('i', 92, 8)	/* get if list */
#endif

#define SIOCLIFREMOVEIF	_IOW('i', 110, struct lifreq)
#define SIOCLIFADDIF	_IOWR('i', 111, struct lifreq)

#define SIOCSLIFADDR	_IOW('i', 112, struct lifreq)
#define SIOCGLIFADDR	_IOWR('i', 113, struct lifreq)
#define SIOCSLIFDSTADDR	_IOW('i', 114, struct lifreq)
#define SIOCGLIFDSTADDR	_IOWR('i', 115, struct lifreq)
#define SIOCSLIFFLAGS	_IOW('i', 116, struct lifreq)
/* one for the channel */
#define SIOCGLIFFLAGS	_IOWR('i', 117, struct lifreq)

#define O_SIOCGLIFCONF	_IOWRN('i', 120, 16)
#define SIOCGLIFCONF	_IOWR('i', 120, struct lifconf)
#define SIOCSLIFMTU	_IOW('i', 121, struct lifreq)
#define SIOCGLIFMTU	_IOWR('i', 122, struct lifreq)
#define SIOCGLIFBRDADDR	_IOWR('i', 123, struct lifreq)
#define SIOCSLIFBRDADDR	_IOW('i', 124, struct lifreq)
#define SIOCGLIFNETMASK	_IOWR('i', 125, struct lifreq)
#define SIOCSLIFNETMASK	_IOW('i', 126, struct lifreq)
#define SIOCGLIFMETRIC	_IOWR('i', 127, struct lifreq)
#define SIOCSLIFMETRIC	_IOW('i', 128, struct lifreq)
#define SIOCSLIFNAME	_IOWR('i', 129, struct lifreq)
#define SIOCGLIFNUM	_IOWR('i', 130, struct lifnum)
#define SIOCGLIFMUXID	_IOWR('i', 131, struct lifreq)
#define SIOCSLIFMUXID	_IOW('i', 132, struct lifreq)

#define SIOCGLIFINDEX	_IOWR('i', 133, struct lifreq)
#define SIOCSLIFINDEX	_IOW('i', 134, struct lifreq)

#define SIOCSLIFTOKEN	_IOW('i', 135, struct lifreq)
#define SIOCGLIFTOEKN	_IOWR('i', 136, struct lifreq)

#define SIOCSLIFSUBNET	_IOW('i', 137, struct lifreq)
#define SIOCGLIFSUBNET	_IOWR('i', 138, struct lifreq)

#define SIOCSLIFLNKINFO	_IOW('i', 139, struct lifreq)
#define SIOCGLIFLNKINFO	_IOWR('i', 140, struct lifreq)

#define SIOCLIFDELND	_IOW('i', 141, struct lifreq)
#define SIOCLIFGETND	_IOWR('i', 142, struct lifreq)
#define SIOCLIFSETND	_IOW('i', 143, struct lifreq)

#define SIOCGLIFZONE	_IOWR('i', 170, struct lifreq)
#define SIOCSLIFZONE	_IOW('i', 171, struct lifreq)

#define SIOCGLIFUSESRC	_IOWR('i', 175, struct lifreq)
#define SIOCSLIFUSESRC	_IOW('i', 171, struct lifreq)

#define SIOCGLIFDADSATE	_IOWR('i', 190, struct lifreq)

#define SIOCSLIFPREFIX	_IOWR('i', 191, struct lifreq)

#define SIOCGLIFHWADDR	_IOWR('i', 192, struct lifreq)

#else				/* !defined _IOW */

#define SOCKIO ('s'<<8)

/* These are from 4.4BSD-Lite. */
#define SIOCSHIWAT	(SOCKIO|0)	/* (int) set hi watermark */
#define SIOCGHIWAT	(SOCKIO|1)	/* (int) get hi watermark */
#define SIOCSLOWAT	(SOCKIO|2)	/* (int) set lo watermark */
#define SIOCGLOWAT	(SOCKIO|3)	/* (int) get lo watermark */

/* Linux sometimes defines these three... */

#ifndef SIOCATMARK
#define SIOCATMARK	(SOCKIO|7)	/* (int) at oob mark */
#endif				/* SIOCATMARK */
#ifndef SIOCSPGRP
#define SIOCSPGRP	(SOCKIO|8)	/* (int) set process group */
#endif				/* SIOCSPGRP */
#ifndef SIOCGPGRP
#define SIOCGPGRP	(SOCKIO|9)	/* (int) get process group */
#endif				/* SIOCGPGRP */

/* STREAMS based socket emulation */
#define SIOCPROTO	(SOCKIO|51)	/* (socknewproto) link proto */
#define SIOCGETNAME	(SOCKIO|52)	/* (sockaddr) getsockname */
#define SIOCGETPEER	(SOCKIO|53)	/* (sockaddr) getpeername */
#define IF_UNITSEL	(SOCKIO|54)	/* (int) set unit number */
#define SIOCXPROTO	(SOCKIO|55)	/* () empty proto table */

#define SIOC ('i'<<8)

#endif				/* _IOW */

#endif				/* __SYS_SOCKIO_H__ */
