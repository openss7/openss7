/*****************************************************************************

 @(#) $Id: mtp2_ioctl.h,v 1.1.2.1 2009-06-21 11:25:34 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2009-06-21 11:25:34 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mtp2_ioctl.h,v $
 Revision 1.1.2.1  2009-06-21 11:25:34  brian
 - added files to new distro

 *****************************************************************************/

#ifndef __MTP2_IOCTL_H__
#define __MTP2_IOCTL_H__

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define MTP_IOC_MAGIC	'm'

#define MTP_OBJ_TYPE_MS		 0	/* default, managed switching element */
#define MTP_OBJ_TYPE_MT		 1	/* MTP user */
#define MTP_OBJ_TYPE_SA		 2	/* MTP service access point */
#define MTP_OBJ_TYPE_NA		 3	/* MTP network appearance */
#define MTP_OBJ_TYPE_GS		 4	/* gateway screening tables */
#define MTP_OBJ_TYPE_GT		 5	/* gateway screening table */
#define MTP_OBJ_TYPE_GR		 6	/* gateway screenning rule */
#define MTP_OBJ_TYPE_SP		 7	/* MTP signalling point */
#define MTP_OBJ_TYPE_L3		 8	/* MTP Level 3 instance */
#define MTP_OBJ_TYPE_RS		 9	/* route set */
#define MTP_OBJ_TYPE_RT		10	/* route */
#define MTP_OBJ_TYPE_LS		11	/* link set */
#define MTP_OBJ_TYPE_SL		12	/* signalling link */

/*****************************************************************************/

/*
 * SIGNALLING LINK OPTIONS
 */
typedef struct mtp_opt_conf_sl {
	mtp_ulong t1;			/* mtpSlL3ProfileTimerT1 */
	mtp_ulong t2;			/* mtpSlL3ProfileTimerT2 */
	mtp_ulong t3;			/* mtpSlL3ProfileTimerT3 */
	mtp_ulong t4;			/* mtpSlL3ProfileTimerT4 */
	mtp_ulong t5;			/* mtpSlL3ProfileTimerT5 */
	mtp_ulong t12;			/* mtpSlL3ProfileTimerT12 */
	mtp_ulong t13;			/* mtpSlL3ProfileTimerT13 */
	mtp_ulong t14;			/* mtpSlL3ProfileTimerT14 */
	mtp_ulong t17;			/* mtpSlL3ProfileTimerT17 */
	mtp_ulong t19a;			/* mtpSlL3ProfileTimerT19A */
	mtp_ulong t20a;			/* mtpSlL3ProfileTimerT20A */
	mtp_ulong t21a;			/* mtpSlL3ProfileTimerT21A */
	mtp_ulong t22;			/* mtpSlL3ProfileTimerT22I */
	mtp_ulong t23;			/* mtpSlL3ProfileTimerT23I */
	mtp_ulong t24;			/* mtpSlL3ProfileTimerT24I */
	mtp_ulong t31a;			/* mtpSlL3ProfileTimerT31A */
	mtp_ulong t32a;			/* mtpSlL3ProfileTimerT32A */
	mtp_ulong t33a;			/* mtpSlL3ProfileTimerT33A */
	mtp_ulong t34a;			/* mtpSlL3ProfileTimerT34A */
	mtp_ulong t1t;			/* mtpSlL3ProfileTimerT1T */
	mtp_ulong t2t;			/* mtpSlL3ProfileTimerT2T */
	mtp_ulong t1s;			/* mtpSlL3ProfileTimerT1S */
	mtp_ulong lgn;			/* mtpSlRelatedLinkGroupNumber */
	mtp_ulong load;			/* mtpSlMaxCapacitySL */
} mtp_opt_conf_sl_t;

/*****************************************************************************/

/*
 * SIGNALLING DATA LINK CONFIGURATION
 *
 * Sufficient identification is the local network point code, adjacent network point code and
 * circuit identification code.
 */
struct mtp_conf_sd {
	uint mtpLocPc;
	uint mtpAdjPc;
	uint mtpSdlCIC;
	uint mtpMuxId;
	/* ---------------------- */
	uint mtpMsId;			/* managed switching element identifier */
	uint mtpSpId;			/* signalling point identifier */
	uint mtpSdlId;			/* signalling data link identifier */
	uint mtpSdlAdjPc;
	int mtpSdlEquipmentPointer;	/* multiplexer index */
	uint mtpSdlCIC;
	uint mtpSdlStmChannel;
	uint mtpSdlVcTTpPointer;	/* VCI and VPI */
	char mtpSdlName[32];
};

