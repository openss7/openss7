/*****************************************************************************

 @(#) $RCSfile: relay.c,v $ $Name:  $($Revision: 1.1.1.3.4.11 $) $Date: 2007/08/15 04:57:59 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/15 04:57:59 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: relay.c,v $
 Revision 1.1.1.3.4.11  2007/08/15 04:57:59  brian
 - GPLv3 updates

 *****************************************************************************/

#ident "@(#) $RCSfile: relay.c,v $ $Name:  $($Revision: 1.1.1.3.4.11 $) $Date: 2007/08/15 04:57:59 $"

static char const ident[] = "$RCSfile: relay.c,v $ $Name:  $($Revision: 1.1.1.3.4.11 $) $Date: 2007/08/15 04:57:59 $";

/*                               -*- Mode: C -*- 
 * relay.c --- A simple relay pushable module
 * Author          : Dave Grothe
 * Created On      : Dec 30, 1995
 * Last Modified By: Dave Grothe
 * RCS Id          : $Id: relay.c,v 1.1.1.3.4.11 2007/08/15 04:57:59 brian Exp $
 * Purpose         : relay messages just to test pushable modules
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  David Grothe <dave@gcom.com>
 *
 */

/*  -------------------------------------------------------------------  */

#include <sys/LiS/module.h>	/* should be first */

#include <sys/LiS/config.h>

#include <sys/stream.h>
#include <sys/osif.h>

/*  -------------------------------------------------------------------  */
/*			  Module definition structs                      */

/* Module info for the relay module
 */
static struct module_info relay_minfo = {
	0,				/* id */
	"relay",			/* name */
	0,				/* min packet size accepted */
	INFPSZ,				/* max packet size accepted */
	10240L,				/* high water mark */
	512L				/* low water mark */
};

static struct module_info relay2_minfo = {
	0,				/* id */
	"relay2",			/* name */
	0,				/* min packet size accepted */
	INFPSZ,				/* max packet size accepted */
	10240L,				/* high water mark */
	512L				/* low water mark */
};

static struct module_info relay3_minfo = {
	0,				/* id */
	"relay3",			/* name */
	0,				/* min packet size accepted */
	INFPSZ,				/* max packet size accepted */
	10240L,				/* high water mark */
	512L				/* low water mark */
};

/* These are the entry points to the driver: open, close, write side put and
 * service procedures and read side service procedure.
 */
static int _RP relay_open(queue_t *, dev_t *, int, int, cred_t *);
static int _RP relay_close(queue_t *, int, cred_t *);
static int _RP relay_wput(queue_t *, mblk_t *);
static int _RP relay_rput(queue_t *, mblk_t *);

#if 0
static int relay_wsrv(queue_t *);
static int relay_rsrv(queue_t *);
#endif

/* qinit structures (rd and wr side) 
 */
static struct qinit relay_rinit = {
	relay_rput,			/* put */
	NULL,				/* service */
	relay_open,			/* open */
	relay_close,			/* close */
	NULL,				/* admin */
	&relay_minfo,			/* info */
	NULL				/* stat */
};

static struct qinit relay_winit = {
	relay_wput,			/* put */
	NULL,				/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&relay_minfo,			/* info */
	NULL				/* stat */
};

/* streamtab for the relay modules
 */
struct streamtab relay_info = {
	&relay_rinit,			/* read queue */
	&relay_winit,			/* write queue */
	NULL,				/* mux read queue */
	NULL				/* mux write queue */
};

static struct qinit relay2_rinit = {
	relay_rput,			/* put */
	NULL,				/* service */
	relay_open,			/* open */
	relay_close,			/* close */
	NULL,				/* admin */
	&relay2_minfo,			/* info */
	NULL				/* stat */
};

static struct qinit relay2_winit = {
	relay_wput,			/* put */
	NULL,				/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&relay2_minfo,			/* info */
	NULL				/* stat */
};

/* streamtab for the relay modules
 */
