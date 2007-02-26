/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : fxphym.h
 * Description                  : fax fw command/reply definitions
 *
 *
 **********************************************************************/

/*
 * Host to board : Phymdimple commands
 */
#define PHYM_OPEN		0x81
#define PHYM_CLOSE		0x82
#define PHYM_XMIT		0x83
#define PHYM_RECV		0x84

/*
 * Board to host : Phymdimple responses
 */
#define PHYM_OPEN_CMPLT		0x81
#define PHYM_CLOSE_CMPLT	0x82
#define PHYM_XMIT_CMPLT		0x83
#define PHYM_RECV_CMPLT		0x84


/*
 * Board to host : Phymdimple response masks for Protocol Module
 */
#define PHYM_OPEN_CMPLT_RPY           ((FAX_ID << 8)|PHYM_OPEN_CMPLT)
#define PHYM_CLOSE_CMPLT_RPY           ((FAX_ID << 8)|PHYM_CLOSE_CMPLT)
#define PHYM_XMIT_CMPLT_RPY           ((FAX_ID << 8)|PHYM_XMIT_CMPLT)
#define PHYM_RECV_CMPLT_RPY           ((FAX_ID << 8)|PHYM_RECV_CMPLT)