/*
 * SIGNALLING TERMINAL CONFIGURATION
 *
 * Sufficient identifications is the managed switching element identifier and signalling terminal
 * identifier.  There is normally more than one signalling terminal.
 */

#define TYPE_NARROWBAND			1
#define TYPE_BROADBAND			2
#define TYPE_M2PA			3

struct mtp_conf_st {
	uint mtpSdtId;
	uint mtpMuxId;
	/* ---------------------- */
	uint mtpMsId;			/* managed switching element identifier */
	uint mtpSdtId;			/* signalling terminal identifier */
	uint mtpSdtType;
	uint mtpSdtEquipmentPointer;	/* multiplexer id */
	char mtpSdtName[32];
	uint mtpSdtPrimaryId;		/* mtpSdtId of primary terminal (or 0 if primary) */
};

/*
 * SIGNALLING LINK CONFIGURATION
 *
 * Sufficient identification is the local network point code, adjacent network point code and
 * signalling link code.  There is normally more than one signalling link.
 */
struct mtp_conf_sl {
	uint mtpLocPc;
	uint mtpAdjPc;
	uint mtpSlCode;
	uint mtpMuxId;
	/* ---------------------- */
	uint mtpMsId;			/* managed switching element identifier */
	uint mtpSpId;			/* signalling point identifier (local network and point code) */
	uint mtpLsId;			/* signalling link set identifier (adjacent point code) */
	uint mtpSlId;			/* signalling link identifier (signalling link code) */
	uint mtpSlSignTermPointer;	/* signalling terminal identifier */
	uint mtpSlSignDataLinkTpPointer;	/* signalling data link identifier */
	char mtpSlName[32];
	uint mtpSlSlsCodeNormalList[8];
	uint mtpSlSdlList[32];
	uint mtpSlSdtList[32];
};

/*
 * SIGNALLING LINK SET CONFIGURATION
 *
 * Sufficient identification is the local network point code and adjacent network point code.  There
 * is normally more than one signalling link set.
 */
struct mtp_conf_ls {
	uint mtpLocPc;
	uint mtpAdjPc;
	/* ---------------------- */
	uint mtpMsId;			/* managed switching element identifier */
	uint mtpSpId;			/* signalling point identifier */
	uint mtpLsId;			/* signalling link set identifier */
	/**** it might not be possible to know these until an SDL is added */
	uint mtpLsAdjPc;
	uint mtpLsRsId;			/* signalling route set identifier (of Adjacent) */
	/****/
	char mtpLsName[32];
};

#define PRIORITYMODE_INSERT			0
#define PRIORITYMODE_EQUAL			1
#define PRIORITYMODE_EXCHANGESINGLE		2
#define PRIORITYMODE_EXCHANGEGROUP		3

/*
 * SIGNALLING ROUTE CONFIGURATION
 *
 * Sufficient identification is the local network point code, remote network point code and adjacent
 * network point code.  There is normally more than one signalling route.
 */
struct mtp_conf_rt {
	uint mtpLocPc;
	uint mtpRemPc;
	uint mtpAdjPc;
	uint mtpPriority;
	/* ---------------------- */
	uint mtpMsId;			/* managed switching element identifier */
	uint mtpSpId;
	uint mtpRsId;
	uint mtpRtId;
	uint mtpRtLsPointer;
	uint mtpRtFixedPriority;
	uint mtpRtFlexiblePriority;
	uint mtpRtPriorityMode;
	char mtpRtName[32];
	uint mtpRtSlsList[8];
	uint mtpRtRlSlot;
	char mtpRtLsaSlsCodelist[16][16];
};

/*
 * SIGNALLING ROUTE SET CONFIGURATION
 *
 * Sufficent identification is the local network point code and remote network point code.  There is
 * normally more than one signalling route set.
 */
#define FLAGS_RSFCLUSTER			(1<<0)
#define FLAGS_RSFXFERFUNC			(1<<1)
#define FLAGS_RSFSECURITY			(1<<2)
#define FLAGS_RSFADJACENT			(1<<3)

