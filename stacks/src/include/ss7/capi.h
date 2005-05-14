/*****************************************************************************

 @(#) $Id: capi.h,v 0.9.2.2 2005/05/14 08:30:44 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/05/14 08:30:44 $ by $Author: brian $

 *****************************************************************************/

#ifndef __CAPI_H__
#define __CAPI_H__

#define CAPI_ALERT_REQ			0x0180	/* initiates sending of ALERT */
#define CAPI_ALERT_CON			0x0181	/* local confirmation of the request */
#define CAPI_CONNECT_REQ		0x0280	/* initiates an outgoing phys connection */
#define CAPI_CONNECT_CON		0x0281	/* local confirmation of the request */
#define CAPI_CONNECT_IND		0x0282	/* indicates an incoming physical connection */
#define CAPI_CONNECT_RES		0x0283	/* response to the indication */
#define CAPI_CONNECT_ACTIVE_IND		0x0382	/* indicates activation of a phys connection */
#define CAPI_CONNECT_ACTIVE_RES		0x0383	/* response to the indication */
#define CAPI_DISCONNECT_REQ		0x0480	/* initiates the clearing of phys connection */
#define CAPI_DISCONNECT_CON		0x0481	/* local confirmation of the request */
#define CAPI_DISCONNECT_IND		0x0482	/* indicates the clearing of phys connection */
#define CAPI_DISCONNECT_RES		0x0483	/* response to the indication */
#define CAPI_LISTEN_REQ			0x0580	/* activates call and info indications */
#define CAPI_LISTEN_CON			0x0581	/* local confirmation of the request */
#define CAPI_INFORMATION_REQ		0x0880	/* initiates sending of signalling information */
#define CAPI_INFORMATION_CON		0x0881	/* local confirmation of the request */
#define CAPI_INFORMATION_IND		0x0882	/* indicates specified signalling information */
#define CAPI_INFORMATION_RES		0x0883	/* response to the indication */
#define CAPI_SELECT_B_PROTOCOL_REQ	0x4180	/* selects protocol stack use for log conn */
#define CAPI_SELECT_B_PROTOCOL_CON	0x4181	/* local confirmation of the request */
#define CAPI_FACILITY_REQ		0x8080	/* requests facilities (e.g., ext equip) */
#define CAPI_FACILITY_CON		0x8081	/* local confirmation of the request */
#define CAPI_FACILITY_IND		0x8082	/* indicates facilities (e.g., ext equip) */
#define CAPI_FACILITY_RES		0x8083	/* response to the indication */
#define CAPI_CONNECT_B3_REQ		0x8280	/* initiats an ougoing logical connection */
#define CAPI_CONNECT_B3_CON		0x8281	/* local confirmation of the request */
#define CAPI_CONNECT_B3_IND		0x8282	/* indicates an incoming logical connection */
#define CAPI_CONNECT_B3_RES		0x8283	/* response to the indication */
#define CAPI_CONNECT_B3_ACTIVE_IND	0x8382	/* indicates the activation of a logical conn */
#define CAPI_CONNECT_B3_ACTIVE_RES	0x8383	/* response to the indication */
#define CAPI_DISCONNECT_B3_REQ		0x8480	/* initiates clearing down of logical connection */
#define CAPI_DISCONNECT_B3_CON		0x8481	/* local confirmation of the request */
#define CAPI_DISCONNECT_B3_IND		0x8482	/* indicates the clearing down of logical conn */
#define CAPI_DISCONNECT_B3_RES		0x8483	/* response to the indication */
#define CAPI_DATA_B3_REQ		0x8680	/* initiates sending of data over logical conn */
#define CAPI_DATA_B3_CON		0x8681	/* local confirmation of the request */
#define CAPI_DATA_B3_IND		0x8682	/* indicates incoming data over logical connection */
#define CAPI_DATA_B3_RES		0x8683	/* response to the indication */
#define CAPI_RESET_B3_REQ		0x8780	/* initiates resetting of a logical connection */
#define CAPI_RESET_B3_CON		0x8781	/* local confirmation of the request */
#define CAPI_RESET_B3_IND		0x8782	/* indicates the resetting of a logical connection */
#define CAPI_RESET_B3_RES		0x8783	/* response to the indication */
#define CAPI_CONNECT_B3_T90_ACTIVE_IND	0x8882	/* indicates switch from T.70NL to T.90NL */
#define CAPI_CONNECT_B3_T90_ACTIVE_RES	0x8883	/* response to the indication */
#define CAPI_MANUFACTURER_REQ		0xff80	/* manufacturer-specific operation */
#define CAPI_MANUFACTURER_CON		0xff81	/* local confirmation of the request */
#define CAPI_MANUFACTURER_IND		0xff82	/* manufacturer-specific operation */
#define CAPI_MANUFACTURER_RES		0xff83	/* response to the indication */

typedef struct CAPI_alert_req {
	uint16_t prim_type;		/* always CAPI_ALERT_REQ */
	uint32_t PLCI;			/* physical link connection identifier */
	CAPI_AdditionalInfo_t info;	/* Additional Information Elements */
} CAPI_alert_req_t;

typedef struct CAPI_alert_con {
	uint16_t prim_type;		/* always CAPI_ALERT_CON */
	uint32_t PLCI;			/* physical link connection identifier */
	uint16_t info;			/* alert information */
#define CAPI_INFO_ALERT_INITIATED	0x0000	/* alert initiated */
#define CAPI_INFO_ALERT_ALREADY_SENT	0x0003	/* alert already sent by another application */
#define CAPI_INFO_OUTSTATE		0x2001	/* message not supported in current state */
#define CAPI_INFO_BAD_PLCI		0x2002	/* Illegal PLCI */
#define CAPI_INFO_BAD_PARM		0x2003	/* Illegal message parameter coding */
} CAPI_alert_con_t;

typedef struct CAPI_connect_req {
	uint16_t prim_type;		/* always CAPI_CONNECT_REQ */
	uint16_t CPIValue;		/* Compatibility Information Profile */
	struct {
	} CalledPartyNumber;		/* Called Party Number */
	struct {
	} CallingPartyNumber;		/* Calling Party Number */
	struct {
	} CalledPartySubaddress;	/* Called Party Subaddress */
	struct {
	} CallingPartySubaddress;	/* Calling Party Subaddress */
	struct {
	} BProtocol;			/* B Protocol */
	struct {
	} BC;				/* Bearer Capability */
	struct {
	} LLC;				/* Low Layer Compatibility */
	struct {
	} HLC;				/* High Layer Compatibility */
	CAPI_AdditionalInfo_t info;	/* Additional Information Elements */
} CAPI_connect_req_t;

#endif				/* __CAPI_H__ */
