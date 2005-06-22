/*****************************************************************************

 @(#) $RCSfile: linux-mdep.c,v $ $Name:  $($Revision: 1.1.1.11.4.9 $) $Date: 2005/06/01 02:42:27 $

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

 Last Modified $Date: 2005/06/01 02:42:27 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: linux-mdep.c,v $ $Name:  $($Revision: 1.1.1.11.4.9 $) $Date: 2005/06/01 02:42:27 $"

/*                               -*- Mode: C -*- 
 * linux-mdep.c --- Linux kernel dependent support for LiS.
 * Author          : Francisco J. Ballesteros
 * Created On      : Sat Jun  4 20:56:03 1994
 * Last Modified By: John A. Boyd Jr.
 * RCS Id          : $Id: linux-mdep.c,v 1.1.1.11.4.9 2005/06/01 02:42:27 brian Exp $
 * Purpose         : provide Linux kernel <-> LiS entry points.
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  Francisco J. Ballesteros, Denis Froschauer
 *    Copyright (C) 1997-2000
 *			  David Grothe, Gcom, Inc <dave@gcom.com>
 *    Copyright (C) 2000  John A. Boyd Jr.  protologos, LLC
 *
 *    You can reach me by email to
 *    nemo@ordago.uc3m.es, 100741.1151@compuserve.com
 *    dave@gcom.com
 *
 * Modified 2001-08-09 by Jeff Goldszer <Jeff_goldszer@cnt.com>
 *    Prevent fdetach from freeing a pipe's inodes before the pipe
 *    has been closed.
 *
 * 2002/11/18 - John Boyd - reworked fattach()/fdetach() code to use
 *    mount()/umount() infrastructure in 2.4.x kernels; old code
 *    removed (per Dave's request...).  Filesystem/superblock code
 *    also reworked, for same purpose.
 */

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#undef __NO_VERSION__
#undef  MODULE
#define MODULE 1

#include <sys/LiS/linux-mdep.h>
#include <sys/lislocks.h>
#include <linux/module.h>
#include <sys/LiS/modcnt.h>		/* after linux-mdep.h & module.h */
#include <linux/version.h>

#include <linux/sched.h>
#define	__KERNEL_SYSCALLS__	1	/* to make kernel_thread visible */
#include <linux/unistd.h>		/* for kernel_thread */

# ifdef RH_71_KLUDGE			/* boogered up incls in 2.4.2 */
#  undef CONFIG_HIGHMEM			/* b_page has semi-circular reference */
# endif
#include <asm/signal.h>
#include <asm/io.h>
#include <sys/strport.h>	/* interface */
#include <sys/LiS/errmsg.h>	/* LiS err msg types */
#include <sys/LiS/buffcall.h>	/* bufcalls */
#include <sys/LiS/head.h>	/* stream head */
#include <sys/osif.h>
#include <sys/stream.h>		/* LiS entry points */
#include <sys/cmn_err.h>
#include <sys/poll.h>
#include <linux/types.h>
#include <linux/mm.h>
#include <linux/file.h>
#include <linux/fs.h>		/* linux file sys externs */
#include <linux/vfs.h>		/* linux file sys externs */
#if defined(KERNEL_2_5)
#include <linux/namei.h>	/* linux file sys externs */
#include <linux/cdev.h>		/* cdev_put */
#endif
#include <linux/pipe_fs_i.h>
#include <linux/mount.h>
#include <linux/proc_fs.h>	/* for stats in /proc/LiS */
#if defined(FATTACH_VIA_MOUNT)
#include <linux/capability.h>
#endif
#include <linux/time.h>

/*  -------------------------------------------------------------------  */
/*
 * S/390 2.4 kernels export smp_num_cpus
 * other 2.4 kernels export num_online_cpus()
 */
#if ( defined(__s390__) || defined(__s390x__) ) && defined(KERNEL_2_4)
#define NUM_CPUS		smp_num_cpus
#elif defined(KERNEL_2_5)
#define NUM_CPUS		num_online_cpus()
#else
#define NUM_CPUS		smp_num_cpus
#endif

/*  -------------------------------------------------------------------  */

int lis_major;
char lis_kernel_version[]= UTS_RELEASE;


extern char	lis_version[] ;
extern char	lis_date[] ;

extern void	lis_start_qsched(void);

char	*lis_stropts_file =
#if   defined(_LIS_STROPTS_H)
                "<LiS/include/stropts.h>"
#elif defined(_LIS_SYS_STROPTS_H)
		"<LiS/include/sys/stropts.h>"
#elif defined(_STROPTS_H)
                "<usr/include/stropts.h>"
#elif defined(_SYS_STROPTS_H)
                "<usr/include/sys/stropts.h>"
#elif defined(_BITS_STROPTS_H)
		"<usr/include/bits/stropts.h>"
#else
		"<unknown/stropts.h>"
#endif
;

/*
 * Load STREAMS module with panic_when_killed=1 (default) to call panic()
 * when assertions fail; otherwise, failed assertions call BUG().
 */
int lis_panic_when_killed = 1;

/* 
 * For running a given number of STREAMS threads on SMP
 * machine. default is zero (use every available CPUs)
 *
 * FIXME: current implementation works only with lis_nthreads=1
 */
int lis_nthreads = 0;

/************************************************************************
*                      System Call Support                              *
*************************************************************************
*									*
* LiS provides wrappers for a selected few system calls.  The functions	*
* return 0 upon success and a negative errno on failure.		*
*									*
************************************************************************/

static int	lis_errnos[LIS_NR_CPUS] ;
#define	errno	lis_errnos[smp_processor_id()]

/*
 * For gcc 3.3.3 the combination of inlining these functions and the
 * register passing conventions causes the parameters to the system
 * call to get messed up.  Simply defeating the inlining takes care
 * of the problem.  You won't see the problem unless you are working
 * with a 2.6 based distribution.  I first noticed it in SuSE 9.1.
 */
#if defined(noinline)		/* kernel has this defined */
#define _NI     noinline
#else				/* no special meaning */
#define	_NI	__attribute__((noinline))
#endif

/* PARISC cannot handle in-kernel system calls in the same way as external
 * calls.  Therefore we rip the necessary symbols in the configuration script
 * and perform a dynamic linkage.  This is done for all architectures when the
 * addresses are available. */

#ifdef HAVE_SYS_MKNOD_ADDR
static asmlinkage long (*syscall_mknod) (const char *filename, int mode, dev_t dev)
    = (typeof(syscall_mknod)) HAVE_SYS_MKNOD_ADDR;
#elif defined _HPPA_LIS_
#define syscall_mknod(name,mode,dev) (-ENOSYS)
#else
#define __NR_syscall_mknod	__NR_mknod
static _NI _syscall3(long, syscall_mknod, const char *, file, int, mode, int, dev)
#endif

#ifdef HAVE_SYS_UNLINK_ADDR
static asmlinkage long (*syscall_unlink) (const char *pathname)
    = (typeof(syscall_unlink)) HAVE_SYS_UNLINK_ADDR;
#elif defined _HPPA_LIS_
#define syscall_unlink(name) (-ENOSYS)
#else
#define __NR_syscall_unlink	__NR_unlink
static _NI _syscall1(long, syscall_unlink, const char *, file)
#endif

#ifdef HAVE_SYS_MOUNT_ADDR
static asmlinkage long (*syscall_mount) (char *dev_name, char *dir_name, char *type,
					 unsigned long flags, void *data)
    = (typeof(syscall_mount)) HAVE_SYS_MOUNT_ADDR;
#elif defined _HPPA_LIS_
#define syscall_mount(dev_name,dir_name,type,flags,data) (-ENOSYS)
#else
#define __NR_syscall_mount	__NR_mount
static _NI _syscall5(long, syscall_mount, char *, dev, char *, dir, char *, type,
			unsigned long, flg, void *, data)
#endif

#ifdef HAVE_SYS_UMOUNT_ADDR
asmlinkage long (*syscall_umount2) (char *name, int flags)
    = (typeof(syscall_umount2)) HAVE_SYS_UMOUNT_ADDR;
#elif defined _HPPA_LIS_
#define syscall_umount2(name,flags) (-ENOSYS)
#else
#if defined(_ASM_IA64_UNISTD_H)
#define __NR_syscall_umount2	__NR_umount
#else
#define __NR_syscall_umount2	__NR_umount2
#endif
static _NI _syscall2(long, syscall_umount2, char *, file, int, flags)
#endif

#undef _NI

/************************************************************************
*                            lis_bug                                    *
*************************************************************************
*									*
* This is called to report bugs in the STREAMS subsystem.               *
*									*
* It sends a warning message to the kernel logger, then calls either	*
* BUG() or panic(), depending on lis_dont_panic                         *
*									*
************************************************************************/
__attribute__ ((format(printf, 3, 4)))
void lis_bug(const char *file, unsigned int line, const char *fmt, ...)
{
	static char buf[256];
	va_list args;

	va_start(args, fmt);
	vsnprintf(buf, sizeof(buf), fmt, args);
	va_end(args);
	if(lis_panic_when_killed) {
		panic("STREAMS file %s line %d: %s\n",
		      file, line, buf);
	} else {
		printk(KERN_EMERG "STREAMS file %s line %d: %s\n",
		       file, line, buf);
		BUG();
	}
}

/************************************************************************
*                            lis_assert_fail                            *
*************************************************************************
*									*
* This is called when the ASSERT() macro fails.                         *
*									*
* It sends a warning message to the kernel logger and kills the STREAMS *
* subsystem with lis_bug						*
*									*
************************************************************************/
void _RP lis_assert_fail(const char *expr, const char *objname,
		         const char *file, unsigned int line)
{
	lis_bug(file, line, "assert(%s) failed in module %s",
		expr, objname);
}

/************************************************************************
*                            Prototypes                                 *
************************************************************************/
extern void do_gettimeofday(struct timeval *tv) _RP;	/* kernel fcn */
extern void lis_spl_init(void);			/* lislocks.c */
extern int  lis_new_file_name_dev(struct file *f, const char *name, dev_t dev);
static struct inode * lis_get_inode( mode_t mode, dev_t dev );
void lis_print_dentry(struct dentry *d, char *comment) ;

#ifdef _S390X_LIS_
extern long sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg);
extern int register_ioctl32_conversion(unsigned int fd, 
                                       int (*handler)(unsigned int fd,
                                                      unsigned int cmd,
                                                      unsigned long arg));
extern int unregister_ioctl32_conversion(unsigned int cmd);
typedef struct strioctl32 {
    int     ic_cmd;                 /* command */
    int     ic_timout;              /* timeout value */
    int     ic_len;                 /* length of data */
    unsigned int ic_dp;             /* pointer to data */
} strioctl32_t;
#endif


/************************************************************************
*                       Storage Declarations                            *
************************************************************************/

lis_spin_lock_t			lis_setqsched_lock ; /* one qsched at a time */
lis_semaphore_t			lis_runq_sems[LIS_NR_CPUS] ;
lis_semaphore_t			lis_runq_kill_sems[LIS_NR_CPUS] ;
extern volatile unsigned long	lis_runq_wakeups[LIS_NR_CPUS] ; /* head.c */
int				lis_runq_sched ;     /* q's are scheduled */
lis_atomic_t			lis_inode_cnt ;
lis_atomic_t                    lis_mnt_cnt;   /* for lis_mnt only, for now */
int                             lis_mnt_init_cnt;  /* initial/final value */
lis_spin_lock_t			lis_task_lock ; /* for creds operations */
/*
 * The following for counting kmem_cache allocations
 */
lis_atomic_t                    lis_locks_cnt;
lis_atomic_t                    lis_head_cnt;
lis_atomic_t                    lis_qband_cnt;
lis_atomic_t                    lis_queue_cnt;
lis_atomic_t                    lis_qsync_cnt;
lis_atomic_t                    lis_msgb_cnt;

typedef struct lis_free_passfp_tg
{
	lis_spin_lock_t	lock ;	/* syncs global operations */
	mblk_t *head;	
	mblk_t *tail;	
} lis_free_passfp_t;

static lis_free_passfp_t free_passfp;

#if defined(USE_KMEM_CACHE)
kmem_cache_t *lis_msgb_cachep = NULL;
kmem_cache_t *lis_queue_cachep = NULL;
kmem_cache_t *lis_qsync_cachep = NULL;
kmem_cache_t *lis_qband_cachep = NULL;
kmem_cache_t *lis_head_cachep = NULL;
#endif

#if defined(USE_KMEM_TIMER) 
kmem_cache_t *lis_timer_cachep = NULL;
struct lis_timer {
      struct timer_list    lt;
      timo_fcn_t 	  *func;
      caddr_t		   arg;
      volatile toid_t	   handle;
};
#endif

#if defined(FATTACH_VIA_MOUNT)
/*
 * fattach instance data
 *
 *  note that fattach instances must be unique by sb (and thus by mounted
 *  dentry), but NOT by path or mounted STREAM (e.g., inode or head),
 *  since a path can be fattach'ed onto more than once, and a STREAM
 *  can be fattached onto more than one path (and thus onto the same
 *  path more than once).
 */
typedef struct lis_fattach {
    struct file        *file;             /* file passed in (->head) */
    struct vfsmount    *mount;            /* file's f_vfsmnt */
    stdata_t           *head;             /* stream head (->inode) */
    const char         *path;             /* path mounted on */
    struct super_block *sb;               /* mounted LiS sb (->dentry) */
    struct dentry      *dentry;           /* mounted dentry */
    struct list_head    list;
} lis_fattach_t;

/*
 *  global list of fattach instances
 */
static LIST_HEAD(lis_fattaches);
lis_atomic_t num_fattaches_allocd = 0;
lis_atomic_t num_fattaches_listed = 0;
lis_spin_lock_t lis_fattaches_lock;

static lis_fattach_t *lis_fattach_new(struct file *file, const char *path);
static void lis_fattach_delete(lis_fattach_t *data);
static void lis_fattach_insert(lis_fattach_t *data);
static void lis_fattach_remove(lis_fattach_t *data);

#endif  /* FATTACH_VIA_MOUNT */

void lis_show_inode_aliases(struct inode *);

/*
 * Declared in head.c
 */
extern lis_atomic_t		lis_runq_cnt ;	/* # of threads running */
extern int			lis_num_cpus ;
extern int			lis_runq_pids[LIS_NR_CPUS] ;
extern lis_atomic_t		lis_runq_active_flags[LIS_NR_CPUS] ;
extern volatile unsigned long	lis_setqsched_cnts[LIS_NR_CPUS] ;
extern volatile unsigned long	lis_setqsched_isr_cnts[LIS_NR_CPUS] ;
extern void			lis_run_queues(int) ;
extern lis_atomic_t		lis_queues_running ;
extern lis_atomic_t		lis_putnext_flag ;
extern lis_atomic_t	 	lis_runq_req_cnt ;
extern lis_spin_lock_t	 	lis_qhead_lock ;

extern void lis_cache_destroy(kmem_cache_t *p, lis_atomic_t *c, char *label);

/*  -------------------------------------------------------------------  */

/* This should be entry points from the kernel into LiS
 * kernel should be fixed to call them when appropriate.
 */
#if defined(KERNEL_2_5)
int lis_strflush(struct file *f);
#endif

/*
 * File operations
 */
struct file_operations
lis_streams_fops = {
    owner:     THIS_MODULE,
    read:      lis_strread,		/* read    		*/
    write:     lis_strwrite,		/* write                */
    poll:      lis_poll_2_1,		/* poll  		*/
    ioctl:     lis_strioctl,		/* ioctl   		*/
    open:      lis_stropen,		/* open                 */
#if defined(KERNEL_2_5)
    flush:     lis_strflush,		/* flush		*/
#endif
    release:   lis_strclose,		/* release 		*/
};

/*
 * Dentry operations
 */
extern int lis_dentry_delete(struct dentry *dentry) ;
extern void lis_dentry_iput(struct dentry *dentry, struct inode *inode);

struct dentry_operations lis_dentry_ops =
{
    d_delete:   lis_dentry_delete,
    d_iput:     lis_dentry_iput
};

/*
 *  D_IS_LIS() is a predicate macro that identifies a dentry as
 *  LiS-specific, i.e., allocated by LiS for LiS use only.
 */
#define D_IS_LIS(d)    ( (d) ? ((d)->d_op == &lis_dentry_ops) : 0 )

/*
 * Inode operations
 */
# if defined(KERNEL_2_5)
  struct dentry *lis_inode_lookup(struct inode *dir, struct dentry *dentry,
	  			  struct nameidata *nd);
# else
  struct dentry *lis_inode_lookup(struct inode *dir, struct dentry *dentry);
# endif

struct inode_operations
lis_streams_iops = {
    lookup:		&lis_inode_lookup,
};

/*
 * LiS inode structure.
 *
 * We use the generic_ip to point back to the stream head.  We also need
 * a place for the LiS dev_t.  In pre-2.6 kernels the i_rdev field is a
 * "short" (16 bits).  We need 32 bits.  So for pre-2.6 kernels we define
 * a structure that we overlay at the "u" field in the inode structure.
 *
 * The 2.6 kernel got rid of the big union, leaving just the generic_ip
 * pointer -- so there is no room for a structure overlay.  This is OK
 * because the 2.6 i_rdev is a 32-bit word, so we can use it directly.
 *
 * This all leads to some messiness of if-def-ing.
 */
#if !defined(KERNEL_2_5)
typedef struct
{
    stdata_t	*hdp ;		/* to stream head structure */
    dev_t	 dev ;		/* LiS device */

} lis_inode_info_t ;

#endif

/*
 * File system operations
 */
#if defined(KERNEL_2_5)
struct super_block *lis_fs_get_sb(struct file_system_type *fs_type,
				  int flags,
				  const char *dev_name,
				  void *ptr) ;
void lis_fs_kill_sb(struct super_block *);
#else
struct super_block *lis_fs_read_super(struct super_block *sb,
				      void *ptr,
				      int silent) ;
#endif

#define LIS_FS_NAME	"LiS"

struct file_system_type
lis_file_system_ops =
{
    name:	LIS_FS_NAME,
#if defined(KERNEL_2_5)
    get_sb:	lis_fs_get_sb,
    kill_sb:	lis_fs_kill_sb,
    owner:	NULL,
#else
    read_super:	lis_fs_read_super,
    owner:	NULL,
#endif
#if defined(FATTACH_VIA_MOUNT)
    fs_flags:   0,
#else
    fs_flags:	(FS_NOMOUNT | FS_SINGLE),
#endif
} ;
#define	LIS_SB_MAGIC	( ('L' << 16) | ('i' << 8) | 'S' )

struct vfsmount		*lis_mnt ;
int			 lis_initial_use_cnt ;


void lis_mnt_cnt_sync_fcn(const char *file, int line, const char *fn)
{
    lis_mnt_cnt_sync();

    if (LIS_DEBUG_REFCNTS)
	printk("lis_mnt_cnt_sync() >> [%d] {%s@%d,%s()}\n",
	       K_ATOMIC_READ(&lis_mnt_cnt), file, line, fn);
}

struct vfsmount *lis_mntget_fcn(struct vfsmount *m,
				const char *file, int line, const char *fn)
{
    struct vfsmount *mm = (m ? mntget(m) : NULL) ;

    lis_mnt_cnt_sync();

    if (LIS_DEBUG_REFCNTS)
    {
	if (mm == NULL)
	    printk("lis_mntget(NULL) {%s@%d,%s()}\n", file,line,fn) ;
	else
	    printk("lis_mntget(m@0x%p/++%d) \"%s\"%s {%s@%d,%s()}\n",
		   mm, MNT_COUNT(mm), mm->mnt_devname,
		   (lis_mnt && mm == lis_mnt?" <lis_mnt>":""),
		   file,line,fn) ;
    }

    return(mm) ;
}

void lis_mntput_fcn(struct vfsmount *m,
		    const char *file, int line, const char *fn)
{
    if (m == NULL)
    {
	if (LIS_DEBUG_REFCNTS)
	    printk("lis_mntput(NULL) {%s@%d,%s()}\n", file,line,fn) ;
	return ;
    }

    if (LIS_DEBUG_REFCNTS)
	printk("lis_mntput(m@0x%p/%d%s) \"%s\"%s {%s@%d,%s()}\n",
	       m,
	       MNT_COUNT(m), (MNT_COUNT(m)>0?"--":""), m->mnt_devname,
	       (lis_mnt && m == lis_mnt?" <lis_mnt>":""),
	       file,line,fn) ;

    if (MNT_COUNT(m) > 0)
	mntput(m) ;

    lis_mnt_cnt_sync_fcn(file, line, fn) ;
}

/*
 * Super block operations
 */
#if defined(KERNEL_2_5)
int lis_super_statfs(struct super_block *sb, struct kstatfs *stat) ;
#else
int lis_super_statfs(struct super_block *sb, struct statfs *stat) ;
#endif

void lis_super_put_inode(struct inode *) ;

#if defined(KERNEL_2_5)
void lis_drop_inode(struct inode *) ;
#endif
#if defined(FATTACH_VIA_MOUNT)
void lis_super_umount_begin(struct super_block *) ;
void lis_super_put_super(struct super_block *) ;
#endif

