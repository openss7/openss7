/*****************************************************************************

 @(#) $Id: dl.h,v 0.9.2.1 2008-06-18 16:43:16 brian Exp $

 -----------------------------------------------------------------------------

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

 Last Modified $Date: 2008-06-18 16:43:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl.h,v $
 Revision 0.9.2.1  2008-06-18 16:43:16  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ifndef __SYS_DL_H__
#define __SYS_DL_H__

#ident "@(#) $RCSfile: dl.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

typedef struct communicationsEntity {
	char Id[SIMPLE_NAME_LEN];
	uint8_t LocalSapNames[MAX_OID_LEN];
	uint32_t OperationalState;
#define OPERATIONALSTATE_ENABLED	0
#define OPERATIONALSTATE_DISABLED	1
} CommunicationsEntity;

typedef struct sap1 {
	struct communicationsEntity base;
	char Id[SIMPLE_NAME_LEN];
	uint32_t Address;
	uint8_t UserEntityNames[MAX_OID_LEN];
} Sap1;

typedef struct sap2 {
	struct sap1 base;
	uint8_t Address[OCTET_STRING_LEN];
	uint8_t UserEntityNames[MAX_OID_LEN];
	uint8_t ProviderEntityNames[MAX_OID_LEN];
} Sap2;

typedef struct clProtocolMachine {
	struct communicationsEntity base;
	char Id[SIMPLE_NAME_LEN];
	uint32_t OperationalState;
#define OPERATIONALSTATE_ENABLED	0
#define OPERATIONALSTATE_DISABLED	1
	uint32_t TotalRemoteSAPs;
} ClProtocolMachine;

typedef struct coProtocolMachine {
	struct communicationsEntity base;
	char Id[SIMPLE_NAME_LEN];
	uint32_t OperationalState;
#define OPERATIONALSTATE_ENABLED	0
#define OPERATIONALSTATE_DISABLED	1
} CoProtocolMachine;

typedef struct singlePeerConnection {
	struct communicationsEntity base;
	struct coProtocolMachine base2;
	char connectionId[SIMPLE_NAME_LEN];
	uint8_t underlyingConnectionNames[MAX_OID_LEN];
	uint8_t supportedConnectionNames[MAX_OID_LEN];
} SinglePeerConnection;

typedef struct physicalEntity {
	struct communicationsEntity base;
	uint8_t PhysicalEntityTitles[MAX_OID_LEN];
} PhysicalEntity;

typedef struct physicalSAP {
	struct communicationsEntity base;
	struct sap1 base2;
} PhysicalSAP;

typedef struct dataCircuit {
	struct communicationsEntity base;
	struct coProtocolMachine base2;
	uint32_t BitErrorsReceived;
	uint32_t BitErrorsTransmitted;
	uint32_t BitErrorsThreshold;
	uint32_t Type;
	uint8_t PhysicalMediaNames[OCTET_STRING_LEN + 1];
	uint8_t PhysicalInterfaceType[MAX_OID_LEN];
	uint8_t PhysicalInterfaceStandard[MAX_OID_LEN];
	uint32_t SyncrhonizationMode;
	uint8_t TransmissionCoding[MAX_OID_LEN];
	uint32_t TransmissionMode;
	uint32_t TransmissionRate;
} DataCircuit;

typedef struct physicalConnection {
	struct communicationsEntity base;
	struct coProtocolMachine base2;
	uint8_t EndpointIdentifier[OCTET_STRING_LEN + 1];
	uint32_t PortNumber;
} PhysicalConnection;

typedef struct datalinkEntity {
	struct communicationsEntity base;
	uint8_t ProviderEntityNames[MAX_OID_LEN];
} DatalinkEntity;

typedef struct dLSAP {
	struct communicationsEntity base;
	struct sap1 base2;
} DLSAP;

typedef struct lAPBDLE {
	struct communicationsEntity base;
	uint32_t mT1Timer;
	uint32_t mT3Timer;
	uint32_t mW;
	uint32_t mXSend;
	uint32_t mXReceive;
	uint32_t mT2Timer;
	uint32_t receivedMlpResets;
	uint32_t timesMT1Expired;
	uint32_t IframesReassignments;
	uint32_t mlpFramesReceived;
	uint32_t mlpFramesSent;
	uint32_t mlpFramesOutsideWindowGuard;
	uint32_t receivedMlpFramesInGuardRegion;
} LAPBDLE;

typedef struct sLPPM {
	struct communicationsEntity base;
	struct coProtocolMachine base2;
	uint32_t AdministrativeState;
#define ADMINISTRATIVESTATE_LOCKED	0
#define ADMINISTRATIVESTATE_UNLOCKED	1
#define ADMINISTRATIVESTATE_SHUTTINGDOWN	2
} SLPPM;

typedef struct sLPConnection {
	struct communicationsEntity base;
	struct coProtocolMachine base2;
	struct singlePeerConnection base3;
	uint32_t InterfaceType;
#define SLPCONNECTION_INTERFACETYPE_DTE		0
#define SLPCONNECTION_INTERFACETYPE_DCE		1
	uint32_t K;
	uint32_t N1;
	uint32_t N2;
	uint32_t SequenceModulus;
#define SLPCONNECTION_SEQUENCEMODULUS_8			(1UL<<3)
#define SLPCONNECTION_SEQUENCEMODULUS_128		(1UL<<7)
#define SLPCONNECTION_SEQUENCEMODULUS_32768		(1UL<<15)
#define SLPCONNECTION_SEQUENCEMODULUS_2147483648	(1UL<<31)
	uint32_t T1Timer;
	uint32_t T2Timer;
	uint32_t FCSErrorsReceived;
	uint32_t FRMRsReceived;
	uint32_t FRMRsSent;
	uint32_t IFrameDataOctetsReceived;
	uint32_t IFrameDataOctetsSent;
	uint32_t IFramesReceived;
	uint32_t IFramesSent;
	uint32_t PollsReceived;
	uint32_t REJsReceived;
	uint32_t REJsSent;
	uint32_t SABMsReceived;
	uint32_t SABMsSent;
	uint32_t ProtocolState;
#define SLPCONNECTION_PROTOCOLSTATE_DISCONNECTED_PHASE				0
#define SLPCONNECTION_PROTOCOLSTATE_LINKDISCONNECTION_PHASE			1
#define SLPCONNECTION_PROTOCOLSTATE_LINK_SET_UP_PHASE				2
#define SLPCONNECTION_PROTOCOLSTATE_INFORMATION_TRANSFER_PHASE			3
#define SLPCONNECTION_PROTOCOLSTATE_FRAME_REJECT_CONDITION			4
#define SLPCONNECTION_PROTOCOLSTATE_BUSY_CONDITION				5
#define SLPCONNECTION_PROTOCOLSTATE_SEND_REJECT_CONDITION			6
#define SLPCONNECTION_PROTOCOLSTATE_SYSTEM_PARAMETERS_AND_ERROR_RECOVERY	7
	uint32_t TimesT1Expired;
	uint32_t T3Timer;
	uint32_t TimesT3Expired;
	uint32_t T4Timer;
	uint32_t TimesT4Expired;
	uint32_t AbormalLinkDisconnectsReceived;
	uint32_t AbormalLinkDisconnectsSent;
	uint32_t LinkResetsReceived;
	uint32_t LinkResetsSent;
	uint32_t TimesN2Reached;
	uint32_t AdministrativeState;
#define ADMINISTRATIVESTATE_LOCKED	0
#define ADMINISTRATIVESTATE_UNLOCKED	1
#define ADMINISTRATIVESTATE_SHUTTINGDOWN	2
	uint32_t OperationalState;
#define OPERATIONALSTATE_ENABLED	0
#define OPERATIONALSTATE_DISABLED	1
	uint32_t UsageState;
#define USAGESTATE_IDLE			0
#define USAGESTATE_ACTIVE		1
#define USAGESTATE_BUSY			2
	uint32_t ProceduralStatus;
#define PROCEDURALSTATUS_INITIALIZATIONREQUIRED		(1UL<<0)
#define PROCEDURALSTATUS_NOTINITIALIZED			(1UL<<1)
#define PROCEDURALSTATUS_INITIALIZING			(1UL<<2)
#define PROCEDURALSTATUS_REPORTING			(1UL<<3)
#define PROCEDURALSTATUS_TERMINATING			(1UL<<4)
	uint32_t AlarmStatus;
#define ALARMSTATUS_UNDERREPAIR		(1UL<<0)
#define ALARMSTATUS_CRITICAL		(1UL<<1)
#define ALARMSTATUS_MAJOR		(1UL<<2)
#define ALARMSTATUS_MINOR		(1UL<<3)
#define ALARMSTATUS_ALARMOUTSTANDING	(1UL<<4)
} SLPConnection;

typedef struct sLPConnectionIVMO {
	struct communicationsEntityId base;
	struct coProtocolMachine base2;
	struct singlePeerConnection base3;
	char id[ID_MAX_LEN];
	uint32_t interfaceType;		/* profile name */
	uint32_t k;			/* initial transmit window */
	uint32_t n1;			/* initial maximum IF size */
	uint32_t n2;			/* intiial maximum retries */
	uint32_t sequenceModulus;	/* initial sequence modulus */
	uint32_t t1Timer;		/* initial T1 timeout */
	uint32_t t2Timer;		/* initial T2 timeout */
	uint32_t t3Timer;		/* initial T3 timeout */
	uint32_t t4Timer;		/* initial T4 timeout */
} SLPConnectionIVMO;

