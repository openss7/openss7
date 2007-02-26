/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dvbmextern.h
 * Description                  : Bulk Data Module externs
 *
 *
 **********************************************************************/

#ifndef __DVBMEXTERN_H__
#define __DVBMEXTERN_H__

/*
 * Define conditional compile entry points to internal debug routines
 */
#ifdef DBGPRT
#   define DLGC_DEBUG_MSG2(y,z)              dlgn_msg((y),(z))
#   define DLGC_DEBUG_MSG3(x,y,z)            dlgn_msg((x),(y),(z))
#   define DLGC_DEBUG_MSG4(w,x,y,z)          dlgn_msg((w),(x),(y),(z))
#   define DLGC_DEBUG_MSG5(v,w,x,y,z)        dlgn_msg((v),(w),(x),(y),(z))
#   define DLGC_DEBUG_MSG6(u,v,w,x,y,z)      dlgn_msg((u),(v),(w),(x),(y),(z))
#   define DLGC_DEBUG_MSG7(t,u,v,w,x,y,z)    dlgn_msg((t),(u),(v),(w),(x),(y),(z))
#   define DLGC_DEBUG_MSG8(s,t,u,v,w,x,y,z)  dlgn_msg((s),(t),(u),(v),(w),(x),(y),(z))
#
#else /* DBGPRT */
#   define DLGC_DEBUG_MSG2(y,z)
#   define DLGC_DEBUG_MSG3(x,y,z)
#   define DLGC_DEBUG_MSG4(w,x,y,z)
#   define DLGC_DEBUG_MSG5(v,w,x,y,z)
#   define DLGC_DEBUG_MSG6(u,v,w,x,y,z)
#   define DLGC_DEBUG_MSG7(t,u,v,w,x,y,z)
#   define DLGC_DEBUG_MSG8(s,t,u,v,w,x,y,z)
#endif /* DBGPRT */
#ifdef LINUX

#ifndef LFS
#define CE_CONT  0      /* continuation */
#define CE_NOTE  1      /* notice       */
#define CE_WARN  2      /* warning      */
#define CE_PANIC 3      /* panic        */
#endif

#ifdef LFS
#   define DLGC_MSG2(x,args...)		printk(args)
#   define DLGC_MSG3(x,args...)		printk(args)
#   define DLGC_MSG4(x,args...)		printk(args)
#   define DLGC_MSG5(x,args...)		printk(args)
#   define DLGC_MSG6(x,args...)		printk(args)
#   define DLGC_MSG7(x,args...)		printk(args)
#   define DLGC_MSG8(x,args...)		printk(args)
#else
#   define DLGC_MSG2(y,z)                printk((z))
#   define DLGC_MSG3(x,y,z)              printk((y),(z))
#   define DLGC_MSG4(w,x,y,z)            printk((x),(y),(z))
#   define DLGC_MSG5(v,w,x,y,z)          printk((w),(x),(y),(z))
#   define DLGC_MSG6(u,v,w,x,y,z)        printk((v),(w),(x),(y),(z))
#   define DLGC_MSG7(t,u,v,w,x,y,z)      printk((u),(v),(w),(x),(y),(z))
#   define DLGC_MSG8(s,t,u,v,w,x,y,z)    printk((t),(u),(v),(w),(x),(y),(z))
#endif
#else
#ifdef LFS
#   define DLGC_MSG2(x,args...)		    dlgn_msg((x),##args)
#   define DLGC_MSG3(x,args...)		    dlgn_msg((x),##args)
#   define DLGC_MSG4(x,args...)		    dlgn_msg((x),##args)
#   define DLGC_MSG5(x,args...)		    dlgn_msg((x),##args)
#   define DLGC_MSG6(x,args...)		    dlgn_msg((x),##args)
#   define DLGC_MSG7(x,args...)		    dlgn_msg((x),##args)
#   define DLGC_MSG8(x,args...)		    dlgn_msg((x),##args)
#else
#   define DLGC_MSG2(y,z)                    dlgn_msg((y),(z))
#   define DLGC_MSG3(x,y,z)                  dlgn_msg((x),(y),(z))
#   define DLGC_MSG4(w,x,y,z)                dlgn_msg((w),(x),(y),(z))
#   define DLGC_MSG5(v,w,x,y,z)              dlgn_msg((v),(w),(x),(y),(z))
#   define DLGC_MSG6(u,v,w,x,y,z)            dlgn_msg((u),(v),(w),(x),(y),(z))
#   define DLGC_MSG7(t,u,v,w,x,y,z)          dlgn_msg((t),(u),(v),(w),(x),(y),(z))
#   define DLGC_MSG8(s,t,u,v,w,x,y,z)        dlgn_msg((s),(t),(u),(v),(w),(x),(y),(z))
#endif

#endif /* LINUX */

/*  Change of entry points to support Solaris loadable drivers. */
#define DLGN_WQREPLY(x,y,z)           dlgn_wqreply((x),(y),(z))

#endif /* __SCTEXTERN_H__ */
