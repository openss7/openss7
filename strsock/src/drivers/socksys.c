/*****************************************************************************

 @(#) $RCSfile: socksys.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/03 13:53:09 $

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

 Last Modified $Date: 2006/10/03 13:53:09 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: socksys.c,v $
 Revision 0.9.2.3  2006/10/03 13:53:09  brian
 - changes to pass make check target
 - added some package config.h files
 - removed AUTOCONFIG_H from Makefile.am's
 - source code changes for compile
 - added missing manual pages
 - renamed conflicting manual pages
 - parameterized include Makefile.am
 - updated release notes

 Revision 0.9.2.2  2006/09/29 11:50:56  brian
 - libtool library tweaks in Makefile.am
 - better rpm spec handling in *.spec.in
 - added AC_LIBTOOL_DLOPEN to configure.ac
 - updated some copyright headers
 - rationalized item in two packages
 - added manual pages, drivers and modules to new strtty package

 Revision 0.9.2.1  2006/09/01 08:55:42  brian
 - added headers and working up code

 *****************************************************************************/

#ident "@(#) $RCSfile: socksys.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/03 13:53:09 $"

static char const ident[] = "$RCSfile: socksys.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/03 13:53:09 $";

/*
 *  A Socket System (SOCKSYS) Driver.
 *
 *  This driver provides a sockfs(5) based approach to providing sockets interface to TPI streams.
 *
 *  Discussion:  This driver implements one of three approaches to providing sockets for STREAMS.
 *  Under this approach there is a user-space library (libsocket(3)) and a cooperating socket system
 *  driver (socksys(4)).  When a socket is opened using the socket(3) or socketpair(3) library
 *  subroutines, the library opens the socksys(4) driver and uses the SIOCSOCKSYS input-output
 *  control with the SO_SOCKET or SO_SOCKPAIR command to open a socket (or pair of sockets) of the
 *  appropriate type.  The socksys(4) driver knows what driver to open internally according to the
 *  population of an internall mapping table performed from user space during system intialization
 *  from the /etc/sock2path configuration file using the soconfig(8) utility and the SIOCPROTO and
 *  SIOCXPROTO input-output controls.  The returned file descriptors represent files within the
 *  sockfs(5) filesystem, and are true sockets, at least down to the struct socket.  All other
 *  socket API calls use the libc sockets system call interface.  The library also supports the
 *  remaining SIOCSOCKSYS commands for backward compatibility (to what!?).
 *
 *  Ok, rather than transforming Streams into Sockets here, the SO_SOCKET and SO_SOCKETPAIR calls
 *  will return either a Stream (if there is a Stream defined for the call parameters) or a Socket
 *  (if there is no Stream defined for the call parameters).
 *
 *  Because, paticularly LiS, cannot execute the topmost module put procedure in user context, calls
 *  that are supposed to return file descriptors cannot.  Therefore, these calls will have to return
 *  0 and the send an M_PASSFP message upstream.  The library will have to perform an I_RECVFD if
 *  the ioctl is successful, to obtain the file descriptor(s).  SO_SOCKET returns 1 file descriptor
 *  on success.  SO_ACCEPT returns 1 file descriptor on success.  SO_SOCKPAIR returns 2 file
 *  descriptors on success.  If these file descriptors are Streams, it is the responsibility of the
 *  library to push "sockmod" to transform them into sockets.
 */

#include <sys/os7/compat.h>

#ifdef LINUX
#undef ASSERT

#include <linux/bitops.h>
#include <linux/interrupt.h>
#include <linux/socket.h>
#endif				/* LINUX */

#include <sys/socksys.h>

#define SOCKSYS_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SOCKSYS_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define SOCKSYS_REVISION	"OpenSS7 $RCSfile: socksys.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/10/03 13:53:09 $"
#define SOCKSYS_DEVICE		"SVR 4.2 STREAMS Socket System Driver (SOCKSYS)"
#define SOCKSYS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SOCKSYS_LICENSE		"GPL"
#define SOCKSYS_BANNER		SOCKSYS_DESCRIP		"\n" \
				SOCKSYS_COPYRIGHT	"\n" \
				SOCKSYS_REVISION	"\n" \
				SOCKSYS_DEVICE		"\n" \
				SOCKSYS_CONTACT		"\n"
#define SOCKSYS_SPLASH		SOCKSYS_DEVICE		" - " \
				SOCKSYS_REVISION

#ifdef CONFIG_STREAMS_SOCKSYS_MODULE
MODULE_AUTHOR(SOCKSYS_CONTACT);
MODULE_DESCRIPTION(SOCKSYS_DESCRIP);
MODULE_SUPPORTED_DEVICE(SOCKSYS_DEVICE);
MODULE_LICENSE(SOCKSYS_LICENSE);
#endif				/* CONFIG_STREAMS_SOCKSYS_MODULE */