typedef struct mACDLE {
	struct communicationsEntity base;
} MACDLE;

typedef struct mAC {
	struct communicationsEntity base;
	uint32_t OperationalState;
#define OPERATIONALSTATE_ENABLED	0
#define OPERATIONALSTATE_DISABLED	1
	uint32_t Id;
} MAC;

typedef struct lLCDLE {
	struct communicationsEntity base;
} LLCDLE;

typedef struct lLCCLPM {
	struct communicationsEntity base;
	struct clProtocolMachine base2;
} LLCCLPM;

typedef struct lLCCOPM {
	struct communicationsEntity base;
	struct coProtocolMachine base2;
} LLCCOPM;

#define SIMPLE_NAME_LEN		32
#define OCTET_STRING_LEN	128
#define DISPLAY_STRING_LEN	128

typedef struct resourceTypeId {
	struct communicationsEntity base;
	char Id[SIMPLE_NAME_LEN];
	char ManufacturerOUI[OCTET_STRING_LEN];
	char ManufacturerName[DISPLAY_STRING_LEN];
	char ManufacturerProductName[DISPLAY_STRING_LEN];
	char ManufacturerProductVersion[DISPLAY_STRING_LEN];
} ResourceTypeId;

typedef struct lLCStation {
	struct communicationsEntity base;
	struct sap1 base2;
	char LLCName[SIMPLE_NAME_LEN];
	uint32_t MaximumLSAPsConfigured;
	uint32_t NumberOfActiveLSAPs;
	uint32_t SupportedServicesTypes;
#define LLCSERVICES_TYPE1		(1UL<<0)
#define LLCSERVICES_TYPE2		(1UL<<1)
#define LLCSERVICES_TYPE3INITIATE	(1UL<<2)
#define LLCSERVICES_TYPE3RECEIVEDATA	(1UL<<3)
#define LLCSERVICES_TYPE3RETURNDATA	(1UL<<4)
	uint32_t Status;
#define LLCSTATIONSTATUS_UP		0
#define LLCSTATIONSTATUS_DOWN		1
#define LLCSTATIONSTATUS_OTHER		2
	uint32_t Type1AcknowledgeTimeoutValue;
	uint32_t Type1MaximumRetryCount;
	uint32_t MaximumPDUN3;
	uint32_t MaximumRetransmissions4;
	uint32_t ReceiveVariableLifetime;
	uint32_t TransmitVariableLifetime;
	uint32_t Type3AcknowledgeTimeoutValue;
	uint32_t Type3Retransmissions;
	uint32_t AvgBufferUseSize;
	uint32_t BufferProblems;
	uint32_t BufferSize;
	uint32_t MaxBufferUseSize;
	uint32_t InactiveLSAP;
	uint32_t PDUsDiscard;
	uint32_t STRIndicator;
#define STRINDICATOR_NSR		0
#define STRINDICATOR_STE		1
	uint32_t VersionNumber;
	uint32_t Type1AcknowledgmentTimerTimeouts;
} LLCStation;