struct mtp_conf_rs {
	uint mtpLocPc;
	uint mtpRemPc;
	/* ---------------------- */
	uint mtpMsId;			/* managed switching element identifier */
	uint mtpSpId;			/* signalling point identifier */
	uint mtpRsId;			/* route set identifier */
	uint mtpRsDest;			/* network point code */
	uint mtpRsFlags;		/* options flags */
	uint mtpRsRemoteExchangeLabel[32];
	char mtpRsName[32];
};

/*
 * GATEWAY SCREENING TABLES
 */

/*
 * MTP LEVEL 3 CONFIGURATION
 *
 * Sufficient identification is the local network point code.  Identification is always implied by
 * the signalling point.
 */
struct mtp_conf_l3 {
	uint mtpLocPc;
	/* ---------------------- */
	uint mtpMsId;			/* managed switching element identifier */
	uint mtpSpId;			/* signalling point identifier */
	uint mtpL3Id;
	char mtpL3Name[32];
};

/*
 * SIGNALLING POINT CONFIGURATION
 *
 * Sufficient identification is the local network point code.  When there is only one local
 * signalling point, identification is implied.
 */

struct mtp_conf_sp {
	uint mtpLocPc;
	/* ---------------------- */
	uint mtpMsId;			/* managed switching element identifier */
	uint mtpSpId;
	uint mtpSpPointCode;
	char mtpSpName[32];
	uint mtpSpNaPointer;
	uint mtpSpUsers;
};

/*
 * MANAGED SWITCHING ELEMENT CONFIGURATION
 *
 * Sufficient identification is the MSE identifier.  When there is only one managed switching
 * element, identification can be implied.
 */
struct mtp_conf_ms {
	uint mtpMsId;
	char mtpMsName[32];
};

/*
 * NETWORK CONFIGURATION
 *
 * Sufficient identification is the network appearance number.  When there is only one network,
 * identification can be implied.
 */
struct mtp_conf_na {
	uint mtpNaId;
	char mtpNaName[32];
};

/*
 * MTP-SAP CONFIGURATION
 *
 * Sufficent identification is the local network point code and MTP user part.  Where there is only
 * one MTP-SAP identification can be implied.
 */
struct mtp_conf_sa {
	uint mtpLocPc;
	uint mtpUserPart;
	/* ---------------------- */
	uint mtpMsId;
	uint mtpSpId;
	uint mtpSapId;
	char mtpSapName[32];
};

/*****************************************************************************/

#define LINKTPSTATUS_LOCALBLOCKED		(1<<0)
#define LINKTPSTATUS_REMOTEBLOCKED		(1<<1)
#define LINKTPSTATUS_LOCALINHIBITED		(1<<2)
#define LINKTPSTATUS_REMOTEINHIBITED		(1<<3)
#define LINKTPSTATUS_FAILED			(1<<4)
#define LINKTPSTATUS_DEACTIVATED		(1<<5)

#define ADMINISTRATIVESTATE_LOCKED		0
#define ADMINISTRATIVESTATE_UNLOCKED		1
#define ADMINISTRATIVESTATE_SHUTTINGDOWN	2

#define OPERATIONALSTATE_DISABLED		0
#define OPERATIONALSTATE_ENABLED		1

#define USAGESTATE_IDLE				0
#define USAGESTATE_ACTIVE			1
#define USAGESTATE_BUSY				2

#define PROCEDURALSTATUS_INITIALIZATIONREQUIRED	(1<<0)
#define PROCEDURALSTATUS_NOTINITIALIZED		(1<<1)
#define PROCEDURALSTATUS_INITIALIZING		(1<<2)
#define PROCEDURALSTATUS_REPORTING		(1<<3)
#define PROCEDURALSTATUS_TERMINATING		(1<<4)

#define CONGESTIONLEVEL_NONE			0
#define CONGESTIONLEVEL_CONGESTIONLEVEL1	1
#define CONGESTIONLEVEL_CONGESTIONLEVEL2	2
#define CONGESTIONLEVEL_CONGESTIONLEVEL3	3

#define ALARMSTATUS_UNDERREPAIR			(1<<0)
#define ALARMSTATUS_CRITICAL			(1<<1)
#define ALARMSTATUS_MAJOR			(1<<2)
#define ALARMSTATUS_MINOR			(1<<3)
#define ALARMSTATUS_ALARMOUTSTANDING		(1<<4)

