/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : timerlibmod.h
 * Description                  : timer library-to-module
 *
 *
 **********************************************************************/

#ifndef __TIMERLIBMOD_H__
#define __TIMERLIBMOD_H__


/*
 * DEFINES
 */

/*
 * Commands and Replies
 */
#define	TM_GETPARM		MC_BASEMSGNUM + 0x00000011
#define	TM_GETPARMCMPLT		TM_GETPARM
#define	TM_SETPARM		MC_BASEMSGNUM + 0x00000012
#define	TM_SETPARMCMPLT		TM_SETPARM
#define	TM_SETTIMER		MC_BASEMSGNUM + 0x00000013
#define	TM_SETTMRCMPLT		TM_SETTIMER
#define	TM_CANCELTMR		MC_BASEMSGNUM + 0x00000014
#define	TM_CANTMRCMPLT		TM_CANCELTMR


/*
 * Events
 */
#define	TM_EVENT	0x00001000


/*
 * Errors
 */
#define TMERR_NOFREEQ	0x00001001	/* No more free Q's */
#define TMERR_NOTIMERS	0x00001002	/* No timers for that device */
#define TMERR_QINUSE	0x00001003	/* Queue is in use, can't reinit */
#define TMERR_NOMEM	0x00001004	/* No memory */


/*
 * Library to timer module data
 */
typedef struct tm_data {
   long	tm_ddd;		/* ddd on which the event will be generated */
   int	tm_id;		/* An id to associate events with	    */
   int	tm_timeout;	/* When used by lib->module, it represents  */
			/*  number of ticks before the event occurs */
			/*  When used by module->lib, it represents */
			/*  number of ticks left after the timer    */
			/*  has been cancelled or 0 when timeout    */
			/*  occurs				    */
   int	tm_rfu;
} TM_DATA;


/*
 * TM_DRVCMD
 */
typedef struct tm_drvcmd {
   GN_CMDMSG	gn_drvhdr;	/* Generic driver header	*/
   TM_DATA	tm_data;	/* Data part of the command	*/
   UCHAR	tm_rfu[12];	/* Reserved for future use	*/
} TM_DRVCMD;

#endif

