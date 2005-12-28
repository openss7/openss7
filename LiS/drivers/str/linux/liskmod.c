/*****************************************************************************

 @(#) $RCSfile: liskmod.c,v $ $Name:  $($Revision: 1.1.1.3.4.5 $) $Date: 2005/07/13 12:01:15 $

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

 Last Modified $Date: 2005/07/13 12:01:15 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: liskmod.c,v $ $Name:  $($Revision: 1.1.1.3.4.5 $) $Date: 2005/07/13 12:01:15 $"

/************************************************************************
*                      LiS Kernel Module                                *
*************************************************************************
*									*
* Some versions of the kernel do not export all the functions that	*
* the more advanced releases of LiS need.  We do not want to patch	*
* the kernel -- users these days like to load external modules against	*
* unmodified kernels from distributions.  We also don't want to 	*
* duplicate kernel code inside LiS for licensing reasons.  Importing	*
* full GNU license code into LiS, which is GNU Library license, would	*
* probably render all of LiS full GNU, which we, the authors of LiS,	*
* explicitly do not want to do so as to encourage the porting of	*
* STREAMS drivers from other platforms such as Solaris and SCO.		*
*									*
* So, we have created this little module which contains code snippets	*
* shamelessly lifted from the kernel source.  The "liskmod" then	*
* exports these routines itself to the rest of the world, including	*
* LiS.									*
*									*
* So, the code in this module, unlike almost all the rest of the code	*
* under LiS, is full GNU license, and not GNU Library.			*
*									*
* Here are the functions that we are exporting.				*
*									*
* put_unused_fd								*
*	Exported in kernels >= 2.2.18 and 2.4.?.  For earlier		*
*	kernels we need to provide it here.				*
*									*
************************************************************************/

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,1,0)
#define KERNEL_2_0
#else
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,3,0)
#define KERNEL_2_2
#else
#define	KERNEL_2_4
#endif
#endif

#include <linux/kernel.h>	/* for printk */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,3,0)
#include <linux/sched.h>
#include <linux/fs.h>
#endif

/*
 * Do not include <sys/osif.h> in this module.  It would create
 * circular dependencies on streams.o.
 */

/************************************************************************
*                       Module Init and Cleanup                         *
*************************************************************************
*									*
* Functions called at module load/unload time.				*
*									*
************************************************************************/

#ifdef KERNEL_2_5
int
liskmod_init_module(void)
#else
int
init_module(void)
#endif
{
	printk("LiS Kernel Module Loaded\n");
	return (0);		/* if we're here, we're OK */
}

#ifdef KERNEL_2_5
void
liskmod_cleanup_module(void)
#else
void
cleanup_module(void)
#endif
{
	printk("LiS Kernel Module Unloading\n");
}

/************************************************************************
*                        put_unused_fd                                  *
*************************************************************************
*									*
* If the kernel version is early enough, include this function.		*
*									*
************************************************************************/

#if   defined(KERNEL_2_2)

# if LINUX_VERSION_CODE < KERNEL_VERSION(2,2,6)	/* inexact version test */

void
put_unused_fd(unsigned int fd)
{
	FD_CLR(fd, &current->files->open_fds);
}

# else
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2,2,18)

void
put_unused_fd(unsigned int fd)
{
	FD_CLR(fd, current->files->open_fds);
	if (fd < current->files->next_fd)
		current->files->next_fd = fd;
}

#  else
	/* kernel version 2.2.17 should have this visible */
#  endif
# endif

#else
#if defined(KERNEL_2_4)

# if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,1)

inline void
__put_unused_fd(struct files_struct *files, unsigned int fd)
{
	FD_CLR(fd, files->open_fds);
	if (fd < files->next_fd)
		files->next_fd = fd;
}

void
put_unused_fd(unsigned int fd)
{
	struct files_struct *files = current->files;

	write_lock(&files->file_lock);
	__put_unused_fd(files, fd);
	write_unlock(&files->file_lock);
}

# else
	/* kernel version 2.4.1 should have this visible */
# endif

#endif
#endif

/************************************************************************
*                           igrab                                       *
*************************************************************************
*									*
* Kernels before about 2.2.12 did not have this function.		*
*									*
************************************************************************/
#if   defined(KERNEL_2_2)

# if LINUX_VERSION_CODE < KERNEL_VERSION(2,2,12)

/*
 * The following code was lifted from 2.2.12.
 */
static void
__wait_on_inode(struct inode *inode)
{
	struct wait_queue wait = { current, NULL };

	add_wait_queue(&inode->i_wait, &wait);
      repeat:
	current->state = TASK_UNINTERRUPTIBLE;
	if (inode->i_state & I_LOCK) {
		schedule();
		goto repeat;
	}
	remove_wait_queue(&inode->i_wait, &wait);
	current->state = TASK_RUNNING;
}

static inline void
wait_on_inode(struct inode *inode)
{
	if (inode->i_state & I_LOCK)
		__wait_on_inode(inode);
}

/*
 * A slight problem here is that we don't have access to "inode_lock"
 * since it is not exported.  So we just have to risk bumping the
 * count without holding the lock.
 */
struct inode *
igrab(struct inode *inode)
{
	/* spin_lock(&inode_lock); */
	if (inode->i_state & I_FREEING)
		inode = NULL;
	else
		inode->i_count++;
	/* spin_unlock(&inode_lock); */
	if (inode)
		wait_on_inode(inode);
	return inode;
}

# endif

#endif

/************************************************************************
*                         Module Information                            *
************************************************************************/

#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL");
#endif
#if defined(MODULE_AUTHOR)
MODULE_AUTHOR("David Grothe <dave@gcom.com>");
#endif
#if defined(MODULE_DESCRIPTION)
MODULE_DESCRIPTION("Linux Kernel Compatibility Code for Linux STREAMS (LiS)");
#endif
#if defined(MODULE_ALIAS)
MODULE_ALIAS("streams-" __stringify(LIS_OBJNAME));
#endif
