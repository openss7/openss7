/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Mahindra British Telecom
 *  155 , Bombay - Pune Road 
 *  Pimpri ,
 *  Pune - 411 018.
 *
 *  Module Name   : JAIN INAP API
 *  File Name     : EventSpecificInformationBCSM.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;

/**
This class contains the EventSpecificInformationBCSM
*/

public class EventSpecificInformationBCSM implements java.io.Serializable{

    private CalledPartyNumber collectedInfoSpecificInfo;
    private CalledPartyNumber analysedInfoSpecificInfo;    
    private Cause routeSelectFailureSpecificInfo;    
    private Cause oCalledPartyBusySpecificInfo;    
    private String oNoAnswerSpecificInfo;    
    private BackwardGVNS oAnswerSpecificInfo;    
    private MidCallSpecificInfo oMidCallSpecificInfo;    
    private DisconnectSpecificInfo oDisconnectSpecificInfo;    
    private Cause tBusySpecificInfo;    
    private String tNoAnswerSpecificInfo;    
    private String tAnswerSpecificInfo;    
    private MidCallSpecificInfo tMidCallSpecificInfo;    
    private DisconnectSpecificInfo tDisconnectSpecificInfo;    
    private String oTermSeizedSpecificInfo;    
    private String oSuspended;    
    private String tSuspended;    
    private String origAttemptAuthorized;    
    private String oReAnswer;    
    private String tReAnswer;    
    private String facilitySelectedAndAvailable;    
    private String callAccepted;    
    private Cause oAbandon;    
    private Cause tAbandon;

private EventSpecificInformationBCSMChoice eventSpecificChoice;

/**
Constructors For EventSpecificInformationBCSM
*/
	public EventSpecificInformationBCSM(BackwardGVNS backwardGVNS)
	{
		setOAnswerSpecificInfo(backwardGVNS);
	}


	public EventSpecificInformationBCSM(Cause cause , EventSpecificInformationBCSMChoice eventSpecificInfoID)
	{
		if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.ROUTE_SELECT_FAILURE_SPECIFIC_INFO.getEventSpecificInformationBCSMChoice())
			setRouteSelectFailureSpecificInfo(cause);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.O_CALLED_PARTY_BUSY_SPECIFIC_INFO.getEventSpecificInformationBCSMChoice())
			setOCalledPartyBusySpecificInfo(cause);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.T_BUSY_SPECIFIC_INFO.getEventSpecificInformationBCSMChoice())
			setTBusySpecificInfo(cause);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.O_ABANDON.getEventSpecificInformationBCSMChoice())	
			setOAbandon(cause);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.T_ABANDON.getEventSpecificInformationBCSMChoice())			
			setTAbandon(cause);

	}


	public EventSpecificInformationBCSM(CalledPartyNumber calledPartyNumber , EventSpecificInformationBCSMChoice eventSpecificInfoID )
	{
		if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.COLLECTED_INFO_SPECIFIC_INFO.getEventSpecificInformationBCSMChoice())
			setCollectedInfoSpecificInfo(calledPartyNumber);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.ANALYSED_INFO_SPECIFIC_INFO.getEventSpecificInformationBCSMChoice())
			setAnalysedInfoSpecificInfo(calledPartyNumber);
	}


	public EventSpecificInformationBCSM(MidCallSpecificInfo midCallSpecificInfo , EventSpecificInformationBCSMChoice eventSpecificInfoID)
	{
		if(eventSpecificInfoID == EventSpecificInformationBCSMChoice.O_MID_CALL_SPECIFIC_INFO)
			setOMidCallSpecificInfo(midCallSpecificInfo);
		else if(eventSpecificInfoID == EventSpecificInformationBCSMChoice.T_MID_CALL_SPECIFIC_INFO)	
			setTMidCallSpecificInfo(midCallSpecificInfo);

	}


	public EventSpecificInformationBCSM(DisconnectSpecificInfo disconnectSpecificInfo , EventSpecificInformationBCSMChoice eventSpecificInfoID)
	{
		if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.O_DISCONNECT_SPECIFIC_INFO.getEventSpecificInformationBCSMChoice())
			setODisconnectSpecificInfo(disconnectSpecificInfo);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.T_DISCONNECT_SPECIFIC_INFO.getEventSpecificInformationBCSMChoice())	
			setTDisconnectSpecificInfo(disconnectSpecificInfo);

	}


	public EventSpecificInformationBCSM(java.lang.String specificInfo , EventSpecificInformationBCSMChoice eventSpecificInfoID)
	{
		if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.O_NO_ANSWER_SPECIFIC_INFO.getEventSpecificInformationBCSMChoice())
			setONoAnswerSpecificInfo(specificInfo);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.T_NO_ANSWER_SPECIFIC_INFO.getEventSpecificInformationBCSMChoice())	
			setTNoAnswerSpecificInfo(specificInfo);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.T_ANSWER_SPECIFIC_INFO.getEventSpecificInformationBCSMChoice())		
			setTAnswerSpecificInfo(specificInfo);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.O_TERM_SEIZED_SPECIFIC_INFO.getEventSpecificInformationBCSMChoice())			
			setOTermSeizedSpecificInfo(specificInfo);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.O_SUSPENDED.getEventSpecificInformationBCSMChoice())				
			setOSuspended(specificInfo);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.T_SUSPENDED.getEventSpecificInformationBCSMChoice())
			setTSuspended(specificInfo);					
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.ORIG_ATTEMPT_AUTHORIZED.getEventSpecificInformationBCSMChoice())	
			setOrigAttemptAuthorized(specificInfo);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.O_RE_ANSWER.getEventSpecificInformationBCSMChoice())		
			setOReAnswer(specificInfo);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.T_RE_ANSWER.getEventSpecificInformationBCSMChoice())
			setTReAnswer(specificInfo);			
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.FACILITY_SELECTED_AND_AVAILABLE.getEventSpecificInformationBCSMChoice())
			setFacilitySelectedAndAvailable(specificInfo);
		else if(eventSpecificInfoID.getEventSpecificInformationBCSMChoice() == EventSpecificInformationBCSMChoice.CALL_ACCEPTED.getEventSpecificInformationBCSMChoice())
			setCallAccepted(specificInfo);	
	}