typedef struct lLCSAP {
	struct communicationsEntity base;
	struct sap1 base2;
	char Name[SIMPLE_NAME_LEN];
	uint8_t LSAP;
	uint32_t RDE;
} LLCSAP;

typedef struct rdeSetup {
	struct communicationsEntity base;
	struct sap1 base2;
	uint32_t AgingEnabled;
	uint32_t AgingValue;
	uint32_t EnableType2Reset;
	uint32_t MaximumRouteDescriptors;
	uint32_t MaximumResponseTime;
	uint32_t MinimumPDUSize;
	uint32_t RDEHold;
	uint32_t RDEReplace;
	uint32_t Name;
	uint32_t ResetOnTestEnabled;
} RDESetup;

typedef struct rdePair {
	struct communicationsEntity base;
	struct sap1 base2;
	unsigned char rDEPairName[20];
	uint32_t rDEPairDiscardCounter;
	uint32_t rDEPairNSRPDUCounter;
	uint32_t rDEPairNSRSelectedCounter;
	uint8_t rDEPairRIF[31];
	uint32_t rDEPairSRFPDUCounter;
	uint32_t rDEPairQueryCounter;
} RDEPair;

typedef struct lLCConnectionLess {
	struct communicationsEntity base;
	struct clProtocolMachine base2;
	struct clProtocolMachine base3;
	char Name[SIMPLE_NAME_LEN];
	uint32_t MaximumLLCInformationFieldSize;
	uint32_t TESTReceivedABBResponse;
	uint32_t TESTReceivedCommand;
	uint32_t TESTReceivedResponse;
	uint32_t TESTSentABBResponse;
	uint32_t TESTSentCommand;
	uint32_t TESTSentResponse;
	uint32_t UIReceived;
	uint32_t UISent;
	uint32_t XIDReceivedCommand;
	uint32_t XIDReceivedResponse;
	uint32_t XIDSentCommand;
	uint32_t XIDSentResponse;
} LLCConnectionLess;

