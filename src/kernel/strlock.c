/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2013  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

#include <linux/compiler.h>
#ifdef NEED_LINUX_AUTOCONF_H
#include NEED_LINUX_AUTOCONF_H
#endif
#include <linux/version.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#ifdef HAVE_KINC_LINUX_STATFS_H
#include <linux/statfs.h>
#endif
#ifdef HAVE_KINC_LINUX_NAMESPACE_H
#include <linux/namespace.h>
#endif
#include <linux/interrupt.h>	/* for irqreturn_t */
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#ifdef HAVE_KINC_LINUX_KTHREAD_H
#include <linux/kthread.h>
#endif
#include <linux/time.h>		/* for struct timespec */
#include <linux/pm.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/if_arp.h>
#include <net/sock.h>
#include <net/protocol.h>
#include <net/udp.h>
#include <net/tcp.h>

#include "sys/strdebug.h"

/* d5f3c4981e2e41f3bdba506861e8b5eca27e48c9b4eecdda */

/* the last 4 octets (9b4eecdda) are salt */

static __u8 strlock_region[48] = {
	0xd5, 0xf3, 0xc4, 0x98, 0x1e, 0x2e, 0x41, 0xf3,
	0xbd, 0xba, 0x50, 0x68, 0x61, 0xe8, 0xb5, 0xec,
	0xa2, 0x7e, 0x48, 0xc9, 0xb4, 0xee, 0xcd, 0xda,
	0xd5, 0xf3, 0xc4, 0x98, 0x1e, 0x2e, 0x41, 0xf3,
	0xbd, 0xba, 0x50, 0x68, 0x61, 0xe8, 0xb5, 0xec,
	0xa2, 0x7e, 0x48, 0xc9, 0xb4, 0xee, 0xcd, 0xda
};

static __u8 *strlock_digest = NULL;

#if defined WITH_SPECFS_LOCK

#include <linux/notifier.h>
#include <linux/crypto.h>

#ifndef MD5_DIGEST_SIZE
#define MD5_DIGEST_SIZE 16
#endif
#ifndef SHA_DIGEST_SIZE
#define SHA_DIGEST_SIZE 20
#endif

struct strlock_text {
	uint32_t expires;
	char addr[8];
};

static struct crypto_hash *strlock_tfm[2];

static int strlock_reverify = 1;
static int strlock_unlocked = 0;
static int strlock_expires = 0;
static int strlock_options = 0;

/* Borrowed largely from sctp2.c. */

#if !defined HAVE_DEV_BASE_HEAD_SYMBOL
#if defined HAVE_KFUNC_FIRST_NET_DEVICE_1_ARG
#define first_net_device() first_net_device(&init_net)
#else				/* defined HAVE_KFUNC_FIRST_NET_DEVICE_1_ARG */
static inline struct net_device *
first_net_device(void)
{
	return (dev_base);
}

static inline struct net_device *
next_net_device(struct net_device *dev)
{
	return (dev->next);
}
#endif				/* defined HAVE_KFUNC_FIRST_NET_DEVICE_1_ARG */
#endif				/* !defined HAVE_DEV_BASE_HEAD_SYMBOL */

struct hwaddr {
	char addr[8];
};

/*
 *  Get hardware addresses.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Get the hardware addresses that are so far allocated on the system.  We
 *  get up to a maximum of 8 addresses.
 */
static inline int
__spec_getkeys(struct hwaddr *hwa)
{
	struct net_device *dev;
	int num = 0;

	__trace();
	read_lock(&dev_base_lock);
	for (dev = first_net_device(); dev; dev = next_net_device(dev)) {
		switch (dev->type) {
		case ARPHRD_ETHER:
		case ARPHRD_IEEE802:
			hwa[num].addr[0] = hwa[num].addr[1] = 0;
			memcpy(&hwa[num].addr[2], dev->dev_addr, 6);
			_printd((KERN_WARNING "Got ether address %02x:%02x:%02x:%02x:%02x:%02x", dev->dev_addr[0],dev->dev_addr[1], dev->dev_addr[2],dev->dev_addr[3], dev->dev_addr[4],dev->dev_addr[5]));
			num++;
			if (num < 8)
				continue;
			break;
		case ARPHRD_IEEE802_TR:
			memcpy(&hwa[num].addr[0], dev->dev_addr, 8);
			_printd((KERN_WARNING "Got token address %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", dev->dev_addr[0],dev->dev_addr[1], dev->dev_addr[2],dev->dev_addr[3], dev->dev_addr[4],dev->dev_addr[5],dev->dev_addr[6],dev->dev_addr[7]));
			num++;
			if (num < 8)
				continue;
			break;
		default:
			continue;
		}
		break;
	}
	read_unlock(&dev_base_lock);
	return (num);
}

static inline int
strlock_getkeys(struct hwaddr *hwa)
{
	unsigned long flags;
	int num;

	__trace();
	local_irq_save(flags);
	num = __spec_getkeys(hwa);
	local_irq_restore(flags);
	return (num);
}