struct super_operations lis_super_ops =
{
    put_inode:		lis_super_put_inode,
    statfs:		lis_super_statfs,
#if defined(KERNEL_2_5)
    drop_inode:		lis_drop_inode,
#endif
#if defined(FATTACH_VIA_MOUNT)
    umount_begin:       lis_super_umount_begin,
    put_super:          lis_super_put_super,
#endif
} ;

#if defined(KERNEL_2_5)
#define S_FS_INFO(s)    ((s)->s_fs_info)
#else
#define S_FS_INFO(s)    ((s)->u.generic_sbp)
#endif

/*
 *  the following predicate macros identify structures as
 *  LiS-specific, i.e., allocated by LiS for LiS use only.
 */
#define S_IS_LIS(s)     ( (s) ? ((s)->s_op == &lis_super_ops) : 0 )
#define I_IS_LIS(i)     ( (i) ? ((i)->i_sb && S_IS_LIS((i)->i_sb)) : 0 )

/***********************************************************************/

/*
 * Base name of a file
 */
#define	BFN(fname)	lis_basename(fname)


/* MODCNT and lis_modcnt would be in <sys/LiS/modcnt.h>, except that I
 * consider them dangerous - JB
 */
#define MODCNT()	lis_modcnt(THIS_MODULE)

static inline
long lis_modcnt( struct module *mod )
{
#if defined(THIS_MODULE)
# if defined(KERNEL_2_5)
#  ifdef CONFIG_MODULE_UNLOAD
    return(module_refcount(mod)) ;
#  else
    return (0);			/* refcnts are very buried */
#  endif
# else
    return (mod ? (long) atomic_read(&(mod->uc.usecount)) : 0);
# endif
#else
    return 0
#endif
}



#if defined(KERNEL_2_5)

#define MODSYNC()	lis_modsync_dbg(__LIS_FILE__,__LINE__,__FUNCTION__)

static
void lis_modsync_dbg(const char *file, int line, const char *fn)
{
#ifdef THIS_MODULE
    long mod_cnt = lis_modcnt(THIS_MODULE);

    if (LIS_DEBUG_REFCNTS)
	if ((THIS_MODULE)->name) {
	    printk("lis_modcnt_sync() <\"%s\"/%ld> {%s@%d,%s()}\n",
		   (THIS_MODULE)->name, mod_cnt, BFN(file), line, fn);
	}
#endif
}

#else				/* defined(KERNEL_2_5) */

#define MODSYNC()	do {} while (0)

#endif				/* defined(KERNEL_2_5) */

void _RP lis_modget_dbg(const char *file, int line, const char *fn)
{
#ifdef THIS_MODULE
    if (LIS_DEBUG_REFCNTS)
	printk("lis_modget() <\"%s\"/%ld>++ {%s@%d,%s()}\n",
	       (THIS_MODULE)->name,
	       lis_modcnt(THIS_MODULE),
	       BFN(file), line, fn) ;

    lis_modget_local(file, line, fn);
#endif
}

void _RP lis_modput_dbg(const char *file, int line, const char *fn)
{
#ifdef THIS_MODULE
    long cnt ;

    if (LIS_DEBUG_REFCNTS)
	printk("lis_modput() <\"%s\"/%ld>-- {%s@%d,%s()}\n",
	       THIS_MODULE->name,
	       lis_modcnt(THIS_MODULE),
	       BFN(file), line, fn) ;

#if !defined(KERNEL_2_5) || defined(CONFIG_MODULE_UNLOAD)
    if ((cnt = lis_modcnt(THIS_MODULE)) <= 0)
    {
	printk("lis_modput() >> error -- count=%ld {%s@%d,%s()}\n",
	       cnt, BFN(file), line, fn) ;
	return ;
    }
#endif

    lis_modput_local(file, line, fn);
#endif
}



/* some kernel memory has been free'd 
 * tell STREAMS
 */
void lis_memfree( void )
{
}/*lis_memfree*/

/* This will copyin usr string pointed by ustr and return the result  in
 * *kstr. It will stop at  '\0' or max bytes copyed in.
 * caller should call FREE(*kstr) on success.
 * Will return 0 or errno
 * STATUS: complete, untested
 */
int 
lis_copyin_str(struct file *f, const char *ustr, char **kstr, int maxb)
{
	int    error;
	char  *mem ;

	(void) f ;
	if (maxb <= 0)
	    return(-ENOMEM);
	error = -EFAULT;

	if((mem = ALLOCF(maxb,"copyin-buf ")) == NULL)
	    return(-ENOMEM);

	*kstr = mem ;
	error = strncpy_from_user(mem, ustr, maxb) ;
	if (error < 0)
	{
	    FREE(mem);
	    return(error) ;
	}

	if (error >= maxb)
	{
	    FREE(mem);
	    return(-ENAMETOOLONG) ;
	}

	return(0) ;
}/*lis_copyin_str*/

/************************************************************************
*                    Major/Minor Device Number Handling                 *
*************************************************************************
*									*
* These routines handle major/minor device numbers in LiS internal	*
* format.								*
*									*
************************************************************************/

#define LIS_MINOR_BITS          20
#define LIS_MINOR_MASK          ( (1 << LIS_MINOR_BITS) - 1 )

major_t _RP lis_getmajor(dev_t dev)
{
    return( dev >> LIS_MINOR_BITS ) ;
}

minor_t _RP lis_getminor(dev_t dev)
{
    return( dev & LIS_MINOR_MASK ) ;
}

dev_t _RP lis_makedevice(major_t majr, minor_t minr)
{
    return( (majr << LIS_MINOR_BITS) | (minr & LIS_MINOR_MASK) ) ;
}

/*
 * dev is really a kernel dev_t structure
 */
dev_t lis_kern_to_lis_dev(dev_t dev)
{
    return( lis_makedevice(STR_KMAJOR(dev), STR_KMINOR(dev)) ) ;
}


/*
 * Extract i_rdev from an inode.  If it's an LiS inode then not much
 * needs to be done.  If it is still a kernel inode then we need to
 * apply our translation routine to reconstruct the device id as an
 * LiS style dev_t.
 */
dev_t lis_i_rdev(struct inode *i)
{
#if defined(KERNEL_2_5)
    if (I_IS_LIS(i))
	return(RDEV_TO_DEV(i->i_rdev)) ;

    return(lis_kern_to_lis_dev(i->i_rdev)) ;
#else
    if (I_IS_LIS(i))
    {
	lis_inode_info_t *p = (lis_inode_info_t *) &i->u ;
	return( p->dev ) ;
    }

    return(lis_kern_to_lis_dev(i->i_rdev)) ;
#endif
}


/*  -------------------------------------------------------------------  */
/*
 * lis_get_new_inode
 *
 * Depending upon kernel version and distribution this is either
 * get_empty_inode or new_inode.  The configuration script has figured
 * out which it is and set the GET_EMPTY_INODE macro accordingly
 */
static struct inode *lis_get_new_inode(struct super_block *sb)
{
    struct inode *i = NULL;;

    if (!sb)
    {
	printk("lis_get_new_inode() - NULL super block pointer\n") ;
	return(NULL) ;
    }

    i = GET_EMPTY_INODE(sb);

    /*
     *  mark this inode as a LiS inode, and count it
     */
    i->i_sb = sb;          /* ASSERT: sb is or will be lis_mnt->mnt_sb */
    K_ATOMIC_INC(&lis_inode_cnt);

    if (LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	printk("lis_get_new_inode(s@0x%p)%s%s >> i@0x%p/%d%s\n",
	       sb,
	       (S_IS_LIS(sb)?" <LiS>":""),
	       (lis_mnt && sb == lis_mnt->mnt_sb?" <lis_mnt>":""),
	       i, (i?I_COUNT(i):0),
	       (i&&I_IS_LIS(i)?" <LiS>":""));
    return(i);
}


/*  -------------------------------------------------------------------  */
/*				    Timeouts                             */

void
lis_tmout(struct timer_list *tl, void (*fn)(ulong), long arg, long ticks)
{
    init_timer(tl);
    tl->function= fn;
    tl->data	= arg;
    tl->expires	= jiffies + ticks;
    add_timer(tl);
}

void
lis_untmout( struct timer_list *tl)
{
    del_timer(tl);		/* don't care if timeout fcn is running */
}

/************************************************************************
*                             lis_time_till                             *
*************************************************************************
*									*
* Given a target time in terms of elapsed milli-seconds, in other words,*
* the same units as jiffies if jiffies were in milli-seconds, return	*
* the number of milli-seconds that it will take to reach to target time.*
*									*
* The return is negative if the target time is in the past, zero	*
* if it is the same as the current target time, and positive if		*
* target time is in the future.						*
*									*
************************************************************************/
long	_RP lis_time_till(long target_time)
{
	if ( ((1000 / HZ) * HZ) == 1000 ) /* ie: HZ == 100 */
		return target_time - jiffies * (1000 / HZ) ;
	else						/* ie: HZ == 1024 */
		return (((target_time * HZ) / 1000 - jiffies) * 1000) / HZ ;

} /* lis_time_till */

/************************************************************************
*                           lis_target_time                             *
*************************************************************************
*									*
* Convert the milli_sec interval to an absolute target time expressed	*
* in milli-seconds.  We compute this relative to the Linux software	*
* clock, jiffies, converted to milliseconds.				*
*									*
************************************************************************/
long	_RP lis_target_time(long milli_sec)
{
	if ( ((1000 / HZ) * HZ) == 1000 ) /* ie: HZ == 100 */
		return jiffies * (1000 / HZ) + milli_sec ;
	else						/* ie: HZ == 1024 */
		return ((jiffies + (milli_sec * HZ) / 1000) * 1000) / HZ ;

} /* lis_target_time */

/************************************************************************
*                           lis_milli_to_ticks                          *
*************************************************************************
*									*
* Convert milli-seconds to native ticks suitable for use with system	*
* timeout routines.							*
*									*
************************************************************************/
long	_RP lis_milli_to_ticks(long milli_sec)
{
	if ( ((1000/HZ)*HZ) == 1000 ) /* ie: HZ == 100 */
		return milli_sec / (1000/HZ) ;
	else						/* ie: HZ == 1024 */
    return (milli_sec*HZ) / 1000 ;
}


/*  -------------------------------------------------------------------  */

/* find the head for a file descriptor.
 * STATUS: complete, untested
 */
stdata_t *
lis_fd2str( int fd )
{
    struct file		* file;
    stdata_t		* hd ;

    if ( (file = fcheck(fd)) == NULL)
	return(NULL) ;

    hd = FILE_STR(file) ;
    if (hd != NULL && hd->magic == STDATA_MAGIC)
	return(hd) ;			/* looks like a stdata structure */

    return(NULL) ;			/* not a streams file */
}

struct inode *lis_file_inode(struct file *f)
{
    return((f)->f_dentry->d_inode) ;
}

char *lis_file_name(struct file *f)
{
    return((char *)((f)->f_dentry->d_name.name)) ;
}

struct stdata *lis_file_str(struct file *f)
{
    return((struct stdata *)(f)->private_data) ;
}

void lis_set_file_str(struct file *f, struct stdata *s)
{
    f->private_data = (void *) s ;
}

struct stdata *lis_inode_str(struct inode *i)
{
#if defined(KERNEL_2_5)
    return((struct stdata *)(i)->u.generic_ip) ;
#else
    {
	lis_inode_info_t *p = (lis_inode_info_t *) &i->u ;
	return(p->hdp) ;
    }
#endif
}

void lis_set_inode_str(struct inode *i, struct stdata *s)
{
#if defined(KERNEL_2_5)
    i->u.generic_ip = (void *) s ;
#else
    lis_inode_info_t *p = (lis_inode_info_t *) &i->u ;
    p->hdp = s ;
#endif
}

struct dentry *lis_d_alloc_root(struct inode *i, int mode)
{
#if defined(FATTACH_VIA_MOUNT)
    struct dentry *d = NULL;
    struct qstr dname;
    int for_mount = (mode == LIS_D_ALLOC_ROOT_MOUNT);

    if (i) {
	stdata_t *head = INODE_STR(i);
	char name[48] = "";

	if (for_mount)
	    strcpy( name, LIS_FS_NAME "/" );
	if (head && *(head->sd_name)) {
	    strcat( name, head->sd_name );
	}
	name[47] = 0;

	dname.name = name;
	dname.len  = strlen(name) ;
	dname.hash = i->i_ino;

        d = d_alloc(NULL, &dname);
	if (d) {
	    d->d_sb     = i->i_sb;
	    d->d_parent = d;
	    d_instantiate(d, i);
	}
    }
    else
    {
	dname.name = LIS_FS_NAME"/";
	dname.len  = strlen(dname.name);
	dname.hash = full_name_hash(dname.name, dname.len);
	d = d_alloc(NULL, &dname);
    }

    /*
     *  The following also identifies the dentry as a LiS-allocated dentry.
     */
    if (d)
	d->d_op = &lis_dentry_ops;

