/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __SYS_LLC_H__
#define __SYS_LLC_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* lLCDLE <- datalinkEntity <- communicationsEntity */

/* communicationsEntity
 * contained by subsystem
 * contains: clProtocolMachine, coProtocolMachine, sap1
 * attributes:
 *  communicationsEntityId(GraphicString) <- index
 *  localSapNames(GroupObjects)
 *
 * datalinkEntity
 * derived from: communicationsEntity
 * contained by datalinkSubsystem
 * contains: dlSAP
 * attributes:
 *  providerEntityNames(GroupObjects)
 *
 * lLCDLE
 * derived from datalinkEntity
 * contains lLCCLPM, lLCCOPM, resourceTypeId, eWMAMetricMonitor
 * attributes: none
 */
/*
 * dLSAP <- sap1
 *
 * sap1
 * conatined by communicationsEntity
 * attributes:
 *  sapId(GraphicString) <- index
 *  sap1Address(INTEGER)
 *  userEntityNames(GroupObjects)
 *
 * dLSAP
 * derived from sap1
 * contained by datalinkEntity
 * attributes none
 *
 * lLCSAP
 * derived from dLSAP
 * contained by lLCDLE
 * contains rDESetup
 * attributes:
 *  lLCSAPName(GraphicString) <- index
 *  lLCAddress(OCTET STRING), rDE
 * 
 */

/*
 * lLCConnection2 <- lLCCOPM <- clProtocolMachine
 *
 * clProtocolMachine
 * contained by communicationsEntity
 * attributes
 *  clProtocolMachineId(GraphicString) <- index
 *  [CONDITIONAL] totalRemoteSAPs
 *
 * lLCCOPM
 * contained by lLCDLE
 * contains lLCConnection2
 * attributes: none
 *
 * lLCConnection2
 * derived from lLCCOPM
 * contained by lLCCOPM
 * contains lLCConnection2IVMO
 * attributes:
 *  lLCConnection2Name <- index,
 *  maximumRetransmissions, receivedWindowSize,
 *	type2AcknowledgementTimeoutValue, type2BusyStateTimeoutValue,
 *	type2PBitTimeoutValue, type2RejectTimeoutValue
 *  [CONDITIONAL] route
 *  [CONDITIONAL] kStep, maxSendWindowSize
 *  [CONDITIONAL] type2ReceivedI, type2SentI
 *  [CONDITIONAL] sentAcks, receivedAcks
 *  [CONDITIONAL] type2ReceivedFRMR, type2SentFRMR
 *  [CONDITIONAL] type2ReceivedRR, type2SentRR
 *  [CONDITIONAL] type2ReceivedRNR, type2SentRNR
 *  [CONDITIONAL] type2ReceivedREJ, type2SentREJ
 *  [CONDITIONAL] type2ReceivedSABME, type2SentSABME
 *  [CONDITIONAL] type2ReceivedUA, type2SentUA
 *  [CONDITIONAL] type2ReceivedDISC, type2SentDISC
 *  [CONDITIONAL] type2ReceivedDM, type2SentDM
 *  [CONDITIONAL] pDUsDiscarded1, pDUsDiscarded2, pDURetransmissions
 *  [CONDITIONAL] optionalTolerationIPDUs, duplicateIPDUsReceived
 *  [CONDITIONAL] type2Violation
 *
 */

/*
 * sLPConnection <- singlePeerConnection
 *
 * singlePeerConnection
 * contained by coProtocolMachine
 * attributes:
 *  connectionId(GraphicString) <- index
 *  underlyingConnectionNames(GroupObjects)
 *  [CONDITIONAL] supportedConnectionNames(GroupObjects)
 *
 * sLPConnection
 * derived from signlePeerConnection
 * contained by sLPPM (index connectionId) (but only for lAPBDLE)
 * attributes:
 *  interfaceType, k, n1, n2, sequenceModulus, t1Timer, t2Timer
 *  fCSErrorsReceived, fRMRsReceived, fRMRsSent, iFrameDataOctetsReceived,
 *	iFrameDataOctetsSent, iFramesReceived, iFramesSent, pollsReceived,
 *	rEJsReceived, rEJsSent, rNRsReceived, rNRsSent, sABMsReceived,
 *	sABMsSnet, sLPProtocolState, timersT1Expired
 *  [CONDITIONAL] t3Timer, timerT3Expired
 *  [CONDITIONAL] t4Timer, timesT4Expired
 *  [CONDITIONAL] abnormalLinkDisconnectsReceived,
 *	abnormalLinkDisconnectsSent, linkResetsReceived, linkResetsSent,
 *	timesN2Reached
 *
 */