//-----------------------------------

/**
Gets  EventSpecificChoice
*/

    public EventSpecificInformationBCSMChoice  getEventSpecificChoice()
    {
        return eventSpecificChoice;
    }
//---------------------------------------

/**
Gets  collectedInfoSpecificInfo
*/

    public CalledPartyNumber getCollectedInfoSpecificInfo()
    {
        return collectedInfoSpecificInfo ;
    }
/**
Sets  collectedInfoSpecificInfo
*/

    public void  setCollectedInfoSpecificInfo(CalledPartyNumber collectedInfoSpecificInfo)
    {
        this.collectedInfoSpecificInfo = collectedInfoSpecificInfo ;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.COLLECTED_INFO_SPECIFIC_INFO;
    }
//---------------------------------------

/**
Gets analysedInfoSpecificInfo
*/

	public CalledPartyNumber getAnalysedInfoSpecificInfo()
	{
		return analysedInfoSpecificInfo;
	}
/**
Sets analysedInfoSpecificInfo
*/
	public void setAnalysedInfoSpecificInfo(CalledPartyNumber analysedInfoSpecificInfo)
	{
		this.analysedInfoSpecificInfo = analysedInfoSpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.ANALYSED_INFO_SPECIFIC_INFO;
	}
	
//---------------------------------------		
	
/**
Gets routeSelectFailureSpecificInfo
*/

	public Cause getRouteSelectFailureSpecificInfo()
	{
		return routeSelectFailureSpecificInfo;
	}
