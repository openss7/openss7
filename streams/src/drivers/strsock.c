/*****************************************************************************

 @(#) strsock.c,v (0.9.2.9) 2003/10/27 12:23:16

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified 2003/10/27 12:23:16 by brian

 *****************************************************************************/

#ident "@(#) strsock.c,v (0.9.2.9) 2003/10/27 12:23:16"

static char const ident[] =
    "strsock.c,v (0.9.2.9) 2003/10/27 12:23:16";

#include <linux/config.h>
#include <linux/version.h>
#ifdef MODVERSIONS
#include <linux/modversions.h>
#endif
#include <linux/module.h>
#include <linux/smp_lock.h>
#include <linux/slab.h>

#include <net/sock.h>

#include <sys/stropts.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/strsubr.h>
#include <sys/ddi.h>

#include <tihdr.h>

#include "strdebug.h"
#include "strreg.h"		/* for struct str_args */
#include "strsched.h"		/* fort sd_get/sd_put */
#include "strhead.h"		/* for autopush */
#include "strsock.h"		/* extern verification */

#define SOCKSYS_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define SOCKSYS_COPYRIGHT	"Copyright (c) 1997-2003 OpenSS7 Corporation.  All Rights Reserved."
#define SOCKSYS_REVISION	"LfS strsock.c,v (0.9.2.9) 2003/10/27 12:23:16"
#define SOCKSYS_DEVICE		"SVR 4.2 STREAMS Sockets Library (SOCKSYS) Support"
#define SOCKSYS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define SOCKSYS_LICENSE		"GPL"
#define SOCKSYS_BANNER	SOCKSYS_DESCRIP		"\n" \
			SOCKSYS_COPYRIGHT	"\n" \
			SOCKSYS_REVISION	"\n" \
			SOCKSYS_DEVICE		"\n" \
			SOCKSYS_CONTACT		"\n"
#define SOCKSYS_SPLASH	SOCKSYS_DEVICE		" - " \
			SOCKSYS_REVISION	"\n"

MODULE_AUTHOR(SOCKSYS_CONTACT);
MODULE_DESCRIPTION(SOCKSYS_DESCRIP);
MODULE_SUPPORTED_DEVICE(SOCKSYS_DEVICE);
MODULE_LICENSE(SOCKSYS_LICENSE);

/* 
 *  T_BIND
 *  -------------------------------------
 */
