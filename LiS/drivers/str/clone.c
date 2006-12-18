/*****************************************************************************

 @(#) $RCSfile: clone.c,v $ $Name:  $($Revision: 1.1.1.2.4.7 $) $Date: 2005/12/18 06:37:52 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2005/12/18 06:37:52 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: clone.c,v $ $Name:  $($Revision: 1.1.1.2.4.7 $) $Date: 2005/12/18 06:37:52 $"

/************************************************************************
*                          Clone Driver                                 *
*************************************************************************
*									*
* Author:	David Grothe <dave@gcom.com>				*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
* This driver's open routine divides the device number passed in into	*
* the major and minor parts.  It treats the minor number as the major	*
* number of the driver that the user really wants to open.  It then	*
* calls that driver's open routine passing in the target driver's	*
* major and minor zero.  It sets the clone option bit in this open.	*
*									*
* The queue passed to clone is given the attributes of the target	*
* driver rather than of the clone driver.				*
*									*
* When this driver returns from its open routine the dev_t structure	*
* passed to it will no doubt be modified.  The caller, perhaps clear	*
* back in the I/O system, must take care to notice this and allocate	*
* a new inode for this stream.						*
*									*
************************************************************************/

#include <sys/stream.h>
#include <sys/osif.h>

static struct module_info clone_minfo = {
	0,				/* id */
	"clone",			/* name */
	0,				/* min packet size accepted */
	0,				/* max packet size accepted */
	0,				/* high water mark */
	0				/* low water mark */
};

static int _RP clone_open(queue_t *, dev_t *, int, int, cred_t *);
static int _RP clone_close(queue_t *, int, cred_t *);

/* qinit structures (rd and wr side) 
 */
static struct qinit clone_rinit = {
	NULL,				/* put */
	NULL,				/* service */
	clone_open,			/* open */
	clone_close,			/* close */
	NULL,				/* admin */
	&clone_minfo,			/* info */
	NULL				/* stat */
};

static struct qinit clone_winit = {
	NULL,				/* put */
	NULL,				/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&clone_minfo,			/* info */
	NULL				/* stat */
};

/* streamtab for the clone driver.
 */
struct streamtab clone_info = {
	&clone_rinit,			/* read queue */
	&clone_winit,			/* write queue */
	NULL,				/* mux read queue */
	NULL				/* mux write queue */
};

static int _RP
clone_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
	int major_dev;
	int rslt;
	streamtab_t *st;

	if (sflag == CLONEOPEN)
		return (OPENFAIL);

	/* 
	 * Minor passed to us functions as major for the target driver
	 */
	major_dev = getminor(*devp);
	st = lis_find_strdev(major_dev);
	if (st == NULL)
		return (ENOENT);	/* no such driver */

	*devp = makedevice(major_dev, 0);	/* construct new major */

	if (st->st_rdinit->qi_qopen == NULL)
		return (OPENFAIL);
	lis_setq(q, st->st_rdinit, st->st_wrinit);	/* xfer queue params */
	rslt = st->st_rdinit->qi_qopen(q, devp, flag, CLONEOPEN, credp);
	if (rslt != 0)
		return (rslt);

	return (0);		/* success */

}				/* clone_open */

static int _RP
clone_close(queue_t *q, int dummy, cred_t *credp)
{
	(void) q;		/* compiler happiness */
	(void) dummy;
	(void) credp;

	return (0);
}

void
clone_init(void)
{
	/* 
	 *  indicate that this is a CLONE pseudo device
	 */
	LIS_DEVFLAGS(LIS_CLONE) |= LIS_MODFLG_CLONE;
}

#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-" __stringify(LIS_OBJNAME));
MODULE_ALIAS("char-major-" __stringify(CLONE__CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(CLONE__CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(CLONE__CMAJOR_0) "-0");
MODULE_ALIAS("/dev/clone_drvr");
#endif