    return d;
#else
    return(d_alloc_root(i)) ;
#endif
}


/*
 * A 2.6 kernel-ism.
 *
 * This is called when a file open is about to succeed.  The premise is
 * that we have replaced the dentry/inode that was originally passed to
 * lis_stropen so we need to "put" the corresponding cdev entry, if it
 * exists.
 *
 * The only way we have to get to the cdev entry is via the i_cdev field in
 * the inode.  The cdev structure has a list of inodes using it threaded 
 * through i_devices in the inode.
 *
 * It is extremely bad practice to dput the dentry/inode and have the inode
 * deallocated while it is still in the cdev list.  So if we are about to do 
 * that (d_count==1 and i_count==1) then we need to remove the inode from
 * the cdev's list.  We can't do this any earlier since by doing so we would
 * then lose our pointer back to the cdev structures, which would have
 * ref-counts too high.
 *
 * Note that file close does this via __fput().  So if the open fails
 * we don't call lis_cdev_put since there will be an __fput() done soon.
 * Also, if we did not replace the inode we also don't do this because
 * it will be done later at file close time.
 *
 * The spin lock that we use to protect this code takes care of the
 * case of simultaneous opens to LiS.  We really should use the kernel's
 * cdev_lock, but they did not export the symbol, so we can't.
 */
static void lis_cdev_put(struct dentry *d)
{
#if defined(KERNEL_2_5)
    struct inode	*inode = d->d_inode ;
    struct cdev		*cp ;
    static spinlock_t	 lock = SPIN_LOCK_UNLOCKED ;
#if HAVE_CDEV_PUT_ADDR
    static void (*cdev_put)(struct cdev *) = (typeof(cdev_put)) HAVE_CDEV_PUT_ADDR;
#endif


    if (LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
    {
	printk("lis_cdev_put: d@0x%p/%d/\"%s\" i@0x%p/%d\n",
		d, D_COUNT(d), d->d_name.name,
		inode, inode ? I_COUNT(inode) : 0) ;
	if (inode && inode->i_cdev)
	    printk(">> i->i_cdev: c@0x%p/%d/%x \"%s\"\n",
		inode->i_cdev,
#if HAVE_KMEMB_STRUCT_KOBJECT_KREF
		K_ATOMIC_READ(&inode->i_cdev->kobj.kref.refcount),
#else
		K_ATOMIC_READ(&inode->i_cdev->kobj.refcount),
#endif
		DEV_TO_INT(inode->i_cdev->dev),
		(inode->i_cdev->owner ?
		 inode->i_cdev->owner->name : "No-Owner")) ;
    }

    if (!inode || !(cp = inode->i_cdev))
	return ;

    spin_lock(&lock) ;
    if (   D_COUNT(d) == 1 && I_COUNT(inode) == 1
	&& !list_empty(&inode->i_devices))
    {
        inode->i_cdev = NULL ;
	list_del_init(&inode->i_devices);
    }

    cdev_put(cp) ;
    spin_unlock(&lock) ;
#endif
}

void lis_dput(struct dentry *d)
{
    struct super_block	*sb = NULL ;

    if (LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	lis_print_dentry(d, "lis_dput") ;

    if (d->d_inode)
    {
	sb = d->d_inode->i_sb ;		/* save before dput */
    }

    /*
     *  don't dput() a dentry to a - ref cnt; if that is needed,
     *  something else is wrong...
     */
    if (D_COUNT(d) > 0) {
	dput(d) ;

	if (sb && lis_mnt && sb == lis_mnt->mnt_sb)
	    MNTPUT(lis_mnt) ;
    }
}

struct dentry *lis_dget(struct dentry *d)
{
    if (d->d_inode != NULL && d->d_inode->i_sb == lis_mnt->mnt_sb)
	(void) MNTGET(lis_mnt) ;
    
    d = dget(d);

    if (LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	lis_print_dentry(d, "lis_dget") ;

    return(d) ;
}

/************************************************************************
*                          lis_select                                   *
*************************************************************************
*									*
* Called from the Linux sys_select routine to interrogate the status	*
* of a stream.								*
*									*
* Return 1 if the stream is ready for the requested operation.  	*
* Return 0, and set up the wait queue entry, if it is not.		*
*									*
************************************************************************/
#if !defined(LINUX_POLL)
int      lis_select(struct inode *inode, struct file *file,
		    int sel_type, select_table *wait)
{
    stdata_t		*hd;
    polldat_t		 pdat ;
    int			 evts ;
    int			 msk ;
    int			 rtn ;

    memset(&pdat, 0, sizeof(pdat)) ;		/* just in case */
retry:
    switch (sel_type)
    {
    case SEL_IN:
	pdat.pd_events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND ;
	msk = POLLNVAL ;
	break ;
    case SEL_OUT:
	pdat.pd_events = POLLOUT | POLLWRNORM | POLLWRBAND ;
	msk = POLLNVAL ;
	break ;
    case SEL_EX:
    default:
	pdat.pd_events = POLLMSG ;			/* SIG-POLL msg */
	msk = POLLERR | POLLHUP | POLLNVAL ;
	break ;
    }

    evts = lis_strpoll(inode, file, &pdat) ;	/* get stream status */
    rtn = ( (evts & (pdat.pd_events | msk)) != 0 ) ;/* expected status */
    if (rtn) return(1) ;			/* yes, can do that */

    /*
     * Requested events are not present, add ourselves to the
     * wait queue for the stream.  Set a flag to be noticed by
     * str_rput.
     *
     * If we get back from select_wait and the flag has been unset then
     * lis_select_wakeup got called before returning to us and possibly
     * before we even got into the wait list.  In that case, we go around
     * again and return the actual events.
     */
    hd = FILE_STR(file) ;
    SET_SD_FLAG(hd,STRSELPND);
    select_wait(&hd->sd_select.sel_wait, wait) ;
    if (!F_ISSET(hd->sd_flag,STRSELPND))	/* lost the race */
	goto retry ;

    return(0) ;

} /* lis_select */
#endif

/************************************************************************
*                        lis_select_wakeup                              *
*************************************************************************
*									*
* Wake up those waiting on this stream.					*
*									*
************************************************************************/
#if !defined(LINUX_POLL)
void lis_select_wakeup(stdata_t *hd)
{
    CLR_SD_FLAG(hd,STRSELPND);	/* no longer waiting on select */
    if (   hd == NULL
	|| hd->magic != STDATA_MAGIC
	|| hd->sd_select.sel_wait == NULL
	|| hd->sd_select.sel_wait == WAIT_QUEUE_HEAD(&hd->sd_select.sel_wait)
       )
	return ;

    wake_up_interruptible(&hd->sd_select.sel_wait) ;

} /* lis_select_wakeup */
#endif

/*
 *  a debugging routine to show the aliases for an inode
 */

void lis_show_inode_aliases( struct inode *i )
{
#if defined(KERNEL_2_1)
    struct list_head *ent;

    if (!(LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS) ||
	list_empty(&(i->i_dentry)))
	return;

    printk("lis_show_inode_aliases(i@0x%p/%d.%d#%lu)%s:\n",
	   i, I_COUNT(i), i->i_nlink, i->i_ino, (I_IS_LIS(i)?" <LiS>":""));

    /*
     *  show aliases in oldest-to-newest order
     */
    for (ent = i->i_dentry.prev;  ent != &(i->i_dentry);  ent = ent->prev) {
	struct dentry *d = list_entry( ent, struct dentry, d_alias );

	lis_print_dentry(d, ">> dentry") ;
    }
#endif
}

/*
 *  kernel assistance to show a file's full path (using kernel's
 *  __d_path() routine)
 */
char *lis_alloc_file_path(void)
{
    return (char *) __get_free_page(GFP_USER);
}

char *lis_format_file_path(struct file *f, char *page)
{
    if (page) {
#if defined(KERNEL_2_5)
	char *path =   d_path( f->f_dentry,
			       FILE_MNT(f),
			       page, PAGE_SIZE);
#else
	char *path = __d_path( f->f_dentry,
			       FILE_MNT(f),
			       FILE_MNT(f)->mnt_root,
			       NULL,
			       page, PAGE_SIZE);
#endif
	return path;
    } else
	return page;  /* which is NULL... */
}

void lis_free_file_path(char *page)
{
    if (page)
    	free_page((unsigned long)page);
}

void lis_print_file_path(struct file *f)
{
    char *page = lis_alloc_file_path();

    if (page) {
	char *path = lis_format_file_path(f, page);

	if (path)
	    printk("%s", path);

	lis_free_file_path(page);
    }
}


/*
 * lis_print_dentry
 */
void lis_print_dentry(struct dentry *d, char *comment)
{
    char	dname[100] ;
    int		len = sizeof(dname)-1 ;
    struct inode *i = (d ? d->d_inode : NULL) ;

    if (d == NULL)
    {
	printk("%s  NULL dentry pointer\n", comment) ;
	return ;
    }

    if (comment == NULL)
	comment = "" ;

    if (d->d_name.len < len)  len = d->d_name.len;
    strncpy(dname, d->d_name.name, len) ;
    dname[len] = 0 ;

    printk("%s: d@0x%p/%d%s m:%d", comment,
	   d, D_COUNT(d), (D_IS_LIS(d)?" <LiS>":""),
	       K_ATOMIC_READ(&lis_mnt_cnt));
    if (i)
    {
	printk(" i@0x%p/%d%s",
	       i, I_COUNT(i), (I_IS_LIS(i)?" <LiS>":""));
#if defined(KERNEL_2_5)
	if (i->i_cdev)
	    printk(" c@0x%p/%x\"%s\"", i->i_cdev, DEV_TO_INT(i->i_cdev->dev),
		(i->i_cdev->owner ? i->i_cdev->owner->name : "No-Owner")) ;
#endif
    }
    if (*dname)
	printk(" \"%s\"", dname );
    printk("\n");

    if (d && d->d_parent != NULL && d->d_parent != d)
	lis_print_dentry(d->d_parent, ">> parent") ;
}

/************************************************************************
*                         lis_super_statfs                              *
*************************************************************************
*									*
* Return file system stats.						*
*									*
************************************************************************/
#if defined(KERNEL_2_5)
int lis_super_statfs(struct super_block *sb, struct kstatfs *stat)
#else
int lis_super_statfs(struct super_block *sb, struct statfs *stat)
#endif
{
    stat->f_type = LIS_SB_MAGIC ;
    stat->f_bsize = 1024 ;
    stat->f_namelen = 255 ;
    return(0) ;

}

static
int lis_fs_fattach_sb( struct super_block *sb, void *ptr, int silent )
{
#if defined(FATTACH_VIA_MOUNT)
    lis_fattach_t *data = (ptr ? *((lis_fattach_t **) ptr) : NULL);
    struct file *file   = (data ? data->file : NULL);
    stdata_t *head      = (data ? data->head : NULL);
    const char *path    = (data ? data->path : NULL);
  
  if (file && head &&
      head == FILE_STR(file) &&
      head->magic == STDATA_MAGIC &&
      head->sd_inode) {
      /*
       *  this is an fattach via the mount() syscall - set it up
       */
      struct inode *i_mount = head->sd_inode;
      struct dentry *d_mount;

      lis_head_get(head);                  /* bumps refcnt */
      MNTSYNC();

      if (LIS_DEBUG_FATTACH || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS) {
	  printk("lis_fs_fattach_sb(s@0x%p,@0x%p,...) << [%d]"
		 " f@0x%p/%d f_vfsmnt 0x%p/%d%s%s\n",
		 sb, data,
		 K_ATOMIC_READ(&lis_mnt_cnt),
		 file, (file?F_COUNT(file):0),
		 FILE_MNT(file),
		 (FILE_MNT(file)?MNT_COUNT(FILE_MNT(file)):0),
		 (S_IS_LIS(sb)?" <LiS>":""),
		 (file&&FILE_MNT(file)==lis_mnt?" <lis_mnt>":""));
      }

      LOCK_INO(i_mount);
      
      sb->s_root = d_mount = lis_d_alloc_root(igrab(i_mount),
					      LIS_D_ALLOC_ROOT_MOUNT);
      if (sb->s_root == NULL) {
	  ULOCK_INO(i_mount);
	  lis_head_put(head);
	  return(-ENOMEM) ;
      }
      S_FS_INFO(sb) = data;
      if (FILE_MNT(file))
	  data->mount = MNTGET(FILE_MNT(file));
      else 
	  data->mount = file->f_vfsmnt = MNTGET(lis_mnt);
      data->sb      = sb;
      data->dentry  = d_mount;

      /*
       *  sd_refcnt used to keep a stream alive until all uses were done,
       *  but doclose essentially ignores it now, and uses sd_opencnt
       *  instead.  So, we have to bump both sd_refcnt (via lis_head_get())
       *  and sd_opencnt in order to make sure this stream doesn't disappear
       *  when the attaching file is closed - the file has to disappear, but
       *  the stream has to stay "open".
       */
      K_ATOMIC_INC(&head->sd_opencnt);

      lis_spin_lock(&lis_fattaches_lock);
      K_ATOMIC_INC(&head->sd_fattachcnt);
      SET_SD_FLAG(head,STRATTACH);
      lis_spin_unlock(&lis_fattaches_lock);
      
      /*
       *  we need the following in order to be able to both read and
       *  write the fattached stream, if permissions are appropriately
       *  set as well - the kernel makes it read-only otherwise
       */
      allow_write_access(file);

      ULOCK_INO(i_mount);
      
      if ((LIS_DEBUG_FATTACH || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS) &&
	  path) {
	  dev_t dev = GET_I_RDEV(i_mount);
          printk("lis_fs_fattach_sb(s@0x%p,@0x%p,%d) "
		 ">> d@0x%p/%d i@0x%p/%d rdev (%d,%d)\n",
		 sb, data, silent,
		 d_mount, D_COUNT(d_mount),
		 i_mount, I_COUNT(i_mount),
		 getmajor(dev), getminor(dev) );
	  printk("lis_fs_fattach_sb(s@0x%p,@0x%p,...)\n"
		 "    >> f@0x%p/%d h@0x%p/%d/%d \"%s\""
		 " sb@0x%p d@0x%p/%d (i@0x%p/%d)\n",
		 sb, data,
		 file, F_COUNT(file),
		 head, LIS_SD_REFCNT(head), LIS_SD_OPENCNT(head),
		 path,
		 data->sb,
		 d_mount, D_COUNT(d_mount),
		 i_mount, I_COUNT(i_mount));
	  lis_show_inode_aliases(i_mount);
      }

      if ((LIS_DEBUG_FATTACH || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS) &&
	  file) {
	  printk("lis_fs_fattach_sb(s@0x%p,@0x%p,...) >> [%d]"
		 " f@0x%p/%d f_vfsmnt 0x%p/%d%s%s\n",
		 sb, data,
		 K_ATOMIC_READ(&lis_mnt_cnt),
		 file, F_COUNT(file),
		 FILE_MNT(file),
		 (FILE_MNT(file)?MNT_COUNT(FILE_MNT(file)):0),
		 (S_IS_LIS(sb)?" <LiS>":""),
		 (FILE_MNT(file)==lis_mnt?" <lis_mnt>":""));
      }

      MNTSYNC();

      return(0);
  } else
      return(-EINVAL);  /* this must be a stream */
#else			/* FATTACH_VIA_MOUNT */
  return(-ENOSYS) ;	/* not implemented */
#endif			/* FATTACH_VIA_MOUNT */
}

static
int lis_fs_kern_mount_sb( struct super_block *sb, void *ptr, int silent )
{
    struct inode *isb = lis_get_new_inode(sb) ;

    if (isb == NULL)
      return(-ENOMEM) ;
  
    isb->i_mode  = S_IFDIR | S_IRUSR | S_IWUSR ;
    isb->i_uid   = 0 ;
    isb->i_gid   = 0 ;
    isb->i_atime = CURRENT_TIME ;
    isb->i_mtime = CURRENT_TIME ;
    isb->i_ctime = CURRENT_TIME ;
    isb->i_op    = &lis_streams_iops;
#if defined(KERNEL_2_5)
    isb->i_rdev  = makedevice(lis_major, 0);	/* LiS dev_t */
#else
    isb->i_rdev  = MKDEV(lis_major, 0);	/* kernel dev_t */
    {
	lis_inode_info_t *p = (lis_inode_info_t *) &isb->u ;
	p->dev = makedevice(lis_major, 0) ;
    }
#endif
    
    sb->s_root = lis_d_alloc_root(isb,LIS_D_ALLOC_ROOT_MOUNT);
    if (sb->s_root == NULL)
        return(-ENOMEM) ;

    if (LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	printk("lis_fs_kern_mount_sb(s@x%p,...)"
	       " >> root d@0x%p/%d i@0x%p/%d rdev (%d,%d)\n",
	       sb,
	       sb->s_root, D_COUNT(sb->s_root),
	       isb, I_COUNT(isb),
	       getmajor(GET_I_RDEV(isb)),
	       getminor(GET_I_RDEV(isb)) );
    
    return(0);
}

int lis_fs_setup_sb(struct super_block *sb, void *ptr, int silent)
{
    sb->s_magic		 = LIS_SB_MAGIC ;
    sb->s_blocksize	 = 1024 ;
    sb->s_blocksize_bits = 10 ;
    sb->s_op		 = &lis_super_ops ;
 
    if (ptr) 
        return lis_fs_fattach_sb( sb, ptr, silent );
    else
        return lis_fs_kern_mount_sb( sb, ptr, silent );
}

#if defined(KERNEL_2_5)
struct super_block *lis_fs_get_sb(struct file_system_type *fs_type,
				  int flags,
				  const char *dev_name,
				  void *ptr)
{
    struct super_block *sb;
  /*
   * Not sure which technique to use.  May need to use get_sb_single
   * in order to utilize sys_mount to implement fattach.  We'll see.
   *
   * 2002/11/18 - nodev is the right one for fattach... - JB
   */
#if defined(FATTACH_VIA_MOUNT) && 1
    sb = get_sb_nodev(fs_type, flags, ptr, lis_fs_setup_sb);
#else
    sb = get_sb_single(fs_type, flags, ptr, lis_fs_setup_sb);
#endif
    return sb;
}

void lis_fs_kill_sb(struct super_block *sb)
{
    kill_anon_super(sb);
}
#else		/* 2.4 kernel */
struct super_block *lis_fs_read_super(struct super_block *sb,
				      void *ptr,
				      int   silent)
{
    if (lis_fs_setup_sb(sb, ptr, silent) < 0)
        return(NULL) ;

    return(sb) ;
}
#endif

/************************************************************************
*                         lis_dentry_delete                             *
*************************************************************************
*									*
* This routine has to be here (in 2.4 kernel) and has to return 1 in	*
* order to get the kernel's dput routine to go ahead and iput the inode	*
* beneath the dentry when the dentry's d_count goes to zero.  Duh.  Like*
* the default behavior should be to just keep the dangling inode around	*
* until the file system unmounts so that then the kernel can print a	*
* message about dangling inodes.					*
*									*
************************************************************************/
int lis_dentry_delete(struct dentry *d)
{
    if (LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	printk("lis_dentry_delete(d@0x%p/%d) [%d]%s\n",
	       d, D_COUNT(d),
	       K_ATOMIC_READ(&lis_mnt_cnt),
	       (D_IS_LIS(d)?" d<LiS>":""));

    return(1);
}

void lis_dentry_iput( struct dentry *d, struct inode *i )
{
    if (LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	lis_print_dentry(d, "lis_dentry_iput") ;
    
    if (i && I_COUNT(i) > 0)
	lis_put_inode(i);
}

/************************************************************************
*                        lis_super_put_inode                            *
*************************************************************************
*									*
* Called when an LiS inode is being "iput".				*
*									*
************************************************************************/
void lis_super_put_inode(struct inode *i)
{
    MNTSYNC();

    if (LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
    {
	printk("lis_super_put_inode(i@0x%p/%d)%s "
	       "i_rdev=0x%x <[%d] %d LiS inode(s)>%s\n",
	       i, I_COUNT(i), (I_IS_LIS(i)?" <LiS>":""),
	       GET_I_RDEV(i),
	       K_ATOMIC_READ(&lis_mnt_cnt),
	       K_ATOMIC_READ(&lis_inode_cnt),
	       (I_COUNT(i) == 1 ? "--" : ""));
    }

    if (I_COUNT(i) <= 1)
    {
	if (I_COUNT(i) < 1)
	    printk("lis_super_put_inode(i@0x%p/%d)%s i_rdev=0x%x UNUSED"
		   " <[%d] %d LiS inode(s)>\n",
		   i, I_COUNT(i), (I_IS_LIS(i)?" <LiS>":""),
		   GET_I_RDEV(i),
		   K_ATOMIC_READ(&lis_mnt_cnt),
		   K_ATOMIC_READ(&lis_inode_cnt));
	else
	    K_ATOMIC_DEC(&lis_inode_cnt) ;

#if !defined(KERNEL_2_5)
	force_delete(i) ;
#endif
    }
}

#if defined(FATTACH_VIA_MOUNT)
/*
 *  lis_super_ops hooks for supporting fdetach() via sys_umount[2]() -
 *
 *  The following two routines work together to make sure that when
 *  umount[2] is called to do an fdetach(), all the work actually
 *  gets done.  The intent is that 'umount2( path, MNT_FORCE )' will
 *  be invoked, and this is exactly equivalent to 'umount -f path'
 *  from a command line.  Unfortunately, that means that the '-f'
 *  flag could be omitted if attempted from the command line.  Luckily,
 *  (in 2.4.19, anyway - not yet tested in 2.5.x), this case still
 *  manages to invoke lis_super_put_super(), but without invoking
 *  lis_super_umount_begin() first, and after partially clobbering
 *  the superblock and dentry.  So, here, we support both the
 *  intended pattern of umount_begin() -> put_super(), but also
 *  put_super() -> umount_begin().
 *
 *  Keep your fingers crossed - this is likely to be affected by
 *  kernel changes... 8^(
 */
void lis_super_umount_begin( struct super_block *sb )
{
    lis_fattach_t *data     = (lis_fattach_t *) S_FS_INFO(sb);
    struct file *file       = (data ? data->file : NULL);
    struct vfsmount *mount  = (data ? data->mount : NULL);
    stdata_t *head          = (data ? data->head : NULL);
    struct dentry *d_umount = (data ? data->dentry : NULL);
    struct inode *i_umount  = (d_umount ? d_umount->d_inode : NULL);

    MNTSYNC();

    /*
     *  If put_super() was called first, 'd_umount' has been partially
     *  clobbered and its inode has been disconnected already -
     *  deal with that by using the (same) inode from the head
     */
    if (data && head && d_umount && !i_umount && head->sd_inode)
	i_umount = head->sd_inode;

    if (LIS_DEBUG_FATTACH || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	printk("lis_super_umount_begin(s@0x%p) << data@0x%p:\n"
	       "    f@0x%p m@0x%p/%d h@0x%p/%d/%d"
	       " d@0x%p/%d (i@0x%p/%d)\n",
	       sb, data, file,
	       mount, (mount?MNT_COUNT(mount):0),
	       head,
	       (head?LIS_SD_REFCNT(head):0),
	       (head?LIS_SD_OPENCNT(head):0),
	       d_umount, (d_umount?D_COUNT(d_umount):0),
	       i_umount, (i_umount?I_COUNT(i_umount):0));

    /*
     *  make sure we have what we need - if we don't, it's not an
     *  fdetach()
     */
    if (data && head && d_umount && i_umount) {
	cred_t creds;

	LOCK_INO(i_umount);

	/*
	 *  give back the write access we established at fattach time
	 */
	put_write_access(i_umount);
	/*
	 *  give back the mount count we set at fattach time
	 */
	if (mount)
	    MNTPUT(mount);

	if (LIS_DEBUG_FATTACH || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS) {
	    printk("lis_super_umount_begin(%p) [fdetach] "
		   "i@0x%p/%d d@0x%p/%d \"%s\"\n",
		   sb,
		   i_umount, I_COUNT(i_umount),
		   d_umount, D_COUNT(d_umount),
		   data->path );

	    lis_show_inode_aliases(i_umount);
	}

	creds.cr_uid  = current->euid;
	creds.cr_gid  = current->egid;
	creds.cr_ruid = current->uid;
	creds.cr_rgid = current->gid;

	/*
	 *  clear the STRATTACH flag if no other fattaches for this stream
	 */
	lis_spin_lock(&lis_fattaches_lock);
	K_ATOMIC_DEC(&head->sd_fattachcnt);
	if (K_ATOMIC_READ(&head->sd_fattachcnt) <= 0)
	    CLR_SD_FLAG(head,STRATTACH);
	lis_spin_unlock(&lis_fattaches_lock);

	ULOCK_INO(i_umount);

	lis_doclose( i_umount, NULL, head, &creds );

	/*
	 *  the file field isn't valid after the fattach, but we
	 *  wait to clear it to indicate we've been here, in case
	 *  'umount <path>' instead of 'umount -f <path' gets used
	 *  and lis_super_put_super() is invoked first as a result
	 *
	 *  similarly, we may get here when the fattach is "busy",
	 *  and need to come back again (e.g., via fattach_all()).
	 *  we clear the head field (which put_super() doesn't use)
	 *  so we won't come back here again.
	 */
	data->mount = NULL;
	data->file  = NULL;
	data->head  = NULL;

	if (LIS_DEBUG_FATTACH || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	    printk("lis_super_umount_begin(s@0x%p) [fdetach] "
		   "d@0x%p/%d (i@0x%p/%d) after doclose\n",
		   sb,
		   d_umount, (d_umount?D_COUNT(d_umount):0),
		   i_umount, (i_umount?I_COUNT(i_umount):0));
    }

    MNTSYNC();
}

void lis_super_put_super( struct super_block *sb )
{
    lis_fattach_t *data     = (lis_fattach_t *) S_FS_INFO(sb);
    struct dentry *d_umount = (data ? data->dentry : NULL);
    struct inode *i_umount  = (d_umount ? d_umount->d_inode : NULL);

    MNTSYNC();

    if (data && data->file) {
	if (LIS_DEBUG_FATTACH)
	    printk("lis_super_put_super(s@0x%p) [fdetach] invoked w/o"
		   " lis_super_umount_begin()!!!\n",
		   sb);

	lis_super_umount_begin(sb);

	/* make sure these are still valid */
	data     = (lis_fattach_t *) S_FS_INFO(sb);
	d_umount = (data ? data->dentry : NULL);
	/*
	 *  don't bother getting the inode from the dentry - it's been
	 *  clobbered by now - get it from the head
	 */
	i_umount = (data && data->head ? data->head->sd_inode : NULL);
    }
    if (d_umount && i_umount) {
	if (LIS_DEBUG_FATTACH || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	    printk("lis_super_put_super(s@0x%p) [fdetach] "
		   "d@0x%p/%d (i@0x%p/%d)\n",
		   sb,
		   d_umount, D_COUNT(d_umount),
		   i_umount, (i_umount ? I_COUNT(i_umount) : 0) );

	lis_dput(d_umount);
    }
    if (data) {
	/*
	 *  remove the fattach instance from the list and delete it
	 */
	lis_fattach_remove(data);
	lis_fattach_delete(data);

	if (LIS_DEBUG_FATTACH)
	    printk("lis_super_put_super(s@0x%p) [fdetach] done\n", sb);
    }

    S_FS_INFO(sb) = NULL;

    MNTSYNC();
}
#endif /* FATTACH_VIA_MOUNT */

/************************************************************************
*                         lis_new_file_name                             *
*************************************************************************
*									*
* Change the name of the "file".  This means reallocate the dentry.	*
* If the allocation of the new dentry fails we just stick with the old	*
* one.									*
*									*
* This is not a rename operation.  We give up the old dentry and 	*
* allocate a new one with the new name.  The old dentry might actually	*
* represent some real file such as /dev/loop_clone.			*
*									*
* We allocate a new dentry using d_alloc.  We do not do a lookup on	*
* the name, because we want each stream to have a separate dentr and	*
* inode even if two streams have the same name.				*
*									*
************************************************************************/
int	lis_new_file_name(struct file *f, const char *name)
{
    /*
     *  don't do this if the dentry is already LiS and already has
     *  the desired name - we'd be changing dentries for nothing...
     */
    if (D_IS_LIS(f->f_dentry) &&
	strcmp(name, f->f_dentry->d_name.name) == 0) {
	if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	    printk("lis_new_file_name(f@0x%p/%d,\"%s\")"
		   " - same name, already <LiS> - ignoring call\n",
		   f, F_COUNT(f), name);
	return(0);
    } else
	return(lis_new_file_name_dev(f, name, 0)) ;
}

int	lis_new_file_name_dev(struct file *f, const char *name, dev_t dev)
{
    struct qstr     dname ;
    struct dentry  *new ;
    struct dentry  *old = f->f_dentry;
    struct dentry  *lis_parent ;
    struct inode   *oldi = NULL;
    struct vfsmount *oldmnt = FILE_MNT(f);

    if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS) {
	struct dentry *d = (f ? f->f_dentry : NULL);

	printk("lis_new_file_name_dev(f@0x%p/%d,\"%s\",0x%x)%s",
	       f, (f?F_COUNT(f):0), (name?name:""), dev,
	       (FILE_MNT(f)==lis_mnt?" <lis_mnt>":""));
	printk(" \"");
	if (FILE_MNT(f))  lis_print_file_path(f);
	printk("\"\n");
	lis_print_dentry(d, ">> dentry") ;
    }
    
    if (dev == 0)			/* must use old inode */
    {
	if (old != NULL)
	    oldi = old->d_inode ;
	else
	    return(-EINVAL) ;
    }
    else
    if ((oldi = lis_get_inode(S_IFCHR|S_IRUSR|S_IWUSR, dev)) == NULL)
	return(-ENOMEM) ;
    else
	oldi->i_mode &= ~current->fs->umask ;	/* umask considerations */

    dname.name = name ;			/* set up for d_alloc */
    dname.len  = strlen(name) ;
    dname.hash = full_name_hash(dname.name, dname.len) ;
    lis_parent = lis_mnt->mnt_sb->s_root ;
    new        = d_alloc(lis_parent, &dname) ;

    if (IS_ERR(new))			/* couldn't */
    {
	if (dev != 0)
	    lis_put_inode(oldi) ;
	return(PTR_ERR(new)) ;
    }

    /*
     *  we may have created a new file pointer with no dentry or inode
     *  and no f_vfsmnt set.  In that case, there's nothing to put, and
     *  we will do the initial setting of f_vfsmnt here.
     */
    f->f_vfsmnt = MNTGET(lis_mnt);	/* (re)mount on LiS */

    new->d_op   = &lis_dentry_ops;

    if (dev == 0)
    {					/* using old inode */
	d_add(new, igrab(oldi)) ;	/* old inode into new dentry */
	lis_dput(old) ;			/* does mntput if lis_mnt */
	if (oldmnt && oldmnt != lis_mnt)
	    MNTPUT(oldmnt) ;	        /* do it if not lis_mnt also */
    }
    else				/* using new inode */
	d_add(new, oldi) ;		/* new inode into new dentry */

    f->f_dentry = new ;			/* d_alloc set count */

    if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS) {
	struct dentry *d = (f ? f->f_dentry : NULL);

	printk("lis_new_file_name_dev(f@0x%p/%d,\"%s\",0x%x)%s",
	       f, (f?F_COUNT(f):0), (name?name:""), dev,
	       (FILE_MNT(f)==lis_mnt?" <lis_mnt>":""));
	printk(" \"");
	if (FILE_MNT(f))  lis_print_file_path(f);
	printk("\"\n");
	lis_print_dentry(d, ">> dentry") ;
    }

    return(0) ;
}

/*  -------------------------------------------------------------------  */
/* lis_new_stream_name							 */
/*
 * Helper routine to set up the name of the stream and associated file
 */
void lis_new_stream_name(struct stdata *head, struct file *f)
{
    if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
    {
	printk("lis_new_stream_name(h@0x%p/%d/%d,f@0x%p/%d)\n",
	       head, LIS_SD_REFCNT(head), LIS_SD_OPENCNT(head),
	       f, F_COUNT(f)) ;
	lis_print_dentry(f->f_dentry, ">> dentry") ;
    }

    sprintf(head->sd_name, "%s%s",
	    lis_strm_name(head), lis_maj_min_name(head));

    lis_mark_mem(head, head->sd_name, MEM_STRMHD) ;
    lis_new_file_name(f, head->sd_name) ;

    if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
    {
	printk("lis_new_stream_name(h@0x%p/%d/%d,f@0x%p/%d)\n",
	       head, LIS_SD_REFCNT(head), LIS_SD_OPENCNT(head),
	       f, F_COUNT(f)) ;
	lis_print_dentry(f->f_dentry, ">> dentry") ;
    }
}

/************************************************************************
*                            lis_new_inode                              *
*************************************************************************
*									*
* This routine is called from stropen().				*
*									*
* 'old' is the inode passed to stropen for a clone open.  'oldf'	*
* is the file structure for the file that was opened.  The stdata	*
* structure hangs off of the file structure.				*
*									*
* 'dev' is the major/minor for the device that was really opened.	*
* Presumably, we are called because the i_rdev of the inode struct	*
* does not match up with dev.						*
*									*
* We need to allocate a new inode struct, and dentry for 2.2 kernels,	*
* and get them linked to this file.  The old inode and dentry		*
* represent the clone device and not the device that was actually	*
* opened.								*
*									*
* This operation is performed for every stream open whether clone or not*
*									*
************************************************************************/
struct inode *
lis_new_inode( struct file *f, dev_t dev )
{
    struct inode *old = FILE_INODE(f);
    struct inode *new;
    stdata_t	 *hd = FILE_STR(f) ;
    struct dentry *oldd = f->f_dentry;
    struct dentry *newd = NULL;
    struct qstr    dname ;
    struct vfsmount *oldmnt = FILE_MNT(f);

    if (!old)
    {						/* param checking */
	printk("lis_new_inode(f@0x%p/%d,d0x%x) - "
	       "old inode must be non-NULL\n",
	       f, F_COUNT(f), DEV_TO_INT(dev));
	return(NULL) ;				/* bad return */
    }

    if (lis_mnt == NULL)
    {
	printk("lis_new_inode(f@0x%p/%d,d0x%x) - "
	       "LiS has been unmounted\n",
	       f, F_COUNT(f), DEV_TO_INT(dev));
	return(NULL) ;				/* bad return */
    }

    if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
    {
	printk("lis_new_inode(f@0x%p/%d,dv0x%x) %s%s",
	       f, F_COUNT(f), DEV_TO_INT(dev),
	       (D_IS_LIS(f->f_dentry)?" <LiS>":""),
	       (FILE_MNT(f)==lis_mnt?" <lis_mnt>":"")
	      ) ;
	printk(" \"");
	if (FILE_MNT(f))  lis_print_file_path(f);
	printk("\"\n");
	lis_print_dentry(oldd, ">> oldd") ;
    }

    if (hd == NULL)
    {
	printk("lis_new_inode(f@0x%p/%d,d0x%x) - "
	       "no STREAM head\n",
	       f, F_COUNT(f), DEV_TO_INT(dev));
	return(NULL) ;				/* bad return */
    }

    /*
     *  we keep an old inode only if it's a LiS-only inode.  Otherwise,
     *  we'll replace a non-LiS inode here with one that is LiS-only
     */
    if (I_IS_LIS(old) && DEV_SAME(GET_I_RDEV(old), dev))
	return (old);

    new = lis_get_inode( old->i_mode, dev ) ;
    if (new != NULL)
    {						/* got a new inode */
	new->i_state = I_DIRTY;       /* keep it off the dirty list */
	new->i_mode  = old->i_mode;		/* inherit mode */
	/*
	 * Set the user/group ids to the opener, set modification times
	 * to the current time.
	 */
	new->i_uid   = current->fsuid;
	new->i_gid   = current->fsgid;
	new->i_atime = new->i_mtime = new->i_ctime = CURRENT_TIME;
	/*
	 * It is difficult to detach an inode from a dentry without
	 * dput-ing the whole dentry, what with alias lists and all.
	 * So we just allocate a new dentry with LiS as the parent
	 * directory and install our new inode into it.  We can then
	 * just dput the old dentry and be done with it.
	 */
	if (hd->sd_name[0])			/* have name for strm yet?  */
	{
	    dname.name = hd->sd_name ;
	    dname.len  = strlen(hd->sd_name) ;
	}
	else					/* use name from old dentry */
	{
	    dname.name = oldd->d_name.name ;
	    dname.len  = oldd->d_name.len ;
	}

	dname.hash = full_name_hash(dname.name, dname.len) ;
	newd = d_alloc(lis_mnt->mnt_sb->s_root, &dname) ;
	if (newd == NULL)
	{
	    lis_put_inode(new) ;			/* oops, couldn't */
	    return(NULL) ;
	}

	/*
	 *  we may have created a new file pointer with no dentry or inode
	 *  and no f_vfsmnt set.  In that case, there's nothing to put, and
	 *  we will do the initial setting of f_vfsmnt here.
	 */
	f->f_vfsmnt = MNTGET(lis_mnt);		/* (re)mount on LiS */
	lis_dput(oldd) ;                        /* does mntput() if lis_mnt */
	if (oldmnt && oldmnt != lis_mnt)
	    MNTPUT(oldmnt) ;                    /* do it if not lis_mnt also */

	newd->d_op = &lis_dentry_ops;
	d_add(newd, new) ;			/* add inode to new dentry */
	f->f_dentry = newd ;
    }

    if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
    {
	printk("lis_new_inode(f@0x%p/%d,d0x%x)%s%s",
	       f, F_COUNT(f), DEV_TO_INT(dev),
	       (D_IS_LIS(f->f_dentry)?" <LiS>":""),
	       (f&&FILE_MNT(f)==lis_mnt?" <lis_mnt>":"")
	      ) ;
	printk(" \"");
	if (f && FILE_MNT(f))  lis_print_file_path(f);
	printk("\"\n");
	lis_print_dentry(newd, ">> newd") ;
    }

    return(new) ;				/* use new inode */
    						/* possibly NULL return */
} /* lis_new_inode */

/*
 *  lis_cleanup_file... - do whatever "puts" need to be done to get
 *  operating system object usage counts decremented.
 *
 *  NB - the counts passed in here may or may not be useful; they're
 *  here for possible future use as much as anything (currently, only
 *  oldmnt_cnt is being used).
 */
void lis_cleanup_file_opening(struct file *f, stdata_t *head,
			       int open_fail,
			       struct dentry *oldd, int oldd_cnt,
			       struct vfsmount *oldmnt, int oldmnt_cnt)
{
    if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
    {
        printk("lis_cleanup_file_opening(f@0x%p/%d,h@0x%p/%d/%d,%d) %s\n",
	       f, (f?F_COUNT(f):0),
	       head,
	       (head?LIS_SD_REFCNT(head):0),
	       (head?LIS_SD_OPENCNT(head):0),
	       open_fail,
	       (FILE_MNT(f)==lis_mnt?" <lis_mnt>":"")) ;

	printk(" \"");
	if (FILE_MNT(f))  lis_print_file_path(f);
	printk("\"\n");
	lis_print_dentry(oldd, ">> dentry") ;
	printk("\n");
    }

    if (open_fail) {
	/*
	 *  open_fail: sys_open has the old dentry and mnt, and will put
	 *  them.  Synchronize accordingly...
	 *
	 *  When open fails chrdev_open does a cdev_put on the cdev
	 *  dangling off of the inode, so we don't do it here.
	 *
	 *  If we're not hooked to the original dentry/inode, put the
	 *  ones we're hooked to now, since sys_open() doesn't care
	 *  about them and won't put them.
	 *
	 */
	if (f->f_dentry != oldd)
	    (void)lis_dput(f->f_dentry);

	if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	    printk("    error %d >> oldmnt@0x%p/%d %c oldmnt_cnt %d\n",
		   open_fail,
		   oldmnt, MNT_COUNT(oldmnt),
		   (MNT_COUNT(oldmnt) < oldmnt_cnt ? '<' :
		    MNT_COUNT(oldmnt) > oldmnt_cnt ? '>' : '='),
		   oldmnt_cnt);
	if (MNT_COUNT(oldmnt) < oldmnt_cnt)
	    (void)MNTGET(oldmnt);
	else
	if (MNT_COUNT(oldmnt) > oldmnt_cnt)
	    MNTPUT(oldmnt);
    } else {
	/*
	 *  open OK - these are extra
	 */
	if (f->f_dentry != oldd)
	    lis_cdev_put(oldd) ;
	lis_dput(oldd);
	MNTPUT(oldmnt);
    }
}

void lis_cleanup_file_closing(struct file *f, stdata_t *head)
{
    /*
     *  close KLUDGE: if the file's dentry count is > 1, the kernel
     *  won't do anything but decrement the count.  Among other things
     *  (possibly), it won't decrement the mount count.  So, we do that
     *  here for lis_mnt, if that's where we are (and it should be...).
     */
    struct dentry *d = (f ? f->f_dentry : NULL);

    if (f && d && D_IS_LIS(d) && D_COUNT(d) > 1 && FILE_MNT(f) == lis_mnt)
	MNTPUT(lis_mnt);

    if (LIS_DEBUG_VCLOSE || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
    {
        printk("lis_cleanup_file_closing(f@0x%p/%d,h@0x%p/%d/%d)"
	       " [%d]%s%s",
	       f, (f?F_COUNT(f):0),
	       head,
	       (head?LIS_SD_REFCNT(head):0),
	       (head?LIS_SD_OPENCNT(head):0),
	       K_ATOMIC_READ(&lis_mnt_cnt),
	       (D_IS_LIS(f->f_dentry)?" <LiS>":""),
	       (FILE_MNT(f)==lis_mnt?" <lis_mnt>":"")
	      ) ;
	printk(" \"");
	if (FILE_MNT(f))  lis_print_file_path(f);
	printk("\"\n");
	lis_print_dentry(d, ">> dentry") ;
    }
}

#if defined(KERNEL_2_5)
/*
 * lis_strflush - see file_operations
 */
int
lis_strflush( struct file *f )
{
    int err = 0;

    if (LIS_DEBUG_VCLOSE || LIS_DEBUG_REFCNTS)
	printk("lis_strflush(f@0x%p)\n", f);

    MODSYNC();

    return err;
}
#endif

/*
 * lis_inode_lookup - must be present for namei on LiS mounted file system
 * to work properly.  Return of NULL should suffice.
 */
#if defined(KERNEL_2_5)

struct dentry *lis_inode_lookup(struct inode *dir, struct dentry *dentry,
				struct nameidata *nd)
{
    return(NULL) ;
}

void lis_drop_inode(struct inode *inode)
{
    generic_delete_inode(inode) ;
}

#else

struct dentry *lis_inode_lookup(struct inode *dir, struct dentry *dentry)
{
    return(NULL) ;
}

#endif

/*
 *  lis_get_filp() - get a LiS-ready file pointer, or set one up for LiS
 */
static struct file *
lis_get_filp( struct file_operations *f_op )
{
    struct file *f = get_empty_filp();

    if (!f)  return NULL;

    f->f_pos    = 0;
    f->f_op     = fops_get(f_op);	/* bumps module ref count */
    f->f_flags  = O_RDWR;
    f->f_mode   = FMODE_READ|FMODE_WRITE;

    if (LIS_DEBUG_VOPEN || LIS_DEBUG_REFCNTS)
	printk("lis_get_filp(...) >> f@0x%p\n", f);

    return f;
}

/*
 * lis_set_up_inode() - make an inode look like an LiS inode
 *
 * Used in stream head when a stream file is opened.
 */
struct inode *lis_set_up_inode(struct file *f, struct inode *inode)
{
    struct inode	*new ;

    if (inode == NULL) return(NULL) ;

    new = lis_new_inode(f, GET_I_RDEV(inode)) ;
    if (new == NULL)
	return(NULL) ;

    /*
     * Set the user/group ids to the opener, set modification times
     * to the current time.
     */
    new->i_uid   = current->fsuid;
    new->i_gid   = current->fsgid;
    new->i_atime = new->i_mtime = new->i_ctime = CURRENT_TIME;

    return(new) ;
}

/*
 * lis_is_stream_inode
 */
int lis_is_stream_inode(struct inode *i)
{
    return(I_IS_LIS(i)) ;
}

/*
 *  lis_get_inode() - allocate and setup a LiS-only inode
 */
static struct inode *
lis_get_inode( mode_t mode, dev_t dev )
{
    struct inode *i = lis_get_new_inode(lis_mnt->mnt_sb);

    if (i)
    {
	i->i_mode  = mode;
	/*
	 *  see the comment above (in lis_new_inode) about the use of
	 *  the dev and rdev fields
	 */
	i->i_op    = &lis_streams_iops;
/*
 *  FIXME - generalize to use an fops per major, so modules can own
 *  their open files.
 */
	i->i_fop   = &lis_streams_fops;
	/*
	 *  char devs are identified by the rdev field; dev identifies
	 *  the hosting file system.  Here, we construct our own dev
	 *  field, reflecting that this is a LiS-only inode which has
	 *  no file system hosting it (other than LiS itself)
	 */
#if defined(KERNEL_2_5)
	i->i_rdev  = DEV_TO_RDEV(dev);	/* set desired dev */
#else
	/*
	 * i_rdev will show our minor device number modulo 256
	 */
	i->i_rdev       = MKDEV( getmajor(dev), getminor(dev) );
	{
	    lis_inode_info_t *p = (lis_inode_info_t *) &i->u ;
	    p->dev = dev ;
	}
#endif

	if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	    printk("lis_get_inode(m0x%x,dv0x%x) >> i@0x%p/%d"
		   " <[%d] %d LiS inodes>\n",
		   mode, dev,
		   i, I_COUNT(i),
		   K_ATOMIC_READ(&lis_mnt_cnt),
		   K_ATOMIC_READ(&lis_inode_cnt));
    }

    return i;
}

/*
 *  lis_old_inode() - swap the inode referenced by f with that referenced
 *  by i.
 */
struct inode *
lis_old_inode( struct file *f, struct inode *i )
{
    struct dentry *oldd = f->f_dentry;
    struct dentry *newd ;
    struct vfsmount *oldmnt = FILE_MNT(f);

    if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS)
	printk("lis_old_inode(f@0x%p/%d,i@0x%p/%d)%s << "
	       "i@0x%p/%d%s (dev 0x%x -> 0x%x)\n",
	       f, F_COUNT(f),
	       i, I_COUNT(i),
	       (I_IS_LIS(FILE_INODE(f))?" <LiS>":""),
	       FILE_INODE(f), I_COUNT(FILE_INODE(f)),
	       (I_IS_LIS(i)?" <LiS>":""),
	       GET_I_RDEV(FILE_INODE(f)),
	       GET_I_RDEV(i));

    newd = lis_d_alloc_root(igrab(i), LIS_D_ALLOC_ROOT_NORMAL);
    if (IS_ERR(newd))
    {
       iput(i);
       return NULL;
    }
    f->f_dentry = newd;

    /*
     *  the caller may have created a new file pointer with no dentry
     *  or inode and no f_vfsmnt set.  In that case, we are doing the
     *  initial setting of f_vfsmnt here.
     */
    f->f_vfsmnt = MNTGET(lis_mnt);	/* (re)mount on LiS */
    lis_dput(oldd);
    if (oldmnt && oldmnt != lis_mnt)  /* lis_dput() does mntput() on lis_mnt */
	MNTPUT(oldmnt);

    if (LIS_DEBUG_VOPEN || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
    {
	printk("lis_old_inode(f@0x%p/%d,i@0x%p/%d)%s\n",
	       f, F_COUNT(f),
	       i, I_COUNT(i),
	       (I_IS_LIS(FILE_INODE(f))?" <LiS>":""));
	lis_print_dentry(newd, ">> dentry") ;
    }

    return (FILE_INODE(f));
}

struct inode *lis_grab_inode(struct inode *ino)
{
    return(igrab(ino)) ;
}

void lis_put_inode(struct inode *ino)
{
    iput(ino) ;
}


/*
 *  lis_get_fifo() - create a new unique fifo
 *
 *  note that lis_get_filp() uses lis_streams_fops, which is what makes
 *  mode S_IFIFO a STREAM instead of a kernel FIFO; i.e., it ensures
 *  that stropen will be called to open the FIFO.
 */
int lis_get_fifo( struct file **f )
{
    dev_t	dev = makedevice( LIS_CLONE, LIS_FIFO );
    char	name[48] ;
    int		error;

    MNTSYNC();

    if (!(*f = lis_get_filp(&lis_streams_fops)))
	return(-ENFILE) ;

    sprintf(name, "clone(%d,%d)", LIS_CLONE, LIS_FIFO) ;

    if ((error = lis_new_file_name_dev(*f, name, dev)) == 0)
    {
	if ((error = lis_stropen( (*f)->f_dentry->d_inode, *f )) < 0)
	{
	    fops_put((*f)->f_op);
	    (*f)->f_op->owner = NULL ;
	    fput(*f) ;
	    *f = NULL ;
	}
    }

    MNTSYNC();

    return(error);
}

/*
 *  lis_get_pipe()
 *
 *  create a new unique pipe as two new unique file pointers and all
 *  lower "plumbing" from them (i.e., inodes, stream heads, queues).
 *
 *  the process is simply to create two new unique FIFOs, and "twists"
 *  them.
 */
int lis_get_pipe( struct file **f0, struct file **f1 )
{
    stdata_t *hd0, *hd1;
    queue_t *wq0, *wq1;
    int error;

    /*
     *  get a pair of unique FIFOs for the pipe ends
     */
    if ((error = lis_get_fifo( f0 )) < 0)  goto no_fifos;
    if ((error = lis_get_fifo( f1 )) < 0)  goto no_fifo1;

    /*
     *  OK - make them peers, and get the head queues for twisting
     */
    hd0 = FILE_STR(*f0);  wq0 = hd0->sd_wq;
    hd1 = FILE_STR(*f1);  wq1 = hd1->sd_wq;
    hd0->sd_peer = hd1;
    hd1->sd_peer = hd0;
    lis_head_get(hd0) ;			/* balanced in lis_qdetach */
    lis_head_get(hd1) ;
    if (LIS_DEBUG_OPEN)
	printk("lis_get_pipe: hd0:%s hd1:%s\n", hd0->sd_name, hd1->sd_name) ;

    /*
     *  twist the write queues to point to the peers' read queues
     */
    wq0->q_next = RD(wq1);
    wq1->q_next = RD(wq0);

    return 0;
    
no_fifo1:
    lis_strclose( FILE_INODE(*f0), *f0 );
no_fifos:
    return error;
}

/*
 *  lis_pipe()
 *
 *  this routine is just an intermediate layer in the pipe system call
 *  process.  It's job is to turn the file pointers returned by lis_get_pipe()
 *  into a pair of file descriptors.
 */
int
lis_pipe( unsigned int *fd )
{
    int fd0, fd1;
    struct file *fp0, *fp1;
    int error;

    if ((fd0 = get_unused_fd()) < 0) {
	error = fd0;  goto no_fds;
    }
    if ((fd1 = get_unused_fd()) < 0) {
	error = fd1;  goto no_fd1;
    }

    if ((error = lis_get_pipe( &fp0, &fp1 )) < 0)  goto no_pipe;

    /*
     *  got everything - hook it up
     */
    fd_install( fd0, fp0 );
    fd_install( fd1, fp1 );
    fd[0] = fd0;
    fd[1] = fd1;

    return 0;
    
no_pipe:
    put_unused_fd(fd1);
no_fd1:
    put_unused_fd(fd0);
no_fds:
    return error;
}

/*
 *  pipe() ioctl wrapper (pretty much the same as the syscall)
 */
int
lis_ioc_pipe( unsigned int *fildes )
{
    unsigned int fd[2];
    int error;
    
    error = lis_pipe(fd);
    if (!error) {
	if (copy_to_user(fildes, fd, 2*sizeof(unsigned int)))
	    error = -EFAULT;
    }
    return error;
}

/*
 *  the following ..._sync() routines are adaptations of the kernel
 *  fifo open routines, used here to implement conventional FIFO
 *  synchronization at open/close time (and elsewhere).  We actually
 *  use the same data structures, so it makes sense to model this
 *  code closely after the kernel's code.  Note that this is only
 *  done for FIFOs and pipes, and only actually needed for O_RDONLY
 *  and O_WRONLY for FIFOs, but it's here for sake of completeness...
 *
 *  Note also that this is safe to do.  LiS now uses inodes which
 *  are totally independent of any host filesystem - we can do whatever
 *  we want with them (as long as the kernel doesn't complain).
 *
 *  the preliminary routines here are support for fifo_open_sync.
 */

static inline void lis_fifo_wait(struct inode * i)
{
	DECLARE_WAITQUEUE(wait, current);
	current->state = TASK_INTERRUPTIBLE;
	add_wait_queue(PIPE_WAIT(*i), &wait);
	lis_kernel_up(PIPE_SEM(*i));
	schedule();
	remove_wait_queue(PIPE_WAIT(*i), &wait);
	current->state = TASK_RUNNING;
	lis_kernel_down(PIPE_SEM(*i));
}

static inline void lis_fifo_wait_for_partner( struct inode* i,
					      unsigned int* cnt)
{
    int cur = *cnt;	
    while (cur == *cnt) {
	lis_fifo_wait(i);
	if (signal_pending(current))
	    break;
    }
}

static inline void lis_fifo_wake_up_partner(struct inode* i)
{
    lis_wake_up_interruptible(PIPE_WAIT(*i));
}

static struct inode* lis_fifo_info_new(struct inode* i)
{
    i->i_pipe = kmalloc(sizeof(struct pipe_inode_info), GFP_KERNEL);
    if (i->i_pipe) {
#if 1
	bzero(i->i_pipe, sizeof(struct pipe_inode_info));
#endif
	init_waitqueue_head(PIPE_WAIT(*i));
#if 0
	PIPE_BASE(*i) = NULL;
	PIPE_START(*i) = PIPE_LEN(*i) = 0;
	PIPE_READERS(*i) = PIPE_WRITERS(*i) = 0;
#if !defined(KERNEL_2_5)
	PIPE_WAITING_READERS(*i) = PIPE_WAITING_WRITERS(*i) = 0;
#endif
	PIPE_RCOUNTER(*i) = PIPE_WCOUNTER(*i) = 1;
#endif
	return i;
    } else {
	return NULL;
    }
}

int
lis_fifo_open_sync( struct inode *i, struct file *f )
{
    stdata_t *head = INODE_STR(i);
    long this_open = K_ATOMIC_READ(&lis_open_cnt);
    int ret = 0;

    if (!i || !f) {
	printk("fifo_open_sync(i@0x%p/%d,f@0x%p/%d)#%ld - NULL PARM!\n",
	       i, (i?I_COUNT(i):0),
	       f, (f?F_COUNT(f):0),
	       this_open );
	return(-EINVAL);
    }

    if (LIS_DEBUG_VOPEN)
	printk("lis_fifo_open_sync(i@0x%p/%d,f@0x%p/%d)#%ld"
	       " \"%s\" << mode 0%o flags 0%o\n",
	       i, I_COUNT(i), f, F_COUNT(f),
	       this_open, head ? head->sd_name : "No-Strm",
	       (int)f->f_mode, (int)f->f_flags );


    ret = -ERESTARTSYS;
    if (lis_kernel_down(PIPE_SEM(*i)))
	goto err_nolock_nocleanup;
    
    if (!i->i_pipe) {
	ret = -ENOMEM;
	if(!lis_fifo_info_new(i))
	    goto err_nocleanup;
    }
    f->f_version = 0;

    switch (f->f_mode & (FMODE_READ|FMODE_WRITE)) {
    case FMODE_READ:
	/*
	 *  O_RDONLY
	 *  POSIX.1 says that O_NONBLOCK means return with the FIFO
	 *  opened, even when there is no process writing the FIFO.
	 */
	PIPE_RCOUNTER(*i)++;
	if (PIPE_READERS(*i)++ == 0)
	    lis_fifo_wake_up_partner(i);
	
	if (!PIPE_WRITERS(*i)) {
	    if ((f->f_flags & O_NONBLOCK)) {
		/* suppress POLLHUP until we have seen a writer */
		f->f_version = PIPE_WCOUNTER(*i);
	    } else 
	    {
		lis_fifo_wait_for_partner(i, &PIPE_WCOUNTER(*i));
		if (signal_pending(current))
		    goto err_rd;
	    }
	}
	break;
	
    case FMODE_WRITE:
	/*
	 *  O_WRONLY
	 *  POSIX.1 says that O_NONBLOCK means return -1 with
	 *  errno=ENXIO when there is no process reading the FIFO.
	 */
	ret = -ENXIO;
	if ((f->f_flags & O_NONBLOCK) && !PIPE_READERS(*i))
	    goto err;
	
	PIPE_WCOUNTER(*i)++;
	if (!PIPE_WRITERS(*i)++)
	    lis_fifo_wake_up_partner(i);
	
	if (!PIPE_READERS(*i)) {
	    lis_fifo_wait_for_partner(i, &PIPE_RCOUNTER(*i));
	    if (signal_pending(current))
		goto err_wr;
	}
	break;
	
    case FMODE_READ|FMODE_WRITE:
	/*
	 *  O_RDWR
	 *  POSIX.1 leaves this case "undefined" when O_NONBLOCK is set.
	 *  This implementation will NEVER block on a O_RDWR open, since
	 *  the process can at least talk to itself.
	 */
	PIPE_READERS(*i)++;
	PIPE_WRITERS(*i)++;
	PIPE_RCOUNTER(*i)++;
	PIPE_WCOUNTER(*i)++;
	if (PIPE_READERS(*i) == 1 || PIPE_WRITERS(*i) == 1)
	    lis_fifo_wake_up_partner(i);
	break;
	
    default:
	ret = -EINVAL;
	goto err;
    }

    lis_kernel_up(PIPE_SEM(*i));

    if (LIS_DEBUG_VOPEN)
	printk("lis_fifo_open_sync(i@0x%p/%d,f@0x%p/%d)#%ld"
	       " \"%s\" >> %d reader(s) %d writer(s)\n",
	       i, I_COUNT(i), f, F_COUNT(f),
	       this_open, head ? head->sd_name : "No-Strm",
	       PIPE_READERS(*i), PIPE_WRITERS(*i));

    return 0;
    
err_rd:
    if (!--PIPE_READERS(*i))
	lis_wake_up_interruptible(PIPE_WAIT(*i));
    ret = -ERESTARTSYS;
    goto err;
    
err_wr:
    if (!--PIPE_WRITERS(*i))
	lis_wake_up_interruptible(PIPE_WAIT(*i));
    ret = -ERESTARTSYS;
    goto err;
    
err:
    if (!PIPE_READERS(*i) && !PIPE_WRITERS(*i)) {
	kfree(i->i_pipe);
	i->i_pipe = NULL;
    }

err_nocleanup:
    lis_kernel_up(PIPE_SEM(*i));

err_nolock_nocleanup:
    if (LIS_DEBUG_VOPEN)
	printk("lis_fifo_open_sync(i@0x%p/%d,f@0x%p/%d)#%ld \"%s\""
	       " >> error(%d)\n",
	       i, (i?I_COUNT(i):0),
	       f, (f?F_COUNT(f):0),
	       this_open, head->sd_name, ret) ;

    return ret;
}

void
lis_fifo_close_sync( struct inode *i, struct file *f )
{
    stdata_t *head = INODE_STR(i);
    long this_close = K_ATOMIC_READ(&lis_close_cnt);

    lis_kernel_down(PIPE_SEM(*i));

    PIPE_READERS(*i) -= (f && f->f_mode & FMODE_READ ? 1 : 0);
    PIPE_WRITERS(*i) -= (f && f->f_mode & FMODE_WRITE ? 1 : 0);

    if (LIS_DEBUG_VCLOSE)
	printk("lis_fifo_close_sync(i@0x%p/%d,f@0x%p/%d)#%ld"
	       " \"%s\" >> %d reader(s) %d writer(s)\n",
	       i, I_COUNT(i), f, (f?F_COUNT(f):0),
	       this_close,
	       (head&&head->sd_name?head->sd_name:""),
	       PIPE_READERS(*i), PIPE_WRITERS(*i));

    if (!PIPE_READERS(*i) && !PIPE_WRITERS(*i)) {
	kfree(i->i_pipe);
	i->i_pipe = NULL;
    } else {
	lis_wake_up_interruptible(PIPE_WAIT(*i));
    }

    lis_kernel_up(PIPE_SEM(*i));
}

int
lis_fifo_write_sync( struct inode *i, int written )
{
    if (!PIPE_READERS(*i)) {
	if (!written)
	    send_sig( SIGPIPE, current, 0 );
	return(-EPIPE);
    }
    else
	return 0;
}

static void
lis_fifo_sendfd_sync( struct inode *i, struct file *f )
{
    PIPE_READERS(*i) += (f->f_mode & FMODE_READ ? 1 : 0);
    PIPE_WRITERS(*i) += (f->f_mode & FMODE_WRITE ? 1 : 0);
}

/*
 *  fattach() -
 *
 *  "mount" a stream on a non-STREAM path.  Subsequent opens of the
 *  path will open the mounted stream, until the path is fdetach()'ed.
 *
 *  Note Linux semantics:
 *  - only one mount is done, for the given path, not for all of its
 *    aliases (i.e., hard links).  The reason is that those aliases
 *    can't be efficiently loaded as dentries (if they aren't already)
 *    because the Linux FS org doesn't allow easy access to hard-linked
 *    directory entries.  It _is_ possible to link all such names that
 *    are already in the dcache, but those that are not will be missed.
 *    To have predictable semantics, then, we only mount the given path.
 *  - Permissions are kept at the underlying inodes in the Linux FS.
 *    Since a stream may be mounted on different paths (e.g., to get
 *    around the above limitation, if the user knows the aliases and
 *    calls fattach separately for each), it is not deemed reasonable
 *    to have this routine "initialize" the streams permissions & ids
 *    to those of the underlying file (since there may be many).
 *    Instead (and more simply), we just check here that the caller has
 *    read/write access to the path.
 *  - We allow mounting to any non-directory.  This allows mounting
 *    not only over regular files and streams, but also over Linux FIFOs
 *    and pipes.
 *
 *  2002/11/18 - FIXME - are the above comments still OK?  -JB
 */
int
lis_fattach( struct file *f, const char *path )
{
#if defined(FATTACH_VIA_MOUNT)
    stdata_t *head = FILE_STR(f);
    int result;

    MNTSYNC();

    if (head && head->magic == STDATA_MAGIC) {
	lis_fattach_t *data = lis_fattach_new(f, path);
	
	if (!data)
	    return(-ENOMEM);

	if (LIS_DEBUG_FATTACH)
	    printk("lis_fattach(f@0x%p/%d,\"%s\") << data@0x%p\n",
		   f, F_COUNT(f), path, data );
	
	/*
	 *  We use the sys_mount() syscall now to do an fattach, with
	 *  help from fs_fattach_sb() in the LiS filesystem structure.
	 *  If it finishes its work OK and sys_mount() doesn't complain,
	 *  we will then add the fattach instance to the global list.
	 *
	 *  Note that we pass a reference to data!!!  This ensures that
	 *  its use is effectively "call by reference", since we need
	 *  the stuff under the mount() syscall to set values in data
	 *  that we will use later, and the syscall wrapping stuff would
	 *  otherwise mess things up.
	 *  (This is OK to do, since we're in kernel space already.)
	 */
	result = lis_mount( NULL, (char *)path, LIS_FS_NAME, 0, &data );
	if (result < 0) {
	    putname(data->path);
	    FREE(data);
	} else {
	    lis_fattach_insert(data);  /* OK! - add to fattaches list */

	    if (LIS_DEBUG_FATTACH || LIS_DEBUG_ADDRS || LIS_DEBUG_REFCNTS)
	    {
		printk("lis_fattach(...) data @ 0x%p:\n"
		       "    >> f@0x%p/%d h@0x%p \"%s\" s@0x%p\n",
		       data,
		       data->file, F_COUNT(data->file),
		       data->head, data->path,
		       data->sb) ;
		lis_print_dentry(data->dentry, ">> dentry") ;
	    }
	}
    } else
	result = -EINVAL;  /* f must be a STREAM */

    MNTSYNC();

    return(result);
#else
    static int	twice ;

    if (++twice <= 2)
	printk("\nfattach is no longer implemented in kernels older "
	    "than 2.4.7\n\n") ;
    return(-ENOSYS) ;
#endif			/* FATTACH_VIA_MOUNT */
}

/*
 *  fattach() ioctl wrapper
 *
 *  this is very similar to the syscall wrapper; it differs in the
 *  first parameter (a file * instead of an fd).  It's being used
 *  because lis_check_umem() and lis_copyin_str() don't seem to be
 *  reliable.
 *
 */
int
lis_ioc_fattach( struct file *f, char *path )
{
    return lis_fattach( f, path );
}

/*
 *  fdetach() -
 *
 *  undo an fattach()'s mount.  If the mount reference for the mounted
 *  stream is the last, close the stream.
 *
 *  This routine is written not for the conventional case, where an
 *  fattach mounts a stream on all aliases of a pathname, but to match
 *  our fattach(), which mounts only on the given pathname.  I.e.,
 *  fattach's to links to the pathname, and fattach's to links on
 *  different streams, are not disturbed.
 */
int
lis_fdetach( const char *path )
{
    if (LIS_DEBUG_FATTACH)
	printk("lis_fdetach(\"%s\")\n", path );

#if defined(FATTACH_VIA_MOUNT)
    /*
     *  We now use the sys_umount syscall to do all the work, with the
     *  help of callback entry points in the LiS superblock structure.
     *  If things go OK, lis_super_umount_begin() will undo an fattach
     *  and remove its instance structure from the list; we don't have
     *  to do anything else here.
     *
     *  We use the MNT_DETACH flag to force unmounting if busy; we
     *  don't care about the mount notion of "busy", and we will in
     *  fact want to umount streams that look "busy" to the OS.
     */
#if defined(MNT_DETACH)
    return lis_umount2( (char *)path, MNT_FORCE|MNT_DETACH );
#else
    return lis_umount2( (char *)path, MNT_FORCE);
#endif
# else
    return(-ENOSYS) ;
# endif			/* FATTACH_VIA_MOUNT */
}

/*
 *  lis_fdetach_stream() 
 *
 *  undo any mount(s) (via fattach) on the stream.  The head may be
 *  disposed of in the process, as may a peer stream head if this is
 *  a pipe.
 *
 *  this routine is provided as a means of cleaning up undone fattach
 *  calls.  It should be called when the peer of an fattach'ed pipe end
 *  disappears, but it might also be used before unloading LiS.
 */
void
lis_fdetach_stream( stdata_t *head )
{
#if defined(FATTACH_VIA_MOUNT)
    int n = num_fattaches_listed;  /* just to make sure we terminate */
    int error;

    if (!K_ATOMIC_READ(&head->sd_fattachcnt))
	return;

    if (LIS_DEBUG_FATTACH || LIS_DEBUG_REFCNTS)
	printk("lis_fdetach_stream(h@0x%p/%d/%d): %d fattaches active...\n",
	       head,
	       (head?LIS_SD_REFCNT(head):0),
	       (head?LIS_SD_OPENCNT(head):0),
	       K_ATOMIC_READ(&head->sd_fattachcnt));

    lis_spin_lock(&lis_fattaches_lock);
    while (n-- && !list_empty(&lis_fattaches)) {
	lis_fattach_t *data =
	    list_entry( lis_fattaches.next, lis_fattach_t, list );
	
	if (data->head == head) {
	    lis_spin_unlock(&lis_fattaches_lock);

	    if (LIS_DEBUG_FATTACH || LIS_DEBUG_ADDRS)
	    {
		printk("    fdetaching: "
		       "data 0x%p head@0x%p \"%s\" sb 0x%p\n",
		       data, data->head, data->path, data->sb);
		lis_print_dentry(data->dentry, ">> dentry") ;
	    }

	    if ((error = lis_fdetach(data->path)) < 0) {
		if (LIS_DEBUG_FATTACH)
		    printk("   fdetach 0x%p failed (%d)...\n",
			   data, error);
	    }

	    if (!K_ATOMIC_READ(&head->sd_fattachcnt))
		return;

	    lis_spin_lock(&lis_fattaches_lock);
	}
    }
    lis_spin_unlock(&lis_fattaches_lock);
#endif 	/* FATTACH_VIA_MOUNT */
}

/*
 *  lis_fdetach_all() -
 *
 *  a wrapper for lis_detach(), to abolish all fattach's (or
 *  at least, those for which the caller has the appropriate privilege).
 *
 *  fattaches can be stacked, so we remove the newest ones first.
 */
void
lis_fdetach_all(void)
{
#if defined(FATTACH_VIA_MOUNT)
    int n = num_fattaches_listed;  /* just to make sure we terminate */
    int error;

    if (LIS_DEBUG_FATTACH)
	printk("lis_fdetach_all() << %d fattach(s) active\n",
	       K_ATOMIC_READ(&num_fattaches_listed));

    lis_spin_lock(&lis_fattaches_lock);
    while (n-- && !list_empty(&lis_fattaches)) {
	lis_fattach_t *data =
	    list_entry( lis_fattaches.next, lis_fattach_t, list );
	lis_spin_unlock(&lis_fattaches_lock);

	if (LIS_DEBUG_FATTACH || LIS_DEBUG_ADDRS)
	    printk("    >> fdetaching data 0x%p head 0x%p sb 0x%p d 0x%p...\n",
		   data, data->head, data->sb, data->dentry);

	if ((error = lis_fdetach(data->path)) < 0) {
	    if (LIS_DEBUG_FATTACH)
		printk("    >> fdetach data 0x%p failed (%d)\n",
		       data, error);
	} else
	    if (LIS_DEBUG_FATTACH)
		printk("    fdetached 0x%p OK\n", data);

	lis_spin_lock(&lis_fattaches_lock);
    }
    lis_spin_unlock(&lis_fattaches_lock);

#endif  /* FATTACH_VIA_MOUNT */
}

/*
 *  fdetach() ioctl wrapper
 */
int
lis_ioc_fdetach( char *path )
{
    char *tmp = getname(path);
    int error = PTR_ERR(tmp);

    if (tmp && !IS_ERR(tmp)) {
	if (strcmp( tmp, "*" ) == 0)
	   {
	     lis_fdetach_all();
	     error = -errno;
	   }
	else
	    error = lis_fdetach(path);  /* arg must be in user space */
	putname(tmp);
    }

    return error;
}

#if defined(FATTACH_VIA_MOUNT)
/*
 *  fattach()/fdetach() instance data support routines
 */

/* allocate a new fattach instance */
static lis_fattach_t *lis_fattach_new(struct file *f, const char *path)
{
    lis_fattach_t *data = (lis_fattach_t *) ZALLOC(sizeof(lis_fattach_t));
    char *tmp = (data ? getname(path) : NULL);
    struct nameidata nd;
    int error = 0;

    if (data && tmp && !IS_ERR(tmp)) {
	data->file = f;
	data->head = FILE_STR(f);

	/*
	 *  path may be relative (to current pwd) -
	 *  convert it to absolute before saving it, since fdetach may
	 *  happen from a different process and thus different pwd
	 *
	 *  Note that the path likely lengthens in this process.
	 *  d_path return -ENAMETOOLONG in that case, but it's unlikely
	 *  to happen if PATH_MAX is long, so the handling here (though
	 *  not efficient) is for sake of completeness.  Note we're also
	 *  assuming that PATH_MAX is the size of the buffer getname()
	 *  allocates.
	 */
	error = user_path_walk(path, &nd);
	if (!error) {
	    data->path = d_path(nd.dentry, nd.mnt, tmp, PATH_MAX);
	
	    path_release(&nd);

	    if (IS_ERR(data->path)) {
		/*
		 *  too long? do the getname again, since failing d_path
		 *  will likely have clobbered it.  A relative path is
		 *  better than no path at all.
		 */
		putname(tmp);
		data->path = tmp = getname(path);
	    }
	} else
	    data->path = tmp;  /* better than nothing */

	K_ATOMIC_INC(&num_fattaches_allocd);
    }
    else if (data) {
	FREE(data);  data = NULL;
    }

    if (LIS_DEBUG_FATTACH || LIS_DEBUG_REFCNTS) {
	    printk("lis_fattach_new(f@0x%p/%d,\"%s\")"
		   " => data@0x%p (%d/%d)\n",
		   f, F_COUNT(f), path, data,
		   K_ATOMIC_READ(&num_fattaches_listed),
		   K_ATOMIC_READ(&num_fattaches_allocd) );
    }

    return data;
}

/* delete an fattach instance - assumed not in list */
static void lis_fattach_delete(lis_fattach_t *data)
{
    K_ATOMIC_DEC(&num_fattaches_allocd);

    if (LIS_DEBUG_FATTACH || LIS_DEBUG_REFCNTS) {
	    printk("lis_fattach_delete(%p) (%d/%d)\n",
		   data,
		   K_ATOMIC_READ(&num_fattaches_listed),
		   K_ATOMIC_READ(&num_fattaches_allocd) );
    }

    if (data && data->path)
	putname(data->path);

    if (data)
	FREE(data);
}

/* insert an instance into the list, at the front */
static void lis_fattach_insert(lis_fattach_t *data)
{
    lis_spin_lock(&lis_fattaches_lock);
    list_add(&data->list, &lis_fattaches);
    lis_spin_unlock(&lis_fattaches_lock);

    K_ATOMIC_INC(&num_fattaches_listed);

    if (LIS_DEBUG_FATTACH || LIS_DEBUG_REFCNTS) {
	    printk("lis_fattach_insert(%p) (%d/%d)\n",
		   data,
		   K_ATOMIC_READ(&num_fattaches_listed),
		   K_ATOMIC_READ(&num_fattaches_allocd) );
    }
}

/* remove an fattach instance from the list */
static void lis_fattach_remove(lis_fattach_t *data)
{
    lis_spin_lock(&lis_fattaches_lock);
    list_del(&data->list);
    lis_spin_unlock(&lis_fattaches_lock);

    K_ATOMIC_DEC(&num_fattaches_listed);

    if (LIS_DEBUG_FATTACH || LIS_DEBUG_REFCNTS) {
	    printk("lis_fattach_remove(%p) (%d/%d)\n",
		   data,
		   K_ATOMIC_READ(&num_fattaches_listed),
		   K_ATOMIC_READ(&num_fattaches_allocd) );
    }
}

#endif /* FATTACH_VIA_MOUNT */

/*
 *  lis_sendfd() -
 *
 *  we allow either a file pointer or a file descriptor as input here,
 *  in order that "internal" uses may be supported (e.g., connld).  In
 *  fact, the file pointer supercedes the file descriptor, if both are
 *  provided.
 *
 *  in the case that an fp is given, it is used with the assumption
 *  the the reference count has been bumped appropriately.
 */
int lis_sendfd( stdata_t *sendhd, unsigned int fd, struct file *fp )
{
    stdata_t *recvhd = NULL;
    mblk_t *mp;
    strrecvfd_t *sendfd;
    struct file *oldfp;
    int error;
    lis_flags_t  psw;

    error = -EPIPE;
    if (!sendhd ||
	!(sendhd->magic == STDATA_MAGIC) ||
	!(recvhd = sendhd->sd_peer) ||
	!(recvhd->magic == STDATA_MAGIC) ||
	recvhd == sendhd ||
	F_ISSET(sendhd->sd_flag, STRHUP))
	goto not_fifo;

    error = -ENOSR;
    if (!(mp = allocb( sizeof(strrecvfd_t), BPRI_HI )))  goto no_msg;

    /*
     *  if fp is set, we assume it's reference count is set so that
     *  we won't lose it.  Otherwise, we must get the fp corresponding
     *  to fd.
     */
    if (!fp) {
	/*
	 *  get the file pointer corresponding to fd in the current (i.e.,
	 *  the sender's) process.  We do fget() here to hold the returned
	 *  fp for us.
	 */
	error = -EBADF;
	if (!(fp = fget(fd)))  goto bad_file;
	oldfp = fp;
    } else
	oldfp = fp;

    /*
     *  there's one case where we don't want the fp's count bumped;
     *  if the fp is for the receiving file itself, it can't be
     *  closed if the count is high and the FD isn't received.  But
     *  we can't tell for sure if this will happen, so to avoid it,
     *  we make a copy of the file pointer and pass it instead.  We
     *  also pass the original file pointer as r.fp, so it can be
     *  used if it is the receiving file.
     */
    if (FILE_INODE(fp) == recvhd->sd_inode)  {
	struct file *dfp = lis_get_filp(&lis_streams_fops);
	struct inode *i = FILE_INODE(fp);

	LOCK_INO(i);
	dfp->f_pos   = fp->f_pos;
	dfp->f_flags = fp->f_flags;
	dfp->f_mode  = fp->f_mode;
	error = -ENOMEM;
	if (!(dfp->f_dentry = lis_d_alloc_root(igrab(i),
					       LIS_D_ALLOC_ROOT_NORMAL))) {
	    ULOCK_INO(i);
	    fops_put(dfp->f_op);
	    dfp->f_op->owner = NULL ;
	    fput(dfp);
	    fput(fp);
	    goto bad_file;
	}
	if (FILE_MNT(fp))
	    dfp->f_vfsmnt = MNTGET(FILE_MNT(fp));
	if (F_ISSET(recvhd->sd_flag,STFIFO))
	    lis_fifo_sendfd_sync( i, dfp );
	SET_FILE_STR(dfp, FILE_STR(fp));

	ULOCK_INO(i);
	
	fput(fp);
	fp = dfp;
    }
    
    /*
     *  OK - set up an M_PASSFP message containing a strrecvfd and
     *  put it in the peer's stream head read queue.
     */
    mp->b_datap->db_type = M_PASSFP;
    sendfd = (strrecvfd_t *) mp->b_rptr;
    sendfd->f.fp  = fp;
    sendfd->uid   = current->euid;
    sendfd->gid   = current->egid;
    sendfd->r.fp  = (FILE_INODE(fp) == recvhd->sd_inode ? oldfp : NULL);
    sendfd->r.hd  = recvhd;
    mp->b_wptr = mp->b_rptr + sizeof(strrecvfd_t);
    lis_spin_lock_irqsave(&recvhd->sd_lock, &psw) ;/* lock rcving strm head */
    (recvhd->sd_rfdcnt)++;
    lis_spin_unlock_irqrestore(&recvhd->sd_lock, &psw) ;

    if (LIS_DEBUG_SNDFD || LIS_DEBUG_IOCTL || LIS_DEBUG_REFCNTS) {
	printk("lis_sendfd(...,%d,f@0x%p/%d) from \"%s\" to \"%s\"",
	       fd,
	       oldfp, F_COUNT(oldfp),
	       sendhd->sd_name, recvhd->sd_name);
	if (sendfd->r.fp)
	    printk(" as f@0x%p/%d\n", fp, F_COUNT(fp));
	else
	    printk("\n");
    }

    /*
     *  the following wakes up a receiver if needed
     */
    if (!(error = lis_lockq(recvhd->sd_rq)))
    {
	lis_strrput( recvhd->sd_rq, mp );
	lis_unlockq(recvhd->sd_rq) ;
	return(0) ;
    }
    /* else discard the message and return the error code */

bad_file:
    freemsg(mp);
no_msg:
not_fifo:
    return error;
}

mblk_t *lis_get_passfp(void)
{
    mblk_t *mp;
    lis_spin_lock(&free_passfp.lock);
    mp = free_passfp.head;
    if(free_passfp.head)
	free_passfp.head =  free_passfp.head->b_next;
    if(free_passfp.tail == mp)
	free_passfp.tail = NULL;
    lis_spin_unlock(&free_passfp.lock) ;
    if(mp)
	mp->b_next = NULL;	
    return mp;	
}

/*
 *  the following will be called from flushq() if an M_PASSFP message
 *  is encountered.  This effectively closes the passed file, then
 *  frees the message.
 */
#if defined(KERNEL_2_5)
void lis_tq_free_passfp( unsigned long arg )
#else
void lis_tq_free_passfp( void *arg )
#endif
{
    mblk_t *mp;
    strrecvfd_t *sent;
    int is_a_stream;

    while ((mp = lis_get_passfp()) != NULL)
    {	
	sent = (strrecvfd_t *) mp->b_rptr;

/* FIXME - need a test not dependent on lis_streams_fops */
	is_a_stream = (sent->f.fp->f_op == (&lis_streams_fops));
/**/

	if (LIS_DEBUG_SNDFD || LIS_DEBUG_VCLOSE || LIS_DEBUG_REFCNTS) {
	    struct file *f = sent->f.fp;

	    printk("lis_tq_free_passfp(m@0x%p)"
		   " %s unreceived %sfile @0x%p/%d\n",
		   mp,
		   (sent->r.fp ? "freeing" : "closing"),
		   (is_a_stream ? "STREAMS " : ""),
		   f, (f?F_COUNT(f):0));
	    if (f) {
		struct dentry *d   = f->f_dentry;
		struct inode *i    = FILE_INODE(f);
		struct vfsmount *m = FILE_MNT(f);
		printk("    << d@0x%p/%d%s i@0x%p/%d%s",
		       d, (d?D_COUNT(d):0),
		       (d&&D_IS_LIS(d)?" <LiS>":""),
		       i, (i?I_COUNT(i):0),
		       (i&&I_IS_LIS(i)?" <LiS>":""));
		printk(" m@0x%p/%d", m, (m?MNT_COUNT(m):0));
		printk("\n");
	    }
	}
	if (is_a_stream && sent->r.fp) {
	    fops_put(sent->f.fp->f_op);
	    sent->f.fp->f_op = NULL;  /* (FIXME?) don't call strclose... */
	}
	fput(sent->f.fp);
    	lis_freemsg(mp);
    }	
}

/*
 *  The following will be called from flushq() if an M_PASSFP message
 *  is encountered.  This code puts the message on a queue and defers the 
 *  freeing of the message until later. This is to prevent recursive calls to 
 *  close and bottleneck any other thread. The messages are actually freed from
 *  the function lis_tq_free_passfp.
 */
void lis_free_passfp( mblk_t *mp )
{
#if defined(KERNEL_2_5)
    static DECLARE_TASKLET(lis_tq, lis_tq_free_passfp,0);
#else
    static struct tq_struct	 lis_tq ;
#endif
    int				 emptyq ;
    lis_flags_t 	         psw;
    strrecvfd_t			*sent;
    stdata_t			*recvhd;

    /*
     *  check the message and its contents for validity.  
    */
    if (mp->b_datap->db_type != M_PASSFP)  goto not_passfp;
    if (mp->b_wptr - mp->b_rptr < sizeof(strrecvfd_t))  goto not_passfp;

    sent = (strrecvfd_t *) mp->b_rptr;
    recvhd = sent->r.hd;
    lis_spin_lock_irqsave(&recvhd->sd_lock, &psw) ;/* lock rcving strm head */
    (recvhd->sd_rfdcnt)--;
    lis_spin_unlock_irqrestore(&recvhd->sd_lock, &psw) ;

    lis_spin_lock(&free_passfp.lock);
    mp->b_next = NULL;	
    if ((emptyq = free_passfp.head == NULL))
	free_passfp.head = mp;
    else	
	free_passfp.tail->b_next =  mp;
    free_passfp.tail = mp;
    lis_spin_unlock(&free_passfp.lock) ;

    if (emptyq)
    {
#if defined(KERNEL_2_5)
	tasklet_schedule(&lis_tq) ;
#else
	lis_tq.routine = lis_tq_free_passfp;	/* 2.4 kernel, do it later */
	schedule_task(&lis_tq);
#endif
    }
    return;

not_passfp:
    lis_freemsg(mp);
}

int lis_recvfd( stdata_t *recvhd, strrecvfd_t *recv, struct file *fp )
{
    mblk_t *mp;
    strrecvfd_t *sent;
    int error;
    lis_flags_t  psw;

    lis_bzero( recv, sizeof(strrecvfd_t) );

    error = -EBADF;
    if (!recvhd || !(recvhd->magic == STDATA_MAGIC))
	goto not_stream;

    /*
     *  we expect the caller to have sync'ed with the sender;
     *  we just fail if no message is waiting
     */
    error = -EAGAIN;
    if (!(mp = getq(recvhd->sd_rq)))
	goto no_msg;

    /*
     *  check the message and its contents for validity.
     */
    if (mp->b_datap->db_type != M_PASSFP)
	goto not_passfp;
    lis_spin_lock_irqsave(&recvhd->sd_lock, &psw) ;/* lock rcving strm head */
    (recvhd->sd_rfdcnt)--;
    lis_spin_unlock_irqrestore(&recvhd->sd_lock, &psw) ;
    if (mp->b_wptr - mp->b_rptr < sizeof(strrecvfd_t))
	goto not_passfp;

    error = -ENFILE;
    if ((recv->f.fd = get_unused_fd()) < 0)
	goto no_fds;

    /*
     *  it's a passed FP - hook up the file that was passed to the new FD
     */
    sent = (strrecvfd_t *) mp->b_rptr;
    recv->uid = sent->uid;
    recv->gid = sent->gid;

    if (sent->r.fp && (sent->r.fp == fp)) {
	if (LIS_DEBUG_SNDFD | LIS_DEBUG_IOCTL || LIS_DEBUG_REFCNTS)
	    printk("lis_recvfd(...,f@0x%p/%d) "
		   "S==R, using fp@0x%p/%d, freeing f@0x%p\n",
		   sent->f.fp, (sent->f.fp?F_COUNT(sent->f.fp):0),
		   fp, (fp?F_COUNT(fp):0),
		   sent->f.fp );

	fd_install( recv->f.fd, fp );
	(void) fget(recv->f.fd);
	fops_put(sent->f.fp->f_op);
	sent->f.fp->f_op = NULL;  /* avoid calling strclose */
	if (F_ISSET(recvhd->sd_flag,STFIFO))
	    lis_fifo_close_sync( FILE_INODE(fp), fp );
	fput(sent->f.fp);
#if 0
	if (LIS_DEBUG_IOCTL)
#endif
	    sent->f.fp = fp;
    } else {
	fd_install( recv->f.fd, sent->f.fp );
    }

    if (LIS_DEBUG_SNDFD || LIS_DEBUG_IOCTL || LIS_DEBUG_REFCNTS) {
	printk("lis_recvfd(...,f@0x%p/%d) as fd %d at \"%s\"\n",
	       sent->f.fp, (sent->f.fp?F_COUNT(sent->f.fp):0),
	       recv->f.fd, recvhd->sd_name);
    }

    freemsg(mp);  /* we can release the sent message now */

    return 0;     /* OK */

no_fds:
not_passfp:
    putbqf( recvhd->sd_rq, mp );   /* put the message back */
no_msg:
not_stream:
    return error;
}

/************************************************************************
*                         Atomic Routines                               *
************************************************************************/

void    _RP lis_atomic_set(lis_atomic_t *atomic_addr, int valu)
{
    atomic_set((atomic_t *)atomic_addr, valu) ;
}

int     _RP lis_atomic_read(lis_atomic_t *atomic_addr)
{
    return(atomic_read(((atomic_t *)atomic_addr))) ;
}

void    _RP lis_atomic_add(lis_atomic_t *atomic_addr, int amt)
{
    atomic_add((amt),((atomic_t *)atomic_addr)) ;
}

void    _RP lis_atomic_sub(lis_atomic_t *atomic_addr, int amt)
{
    atomic_sub((amt),((atomic_t *)atomic_addr)) ;
}

void    _RP lis_atomic_inc(lis_atomic_t *atomic_addr)
{
    atomic_inc(((atomic_t *)atomic_addr)) ;
}

void    _RP lis_atomic_dec(lis_atomic_t *atomic_addr)
{
    atomic_dec(((atomic_t *)atomic_addr)) ;
}

int     _RP lis_atomic_dec_and_test(lis_atomic_t *atomic_addr)
{
    return(atomic_dec_and_test(((atomic_t *)atomic_addr))) ;
}

/************************************************************************
*                       lis_in_interrupt                                *
*************************************************************************
*									*
* Returns true if the kernel is at interrupt level or holding spin	*
* locks (2.6).  Basically, if this returns true then you need to make	*
* memory allocator calls using the "atomic" rather then "kernel" forms	*
* of the routines.							*
*									*
************************************************************************/
int      _RP lis_in_interrupt(void)
{
#if defined(KERNEL_2_5)
    return(in_atomic() || irqs_disabled()) ;
#else
    return(in_interrupt()) ;
#endif
}

/************************************************************************
*                         Kernel Semaphores                             *
*************************************************************************
*									*
* These routines are used with doing a down/up on a kernel semaphore.	*
* lis_down/up are used for LiS type semaphores.  Kernel semaphores 	*
* occur in kernel structures, such as inodes.				*
*									*
************************************************************************/
int _RP lis_kernel_down(struct semaphore *sem)
{
    return(down_interruptible(sem)) ;
}

void _RP lis_kernel_up(struct semaphore *sem)
{
    up(sem) ;
}

/************************************************************************
*                         User Space Access                             *
*************************************************************************
*									*
* copy to/from user space.						*
*									*
************************************************************************/

int	lis_copyin(struct file *fp, void *kbuf, const void *ubuf, int len)
{
    return(copy_from_user(kbuf,ubuf,len)?-EFAULT:0) ;

}

int	lis_copyout(struct file *fp, const void *kbuf, void *ubuf, int len)
{
    return(copy_to_user  (ubuf,kbuf,len)?-EFAULT:0) ;
}

int lis_check_umem(struct file *fp, int rd_wr_fcn,
		   const void *usr_addr, int lgth)
{
#if HAVE_KMACRO_ACCESS_OK
    return(access_ok(rd_wr_fcn,usr_addr,lgth) ? 0 : -EFAULT);
#else
    return(verify_area(rd_wr_fcn,usr_addr,lgth)) ;
#endif
}

/************************************************************************
*                         lis_gettimeofday                              *
*************************************************************************
*									*
* A slightly slower version of the kernel routine.			*
*									*
************************************************************************/
void _RP lis_gettimeofday(struct timeval *tv)
{
    return(do_gettimeofday(tv));
}



/*  -------------------------------------------------------------------  */
/*				    Module                               */
#ifdef LINUX


/*
 *  lis_loadable_load - load a loadable module.
 *
 *  This function may sleep. On failure it may return a negative errno, or
 *  it may return 0. The module should have been loaded on return from this
 *  function, and the caller should check for this.
 */
int lis_loadable_load(const char *name)
{
#ifdef LIS_LOADABLE_SUPPORT
	return request_module(name);
#else
	printk("lis_loadable_load: %s: "
		"kernel not compiled for dynamic module loading\n",
		name) ;
	return(-ENOSYS) ;
#endif
}


#ifdef _S390X_LIS_
int lis_ioctl32_str (unsigned int fd, unsigned int cmd, unsigned long arg)
{
  strioctl_t par64;
  strioctl32_t par32;
  strioctl32_t * ptr32;
  char * data32p;
  mm_segment_t old_fs;
  char * datap = NULL;
  int rc;

  ptr32 = (strioctl32_t*)arg;
  if (copy_from_user((void*)&par32,(void*)ptr32,sizeof(strioctl32_t)))
  {
    printk("Unable to get parameter block for 32 bit ioctl I_STR (length %d)\n",
           sizeof(strioctl32_t));
    rc = -EFAULT;
    goto ioctl32_end;
  }
  par64.ic_cmd    = par32.ic_cmd;
  par64.ic_timout = par32.ic_timout;
  par64.ic_len    = par32.ic_len;
  data32p = (char*)par32.ic_dp;
  if (par64.ic_len > 0)
  {
    datap = ALLOCF(par64.ic_len,"ioctl32 ");
    if (datap == NULL)
    {
      printk("Unable to get memory to convert 32 bit ioctl I_STR (length %d)\n",
             par64.ic_len);
      rc = -ENOSR;
      goto ioctl32_end;
    }
    if (copy_from_user((void*)datap,(void*)data32p,par64.ic_len))
    {
      printk("Unable to get data block for 32 bit ioctl I_STR (length %d)\n",
             par64.ic_len);
      rc = -EFAULT;
      goto ioctl32_end;
    }
    par64.ic_dp = datap;
  }
  else
  {
    par64.ic_dp = NULL;
  }
  old_fs = get_fs();
  set_fs(KERNEL_DS);
  rc = sys_ioctl(fd,cmd,(unsigned long)&par64);
  set_fs(old_fs);
 
  if (copy_to_user((void*)&(ptr32->ic_cmd),(void*)&(par64.ic_cmd),sizeof(int)))
  {
    printk("Unable to return command parameter for 32 bit ioctl I_STR\n");
    rc = -EFAULT;
    goto ioctl32_end;
  }
  if (copy_to_user((void*)&(ptr32->ic_timout),(void*)&(par64.ic_timout),
                   sizeof(int)))
  {
    printk("Unable to return timeout parameter for 32 bit ioctl I_STR\n");
    rc = -EFAULT;
    goto ioctl32_end;
  }
  if (copy_to_user((void*)&(ptr32->ic_len),(void*)&(par64.ic_len),sizeof(int)))
  {
    printk("Unable to return length parameter for 32 bit ioctl I_STR\n");
    rc = -EFAULT;
    goto ioctl32_end;
  }
  if ((par64.ic_len > 0) && (data32p != NULL) && (datap != NULL))
  {
    if (copy_to_user((void*)data32p,(void*)datap,par64.ic_len))
    {
      printk("Unable to return data block for 32 bit ioctl I_STR (length %d)\n",
             par64.ic_len);
      rc = -EFAULT;
      goto ioctl32_end;
    }
  }

ioctl32_end:
  if (datap != NULL)
  {
    FREE(datap);
  }
 
  return(rc);
}
#endif

#ifdef LIS_ATOMIC_STATS
static struct proc_dir_entry *lis_proc_file = NULL;

static int lis_write_qstats(char *buf, struct module_stat *ms)
{
	return sprintf(buf,
		       "% 8d% 8d% 8d% 8d% 8d\n",
		       lis_atomic_read(&ms->ms_pcnt),
		       lis_atomic_read(&ms->ms_scnt),
		       lis_atomic_read(&ms->ms_ocnt),
		       lis_atomic_read(&ms->ms_ccnt),
		       lis_atomic_read(&ms->ms_acnt));
}

static int lis_write_mstats(char *buf, struct streamtab *st, int rq)
{
	struct qinit *qi = (rq ? st->st_rdinit : st->st_wrinit);

	if(!qi || !qi->qi_mstat)
		return sprintf(buf, "\n");
	else
		return lis_write_qstats(buf, qi->qi_mstat);
}

static int lis_proc_read(char *page, char **start, off_t off, int count,
			 int *eof, void *data)
{
	extern struct streamtab strmhd_info;

	int len = 0, i;

	len += sprintf(page+len, "%18s: %8s %8s %8s %8s %8s\n",
		       "driver/module", "#put", "#srvc", "#open", "#close", "#admin");
	len += sprintf(page+len, "%15s.rq:", "HEAD");
	len += lis_write_mstats(page+len, &strmhd_info, 1);
	len += sprintf(page+len, "%15s.wq:", "HEAD");
	len += lis_write_mstats(page+len, &strmhd_info, 0);
	for(i = 0; i < MAX_STRDEV; i++) {
		struct streamtab *st = lis_fstr_sw[i].f_str;
		if(st == NULL)
			continue;
		len += sprintf(page+len, "(d)%12s.rq:", lis_fstr_sw[i].f_name);
		len += lis_write_mstats(page+len, st, 1);
		len += sprintf(page+len, "(d)%12s.wq:", lis_fstr_sw[i].f_name);
		len += lis_write_mstats(page+len, st, 0);
	}
	for(i = 0; i < MAX_STRMOD; i++) {
		struct streamtab *st = lis_fmod_sw[i].f_str;
		if(st == NULL)
			continue;
		len += sprintf(page+len, "(m)%12s.rq:", lis_fmod_sw[i].f_name);
		len += lis_write_mstats(page+len, st, 1);
		len += sprintf(page+len, "(m)%12s.wq:", lis_fmod_sw[i].f_name);
		len += lis_write_mstats(page+len, st, 0);
	}
	return len;
}

#endif /* LIS_ATOMIC_STATS */

int lis_init_module( void )
{
    extern char	*lis_poll_file ;
    extern void  lis_mem_init(void) ;

    printk(
	"==================================================================\n"
	"Linux STREAMS Subsystem loading...\n");

    /*
     * FIXME: single threading only support 0 & 1 value at this time.
     * Any non-1 value is trimmed down to 0, to trigger the usual LiS
     * behavior
     */
    lis_nthreads = (lis_nthreads == 1);

    current->fs->umask = 0 ;		/* can set any permissions */

    lis_mem_init() ;			/* in lismem.c */
    lis_major = register_chrdev(0,"streams",&lis_streams_fops);
    if	(lis_major < 0)
    {
	printk("Unable to register STREAMS Subsystem\n");
	return -EIO;
    }
    /* Initialize every global variable to a default value, if there're
     * modules w/ not-exported globals we should create init_() functions for
     * them and call them from here. */

    lis_spl_init() ;
    lis_spin_lock_init(&lis_setqsched_lock, "SetQsched-Lock") ;
    lis_spin_lock_init(&lis_task_lock, "Task-Lock") ;
    lis_spin_lock_init(&free_passfp.lock,"Free Passfp Lock");
#if defined(FATTACH_VIA_MOUNT)
    lis_spin_lock_init(&lis_fattaches_lock, "fattach list lock");
#endif
    lis_init_head();

    {
	int	 err ;

	err = register_filesystem(&lis_file_system_ops) ;
	if (err == 0)
	{
	    lis_mnt = kern_mount(&lis_file_system_ops) ;
	    err = PTR_ERR(lis_mnt) ;
	    if (IS_ERR(lis_mnt))
		unregister_filesystem(&lis_file_system_ops) ;
	    else {
#if defined(MODULE)
		lis_file_system_ops.owner = THIS_MODULE;
#endif
		lis_mnt_init_cnt = MNT_COUNT(lis_mnt);
		MNTSYNC();
		err = 0 ;
	    }
	}

	if (err != 0)
	{
	    printk(
	     "Linux Streams Subsystem failed to register its file system (%d).\n",
	     err) ;
	    return(err) ;
	}
    }

    lis_start_qsched() ;		/* ensure q running process going */

#ifdef _S390X_LIS_
    register_ioctl32_conversion(I_SETSIG,sys_ioctl);
    register_ioctl32_conversion(I_SRDOPT,sys_ioctl);
    register_ioctl32_conversion(I_PUSH,sys_ioctl);
    register_ioctl32_conversion(I_LINK,sys_ioctl);
    register_ioctl32_conversion(I_UNLINK,sys_ioctl);
 
    register_ioctl32_conversion(I_STR,lis_ioctl32_str);
#endif
#ifdef LIS_ATOMIC_STATS
    lis_proc_file = create_proc_read_entry("LiS", 0444, NULL,
					   lis_proc_read, NULL);

#endif

    printk(
	"Linux STREAMS Subsystem ready.\n"
	"Copyright (c) 1997-2004 David Grothe, et al, http://www.gcom.com\n"
	"Major device number %d.\n"
	"Version %s %s. Compiled for kernel version %s.\n"
	"Using %s %s\n"
	"==================================================================\n",
	lis_major, lis_version, lis_date, lis_kernel_version,
	lis_poll_file, lis_stropts_file);

    return(0);
}

#ifdef MODULE			/* for loadable module support */

/*
 * Magic named routine called by kernel module support code.
 */
#ifdef KERNEL_2_5
int _lis_init_module( void )
#else
int init_module( void )
#endif
{
    return(lis_init_module()) ;
}

/*
 * Magic named routine called by kernel module support code.
 */
#ifdef KERNEL_2_5
void _lis_cleanup_module( void )
#else
void cleanup_module( void )
#endif
{
   extern void	lis_kill_qsched(void) ;
   extern void	lis_mem_terminate(void) ;
   extern void  lis_terminate_final(void) ;

#if defined(FATTACH_VIA_MOUNT)
   /*
    *  It never made sense to do this here before, but it does now,
    *  as of 2.4.x (using mount() for fattach()), because the fdetach()
    *  process via mount can be left partially undone, e.g., if
    *  umount is used directly, or if an fattach'ed STREAM is "busy"
    *  when the fdetach is first tried (an unreceived PASSFP message,
    *  for example).  In such cases, the mount portion of the fdetach
    *  may have finished, so that the OS doesn't think there's any
    *  reason not to allow STREAMS to unload, but the fattach list
    *  may not be empty, and may reflect busy dentries, inodes, memory,
    *  and such.  So we try here to finish cleaning up after partially
    *  undone fattaches...
    */
   lis_fdetach_all();
#endif

#ifdef LIS_ATOMIC_STATS
   if(lis_proc_file != NULL)
	   remove_proc_entry("LiS", NULL);
#endif

   /*
    * Make sure no streams modules are running,
    * and de-register devices unregister_netdev (dev);
    */

    lis_kill_qsched() ;			/* drivers/str/runq.c */
    lis_terminate_head();

#if (!defined(_S390_LIS_) && !defined(_S390X_LIS_) && !defined(_HPPA_LIS_))
    {
        extern void	lis_pci_cleanup(void) ;
        lis_pci_cleanup() ;
    } 
#endif          /* S390 or S390X */

    unregister_chrdev(lis_major,"streams");
    MNTSYNC();
    if (lis_mnt && MNT_COUNT(lis_mnt) > lis_mnt_init_cnt) {
	printk("LiS mount count is %d, should be %d\n",
	       MNT_COUNT(lis_mnt), lis_mnt_init_cnt);
    }

    if (lis_mnt != NULL)
	MNTPUT(lis_mnt) ;

    unregister_filesystem(&lis_file_system_ops) ;
    {
	int	n ;

	if ((n = K_ATOMIC_READ(&lis_inode_cnt)) != 0)
	    printk("LiS inode count is %d, should be 0\n", n) ;
    }

#ifdef KERNEL_2_5
    {
	void lis_free_devid_list(void) ;	/* in osif.c */
	lis_free_devid_list() ;
    }
#endif
    lis_terminate_final() ;		/* LiS internal memory */
    lis_mem_terminate() ;		/* LiS use of slab allocator */

#ifdef _S390X_LIS_
    unregister_ioctl32_conversion(I_SETSIG);
    unregister_ioctl32_conversion(I_SRDOPT);
    unregister_ioctl32_conversion(I_PUSH);
    unregister_ioctl32_conversion(I_LINK);
    unregister_ioctl32_conversion(I_UNLINK);
 
    unregister_ioctl32_conversion(I_STR);
#endif

    printk ("Linux STREAMS Subsystem removed\n");
}

#ifdef KERNEL_2_5
module_init(_lis_init_module) ;
module_exit(_lis_cleanup_module) ;
#endif

#endif


/************************************************************************
*                          streams_init                                 *
*************************************************************************
*									*
* This routine is called from the Linux main.c.  It registers the 	*
* streams driver and initializes some streams variables.		*
*									*
* No memory is allocated.						*
*									*
************************************************************************/
void	streams_init(void)
{

    lis_init_module() ;		/* register STREAMS with Linux */

} /* streams_init */

#endif /* LINUX */

/************************************************************************
*                         Thread Startup                                *
*************************************************************************
*									*
* This routine can be called from other drivers, as well as from LiS	*
* internally, to start up a kernel thread.  It takes care of the	*
* business of shedding user mapped pages and such details.  The idea is	*
* to be able to keep the low-level kernel interactions out of the user's*
* thread code.								*
*									*
* The prototype for this resides in linux-mdep.h and will be automatic-	*
* ally included when you include <sys/stream.h>.			*
*									*
************************************************************************/

/*
 * Some defines for manipulating signals.
 */
#define	MY_SIGS		current->pending.signal
#define	MY_SIG		MY_SIGS.sig[0]
#define	MY_BLKS		current->blocked
#define	MY_BLKD		MY_BLKS.sig[0]

/*
 * This is the function that we start up.  It sheds user memory 
 * and calls the user's thread function.  When the user's function
 * returns, it exits.
 *
 * By default these threads run at normal priority.  LiS's own queue runner
 * thread ups its priority to that of a real-time process.
 */
typedef struct
{
    char	 name[sizeof(current->comm)] ;
    int        (*func)(void *func_arg) ;
    void	*func_arg ;

} arg_t ;

int	lis_thread_func(void *argp)
{
    arg_t		*arg = (arg_t *) argp ;
    int		       (*func)(void *) ;
    void		*func_arg ;

#if defined(KERNEL_2_5)
    daemonize("%s", arg->name) ;	/* make me a daemon */
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,16)
    reparent_to_init() ;		/* disown all parentage */
#else
    daemonize() ;			/* make me a daemon */
#endif

    current->uid = 0 ;			/* become root */
    current->euid = 0 ;			/* become root */
#if !defined(KERNEL_2_5)
    strcpy(current->comm, arg->name) ;
#endif

    func = arg->func ;
    func_arg = arg->func_arg ;
    FREE(argp) ;			/* don't need args anymore */

    return(func(func_arg)) ;		/* enter caller's function */
    					/* without holding big kernel lock */
}

/*
 * Start the thread with "fcn(arg)" as the entry point.  Return the pid for the
 * new process, or < 0 for error.
 */
pid_t	_RP lis_thread_start(int (*fcn)(void *), void *arg, const char *name)
{
    arg_t	*argp ;

    argp = ALLOCF(sizeof(*argp), "Thread ") ;
    if (argp == NULL)
	return(-ENOMEM) ;

    if (name != NULL && name[0] != 0)
	strncpy(argp->name, name, sizeof(argp->name)) ;
    else
	strcpy(argp->name, "LiS-Thread") ;

    argp->func = fcn ;
    argp->func_arg = arg ;

    return(kernel_thread(lis_thread_func, (void *) argp, 0)) ;
}

int _RP
lis_thread_stop(pid_t pid)
{
    return(kill_proc(pid, SIGTERM, 1));
}


/************************************************************************
*                       Queue Running                                   *
*************************************************************************
*									*
* The following routines are used to manage running the STREAMS queues.	*
*									*
* The routines lis_setqsched and lis_kill_qsched are called from outside*
* this module.  lis_setqsched is a request to run the queues.  It means	*
* "now would be a good time to _schedule_ the queues to be run".	*
* list_kill_qsched is called from module unload to kill the kernel	*
* thread.  There is a semaphore interlock on this process to make sure	*
* that the process has been killed before unloading the module.		*
*									*
************************************************************************/


/*
 * This is the entry point of a kernel thread
 *
 * It appears that when this thread starts up it no longer holds the "big
 * kernel lock" (on 2.2 kernels).  So the first order of business is to acquire
 * that lock, sort of like we got here as the result of a system call.  Running
 * the queues, and calling kernel routines, without holding that lock is
 * troublesome for the 2.2 kernel.
 */
int	lis_thread_runqueues(void *p)
{
    intptr_t		 cpu_id = (intptr_t) p ;
    int			 sig_cnt  = 0 ;
    unsigned long	 seconds = 0 ;
    lis_semaphore_t	*semp = &lis_runq_sems[cpu_id] ;
    extern char		 lis_print_buffer[] ;
    extern char		*lis_nxt_print_ptr ;

    printk(KERN_INFO "LiS-RunQ-%s running on CPU %d pid=%d\n",
	    lis_version, cpu_id, current->pid) ;

    current->fs->umask = 0 ;		/* can set any permissions */
    current->policy = SCHED_FIFO ;	/* real-time: run when ready */
    current->rt_priority = 50 ;		/* middle value real-time priority */
    sigfillset(&MY_BLKS) ;		/* block all signals */
    sigdelset(&MY_BLKS, SIGKILL) ;	/* enable KILL */
#if CONFIG_SMP
#if defined(SET_CPUS_ALLOWED)
# if defined(KERNEL_2_5)
    set_cpus_allowed(current, cpumask_of_cpu(cpu_id)) ;
# else
    set_cpus_allowed(current, 1 << cpu_id) ;
# endif
#else
    current->cpus_allowed = (1 << cpu_id) ;	/* older 2.4 kernels */
#endif
#endif

#if defined(KERNEL_2_5)
    yield() ;				/* reschedule our thread */
    					/* we will wake up on proper CPU */
#else
    schedule() ;			/* maybe this will do the same */
#endif
    for (;;)
    {
	lis_run_queues(cpu_id) ;	/* run the STREAMS queues */

	/*
	 * When rebooting the kernel sends a TERM signal to all processes
	 * and then a KILL signal.  We want to keep running after the
	 * TERM so that we can close streams from user processes.  We
	 * will go away from a KILL or INT signal so that we can also
	 * be easily killed from the keyboard.
	 */
	if (lis_down(semp) < 0)			/* sleep */
	{					/* interrupted */
	    if (   signal_pending(current)	/* killed */
		&& sigismember(&MY_SIGS, SIGKILL)
	       )
		break ;				/* process killed */

	    if (++sig_cnt >= 5)			/* nothing but signals */
	    {
		printk("%s: Signalled: signal=0x%lx blocked=0x%lx, exiting\n",
			current->comm, MY_SIG, MY_BLKD) ;
		sig_cnt = 0 ;
		break ;
	    }
	}
	else
	    sig_cnt = 0 ;

	lis_runq_wakeups[cpu_id]++ ;
	if (cpu_id != smp_processor_id())
	{
	    static int	msg_cnt ;
	    char	buf[200] ;

#if HAVE_KFUNC_CPUMASK_SCNPRINTF || HAVE_KMACRO_CPUMASK_SCNPRINTF
	    cpumask_scnprintf(buf, sizeof(buf), current->cpus_allowed) ;
#else
	    sprintf(buf, "0x%lx", current->cpus_allowed) ;
#endif
	    if (++msg_cnt < 5)
		printk("%s woke up running on CPU %d -- cpu_id=%d mask=%s\n",
			current->comm, smp_processor_id(), cpu_id, buf) ;
	}
	/*
	 * If there are characters queued up in need of printing, print them if
	 * some time has elapsed.
	 */
	if (cpu_id == 0 && lis_nxt_print_ptr != lis_print_buffer)
	{
	    if (seconds == 0)			/* not timing yet */
		seconds = lis_secs() ;		/* start timing */
	    else
	    if (lis_secs() - seconds > 1)	/* has enough time gone by? */
	    {
		lis_flush_print_buffer() ;
		seconds = 0 ;			/* stop timing */
	    }
	}
    }

    printk(KERN_INFO "%s exiting pid=%d\n", current->comm, current->pid) ;
    lis_sem_destroy(&lis_runq_sems[cpu_id]) ;	/* de-init semaphore */
    lis_runq_pids[cpu_id] = 0 ;			/* process gone */
    lis_up(&lis_runq_kill_sems[cpu_id]) ;	/* OK, we're killed */

    return(0) ;
}

/*
 * Called to start the queue running process.
 */
void	lis_start_qsched(void)
{
    int		cpu ;
    int		ncpus ;
    char	name[16] ;

    ncpus = lis_nthreads? 1:NUM_CPUS ;
    lis_num_cpus = ncpus ;
    for (cpu = 0; cpu < ncpus; cpu++)
    {
	if (lis_runq_pids[cpu] > 0)		/* already running */
	    continue ;

	lis_sem_init(&lis_runq_sems[cpu], 0) ;	/* initialize semaphore */
	lis_sem_init(&lis_runq_kill_sems[cpu], 0) ;/* initialize semaphore */

	sprintf(name, "LiS-%s:%u", lis_version, cpu) ;
	lis_runq_pids[cpu] = lis_thread_start(lis_thread_runqueues,
					      (void *)(long)cpu, /* __LP64__ */
					      name) ;
	if (lis_runq_pids[cpu] < 0)		/* failed to fork */
	{
	    printk("lis_start_qsched: %s: lis_thread_start error %d\n",
		    name, lis_runq_pids[cpu]);
	    lis_sem_destroy(&lis_runq_sems[cpu]) ;
	    lis_sem_destroy(&lis_runq_kill_sems[cpu]) ;
	    continue ;
	}

	K_ATOMIC_INC(&lis_runq_cnt) ;		/* one more running */
    }
}

/*
 * The "can_call" parameter is true if it is OK to just call the queue runner
 * from this routine.  It is false if it is necessary to defer the queue
 * running until later.  Typically qenable() passes 0 and others pass 1.
 */
void	lis_setqsched(int can_call)		/* kernel thread style */
{
    int		queues_running ;
    int		queue_load_thresh ;
    int		req_cnt ;
    int		in_intr = in_interrupt() ;
    int		my_cpu = smp_processor_id() ;
    int		cpu ;
    static int	qsched_running ;
    lis_flags_t psw;

#define WORK_INCR	13

    lis_spin_lock_irqsave(&lis_setqsched_lock, &psw) ;

    lis_setqsched_cnts[my_cpu]++ ;		/* keep statistics */
    if (in_intr)
	lis_setqsched_isr_cnts[my_cpu]++ ;	/* keep statistics */

    if (qsched_running)
    {						/* one of these is enough */
	lis_spin_unlock_irqrestore(&lis_setqsched_lock, &psw) ;
	return ;
    }

    qsched_running = 1 ;
    lis_spin_unlock_irqrestore(&lis_setqsched_lock, &psw) ;

    if (K_ATOMIC_READ(&lis_runq_cnt) == 0)
    {					/* no threads running */
	static int	cnt ;

	if (cnt++ < 5)
	    printk("lis_setqsched: No qrun process\n");

	goto return_point ;
    }

    /*
     * We keep waking up queue runner threads until they are all running.  We
     * don't want more queue runners actually running than there are queues to
     * be run.
     *
     * lis_runq_cnt is the number of threads that we have started.
     *
     * lis_queues_running is the number of them that are awake and actively
     * processing streams queues.
     *
     * lis_runq_req_cnt is the number of items that are queued for deferred
     * execution by the "queuerun" function.  These are enabled queues, scan
     * queue elements and buffcall callouts pending.  As a heuristic, do not
     * wake up a second queue runner thread until there are more than 4 things
     * queued.  The theory is that it takes some time to wake up and schedule
     * another thread, so there should be enough work queued to make it worth
     * doing.
     *
     * Queueing theory suggests that between 90-95% utilization the average
     * queue length is expected to be between 9 and 19.  With 80% utilization
     * you would expect to see 12 or more items queued 5% of the time.
     * With 70% utilization you would expect to see 12 or more items queued 1%
     * of the time.
     *
     * The value of 13 was determined experimentally as seeming to keep the
     * queue runners busy on a CPU or two in a high speed data passing test.
     * There is some efficiency to be gained by not having the queue runner
     * threads sleeping/waking up frequently.
     *
     * Once we decide to wake up threads, we do so until we have enough threads
     * running to cover the items queued at 13 items per cpu, or until we have
     * awakened the queue runner threads on all available cpus.  This is
     * something of a heuristic so we don't care that some of these numbers
     * may be changing on other cpus while we execute this loop.
     *
     * Because the threads are running at real-time priority we will likely be
     * preempted as soon as we wake up the thread that is bound to the cpu that
     * we are running on.  This routine is always called from a context in
     * which the caller anticipates preemption, so we need not worry about
     * this.
     */

    queues_running = K_ATOMIC_READ(&lis_queues_running) ;
    queue_load_thresh = WORK_INCR * queues_running ;
    req_cnt = K_ATOMIC_READ(&lis_runq_req_cnt) ;

    /*
     * If the number of outstanding requests does not cross the upper
     * threhold for needing more queue run threads running, or if they
     * are all running already, just return.  There is nothing that we
     * can do to improve things.
     */
    if (   req_cnt <= queue_load_thresh
	|| queues_running == K_ATOMIC_READ(&lis_runq_cnt)
       )
	goto return_point ;

    /*
     * We could benefit by having another queue run thread wake up
     * and help with the queue processing.  Don't wake up my cpu
     * until after loop completion since we don't want this loop
     * to be preempted.
     */
    for (cpu = 0; cpu < K_ATOMIC_READ(&lis_runq_cnt); cpu++)
    {					/* find a sleeping thread */
	if (   cpu != my_cpu		/* don't start on my cpu yet */
	    && !K_ATOMIC_READ(&lis_runq_active_flags[cpu]))
	{					/* sleeping thread */
	    lis_up(&lis_runq_sems[cpu]) ;	/* wake up a thread */
	    queue_load_thresh += WORK_INCR ;	/* increase threshold */
	    if (K_ATOMIC_READ(&lis_runq_req_cnt) <= queue_load_thresh)
		goto return_point ;		/* enough threads running */
	}
    }

    /*
     * If we get to here we still need another queue runner.  See if
     * my_cpu is available.
     */
    if (!K_ATOMIC_READ(&lis_runq_active_flags[my_cpu]))
	lis_up(&lis_runq_sems[my_cpu]) ;	/* wake up thread on my cpu */

return_point:
    qsched_running = 0 ;

} /* lis_setqsched */

/*
 * Called from head code when time to unload LiS module.
 */
void	lis_kill_qsched(void)
{
    int		cpu ;

    for (cpu = 0; cpu < lis_num_cpus; cpu++)
    {
	if (lis_runq_pids[cpu] > 0) /* Only stop the kernel thread if running */
	{
	    kill_proc(lis_runq_pids[cpu], SIGKILL, 1) ;
	    lis_down(&lis_runq_kill_sems[cpu]) ;
	    K_ATOMIC_DEC(&lis_runq_cnt) ;		/* one fewer running */
	    lis_sem_destroy(&lis_runq_kill_sems[cpu]) ;	/* de-init semaphore */
	}
    }
}


/************************************************************************
*                        Process Kill                                   *
************************************************************************/

int      lis_kill_proc(int pid, int sig, int priv)
{
    return(kill_proc(pid, sig, priv)) ;
}
int      lis_kill_pg (int pgrp, int sig, int priv)
{
    return(kill_pg(pgrp, sig, priv)) ;
}

/************************************************************************
*                        Signal Sets                                    *
*************************************************************************
*									*
* We can save and restore signals from the current task structure in	*
* an stdata structure.  We need to do this when closing a stream.  The	*
* close procedure needs to do some semaphore waits, but if the stream	*
* is closing because the process was killed (signalled) then the 	*
* semaphore waits will all fail with EINTR.				*
*									*
************************************************************************/

#if defined(SIGMASKLOCK)
#define LOCK_MASK    spin_lock_irq(&current->sigmask_lock)
#define UNLOCK_MASK    recalc_sigpending(current); \
		       spin_unlock_irq(&current->sigmask_lock)
#else
#define LOCK_MASK    spin_lock_irq(&current->sighand->siglock)
#define UNLOCK_MASK    recalc_sigpending(); \
		       spin_unlock_irq(&current->sighand->siglock)
#endif

void lis_clear_and_save_sigs(stdata_t *hd)
{
    sigset_t	*hd_sigs = (sigset_t *) hd->sd_save_sigs ;

    LOCK_MASK ;
    *hd_sigs = current->blocked ;
    sigfillset(&current->blocked) ;		/* block all signals */
    UNLOCK_MASK ;
}

void lis_restore_sigs(stdata_t *hd)
{
    sigset_t	*hd_sigs = (sigset_t *) hd->sd_save_sigs ;

    LOCK_MASK ;
    current->blocked = *hd_sigs ;
    UNLOCK_MASK ;
}

/************************************************************************
*                      Credentials Manipulation                         *
*************************************************************************
*									*
* These routines copy credentials to/from the task structure.		*
*									*
* This is only needed in a single CPU environment in which we run the	*
* queue schedule on top of a call from users.  In that case we need to	*
* assume kernel credentials while running the queues, so these routines	*
* need to actually do something.					*
*									*
* The lock here is high contention, so we avoid doing this work in a	*
* multi-cpu environment.						*
*									*
************************************************************************/
void	lis_task_to_creds(lis_kcreds_t *cp)
{
#if 0
    int		i ;
    lis_flags_t psw;

    lis_spin_lock_irqsave(&lis_task_lock, &psw) ;
    cp->uid = current->uid ;
    cp->euid = current->euid ;
    cp->suid = current->suid ;
    cp->fsuid = current->fsuid ;
    cp->gid = current->gid ;
    cp->egid = current->egid ;
    cp->sgid = current->sgid ;
    cp->fsgid = current->fsgid ;
    cp->cap_effective = current->cap_effective ;
    cp->cap_inheritable = current->cap_inheritable ;
    cp->cap_permitted = current->cap_permitted ;
    cp->ngroups = current->ngroups ;
    for (i = 0; i < current->ngroups; i++)
	cp->groups[i] = current->groups[i] ;
    lis_spin_unlock_irqrestore(&lis_task_lock, &psw) ;
#endif
}

void	lis_creds_to_task(lis_kcreds_t *cp)
{
#if 0
    int		i ;
    lis_flags_t psw;

    lis_spin_lock_irqsave(&lis_task_lock, &psw) ;
    current->uid = cp->uid ;
    current->euid = cp->euid ;
    current->suid = cp->suid ;
    current->fsuid = cp->fsuid ;
    current->gid = cp->gid ;
    current->egid = cp->egid ;
    current->sgid = cp->sgid ;
    current->fsgid = cp->fsgid ;
    current->cap_effective = cp->cap_effective ;
    current->cap_inheritable = cp->cap_inheritable ;
    current->cap_permitted = cp->cap_permitted ;
    current->ngroups = cp->ngroups ;
    for (i = 0; i < cp->ngroups; i++)
	current->groups[i] = cp->groups[i] ;
    lis_spin_unlock_irqrestore(&lis_task_lock, &psw) ;
#endif
}

/************************************************************************
*                           lis_mknod                                   *
*************************************************************************
*									*
* Like the system call.  "name" is the full pathname to create with the	*
* requested mode and device major/minor.				*
*									*
************************************************************************/
int	_RP lis_mknod(char *name, int mode, dev_t dev)
{
    mm_segment_t	old_fs;
    int			ret;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

#if defined(KERNEL_2_5) && !defined(KERNEL_2_6)
    ret = syscall_mknod(name, mode, kdev_val(dev)) ;
#else
    ret = syscall_mknod(name, mode, dev) ;
#endif

    set_fs(old_fs);
    return(ret < 0 ? -errno : ret) ;
}

/************************************************************************
*                           lis_unlink                                  *
*************************************************************************
*									*
* Remove a name from the directory structure.				*
*									*
************************************************************************/
int	_RP lis_unlink(char *name)
{
    mm_segment_t	old_fs;
    int			ret;

    old_fs = get_fs();
    set_fs(KERNEL_DS);

    ret = syscall_unlink(name) ;

    set_fs(old_fs);
    return(ret < 0 ? -errno : ret) ;
}

#if defined(FATTACH_VIA_MOUNT)
/*
 *  The following is an adaptation of 'permission(inode, mask)'; we
 *  need it because our argument is a path, not an inode.  Additionally,
 *  however, we want to check ownership for non-superuser processes.
 *
 *  The semantics here are patterned after what Solaris does for fattach,
 *  i.e., (root || (owner && write permission)).  It applies to both
 *  fattach & fdetach (i.e., mount/umount).  We expect EPERM if not non-root
 *  owner, otherwise EACCES if not write permission.
 *                                                     - JB 9/26/03
 */
int mount_permission(char * path)
{
    int mask = MAY_WRITE;	/* read/exec access not needed */
    int error = 0;
    struct nameidata nd;
    
    /*
     *  Always grant permission to superuser; no need for further checks
     */
    if (capable(CAP_SYS_ADMIN))  return 0;
    
    /*
     *  Otherwise, we need to check the owner, and permissions, at the
     *  path's inode.  The owner check we do is process effective user
     *  (FIXME if this isn't appropriate).
     */
    error = user_path_walk(path, &nd);
    if (!error) {
	struct dentry *dentry = nd.dentry;
	struct inode *inode   = (dentry ? dentry->d_inode : NULL);

#if !defined(KERNEL_2_5)
	/* 2.4.x kernels do something like the following... */
	if (inode && inode->i_op && inode->i_op->revalidate)
	    error = inode->i_op->revalidate(dentry);
#endif
	
	/* check process euid == inode uid */
	if (!error && (current->euid != inode->i_uid))
	    error = -EPERM;  /* Solaris uses this for 'Not owner' */
	
	/* check permission(s) */
	if (!error)
#if defined(KERNEL_2_5)
	    error = permission(inode, mask, &nd);
#else
	    error = permission(inode, mask);
#endif
	
	path_release(&nd);
    }

    return error;
}
#endif

/************************************************************************
*                             lis_mount                                 *
*************************************************************************
*									*
* A wrapper for a mount system call.					*
*									*
************************************************************************/
int	lis_mount(char *dev_name,
		  char *dir_name,
		  char *fstype,
		  unsigned long rwflag,
		  void *data)
{
    mm_segment_t	old_fs;
    int			ret;
#if defined(FATTACH_VIA_MOUNT)
    kernel_cap_t        cap = current->cap_effective;
#endif

    old_fs = get_fs();
    set_fs(KERNEL_DS);

#if defined(FATTACH_VIA_MOUNT)
    if (!(ret = mount_permission(dir_name))) {

	if (!cap_raise(current->cap_effective, CAP_SYS_ADMIN))
	    ret = -EPERM;
	else
	    ret = syscall_mount(dev_name, dir_name, fstype, rwflag, data) ;

	current->cap_effective = cap;
    }
#else
    ret = syscall_mount(dev_name, dir_name, fstype, rwflag, data) ;
#endif

    set_fs(old_fs);

    return(ret < 0 ? ret : -ret) ;
}

/************************************************************************
*                            lis_umount                                 *
*************************************************************************
*									*
* A wrapper for a umount system call.					*
*									*
* We must get the flags passed, hence we must use umount2; umount       *
* ignores flags, and fdetach via umount[2]() doesn't work without the   *
* MNT_FORCE flag being passed.   - JB 2002/11/18                        *
************************************************************************/
int	lis_umount2(char *path, int flags)
{
    mm_segment_t	old_fs;
    int			ret;
#if defined(FATTACH_VIA_MOUNT)
    kernel_cap_t        cap = current->cap_effective;
#endif

    old_fs = get_fs();
    set_fs(KERNEL_DS);
#if defined(FATTACH_VIA_MOUNT)
    if (!(ret = mount_permission(path))) {

	if (!cap_raise(current->cap_effective, CAP_SYS_ADMIN))
	    ret = -EPERM;
	else
	    ret = syscall_umount2(path, flags) ;

	current->cap_effective = cap;
    }
#else
    ret = syscall_umount2(path, flags) ;
#endif

    set_fs(old_fs);

    return(ret < 0 ? ret : -ret) ;
}

/************************************************************************
*                         Module Information                            *
************************************************************************/

/*
 * Note:  We are labeling the module license as "GPL and additional rights".
 * This is said to be equivalent to GPL for symbol exporting purposes and
 * is also supposed to span LGPL.
 */

/* Note: this release is GPL. -bb */

#if defined(MODULE_LICENSE)
MODULE_LICENSE("GPL");
#endif
#if defined(MODULE_AUTHOR)
MODULE_AUTHOR("David Grothe <dave@gcom.com>");
#endif
#if defined(MODULE_DESCRIPTION)
MODULE_DESCRIPTION("SVR4 STREAMS for Linux (GPL Code)");
#endif
#if defined(MODULE_ALIAS)
MODULE_ALIAS("streams-" __stringify(LIS_OBJNAME));
#endif

/************************************************************************
*                 Linux Kernel Cache Memory Routines                    *
*                 for allocating and freeing mdbblocks                  *
*             as a LiS performance improvement under Linux              *
************************************************************************/

#if defined(USE_KMEM_CACHE) 
void lis_init_msg(void)
{
        lis_msgb_cachep =
            kmem_cache_create("LiS-msgb", sizeof(struct mdbblock),
                                0, SLAB_HWCACHE_ALIGN, NULL, NULL);
        if (!lis_msgb_cachep) 
                printk("lis_init_msg: lis_msgb_cachep is NULL. "
			"kmem_cache_create failed\n");
}

void lis_msgb_cache_freehdr(void *bp)
{
      if (lis_msgb_cachep) {
              LisDownCount(HEADERS);
	      K_ATOMIC_DEC(&lis_msgb_cnt) ;
              kmem_cache_free(lis_msgb_cachep, (struct mdbblock *) bp);
	      if (LIS_DEBUG_CACHE)
		printk("lis_msgb_cache_freehdr: freed %p\n", bp);
      }
}/*lis_msgb_cachep_free*/


struct mdbblock *lis_kmem_cache_allochdr(void)
{
      extern lis_atomic_t  lis_strcount ;     /* # bytes allocated to msgs   */
      extern long          lis_max_msg_mem ;  /* maximum to allocate */

      struct mdbblock *md;

      if (   (   !lis_max_msg_mem
	      || K_ATOMIC_READ(&lis_strcount) < lis_max_msg_mem
	     )
	  && (md = kmem_cache_alloc(lis_msgb_cachep, GFP_ATOMIC))
	 )
      {
	  LisUpCount(HEADERS);
	  K_ATOMIC_INC(&lis_msgb_cnt) ;
	  if (LIS_DEBUG_CACHE)
	      printk("lis_kmem_cache_allochdr: allocated %p\n", md);
	  md->msgblk.m_mblock.b_next = NULL;
	  return (md);
      } 
      
      printk("lis_kmem_cache_allochdr: msgb allocation failed\n");
      LisUpFailCount(HEADERS);
      return (NULL);
}

void lis_terminate_msg(void)
{
    lis_cache_destroy(lis_msgb_cachep, &lis_msgb_cnt, "LiS-msgb") ;
}

/************************************************************************
*                 Linux Kernel Cache Memory Routines                    *
*             for allocating and freeing queues and qbands              *
*             as a LiS performance improvement under Linux              *
************************************************************************/

void lis_cache_destroy(kmem_cache_t *p, lis_atomic_t *c, char *label)
{
    int		n = K_ATOMIC_READ(c);

    if (n)
    {
	printk("lis_cache_destroy: "
	       "Kernel cache \"%s\" has %d blocks still allocated\n"
	       "                   "
	       "Not destroying kernel cache.  You will probably have\n"
	       "                   "
	       "to reboot to clear this condition.\n",
	       label, n) ;
	return ;
    }

    kmem_cache_destroy(p);
}

void lis_init_queues(void)
{
      lis_queue_cachep =
          kmem_cache_create("LiS-queue", sizeof(queue_t)*2, 0,
                            SLAB_HWCACHE_ALIGN, NULL, NULL);
      lis_qsync_cachep =
          kmem_cache_create("LiS-qsync", sizeof(lis_q_sync_t), 0,
                            SLAB_HWCACHE_ALIGN, NULL, NULL);
      lis_qband_cachep =
          kmem_cache_create("LiS-qband", sizeof(qband_t), 0,
                            SLAB_HWCACHE_ALIGN, NULL, NULL);
      lis_head_cachep =
          kmem_cache_create("LiS-head", sizeof(stdata_t), 0,
                            SLAB_HWCACHE_ALIGN, NULL, NULL);
}

void lis_terminate_queues(void)
{
      lis_cache_destroy(lis_head_cachep, &lis_head_cnt, "LiS-head");
      lis_cache_destroy(lis_qband_cachep, &lis_qband_cnt, "LiS-qband");
      lis_cache_destroy(lis_queue_cachep, &lis_queue_cnt, "LiS-queue");
      lis_cache_destroy(lis_qsync_cachep, &lis_qsync_cnt, "LiS-qsync");
}
#endif

/************************************************************************
*            LINUX kernel cache based SVR4 Compatible timeout           *
*************************************************************************
*                                                                       *
* Uses the kernel cache mechanism to speed allocations and keep timers	*
* grouped together.							*
*                                                                       *
************************************************************************/

int lis_timer_size ;

void lis_init_timers(int size)
{
#if defined(USE_KMEM_TIMER) 
    lis_timer_cachep = kmem_cache_create("lis_timer_cachep", 
					 size, 
					 0, SLAB_HWCACHE_ALIGN, NULL, NULL);
    if (!lis_timer_cachep) 
	printk("lis_init_dki: lis_timer_cachep is NULL. "
			"kmem_cache_create failed\n");
#endif
    lis_timer_size = size ;
}

void lis_terminate_timers(void)
{
#if defined(USE_KMEM_TIMER) 
    kmem_cache_destroy(lis_timer_cachep) ;
#endif
}

void *lis_alloc_timer(char *file, int line)
{
    void	*t ;

#if !defined(USE_KMEM_TIMER)
    t = LISALLOC(lis_timer_size,file, line) ;
#else
    t = kmem_cache_alloc(lis_timer_cachep, GFP_ATOMIC) ;
#endif

    return(t) ;
}

void *lis_free_timer(void *timerp)
{
#if !defined(USE_KMEM_TIMER)
    FREE(timerp) ;
#else
    kmem_cache_free(lis_timer_cachep, timerp);
#endif
    return(NULL) ;
}



/*  -------------------------------------------------------------------  */
