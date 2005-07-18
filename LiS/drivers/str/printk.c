/*****************************************************************************

 @(#) $RCSfile: printk.c,v $ $Name:  $($Revision: 1.1.1.2.4.5 $) $Date: 2005/07/13 12:01:14 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/07/13 12:01:14 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: printk.c,v $ $Name:  $($Revision: 1.1.1.2.4.5 $) $Date: 2005/07/13 12:01:14 $"

/************************************************************************
*                          Printk Driver                                *
*************************************************************************
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
* This driver, /dev/printk, allows user level programs to write to	*
* the kernel's log.  When a user program writes to /dev/printk it	*
* turns into a kernel printk() call.  The purpose of this driver is	*
* to allow user level test programs of STREAMS to get their messages	*
* in sequence with the debug messages originating from the kernel.	*
*									*
************************************************************************/

#include <sys/stream.h>
#include <sys/osif.h>

static struct module_info printk_minfo = {
	0,				/* id */
	"printk",			/* name */
	0,				/* min packet size accepted */
	INFPSZ,				/* max packet size accepted */
	50000,				/* high water mark */
	40000				/* low water mark */
};

static int printk_open(queue_t *, dev_t *, int, int, cred_t *);
static int printk_close(queue_t *, int, cred_t *);
static int printk_wput(queue_t *, mblk_t *);

/* qinit structures (rd and wr side) 
 */
static struct qinit printk_rinit = {
	NULL,				/* put */
	NULL,				/* service */
	printk_open,			/* open */
	printk_close,			/* close */
	NULL,				/* admin */
	&printk_minfo,			/* info */
	NULL				/* stat */
};

static struct qinit printk_winit = {
	printk_wput,			/* put */
	NULL,				/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&printk_minfo,			/* info */
	NULL				/* stat */
};

/* streamtab for the printk driver.
 */
struct streamtab printk_info = {
	&printk_rinit,			/* read queue */
	&printk_winit,			/* write queue */
	NULL,				/* mux read queue */
	NULL				/* mux write queue */
};

/*
 * Open routine grants all opens
 */
static int
printk_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
	(void) q;		/* compiler happiness */
	(void) devp;		/* compiler happiness */
	(void) flag;		/* compiler happiness */
	(void) sflag;		/* compiler happiness */
	(void) credp;		/* compiler happiness */

	return (0);		/* success */

}				/* printk_open */

static int
printk_close(queue_t *q, int dummy, cred_t *credp)
{
	(void) q;		/* compiler happiness */
	(void) dummy;
	(void) credp;

	return (0);
}

static int
printk_wput(queue_t *q, mblk_t *msg)
{
	mblk_t *mp;
	char *p;
	char c;

	(void) q;		/* compiler happiness */

	if (msg->b_datap->db_type != M_DATA) {
		freemsg(msg);
		return (0);
	}

	for (mp = msg; mp != NULL; mp = mp->b_cont) {
		if (mp->b_rptr >= mp->b_wptr)
			continue;

		p = mp->b_wptr - 1;	/* to last chr of msg */
		c = *p;		/* fetch last chr */
		*p = 0;		/* last chr to NUL */
		printk("%s%c", mp->b_rptr, c);	/* write msg */
	}

	freemsg(msg);		/* done with msg */
	return (0);

}				/* printk_wput */

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-printk");
MODULE_ALIAS("char-major-" __stringify(PRINTK__CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(PRINTK__CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(PRINTK__CMAJOR_0) "-0");
MODULE_ALIAS("/dev/printk");
#endif
