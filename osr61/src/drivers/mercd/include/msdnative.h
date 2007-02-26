/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : mercd_native.h
 * Description                  : native functions definitions
 *
 *
 **********************************************************************/

#ifndef _MSD_NATIVE_
#define _MSD_NATIVE_

#if defined LiS || defined LIS || defined LFS
#if !defined LFS
#define abstract_alloc_dma(s)		lis_alloc_dma(s)
#define abstract_bufcall(s,p,f,a)	bufcall(s,p,f,a)
#define abstract_esballoc(s,b,p,f)	esballoc(s,b,p,f)
#define abstract_flushq(q,t)		flushq(q, t)
#define abstract_freeb(p)		freeb(p)
#define abstract_free_mem(a)		lis_free_mem(a)
#define abstract_free_pages(p)		lis_free_pages(p)
#define abstract_get_free_pages(n)	lis_get_free_pages(n)
#define abstract_jiffies()		lis_jiffies()
#define abstract_putq(q,m)		putq(q, m)
#define abstract_register_dev(n,i,o,s)	lis_register_strdev(n,i,o,s) 
#define abstract_unregister_dev(n,s)	lis_unregister_strdev(n)
#else
/* FIXME: convert these to LFS */
#define abstract_alloc_dma(s)		kmem_alloc(s,KM_DMA)
#define abstract_alloc_atomic(s)	kmem_alloc(s,KM_NOSLEEP)
#define abstract_alloc_kernel(s)	kmem_alloc(s,KM_SLEEP)
#define abstract_bufcall(s,p,f,a)	bufcall(s,p,f,(long)a)
#define abstract_esballoc(s,b,p,f)	esballoc(s,b,p,f)
#define abstract_flushq(q,t)		flushq(q, t)
#define abstract_freeb(p)		freeb(p)
#define abstract_free_mem(a)		kmem_free(a,sizeof(*a))
#define abstract_free_pages(p)		lis_free_pages(p)
#define abstract_get_free_pages(n)	lis_get_free_pages(n,GFP_ATOMIC)
#define abstract_jiffies()		jiffies
#define abstract_putq(q,m)		putq(q, m)
#define abstract_register_dev(n,i,o,s)	lis_register_strdev(n,i,o,s) 
#define abstract_unregister_dev(n,s)	lis_unregister_strdev(n)
#endif


#else 

// native defs
#define abstract_alloc_dma(s)		native_alloc_dma(s)
#define abstract_bufcall(s,p,f,a)	
#define abstract_esballoc(s,b,p,f)	
#define abstract_flushq(q,t)		
#define abstract_freeb(p)		native_freeb(p)
#define abstract_free_mem(a)		MSD_FREE_KERNEL_MEMORY(a,0)
#define abstract_free_pages(p)		
#define abstract_get_free_pages(n)	
#define abstract_jiffies()		jiffies
#define abstract_putq(q,m)			
#define abstract_register_dev(n,i,o,s)	register_chrdev(n,s,i) 
#define abstract_unregister_dev(n,s)	unregister_chrdev(n,s)

#define cmn_err 			native_cmn_err

#define CE_CONT         0       /* continue printing */
#define CE_NOTE         1       /* NOTICE */
#define CE_WARN         2       /* WARNING */
#define CE_PANIC        3       /* PANIC */

#define M_DATA          0
#define M_PROTO         1
#define M_BREAK         2

#define BPRI_HI         0
#define BPRI_MED        1
#define BPRI_LO         2

#define FLUSHR          0x01    /* flush read side */
#define FLUSHW          0x02    /* flush write side */
#define FLUSHRW         0x03    /* flush both read & write sides */

/* 
 * STREAMS queue flags (q_flag) mask values
 */
#define QENAB           0x000001 /* queue enabled */
#define QWANTR          0x000002 /* usr wants to read */
#define QWANTW          0x000004 /* usr wants to write */
#define QFULL           0x000008 /* queue is full */
#define QREADR          0x000010 /* it's the reader */
#define QUSE            0x000020 /* not a free queue */
#define QNOENB          0x000040 /* don't enable with putq() */
#define QBACK           0x000080 /* a back-enabled queue */
#define QRETRY          0x000100 /* retry timer's set */
#define QSCAN           0x000200 /* queue in the scan list */
#define QCLOSING        0x000400 /* strm hd wants to close this queue */
#define QWASFULL        0x000800 /* QFULL was set, still above low water */
#define QCLOSEWT        0x001000 /* strm hd waiting for queue to drain */
#define QPROCSOFF       0x002000 /* don't call Q put/svc routines */
#define QRUNNING        0x004000 /* svc proc is actually running */
#define QWAITING        0x008000 /* qdetach is waiting for wakeup */

