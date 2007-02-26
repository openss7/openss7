/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : sbalibdrv.h
 * Description                  : sba-device definitions
 *
 *
 **********************************************************************/

#ifndef __SBALIBDRV_H__
#define __SBALIBDRV_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "gnmsg.h"
#include "sbdmsg.h"
#endif


/*
 * FWI defines
 */
#define IO_DEFFWI	0		/* Default FWI Type */

/*
 * Device-Specific Error Codes
 */
#define SBE_BADRANGE	1		/* Bad/overlapping phys mem range */
#define SBE_TIMEOUT	2		/* Timed out waiting for reply */
#define SBE_BADSIZE	3		/* Message too big or too small */
#define SBE_BADBOARD	4		/* Board missing or dead */
#define SBE_NOMEM	5		/* Can't map or allocate memory */
#define SBE_BADCMD	6		/* Invalid or undefined command */
#define SBE_BADPARAM	7		/* Invalid command parameter */
#define SBE_FWERROR	8		/* FW returned an error as reply */
#define SBE_SKIPREPLY	9		/* A required reply was skipped */
#define SBE_TOOBIG	10		/* Reply too big for user buffer */

/*
 * SB_DEVINIT
 *
 * DV_DEVINITDNLD argument structure
 *
 * This is passed to the DV_DEVINITDNLD ioctl/I_STR so we can pass all the
 * needed args in one go. Immediately follow this structure with a BYTE
 * array of size di_length.
 */
typedef struct sb_devinit {
   unsigned short   di_offset;	/* Offset in Bytes from Last Board Org */
   unsigned short   di_length;	/* Number of Bytes of Data to Download */
} SB_DEVINIT;

/*
 * SB_DRVCMD Message Structure
 * SIZE = (37 + 4 + 4 + 3) * 4 = 48 * 4 = 192 bytes
 */
typedef struct sb_drvcmd {
   GN_CMDMSG	  gn_drvhdr;	/* Generic driver header */
   SB_MESSAGE	  dc_cmnd;	/* Command message to Send to SBA */
   SB_MESSAGE	  dc_reply;	/* Command message to wait for from SBA */
   unsigned char  dc_rfu[12];
} SB_DRVCMD;

#endif

