/*
 @(#) src/java/javax/jain/ss7/inap/datatype/EventSpecificInformationBCSM.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package javax.jain.ss7.inap.datatype;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This class contains the EventSpecificInformationBCSM.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class EventSpecificInformationBCSM implements java.io.Serializable{ 
    /** Constructors For EventSpecificInformationBCSM.  */
    public EventSpecificInformationBCSM(BackwardGVNS backwardGVNS) {
        setOAnswerSpecificInfo(backwardGVNS);
    }
    /** Constructors For EventSpecificInformationBCSM.  */
    public EventSpecificInformationBCSM(Cause cause, EventSpecificInformationBCSMChoice eventSpecificInfoID) {
        if(eventSpecificInfoID == EventSpecificInformationBCSMChoice.ROUTE_SELECT_FAILURE_SPECIFIC_INFO)
            setRouteSelectFailureSpecificInfo(cause);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.O_CALLED_PARTY_BUSY_SPECIFIC_INFO)
            setOCalledPartyBusySpecificInfo(cause);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.T_BUSY_SPECIFIC_INFO)
            setTBusySpecificInfo(cause);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.O_ABANDON)
            setOAbandon(cause);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.T_ABANDON)
            setTAbandon(cause);
    }
    /** Constructors For EventSpecificInformationBCSM.  */
    public EventSpecificInformationBCSM(CalledPartyNumber calledPartyNumber, EventSpecificInformationBCSMChoice eventSpecificInfoID ) {
        if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.COLLECTED_INFO_SPECIFIC_INFO)
            setCollectedInfoSpecificInfo(calledPartyNumber);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.ANALYSED_INFO_SPECIFIC_INFO)
            setAnalysedInfoSpecificInfo(calledPartyNumber);
    }
    /** Constructors For EventSpecificInformationBCSM.  */
    public EventSpecificInformationBCSM(MidCallSpecificInfo midCallSpecificInfo, EventSpecificInformationBCSMChoice eventSpecificInfoID) {
        if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.O_MID_CALL_SPECIFIC_INFO)
            setOMidCallSpecificInfo(midCallSpecificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.T_MID_CALL_SPECIFIC_INFO)
            setTMidCallSpecificInfo(midCallSpecificInfo);
    }
    /** Constructors For EventSpecificInformationBCSM.  */
    public EventSpecificInformationBCSM(DisconnectSpecificInfo disconnectSpecificInfo, EventSpecificInformationBCSMChoice eventSpecificInfoID) {
        if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.O_DISCONNECT_SPECIFIC_INFO)
            setODisconnectSpecificInfo(disconnectSpecificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.T_DISCONNECT_SPECIFIC_INFO)
            setTDisconnectSpecificInfo(disconnectSpecificInfo);
    }
    /** Constructors For EventSpecificInformationBCSM.  */
    public EventSpecificInformationBCSM(java.lang.String specificInfo , EventSpecificInformationBCSMChoice eventSpecificInfoID) {
        if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.O_NO_ANSWER_SPECIFIC_INFO)
            setONoAnswerSpecificInfo(specificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.T_NO_ANSWER_SPECIFIC_INFO)
            setTNoAnswerSpecificInfo(specificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.T_ANSWER_SPECIFIC_INFO)
            setTAnswerSpecificInfo(specificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.O_TERM_SEIZED_SPECIFIC_INFO)
            setOTermSeizedSpecificInfo(specificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.O_SUSPENDED)
            setOSuspended(specificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.T_SUSPENDED)
            setTSuspended(specificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.ORIG_ATTEMPT_AUTHORIZED)
            setOrigAttemptAuthorized(specificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.O_RE_ANSWER)
            setOReAnswer(specificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.T_RE_ANSWER)
            setTReAnswer(specificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.FACILITY_SELECTED_AND_AVAILABLE)
            setFacilitySelectedAndAvailable(specificInfo);
        else if (eventSpecificInfoID == EventSpecificInformationBCSMChoice.CALL_ACCEPTED)
            setCallAccepted(specificInfo);
    }
    /** Gets EventSpecificChoice.  */
    public EventSpecificInformationBCSMChoice getEventSpecificChoice() {
        return eventSpecificChoice;
    }
    /** Gets collectedInfoSpecificInfo.  */
    public CalledPartyNumber getCollectedInfoSpecificInfo() {
        return collectedInfoSpecificInfo;
    }
    /** Sets collectedInfoSpecificInfo.  */
    public void setCollectedInfoSpecificInfo(CalledPartyNumber collectedInfoSpecificInfo) {
        this.collectedInfoSpecificInfo = collectedInfoSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.COLLECTED_INFO_SPECIFIC_INFO;
    }
    /** Gets analysedInfoSpecificInfo.  */
    public CalledPartyNumber getAnalysedInfoSpecificInfo() {
        return analysedInfoSpecificInfo;
    }
    /** Sets analysedInfoSpecificInfo.  */
    public void setAnalysedInfoSpecificInfo(CalledPartyNumber analysedInfoSpecificInfo) {
        this.analysedInfoSpecificInfo = analysedInfoSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.ANALYSED_INFO_SPECIFIC_INFO;
    }
    /** Gets routeSelectFailureSpecificInfo.  */
    public Cause getRouteSelectFailureSpecificInfo() {
        return routeSelectFailureSpecificInfo;
    }
    /** Sets routeSelectFailureSpecificInfo.  */
    public void setRouteSelectFailureSpecificInfo(Cause routeSelectFailureSpecificInfo) {
        this.routeSelectFailureSpecificInfo = routeSelectFailureSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.ROUTE_SELECT_FAILURE_SPECIFIC_INFO;
    }
    /** Gets oCalledPartyBusySpecificInfo.  */
    public Cause getOCalledPartyBusySpecificInfo() {
        return oCalledPartyBusySpecificInfo;
    }
    /** Sets oCalledPartyBusySpecificInfo.  */
    public void setOCalledPartyBusySpecificInfo(Cause oCalledPartyBusySpecificInfo) {
        this.oCalledPartyBusySpecificInfo = oCalledPartyBusySpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.O_CALLED_PARTY_BUSY_SPECIFIC_INFO;
    }
    /** Gets oNoAnswerSpecificInfo.  */
    public java.lang.String getONoAnswerSpecificInfo() {
        return oNoAnswerSpecificInfo;
    }
    /** Sets oNoAnswerSpecificInfo.  */
    public void setONoAnswerSpecificInfo(java.lang.String oNoAnswerSpecificInfo) {
        this.oNoAnswerSpecificInfo = oNoAnswerSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.O_NO_ANSWER_SPECIFIC_INFO;
    }
    /** Gets oAnswerSpecificInfo.  */
    public BackwardGVNS getOAnswerSpecificInfo() {
        return oAnswerSpecificInfo;
    }
    /** Sets oAnswerSpecificInfo.  */
    public void setOAnswerSpecificInfo(BackwardGVNS oAnswerSpecificInfo) {
        this.oAnswerSpecificInfo = oAnswerSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.O_ANSWER_SPECIFIC_INFO;
    }
    /** Gets oMidCallSpecificInfo.  */
    public MidCallSpecificInfo getOMidCallSpecificInfo() {
        return oMidCallSpecificInfo;
    }
    /** Sets oMidCallSpecificInfo.  */
    public void setOMidCallSpecificInfo(MidCallSpecificInfo oMidCallSpecificInfo) {
        this.oMidCallSpecificInfo = oMidCallSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.O_MID_CALL_SPECIFIC_INFO;
    }
    /** Gets oDisconnectSpecificInfo.  */
    public DisconnectSpecificInfo getODisconnectSpecificInfo() {
        return oDisconnectSpecificInfo;
    }
    /** Sets oDisconnectSpecificInfo.  */
    public void setODisconnectSpecificInfo(DisconnectSpecificInfo oDisconnectSpecificInfo) {
        this.oDisconnectSpecificInfo = oDisconnectSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.O_DISCONNECT_SPECIFIC_INFO;
    }
    /** Gets tBusySpecificInfo.  */
    public Cause getTBusySpecificInfo() {
        return tBusySpecificInfo;
    }
    /** Sets tBusySpecificInfo.  */
    public void setTBusySpecificInfo(Cause tBusySpecificInfo) {
        this.tBusySpecificInfo = tBusySpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.T_BUSY_SPECIFIC_INFO;
    }
    /** Gets tNoAnswerSpecificInfo.  */
    public java.lang.String getTNoAnswerSpecificInfo() {
        return tNoAnswerSpecificInfo;
    }
    /** Sets tNoAnswerSpecificInfo.  */
    public void setTNoAnswerSpecificInfo(java.lang.String tNoAnswerSpecificInfo) {
        this.tNoAnswerSpecificInfo = tNoAnswerSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.T_NO_ANSWER_SPECIFIC_INFO;
    }
    /** Gets tAnswerSpecificInfo.  */
    public java.lang.String getTAnswerSpecificInfo() {
        return tAnswerSpecificInfo;
    }
    /** Sets tAnswerSpecificInfo.  */
    public void setTAnswerSpecificInfo(java.lang.String tAnswerSpecificInfo) {
        this.tAnswerSpecificInfo = tAnswerSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.T_ANSWER_SPECIFIC_INFO;
    }
    /** Gets tMidCallSpecificInfo.  */
    public MidCallSpecificInfo getTMidCallSpecificInfo() {
        return tMidCallSpecificInfo;
    }
    /** Sets tMidCallSpecificInfo.  */
    public void setTMidCallSpecificInfo(MidCallSpecificInfo tMidCallSpecificInfo) {
        this.tMidCallSpecificInfo = tMidCallSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.T_MID_CALL_SPECIFIC_INFO;
    }
    /** Gets tDisconnectSpecificInfo.  */
    public DisconnectSpecificInfo getTDisconnectSpecificInfo() {
        return tDisconnectSpecificInfo;
    }
    /** Sets tDisconnectSpecificInfo.  */
    public void setTDisconnectSpecificInfo(DisconnectSpecificInfo tDisconnectSpecificInfo) {
        this.tDisconnectSpecificInfo = tDisconnectSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.T_DISCONNECT_SPECIFIC_INFO;
    }
    /** Gets oTermSeizedSpecificInfo.  */
    public java.lang.String getOTermSeizedSpecificInfo() {
        return oTermSeizedSpecificInfo;
    }
    /** Sets oTermSeizedSpecificInfo.  */
    public void setOTermSeizedSpecificInfo(java.lang.String oTermSeizedSpecificInfo) {
        this.oTermSeizedSpecificInfo = oTermSeizedSpecificInfo;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.O_TERM_SEIZED_SPECIFIC_INFO;
    }
    /** Gets oSuspended.  */
    public java.lang.String getOSuspended() {
        return oSuspended;
    }
    /** Sets oSuspended.  */
    public void setOSuspended(java.lang.String oSuspended) {
        this.oSuspended = oSuspended;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.O_SUSPENDED;
    }
    /** Gets tSuspended.  */
    public java.lang.String getTSuspended() {
        return tSuspended;
    }
    /** Sets tSuspended.  */
    public void setTSuspended(java.lang.String tSuspended) {
        this.tSuspended = tSuspended;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.T_SUSPENDED;
    }
    /** Gets origAttemptAuthorized.  */
    public java.lang.String getOrigAttemptAuthorized() {
        return origAttemptAuthorized;
    }
    /** Sets origAttemptAuthorized.  */
    public void setOrigAttemptAuthorized(java.lang.String origAttemptAuthorized) {
        this.origAttemptAuthorized = origAttemptAuthorized;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.ORIG_ATTEMPT_AUTHORIZED;
    }
    /** Gets oReAnswer.  */
    public java.lang.String getOReAnswer() {
        return oReAnswer;
    }
    /** Sets oReAnswer.  */
    public void setOReAnswer(java.lang.String oReAnswer) {
        this.oReAnswer = oReAnswer;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.O_RE_ANSWER;
    }
    /** Gets tReAnswer.  */
    public java.lang.String getTReAnswer() {
        return tReAnswer;
    }
    /** Sets tReAnswer. */
    public void setTReAnswer(java.lang.String tReAnswer) {
        this.tReAnswer = tReAnswer;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.T_RE_ANSWER;
    }
    /** Gets facilitySelectedAndAvailable.  */
    public java.lang.String getFacilitySelectedAndAvailable() {
        return facilitySelectedAndAvailable;
    }
    /** Sets facilitySelectedAndAvailable.  */
    public void setFacilitySelectedAndAvailable(java.lang.String facilitySelectedAndAvailable) {
        this.facilitySelectedAndAvailable = facilitySelectedAndAvailable;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.FACILITY_SELECTED_AND_AVAILABLE;
    }
    /** Gets callAccepted.  */
    public java.lang.String getCallAccepted() {
        return callAccepted;
    }
    /** Sets callAccepted.  */
    public void setCallAccepted(java.lang.String callAccepted) {
        this.callAccepted = callAccepted;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.CALL_ACCEPTED;
    }
    /** Gets oAbandon.  */
    public Cause getOAbandon() {
        return oAbandon;
    }
    /** Sets oAbandon.  */
    public void setOAbandon(Cause oAbandon) {
        this.oAbandon = oAbandon;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.O_ABANDON;
    }
    /** Gets tAbandon.  */
    public Cause getTAbandon() {
        return tAbandon;
    }
    /** Sets tAbandon.  */
    public void setTAbandon(Cause tAbandon) {
        this.tAbandon = tAbandon;
        eventSpecificChoice = EventSpecificInformationBCSMChoice.T_ABANDON;
    }
    private CalledPartyNumber collectedInfoSpecificInfo;
    private CalledPartyNumber analysedInfoSpecificInfo;    
    private Cause routeSelectFailureSpecificInfo;    
    private Cause oCalledPartyBusySpecificInfo;    
    private java.lang.String oNoAnswerSpecificInfo;    
    private BackwardGVNS oAnswerSpecificInfo;    
    private MidCallSpecificInfo oMidCallSpecificInfo;    
    private DisconnectSpecificInfo oDisconnectSpecificInfo;    
    private Cause tBusySpecificInfo;    
    private java.lang.String tNoAnswerSpecificInfo;    
    private java.lang.String tAnswerSpecificInfo;    
    private MidCallSpecificInfo tMidCallSpecificInfo;    
    private DisconnectSpecificInfo tDisconnectSpecificInfo;    
    private java.lang.String oTermSeizedSpecificInfo;    
    private java.lang.String oSuspended;    
    private java.lang.String tSuspended;    
    private java.lang.String origAttemptAuthorized;    
    private java.lang.String oReAnswer;    
    private java.lang.String tReAnswer;    
    private java.lang.String facilitySelectedAndAvailable;    
    private java.lang.String callAccepted;    
    private Cause oAbandon;    
    private Cause tAbandon;
    private EventSpecificInformationBCSMChoice eventSpecificChoice;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
