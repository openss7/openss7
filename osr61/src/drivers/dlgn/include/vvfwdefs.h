/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : vvfwdefs.h
 * Description                  : voice view firmware equates
 *
 *
 **********************************************************************/

#define VVIEW_ID   0x035

/* Voice View Commands ... */
#define		VV_OPEN			1
#define		VV_CLOSE		2
#define		VV_XMITPREP		3		/* VV_TX_DATA in vvfwdefs.h */
#define		VV_RCVPREP		4		/* VV_RX_DATA in vvfwdefs.h */
#define		VV_ATCMD		5

/* Replies ... */
#define		VV_OPEN_CMPLT		VV_OPEN	
#define		VV_CLOSE_CMPLT		VV_CLOSE
#define		VV_XMITPREP_CMPLT	VV_XMITPREP
#define		VV_RCVPREP_CMPLT	VV_RCVPREP		
#define		VV_ATCMD_CMPLT		VV_ATCMD
#define     VV_B0FULL			17
#define     VV_B1FULL		    18
#define     VV_B0EMPTY          19
#define     VV_B1EMPTY          20


/* Encoded replies for use in the PM */
#define VV_XMITPREP_CMPLT_RPY  ((VVIEW_ID << 8) | VV_XMITPREP_CMPLT)
#define VV_RCVPREP_CMPLT_RPY   ((VVIEW_ID << 8) | VV_RCVPREP_CMPLT) 
#define VV_B0FULL_RPY          ((VVIEW_ID << 8) | VV_B0FULL)
#define VV_B1FULL_RPY          ((VVIEW_ID << 8) | VV_B1FULL)
#define VV_B0EMPTY_RPY         ((VVIEW_ID << 8) | VV_B0EMPTY)
#define VV_B1EMPTY_RPY         ((VVIEW_ID << 8) | VV_B1EMPTY)