typedef struct lLCConnection2 {
	struct communicationsEntity base;
	struct coProtocolMachine base2;
	struct coProtocolMachine base3;
	char Name[SIMPLE_NAME_LEN];
	uint32_t MaximumRetransmissions;
	uint32_t ReceivedWindowSize;
	uint32_t SendWindowSize;
	uint32_t AcknowledgeTimeoutValue;
	uint32_t BusyStateTimeoutValue;
	uint32_t PBitTimeoutValue;
	uint32_t RejectTimeoutValue;
	uint32_t LocalBusy;
	uint32_t RemoteBusy;
	uint32_t RemoteReset;
	uint32_t LocalReset;
	uint32_t ProviderReset;
	uint8_t Route[OCTET_STRING_LEN + 1];
	uint32_t KStep;
	uint32_t MaxSendWindowSize;
	uint32_t ReceivedI;
	uint32_t SentI;
	uint32_t SentAcks;
	uint32_t ReceivedAcks;
	uint32_t ReceivedFRMR;
	uint32_t SentFRMR;
	uint32_t ReceivedRR;
	uint32_t SentRR;
	uint32_t ReceivedRNR;
	uint32_t SentRNR;
	uint32_t ReceivedREJ;
	uint32_t SentREJ;
	uint32_t ReceivedSABME;
	uint32_t SentSABME;
	uint32_t ReceivedUA;
	uint32_t SentUA;
	uint32_t ReceivedDISC;
	uint32_t SentDISC;
	uint32_t ReceivedDM;
	uint32_t SentDM;
	uint32_t PDUsDiscarded1;
	uint32_t PDUsDiscarded2;
	uint32_t PDURetransmissions;
	uint32_t OptionalTolerationIPDUs;
	uint32_t DuplicateIPDUsReceived;
	uint32_t Violation;
	uint32_t ProtocolState;
	uint32_t AdministrativeState;
#define ADMINISTRATIVESTATE_LOCKED		0
#define ADMINISTRATIVESTATE_UNLOCKED		1
#define ADMINISTRATIVESTATE_SHUTTINGDOWN	2
	uint32_t OperationalState;
#define OPERATIONALSTATE_ENABLED	0
#define OPERATIONALSTATE_DISABLED	1
	uint32_t UsageState;
#define USAGESTATE_IDLE			0
#define USAGESTATE_ACTIVE		1
#define USAGESTATE_BUSY			2
	uint32_t ProceduralStatus;
#define PROCEDURALSTATUS_INITIALIZATIONREQUIRED		(1UL<<0)
#define PROCEDURALSTATUS_NOTINITIALIZED			(1UL<<1)
#define PROCEDURALSTATUS_INITIALIZING			(1UL<<2)
#define PROCEDURALSTATUS_REPORTING			(1UL<<3)
#define PROCEDURALSTATUS_TERMINATING			(1UL<<4)
	uint32_t AlarmStatus;
#define ALARMSTATUS_UNDERREPAIR		(1UL<<0)
#define ALARMSTATUS_CRITICAL		(1UL<<1)
#define ALARMSTATUS_MAJOR		(1UL<<2)
#define ALARMSTATUS_MINOR		(1UL<<3)
#define ALARMSTATUS_ALARMOUTSTANDING	(1UL<<4)
} LLCConnection2;