static int t_bind(struct inode *inode, struct sockaddr *umyaddr, int sockaddr_len)
{
	struct stdata *sd = (struct stdata *)inode->i_pipe;
	struct T_bind_req *p;
	mblk_t *mp;
	if ((mp = allocb(sizeof(*p) + sockaddr_len, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = T_BIND_REQ;
		p->ADDR_offset = sizeof(*p);
		p->ADDR_length = sockaddr_len;
		p->CONIND_number = 0;
		bcopy(umyaddr, mp->b_wptr, sockaddr_len);
		mp->b_wptr += sockaddr_len;
		putnext(sd->sd_wq, mp);
		for (;;) {
			while (!(mp = getq(sd->sd_rq)))
				if (qwait_sig(sd->sd_rq))
					break;
			if (!mp)
				return (-EINTR);
			if (*(ulong *) mp->b_rptr == T_BIND_ACK) {
				break;
			}
			freemsg(mp);
		}
		/* wait */
		return (0);
	}
	return (-ENOSR);
}

/* 
 *  T_UNBIND
 *  -------------------------------------
 */
#if 0
static int t_unbind(struct inode *inode)
{
	struct stdata *sd = (struct stdata *)inode->i_pipe;
	struct T_unbind_req *p;
	mblk_t *mp;
	if ((mp = allocb(sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr++;
		p->PRIM_type = T_UNBIND_REQ;
		putnext(sd->sd_wq, mp);
		/* wait */
		return (0);
	}
	return (-ENOSR);
}
#endif

/* 
 *  TLI RELEASE
 *  -------------------------------------
 */
static int tli_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	if (sk) {
		long timeout;
		timeout = 0;
		if (sk->linger && !(current->flags & PF_EXITING))
			timeout = sk->lingertime;
		sock->sk = NULL;
		sk->prot->close(sk, timeout);
	}
	return (0);
}

/* 
 *  TLI BIND
 *  -------------------------------------
 */
static int tli_bind(struct socket *sock, struct sockaddr *umyaddr, int sockaddr_len)
{
	int err;
	/* bind */
	switch (sock->state) {
	case TS_UNBND:
	case TS_IDLE:		/* protocol might accept double bind */
		if ((err = t_bind(sock->inode, umyaddr, sockaddr_len))) {
			if (err & 0xff)
				return (-(err & 0xff));
			switch ((err >> 8) & 0xff) {
			case TACCES:
				return (-EACCES);
			case TADDRBUSY:
				return (-EADDRINUSE);
			case TBADADDR:
			case TNOADDR:
				return (-EADDRNOTAVAIL);
			case TOUTSTATE:
				return (-EINVAL);
			case TSYSERR:
				swerr();
				return (-(err & 0xff));
			}
		}
	default:
	case TS_DATA_XFER:
		return (-EISCONN);
	}
	if (umyaddr->sa_family == AF_UNSPEC) {
		switch (sock->state) {
		case TS_UNBND:
			/* already unbound */
			return (0);
		case TS_IDLE:
			/* unbind */
		default:
			break;
		}
	}
	return (-EAFNOSUPPORT);
}

/* 
 *  TLI CONNECT
 *  -------------------------------------
 */
static int tli_connect(struct socket *sock, struct sockaddr *uservaddr, int sockaddr_len, int oflag)
{
	struct sock *sk = sock->sk;
	long timeo;
	int err;
	lock_sock(sk);
	if (uservaddr->sa_family == AF_UNSPEC) {
		err = sk->prot->disconnect(sk, oflag);
		sock->state = err ? SS_DISCONNECTING : SS_UNCONNECTED;
		goto out;
	}
	switch (sock->state) {
	default:
		err = -EINVAL;
		goto out;
	case SS_CONNECTED:
		err = -EISCONN;
		goto out;
	case SS_CONNECTING:
		err = -EALREADY;
		break;
	case SS_UNCONNECTED:
		err = -EISCONN;
		if (sk->state != TS_IDLE)
			goto out;
		err = -EAGAIN;
		if (sk->num == 0) {
			if (sk->prot->get_port(sk, 0) != 0)
				goto out;
			sk->sport = htons(sk->num);
		}
		err = sk->prot->connect(sk, uservaddr, sockaddr_len);
		if (err < 0)
			goto out;
		sock->state = SS_CONNECTING;
		err = -EINPROGRESS;
		break;
	}
	timeo = sock_sndtimeo(sk, oflag & FNDELAY);
	if ((1 << sk->state) & ((1 << TS_WCON_CREQ) | (1 << TS_WRES_CIND))) {
		if (timeo) {
			DECLARE_WAITQUEUE(wait, current);
			__set_current_state(TASK_INTERRUPTIBLE);
			add_wait_queue(sk->sleep, &wait);
			while ((1 << sk->state) & ((1 << TS_WCON_CREQ) || (1 << TS_WRES_CIND))) {
				release_sock(sk);
				timeo = schedule_timeout(timeo);
				lock_sock(sk);
				if (signal_pending(current) || !timeo)
					break;
				set_current_state(TASK_INTERRUPTIBLE);
			}
			__set_current_state(TASK_RUNNING);
			remove_wait_queue(sk->sleep, &wait);
		}
		if (!timeo)
			goto out;
		err = sock_intr_errno(timeo);
		if (signal_pending(current))
			goto out;
	}
	if (sk->state == (1 << TS_IDLE))
		goto sock_error;
	sock->state = SS_CONNECTED;
	err = 0;
      out:
	release_sock(sk);
	return err;
      sock_error:
	err = sock_error(sk) ? : -ECONNABORTED;
	sock->state = SS_UNCONNECTED;
	if (sk->prot->disconnect(sk, oflag))
		sock->state = SS_DISCONNECTING;
	goto out;
}

/* 
 *  TLI SOCKETPAIR
 *  -------------------------------------
 */
static int tli_socketpair(struct socket *sock1, struct socket *sock2)
{
	return (-ENOSYS);
}

/* 
 *  TLI ACCEPT
 *  -------------------------------------
 */
static int tli_accept(struct socket *sock, struct socket *newsock, int flags)
{
	struct sock *sk1 = sock->sk;
	struct sock *sk2;
	int err;
	err = -EINVAL;
	if ((sk2 = sk1->prot->accept(sk1, flags, &err)) == NULL)
		goto error;
	lock_sock(sk2);
	if ((1 << sk2->state) & ((1 << TS_DATA_XFER) | (1 << TS_IDLE)))
		swerr();
	sock_graft(sk2, newsock);
	newsock->state = SS_CONNECTED;
	release_sock(sk2);
      error:
	return (err);
}

/* 
 *  TLI GETNAME
 *  -------------------------------------
 */
static int tli_getname(struct socket *sock, struct sockaddr *uaddr, int *usockaddr_len, int peer)
{
	uaddr->sa_family = sock->ops->family;
	if (peer) {
	} else {
	}
	return (-ENOSYS);
}

/* 
 *  TLI POLL
 *  -------------------------------------
 */
static unsigned int tli_poll(struct file *file, struct socket *sock, struct poll_table_struct *wait)
{
	return (-ENOSYS);
}

/* 
 *  TLI IOCTL
 *  -------------------------------------
 */
static int tli_ioctl(struct socket *sock, unsigned int cmd, unsigned long arg)
{
	return (-ENOSYS);
}

/* 
 *  TLI LISTEN
 *  -------------------------------------
 */
static int tli_listen(struct socket *sock, int len)
{
	return (-ENOSYS);
}

/* 
 *  TLI SHUTDOWN
 *  -------------------------------------
 */
static int tli_shutdown(struct socket *sock, int flags)
{
	return (-ENOSYS);
}

/* 
 *  TLI SETSOCKOPT
 *  -------------------------------------
 */
static int tli_setsockopt(struct socket *sock, int level, int optname, char *optval, int optlen)
{
	return (-ENOSYS);
}

/* 
 *  TLI GETSOCKOPT
 *  -------------------------------------
 */
static int tli_getsockopt(struct socket *sock, int level, int optname, char *optval, int *optlen)
{
	return (-ENOSYS);
}

/* 
 *  TLI SENDMSG
 *  -------------------------------------
 */
static int tli_sendmsg(struct socket *sock, struct msghdr *m, int total_len, struct scm_cookie *scm)
{
	return (-ENOSYS);
}

/* 
 *  TLI RECVMSG
 *  -------------------------------------
 */
static int tli_recvmsg(struct socket *sock, struct msghdr *m, int total_len, int flags,
		       struct scm_cookie *scm)
{
	return (-ENOSYS);
}

/* 
 *  TLI MMAP
 *  -------------------------------------
 */
static int tli_mmap(struct file *file, struct socket *sock, struct vm_area_struct *vma)
{
	return (-ENOSYS);
}

/* 
 *  TLI SENDPAGE
 *  -------------------------------------
 */
static ssize_t tli_sendpage(struct socket *sock, struct page *page, int offset, size_t size,
			    int flags)
{
	return (-ENOSYS);
}

#define socki_lookup(__inode) (&__inode->u.socket_i)

#define stri_lookup(__f) ((struct stdata *)(__f)->private_data)
#define sdev_lookup(__i) ((struct cdevsw *)(__i)->i_cdev->data)

static loff_t socksys_llseek(struct file *f, loff_t off, int whence)
{
	return (-ESPIPE);
}

static ssize_t socksys_read(struct file *f, char *buf, size_t len, loff_t *ppos)
{
	struct socket *sock;
	struct iovec iov;
	struct msghdr msg;
	int flags;
	if (ppos != &f->f_pos)
		return -ESPIPE;
	if (len == 0)		/* Match SYS5 behaviour */
		return 0;
	sock = socki_lookup(f->f_dentry->d_inode);
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	iov.iov_base = buf;
	iov.iov_len = len;
	flags = !(f->f_flags & FNDELAY) ? 0 : MSG_DONTWAIT;
	return sock_recvmsg(sock, &msg, len, flags);
}

static ssize_t socksys_write(struct file *f, const char *buf, size_t len, loff_t *ppos)
{
	struct socket *sock;
	struct msghdr msg;
	struct iovec iov;
	if (ppos != &f->f_pos)
		return -ESPIPE;
	if (len == 0)		/* Match SYS5 behaviour */
		return 0;
	sock = socki_lookup(f->f_dentry->d_inode);
	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = !(f->f_flags & FNDELAY) ? 0 : MSG_DONTWAIT;
	if (sock->type == SOCK_SEQPACKET)
		msg.msg_flags |= MSG_EOR;
	iov.iov_base = (void *) buf;
	iov.iov_len = len;
	return sock_sendmsg(sock, &msg, len);
}

static unsigned int socksys_poll(struct file *f, struct poll_table_struct *poll)
{
	struct socket *sock;
	sock = socki_lookup(f->f_dentry->d_inode);
	return sock->ops->poll(f, sock, poll);
}

static int socksys_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
{
	struct socket *sock;
	int err;
	unlock_kernel();
	sock = socki_lookup(i);
	err = sock->ops->ioctl(sock, cmd, arg);
	lock_kernel();
	return err;
}

static int socksys_mmap(struct file *f, struct vm_area_struct *vma)
{
	struct socket *sock = socki_lookup(f->f_dentry->d_inode);
	return sock->ops->mmap(f, sock, vma);
}

extern struct streamtab str_info;

static int socksys_open(struct inode *i, struct file *f)
{
	return (-ENXIO);
}

static int socksys_fasync(int fd, struct file *f, int on)
{
	struct stdata *sd = stri_lookup(f);
	int err;
	if ((err = fasync_helper(fd, f, on, &sd->sd_siglist)) < 0)
		goto error;
	return (0);
      error:
	return (err);
}

static int socksys_close(struct inode *i, struct file *f)
{
	if (!i) {
		swerr();
		return (0);
	}
	socksys_fasync(-1, f, 0);
	sock_release(&i->u.socket_i);
	return (0);
}

static typeof(&sock_readv_writev) _sock_readv_writev
    = (typeof(_sock_readv_writev)) HAVE_SOCK_READV_WRITEV_ADDR;

static ssize_t socksys_readv(struct file *f, const struct iovec *iov, unsigned long len,
			     loff_t *ppos)
{
	size_t tot_len = 0;
	int i;
	for (i = 0; i < len; i++)
		tot_len += iov[i].iov_len;
	return _sock_readv_writev(VERIFY_WRITE, f->f_dentry->d_inode, f, iov, len, tot_len);
}

static ssize_t socksys_writev(struct file *f, const struct iovec *iov, unsigned long count,
			      loff_t *ppos)
{
	size_t tot_len = 0;
	int i;
	for (i = 0; i < count; i++)
		tot_len += iov[i].iov_len;
	return _sock_readv_writev(VERIFY_READ, f->f_dentry->d_inode, f, iov, count, tot_len);
}

static ssize_t socksys_sendpage(struct file *f, struct page *page, int offset, size_t size,
				loff_t *ppos, int more)
{
	struct socket *sock;
	int flags;
	if (ppos != &f->f_pos)
		return -ESPIPE;
	sock = socki_lookup(f->f_dentry->d_inode);
	flags = !(f->f_flags & FNDELAY) ? 0 : MSG_DONTWAIT;
	if (more)
		flags |= MSG_MORE;
	return sock->ops->sendpage(sock, page, offset, size, flags);
}

#if 0
static int socksysputpmsg(struct file *f, struct strbuf *ctlp, struct strbuf *datp, int band,
			  int flags)
{
	return (-ENOSYS);
}

static int socksysgetpmsg(struct file *f, struct strbuf *ctlp, struct strbuf *datp, int *band,
			  int *flagsp)
{
	return (-ENOSYS);
}
#endif

extern struct proto_ops *sock_p_ops;

static struct proto_ops socksys_p_ops ____cacheline_aligned = {
	family:0,
	release:tli_release,
	bind:tli_bind,
	connect:tli_connect,
	socketpair:tli_socketpair,
	accept:tli_accept,
	getname:tli_getname,
	poll:tli_poll,
	ioctl:tli_ioctl,
	listen:tli_listen,
	shutdown:tli_shutdown,
	setsockopt:tli_setsockopt,
	getsockopt:tli_getsockopt,
	sendmsg:tli_sendmsg,
	recvmsg:tli_recvmsg,
	mmap:tli_mmap,
	sendpage:tli_sendpage,
};

extern struct file_operations *sock_f_ops;

struct file_operations socksys_f_ops ____cacheline_aligned = {
	owner:THIS_MODULE,
	llseek:socksys_llseek,
	read:socksys_read,
	write:socksys_write,
	poll:socksys_poll,
	ioctl:socksys_ioctl,
	mmap:socksys_mmap,
	open:socksys_open,
	release:socksys_close,
	fasync:socksys_fasync,
	readv:socksys_readv,
	writev:socksys_writev,
	sendpage:socksys_sendpage,
//	getpmsg:socksysgetpmsg,
//	putpmsg:socksysputpmsg,
};

static int __init socksys_init(void)
{
#ifdef MODULE
	printk(KERN_INFO SOCKSYS_BANNER);
#else
	printk(KERN_INFO SOCKSYS_SPLASH);
#endif
	xchg(&sock_f_ops, &socksys_f_ops);
	xchg(&sock_p_ops, &socksys_p_ops);
	return (0);
};
static void __exit socksys_exit(void)
{
	xchg(&sock_f_ops, NULL);
	xchg(&sock_p_ops, NULL);
	return (void) (0);
};

module_init(socksys_init);
module_exit(socksys_exit);
