/*****************************************************************************

 @(#) $RCSfile: cdiapi.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-07-06 14:58:21 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2008-07-06 14:58:21 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: cdiapi.c,v $
 Revision 0.9.2.5  2008-07-06 14:58:21  brian
 - improvements

 Revision 0.9.2.4  2008-07-01 12:06:40  brian
 - updated manual pages, added new API library headers and impl files

 Revision 0.9.2.3  2008-04-25 11:39:33  brian
 - updates to AGPLv3

 Revision 0.9.2.2  2007/08/14 03:31:17  brian
 - GPLv3 header update

 Revision 0.9.2.1  2007/01/05 06:14:55  brian
 - added GCOM api files, doc updates

 *****************************************************************************/

#ident "@(#) $RCSfile: cdiapi.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-07-06 14:58:21 $"

static char const ident[] =
    "$RCSfile: cdiapi.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-07-06 14:58:21 $";

/*
 * This is an OpenSS7 implementation of the GCOM cdiapi library.  It builds
 * and installs as both a shared object library as well as a static library.
 * GCOM originally implemented this library as a static library only called
 * cdiapi.a.
 */

/* This file can be processed with doxygen(1). */

/** @weakgroup cdi OpenSS7 CDI API Library
  * @{ */

/** @file
  * OpenSS7 Communications Device Interface (CDI) API Library implementation file. */

#define _XOPEN_SOURCE	600
#define _REENTRANT
#define _THREAD_SAFE

#define NEED_T_USCALAR_T	1

#include <stdlib.h>
#include "gettext.h"
#include <sys/types.h>
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#include <sys/stropts.h>
#include <sys/poll.h>
#include <fcntl.h>

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

#ifndef __EXCEPTIONS
#define __EXCEPTIONS	1
#endif

#include <unistd.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stropts.h>
#include <pthread.h>
#include <linux/limits.h>
#include <values.h>
#include <syslog.h>
#include <stdarg.h>

#ifndef __P
#define __P(__prototype) __prototype
#endif

#include <sys/cdi.h>
#include <cdiapi.h>

#if defined __i386__ || defined __x86_64__ || defined __k8__
#define fastcall __attribute__((__regparm__(3)))
#else
#define fastcall
#endif

#define __hot __attribute__((section(".text.hot")))
#define __unlikely __attribute__((section(".text.unlikely")))

#if __GNUC__ < 3
#define inline inline fastcall __hot
#define noinline extern fastcall __unlikely
#else
#define inline inline __attribute__((always_inline)) fastcall __hot
#define noinline static __attribute__((noinline)) fastcall __unlikely
#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#undef min
#define min(a, b) (a < b ? a : b)

/** @brief thread-specific data.
  */
struct __cdi_tsd {
	int cerrno;
	unsigned char strbuf[BUFSIZ]; /* string buffer */
	int data_cnt;
	int ctl_cnt;
	unsigned char data_buf[CDI_DATA_BUF_SIZE];
	unsigned char ctl_buf[CDI_CTL_BUF_SIZE];
};

/** @brief once condition for Thread-Specific Data key creation.
  */
static pthread_once_t __cdi_tsd_once = PTHREAD_ONCE_INIT;

/** @brief CDI Library Thread-Specific Data Key
  */
static pthread_key_t __cdi_tsd_key = 0;

static void
__cdi_tsd_free(void *buf)
{
	pthread_setspecific(__cdi_tsd_key, NULL);
	free(buf);
}

static void
__cdi_tsd_alloc(void)
{
	int ret;
	void *buf;

	ret = pthread_key_create(&__cdi_tsd_key, __cdi_tsd_free);
	buf = malloc(sizeof(struct __cdi_tsd));
	bzero(buf, sizeof(*buf));
	ret = pthread_setspecific(__cdi_tsd_key, buf);
	return;
}

static struct __cdi_tsd *
__cdi_get_tsd(void)
{
	pthread_once(&__cdi_tsd_once, __cdi_tsd_alloc);
	return (struct __cdi_tsd *) pthread_getspecific(__cdi_tsd_key);
}

/** @brief #cerrno location function.
  * @version CDIAPI_1.0
  * @par Alias:
  * This function is an implementation of _cerrno().
  */
__hot int *
__cdi__cerrno(void)
{
	return (&__cdi_get_tsd()->cerrno);
}

/** @fn int *_cerrno(void)
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi__cerrno().
  *
  * This function provides the location of the integer that contains the CDI
  * API error number returned by the last CDI API function that failed.  This
  * is normally used to provide #cerrno in a thread-safe way as follows:
  *
  * @code
  * #define cerrno (*(_cerrno()))
  * @endcode
  */
__asm__(".symver __cdi__cerrno,_cerrno@@CDIAPI_1.0");

int *
__cdi_data_cnt(void)
{
	return (&(__cdi_get_tsd()->data_cnt));
}
__asm__(".symver __cdi_data_cnt,_cdi_data_cnt@@CDIAPI_1.0");

int *
__cdi_ctl_cnt(void)
{
	return (&(__cdi_get_tsd()->ctl_cnt));
}
__asm__(".symver __cdi_ctl_cnt,_cdi_ctl_cnt@@CDIAPI_1.0");

unsigned char *
__cdi_data_buf(void)
{
	return (__cdi_get_tsd()->data_buf);
}
__asm__(".symver __cdi_data_buf,_cdi_data_buf@@CDIAPI_1.0");

unsigned char *
__cdi_ctl_buf(void)
{
	return (__cdi_get_tsd()->ctl_buf);
}
__asm__(".symver __cdi_ctl_buf,_cdi_ctl_buf@@CDIAPI_1.0");

/** @brief put control and data
  * @param fd the CDI Stream.
  * @param ctrl_ptr control portion data buffer.
  * @param ctrl_length length of control buffer.
  * @param data_ptr data portion data buffer.
  * @param data_length length of data buffer.
  * @param flags retry on interrupt.
  *
  * This is really just a ridiculous wrapper for putmsg(2s).
  */
