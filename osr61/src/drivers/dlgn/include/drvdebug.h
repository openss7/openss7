/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : drvdebug.h
 * Description                  :  driver drbug
 *
 *
 **********************************************************************/

/***********************************************************************
 * Debugging information for UNIX device driver modules.
 * Debugging may be enabled by defining the __DRVDEBUG_H__
 * macro and then including this file. Compatible with
 * certain information found in typedefs.h.
 **********************************************************************/


#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * Guarantee that a known UNIX version has been defined. See the
 * appropriate UNIX.* files.
 */
#if ( !defined(LINUX) && !defined(SVR3) && !defined(SVR4) && !defined(UNISYS_MP) && !defined(UNISYS_UP))
   *** ERROR: debug.h: UNKNOWN UNIX VERSION ***
#endif


#ifdef __DRVDEBUG_H__

/*
 * Make PRIVATE functions/data visible to the kernel debugger.
 */
#ifdef PRIVATE
#undef PRIVATE
#endif
#define PRIVATE

/*
 * Enable debug printing.
 */
#define DBGPRT

/*
 * Define the debug printing flag. <sys/cmn_err.h> must be included.
 */
#ifndef LFS
#ifdef CE_PANIC
#define CE_DEBUG  (CE_PANIC + CE_WARN + CE_NOTE + CE_CONT)
#endif
#endif


#endif