/*  -------------------------------------------------------------------  */
/* STREAMS queue's qband flags
 */
#define QB_FULL         0x000001 /* band is full */
#define QB_WANTW        0x000002
#define QB_BACK         0x000004
#define QB_WASFULL      0x000008

/* flushq() flags
 */
#define FLUSHDATA 0              /* do not flush cntl msgs */
#define FLUSHALL  1              /* flush every msg */

/* what to qtrqget/set
 */
#define QHIWAT          0
#define QLOWAT          1
#define QMAXPSZ         2
#define QMINPSZ         3
#define QCOUNT          4
#define QFIRST          5
#define QLAST           6
#define QFLAG           7

/*  *******************************************************************  */
/*                                 Symbols                               */

/*  -------------------------------------------------------------------  */
/* STREAMS normal priority message types
 */
 
#define QNORM           0

#define M_DATA          0
#define M_PROTO         1
#define M_BREAK         2
#define M_CTL           3
#define M_DELAY         4
#define M_IOCTL         5
#define M_PASSFP        6
#define M_RSE           7
#define M_SETOPTS       8
#define M_SIG           9

/*  -------------------------------------------------------------------  */
/* STREAMS high priority message types
 */

#define QPCTL           10

#define M_COPYIN        (QPCTL+0)
#define M_COPYOUT       (QPCTL+1)
#define M_ERROR         (QPCTL+2)
#define M_FLUSH         (QPCTL+3)
#define M_HANGUP        (QPCTL+4)
#define M_IOCACK        (QPCTL+5)
#define M_IOCNAK        (QPCTL+6)
#define M_IOCDATA       (QPCTL+7)
#define M_PCPROTO       (QPCTL+8)
#define M_PCRSE         (QPCTL+9)
#define M_PCSIG         (QPCTL+10)
#define M_READ          (QPCTL+11)
#define M_STOP          (QPCTL+12)
#define M_START         (QPCTL+13)
#define M_STARTI        (QPCTL+14)
#define M_STOPI         (QPCTL+15)


// native mem
#define native_alloc_atomic(n)   native_alloc_atomic_asi((n), __FILE__, __LINE__)
#define native_alloc_kernel(n)   native_alloc_kernel_asi((n), __FILE__, __LINE__)
#define native_alloc_dma(n)      native_alloc_dma_asi((n), __FILE__, __LINE__)
#define native_free_mem(p)       native_free_mem_asi((p), __FILE__, __LINE__)
#define native_get_free_pages(n) native_get_free_pages_asi((n), __FILE__, __LINE__)
//#define native_free_pages(p)     native_free_pages_asi((p), __FILE__, __LINE__)
#define native_virt_to_phys(p)   virt_to_phys(p)
#define msgdsize(mp)             mp->b_wptr - mp->b_rptr

#define NATIVE_MAX_POOL_SIZE     3000
#define NATIVE_SEQ_POOL_SIZE     100 
#define NATIVE_MAX_BANK_SIZE     1000

// native types
typedef struct datab {
   unsigned char   *db_base;
   unsigned char   *db_lim;
   unsigned char   db_ref;
   unsigned char   db_type;
   unsigned int    db_size;
} datab_t;

typedef datab_t dblk_t;

typedef struct  msgb {
   struct  msgb    *b_next; /* next msg on queue */
   struct  msgb    *b_prev; /* prev msg on queue */
   struct  msgb    *b_cont; /* next blk of msg */
   unsigned char   *b_rptr; /* 1st unread byte */
   unsigned char   *b_wptr; /* 1st unwriten byte */
   struct datab    *b_datap; /* pointer to data */
#if 0
   /* pools allocation doesn't require this */
   unsigned char    b_band;  /* message priority */ /* Not used now */
   unsigned char    b_pad1;
   unsigned short   b_flag;
   long             b_pad2;
#endif
} msgb_t;

#endif /* LiS */

#endif
