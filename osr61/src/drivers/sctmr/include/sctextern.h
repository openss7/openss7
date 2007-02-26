/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : sctextern.h
 * Description                  : specific interface to externs
 *
 *
 **********************************************************************/

#ifndef __SCTEXTERN_H__
#define __SCTEXTERN_H__

/* mutexes required */
#ifdef SOLARIS_24
extern kmutex_t st_sctlock;
extern kmutex_t st_uselock;
extern kmutex_t st_freelock;
#endif /* SOLARIS_24 */

#ifdef LINUX
#ifdef LFS
extern spinlock_t st_sctlock;
extern spinlock_t st_uselock;
extern spinlock_t st_freelock;
#else
extern lis_spin_lock_t st_sctlock;
extern lis_spin_lock_t st_uselock;
extern lis_spin_lock_t st_freelock;
#endif
#endif /* LINUX */

/*
 * Define conditional compile entry points to internal debug routines
 */
#ifdef DBGPRT
#   define DEBUG_PRINT_QUEUE(z)        sct_print_queue((z))
# ifdef SOLARIS_24
#   define DLGC_MSG2(y,z)              sct_msg((y),(z))
#   define DLGC_MSG3(x,y,z)            sct_msg((x),(y),(z))
#   define DLGC_MSG4(w,x,y,z)          sct_msg((w),(x),(y),(z))
#   define DLGC_MSG5(v,w,x,y,z)        sct_msg((v),(w),(x),(y),(z))
#   define DLGC_MSG6(u,v,w,x,y,z)      sct_msg((u),(v),(w),(x),(y),(z))
#   define DLGC_MSG7(t,u,v,w,x,y,z)    sct_msg((t),(u),(v),(w),(x),(y),(z))
#   define DLGC_MSG8(s,t,u,v,w,x,y,z)  sct_msg((s),(t),(u),(v),(w),(x),(y),(z))
# else /* SOLARIS_24 */
#   define DLGC_MSG2(y,z)              dlgn_msg((y),(z))
#   define DLGC_MSG3(x,y,z)            dlgn_msg((x),(y),(z))
#   define DLGC_MSG4(w,x,y,z)          dlgn_msg((w),(x),(y),(z))
#   define DLGC_MSG5(v,w,x,y,z)        dlgn_msg((v),(w),(x),(y),(z))
#   define DLGC_MSG6(u,v,w,x,y,z)      dlgn_msg((u),(v),(w),(x),(y),(z))
#   define DLGC_MSG7(t,u,v,w,x,y,z)    dlgn_msg((t),(u),(v),(w),(x),(y),(z))
#   define DLGC_MSG8(s,t,u,v,w,x,y,z)  dlgn_msg((s),(t),(u),(v),(w),(x),(y),(z))
# endif /* SOLARIS_24 */
#
#elif LINUX

#define CE_CONT  0	/* continuation */
#define CE_NOTE  1	/* notice       */
#define CE_WARN  2	/* warning      */
#define CE_PANIC 3	/* panic        */
 
#   define DLGC_MSG2(y,z)	         
#   define DLGC_MSG3(x,y,z)	        
#   define DLGC_MSG4(w,x,y,z)	       
#   define DLGC_MSG5(v,w,x,y,z)	      
#   define DLGC_MSG6(u,v,w,x,y,z)    
#   define DLGC_MSG7(t,u,v,w,x,y,z) 
#   define DLGC_MSG8(s,t,u,v,w,x,y,z) 
#else /* DBGPRT */
#   define DEBUG_PRINT_QUEUE(z)
#   define DLGC_MSG2(y,z)
#   define DLGC_MSG3(x,y,z)
#   define DLGC_MSG4(w,x,y,z)
#   define DLGC_MSG5(v,w,x,y,z)
#   define DLGC_MSG6(u,v,w,x,y,z)
#   define DLGC_MSG7(t,u,v,w,x,y,z)
#   define DLGC_MSG8(s,t,u,v,w,x,y,z)
#endif /* DBGPRT */

#define SCT_WQREPLY(x,y,z)           dlgn_wqreply((x),(y),(z))

#endif /* __SCTEXTERN_H__ */
