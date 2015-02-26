/*****************************************************************************

 @(#) src/include/sys/nlm_ioctl.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

#ifndef __NLM_IOCTL_H__
#define __NLM_IOCTL_H__

/* This file can be processed by doxygen(1). */

#include <linux/ioctl.h>

#define NLM_IOC_MAGIC '2'

#define NLM_OBJ_TYPE_DF		 0	/* default */
#define NLM_OBJ_TYPE_NP		 1	/* NLS User */
#define NLM_OBJ_TYPE_NE		 2	/* Network Entity */
#define NLM_OBJ_TYPE_CN		 3	/* Network Connection */
#define NLM_OBJ_TYPE_XP		 4	/* X.25 PLE Profile */
#define NLM_OBJ_TYPE_VC		 5	/* Virtual Call */
#define NLM_OBJ_TYPE_VP		 6	/* Virtual Call Profile */
#define NLM_OBJ_TYPE_DL		 7	/* Data link */
#define NLM_OBJ_TYPE_AJ		 8	/* ISIS Adjacency */
#define NLM_OBJ_TYPE_VA		 9	/* ISIS Virtual Adjacency */
#define NLM_OBJ_TYPE_DS		10	/* ISIS Destination System */
#define NLM_OBJ_TYPE_DA		11	/* ISIS Destination Area */
#define NLM_OBJ_TYPE_RA		12	/* ISIS Reachable Address */

/*
 * INFORMATION
 *
 * Provides read-only information that is not available for the manager to
 * configure.  Also, this information does not change while the object exists:
 * it is purely static information.
 */
struct nlm_info_df {
};
struct nlm_info_np {
};
struct nlm_info_ne {
};
struct nlm_info_cn {
};
struct nlm_info_xp {
};
struct nlm_info_vc {
};
struct nlm_info_vp {
};
struct nlm_info_dl {
#define NLM_CIRCUITTYPE_BROADCAST	0
#define NLM_CIRCUITTYPE_PTTOPT		1
#define NLM_CIRCUITTYPE_STATICIN	2
#define NLM_CIRCUITTYPE_STATICOUT	3
#define NLM_CIRCUITTYPE_DA		4
	uint type;
};
struct nlm_info_aj {
};
struct nlm_info_va {
};
struct nlm_info_ds {
};
struct nlm_info_da {
};
struct nlm_info_ra {
};

union nlm_info_obj {
	struct nlm_info_df df;
	struct nlm_info_np np;
	struct nlm_info_ne ne;
	struct nlm_info_cn cn;
	struct nlm_info_xp xp;
	struct nlm_info_vc vc;
	struct nlm_info_vp vp;
	struct nlm_info_dl dl;
	struct nlm_info_aj aj;
	struct nlm_info_va va;
	struct nlm_info_ds ds;
	struct nlm_info_da da;
	struct nlm_info_ra ra;
};
typedef struct nlm_info {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union nlm_info_obj info[0];
} nlm_info_t;

#define NLM_IOCGINFO	_IOWR(NLM_IOC_MAGIC,	 0, nlm_info_t)	/* get */

/*
 * OPTIONS
 *
 * Provides options not necessary for configuration but that can be changed
 * while the object exists without reconfiguration of the object (that is, a
 * change to these items does not change the relationship between the object
 * changed and other objects).
 *
 * Note that one of these options structures can optionally be appended to a
 * configuration structure when an object is created or moved.
 */
struct nlm_opt_conf_df {
};
struct nlm_opt_conf_np {
};

#define NLM_CLNS_ENABLECHECKSUM				(1<<0)

