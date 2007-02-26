/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dlgnextern.h
 * Description                  : dlgn extern definitions
 *
 *
 **********************************************************************/

#ifndef __DLGNEXTERN_H__
#define __DLGNEXTERN_H__

/*
 * Define conditional compile entry points to internal debug routines
 */
#ifdef VME_DEBUG
#   define DLGC_VMEDEBUG_MSG2(y,z)              dlgn_msg((y),(z))
#   define DLGC_VMEDEBUG_MSG3(x,y,z)            dlgn_msg((x),(y),(z))
#   define DLGC_VMEDEBUG_MSG4(w,x,y,z)          dlgn_msg((w),(x),(y),(z))
#   define DLGC_VMEDEBUG_MSG5(v,w,x,y,z)        dlgn_msg((v),(w),(x),(y),(z))
#   define DLGC_VMEDEBUG_MSG6(u,v,w,x,y,z)      dlgn_msg((u),(v),(w),(x),(y),(z))
#   define DLGC_VMEDEBUG_MSG7(t,u,v,w,x,y,z)    dlgn_msg((t),(u),(v),(w),(x),(y),(z))
#   define DLGC_VMEDEBUG_MSG8(s,t,u,v,w,x,y,z)  dlgn_msg((s),(t),(u),(v),(w),(x),(y),(z))
#
#else /* DBGPRT */
#   define DLGC_VMEDEBUG_MSG2(y,z)
#   define DLGC_VMEDEBUG_MSG3(x,y,z)
#   define DLGC_VMEDEBUG_MSG4(w,x,y,z)
#   define DLGC_VMEDEBUG_MSG5(v,w,x,y,z)
#   define DLGC_VMEDEBUG_MSG6(u,v,w,x,y,z)
#   define DLGC_VMEDEBUG_MSG7(t,u,v,w,x,y,z)
#   define DLGC_VMEDEBUG_MSG8(s,t,u,v,w,x,y,z)
#endif /* DBGPRT */
#ifdef LINUX

#ifndef LFS
#define CE_CONT  0      /* continuation */
#define CE_NOTE  1      /* notice       */
#define CE_WARN  2      /* warning      */
#define CE_PANIC 3      /* panic        */ 

#   define DLGC_MSG2(y,z)                lis_printk((z))
#   define DLGC_MSG3(x,y,z)              lis_printk((y),(z))
#   define DLGC_MSG4(w,x,y,z)            lis_printk((x),(y),(z))
#   define DLGC_MSG5(v,w,x,y,z)          lis_printk((w),(x),(y),(z))
#   define DLGC_MSG6(u,v,w,x,y,z)        lis_printk((v),(w),(x),(y),(z))
#   define DLGC_MSG7(t,u,v,w,x,y,z)      lis_printk((u),(v),(w),(x),(y),(z))
#   define DLGC_MSG8(s,t,u,v,w,x,y,z)    lis_printk((t),(u),(v),(w),(x),(y),(z))
#else
#   define DLGC_MSG2(x,args...)		 cmn_err((x),##args)
#   define DLGC_MSG3(x,args...)		 cmn_err((x),##args)
#   define DLGC_MSG4(x,args...)		 cmn_err((x),##args)
#   define DLGC_MSG5(x,args...)		 cmn_err((x),##args)
#   define DLGC_MSG6(x,args...)		 cmn_err((x),##args)
#   define DLGC_MSG7(x,args...)		 cmn_err((x),##args)
#   define DLGC_MSG8(x,args...)		 cmn_err((x),##args)
#endif
#else
#ifndef LFS
#   define DLGC_MSG2(y,z)                       dlgn_msg((y),(z))
#   define DLGC_MSG3(x,y,z)                     dlgn_msg((x),(y),(z))
#   define DLGC_MSG4(w,x,y,z)                   dlgn_msg((w),(x),(y),(z))
#   define DLGC_MSG5(v,w,x,y,z)                 dlgn_msg((v),(w),(x),(y),(z))
#   define DLGC_MSG6(u,v,w,x,y,z)               dlgn_msg((u),(v),(w),(x),(y),(z))
#   define DLGC_MSG7(t,u,v,w,x,y,z)             dlgn_msg((t),(u),(v),(w),(x),(y),(z))
#   define DLGC_MSG8(s,t,u,v,w,x,y,z)           dlgn_msg((s),(t),(u),(v),(w),(x),(y),(z))
#else
#   define DLGC_MSG2(x,args...)		 dlgn_msg((x),##args)
#   define DLGC_MSG3(x,args...)		 dlgn_msg((x),##args)
#   define DLGC_MSG4(x,args...)		 dlgn_msg((x),##args)
#   define DLGC_MSG5(x,args...)		 dlgn_msg((x),##args)
#   define DLGC_MSG6(x,args...)		 dlgn_msg((x),##args)
#   define DLGC_MSG7(x,args...)		 dlgn_msg((x),##args)
#   define DLGC_MSG8(x,args...)		 dlgn_msg((x),##args)
#endif
#endif /* LINUX */
#endif /* __DLGNEXTERN_H__ */
