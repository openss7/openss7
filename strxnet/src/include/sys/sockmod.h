/*****************************************************************************

 @(#) $Id: sockmod.h,v 0.9.2.2 2005/05/14 08:28:28 brian Exp $

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

 Last Modified $Date: 2005/05/14 08:28:28 $ by $Author: brian $

 *****************************************************************************/

#ifndef __SYS_SOCKMOD_H__
#define __SYS_SOCKMOD_H__

#ident "@(#) $RCSfile: sockmod.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

#define SOCKMOD_IOC_MAGIC	'I'

#define SIMOD ('I'<<8)

#define O_SI_GETUDATA	(SIMOD|101)
#define SI_SHUTDOWN	(SIMOD|102)
#define SI_LISTEN	(SIMOD|103)
#define SI_SETMYNAME	(SIMOD|104)
#define SI_SETPEERNAME	(SIMOD|105)
#define SI_GETINTRANSIT	(SIMOD|106)
#define SI_TCL_LINK	(SIMOD|107)
#define SI_TCL_UNLINK	(SIMOD|108)
#define SI_SOCKPARAMS	(SIMOD|109)
#define SI_GETUDATA	(SIMOD|110)

#if 0
/* These need to be defined in the library. */
#define S_WINFO			0x01000 /* Waiting for T_INFO_ACK */
#define S_WRDISABLE		0x02000 /* Write service queue disabled. */
#define S_WUNBIND		0x04000 /* Waiting for T_OK_ACK (for T_UNBIND_REQ) */
#define S_RBLOCKED		0x08000 /* Processing T_DISCON_IND draining messages */
#define S_IGNORE_FLOW		0x08000 /* Processing T_DISCON_IND draining messages */
#define S_WBLOCKED		0x10000 /* Sent T_UNBIND_REQ, waiting T_OK_ACK on receiving T_DISCON_IND */
#define S_WUNBIND_DISCON	0x10000 /* Sent T_UNBIND_REQ, waiting T_OK_ACK on receiving T_DISCON_IND */

#define SC_WCLOSE		0x20000 /* Wait before closing (esballoc'ed msgs oustanding) */
#define SC_NOENABLE		0x40000
#define S_INET_LISTEN		0x40000 /* Processing PF_INET SI_LISTEN */
#define SC_KEEPOPEN		0x80000
#endif

#define SOCKMOD_TIMEOUT	(-1)

#if 0
/* These need to be defined in the library. */
struct _si_user {
	struct _si_user *next;
	struct _si_user *prev;
	int fd;
	int ctlsize;
	char *ctlbuf;
	struct si_udata udata;
	int flags;
	int fflags;
	pthread_rwlock_t lock;
};

#define S_SIGIO		0x1 /* User has SIGIO enabled. */
#define S_SIGURG	0x2 /* User has SIGURG enabled. */


extern struct _si_user *_s_checkfd(int fd);
extern struct _si_user *_s_socreate(int domain, int type, int famiy);
extern void _s_aligned_copy(char *, int, int, char *, int *);
extern struct netconfig *_s_match(int domain, int type, int family, void **handle);
extern int _s_sosend(struct _si_user *user, struct msghdr *msg, int flags);
extern int _s_soreceive(struct _si_user *user, struct msghdr *msg, int flags);
extern int _s_synch(struct _si_user *user);
extern int _s_is_ok(struct _si_user *user, long, struct strbuf *);
extern int _s_do_ioctl(int fd, char *dp, int len, int, int *rval);
extern int _s_min(int a, int b);
extern int _s_max(int a, int b);
extern void _s_close(struct _si_user *user);
extern int _s_getfamily(struct _si_user *user);
extern int _s_uxpathlen(struct sockaddr_un *addr);
extern void _s_blockallsignals(sigset_t *sigset);
extern void _s_restoresigmask(sigset_t *sigset);
extern int _s_cbuf_alloc(struct _si_user *user, char **ptr);
#endif

struct si_sockparams {
	int sp_family;
	int sp_type;
	int sp_protocol;
};

/*
 *  In the old sockmod it appears that the domain, type and protocol where handled in the user-space
 *  library, whereas in the later sockmod, the docmain, type and protocol are handled in the kernel.
 */

struct o_si_udata {
	int tidusize;
	int addrsize;
	int optsize;
	int etsdusize;
	int servtype;
	int so_state;
	int so_options;
	int tsdusize;
};

struct si_udata {
	int tidusize;
	int addrsize;
	int optsize;
	int etsdusize;
	int servtype;
	int so_state;
	int so_options;
	int tsdusize;
	struct si_sockparams sockparams;
};

#if 0
#define O_SI_GETUDATA		_IORW(SOCKMOD_IOC_MAGIC, 101, struct o_si_udata)
#define SI_SHUTDOWN		_IORW(SOCKMOD_IOC_MAGIC, 102, void)
#define SI_LISTEN		_IORW(SOCKMOD_IOC_MAGIC, 103, void)
#define SI_SETMYNAME		_IORW(SOCKMOD_IOC_MAGIC, 104, void)
#define SI_SETPEERNAME		_IORW(SOCKMOD_IOC_MAGIC, 105, void)
#define SI_GETINTRANSIT		_IORW(SOCKMOD_IOC_MAGIC, 106, void)
#define SI_TCL_LINK		_IORW(SOCKMOD_IOC_MAGIC, 107, void)
#define SI_TCL_UNLINK		_IORW(SOCKMOD_IOC_MAGIC, 108, void)
#define SI_SOCKPARAMS		_IORW(SOCKMOD_IOC_MAGIC, 109, struct si_sockparams)
#define SI_GETUDATA		_IORW(SOCKMOD_IOC_MAGIC, 110, struct si_udata)
#endif

#if 0
/* This needs to be defined in sockmod.c and only in the kernel. */
struct so_so {
	long flags;
	long closeflags;
	queue_t *rdq;
	mblk_t *iocsae;
	struct t_info tp_info;
	struct netbuf raddr;
	struct netbuf laddr;
	struct ux_extaddr lux_dev;
	struct ux_extaddr rux_dev;
	int so_error;
	mblk_t *oob;
	struct so_so *so_conn;
	mblk_t *consave;
	struct si_udata udata;
	int so_option;
	mblk_t *bigmsg;
	struct so_ux so_ux;
	int hasoutofband;
	mblk_t *urg_msg;
	int sndbuf;
	int rcvbuf;
	int sndlowat;
	int rcvlowat;
	int linger;
	int sndtimeo;
	int rcvtimeo;
	int prototype;
	int so_cbacks_outstanding;
	struct so_so *so_next;
	struct so_so **so_ptpn;
	int wbufcid;
	int rbufcid;
	int wtimeoutid;
	int rtimeoutid;
	queue_t *so_driverq;
	int so_isaccepting;
	struct so_so *so_acceptor;
	int so_id;
	int so_cbacks_inprogress;
};
#endif

#endif				/* __SYS_SOCKMOD_H__ */