struct nlm_opt_conf_ne {
	uint32_t flags;
#define NLM_SYSTEMTYPES_ES	(1<<1)
#define NLM_SYSTEMTYPES_IS	(1<<2)
	uint systemTypes;
#define NLM_SYSTEMTYPE_ES	1
#define NLM_SYSTEMTYPE_IS	2
	struct {
		uint32_t flags;
		uint operationalState;
		uint administrativeState;
		uint operationalSystemType;
#define NLM_ISTYPE_LEVEL1IS	1
#define NLM_ISTYPE_LEVEL2IS	2
		uint iSType;
		uint maximumLifetime;
		uint maximumPathSplits;
		uint originatingL1BufferSize;
		uint maximumAreaAddresses;
		uint maximumVirtualAdjacencies;
		uint originatingL2LSPBufferSize;
	} clns;
	struct {
		uint operationalSystemType;
	} cons;
};
struct nlm_opt_conf_cn {
};
struct nlm_opt_conf_xp {
};
struct nlm_opt_conf_vc {
	struct {
		struct {
			uint32_t flags;
		} pvc;
		struct {
			uint32_t flags;
		} svc;
	} dte;
	struct {
		struct {
#define NLM_DEC_PVC_CHARGINGDIRECTION			(1<<0)
			uint32_t flags;
		} pvc;
		struct {
			uint32_t flags;
		} svc;
	} dce;
};
struct nlm_opt_conf_vp {
	uint32_t defaultPacketSizeIncoming;
	uint32_t defaultPacketSizeOutgoing;
	uint32_t defaultThroughputClassIncoming;
	uint32_t defaultThroughputClassOutgoing;
	uint32_t defaultWindowSizeIncoming;
	uint32_t defaultWindowSizeOutgoing;
	struct {
		uint32_t flags;
	} pvc;
	struct {
#define NLM_SVC_REVERSECHARGING				(1<<0)
		uint32_t flags;
		uint logicalChannelId;
	} svc;
	struct {
		uint32_t maxActiveCircuits;
		uint32_t clearRequestRetransmissionCount;
		uint32_t extendedPacketSequenceNumbering;
		uint32_t maxActiveCircuits;
		uint32_t resetRequestRetransmissionCount;
		uint32_t restartRequestRetransmissionCount;
		uint32_t dataPacketRetransmissionCount;
		uint32_t rejectRetransmissionCount;
		struct {
			uint32_t flags;
		} pvc;
		struct {
#define NLM_DTE_SVC_REVERSECHARGING			(1<<0)
			uint32_t flags;
			uint32_t throughputClassIncoming;
			uint32_t throughputClassOutgoing;
		} svc;
	} dte;
	struct {
		struct {
			uint32_t flags;
		} pvc;
		struct {
#define NLM_DCE_SVC_CHARGINGDIRECTION				(1<<0)
#define NLM_DCE_SVC_CUGSELECTION				(1<<1)
#define NLM_DCE_SVC_TRANSITDELAYSELECTIONANDINDICATION		(1<<2)
#define NLM_DCE_SVC_BILATERALCUGSELECTION			(1<<3)
#define NLM_DCE_SVC_CALLREDIRECTIONDEFLECTIONNOTIFICATION	(1<<4)
#define NLM_DCE_SVC_CALLEDLINEADRESSMODIFIEDNOTIFICATION	(1<<5)
#define NLM_DCE_SVC_CUGWITHOUTGOINGACCESSSELECTION		(1<<6)
#define NLM_DCE_SVC_NUISELECTION				(1<<7)
#define NLM_DCE_SVC_REVERSECHARGING				(1<<8)
#define NLM_DCE_SVC_ROASELECTION				(1<<9)
			uint32_t flags;
		} svc;
	} dce;
};
struct nlm_opt_conf_dl {
#define NLM_DL_ENABLECHECKSUM				(1<<0)
#define NLM_DL_EXTERNALDOMAIN				(1<<1)
#define NLM_DL_MANUALL2ONLYMODE				(1<<2)
	uint32_t flags;
	uint8_t l1DefaultMetric;
	uint8_t l1DelayMetric;
	uint8_t l1ExpenseMetric;
	uint8_t l1ErrorMetric;
	uint8_t callEstablishmentDefaultMetricIncrement;
	uint8_t callEstablishmentDelayMetricIncrement;
	uint8_t callEstablishmentExpenseMetricIncrement;
	uint8_t callEstablishmentErrorMetricIncrement;
	uint8_t l2DefaultMetric;
	uint8_t l2DelayMetric;
	uint8_t l2ExpenseMetric;
	uint8_t l2ErrorMetric;
	struct {
#define NLM_X25_FLOWCONTROLPARAMETERNEGOTIATION		(1<<0)
#define NLM_X25_THROUGHPUTCLASSNEGOTIATION		(1<<1)
		uint flags;
		uint defaultThroughputClassIncoming;
		uint defaultThroughputClassOutgoing;
		uint defaultPacketSizeIncoming;
		uint defaultPacketSizeOutgoing;
		uint defaultWindowSizeIncoming;
		uint defaultWindowSizeOutgoing;
		struct {
#define NLM_DTE_CALLDEFLECTIONSUBSCRIPTION		(1<<0)
#define NLM_DTE_REGISTRATIONPERMITTED			(1<<1)
			uint32_t flags;
			uint32_t restartRequestRetransmissionCount;
			uint32_t resetRequestRetransmissionCount;
			uint32_t clearRequestRetransmissionCount;
			uint32_t dataPacketRetransmissionCount;
			uint32_t rejectRetransmissionCount;
			uint32_t registrationRequestRetransmissionCount;
			struct {
				uint8_t redirectReason;
			} svc;
		} dte;
		struct {
#define NLM_DCE_CUG					(1<<0)
#define NLM_DCE_FASTSELECTACCEPTANCE			(1<<1)
#define NLM_DCE_INCOMINGCALLSBARRED			(1<<2)
#define NLM_DCE_ONEWAYLOGICALCHANNELOUTGOING		(1<<3)
#define NLM_DCE_OUTGOINGCALLSBARRED			(1<<4)
#define NLM_DCE_BILATERALCUG				(1<<5)
#define NLM_DCE_BILATERALCUGWITHOUTGOINGACCESS		(1<<6)
#define NLM_DCE_CALLDEFLECTIONSUBSCRIPTION		(1<<7)
#define NLM_DCE_CALLREDIRECTION				(1<<8)
#define NLM_DCE_CHARGINGINFORMATION			(1<<9)
#define NLM_DCE_CUGWITHINCOMINGACCESS			(1<<10)
#define NLM_DCE_CUGWITHOUTGOINGACCESS			(1<<11)
#define NLM_DCE_DBITMODIFICATION			(1<<12)
#define NLM_DCE_DEFAULTTHROUGHPUTCLASSESASSIGNMENT	(1<<13)
#define NLM_DCE_HUNTGROUP				(1<<14)
#define NLM_DCE_INCOMINGCALLBARREDWITHINCUG		(1<<15)
#define NLM_DCE_LOCALCHARGINGPREVENTION			(1<<16)
#define NLM_DCE_NONSTANDARDDEFAULTPACKETSIZES		(1<<17)
#define NLM_DCE_NONSTANDARDDEFAULTWINDOWSIZES		(1<<18)
#define NLM_DCE_NUIOVERRIDE				(1<<19)
#define NLM_DCE_NUISUBSCRIPTION				(1<<20)
#define NLM_DCE_ONEWAYLOGICALCHANNELINCOMING		(1<<21)
#define NLM_DCE_ONLINEFACILITYREGISTRATION		(1<<22)
#define NLM_DCE_OUTGOINGCALLBARREDWITHINCUG		(1<<23)
#define NLM_DCE_PACKETRETRANSMISSION			(1<<24)
#define NLM_DCE_REVERSECHARGINGACCEPTANCE		(1<<25)
#define NLM_DCE_ROASUBSCRIPTION				(1<<26)
			uint32_t flags;
			struct {
				uint remoteLogicalChannel;
			} pvc;
		} dce;
	} x25;
};
struct nlm_opt_conf_aj {
};
struct nlm_opt_conf_va {
};
struct nlm_opt_conf_ds {
};
struct nlm_opt_conf_da {
};
struct nlm_opt_conf_ra {
};