struct streamtab relay2_info = {
	&relay2_rinit,			/* read queue */
	&relay2_winit,			/* write queue */
	NULL,				/* mux read queue */
	NULL				/* mux write queue */
};

static struct qinit relay3_rinit = {
	relay_rput,			/* put */
	NULL,				/* service */
	relay_open,			/* open */
	relay_close,			/* close */
	NULL,				/* admin */
	&relay3_minfo,			/* info */
	NULL				/* stat */
};

static struct qinit relay3_winit = {
	relay_wput,			/* put */
	NULL,				/* service */
	NULL,				/* open */
	NULL,				/* close */
	NULL,				/* admin */
	&relay3_minfo,			/* info */
	NULL				/* stat */
};

/* streamtab for the relay modules
 */
struct streamtab relay3_info = {
	&relay3_rinit,			/* read queue */
	&relay3_winit,			/* write queue */
	NULL,				/* mux read queue */
	NULL				/* mux write queue */
};

/*  -------------------------------------------------------------------  */
/*			    Module implementation                        */
/*  -------------------------------------------------------------------  */

/*  -------------------------------------------------------------------  */
/*				relay_open				 */
/*  -------------------------------------------------------------------  */
static int _RP
relay_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *credp)
{
	printk("relay_open(dev=0x%x, flag=0x%x, sflag=0x%x)\n", DEV_TO_INT(*devp), flag, sflag);
	if (sflag == CLONEOPEN)
		return (EINVAL);

	return 0;		/* success */

}

/*  -------------------------------------------------------------------  */
/*				relay_wput				 */
/*  -------------------------------------------------------------------  */

static int _RP
relay_wput(queue_t *q, mblk_t *mp)
{
#ifdef DEBUG
	lis_print_msg(mp, "relay_wput", PRINT_DATA_RDWR);
#endif
	putnext(q, mp);		/* relay downstream */
	return (0);
}

/*  -------------------------------------------------------------------  */
/*				relay_rput				 */
/*  -------------------------------------------------------------------  */

static int _RP
relay_rput(queue_t *q, mblk_t *mp)
{
#ifdef DEBUG
	lis_print_msg(mp, "relay_rput", PRINT_DATA_RDWR);
#endif
	putnext(q, mp);		/* relay upstream */
	return (0);
}

#if 0
/*  -------------------------------------------------------------------  */
/*				relay_wsrv				 */
/*  -------------------------------------------------------------------  */

static int
relay_wsrv(queue_t *q)
{
	/* not used */
	return (0);
}
#endif

#if 0
/*  -------------------------------------------------------------------  */
/*				relay_rsrv				 */
/*  -------------------------------------------------------------------  */

static int
relay_rsrv(queue_t *q)
{
	/* not used */
	return (0);
}
#endif

/*  -------------------------------------------------------------------  */
/*				relay_close				 */
/*  -------------------------------------------------------------------  */

static int _RP
relay_close(queue_t *q, int dummy, cred_t *credp)
{
	printk("relay_close\n");
	return 0;
}

/*  -------------------------------------------------------------------  */
/*				Module Routines				 */
/*  -------------------------------------------------------------------  */

#if !defined __NO_VERSION__

#ifdef KERNEL_2_5
int
relay3_init_module(void)
#else
int
init_module(void)
#endif
{
	int ret = lis_register_strmod(&relay3_info, "relay3");

	if (ret < 0) {
		printk("relay3.init_module: Unable to register module.\n");
		return ret;
	}
	return 0;
}

#ifdef KERNEL_2_5
void
relay3_cleanup_module(void)
#else
void
cleanup_module(void)
#endif
{
	if (lis_unregister_strmod(&relay3_info) < 0)
		printk("relay3.cleanup_module: Unable to unregister module.\n");
	else
		printk("relay3.cleanup_module: Unregistered, ready to be unloaded.\n");
	return;
}

#ifdef KERNEL_2_5
module_init(relay3_init_module);
module_exit(relay3_cleanup_module);
#endif
#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL");
#endif

#endif				/* !defined __NO_VERSION__ */