#ifdef LFS
#define SOCKSYS_DRV_ID		CONFIG_STREAMS_SOCKSYS_MODID
#define SOCKSYS_DRV_NAME	CONFIG_STREAMS_SOCKSYS_NAME
#define SOCKSYS_CMAJORS		CONFIG_STREAMS_SOCKSYS_NMAJORS
#define SOCKSYS_CMAJOR_0	CONFIG_STREAMS_SOCKSYS_MAJOR
#define SOCKSYS_UNITS		CONFIG_STREAMS_SOCKSYS_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_SOCKSYS_MODID));
MODULE_ALIAS("streams-driver-socksys");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_SOCKSYS_MAJOR));
MODULE_ALIAS("/dev/streams/socksys");
MODULE_ALIAS("/dev/streams/socksys/*");
MODULE_ALIAS("/dev/streams/clone/socksys");
#endif
MODULE_ALIAS("char-major-" __stringify(SOCKSYS_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(SOCKSYS_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(SOCKSYS_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/socksys");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

/*
 *  STREAMS Definitions
 *  ==========================================================================
 */

#define DRV_ID		SOCKSYS_DRV_ID
#define DRV_NAME	SOCKSYS_DRV_NAME
#define CMAJORS		SOCKSYS_CMAJORS
#define CMAJOR_0	SOCKSYS_CMAJOR_0
#define UNITS		SOCKSYS_UNITS
#ifdef MODULE
#define DRV_BANNER	SOCKSYS_BANNER
#else				/* MODULE */
#define DRV_BANNER	SOCKSYS_SPLASH
#endif				/* MODULE */

STATIC struct module_info socksys_minfo = {
	.mi_idnum = DRV_ID,		/* Module ID number */
	.mi_idname = DRV_NAME,		/* Module name */
	.mi_minpsz = STRMINPSZ,		/* Min packet size accepted */
	.mi_maxpsz = STRMAXPSZ,		/* Max packet size accepted */
	.mi_hiwat = STRHIGH,		/* Hi water mark */
	.mi_lowat = STRLOW,		/* Lo water mark */
};

STATIC struct module_stat socksys_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

STATIC streamscall int socksys_qopen(queue_t *, dev_t *, int, int, cred_t *);
STATIC streamscall int socksys_qclose(queue_t *, int, cred_t *);

STATIC streamscall int socksys_put(queue_t *, mblk_t *);

STATIC struct qinit socksys_qinit = {
	.qi_putp = socksys_put,	/* Put procedure (messgae from above or below) */
	.qi_qopen = socksys_qopen,	/* Each open */
	.qi_qclose = socksys_qclose,	/* Last close */
	.qi_minfo = &socksys_minfo,	/* Module information */
	.qi_mstat = &socksys_mstat,	/* Module statistics */
};

MODULE_STATIC struct streamtab socksys_info = {
	.st_rdinit = &socksys_qinit,	/* Upper read queue */
	.st_wrinit = &socksys_qinit,	/* Upper write queue */
};

/*
 *  Locking
 */

/* Must always be bottom-half versions to avoid lock badness.  But give these
 * different names to avoid conflict with generic definitions.  */

//#if defined CONFIG_STREAMS_NOIRQ || defined CONFIG_STREAMS_TEST
#if 1

#define spin_lock_str2(__lkp, __flags) \
	do { (void)__flags; spin_lock_bh(__lkp); } while (0)
#define spin_unlock_str2(__lkp, __flags) \
	do { (void)__flags; spin_unlock_bh(__lkp); } while (0)
#define write_lock_str2(__lkp, __flags) \
	do { (void)__flags; write_lock_bh(__lkp); } while (0)
#define write_unlock_str2(__lkp, __flags) \
	do { (void)__flags; write_unlock_bh(__lkp); } while (0)
#define read_lock_str2(__lkp, __flags) \
	do { (void)__flags; read_lock_bh(__lkp); } while (0)
#define read_unlock_str2(__lkp, __flags) \
	do { (void)__flags; read_unlock_bh(__lkp); } while (0)
#define local_save_str2(__flags) \
	do { (void)__flags; local_bh_disable(); } while (0)
#define local_restore_str2(__flags) \
	do { (void)__flags; local_bh_enable(); } while (0)

#else

#define spin_lock_str2(__lkp, __flags) \
	spin_lock_irqsave(__lkp, __flags)
#define spin_unlock_str2(__lkp, __flags) \
	spin_unlock_irqrestore(__lkp, __flags)
#define write_lock_str2(__lkp, __flags) \
	write_lock_irqsave(__lkp, __flags)
#define write_unlock_str2(__lkp, __flags) \
	write_unlock_irqrestore(__lkp, __flags)
#define read_lock_str2(__lkp, __flags) \
	read_lock_irqsave(__lkp, __flags)
#define read_unlock_str2(__lkp, __flags) \
	read_unlock_irqrestore(__lkp, __flags)
#define local_save_str2(__flags) \
	local_irq_save(__flags)
#define local_restore_str2(__flags) \
	local_irq_restore(__flags)

#endif


/*
 *  Primary data structures.
 */

#ifdef __LP64__
#  undef  WITH_32BIT_CONVERSION
#  define WITH_32BIT_CONVERSION 1
#endif

/* private structures */
struct ssys {
	struct ssys *next;
	struct ssys **prev;
	struct {
		major_t cmajor;
		minor_t cminor;
	} dev;
	int assigned;
	int iocstate;
	int transparent;
};

#define SOCKSYS_PRIV(__q) ((struct ssys *)((__q)->q_ptr))

static kmem_cache_t *ssys_priv_cachep = NULL;

static __unlikely int
ssys_alloc_priv(queue_t *q, struct ssys **sp, int type, dev_t *devp, cred_t *crp)
{
	struct ssys *ss;

	if ((ss = kmem_cache_alloc(ssys_priv_cachep, SLAB_ATOMIC))) {
		bzero(ss, sizeof(*ss));
		ss->dev.cmajor = getmajor(*devp);
		ss->dev.cminor = getminor(*devp);
		ss->assigned = 0;
		ss->iocstate = 0;
		ss->transparent = 0;
		if ((ss->next = *sp))
			ss->next->prev = &ss->next;
		ss->prev = sp;
	}
	return (ss != NULL);
}

#if 0
/*
 * This is not a good way to do things.  Transforming the Stream with a sockmod(4) approach is
 * better.  This also relies on the fact that the put procedure of the socksys(4) must execute in
 * user context.  Unfortunately, this is not always the case for LiS-2.18 derived implementations.
 * This will probably not work with LiS.  Do not use LiS.  Use Linux Fast-STREAMS instead.  In fact,
 * I cannot see how this will work, so strap it out for now...
 */

long
tpi_accept(int fd, struct sockaddr *addr, int *alen)
{
	return (-ENOTSOCK);
}

long
tpi_bind(int fd, struct sockaddr *addr, int alen)
{
	return (-ENOTSOCK);
}

long
tpi_connect(int fd, struct sockaddr *addr, int alen)
{
	return (-ENOTSOCK);
}

long
tpi_getpeername(int fd, struct sockaddr *addr, int *alen)
{
	return (-ENOTSOCK);
}

long
tpi_getsockname(int fd, struct sockaddr *addr, int *alen)
{
	return (-ENOTSOCK);
}

long
tpi_getsockopt(int fd, int level, int name, char *value, int *olen)
{
	return (-ENOTSOCK);
}

long
tpi_listen(int fd, int backlog)
{
	return (-ENOTSOCK);
}

long
tpi_recv(int fd, void *ubuf, size_t size, unsigned flags)
{
	return (-ENOTSOCK);
}

long
tpi_recvfrom(int fd, void *ubuf, size_t size, unsigned flags, struct sockaddr *addr, int *alen)
{
	return (-ENOTSOCK);
}

long
tpi_send(int fd, void *buff, size_t len, unsigned flags)
{
	return (-ENOTSOCK);
}

long
tpi_sendto(int fd, void *buff, size_t len, unsigned flags, struct sockaddr *addr, int alen)
{
	return (-ENOTSOCK);
}

long
tpi_setsockopt(int fd, int level, int name, char *value, int olen)
{
	return (-ENOTSOCK);
}

long
tpi_shutdown(int fd, int how)
{
	return (-ENOTSOCK);
}

long
tpi_recvmsg(int fd, struct msghdr *msg, unsigned int flags)
{
	return (-ENOTSOCK);
}

long
tpi_sendmsg(int fd, struct msghdr *msg, unsigned flags)
{
	return (-ENOTSOCK);
}
#endif

int
so_socksys(struct socksysreq *req)
{
	int err = -EINVAL;
	int cmd = req->args[0];

	if ((1 << cmd) & ((1 << SO_ACCEPT) | (1 << SO_BIND) | (1 << SO_CONNECT) |
			  (1 << SO_GETPEERNAME) | (1 << SO_GETSOCKNAME) | (1 << SO_GETSOCKOPT) |
			  (1 << SO_LISTEN) | (1 << SO_RECV) | (1 << SO_RECVFROM) | (1 << SO_SEND) |
			  (1 << SO_SENDTO) | (1 << SO_SETSOCKOPT) | (1 << SO_SHUTDOWN) |
			  (1 << SO_RECVMSG) | (1 << SO_SENDMSG))) {
		int fd = req->args[1];

		(void) fd;
		/* These are all socket related and accept a file (socket) descriptor as their
		   first argument.  In situations where we are incapable of providing back a real
		   socket, we must here first distinguish if the file descriptor corresponds to a
		   socket or a stream. */
#if 0
		if (it_is_a_socket) {
#endif
			/* In this case, we have a real socket from the operating system's
			   perspective and we can simply pass the arguments to the appropriate
			   system call. */
#if 0
			switch (cmd) {
			case SO_ACCEPT:
				/* FIXME: 32/64 conversion */
				err = sys_accept(fd, (struct sockaddr *) req->args[2],
						 req->args[3]);
				break;
			case SO_BIND:
				/* FIXME: 32/64 conversion */
				err = sys_bind(fd, (struct sockaddr *) req->args[2], req->args[3]);
				break;
			case SO_CONNECT:
				/* FIXME: 32/64 conversion */
				err = sys_connect(fd, (struct sockaddr *) req->args[2],
						  req->args[3]);
				break;
			case SO_GETPEERNAME:
				/* FIXME: 32/64 conversion */
				err = sys_getpeername(fd, (struct sockaddr *) req->args[2],
						      (int *) req->args[3]);
				break;
			case SO_GETSOCKNAME:
				/* FIXME: 32/64 conversion */
				err = sys_getsockname(fd, (struct sockaddr *) req->args[2],
						      (int *) req->args[3]);
				break;
			case SO_GETSOCKOPT:
				/* FIXME: 32/64 conversion */
				err = sys_getsockopt(fd, req->args[2], req->args[3],
						     (char *) req->args[4], (int *) req->args[5]);
				break;
			case SO_LISTEN:
				/* FIXME: 32/64 conversion */
				err = sys_listen(fd, req->args[2]);
				break;
			case SO_RECV:
				/* FIXME: 32/64 conversion */
				err = sys_recv(fd, (void *) req->args[2], req->args[3],
					       req->args[4]);
				break;
			case SO_RECVFROM:
				/* FIXME: 32/64 conversion */
				err = sys_recvfrom(fd, (void *) req->args[2], req->args[3],
						   req->args[4], (struct sockaddr *) req->args[5],
						   (int *) req->args[6]);
				break;
			case SO_SEND:
				/* FIXME: 32/64 conversion */
				err = sys_send(fd, (void *) req->args[2], req->args[3],
					       req->args[4]);
				break;
			case SO_SENDTO:
				/* FIXME: 32/64 conversion */
				err = sys_sendto(fd, (void *) req->args[2], req->args[3],
						 req->args[4], (struct sockaddr *) req->args[5],
						 req->args[6]);
				break;
			case SO_SETSOCKOPT:
				/* FIXME: 32/64 conversion */
				err = sys_setsockopt(fd, req->args[2], req->args[3],
						     (char *) req->args[4], req->args[5]);
				break;
			case SO_SHUTDOWN:
				/* FIXME: 32/64 conversion */
				err = sys_shutdown(fd, req->args[2]);
				break;
			case SO_RECVMSG:
				/* FIXME: 32/64 conversion */
				err = sys_recvmsg(fd, (struct msghdr *) req->args[2], req->args[3]);
				break;
			case SO_SENDMSG:
				/* FIXME: 32/64 conversion */
				err = sys_sendmsg(fd, (struct msghdr *) req->args[2], req->args[3]);
				break;
			}
#endif
#if 0
		} else {
			/* In this case, we do not have a real socket, but have a TPI stream from
			   the operating system's perspective, and we will directly call the
			   associated TPI routine. */
			switch (cmd) {
			case SO_ACCEPT:
				/* FIXME: 32/64 conversion */
				err = tpi_accept(fd, (struct sockaddr *) req->args[2],
						 req->args[3]);
				break;
			case SO_BIND:
				/* FIXME: 32/64 conversion */
				err = tpi_bind(fd, (struct sockaddr *) req->args[2], req->args[3]);
				break;
			case SO_CONNECT:
				/* FIXME: 32/64 conversion */
				err = tpi_connect(fd, (struct sockaddr *) req->args[2],
						  req->args[3]);
				break;
			case SO_GETPEERNAME:
				/* FIXME: 32/64 conversion */
				err = tpi_getpeername(fd, (struct sockaddr *) req->args[2],
						      (int *) req->args[3]);
				break;
			case SO_GETSOCKNAME:
				/* FIXME: 32/64 conversion */
				err = tpi_getsockname(fd, (struct sockaddr *) req->args[2],
						      (int *) req->args[3]);
				break;
			case SO_GETSOCKOPT:
				/* FIXME: 32/64 conversion */
				err = tpi_getsockopt(fd, req->args[2], req->args[3],
						     (char *) req->args[4], (int *) req->args[5]);
				break;
			case SO_LISTEN:
				/* FIXME: 32/64 conversion */
				err = tpi_listen(fd, req->args[2]);
				break;
			case SO_RECV:
				/* FIXME: 32/64 conversion */
				err = tpi_recv(fd, (void *) req->args[2], req->args[3],
					       req->args[4]);
				break;
			case SO_RECVFROM:
				/* FIXME: 32/64 conversion */
				err = tpi_recvfrom(fd, (void *) req->args[2], req->args[3],
						   req->args[4], (struct sockaddr *) req->args[5],
						   (int *) req->args[6]);
				break;
			case SO_SEND:
				/* FIXME: 32/64 conversion */
				err = tpi_send(fd, (void *) req->args[2], req->args[3],
					       req->args[4]);
				break;
			case SO_SENDTO:
				/* FIXME: 32/64 conversion */
				err = tpi_sendto(fd, (void *) req->args[2], req->args[3],
						 req->args[4], (struct sockaddr *) req->args[5],
						 req->args[6]);
				break;
			case SO_SETSOCKOPT:
				/* FIXME: 32/64 conversion */
				err = tpi_setsockopt(fd, req->args[2], req->args[3],
						     (char *) req->args[4], req->args[5]);
				break;
			case SO_SHUTDOWN:
				/* FIXME: 32/64 conversion */
				err = tpi_shutdown(fd, req->args[2]);
				break;
			case SO_RECVMSG:
				/* FIXME: 32/64 conversion */
				err = tpi_recvmsg(fd, (struct msghdr *) req->args[2], req->args[3]);
				break;
			case SO_SENDMSG:
				/* FIXME: 32/64 conversion */
				err = tpi_sendmsg(fd, (struct msghdr *) req->args[2], req->args[3]);
				break;
			}
		}
#endif
	}
	if ((1 << cmd) & ((1 << SO_SOCKET) | (1 << SO_SOCKPAIR) | (1 << SO_SELECT) |
			  (1 << SO_GETIPDOMAIN) | (1 << SO_SETIPDOMAIN) | (1 << SO_ADJTIME) |
			  (1 << SO_SETREUID) | (1 << SO_SETREGID) | (1 << SO_GETTIME) |
			  (1 << SO_SETTIME) | (1 << SO_GETITIMER) | (1 << SO_SETITIMER))) {
		/* These are BSD compatibiltiy functions and are how we create sockets in the first
		   place.  The BSD compatibility functions all have system calls in Linux, but we
		   provide them for backward compatibility (to what!?). */
#if 0
		switch (cmd) {
		case SO_SOCKET:
			/* FIXME: 32/64 conversion */
			/* XXX: don't think so..., after checking for a stream */
			err = sys_socket(req->args[1], req->args[2], req->args[3]);
			break;
		case SO_SOCKPAIR:
			/* FIXME: 32/64 conversion */
			/* XXX: don't think so..., after checking for a stream */
			err = sys_socketpair(req->args[1], req->args[2], req->args[3],
					     (int *) req->args[4]);
			err = -EOPNOTSUPP;
			break;
		case SO_SELECT:
			/* FIXME: 32/64 conversion */
			err = sys_select(req->args[1], (fd_set *) req->args[2],
					 (fd_set *) req->args[3], (fd_set *) req->args[4],
					 (struct timeval *) req->args[5]);
			break;
		case SO_GETIPDOMAIN:
			/* FIXME: 32/64 conversion */
			todo(("Process SO_GETIPDOMAIN for compatibility.\n"));
			/* does not exist in Linux, need to use sys_newuname and copy the domainname portion */
			err = -ENOSYS;
			break;
		case SO_SETIPDOMAIN:
			/* FIXME: 32/64 conversion */
			err = sys_setdomainname((char *)req->args[1], req->args[2]);
			break;
		case SO_ADJTIME:
			/* FIXME: 32/64 conversion */
			err = sys_admtimex((struct timex *)req->args[1]);
			break;
		case SO_SETREUID:
			/* FIXME: 32/64 conversion */
			err = sys_setreuid(req->args[1], req->args[2]);
			break;
		case SO_SETREGID:
			/* FIXME: 32/64 conversion */
			err = sys_setregid(req->args[1], req->args[2]);
			break;
		case SO_GETTIME:
			/* FIXME: 32/64 conversion */
			err = sys_gettimeofday((struct timeval *) req->args[1],
					     (struct timezone *) req->args[2]);
			break;
		case SO_SETTIME:
			/* FIXME: 32/64 conversion */
			err = sys_settimeofday((struct timeval *) req->args[1],
					     (struct timezone *) req->args[2]);
			break;
		case SO_GETITIMER:
			/* FIXME: 32/64 conversion */
			err = sys_getitimer(req->args[1], (struct itimerval *) req->args[2]);
			break;
		case SO_SETITIMER:
			/* FIXME: 32/64 conversion */
			err = sys_getitimer(req->args[1], (struct itimerval *) req->args[2],
					  (struct itimerval *) req->args[3]);
			break;
		}
#endif
	}

	return (err);
}

/*
 *  Put procedure.
 */
STATIC streamscall int
socksys_put(queue_t *q, mblk_t *mp)
{
	struct ssys *ss = q->q_ptr;
	union ioctypes *ioc;
	int err = 0, rval = 0, count = 0;
	mblk_t *dp = mp->b_cont;
	caddr_t req_addr, prot_addr;
	size_t req_size, prot_size;

	(void) prot_size;
	(void) prot_addr;
	switch (mp->b_datap->db_type) {
	case M_FLUSH:
		mp->b_rptr[0] &= ~FLUSHW;
		if (mp->b_rptr[0] & FLUSHR) {
			qreply(q, mp);
			return (0);
		}
		break;
	case M_IOCTL:
		ioc = (typeof(ioc)) mp->b_rptr;
#ifdef WITH_32BIT_CONVERSION
		if (ioc->iocblk.ioc_flag == IOC_ILP32) {
			/* XXX: following pointer conversion does not work on all architectures */
			req_addr =
			    (caddr_t) (unsigned long) (uint32_t) *(unsigned long *) dp->b_rptr;
			req_size = sizeof(struct socksysreq32);
		} else
#endif				/* WITH_32BIT_CONVERSION */
		{
			req_addr = (caddr_t) *(unsigned long *) dp->b_rptr;
			req_size = sizeof(struct socksysreq);
		}
		switch (ioc->iocblk.ioc_cmd) {
		case SIOCATMARK:
		case SIOCGPGRP:
		case SIOCSPGRP:
#if 0
		case SIOCHIWAT:
		case SIOGHIWAT:
		case SIOCLOWAT:
		case SIOGLOWAT:
		case SIOCPROTO:
		case SIOCGETNAME:
		case SIOCGETPEER:
		case SIOXPROTO:
		case SIOCSOCKSYS:
#endif
			err = -EFAULT;
			if (!dp || dp->b_wptr < dp->b_rptr + req_size)
				goto nak;
			if (ioc->iocblk.ioc_count == TRANSPARENT) {
				mp->b_datap->db_type = M_COPYIN;
				ioc->copyreq.cq_addr = req_addr;
				ioc->copyreq.cq_size = req_size;
				ioc->copyreq.cq_flag = 0;
				ioc->copyreq.cq_private = (mblk_t *) ioc->copyreq.cq_addr;
				ss->transparent = 1;
				ss->iocstate = 1;
				qreply(q, mp);
				return (0);
			}
			ss->transparent = 0;
			ss->iocstate = 1;
			goto socksys_state_1;
		}
		break;
	case M_IOCDATA:
		ioc = (typeof(ioc)) mp->b_rptr;
		if (ioc->copyresp.cp_rval != (caddr_t) 0) {
			ss->transparent = 0;
			ss->iocstate = 0;
			goto abort;
		}
		switch (ioc->copyresp.cp_cmd) {
		case SIOCATMARK:
		case SIOCGPGRP:
		case SIOCSPGRP:
#if 0
		case SIOCHIWAT:
		case SIOGHIWAT:
		case SIOCLOWAT:
		case SIOGLOWAT:
		case SIOCPROTO:
		case SIOCGETNAME:
		case SIOCGETPEER:
		case SIOXPROTO:
		case SIOCSOCKSYS:
#endif
		{
			struct socksysreq sr, *req = &sr;

#ifdef WITH_32BIT_CONVERSION
			if (ioc->iocblk.ioc_flag == IOC_ILP32) {
				struct socksysreq32 *req32 = (typeof(req32)) dp->b_rptr;

				err = -EFAULT;
				if (!dp || dp->b_wptr < dp->b_rptr + sizeof(*req32))
					goto nak;
				req->args[0] = (unsigned long) (uint32_t) req32->args[0];
				req->args[1] = (unsigned long) (uint32_t) req32->args[1];
				req->args[2] = (unsigned long) (uint32_t) req32->args[2];
				req->args[3] = (unsigned long) (uint32_t) req32->args[3];
				req->args[4] = (unsigned long) (uint32_t) req32->args[4];
				req->args[5] = (unsigned long) (uint32_t) req32->args[5];
				req->args[6] = (unsigned long) (uint32_t) req32->args[6];
			} else
#endif				/* WITH_32BIT_CONVERSION */
			{
				err = -EFAULT;
				if (!dp || dp->b_wptr < dp->b_rptr + sizeof(*req))
					goto nak;
				bcopy(dp->b_rptr, req, sizeof(*req));
			}
			switch (ss->iocstate) {
			case 1:
			      socksys_state_1:
				err = -EINVAL;
				if (sr.args[0] < SO_ACCEPT || sr.args[0] > SO_SOCKPAIR)
					goto nak;
				rval = err = so_socksys(req);
				if (err < 0)
					goto nak;
				count = 0;
				goto ack;
			}
			err = -EIO;
			goto nak;
		}
		}
		break;
	}
      abort:
	freemsg(mp);
	return (0);
      nak:
	mp->b_datap->db_type = M_IOCNAK;
	ioc->iocblk.ioc_count = 0;
	ioc->iocblk.ioc_rval = -1;
	ioc->iocblk.ioc_error = -err;
	ss->transparent = 0;
	ss->iocstate = 0;
	qreply(q, mp);
	return (0);
      ack:
	mp->b_datap->db_type = M_IOCACK;
	ioc->iocblk.ioc_count = count;
	ioc->iocblk.ioc_rval = rval;
	ioc->iocblk.ioc_error = 0;
	ss->transparent = 0;
	ss->iocstate = 0;
	qreply(q, mp);
	return (0);
}

/**
 * ssys_free_priv - deallocate a private structure for the close routine
 * @q: read queue of closing Stream
 */
STATIC noinline void
ssys_free_priv(queue_t *q)
{
	struct ssys *s;

	ensure(q, return);
	s = SOCKSYS_PRIV(q);
	ensure(q, return);
#if 0
	strlog(DRV_ID, s->dev.cminor, 0, SL_TRACE,
	       "unlinking private structure: reference count = %d", atomic_read(&s->refcnt));
#else
	printd(("%s: unlinking private structure, reference count = %d\n", DRV_NAME,
		atomic_read(&s->refcnt)));
#endif
	/* remove from master list */
	if ((*s->prev = s->next))
		s->next->prev = s->prev;
	s->next = NULL;

}

/*
 *  Open and Close
 */
#define FIRST_CMINOR	0
#define  LAST_CMINOR	0
#define  FREE_CMINOR	1

STATIC int ssys_majors[CMAJORS] = { CMAJOR_0, };
STATIC struct ssys *socksys_opens = NULL;
STATIC rwlock_t socksys_lock = RW_LOCK_UNLOCKED;

/**
 * socksys_qopen - SOCKSYS driver STREAMS open routine
 * @q: read queue of opened Stream
 * @devp: pointer to device number opened
 * @oflag: flags to the open call
 * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
 * @crp: pointer to opener's credentials
 */
STATIC streamscall int
socksys_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	int mindex = 0;
	int type = 0;
	major_t cmajor = getmajor(*devp);
	minor_t cminor = getminor(*devp);
	struct ssys **sp = &socksys_opens;
	unsigned long flags;

	if (q->q_ptr != NULL) {
		return (0);	/* already open */
	}
	if (sflag == MODOPEN || WR(q)->q_next) {
		ptrace(("%s: ERROR: cannot push as module\n", DRV_NAME));
		return (EIO);
	}
	/* Linux Fast-STREAMS always passes internal major device number (module id).  Note also,
	   however, that strconf-sh attempts to allocate module ids that are identical to the base
	   major device number anyway. */
#if defined LIS
	if (cmajor != CMAJOR_0)
		return (ENXIO);
#endif
#if defined LFS
	/* Linux Fast-STREAMS always passes internal major device numbers (module ids) */
	if (cmajor != DRV_ID)
		return (ENXIO);
#endif
	/* sorry, you cannot open by minor device */
	if (cminor > LAST_CMINOR) {
		return (ENXIO);
	}
	type = cminor;
#if 0
	if (sflag == CLONEOPEN)
#endif
		cminor = FREE_CMINOR;
	write_lock_str2(&socksys_lock, flags);
	for (; *sp; sp = &(*sp)->next) {
		if (cmajor != (*sp)->dev.cmajor)
			break;
		if (cmajor == (*sp)->dev.cmajor) {
			if (cminor < (*sp)->dev.cminor)
				break;
			if (cminor == (*sp)->dev.cminor) {
				if (++cminor >= NMINORS) {
					if (++mindex >= CMAJORS || !(cmajor = ssys_majors[mindex]))
						break;
					cminor = 0;	/* start next major */
				}
				continue;
			}
		}
	}
	if (mindex >= CMAJORS || !cmajor) {
		ptrace(("%s: ERROR: no device numbers available\n", DRV_NAME));
		write_unlock_str2(&socksys_lock, flags);
		return (ENXIO);
	}
	_printd(("%s: opened character device %d:%d\n", DRV_NAME, cmajor, cminor));
	*devp = makedevice(cmajor, cminor);
	if (!ssys_alloc_priv(q, sp, type, devp, crp)) {
		ptrace(("%s: ERROR: No memory\n", DRV_NAME));
		write_unlock_str2(&socksys_lock, flags);
		return (ENOMEM);
	}
	write_unlock_str2(&socksys_lock, flags);
	qprocson(q);
	return (0);
}

/**
 * socksys_qclose - SOCKSYS driver STREAMS close routine
 * @q: read queue of closing Stream
 * @oflag: flags to open call
 * @crp: pointer to closer's credentials
 */
STATIC streamscall int
socksys_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct ssys *s = SOCKSYS_PRIV(q);

	(void) oflag;
	(void) crp;
	(void) s;
	_printd(("%s: closing character device %d:%d\n", DRV_NAME, s->dev.cmajor,
		 s->dev.cminor));
#if defined LIS
	/* protect against LiS bugs */
	if (q->q_ptr == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS double-close bug detected.", DRV_NAME, __FUNCTION__);
		goto quit;
	}
	if (q->q_next == NULL) {
		cmn_err(CE_WARN, "%s: %s: LiS pipe bug: called with NULL q->q_next pointer",
			DRV_NAME, __FUNCTION__);
		goto skip_pop;
	}
#endif				/* defined LIS */
	goto skip_pop;
      skip_pop:
	/* make sure procedures are off */
	qprocsoff(q);
	ssys_free_priv(q);	/* free and unlink the structure */
	goto quit;
      quit:
	return (0);
}

/*
 *  Private struct reference counting, allocation, deallocation and cache
 */
STATIC __unlikely int
ssys_term_caches(void)
{
	if (ssys_priv_cachep != NULL) {
		if (kmem_cache_destroy(ssys_priv_cachep)) {
			cmn_err(CE_WARN, "%s: did not destroy ssys_priv_cachep", __FUNCTION__);
			return (-EBUSY);
		}
		_printd(("%s: destroyed ssys_priv_cachep\n", DRV_NAME));
		ssys_priv_cachep = NULL;
	}
	return (0);
}
STATIC __unlikely int
ssys_init_caches(void)
{
	if (ssys_priv_cachep == NULL) {
		ssys_priv_cachep =
		    kmem_cache_create("ssys_priv_cachep", sizeof(struct ssys), 0,
				      SLAB_HWCACHE_ALIGN, NULL, NULL);
		if (ssys_priv_cachep == NULL) {
			cmn_err(CE_WARN, "%s: Cannot allocation ssys_priv_cachep", __FUNCTION__);
			ssys_term_caches();
			return (-ENOMEM);
		}
		_printd(("%s: initialize driver private structure cache\n", DRV_NAME));
	}
	return (0);
}

/*
 *  Registration and initialization
 */
#ifdef LINUX
/*
 *  Linux registration
 */
unsigned short modid = DRV_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for the SOCKSYS driver. (0 for allocation.)");

major_t major = CMAJOR_0;

#ifndef module_param
MODULE_PARM(major, "h");
#else
module_param(major, uint, 0);
#endif
MODULE_PARM_DESC(major, "Device number for the SOCKSYS driver. (0 for allocation.)");

#ifdef LFS
/*
 *  Linux Fast-STREAMS Registration
 */
STATIC struct cdevsw ssys_cdev = {
	.d_name = DRV_NAME,
	.d_str = &socksys_info,
	.d_flag = D_MP,
	.d_fop = NULL,
	.d_mode = S_IFCHR,
	.d_kmod = THIS_MODULE,
};

STATIC __unlikely int
ssys_register_strdev(major_t major)
{
	int err;

	if ((err = register_strdev(&ssys_cdev, major)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
ssys_unregister_strdev(major_t major)
{
	int err;

	if ((err = unregister_strdev(&ssys_cdev, major)) < 0)
		return (err);
	return (0);
}
#endif				/* LFS */

#ifdef LIS
/*
 *  Linux STREAMS Registration
 */
STATIC __unlikely int
ssys_register_strdev(major_t major)
{
	int err;

	if ((err = lis_register_strdev(major, &socksys_info, UNITS, DRV_NAME)) < 0)
		return (err);
	return (0);
}

STATIC __unlikely int
ssys_unregister_strdev(major_t major)
{
	int err;

	if ((err = lis_unregister_strdev(major)) < 0)
		return (err);
	return (0);
}
#endif				/* LIS */

MODULE_STATIC void __exit
ssysterminate(void)
{
	int err, mindex;

	for (mindex = CMAJORS - 1; mindex >= 0; mindex--) {
		if (ssys_majors[mindex]) {
			if ((err = ssys_unregister_strdev(ssys_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", DRV_NAME,
					ssys_majors[mindex]);
			if (mindex)
				ssys_majors[mindex] = 0;
		}
	}
	if ((err = ssys_term_caches()))
		cmn_err(CE_WARN, "%s: could not terminate caches", DRV_NAME);
	return;
}

MODULE_STATIC int __init
ssysinit(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, DRV_BANNER);
	if ((err = ssys_init_caches())) {
		cmn_err(CE_WARN, "%s: could not init caches, err = %d", DRV_NAME, err);
		ssysterminate();
		return (err);
	}
	for (mindex = 0; mindex < CMAJORS; mindex++) {
		if ((err = ssys_register_strdev(ssys_majors[mindex])) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", DRV_NAME,
					ssys_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					DRV_NAME, err);
				ssysterminate();
				return (err);
			}
		}
		if (ssys_majors[mindex] == 0)
			ssys_majors[mindex] = err;
#if 0
		LIS_DEVFLAGS(ssys_majors[index]) |= LIS_MODFLG_CLONE;
#endif
		if (major == 0)
			major = ssys_majors[0];
	}
	return (0);
}

/*
 *  Linux Kernel Module Initialization
 *  -------------------------------------------------------------------------
 */
module_init(ssysinit);
module_exit(ssysterminate);

#endif				/* LINUX */