union nlm_option_obj {
	struct nlm_opt_conf_df df;
	struct nlm_opt_conf_np np;
	struct nlm_opt_conf_ne ne;
	struct nlm_opt_conf_cn cn;
	struct nlm_opt_conf_xp xp;
	struct nlm_opt_conf_vc vc;
	struct nlm_opt_conf_vp vp;
	struct nlm_opt_conf_dl dl;
	struct nlm_opt_conf_aj aj;
	struct nlm_opt_conf_va va;
	struct nlm_opt_conf_ds ds;
	struct nlm_opt_conf_da da;
	struct nlm_opt_conf_ra ra;
};
typedef struct nlm_option {
	uint type;			/* object type */
	uint id;			/* object identifier */
	/* followed by object-specific structure */
	struct nlm_option_obj option[0];
} nlm_option_t;

#define NLM_IOCGOPTION	_IOWR(NLM_IOC_MAGIC,	 0, nlm_option_t)	/* get */
#define NLM_IOCSOPTION	_IOWR(NLM_IOC_MAGIC,	 1, nlm_option_t)	/* set */

/*
 * CONFIGURATION
 *
 * Configures the object (creates, destroys, moves).  These actions require
 * reconfiguration of the object and a change in its relationship to other
 * objects.  Also, this structure contains information that must be specified
 * during creation of the object.  Each structure can optionally be followed by
 * an options structure to specify the options during creation.
 */
struct nlm_conf_df {
};
struct nlm_conf_np {
};
struct nlm_conf_ne {
};
struct nlm_conf_cn {
};
struct nlm_conf_xp {
};
struct nlm_conf_vc {
};
struct nlm_conf_vp {
};
struct nlm_conf_dl {
};
struct nlm_conf_aj {
};
struct nlm_conf_va {
};
struct nlm_conf_ds {
};
struct nlm_conf_da {
};
struct nlm_conf_ra {
};

union nlm_conf_obj {
	struct nlm_conf_df df;
	struct nlm_conf_np np;
	struct nlm_conf_ne ne;
	struct nlm_conf_cn cn;
	struct nlm_conf_xp xp;
	struct nlm_conf_vc vc;
	struct nlm_conf_vp vp;
	struct nlm_conf_dl dl;
	struct nlm_conf_aj aj;
	struct nlm_conf_va va;
	struct nlm_conf_ds ds;
	struct nlm_conf_da da;
	struct nlm_conf_ra ra;
};

