/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dlgcos.h
 * Description                  : OS specific definitions
 *
 *
 **********************************************************************/

#ifdef sun
#   define DLGCQPROCSON(z)    qprocson((z))
#   define DLGCQPROCSOFF(z)   qprocsoff((z))
#   define DLGCDECLARESPL(z)
#   define DLGCSPLSTR(z)
#   define DLGCSPLHI(z)
#   define DLGCSPLX(z)
#elif LINUX
#ifdef LFS
#   define DLGCQPROCSON(z)    qprocson((z))
#   define DLGCQPROCSOFF(z)   qprocsoff((z))
#   define DLGCDECLARESPL(z)
#   define DLGCSPLSTR(z)
#   define DLGCSPLHI(z)
#   define DLGCSPLX(z)
#   define inb(p)		inb_p((p))
#   define outb(p,v)		outb_p((v),(p))
#else
#   define DLGCQPROCSOFF(z)
#   define DLGCDECLARESPL(z)  unsigned long (z);
#   define DLGCSPLSTR(z)
#   define DLGCSPLHI(z)
#   define DLGCSPLX(z)
#   define inb(p)		inb_p((p))
#   define outb(p,v)		outb_p((v),(p))
#endif
#else           
#   define DLGCQPROCSOFF(z)
#   define DLGCDECLARESPL(z)  int (z);
#   define DLGCSPLSTR(z)      (z) = splstr()
#   define DLGCSPLHI(z)       (z) = splhi()
#   define DLGCSPLX(z)        splx((z))
#endif /* sun */

/*
 *  These are definitions for locking routines that are needed for multiprocessor
 *  machines.
 */
#ifdef sun
#   define DLGCMUTEX_ENTER(z)  mutex_enter((z))
#   define DLGCMUTEX_EXIT(z)   mutex_exit((z))
#   define DLGCTASB(y,z)
#   define DLGCCB(z)
#   define DLGCSPINLOCKINIT(y,z)
#   define DLGCSPINLOCK(y,z)
#   define DLGCSPINUNLOCK(y,z)
#elif LINUX
#include <asm/bitops.h>
#   define DLGCTASB(y,z)		(z) = test_and_set_bit(0, (void*)&y)
#   define DLGCCB(z)			clear_bit(0, (void*)&z)
#ifdef LFS
#   define DLGCSPINLOCKINIT(y,z)	spin_lock_init(&y)
#   define DLGCSPINLOCK(y,z)		spin_lock_irqsave(&y, z)
#   define DLGCSPINUNLOCK(y,z)		spin_unlock_irqrestore(&y, z)
#   define DLGCMUTEX_ENTER(z)
#   define DLGCMUTEX_EXIT(z)
#else
#include <sys/lislocks.h>
#   define DLGCSPINLOCKINIT(y,z)	lis_spin_lock_init(&y, z)
#   define DLGCSPINLOCK(y,z)		lis_spin_lock_irqsave(&y, &z)
#   define DLGCSPINUNLOCK(y,z)		lis_spin_unlock_irqrestore(&y, &z)
#   define DLGCMUTEX_ENTER(z)
#   define DLGCMUTEX_EXIT(z)
#endif
#else 
#   define DLGCMUTEX_ENTER(z)
#   define DLGCMUTEX_EXIT(z)
#   define DLGCTASB(y,z)
#   define DLGCCB(z)
#   define DLGCSPINLOCKINIT(y,z)
#   define DLGCSPINLOCK(y,z)
#   define DLGCSPINUNLOCK(y,z)
#endif /* sun */

/*
 *  Some minor things to make the code prettier.
 */
#ifdef SVR3
#   define GETMINOR()   minor(dev)
#else
#   define GETMINOR()   getminor(*devp)
#endif /* SVR3 */

