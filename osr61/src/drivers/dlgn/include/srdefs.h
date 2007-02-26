/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srdefs.h
 * Description                  : SRAM protocol general definitions
 *
 *
 **********************************************************************/

#ifndef __SRDEFS_H__
#define __SRDEFS_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * Miscellaneous defines
 */
#define PM_LOMARK                0x0000

#if !defined BIG_ENDIAN && !defined LFS
#define PM_HIMARK                0x10000/* Bug: cannot fit bp_hiwater */
#else
#define PM_HIMARK                0x8000
#endif /* VME_SPAN */

#define STOP_SENDREPLY           0x00
#define STOP_NOSENDREPLY         0x01
#define STOP_STOPONLY            0x02
#define PM_DRVMSGSIZE            (sizeof(DL_DRVCMD) - sizeof(GN_CMDMSG))

#define SRAMPM_ID		501
#define SRAMPM_PLAY_ID		502
#define SRAMPM_RECORD_ID	503
#define SRAMPM_QUEUE_ID	        504
#define SRAMPM_FWCMD_ID	        505

#ifdef VME_DEBUG

#define STRACE0(x)	\
		strlog(SRAMPM_ID, 0, 0, SL_TRACE, x)
#define STRACE1(x, one)	\
		strlog(SRAMPM_ID, 0, 0, SL_TRACE, x, one)
#define STRACE2(x, one, two)	\
		strlog(SRAMPM_ID, 0, 0, SL_TRACE, x, one, two)
#define STRACE3(x, one, two, three)	\
		strlog(SRAMPM_ID, 0, 0, SL_TRACE, x, one, two, three)
#define STRACE3_PLAY(x, one, two, three)	\
		strlog(SRAMPM_PLAY_ID, 0, 0, SL_TRACE, x, one, two, three)
#define STRACE3_RECORD(x, one, two, three)	\
		strlog(SRAMPM_RECORD_ID, 0, 0, SL_TRACE, x, one, two, three)
#define STRACE3_QUEUE(x, one, two, three)	\
		strlog(SRAMPM_QUEUE_ID, 0, 0, SL_TRACE, x, one, two, three)
#define STRACE3_FWCMD(x, one, two, three)	\
		strlog(SRAMPM_FWCMD_ID, 0, 0, SL_TRACE, x, one, two, three)

#else

#define STRACE0(x) 
#define STRACE1(x, one) 
#define STRACE2(x, one, two) 
#define STRACE3(x, one, two, three) 
#define STRACE3_PLAY(x, one, two, three) 
#define STRACE3_RECORD(x, one, two, three) 
#define STRACE3_QUEUE(x, one, two, three) 
#define STRACE3_FWCMD(x, one, two, three)

#endif /* VME_DEBUG */

#endif