typedef struct nlm_config {
	np_ulong type;			/* object type */
	np_ulong id;			/* object id */
	np_ulong cmd;			/* configuration command */
	/* followed by object-specific configuration structure */
	nlm_conf_obj_t config[0];
} nlm_config_t;

#define NLM_GET		0	/* get configuration */
#define NLM_ADD		1	/* add configuration */
#define NLM_CHA		2	/* cha configuration */
#define NLM_DEL		3	/* del configuration */
#define NLM_TST		4	/* tst configuration */
#define NLM_COM		5	/* com configuration */

#define NLM_IOCLCONFIG	_IOWR(NLM_IOC_MAGIC,	 3, nlm_config_t)	/* list */
#define NLM_IOCGCONFIG	_IOWR(NLM_IOC_MAGIC,	 4, nlm_config_t)	/* get */
#define NLM_IOCSCONFIG	_IOWR(NLM_IOC_MAGIC,	 5, nlm_config_t)	/* set */
#define NLM_IOCTCONFIG	_IOWR(NLM_IOC_MAGIC,	 6, nlm_config_t)	/* test */
#define NLM_IOCCCONFIG	_IOWR(NLM_IOC_MAGIC,	 7, nlm_config_t)	/* commit */

/*
 * STATE
 *
 * Provides state machine state information.  The purpose of these items is
 * primarily for debugging: to view the internal state of the state machine.
 * Some of these state variables might also be reflected in status fields.
 */
struct nlm_timers_df {
};
struct nlm_timers_np {
};
struct nlm_timers_ne {
	struct {
		uint32_t nlmCLNSminimumLSPTransmissionInterval;
		uint32_t nlmCLNSmaximumLSPGenerationInterval;
		uint32_t nlmCLNSminimumBroadcastLSPTransmissionInterval;
		uint32_t nlmCLNScompleteSNPInterval;
		uint32_t nlmCLNSminimumLSPGenerationInterval;
		uint32_t nlmCLNSpollESHelloRate;
		uint32_t nlmCLNSpartialSNPInterval;
		uint32_t nlmCLNSwaitingTime;
		uint32_t nlmCLNSdRISISHelloTimer;
	} clns;
};
struct nlm_timers_cn {
};
struct nlm_timers_xp {
	struct {
		uint32_t callRequestResponseTimer;
		uint32_t clearRequestResponseTimer;
		uint32_t interruptResponseTimer;
		uint32_t minimumRecallTimer;
		uint32_t registrationRequestResponseTimer;
		uint32_t rejectResponseTimer;
		uint32_t resetRequestResponseTimer;
		uint32_t restartRequestResponseTimer;
		uint32_t windowRotationTimer;
		uint32_t windowStatusTransmissionTimer;
	} dte;
	struct {
	} dce;
};
struct nlm_timers_vc {
};
struct nlm_timers_vp {
};
struct nlm_timers_dl {
	uint32_t activeESConfigTimer;
	uint32_t defaultESConfigTimer;
	uint32_t idleTimer;
	uint32_t initialMinimumTimer;
	uint32_t iSConfigurationTimer;
	uint32_t iSISHelloTimer;
	uint32_t reserveTimer;
	uint32_t suggestedESConfigurationTimer;
	struct {
		uint32_t callRequestResponseTimer;
		uint32_t clearRequestResponseTimer;
		uint32_t interruptResponseTimer;
		uint32_t minimumRecallTimer;
		uint32_t registrationRequestResponseTimer;
		uint32_t rejectResponseTimer;
		uint32_t resetRequestResponseTimer;
		uint32_t restartRequestResponseTimer;
		uint32_t windowRotationTimer;
		uint32_t windowStatusTransmissionTimer;
	} dte;
	struct {
		uint32_t clearIndication;
		uint32_t incomingCall;
		uint32_t resetIndication;
		uint32_t restartIndication;
	} dce;
};
struct nlm_timers_aj {
};
struct nlm_timers_va {
};
struct nlm_timers_ds {
};
struct nlm_timers_da {
};
struct nlm_timers_ra {
};

union nlm_statm_obj {
	struct nlm_statm_df df;
	struct nlm_statm_np np;
	struct nlm_statm_ne ne;
	struct nlm_statm_cn cn;
	struct nlm_statm_xp xp;
	struct nlm_statm_vc vc;
	struct nlm_statm_vp vp;
	struct nlm_statm_dl dl;
	struct nlm_statm_aj aj;
	struct nlm_statm_va va;
	struct nlm_statm_ds ds;
	struct nlm_statm_da da;
	struct nlm_statm_ra ra;
};

