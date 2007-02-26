/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : genboot.h
 * Description                  : genboot FW
 *
 *
 **********************************************************************/

#ifndef __GENBOOTDEF_H__
#define __GENBOOTDEF_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


#pragma pack(1)


#define GNB_FWLBUFSIZE 0x600
#define GNB_BOOTCMDOFF  0x01ea0     /* GENBOOT boot command area offset */


/*
 *  Genboot boot command area structure
 * SIZE == ? * ? == ?? bytes
 */

typedef struct gnb_bootcmd {
   UCHAR	memtstflg;
   UCHAR	protocol;
   UCHAR	ccmack;
   UCHAR	modtype;
   UCHAR	clksrc;
   UCHAR	numts;
   UCHAR	sigtype;
   UCHAR	encode;
   UCHAR	isdn;
   ULONG	featmask;
   ULONG	defmask;
   UCHAR	irqnum;
   UCHAR	irqterm;
   UCHAR	asicspeed;
   UCHAR	board_id;
   UCHAR	isa_width; 
   UCHAR	rfu2[ 0x20 - 0x1F ];
   UCHAR	dsphtest;
   ULONG	dspdel;
   UCHAR	debug_level;
}GNB_BOOTCMD;

#define	GNB_BOOTCMD_FORMAT	"[9]144[7]141"

/*
 * Restore default pack alignment
 */
#pragma pack()


#endif