/*  Debug Functions  */
#ifdef DBGPRT
# ifdef sun
#   define DEBUG_SCT_MSG2(y,z)               sct_msg((y),(z))
#   define DEBUG_SCT_MSG3(x,y,z)             sct_msg((x),(y),(z))
#   define DEBUG_SCT_MSG4(w,x,y,z)           sct_msg((w),(x),(y),(z))
#   define DEBUG_SCT_MSG5(v,w,x,y,z)         sct_msg((v),(w),(x),(y),(z))
#   define DEBUG_SCT_MSG6(u,v,w,x,y,z)       sct_msg((u),(v),(w),(x),(y),(z))
#   define DEBUG_SCT_MSG7(t,u,v,w,x,y,z)     sct_msg((t),(u),(v),(w),(x),(y),(z))
#   define DEBUG_SCT_MSG8(s,t,u,v,w,x,y,z)   sct_msg((s),(t),(u),(v),(w),(x),(y),(z))
#   define DEBUG_DLGN_MSG2(y,z)              sct_msg((y),(z))
#   define DEBUG_DLGN_MSG3(x,y,z)            sct_msg((x),(y),(z))
#   define DEBUG_DLGN_MSG4(w,x,y,z)          sct_msg((w),(x),(y),(z))
#   define DEBUG_DLGN_MSG5(v,w,x,y,z)        sct_msg((v),(w),(x),(y),(z))
#   define DEBUG_DLGN_MSG6(u,v,w,x,y,z)      sct_msg((u),(v),(w),(x),(y),(z))
#   define DEBUG_DLGN_MSG7(t,u,v,w,x,y,z)    sct_msg((t),(u),(v),(w),(x),(y),(z))
#   define DEBUG_DLGN_MSG8(s,t,u,v,w,x,y,z)  sct_msg((s),(t),(u),(v),(w),(x),(y),(z))
# else 
#   define DEBUG_SCT_MSG2(y,z)               dlgn_msg((y),(z))
#   define DEBUG_SCT_MSG3(x,y,z)             dlgn_msg((x),(y),(z))
#   define DEBUG_SCT_MSG4(w,x,y,z)           dlgn_msg((w),(x),(y),(z))
#   define DEBUG_SCT_MSG5(v,w,x,y,z)         dlgn_msg((v),(w),(x),(y),(z))
#   define DEBUG_SCT_MSG6(u,v,w,x,y,z)       dlgn_msg((u),(v),(w),(x),(y),(z))
#   define DEBUG_SCT_MSG7(t,u,v,w,x,y,z)     dlgn_msg((t),(u),(v),(w),(x),(y),(z))
#   define DEBUG_SCT_MSG8(s,t,u,v,w,x,y,z)   dlgn_msg((s),(t),(u),(v),(w),(x),(y),(z))
#   define DEBUG_DLGN_MSG2(y,z)              dlgn_msg((y),(z))
#   define DEBUG_DLGN_MSG3(x,y,z)            dlgn_msg((x),(y),(z))
#   define DEBUG_DLGN_MSG4(w,x,y,z)          dlgn_msg((w),(x),(y),(z))
#   define DEBUG_DLGN_MSG5(v,w,x,y,z)        dlgn_msg((v),(w),(x),(y),(z))
#   define DEBUG_DLGN_MSG6(u,v,w,x,y,z)      dlgn_msg((u),(v),(w),(x),(y),(z))
#   define DEBUG_DLGN_MSG7(t,u,v,w,x,y,z)    dlgn_msg((t),(u),(v),(w),(x),(y),(z))
#   define DEBUG_DLGN_MSG8(s,t,u,v,w,x,y,z)  dlgn_msg((s),(t),(u),(v),(w),(x),(y),(z))
# endif /* sun */
#else /* DBGPRT */
#   define DEBUG_SCT_MSG2(y,z)
#   define DEBUG_SCT_MSG3(x,y,z)
#   define DEBUG_SCT_MSG4(w,x,y,z)
#   define DEBUG_DLGN_MSG2(y,z)
#   define DEBUG_DLGN_MSG3(x,y,z)
#   define DEBUG_DLGN_MSG4(w,x,y,z)
#endif /* DBGPRT */



/*  Solaris debug functions  */
#ifdef SOLARIS_DEBUG
#   define SOL_DEBUG_CMN_ERR2(y,z)   cmn_err((y),(z))
#else /* SOLARIS_DEBUG */
#   define SOL_DEBUG_CMN_ERR2(y,z)
#endif /* SOLARIS_DEBUG */