typedef struct nlm_statem {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union nlm_statem_obj statem[0];
} nlm_statem_t;

#define NLM_IOCGSTATEM	_IOWR(NLM_IOC_MAGIC,	 8, nlm_statem_t)	/* get */
#define NLM_IOCCMRESET	_IOWR(NLM_IOC_MAGIC,	 9, nlm_statem_t)	/* master reset */

/*
 * STATUS
 *
 * Provides status on the object.  Many of these are X.721 status fields or
 * object-specific status information.  Only read-write status fields may be
 * altered with a set or clear command.
 */
struct nlm_status_df {
};
struct nlm_status_np {
};
struct nlm_status_ne {
	uint operationalState;
	struct {
		uint operationalState;
		uint administrativeState;
		uint systemTypes;
		uint supportedProtocols;
	} clns;
	struct {
		uint operationalState;
		uint administrativeState;
		uint systemTypes;
	} cons;
};
struct nlm_status_cn {
};
struct nlm_status_xp {
};
struct nlm_status_vc {
	uint operationalState;
};
struct nlm_status_vp {
};
struct nlm_status_dl {
	uint operationalState;
	uint administrativeState;
};
struct nlm_status_aj {
};
struct nlm_status_va {
};
struct nlm_status_ds {
};
struct nlm_status_da {
};
struct nlm_status_ra {
};
union nlm_status_obj {
	struct nlm_status_df df;
	struct nlm_status_np np;
	struct nlm_status_ne ne;
	struct nlm_status_cn cn;
	struct nlm_status_xp xp;
	struct nlm_status_vc vc;
	struct nlm_status_vp vp;
	struct nlm_status_dl dl;
	struct nlm_status_aj aj;
	struct nlm_status_va va;
	struct nlm_status_ds ds;
	struct nlm_status_da da;
	struct nlm_status_ra ra;
};
typedef struct nlm_status {
	uint type;			/* object type */
	uint id;			/* object identifier */
	/* followed by object-specific structure */
	union nlm_status_obj status[0];
} nlm_status_t;

#define NLM_IOCGSTATUS	_IOWR(NLM_IOC_MAGIC,	10, nlm_status_t)	/* get */
#define NLM_IOCSSTATUS	_IOWR(NLM_IOC_MAGIC,	11, nlm_status_t)	/* set */
#define NLM_IOCCSTATUS	_IOWR(NLM_IOC_MAGIC,	12, nlm_status_t)	/* clear */

/*
 * STATISTICS
 */