#define AVAILABILITYSTATUS_INTEST		(1<<0)
#define AVAILABILITYSTATUS_FAILED		(1<<1)
#define AVAILABILITYSTATUS_POWEROFF		(1<<2)
#define AVAILABILITYSTATUS_OFFLINE		(1<<3)
#define AVAILABILITYSTATUS_OFFDUTY		(1<<4)
#define AVAILABILITYSTATUS_DEPENDENCY		(1<<5)
#define AVAILABILITYSTATUS_DEGRADED		(1<<6)
#define AVAILABILITYSTATUS_NOTINSTALLED		(1<<7)
#define AVAILABILITYSTATUS_LOGFULL		(1<<8)

#define STANDBYSTATUS_HOTSTANDBY		0
#define STANDBYSTATUS_COLDSTANDBY		1
#define STANDBYSTATUS_PROVIDINGSERVICE		2

/*
 * SIGNALLING DATA LINK STATE
 */
typedef struct mtp_statem_sd {
	uint mtpSdlOperationalState;
} mtp_statem_sd_t;

/*
 * SIGNALLING DATA TERMINAL STATE
 */
typedef struct mtp_statem_st {
	uint mtpSdtAdministrativeState;
	uint mtpSdtOperationalState;
	uint mtpSdtUsageState;
	uint mtpSdtProceduralStatus;
	uint mtpSdtAvailabilityStatus;
	uint mtpSdtStandbyStatus;
} mtp_statem_st_t;

/*
 * SIGNALLING LINK STATE
 */
typedef struct mtp_statem_sl {
	uint mtpSlSlsCodeCurrentList[8];
	uint mtpSlMaxCapacitySL;
	uint mtpSlCurrentCapacitySL;
	uint mtpSlLinkTpStatus;
	uint mtpSlAdministrativeState;
	uint mtpSlOperationalState;
	uint mtpSlUsageState;
	uint mtpSlProceduralStatus;
	uint mtpSlCongestionLevel;
	uint mtpSlAlarmStatus;
} mtp_statem_sl_t;

/*
 * SIGNALLING LINK SET STATE
 */
typedef struct mtp_statem_ls {
	uint mtpLsCurrentCapacity;
	uint mtpLsUsageState;
	uint mtpLsOperationalState;
	uint mtpLsAvailabilityStatus;
	uint mtpLsAdministrativeState;
	uint mtpLsAlarmStatus;
} mtp_statem_ls_t;

/*
 * SIGNALLING ROUTE STATE
 */
typedef struct mtp_statem_rt {
	uint mtpRtAdministrativeState;
	uint mtpRtOperationalState;
	uint mtpRtAvailabilityStatus;
	uint mtpRtSlsList[8];
	uint mtpRtUsageState;
} mtp_statem_rt_t;

/*
 * SIGNALLING ROUTE SET STATE
 */
typedef struct mtp_statem_rs {
	uint mtpRsAdministrativeState;
	uint mtpRsOperationalState;
	uint mtpRsCongestedState;
	uint mtpRsCongestionLevel;
	uint mtpRsAlarmStatus;
} mtp_statem_rs_t;

/*
 * MTP LEVEL 3 STATE
 */
typedef struct mtp_statem_l3 {
	uint mtpL3AdministrativeState;
	uint mtpL3OperationalState;
	uint mtpL3UsageState;
	uint mtpL3ProceduralStatus;
} mtp_statem_l3_t;

/*
 * SIGNALLING POINT STATE
 */
typedef struct mtp_statem_sp {
	uint mtpSpOperationalState;
	uint mtpSpProceduralStatus;
	uint mtpSpAvailabilityStatus;
	uint mtpSpAlarmStatus;
} mtp_statem_sp_t;

/*
 * MTP-SAP STATE
 */
typedef struct mtp_statem_sa {
	uint mtpSapUsageState;
} mtp_statem_sa_t;

/*
 * MANAGED SWITCHING ELEMENT (DEFAULT) STATE
 */
typedef struct mtp_statem_ms {
	uint mtpMsAlarmStatus;
	uint mtpMsOperationalState;
	uint mtpMsUsageState;
} mtp_statem_ms_t;

#endif				/* __MTP2_IOCTL_H__ */