static int
strlock_notifier(struct notifier_block *self, unsigned long msg, void *data)
{
	struct net_device *dev = (struct net_device *) data;

	__trace();
	switch (dev->type) {
	case ARPHRD_ETHER:
	case ARPHRD_IEEE802:
	case ARPHRD_IEEE802_TR:
		switch (msg) {
		case NETDEV_UP:
		case NETDEV_REBOOT:
			strlock_reverify = 1;
			break;
		case NETDEV_DOWN:
		case NETDEV_GOING_DOWN:
			break;
		case NETDEV_CHANGEADDR:
			break;
		default:
		case NETDEV_CHANGE:
		case NETDEV_UNREGISTER:
		case NETDEV_REGISTER:
		case NETDEV_CHANGENAME:
			break;
		}
	}
	return NOTIFY_DONE;
}

/*
 * Verify filesystem.
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Verify the access key for the filesystem.  Verify the signature on the access
 * key.
 */
static inline void
strlock_verify(void)
{
	struct hwaddr hwa[8];
	struct strlock_text text;
	struct hash_desc desc_md5, desc_sha;
	struct scatterlist sg;
	__u8 digest[SHA_DIGEST_SIZE];
	int i, num;

	__trace();
	if (strlock_digest == NULL) {
		_ptrace((KERN_WARNING "no disgest"));
		return;
	}

	if ((num = strlock_getkeys(hwa)) == 0) {
		_ptrace((KERN_WARNING "no hardware addresses"));
		return;
	}

	desc_md5.tfm = strlock_tfm[0];
	desc_md5.flags = 0;
	desc_sha.tfm = strlock_tfm[1];
	desc_sha.flags = 0;

	memcpy(&text.expires, strlock_digest, sizeof(text.expires));

	for (i = 0; i < num; i++) {
		memcpy(text.addr, hwa[i].addr, 8);
		if (crypto_hash_setkey(desc_md5.tfm, hwa[i].addr, 8) != 0) {
			_ptrace((KERN_WARNING "could not set md5 key"));
			continue;
		}
		sg_init_one(&sg, &text, sizeof(text));
		crypto_hash_digest(&desc_md5, &sg, sg.length, &digest[4]);
		memcpy(digest, &text.expires, sizeof(text.expires));
		_ptrace((KERN_WARNING "Got md5 digest: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
					digest[ 4],digest[ 5],digest[ 6],digest[ 7],
					digest[ 8],digest[ 9],digest[10],digest[11],
					digest[12],digest[13],digest[14],digest[15],
					digest[16],digest[17],digest[18],digest[19]));
		if ((strlock_unlocked = (memcmp(digest, strlock_digest, SHA_DIGEST_SIZE) == 0))) {
			_ptrace((KERN_WARNING "md5 digest matched!"));
			strlock_reverify = 0;
			return;
		} else {
			_ptrace((KERN_WARNING "md5 digest did not match"));
		}
		if (crypto_hash_setkey(desc_sha.tfm, hwa[i].addr, 8) != 0) {
			_ptrace((KERN_WARNING "could not set sha1 key"));
			continue;
		}
		sg_init_one(&sg, &text, sizeof(text));
		crypto_hash_digest(&desc_sha, &sg, sg.length, digest);
		memcpy(digest, &text.expires, sizeof(text.expires));
		_ptrace((KERN_WARNING "Got sha1 digest: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
					digest[ 0],digest[ 1],digest[ 2],digest[ 3],
					digest[ 4],digest[ 5],digest[ 6],digest[ 7],
					digest[ 8],digest[ 9],digest[10],digest[11],
					digest[12],digest[13],digest[14],digest[15],
					digest[16],digest[17],digest[18],digest[19]));
		if ((strlock_unlocked = (memcmp(digest, strlock_digest, SHA_DIGEST_SIZE) == 0))) {
			_ptrace((KERN_WARNING "sha1 digest matched!"));
			strlock_reverify = 0;
			return;
		} else {
			_ptrace((KERN_WARNING "sha1 digest did not match"));
		}
	}
}

#define STRLOCK_FAIL_LOAD_DEADLINE  (1<<7) /* fail load after deadline	  when not set */
#define STRLOCK_FAIL_OPEN_DEADLINE  (1<<6) /* fail open after deadline	  when not set */
#define STRLOCK_FAIL_LOAD_EXPIRE    (1<<5) /* fail load after expiry	  when not set */
#define STRLOCK_FAIL_OPEN_EXPIRE    (1<<4) /* fail open after expiry	  when not set */
#define STRLOCK_FAIL_LOAD_EMPTY	    (1<<3) /* fail load when empty	  when not set */
#define STRLOCK_FAIL_OPEN_EMPTY	    (1<<2) /* fail open when empty	  when not set */
#define STRLOCK_FAIL_LOAD_VERIFY    (1<<1) /* fail load when not verified when not set */
#define STRLOCK_FAIL_OPEN_VERIFY    (1<<0) /* fail open when not verified when not set */