struct nlm_stats_df {
};
struct nlm_stats_np {
};
struct nlm_stats_ne {
	struct {
		uint64_t totalRemoteSAPs;
		uint64_t octetsSentCounter;
		uint64_t octetsReceivedCounter;
		uint64_t segmentsReceived;
		uint64_t segmentsSent;
		uint64_t segmentsDiscarded;
		uint64_t assemblingSegmentsDiscarded;
		uint64_t errorReportsReceived;
		uint64_t pDUDiscards;
		uint64_t congestionDiscards;
		uint64_t corruptedLSPsDetected;
		uint64_t lSPL1DatabaseOverloads;
		uint64_t manualAddressesDroppedFromArea;
		uint64_t attemptsToExceedMaximumSequenceNumber;
		uint64_t sequenceNumberSkips;
		uint64_t ownLSPPurges;
		uint64_t iDFieldLengthMismatches;
		uint64_t maximumAreaAddressesMismatches;
		uint64_t originatingLSPBufferSizeMismatches;
		uint64_t lSPTooLargeToPropagate;
		uint64_t authenticationFailures;
		uint64_t partitionVirtualLinkChanges;
		uint64_t lSPL2DatabaseOverloads;
	} clns;
	struct {
	} cons;
};
struct nlm_stats_cn {
};
struct nlm_stats_xp {
};
struct nlm_stats_vc {
	struct {
		uint64_t octetsSentCounter;
		uint64_t octetsReceivedCounter;
		uint64_t dataPacketsReceived;
		uint64_t dataPacketsSent;
		uint64_t interruptTimerExpiries;
		uint64_t providerInitiatedResets;
		uint64_t remotelyInitiatedResets;
		uint64_t resetTimeouts;
	} dte;
	struct {
		uint64_t dataPacketsReceived;
		uint64_t dataPacketsSent;
		uint64_t interruptPacketsReceived;
		uint64_t interruptPacketsSent;
		uint64_t interruptTimerExpiries;
		uint64_t octetsReceivedCounter;
		uint64_t octetsSentCounter;
		uint64_t providerInitiatedDisconnects;
		uint64_t providerInitiatedResets;
		uint64_t remotelyInitiatedRestarts;
		uint64_t remotelyInitiatedResets;
		uint64_t resetTimeouts;
		uint64_t x25SegmentsReceived;
		uint64_t x25SegmentsSent;
	} dce;
	struct {
		uint64_t resetRequestIndicationPackets;
		uint64_t segmentsSent;
		uint64_t segmentsReceived;
	} dseries;
};
struct nlm_stats_vp {
};
struct nlm_stats_dl {
	struct {
		uint64_t eSReachabilityChanges;
		uint64_t invalid9542PDUs;
		uint64_t iSReachabilityChanges;
		uint64_t callsPlaced;
		uint64_t callsFailed;
		uint64_t changeInAdjacencyState;
		uint64_t initialisationFailures;
		uint64_t rejectedAdjacencies;
		uint64_t iSISControlPDUsSent;
		uint64_t iSISControlPDUsReceived;
		uint64_t iDFieldLengthMistmatches;
		uint64_t maximumAreaAddressesMismatches;
		uint64_t linkageAuthenticationFailures;
		uint64_t lanL1DesignatedIntermediateSystemChanges;
		uint64_t lanL2DesignatedIntermediateSystemChanges;
	} linkage;
	struct {
		uint64_t callAttempts;
		uint64_t protocolErrorsDetectedLocally;
		uint64_t protocolErrorsAccusedOf;
		uint64_t callEstablishmentRetryCountsExceeded;
		uint64_t octetsReceivedCounter;
		uint64_t octetsSentCounter;
		uint64_t callTimeouts;
		uint64_t callsConnected;
		uint64_t clearCountsExceeded;
		uint64_t clearTimeouts;
		uint64_t dataPacketsReceived;
		uint64_t dataPacketsSent;
		uint64_t dataRetransmissionTimerExpiries;
		uint64_t providerInitiatedResets;
		uint64_t providerInitiatedDisconnects;
		uint64_t remotelyInitiatedResets;
		uint64_t remotelyInitiatedRestarts;
		uint64_t resetTimeouts;
		uint64_t restartCountsExceeded;
	} dte;
	struct {
		uint64_t callAttempts;
		uint64_t callsConnected;
		uint64_t dataPacketsReceived;
		uint64_t dataPacketsSent;
		uint64_t interruptPacketsReceived;
		uint64_t interruptPacketsSent;
		uint64_t interruptTimerExpiries;
		uint64_t octetsReceivedCounter;
		uint64_t octetsSentCounter;
		uint64_t providerInitiatedDisconnects;
		uint64_t providerInitiatedResets;
		uint64_t remotelyInitiatedRestarts;
		uint64_t remotelyInitiatedResets;
		uint64_t resetTimeouts;
		uint64_t x25SegmentsReceived;
		uint64_t x25SegmentsSent;
	} dce;
};
struct nlm_stats_aj {
};
struct nlm_stats_va {
};
struct nlm_stats_ds {
};
struct nlm_stats_da {
};
struct nlm_stats_ra {
};
union nlm_stats_obj {
	struct nlm_stats_df df;
	struct nlm_stats_np np;
	struct nlm_stats_ne ne;
	struct nlm_stats_cn cn;
	struct nlm_stats_xp xp;
	struct nlm_stats_vc vc;
	struct nlm_stats_vp vp;
	struct nlm_stats_dl dl;
	struct nlm_stats_aj aj;
	struct nlm_stats_va va;
	struct nlm_stats_ds ds;
	struct nlm_stats_da da;
	struct nlm_stats_ra ra;
};
typedef struct nlm_stats {
	uint type;			/* object type */
	uint id;			/* object id */
	uint interval;			/* time interval */
	uint time;			/* time period */
	uint stamp;			/* time stamp */
	/* followed by object-specific structure */
	union nlm_stats_obj stats[0];
} nlm_stats_t;

#define NLM_IOCGSTATSP	_IOWR(NLM_IOC_MAGIC,	 8,  nlm_stats_t)	/* get period */
#define NLM_IOCSSTATSP	_IOWR(NLM_IOC_MAGIC,	 9,  nlm_stats_t)	/* set period */
#define NLM_IOCGSTATS	_IOWR(NLM_IOC_MAGIC,	10,  nlm_stats_t)	/* get */
#define NLM_IOCCSTATS	_IOWR(NLM_IOC_MAGIC,	11,  nlm_stats_t)	/* get and clear */

/*
 * EVENTS
 *
 * Provides a mechanism for requesting event notifications.  The flags field in
 * the header is used to requesting X.721 events (e.g. object creation
 * notification).  Events can be requested by object class, and a mechanism
 * exists for requesting events by specific object; however, the driver is not
 * required to support per-object notification and will return EOPNOTSUPP.  To
 * request notification for an object class, specify zero (0) for the
 * object identifier.
 *
 * Event requests are per requesting Stream.  Non-management streams should only
 * be notified of events for objects associated with the Stream and not X.721
 * events unless also specified as a specific event (e.g. communications alarm
 * events).  Management Streams can be notified of any object.  Notifications
 * are provided using the LMI_EVENT_IND (event indication) for X.721 events, and
 * the NLM_EVENT_IND (for other events).
 */