/* These LAPB structures are consistent with X.282 | ISO/IEC 10742 GDMO MIBs */

struct lapb_timers {
	unsigned int t1Timer;
	unsigned int t2Timer;
	unsigned int t3Timer;
	unsigned int t4Timer;
};

struct lapb_opts {
	unsigned int interfaceType;
	unsigned int k;
	unsigned int n1;
	unsigned int n2;
	unsigned int sequenceModulus;
	struct lapb_timers timers;
};

struct lapb_stats {
	uint64_t fCSErrorsReceived;
	uint64_t fRMRsReceived;
	uint64_t fRMRsSent;
	uint64_t iFrameDataOctetsReceived;
	uint64_t iFrameDataOctetsSent;
	uint64_t iFramesReceived;
	uint64_t iFramesSent;
	uint64_t pollsReceived;
	uint64_t rEJsReceived;
	uint64_t rEJsSent;
	uint64_t rNRsReceived;
	uint64_t rNRsSent;
	uint64_t sABMsReceived;
	uint64_t sABMsSent;
	uint64_t timerT1Expired;
	uint64_t timerT3Expired;
	uint64_t timerT4Expired;
	uint64_t abnormalLinkDisconnectsReceived;
	uint64_t abnormalLInkDisconnectsSent;
	uint64_t linkResetsReceived;
	uint64_t linkResetsSent;
	uint64_t timesN2Reached;
};

/* These LLC2 structures are consistent with ISO/IEC 8802-2 GDMO MIBs */

struct llc2_timers {
	/* This attribute defines the timer iterval during which the LLC shall expect to receive
	   an acknowledgement to one or more outstanding I PDUs or an expected response PDU to a
	   sent unnumbered command PDU.  The value is measured in milliseconds. */
	unsigned int type2AcknowledgementTimeoutValue;
	/* This attribute defines the time interval during which the LLC shall wait for an
	   indication of the clearance of a busy condition at the other LLC.  The time value is
	   measured in milliseconds. */
	unsigned int type2BusyStateTimeoutValue;
	/* The P-bit timer defines the time interval during which the LLC shall expecte to receive 
	   a PDU with the F bit set to 1 in response to a sent Type 2 command with the P bit set to 
	   1.  This value is measured in seconds. */
	unsigned int type2PBitTimeoutValue;
	/* The Reject timer defines the time interval during which the LLC shall expect to receive
	   a reply to a sent REJ PDU.  This value is measured in milliseconds. */
	unsigned int type2RejectTimeoutValue;
};

struct llc2_opts {
	/* The number of times that the LLC Type 2 should attempt to realize a successful PDU
	   transfer. */
	unsigned int maximumRetransmissions;
	/* The value of the receive window. */
	unsigned int receivedWindowSize;
	/* The value of the send window. */
	unsigned int receiveWindowSize;
	struct llc2_timers timers;
	/* This is a conditional part oft he LSAP Pair Object.  It is present if source routing is
	   used and the LSAP Pair is cognizant of its route.  This attribute is used by the LSAP
	   Pair to communicate. Source Routing is described in ISO/IEC 10038 : 1993 (IEEE 802.1D
	   1993). */
	unsigned int route;
	unsigned int kStep;
	unsigned int maxSendWindowSize;	/* The maximum size of the send window. */
	unsigned int optionalTolerationIPDUs;
};

struct llc2_stats {
	uint64_t type2ReceivedI;
	uint64_t type2SendI;
	uint64_t receivedAcks;
	uint64_t sentAcks;
	uint64_t type2ReceivedFRMR;
	uint64_t type2SentFRMR;
	uint64_t type2ReceivedRR;
	uint64_t type2SentRR;
	uint64_t type2ReceivedRNR;
	uint64_t type2SentRNR;
	uint64_t type2ReceivedREJ;
	uint64_t type2SentREJ;
	uint64_t type2ReceivedSABME;
	uint64_t type2SentSABME;
	uint64_t type2ReceivedUA;
	uint64_t type2SentUA;
	uint64_t type2ReceivedDISC;
	uint64_t type2SentDISC;
	uint64_t type2ReceivedDM;
	uint64_t type2SentDM;
	uint64_t pDUsDiscarded1;
	uint64_t pDUsDiscarded2;
	uint64_t pDURetransmissions;
	uint64_t duplicateIPDUsReceived;
	uint64_t type2Violation;
};

#endif				/* __SYS_LLC_H__ */