typedef struct lLCConnection2IVMO {
	struct communicationsEntity base;
	struct coProtocolMachine base2;
	struct coProtocolMachine base3;
	char Name[SIMPLE_NAME_LEN];
	uint32_t MaximumRetransmissions;
	uint32_t ReceivedWindowSize;
	uint32_t SendWindowSize;
	uint32_t AcknowledgeTimeoutValue;
	uint32_t BusyStateTimeoutValue;
	uint32_t BitTimeoutValue;
	uint32_t RejectTimeoutValue;
	uint32_t Route;
	uint32_t KStep;
	uint32_t MaxSendWindowSize;
	uint32_t OptionalTolerationIPDUs;
} LLCConnection2IVMO;

typedef struct lLCConnectionlessAck {
	struct communicationsEntity base;
	struct coProtocolMachine base2;
	struct coProtocolMachine base3;
	char Name[SIMPLE_NAME_LEN];
	uint32_t MaximumLLCInformationFieldSize;
	uint32_t MaximumRetransmissions;
	uint32_t TESTReceivedABBResponse;
	uint32_t TESTReceivedCommand;
	uint32_t TESTReceivedResponse;
	uint32_t TESTSentABBResponse;
	uint32_t TESTSentCommand;
	uint32_t TESTSentResponse;
	uint32_t ReceiveResources;
	uint32_t UIReceived;
	uint32_t UISent;
	uint32_t XIDReceivedCommand;
	uint32_t XIDReceivedResponse;
	uint32_t XIDSentCommand;
	uint32_t XIDSentResponse;
	uint32_t Retransmissions;
	uint32_t NoResponse;
	uint32_t CommandIP;
	uint32_t CommandIT;
	uint32_t CommandOK;
	uint32_t CommandPE;
	uint32_t CommandRS;
	uint32_t CommandUE;
	uint32_t CommandUN;
	uint32_t ReceivedACCommand;
	uint32_t SentACCommand;
	uint32_t ResponseIP;
	uint32_t ResponseIT;
	uint32_t ResponseNE;
	uint32_t ResponseNR;
	uint32_t ResponseOK;
	uint32_t ResponseRS;
	uint32_t ResponseUE;
	uint32_t ResponseUN;
	uint32_t Violation;
} LLCConnectionlessAck;

typedef struct lLCConnectionlessAckIVMO {
	struct communicationsEntity base;
	struct coProtocolMachine base2;
	struct coProtocolMachine base3;
	char Name[SIMPLE_NAME_LEN];
	uint32_t MaximumLLCInformationFieldSize;
	uint32_t MaximumRetransmissions;
} LLCConnectionlessAckIVMO;

#endif				/* __SYS_DL_H__ */