struct nlm_notify_df {
	uint events;
} nlm_notify_df_t;
struct nlm_notify_np {
	uint events;
} nlm_notify_np_t;
struct nlm_notify_ne {
	uint events;
} nlm_notify_ne_t;
struct nlm_notify_cn {
	uint events;
} nlm_notify_cn_t;
struct nlm_notify_xp {
	uint events;
} nlm_notify_xp_t;
struct nlm_notify_vc {
	uint events;
} nlm_notify_vc_t;
struct nlm_notify_vp {
	uint events;
} nlm_notify_vp_t;
struct nlm_notify_dl {
	uint events;
} nlm_notify_dl_t;
struct nlm_notify_aj {
	uint events;
} nlm_notify_aj_t;
struct nlm_notify_va {
	uint events;
} nlm_notify_va_t;
struct nlm_notify_ds {
	uint events;
} nlm_notify_ds_t;
struct nlm_notify_da {
	uint events;
} nlm_notify_da_t;
struct nlm_notify_ra {
	uint events;
} nlm_notify_ra_t;
union nlm_notify_obj {
	struct nlm_notify_df df;
	struct nlm_notify_np np;
	struct nlm_notify_ne ne;
	struct nlm_notify_cn cn;
	struct nlm_notify_xp xp;
	struct nlm_notify_vc vc;
	struct nlm_notify_vp vp;
	struct nlm_notify_dl dl;
	struct nlm_notify_aj aj;
	struct nlm_notify_va va;
	struct nlm_notify_ds ds;
	struct nlm_notify_da da;
	struct nlm_notify_ra ra;
} nlm_notify_obj_t;
typedef struct nlm_notify {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint flags;			/* X.721 flags */
	/* followed by object-specific structure */
	union nlm_notify_obj events[0];
} nlm_notify_t
#define NLM_IOCGNOTIFY	_IOWR(NLM_IOC_MAGIC, 17,  nlm_notify_t)	/* get */
#define NLM_IOCSNOTIFY	_IOWR(NLM_IOC_MAGIC, 18,  nlm_notify_t)	/* set */
#define NLM_IOCCNOTIFY	_IOWR(NLM_IOC_MAGIC, 19,  nlm_notify_t)	/* clear */
/*
 * ATTRIBUTES
 *
 * Provides a structure that contains all of the other information structures
 * pertaining to an object.  This is the total view of the object.  This 
 * structure can only be read and cannot be used to reconfigure or change
 * options associated with an object.  This structure is primarily for the
 * convenience of management agents so that it is possible to atomically read
 * all of the attributes associated with an object.
 */
    struct nlm_attr_df {
	struct nlm_conf_df config;
	struct nlm_opt_conf_df option;
	struct nlm_info_df inform;
	struct nlm_statem_df statem;
	struct nlm_status_df status;
	struct nlm_stats_df stats;
	struct nlm_notify_df events;
};
struct nlm_attr_np {
	struct nlm_conf_np config;
	struct nlm_opt_conf_np option;
	struct nlm_info_np inform;
	struct nlm_statem_np statem;
	struct nlm_status_np status;
	struct nlm_stats_np stats;
	struct nlm_notify_np events;
};
struct nlm_attr_ne {
	struct nlm_conf_ne config;
	struct nlm_opt_conf_ne option;
	struct nlm_info_ne inform;
	struct nlm_statem_ne statem;
	struct nlm_status_ne status;
	struct nlm_stats_ne stats;
	struct nlm_notify_ne events;
};
struct nlm_attr_cn {
	struct nlm_conf_cn config;
	struct nlm_opt_conf_cn option;
	struct nlm_info_cn inform;
	struct nlm_statem_cn statem;
	struct nlm_status_cn status;
	struct nlm_stats_cn stats;
	struct nlm_notify_cn events;
};
struct nlm_attr_xp {
	struct nlm_conf_xp config;
	struct nlm_opt_conf_xp option;
	struct nlm_info_xp inform;
	struct nlm_statem_xp statem;
	struct nlm_status_xp status;
	struct nlm_stats_xp stats;
	struct nlm_notify_xp events;
};
struct nlm_attr_vc {
	struct nlm_conf_vc config;
	struct nlm_opt_conf_vc option;
	struct nlm_info_vc inform;
	struct nlm_statem_vc statem;
	struct nlm_status_vc status;
	struct nlm_stats_vc stats;
	struct nlm_notify_vc events;
};
struct nlm_attr_vp {
	struct nlm_conf_vp config;
	struct nlm_opt_conf_vp option;
	struct nlm_info_vp inform;
	struct nlm_statem_vp statem;
	struct nlm_status_vp status;
	struct nlm_stats_vp stats;
	struct nlm_notify_vp events;
};
struct nlm_attr_dl {
	struct nlm_conf_dl config;
	struct nlm_opt_conf_dl option;
	struct nlm_info_dl inform;
	struct nlm_statem_dl statem;
	struct nlm_status_dl status;
	struct nlm_stats_dl stats;
	struct nlm_notify_dl events;
};
struct nlm_attr_aj {
	struct nlm_conf_aj config;
	struct nlm_opt_conf_aj option;
	struct nlm_info_aj inform;
	struct nlm_statem_aj statem;
	struct nlm_status_aj status;
	struct nlm_stats_aj stats;
	struct nlm_notify_aj events;
};
struct nlm_attr_va {
	struct nlm_conf_va config;
	struct nlm_opt_conf_va option;
	struct nlm_info_va inform;
	struct nlm_statem_va statem;
	struct nlm_status_va status;
	struct nlm_stats_va stats;
	struct nlm_notify_va events;
};
struct nlm_attr_ds {
	struct nlm_conf_ds config;
	struct nlm_opt_conf_ds option;
	struct nlm_info_ds inform;
	struct nlm_statem_ds statem;
	struct nlm_status_ds status;
	struct nlm_stats_ds stats;
	struct nlm_notify_ds events;
};
struct nlm_attr_da {
	struct nlm_conf_da config;
	struct nlm_opt_conf_da option;
	struct nlm_info_da inform;
	struct nlm_statem_da statem;
	struct nlm_status_da status;
	struct nlm_stats_da stats;
	struct nlm_notify_da events;
};
struct nlm_attr_ra {
	struct nlm_conf_ra config;
	struct nlm_opt_conf_ra option;
	struct nlm_info_ra inform;
	struct nlm_statem_ra statem;
	struct nlm_status_ra status;
	struct nlm_stats_ra stats;
	struct nlm_notify_ra events;
};
typedef struct nlm_attr {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union nlm_attr_obj attrs[0];
} nlm_attr_t;