/**
Sets routeSelectFailureSpecificInfo
*/
	public void setRouteSelectFailureSpecificInfo(Cause routeSelectFailureSpecificInfo)
	{
		this.routeSelectFailureSpecificInfo = routeSelectFailureSpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.ROUTE_SELECT_FAILURE_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets oCalledPartyBusySpecificInfo
*/

	public Cause getOCalledPartyBusySpecificInfo()
	{
		return oCalledPartyBusySpecificInfo;
	}
/**
Sets oCalledPartyBusySpecificInfo
*/
	public void setOCalledPartyBusySpecificInfo(Cause oCalledPartyBusySpecificInfo)
	{
		this.oCalledPartyBusySpecificInfo = oCalledPartyBusySpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.O_CALLED_PARTY_BUSY_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets oNoAnswerSpecificInfo
*/

	public java.lang.String getONoAnswerSpecificInfo()
	{
		return oNoAnswerSpecificInfo;
	}
/**
Sets oNoAnswerSpecificInfo
*/
	public void setONoAnswerSpecificInfo(java.lang.String oNoAnswerSpecificInfo)
	{
		this.oNoAnswerSpecificInfo = oNoAnswerSpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.O_NO_ANSWER_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets oAnswerSpecificInfo
*/

	public BackwardGVNS getOAnswerSpecificInfo()
	{
		return oAnswerSpecificInfo;
	}
/**
Sets oAnswerSpecificInfo
*/
	public void setOAnswerSpecificInfo(BackwardGVNS oAnswerSpecificInfo)
	{
		this.oAnswerSpecificInfo = oAnswerSpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.O_ANSWER_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets oMidCallSpecificInfo
*/

	public MidCallSpecificInfo getOMidCallSpecificInfo()
	{
		return oMidCallSpecificInfo;
	}
/**
Sets oMidCallSpecificInfo
*/
	public void setOMidCallSpecificInfo(MidCallSpecificInfo oMidCallSpecificInfo)
	{
		this.oMidCallSpecificInfo = oMidCallSpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.O_MID_CALL_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets oDisconnectSpecificInfo
*/

	public DisconnectSpecificInfo getODisconnectSpecificInfo()
	{
		return oDisconnectSpecificInfo;
	}
/**
Sets oDisconnectSpecificInfo
*/
	public void setODisconnectSpecificInfo(DisconnectSpecificInfo oDisconnectSpecificInfo)
	{
		this.oDisconnectSpecificInfo = oDisconnectSpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.O_DISCONNECT_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets tBusySpecificInfo
*/

	public Cause getTBusySpecificInfo()
	{
		return tBusySpecificInfo;
	}
/**
Sets tBusySpecificInfo
*/
	public void setTBusySpecificInfo(Cause tBusySpecificInfo)
	{
		this.tBusySpecificInfo = tBusySpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.T_BUSY_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets tNoAnswerSpecificInfo
*/

	public java.lang.String getTNoAnswerSpecificInfo()
	{
		return tNoAnswerSpecificInfo;
	}
/**
Sets tNoAnswerSpecificInfo
*/
	public void setTNoAnswerSpecificInfo(java.lang.String tNoAnswerSpecificInfo)
	{
		this.tNoAnswerSpecificInfo = tNoAnswerSpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.T_NO_ANSWER_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets tAnswerSpecificInfo
*/

	public java.lang.String getTAnswerSpecificInfo()
	{
		return tAnswerSpecificInfo;
	}
/**
Sets tAnswerSpecificInfo
*/
	public void setTAnswerSpecificInfo(java.lang.String tAnswerSpecificInfo)
	{
		this.tAnswerSpecificInfo = tAnswerSpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.T_ANSWER_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets tMidCallSpecificInfo
*/

	public MidCallSpecificInfo getTMidCallSpecificInfo()
	{
		return tMidCallSpecificInfo;
	}
/**
Sets tMidCallSpecificInfo
*/
	public void setTMidCallSpecificInfo(MidCallSpecificInfo tMidCallSpecificInfo)
	{
		this.tMidCallSpecificInfo = tMidCallSpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.T_MID_CALL_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets tDisconnectSpecificInfo
*/

	public DisconnectSpecificInfo getTDisconnectSpecificInfo()
	{
		return tDisconnectSpecificInfo;
	}
/**
Sets tDisconnectSpecificInfo
*/
	public void setTDisconnectSpecificInfo(DisconnectSpecificInfo tDisconnectSpecificInfo)
	{
		this.tDisconnectSpecificInfo = tDisconnectSpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.T_DISCONNECT_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets oTermSeizedSpecificInfo
*/

	public java.lang.String getOTermSeizedSpecificInfo()
	{
		return oTermSeizedSpecificInfo;
	}
/**
Sets oTermSeizedSpecificInfo
*/
	public void setOTermSeizedSpecificInfo(java.lang.String oTermSeizedSpecificInfo)
	{
		this.oTermSeizedSpecificInfo = oTermSeizedSpecificInfo;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.O_TERM_SEIZED_SPECIFIC_INFO;
	}
	
//---------------------------------------		

/**
Gets oSuspended
*/

	public java.lang.String getOSuspended()
	{
		return oSuspended;
	}
/**
Sets oSuspended
*/
	public void setOSuspended(java.lang.String oSuspended)
	{
		this.oSuspended = oSuspended;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.O_SUSPENDED;
	}
	
//---------------------------------------		

/**
Gets tSuspended
*/

	public java.lang.String getTSuspended()
	{
		return tSuspended;
	}
/**
Sets tSuspended
*/
	public void setTSuspended(java.lang.String tSuspended)
	{
		this.tSuspended = tSuspended;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.T_SUSPENDED;
	}
	
//---------------------------------------		

/**
Gets origAttemptAuthorized
*/

	public java.lang.String getOrigAttemptAuthorized()
	{
		return origAttemptAuthorized;
	}
/**
Sets origAttemptAuthorized
*/
	public void setOrigAttemptAuthorized(java.lang.String origAttemptAuthorized)
	{
		this.origAttemptAuthorized = origAttemptAuthorized;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.ORIG_ATTEMPT_AUTHORIZED;
	}
	
//---------------------------------------		

/**
Gets oReAnswer
*/

	public java.lang.String getOReAnswer()
	{
		return oReAnswer;
	}
/**
Sets oReAnswer
*/
	public void setOReAnswer(java.lang.String oReAnswer)
	{
		this.oReAnswer = 	oReAnswer;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.O_RE_ANSWER;
	}
	
//---------------------------------------		

/**
Gets tReAnswer
*/

	public java.lang.String getTReAnswer()
	{
		return tReAnswer;
	}
/**
Sets tReAnswer
*/
	public void setTReAnswer(java.lang.String tReAnswer)
	{
		this.tReAnswer = tReAnswer;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.T_RE_ANSWER;
	}
	
//---------------------------------------		

/**
Gets facilitySelectedAndAvailable
*/

	public java.lang.String getFacilitySelectedAndAvailable()
	{
		return facilitySelectedAndAvailable;
	}
/**
Sets facilitySelectedAndAvailable
*/
	public void setFacilitySelectedAndAvailable(java.lang.String facilitySelectedAndAvailable)
	{
		this.facilitySelectedAndAvailable = facilitySelectedAndAvailable;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.FACILITY_SELECTED_AND_AVAILABLE;
	}
	
//---------------------------------------		

/**
Gets callAccepted
*/

	public java.lang.String getCallAccepted()
	{
		return callAccepted;
	}
/**
Sets callAccepted
*/
	public void setCallAccepted(java.lang.String callAccepted)
	{
		this.callAccepted = callAccepted;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.CALL_ACCEPTED;
	}
	
//---------------------------------------		

/**
Gets oAbandon
*/

	public Cause getOAbandon()
	{
		return oAbandon;
	}
/**
Sets oAbandon
*/
	public void setOAbandon(Cause oAbandon)
	{
		this.oAbandon = oAbandon;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.O_ABANDON;
	}
	
//---------------------------------------		

/**
Gets tAbandon
*/

	public Cause getTAbandon()
	{
		return tAbandon;
	}
/**
Sets tAbandon
*/
	public void setTAbandon(Cause tAbandon)
	{
		this.tAbandon = tAbandon;
		eventSpecificChoice = EventSpecificInformationBCSMChoice.T_ABANDON;
	}
	
//---------------------------------------		


}