int
__cdi_put_both(int fd, char *ctrl_ptr, int ctrl_length, char *data_ptr, int data_length, int flags)
{
	struct strbuf ctrl, data, *ctl = NULL, *dat = NULL;
	int ret;

	if (ctrl_length >= 0) {
		ctrl.buf = ctrl_ptr;
		ctrl.len = ctrl_length;
		ctrl.maxlen = -1;
		ctl = &ctrl;
	}
	if (data_length >= 0) {
		data.buf = data_ptr;
		data.len = data_length;
		data.maxlen = -1;
		dat = &data;
	}
	while ((ret = putmsg(fd, ctl, dat, 0)) < 0) {
		if (flags & RetryOnSignal) {
			switch (errno) {
			case EAGAIN:
			case EINTR:
			case ERESTART:
				continue;
			default:
				break;
			}
		}
		break;
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_put_both().
  * @param fd the CDI Stream.
  * @param ctrl_ptr control portion data buffer.
  * @param ctrl_length length of control buffer.
  * @param data_ptr data portion data buffer.
  * @param data_length length of data buffer.
  * @param flags retry on interrupt.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of cdi_put_both().
  */
int
__cdi_put_both_r(int fd, char *ctrl_ptr, int ctrl_length, char *data_ptr, int data_length,
		 int flags)
{
	return __cdi_put_both(fd, ctrl_ptr, ctrl_length, data_ptr, data_length, flags);
}

/** @fn int cdi_put_both(int fd, char *ctrl_ptr, int ctrl_length, char *data_ptr, int data_length, int flags)
  * @param fd the CDI Stream.
  * @param ctrl_ptr control portion data buffer.
  * @param ctrl_length length of control buffer.
  * @param data_ptr data portion data buffer.
  * @param data_length length of data buffer.
  * @param flags retry on interrupt.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_put_both_r().
  */
__asm__(".symver __cdi_put_both_r,cdi_put_both@@CDIAPI_1.0");

/** @brief put a control message
  * @param fd the CDI Stream.
  * @param nbytes the number of bytes in the control message.
  * @param flags whether to retry on interrupt.
  *
  * This is really just a ridiculous wrapper for putmsg(2s).
  */
int
__cdi_put_proto(int fd, int nbytes, long flags)
{
	return __cdi_put_both(fd, (char *)cdi_ctl_buf, nbytes, NULL, -1, flags);
}

/** @brief The reentrant version of __cdi_put_proto().
  * @param fd the CDI Stream.
  * @param nbytes the number of bytes in the control message.
  * @param flags whether to retry on interrupt.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of cdi_put_proto().
  */
int
__cdi_put_proto_r(int fd, int nbytes, long flags)
{
	return __cdi_put_proto(fd, nbytes, flags);
}

/** @fn cdi_put_proto(int fd, int nbytes, long retry)
  * @param fd the CDI Stream.
  * @param nbytes the number of bytes in the control message.
  * @param flags whether to retry on interrupt.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_put_proto_r().
  */
__asm__(".symver __cdi_put_proto_r,cdi_put_proto@@CDIAPI_1.0");

/** @brief put a data message
  * @param fd of the CDI Stream.
  * @param buf the buffer containing the data message.
  * @param nbytes the number of bytes in the data message.
  * @param flags whether to retry on interrupt.
  *
  * This is really just a ridiculous wrapper for putmsg(2s).
  */
int
__cdi_put_data(int fd, char *buf, int nbytes, long flags)
{
	return __cdi_put_both(fd, NULL, -1, buf, nbytes, flags);
}

/** @brief The reentrant version of __cdi_put_data().
  * @param fd of the CDI Stream.
  * @param buf the buffer containing the data message.
  * @param nbytes the number of bytes in the data message.
  * @param flags whether to retry on interrupt.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of cdi_put_data().
  */
int
__cdi_put_data_r(int fd, char *buf, int nbytes, long flags)
{
	return __cdi_put_data(fd, buf, nbytes, flags);
}

/** @fn int cdi_put_data(int fd, char *buf, int nbytes, long flags)
  * @param fd of the CDI Stream.
  * @param buf the buffer containing the data message.
  * @param nbytes the number of bytes in the data message.
  * @param flags whether to retry on interrupt.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_put_data_r().
  */
__asm__(".symver __cdi_put_data_r,cdi_put_data@@CDIAPI_1.0");

/** @brief put CD_ALLOW_INPUT_REQ primitive
  * @param fd the CDI Stream.
  */
int
__cdi_put_allow_input_req(int fd)
{
	cd_allow_input_req_t *p;
	int ret;

	p = (typeof(p)) cdi_ctl_buf;
	cdi_ctl_cnt = sizeof(*p);

	p->cd_primitive = CD_ALLOW_INPUT_REQ;

	if ((ret = __cdi_put_proto(fd, cdi_ctl_cnt, 0))) {
		/* FIXME */
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_put_allow_input_req().
  * @param fd the CDI Stream.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of cdi_put_allow_input_req().
  */
int
__cdi_put_allow_input_req_r(int fd)
{
	return __cdi_put_allow_input_req(fd);
}

/** @fn int cdi_put_allow_input_req(int fd)
  * @param fd the CDI Stream.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_put_allow_input_req_r().
  */
__asm__(".symver __cdi_put_allow_input_req_r,cdi_put_allow_input_req@@CDIAPI_1.0");

/** @brief put a CD_ATTACH_REQ primitive
  * @param fd the CDI Stream.
  * @param ppa the Physcial Point of Attachment.
  */
int
__cdi_put_attach_req(int fd, long ppa)
{
	cd_attach_req_t *p;
	int ret;

	p = (typeof(p)) cdi_ctl_buf;
	cdi_ctl_cnt = sizeof(*p);

	p->cd_primitive = CD_ATTACH_REQ;
	p->cd_ppa = ppa;

	if ((ret = __cdi_put_proto(fd, cdi_ctl_cnt, 0))) {
		/* FIXME */
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_put_attach_req().
  * @param fd the CDI Stream.
  * @param ppa the Physcial Point of Attachment.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of cdi_put_attach_req().
  */
int
__cdi_put_attach_req_r(int fd, long ppa)
{
	return __cdi_put_attach_req(fd, ppa);
}

/** @fn int cdi_put_attach_req(int fd, long ppa)
  * @param fd the CDI Stream.
  * @param ppa the Physcial Point of Attachment.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_put_attach_req_r().
  */
__asm__(".symver __cdi_put_attach_req_t,cdi_put_attach_req@@CDIAPI_1.0");

/** @brief put a CD_DETACH_REQ primitive
  * @param fd the CDI Stream.
  */
int
__cdi_put_detach_req(int fd)
{
	cd_detach_req_t *p;
	int ret;

	p = (typeof(p)) cdi_ctl_buf;
	cdi_ctl_cnt = sizeof(*p);

	p->cd_primitive = CD_DETACH_REQ;

	if ((ret = __cdi_put_proto(fd, cdi_ctl_cnt, 0))) {
		/* FIXME */
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_put_detach_req().
  * @param fd the CDI Stream.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of cdi_put_detach_req().
  */
int
__cdi_put_detach_req_r(int fd)
{
	return __cdi_put_detach_req(fd);
}

/** @fn int cdi_put_detach_req(int fd)
  * @param fd the CDI Stream.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_put_detach_req_r().
  */
__asm__(".symver __cdi_put_detach_req_r,cdi_put_detach_req@@CDIAPI_1.0");

/** @brief put a CD_DISABLE_REQ primitive
  * @param fd the CDI Stream.
  * @param disposal disposal type for queued messages.
  *
  * #disposal can be %CD_FLUSH, %CD_WAIT or %CD_DELIVER.
  */
int
__cdi_put_disable_req(int fd, unsigned long disposal)
{
	cd_disable_req_t *p;
	int ret;

	p = (typeof(p)) cdi_ctl_buf;
	cdi_ctl_cnt = sizeof(*p);

	p->cd_primitive = CD_DISABLE_REQ;
	p->cd_disposal = disposal;

	if ((ret = __cdi_put_proto(fd, cdi_ctl_cnt, 0))) {
		/* FIXME */
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_put_disable_req().
  * @param fd the CDI Stream.
  * @param disposal disposal type for queued messages.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of cdi_put_disable_req().
  */
int
__cdi_put_disable_req_r(int fd, unsigned long disposal)
{
	return __cdi_put_disable_req(fd, disposal);
}

/** @fn int cdi_put_disable_req(int fd, unsigned long disposal)
  * @param fd the CDI Stream.
  * @param disposal disposal type for queued messages.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_put_disable_req_r().
  */
__asm__(".symver __cdi_put_disable_req_r,cdi_put_disable_req@@CDIAPI_1.0");

/** @brief put a CD_ENABLE_REQ primitive with dial string.
  * @param fd the CDI Stream.
  * @param dial_string the dial string (or NULL).
  * @param dial_length the dial string length (or zero).
  */
int
__cdi_put_dial_req(int fd, char *dial_string, int dial_length)
{
	cd_enable_req_t *p;
	int ret;

	p = (typeof(p)) cdi_ctl_buf;
	cdi_ctl_cnt = sizeof(*p);

	p->cd_primitive = CD_ENABLE_REQ;
	p->cd_dial_type = dial_string ? 0 : CD_NODIAL;
	p->cd_dial_length = dial_length;
	p->cd_dial_offset = dial_length ? sizeof(*p) : 0;

	if ((ret = __cdi_put_proto(fd, cdi_ctl_cnt, 0)) < 0) {
		/* FIXME */
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_put_dial_req().
  * @param fd the CDI Stream.
  * @param dial_string the dial string (or NULL).
  * @param dial_length the dial string length (or zero).
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_put_dial_req().
  */
int
__cdi_put_dial_req_r(int fd, char *dial_string, int dial_length)
{
	return __cdi_put_dial_req(fd, dial_string, dial_length);
}

/** @fn int cdi_put_dial_req(int fd, char *dial_string, int dial_length)
  * @param fd the CDI Stream.
  * @param dial_string the dial string (or NULL).
  * @param dial_length the dial string length (or zero).
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_put_dial_req_r().
  */
__asm__(".symver __cdi_put_dial_req_r,cdi_put_dial_req@@CDIAPI_1.0");

/** @brief put a CD_ENABLE_REQ primitive
  * @param fd the CDI Stream.
  */
int
__cdi_put_enable_req(int fd)
{
	return __cdi_put_dial_req(fd, NULL, 0);
}

/** @brief The reentrant version of __cdi_put_enable_req().
  * @param fd the CDI Stream.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is an implementation of cdi_put_enable().
  */
int
__cdi_put_enable_req_r(int fd)
{
	return __cdi_put_enable_req(fd);
}

/** @fn int cdi_put_enable_req(int fd)
  * @param fd the CDI Stream.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_put_enable_req_r().
  */
__asm__(".symver __cdi_put_enable_req_r,cdi_put_enable_req@@CDIAPI_1.0");

/** @brief put a frame
  * @param fd the CDI Stream.
  * @param address the address field.
  * @param control the control field.
  * @param buf pointer to data buffer.
  * @param nbytes the number of bytes in the data buffer.
  */
int
__cdi_put_frame(int fd, unsigned char address, unsigned char control, unsigned char *buf, int nbytes)
{
	cdi_data_buf[0] = address;
	cdi_data_buf[1] = control;
	if (nbytes > CDI_DATA_BUF_SIZE - 2) {
		errno = EMSGSIZE;
		return (-1);
	}
	cdi_data_cnt = nbytes + 2;
	strncpy((char *)&cdi_data_buf[2], (char *)buf, nbytes);
	return __cdi_put_data(fd, (char *)cdi_data_buf, cdi_data_cnt, 0);
}

/** @brief The reentrant version of __cdi_put_frame().
  * @param fd the CDI Stream.
  * @param address the address field.
  * @param control the control field.
  * @param buf pointer to data buffer.
  * @param nbytes the number of bytes in the data buffer.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_put_frame().
  */
int
__cdi_put_frame_r(int fd, unsigned char address, unsigned char control, unsigned char *buf, int nbytes)
{
	return __cdi_put_frame(fd, address, control, buf, nbytes);
}

/** @fn int cdi_put_frame(int fd, unsigned char address, unsigned char control, unsigned char *buf, int nbytes)
  * @param fd the CDI Stream.
  * @param address the address field.
  * @param control the control field.
  * @param buf pointer to data buffer.
  * @param nbytes the number of bytes in the data buffer.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_put_frame_r().
  */
__asm__(".symver __cdi_put_frame_r,cdi_put_frame@@CDIAPI_1.0");

/** @brief put a CD_MODEM_SIG_POLL primitive
  * @param fd the CDI Stream.
  */
int
__cdi_modem_sig_poll(int fd)
{
	cd_modem_sig_poll_t *p;
	int ret;

	p = (typeof(p)) cdi_ctl_buf;
	cdi_ctl_cnt = sizeof(*p);

	p->cd_primitive = CD_MODEM_SIG_POLL;

	if ((ret = __cdi_put_proto(fd, cdi_ctl_cnt, 0))) {
		/* FIXME */
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_modem_sig_poll().
  * @param fd the CDI Stream.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_modem_sig_poll().
  */
int
__cdi_modem_sig_poll_r(int fd)
{
	return __cdi_modem_sig_poll(fd);
}

/** @fn int cid_modem_sig_poll(int fd)
  * @param fd the CDI Stream.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_modem_sig_poll_r().
  */
__asm__(".symver __cdi_modem_sig_poll_r,cdi_modem_sig_poll@@CDIAPI_1.0");

/** @brief get a message
  * @param fd the CDI Stream.
  * @param buf buffer for data portion of message.
  * @param nbytes size of buffer.
  */
int
__cdi_get_a_msg(int fd, char *buf, int nbytes)
{
	struct strbuf ctrl, data;
	int flags;
	int ret;

	ctrl.buf = (char *)cdi_ctl_buf;
	ctrl.len = -1;
	ctrl.maxlen = CDI_CTL_BUF_SIZE;

	data.buf = buf;
	data.len = -1;
	data.maxlen = nbytes;

	if ((ret = getmsg(fd, &ctrl, &data, &flags)) < 0) {
		/* FIXME */
		return (ret);
	}
	cdi_ctl_cnt = ctrl.len;
	cdi_data_cnt = data.len;
	return (ret);
}

/** @brief The reentrant version of __cdi_get_a_msg().
  * @param fd the CDI Stream.
  * @param buf buffer for data portion of message.
  * @param nbytes size of buffer.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_get_a_msg().
  */
int
__cdi_get_a_msg_r(int fd, char *buf, int nbytes)
{
	return __cdi_get_a_msg_r(fd, buf, nbytes);
}

/** @fn int cdi_get_a_msg(int fd, char *buf, int nbytes)
  * @param fd the CDI Stream.
  * @param buf buffer for data portion of message.
  * @param nbytes size of buffer.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_get_a_msg_r().
  */
__asm__(".symver __cdi_get_a_msg_r,cdi_get_a_msg@@CDIAPI_1.0");

/** @brief receive a message.
  * @param fd the CDI Stream.
  * @param buf data buffer.
  * @param nbytes length of data buffer.
  * @param flags expected message types.
  */
int
__cdi_rcv_msg(int fd, char *buf, int nbytes, long flags)
{
	int ret;

	for (;;) {
		while ((ret = __cdi_get_a_msg(fd, buf, nbytes)) < 0) {
			if (flags & RetryOnSignal) {
				switch (errno) {
				case EAGAIN:
				case EINTR:
				case ERESTART:
					continue;
				default:
					break;
				}
			}
			return (ret);
		}
		if (cdi_ctl_cnt > 0) {
			switch (*(cd_ulong *) cdi_ctl_buf) {
			case CD_ERROR_ACK:
				if (flags & Return_error_ack)
					return (0);
				break;
			case CD_INFO_ACK:
				if (flags & Return_info_ack)
					return (0);
				break;
			case CD_UNITDATA_ACK:
				if (flags & Return_unidata_ack)
					return (0);
				break;
			case CD_ERROR_IND:
				if (flags & Return_error_ind)
					return (0);
				break;
			case CD_DISABLE_CON:
				if (flags & Return_disable_con)
					return (0);
				break;
			case CD_ENABLE_CON:
				if (flags & Return_enable_con)
					return (0);
				break;
			case CD_OK_ACK:
				if (flags & Return_ok_ack)
					return (0);
				break;
			case CD_BAD_FRAME_IND:
				if (flags & Return_bad_frame_ind)
					return (0);
				break;
			case CD_MODEM_SIG_IND:
				if (flags & Return_modem_sig_ind)
					return (0);
				break;
			}
		}
		if (cdi_data_cnt > 0) {
			return (cdi_data_cnt);
		}
	}
}

/** @brief The reentrant version of __cdi_rcv_msg().
  * @param fd the CDI Stream.
  * @param buf data buffer.
  * @param nbytes length of data buffer.
  * @param flags expected message types.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_rcv_msg().
  */
int
__cdi_rcv_msg_r(int fd, char *buf, int nbytes, long flags)
{
	return __cdi_rcv_msg_r(fd, buf, nbytes, flags);
}

/** @fn int cdi_rcv_msg(int fd, char *buf, int nbytes, long flags)
  * @param fd the CDI Stream.
  * @param buf data buffer.
  * @param nbytes length of data buffer.
  * @param flags expected message types.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_rcv_msg_r().
  */
__asm__(".symver __cdi_rcv_msg_r,cdi_rcv_msg@@CDIAPI_1.0");

/** @brief read data
  * @param fd the CDI Stream.
  * @param buf the data buffer into which to read.
  * @param nbytes the size of the data buffer.
  */
int
__cdi_read_data(int fd, char *buf, int nbytes)
{
	return __cdi_rcv_msg(fd, buf, nbytes, -1UL);
}

/** @brief The reentrant version of __cdi_read_data().
  * @param fd the CDI Stream.
  * @param buf the data buffer into which to read.
  * @param nbytes the size of the data buffer.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_read_data().
  */
int
__cdi_read_data_r(int fd, char *buf, int nbytes)
{
	return __cdi_read_data(fd, buf, nbytes);
}

/** @fn int cdi_read_data(int fd, char *buf, int nbytes)
  * @param fd the CDI Stream.
  * @param buf the data buffer into which to read.
  * @param nbytes the size of the data buffer.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_read_data_r().
  */
__asm__(".symver __cdi_read_data_r,cdi_read_data@@CDIAPI_1.0");

/** @brief write data to CDI Stream.
  * @param fd the CDI Stream.
  * @param buf the data buffer to write.
  * @param nbytes the length of the data buffer to write.
  */
int
__cdi_write_data(int fd, char *buf, int nbytes)
{
	return __cdi_put_data(fd, buf, nbytes, 0);
}

/** @brief The reentrant version of __cdi_write_data().
  * @param fd the CDI Stream.
  * @param buf the data buffer to write.
  * @param nbytes the length of the data buffer to write.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_write_data().
  */
int
__cdi_write_data_r(int fd, char *buf, int nbytes)
{
	return __cdi_write_data(fd, buf, nbytes);
}

/** @fn int cdi_write_data(int fd, char *buf, int nbytes)
  * @param fd the CDI Stream.
  * @param buf the data buffer to write.
  * @param nbytes the length of the data buffer to write.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_write_data_r().
  */
__asm__(".symver __cdi_write_data_r,cdi_write_data@@CDIAPI_1.0");

/** @brief wait for an acknowlegement.
  * @param fd the CDI Stream.
  * @param primitive the expected primitive.
  * @param state_ptr a pointer to state variable (or NULL).
  */
int
__cdi_wait_ack(int fd, long primitive, int *state_ptr)
{
	long flags = RetryOnSignal;
	int ret;

	switch (primitive) {
	case CD_INFO_ACK:
		flags |= Return_error_ack | Return_info_ack;
		break;
	case CD_UNITDATA_ACK:
		flags |= Return_error_ack | Return_unidata_ack;
		break;
	case CD_ERROR_IND:
		flags |= Return_error_ind;
		break;
	case CD_DISABLE_CON:
		flags |= Return_error_ack | Return_disable_con;
		break;
	case CD_ENABLE_CON:
		flags |= Return_error_ack | Return_enable_con;
		break;
	case CD_OK_ACK:
		flags |= Return_error_ack | Return_ok_ack;
		break;
	case CD_BAD_FRAME_IND:
		flags |= Return_bad_frame_ind;
		break;
	default:
		errno = EINVAL;
		return (-1);
	}

	while ((ret = __cdi_rcv_msg(fd, (char *)cdi_data_buf, CDI_DATA_BUF_SIZE, flags)) > 0) ;
	if (ret < 0) {
		/* FIXME */
		return (ret);
	}
	/* Have expected control primitive. */
	if (state_ptr != NULL) {
		/* cd_state is always in the second cd_ulong location */
		*state_ptr = ((cd_ulong *) cdi_ctl_buf)[1];
	}
	return (*(cd_ulong *) cdi_ctl_buf == (cd_ulong) primitive);
}

/** @brief The reentrant version of __cdi_wait_ack().
  * @param fd the CDI Stream.
  * @param primitive the expected primitive.
  * @param state_ptr a pointer to state variable (or NULL).
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_wait_ack().
  */
int
__cdi_wait_ack_r(int fd, long primitive, int *state_ptr)
{
	return __cdi_wait_ack(fd, primitive, state_ptr);
}

/** @fn int cdi_wait_ack(int fd, long primitive, int *state_ptr)
  * @param fd the CDI Stream.
  * @param primitive the expected primitive.
  * @param state_ptr a pointer to state variable (or NULL).
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_wait_ack_r().
  */
__asm__(".symver __cdi_wait_ack_r,cdi_wait_ack@@CDIAPI_1.0");

/** @brief open a CDI Stream.
  */
int
__cdi_open_data(void)
{
	int fd;

	if ((fd = open("/dev/streams/clone/cdmux", O_RDWR)) < 0) {
		/* FIXME */
	}
	return (fd);
}

/** @brief The reentrant version of __cdi_open_data().
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_open_data().
  */
int
__cdi_open_data_r(void)
{
	return __cdi_open_data();
}

/** @fn int cdi_open_data(void)
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_open_data_r().
  */
__asm__(".symver __cdi_open_data_r,cdi_open_data@@CDIAPI_1.0");

/** @brief open a CDI Stream.
  * @param hostname name of remote host (or NULL).
  */
int
__cdi_open(char *hostname)
{
	if (hostname != NULL) {
		errno = ENOTSUP;
		return (-1);
	}
	return __cdi_open_data();
}

/** @brief The reentrant version of __cdi_open().
  * @param hostname name of remote host (or NULL).
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_open().
  */
int
__cdi_open_r(char *hostname)
{
	return __cdi_open(hostname);
}

/** @fn int cdi_open(char *hostname)
  * @param hostname name of remote host (or NULL).
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_open_r().
  */
__asm__(".symver __cdi_open_r,cdi_open@@CDIAPI_1.0");

/** @brief close a CDI Stream.
  * @param fd the CDI Stream.
  */
int
__cdi_close(int fd)
{
	int ret;

	if ((ret = close(fd)) < 0) {
		/* FIXME */
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_close().
  * @param fd the CDI Stream.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implemnetation of cdi_close().
  */
int
__cdi_close_r(int fd)
{
	return __cdi_close(fd);
}

/** @fn int cdi_close(int fd)
  * @param fd the CDI Stream.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_close_r().
  */
__asm__(".symver __cdi_close_r,cdi_close@@CDIAPI_1.0");

/** @brief initialize CDI library with log file.
  * @param log_options logging options flags.
  * @param log_file file stream of open log file.
  */
int
__cdi_init_FILE(int log_options, FILE *log_file)
{
	return (0); /* FIXME */
}

/** @brief The reentrant version of __cdi_init_FILE().
  * @param log_options logging options flags.
  * @param log_file file stream of open log file.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_init_FILE().
  */
int
__cdi_init_FILE_r(int log_options, FILE *log_file)
{
	return __cdi_init_FILE(log_options, log_file);
}

/** @fn int cdi_init_FILE(int log_options, FILE *log_file)
  * @param log_options logging options flags.
  * @param log_file file stream of open log file.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_init_FILE_r().
  */
__asm__(".symver __cdi_init_FILE_r,cdi_init_FILE@@CDIAPI_1.0");

/** @brief initialize CDI library.
  * @param log_options logging options flags.
  * @param log_name name of log file (or NULL for default).
  */
int
__cdi_init(int log_options, char *log_name)
{
	FILE *f;
	int ret;

	if (log_name == NULL) {
		log_name = "cdilogfile";
	}
	if ((f = fopen(log_name, "+w")) == NULL) {
		/* FIXME */
		return (-1);
	}
	if ((ret = __cdi_init_FILE(log_options, f)) < 0) {
		/* FIXME */
		fclose(f);
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_init().
  * @param log_options logging options flags.
  * @param log_name name of log file (or NULL for default).
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_init().
  */
int
__cdi_init_r(int log_options, char *log_name)
{
	return __cdi_init(log_options, log_name);
}

/** @fn int cdi_init(int log_options, char *log_name)
  * @param log_options logging options flags.
  * @param log_name name of log file (or NULL for default).
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_init_r().
  */
__asm__(".symver __cdi_init_r,cdi_init@@CDIAPI_1.0");

/** @brief set log file size.
  * @param nbytes maximum size of log file (<=0 infinite).
  */
int
__cdi_set_log_size(long nbytes)
{
	if (nbytes > 0) {
		errno = EINVAL;
		return (-1);
	}
	return (0);
}

/** @brief The reentrant version of __cdi_set_log_size().
  * @param nbytes maximum size of log file (<=0 infinite).
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_set_log_size().
  */
int
__cdi_set_log_size_r(long nbytes)
{
	return __cdi_set_log_size(nbytes);
}

/** @fn int cdi_set_log_size(long nbytes)
  * @param nbytes maximum size of log file (<=0 infinite).
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_set_log_size_r().
  */
__asm__(".symver __cdi_set_log_size_r,cdi_set_log_size@@CDIAPI_1.0");

/** @brief sent CD_ALLOW_INPUT_REQ and wait for acknowlegement.
  * @param fd the CDI Stream.
  * @param state_ptr location for returned state variable.
  */
int
__cdi_allow_input_req(int fd, int *state_ptr)
{
	int ret;

	if ((ret = __cdi_put_allow_input_req(fd)) < 0) {
		/* FIXME */
		return (ret);
	}
	if ((ret = __cdi_wait_ack(fd, CD_OK_ACK, state_ptr)) < 0) {
		/* FIXME */
		return (ret);
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_allow_input_req().
  * @param fd the CDI Stream.
  * @param state_ptr location for returned state variable.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_allow_input_req().
  */
int
__cdi_allow_input_req_r(int fd, int *state_ptr)
{
	return __cdi_allow_input_req(fd, state_ptr);
}

/** @fn int cdi_allow_input_req(int fd, int *state_ptr)
  * @param fd the CDI Stream.
  * @param state_ptr location for returned state variable.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_allow_input_req_r().
  */
__asm__(".symver __cdi_allow_input_req_r,cdi_allow_input_req@@CDIAPI_1.0");

/** @brief send CD_ATTACH_REQ and await acknowledgement.
  * @param fd the CDI Stream.
  * @param ppa the Physical Point of Attachment.
  * @param state_ptr location for returned state variable.
  */
int
__cdi_attach_req(int fd, long ppa, int *state_ptr)
{
	int ret;

	if ((ret = __cdi_put_attach_req(fd, ppa)) < 0) {
		/* FIXME */
		return (ret);
	}
	if ((ret = __cdi_wait_ack(fd, CD_OK_ACK, state_ptr)) < 0) {
		/* FIXME */
		return (ret);
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_attach_req().
  * @param fd the CDI Stream.
  * @param ppa the Physical Point of Attachment.
  * @param state_ptr location for returned state variable.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_attach_req().
  */
int
__cdi_attach_req_r(int fd, long ppa, int *state_ptr)
{
	return __cdi_attach_req(fd, ppa, state_ptr);
}

/** @fn int cdi_attach_req(int fd, long ppa, int *state_ptr)
  * @param fd the CDI Stream.
  * @param ppa the Physical Point of Attachment.
  * @param state_ptr location for returned state variable.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_attach_req_r().
  */
__asm__(".symver __cdi_attach_req_r,cdi_attach_req@@CDIAPI_1.0");

/** @brief send CD_DETACH_REQ and await acknowledgement.
  * @param fd the CDI Stream.
  * @param state_ptr location for returned state variable.
  */
int
__cdi_detach_req(int fd, int *state_ptr)
{
	int ret;

	if ((ret = __cdi_put_detach_req(fd)) < 0) {
		/* FIXME */
		return (ret);
	}
	if ((ret = __cdi_wait_ack(fd, CD_OK_ACK, state_ptr)) < 0) {
		/* FIXME */
		return (ret);
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_detach_req().
  * @param fd the CDI Stream.
  * @param state_ptr location for returned state variable.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_detach_req().
  */
int
__cdi_detach_req_r(int fd, int *state_ptr)
{
	return __cdi_detach_req(fd, state_ptr);
}

/** @fn int cdi_detach_req(int fd, int *state_ptr)
  * @param fd the CDI Stream.
  * @param state_ptr location for returned state variable.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_detach_req_r().
  */
__asm__(".symver __cdi_detach_req_r,cdi_detach_req@@CDIAPI_1.0");

/** @brief send CDI_DISABLE_REQ and await acknolwedgement.
  * @param fd the CDI Stream.
  * @param displosal the disposal method for unacknolwedged frames.
  * @param state_ptr location for returned state variable.
  */
int
__cdi_disable_req(int fd, unsigned long disposal, int *state_ptr)
{
	int ret;

	if ((ret = __cdi_put_disable_req(fd, disposal)) < 0) {
		/* FIXME */
		return (ret);
	}
	if ((ret = __cdi_wait_ack(fd, CD_DISABLE_CON, state_ptr)) < 0) {
		/* FIXME */
		return (ret);
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_disable_req().
  * @param fd the CDI Stream.
  * @param displosal the disposal method for unacknolwedged frames.
  * @param state_ptr location for returned state variable.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_disable_req().
  */
int
__cdi_disable_req_r(int fd, unsigned long disposal, int *state_ptr)
{
	return __cdi_disable_req(fd, disposal, state_ptr);
}

/** @fn int cdi_disable_req(int fd, unsigned long disposal, int *state_ptr)
  * @param fd the CDI Stream.
  * @param displosal the disposal method for unacknolwedged frames.
  * @param state_ptr location for returned state variable.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_disable_req_r().
  */
__asm__(".symver __cdi_disable_req_r,cdi_disable_req@@CDIAPI_1.0");

/** @brief send CD_ENABLE_REQ and await acknowledgement.
  * @param fd the CDI Stream.
  * @param state_ptr location for returned state variable.
  */
int
__cdi_enable_req(int fd, int *state_ptr)
{
	int ret;

	if ((ret = __cdi_put_enable_req(fd)) < 0) {
		/* FIXME */
		return (ret);
	}
	if ((ret = __cdi_wait_ack(fd, CD_ENABLE_CON, state_ptr)) < 0) {
		/* FIXME */
		return (ret);
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_enable_req().
  * @param fd the CDI Stream.
  * @param state_ptr location for returned state variable.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_enable_req().
  */
int
__cdi_enable_req_r(int fd, int *state_ptr)
{
	return __cdi_enable_req(fd, state_ptr);
}

/** @fn int cdi_enable_req(int fd, int *state_ptr)
  * @param fd the CDI Stream.
  * @param state_ptr location for returned state variable.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_enable_req_r().
  */
__asm__(".symver __cdi_enable_req_r,cdi_enable_req@@CDIAPI_1.0");

/** @brief send CD_MODEM_SIG_POLL and await response.
  * @param fd the CDI Stream.
  * @param flags extra flags for received control messages.
  */
int
__cdi_get_modem_sigs(int fd, int flags)
{
	int ret;

	if ((ret = __cdi_modem_sig_poll(fd)) < 0) {
		/* FIXME */
		return (ret);
	}
	flags |= Return_modem_sig_ind;
	while ((ret = __cdi_rcv_msg(fd, (char *)cdi_data_buf, CDI_DATA_BUF_SIZE, flags)) > 0) ;
	if (ret < 0) {
		/* FIXME */
		return (ret);
	}
	/* Have expected control primitive */
	if (*(cd_ulong *) cdi_ctl_buf == CD_MODEM_SIG_IND)
		return (((cd_modem_sig_ind_t *) cdi_ctl_buf)->cd_sigs);
	return (0);
}

/** @brief The reentrant version of __cdi_get_modem_sigs().
  * @param fd the CDI Stream.
  * @param flags extra flags for received control messages.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_get_modem_sigs().
  */
int
__cdi_get_modem_sigs_r(int fd, int flags)
{
	return __cdi_get_modem_sigs(fd, flags);
}

/** @fn int cdi_get_modem_sigs(int fd, int flags)
  * @param fd the CDI Stream.
  * @param flags extra flags for received control messages.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_get_modem_sigs_r().
  */
__asm__(".symver __cdi_get_modem_sigs_r,cdi_get_modem_sigs@@CDIAPI_1.0");

/** @brief send CD_MODEM_SIG_REQ primitive.
  * @param fd the CDI Stream.
  * @param sig the signals requested.
  */
int
__cdi_modem_sig_req(int fd, unsigned int sigs)
{
	cd_modem_sig_req_t *p;
	int ret;

	p = (typeof(p)) cdi_ctl_buf;
	cdi_ctl_cnt = sizeof(*p);

	p->cd_primitive = CD_MODEM_SIG_REQ;
	p->cd_sigs = sigs;

	if ((ret = __cdi_put_proto(fd, cdi_ctl_cnt, 0)) < 0) {
		/* FIXME */
	}
	return (ret);
}

/** @brief The reentrant version of __cdi_modem_sig_req().
  * @param fd the CDI Stream.
  * @param sig the signals requested.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_modem_sig_req().
  */
int
__cdi_modem_sig_req_r(int fd, unsigned int sigs)
{
	return __cdi_modem_sig_req(fd, sigs);
}

/** @fn int cdi_modem_sig_req(int fd, unsigned int sigs)
  * @param fd the CDI Stream.
  * @param sig the signals requested.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_modem_sig_req_r().
  */
__asm__(".symver __cdi_modem_sig_req_r,cdi_modem_sig_req@@CDIAPI_1.0");

/** @brief dial a modem.
  * @param fd the CDI Stream.
  * @param ppa the upper ppa.
  * @param dial_string the modem dial string.
  * @param dial_length the modem dial string length.
  *
  * This doesn't seem to be quite right...
  */
int
__cdi_dial_req(int fd, uint ppa, uint sigs, char *dial_string, int dial_length)
{
	int ret, state = 0;

	if ((ret = __cdi_attach_req(fd, ppa, &state)) < 0) {
		/* FIXME */
		return (ret);
	}
	if ((ret = __cdi_modem_sig_req(fd, sigs)) < 0) {
		/* FIXME */
		return (ret);
	}
	if ((ret = __cdi_put_dial_req(fd, dial_string, dial_length)) < 0) {
		/* FIXME */
		return (ret);
	}
	if ((ret = __cdi_wait_ack(fd, CD_ENABLE_CON, &state)) < 0) {
		/* FIXME */
		return (ret);
	}
	return (ret);
}

/** @brief The reentrante version of __cdi_dial_req().
  * @param fd the CDI Stream.
  * @param ppa the upper ppa.
  * @param dial_string the modem dial string.
  * @param dial_length the modem dial string length.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_dial_req().
  */
int
__cdi_dial_req_r(int fd, uint ppa, uint sigs, char *dial_string, int dial_length)
{
	return __cdi_dial_req(fd, ppa, sigs, dial_string, dial_length);
}

/** @fn int cdi_dial_req(int fd, uint ppa, uint sigs, char *dial_string, int dial_length)
  * @param fd the CDI Stream.
  * @param ppa the upper ppa.
  * @param dial_string the modem dial string.
  * @param dial_length the modem dial string length.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_dial_req_r().
  */
__asm__(".symver __cdi_dial_req_r,cdi_dial_req@@CDIAPI_1.0");

/** @brief perror() to log file.
  * @param msg the prefix message.
  *
  * Note that msg should be const char *.
  */
int
__cdi_perror(char *msg)
{
	syslog(LOG_INFO, "%s: %m", msg);
	return (0); /* XXX */
}

/** @brief the reentrant version of __cdi_perror().
  * @param msg the prefix message.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_perror().
  */
int
__cdi_perror_r(char *msg)
{
	return __cdi_perror(msg);
}

/** @fn int cdi_perror(char *msg)
  * @param msg the prefix message.
  * @verison CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_perror_r().
  */
__asm__(".symver __cdi_perror_r,cdi_perror@@CDIAPI_1.0");

/** @brief print message to log file.
  * @param fmt format string.
  */
void
__cdi_printf(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vsyslog(LOG_INFO, fmt, args);
	va_end(args);
}

/** @brief The reentrant version of __cdi_printf().
  * @param fmt format string.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_printf().
  */
void
__cdi_printf_r(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vsyslog(LOG_INFO, fmt, args);
	va_end(args);
}

/** @fn void cdi_printf(char *fmt, ...)
  * @param fmt format string.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_printf_r().
  */
__asm__(".symver __cdi_printf_r,cdi_printf@@CDIAPI_1.0");

/** @brief print data with indented hexadecimal.
  * @param buf buffer contianing data.
  * @param nbytes number of bytes to print.
  * @param indent the number of spaces to indent.
  */
int
__cdi_print_msg(unsigned char *buf, unsigned int nbytes, int indent)
{
	char tmp[BUFSIZ];
	int i, rows, cols, remaining;
	char *p = (char *) buf, *q = tmp;

	if (indent > 5)
		indent = 5;
	for (remaining = nbytes, rows = (nbytes + 15) >> 4; rows > 0; rows--, remaining -= cols) {
		cols = (remaining >= 16) ? 16 : remaining;
		for (q = tmp, i = 0; i < indent; i++, q++)
			*q = '\t';
		for (i = 0; i < cols; i++, q += 3, p++)
			sprintf(q, "%02X ", *p);
		syslog(LOG_INFO, "%s", tmp);
	}
	return (0); /* XXX */
}

/** @brief The reentrant version of __cdi_print_msg().
  * @param buf buffer contianing data.
  * @param nbytes number of bytes to print.
  * @param indent the number of spaces to indent.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_print_msg().
  */
int
__cdi_print_msg_r(unsigned char *buf, unsigned int nbytes, int indent)
{
	return __cdi_print_msg(buf, nbytes, indent);
}

/** @fn int cdi_print_msg(unsigned char *buf, unsigned int nbytes, int indent)
  * @param buf buffer contianing data.
  * @param nbytes number of bytes to print.
  * @param indent the number of spaces to indent.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_print_msg_r().
  */
__asm__(".symver __cdi_print_msg_r,cdi_print_msg@@CDIAPI_1.0");

/** @brief decode primtiive and output to syslog.
  * @param msg message to prefix to output.
  */
int
__cdi_decode_ctl(char *msg)
{
	union CD_primitives *p = (typeof(p)) msg;

	if (cdi_ctl_cnt >= sizeof(p->cd_primitive)) {
		switch (p->cd_primitive) {
		case CD_INFO_REQ:
			syslog(LOG_INFO, "%s: CD_INFO_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->info_req)) {
				/* XXX decode more */
			}
			return (0);
		case CD_INFO_ACK:
			syslog(LOG_INFO, "%s: CD_INFO_ACK", msg);
			if (cdi_ctl_cnt >= sizeof(p->info_ack)) {
				/* XXX decode more */
			}
			return (0);
		case CD_ATTACH_REQ:
			syslog(LOG_INFO, "%s: CD_ATTACH_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->attach_req)) {
				/* XXX decode more */
			}
			return (0);
		case CD_DETACH_REQ:
			syslog(LOG_INFO, "%s: CD_DETACH_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->detach_req)) {
				/* XXX decode more */
			}
			return (0);
		case CD_ENABLE_REQ:
			syslog(LOG_INFO, "%s: CD_ENABLE_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->enable_req)) {
				/* XXX decode more */
			}
			return (0);
		case CD_DISABLE_REQ:
			syslog(LOG_INFO, "%s: CD_DISABLE_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->disable_req)) {
				/* XXX decode more */
			}
			return (0);
		case CD_OK_ACK:
			syslog(LOG_INFO, "%s: CD_OK_ACK", msg);
			if (cdi_ctl_cnt >= sizeof(p->ok_ack)) {
				/* XXX decode more */
			}
			return (0);
		case CD_ERROR_ACK:
			syslog(LOG_INFO, "%s: CD_ERROR_ACK", msg);
			if (cdi_ctl_cnt >= sizeof(p->error_ack)) {
				/* XXX decode more */
			}
			return (0);
		case CD_ENABLE_CON:
			syslog(LOG_INFO, "%s: CD_ENABLE_CON", msg);
			if (cdi_ctl_cnt >= sizeof(p->enable_con)) {
				/* XXX decode more */
			}
			return (0);
		case CD_DISABLE_CON:
			syslog(LOG_INFO, "%s: CD_DISABL_CON", msg);
			if (cdi_ctl_cnt >= sizeof(p->disable_con)) {
				/* XXX decode more */
			}
			return (0);
		case CD_ERROR_IND:
			syslog(LOG_INFO, "%s: CD_ERROR_IND", msg);
			if (cdi_ctl_cnt >= sizeof(p->error_ind)) {
				/* XXX decode more */
			}
			return (0);
		case CD_ALLOW_INPUT_REQ:
			syslog(LOG_INFO, "%s: CD_ALLOW_INPUT_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->allow_input_req)) {
				/* XXX decode more */
			}
			return (0);
		case CD_READ_REQ:
			syslog(LOG_INFO, "%s: CD_READ_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->read_req)) {
				/* XXX decode more */
			}
			return (0);
		case CD_UNITDATA_REQ:
			syslog(LOG_INFO, "%s: CD_UNITDATA_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->unitdata_req)) {
				/* XXX decode more */
			}
			return (0);
		case CD_WRITE_READ_REQ:
			syslog(LOG_INFO, "%s: CD_WRITE_READ_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->write_read_req)) {
				/* XXX decode more */
			}
			return (0);
		case CD_UNITDATA_ACK:
			syslog(LOG_INFO, "%s: CD_UNITDATA_ACK", msg);
			if (cdi_ctl_cnt >= sizeof(p->unitdata_ack)) {
				/* XXX decode more */
			}
			return (0);
		case CD_UNITDATA_IND:
			syslog(LOG_INFO, "%s: CD_UNITDATA_IND", msg);
			if (cdi_ctl_cnt >= sizeof(p->unitdata_ind)) {
				/* XXX decode more */
			}
			return (0);
		case CD_HALT_INPUT_REQ:
			syslog(LOG_INFO, "%s: CD_HALT_INPUT_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->halt_input_req)) {
				/* XXX decode more */
			}
			return (0);
		case CD_ABORT_OUTPUT_REQ:
			syslog(LOG_INFO, "%s: CD_ABORT_OUTPUT_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->abort_output_req)) {
				/* XXX decode more */
			}
			return (0);
#if 0
		case CD_MUX_NAME_REQ:
			syslog(LOG_INFO, "%s: CD_MUX_NAME_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->mux_name_req)) {
				/* XXX decode more */
			}
			return (0);
#endif
		case CD_BAD_FRAME_IND:
			syslog(LOG_INFO, "%s: CD_BAD_FRAME_IND", msg);
			if (cdi_ctl_cnt >= sizeof(p->bad_frame_ind)) {
				/* XXX decode more */
			}
			return (0);
		case CD_MODEM_SIG_REQ:
			syslog(LOG_INFO, "%s: CD_MODEM_SIG_REQ", msg);
			if (cdi_ctl_cnt >= sizeof(p->modem_sig_req)) {
				/* XXX decode more */
			}
			return (0);
		case CD_MODEM_SIG_IND:
			syslog(LOG_INFO, "%s: CD_MODEM_SIG_IND", msg);
			if (cdi_ctl_cnt >= sizeof(p->modem_sig_ind)) {
				/* XXX decode more */
			}
			return (0);
		case CD_MODEM_SIG_POLL:
			syslog(LOG_INFO, "%s: CD_MODEM_SIG_POLL", msg);
			if (cdi_ctl_cnt >= sizeof(p->modem_sig_poll)) {
				/* XXX decode more */
			}
			return (0);
		}
	}
	syslog(LOG_INFO, "%s:", msg);
	return (-1);
}

/** @brief The reentrant version of __cdi_decode_ctl().
  * @param msg message to prefix to output.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_decode_ctl().
  */
int
__cdi_decode_ctl_r(char *msg)
{
	return __cdi_decode_ctl(msg);
}

/** @fn int cdi_decode_ctl(char *msg)
  * @param msg message to prefix to output.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_decode_ctl_r().
  */
__asm__(".symver __cdi_decode_ctl_r,cdi_decode_ctl@@CDIAPI_1.0");

/** @brief decode modem signals to string.
  * @param sigs the modem signals.
  */
char *
__cdi_decode_modem_sigs(uint sigs)
{
	char *mybuf = (char *) __cdi_get_tsd()->strbuf, *buf = mybuf;

	buf += sprintf(buf, "%s", (sigs & CD_DTR) ? "DTR" : "~DTR");
	buf += sprintf(buf, "%c", ',');
	buf += sprintf(buf, "%s", (sigs & CD_RTS) ? "RTS" : "~RTS");
	buf += sprintf(buf, "%c", ',');
	buf += sprintf(buf, "%s", (sigs & CD_DSR) ? "DSR" : "~DSR");
	buf += sprintf(buf, "%c", ',');
	buf += sprintf(buf, "%s", (sigs & CD_DCD) ? "DCD" : "~DCD");
	buf += sprintf(buf, "%c", ',');
	buf += sprintf(buf, "%s", (sigs & CD_CTS) ? "CTS" : "~CTS");
	buf += sprintf(buf, "%c", ',');
	buf += sprintf(buf, "%s", (sigs & CD_RI) ? "RI" : "~RI");
	return (mybuf);
}

/** @brief The reentrant version of __cdi_decode_modem_sigs().
  * @param sigs the modem signals.
  * @version CDIAPI_1.0
  * @par Alias:
  * This is an implementation of cdi_decode_modem_sigs().
  */
char *
__cdi_decode_modem_sigs_r(uint sigs)
{
	return __cdi_decode_modem_sigs(sigs);
}

/** @fn char *cdi_decode_modem_sigs(uint sigs)
  * @param sigs the modem signals.
  * @version CDIAPI_1.0
  * @par Alias:
  * This symbol is a strong alias of __cdi_decode_modem_sigs_r().
  */
__asm__(".symver __cdi_decode_modem_sigs_r,cdi_decode_modem_sigs@@CDIAPI_1.0");


/** @} */

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