STATIC streams_fastcall int
strlock_open_check(void)
{
	struct timeval tv;

	__trace();
	if (strlock_digest == NULL) {
		_ptrace((KERN_WARNING "digest is empty"));
		if ((strlock_options & STRLOCK_FAIL_OPEN_EMPTY) == 0) {
			return (-ENXIO);
		}
	}
	if (strlock_reverify)
		strlock_verify();
	if (!strlock_unlocked) {
		_ptrace((KERN_WARNING "digest not verified"));
		if ((strlock_options & STRLOCK_FAIL_OPEN_VERIFY) == 0) {
			return (-ENXIO);
		}
		return (-ENXIO);	/* always fail open until verified */
	}
	do_gettimeofday(&tv);
	if (tv.tv_sec > strlock_expires) {
		_ptrace((KERN_WARNING "digest expired"));
		if ((strlock_options & STRLOCK_FAIL_OPEN_EXPIRE) == 0) {
			return (-ENXIO);
		}
	}
	if (tv.tv_sec > STRLOCK_EXPIRES) {
		_ptrace((KERN_WARNING "digest past deadline"));
		if ((strlock_options & STRLOCK_FAIL_OPEN_DEADLINE) == 0) {
			return (-ENXIO);
		}
	}
	return (0);
}

EXPORT_SYMBOL_GPL(strlock_open_check);

static inline int
strlock_load_check(void)
{
	struct timeval tv;

	__trace();
	if (strlock_digest == NULL) {
		_ptrace((KERN_WARNING "digest is empty"));
		if ((strlock_options & STRLOCK_FAIL_LOAD_EMPTY) == 0) {
			return (-EACCES);
		}
		return (-EACCES); /* always fail load when empty */
	} else {
		memcpy(&strlock_expires, strlock_digest, 4);
		strlock_expires = ntohl(strlock_expires);
		strlock_options = ((strlock_expires & 0xff000000) >> 24);
		strlock_expires = ((strlock_expires & 0x00ffffff) << 8);
	}
	if (strlock_reverify)
		strlock_verify();
	if (!strlock_unlocked) {
		_ptrace((KERN_WARNING "digest not verified"));
		if ((strlock_options & STRLOCK_FAIL_LOAD_VERIFY) == 0) {
			return (-EACCES);
		}
	}
	do_gettimeofday(&tv);
	if (tv.tv_sec > strlock_expires) {
		_ptrace((KERN_WARNING "digest expired"));
		if ((strlock_options & STRLOCK_FAIL_LOAD_EXPIRE) == 0) {
			return (-EACCES);
		}
	}
	if (tv.tv_sec > STRLOCK_EXPIRES) {
		_ptrace((KERN_WARNING "digest past deadline"));
		if ((strlock_options & STRLOCK_FAIL_LOAD_DEADLINE) == 0) {
			return (-EACCES);
		}
	}
	return (0);
}


static struct notifier_block strlock_netdev_notifier = {
	.notifier_call = &strlock_notifier,
};

int
strlock_init(void)
{
	int err, i;

	__trace();
	strlock_tfm[0] = crypto_alloc_hash("hmac(md5)", 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(strlock_tfm[0])) {
		err = PTR_ERR(strlock_tfm[0]);
		_ptrace((KERN_WARNING "could not allocate hmac(md5), %d", err));
		goto done;
	}
	strlock_tfm[1] = crypto_alloc_hash("hmac(sha1)", 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(strlock_tfm[1])) {
		err = PTR_ERR(strlock_tfm[1]);
		_ptrace((KERN_WARNING "could not allocate hmac(sha1), %d", err));
		goto no_sha1;
	}
	if ((err = register_netdevice_notifier(&strlock_netdev_notifier))) {
		_ptrace((KERN_WARNING "could not register notifier, %d", err));
		goto no_notify;
	}
	if (memcmp(&strlock_region[0], &strlock_region[24], 24) != 0) {
		_ptrace((KERN_WARNING "strlock_region filled"));
		if (memcmp(&strlock_region[20], &strlock_region[44], 4) == 0) {
			_ptrace((KERN_WARNING "trailer matches"));
			strlock_digest = &strlock_region[24];
			for (i = 0; i < 4; i++)
				strlock_digest[i] ^= 0x5C;
		} else {
			_printd((KERN_WARNING "trailer does not match!"));
		}
	} else {
		_printd((KERN_WARNING "strlock_region is empty!"));
	}
	if ((err = strlock_load_check()))
		goto no_check;
	goto done;
      no_check:
	unregister_netdevice_notifier(&strlock_netdev_notifier);
      no_notify:
	crypto_free_hash(strlock_tfm[1]);
      no_sha1:
	crypto_free_hash(strlock_tfm[0]);
      done:
	return (err);

}

void
strlock_exit(void)
{
	unregister_netdevice_notifier(&strlock_netdev_notifier);
	crypto_free_hash(strlock_tfm[1]);
	crypto_free_hash(strlock_tfm[0]);
}

#else				/* defined WITH_SPECFS_LOCK */

STATIC streams_fastcall int
strlock_open_check(void)
{
	return (0);
}

EXPORT_SYMBOL_GPL(strlock_open_check);

int
strlock_init(void)
{
	return (0);
}

void
strlock_exit(void)
{
}

#endif				/* defined WITH_SPECFS_LOCK */