#define	MTP_IOCGATTR	_IOWR(MTP_IOC_MAGIC,	20, mtp_attr_t)	/* get attributes */

/*
 *  MANAGEMENT
 *
 * Provides specific and defined management actions that can be affected on
 * objects.
 */
struct nlm_action_df {
};
struct nlm_action_np {
};
struct nlm_action_ne {
};
struct nlm_action_cn {
};
struct nlm_action_xp {
};
struct nlm_action_vc {
};
struct nlm_action_vp {
};
struct nlm_action_dl {
};
struct nlm_action_aj {
};
struct nlm_action_va {
};
struct nlm_action_ds {
};
struct nlm_action_da {
};
struct nlm_action_ra {
};
union nlm_action_obj {
	struct nlm_action_df df;
	struct nlm_action_np np;
	struct nlm_action_ne ne;
	struct nlm_action_cn cn;
	struct nlm_action_xp xp;
	struct nlm_action_vc vc;
	struct nlm_action_vp vp;
	struct nlm_action_dl dl;
	struct nlm_action_aj aj;
	struct nlm_action_va va;
	struct nlm_action_ds ds;
	struct nlm_action_da da;
	struct nlm_action_ra ra;
} nlm_action_obj_t;
typedef struct nlm_mgmt {
	uint type;			/* object type */
	uint id;			/* object identifier */
	uint cmd;			/* command */
	/* followed by object-specific structure */
	union nlm_action_obj action[0];
} nlm_mgmt_t;

#define NLM_IOCCMGMT	_IOWR(NLM_IOC_MAGIC,	21, nlm_mgmt_t)	/* command */

/*
 * PASS LOWER
 *
 * This structure is deprecated and as largely used for testing as a mechanism
 * to bypass an input-output control to a stream linked under a multiplexing
 * driver supporting this interface.
 */
typedef struct nlm_pass {
	uint muxid;			/* mux index of lower stream to pass message to */
	uint8_t type;			/* type of message block */
	uint8_t band;			/* band of message block */
	uint ctl_length;		/* length of ctrl part */
	uint dat_length;		/* length of data part */
	/* followed by ctrl and data part of message to pass */
} nlm_pass_t;

#define NLM_IOCCPASS	_IOW(	NLM_IOC_MAGIC,	22,	nlm_pass_t	)

#define NLM_IOC_FIRST    0
#define NLM_IOC_LAST    22
#define NLM_IOC_PRIVATE 32

#endif				/* __NLM_IOCTL_H__ */
