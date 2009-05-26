/******************************************************************************
*                                                                             *
*                                                                             *
* Copyright (c) SS8 Networks, Inc.                                            *
* All rights reserved.                                                        *
*                                                                             *
* This document contains confidential and proprietary information in which    *
* any reproduction, disclosure, or use in whole or in part is expressly       *
* prohibited, except as may be specifically authorized by prior written       *
* agreement or permission of SS8 Networks, Inc.                               *
*                                                                             *
*******************************************************************************
* VERSION      : $Revision: 1.1 $
* DATE         : $Date: 2008/05/16 12:24:02 $
* 
* MODULE NAME  : $RCSfile: InformationEvent.java,v $
* AUTHOR       : Nilgun Baykal [SS8]
* DESCRIPTION  : 
* DATE 1st REL : 
* REV.HIST.    : 
* 
* Date      Owner  Description
* ========  =====  ===========================================================
* 
* 
*******************************************************************************
*                                                                             *
*                     RESTRICTED RIGHTS LEGEND                                *
* Use, duplication, or disclosure by Government Is Subject to restrictions as *
* set forth in subparagraph (c)(1)(ii) of the Rights in Technical Data and    *
* Computer Software clause at DFARS 252.227-7013                              *
*                                                                             *
******************************************************************************/


package javax.jain.ss7.isup;

import javax.jain.*;
import javax.jain.ss7.*;

public abstract class InformationEvent extends IsupEvent{


	protected InformationEvent(java.lang.Object source,
                           SignalingPointCode dpc,
                           SignalingPointCode opc,
                           byte sls,
                           int cic,
                           byte congestionPriority,
                           InfoInd in_infoInd)
                    throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		infoInd = in_infoInd;
		m_infoIndPresent = true;
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		if(m_infoIndPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();		
	}
		
	public byte getCallingPartyCat()
                        throws ParameterNotSetException{
		if(isCallingPartyCat == true)
				return callingPartyCat;
		else throw new ParameterNotSetException();
	}
		
	public boolean isCallingPartyCatPresent(){
		return isCallingPartyCat;

	}

	public void setCallingPartyCat(byte cat)
                        throws ParameterRangeInvalidException{
		 if ((cat >= CPC_UNKNOWN) &&
		  (cat <= CPC_NATIONAL_SECURITY_EMERGENCY_PREPAREDNESS_CALL)) {
			callingPartyCat = cat;
			isCallingPartyCat = true;
		 }
		 else 
			throw new ParameterRangeInvalidException();

	}
		
	public CallingPartyNumber getCallingPartyNumber()
                                    throws ParameterNotSetException{
		if(isCallingPartyNumber == true)
				return callingPartyNumber;
		else throw new ParameterNotSetException();
	}

	public boolean isCallingPartyNumberPresent(){
		return isCallingPartyNumber;
	}

	public void setCallingPartyNumber(
				CallingPartyNumber callingPartyNum){
		callingPartyNumber = callingPartyNum;
		isCallingPartyNumber = true; 
	}

	public CallReference getCallReference()
                               throws ParameterNotSetException{
		if(isCallReference == true)
				return callReference;
		else throw new ParameterNotSetException();
	}

	public boolean isCallReferencePresent(){
		return isCallReference;
	}

	public void setCallReference(CallReference callRef){
		callReference = callRef;
		isCallReference = true;
	}

	public ConReq getConReq()
                 throws ParameterNotSetException{
		if(isConReq == true)
				return conReq;
		else throw new ParameterNotSetException();
	}

	public boolean isConReqPresent(){
		return isConReq; 
	}

	public void setConReq(ConReq connectionReq){
		conReq = connectionReq;
		isConReq = true;
	}

	public InfoInd getInfoInd()
                   throws MandatoryParameterNotSetException{
		if(m_infoIndPresent == true)
			return infoInd;
		else throw  new MandatoryParameterNotSetException();
	}
		
	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_INFORMATION;
	}	

	public void setInfoInd(InfoInd infomationInd){
		infoInd = infomationInd;
		m_infoIndPresent = true;
	}

	/**
    * String representation of class InformationEvent
    *
    * @return    String provides description of class InformationEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisCallingPartyCat = ");
				buffer.append(isCallingPartyCat);
				buffer.append("\ncallingPartyCat = ");
				buffer.append(callingPartyCat);
				buffer.append("\n\nisCallingPartyNumber = ");
				buffer.append(isCallingPartyNumber);
				buffer.append("\ncallingPartyNumber = ");
				buffer.append(callingPartyNumber);
				buffer.append("\n\nisCallReference = ");
				buffer.append(isCallReference);
				buffer.append("\ncallReference = ");
				buffer.append(callReference);
				buffer.append("\n\nisConReq = ");
				buffer.append(isConReq);
				buffer.append("\nconReq = ");
				buffer.append(conReq);
				buffer.append("\n\ninfoInd = ");
				buffer.append(infoInd);
				return buffer.toString();
		
		}

	
	byte               callingPartyCat;
	CallingPartyNumber callingPartyNumber;
	CallReference      callReference;
	ConReq             conReq;
	InfoInd            infoInd;
	boolean            isCallingPartyCat;
	boolean            isCallingPartyNumber;
	boolean            isCallReference;
	boolean            isConReq;

	private boolean m_infoIndPresent = false;

	public static final int CPC_UNKNOWN = 0; 
	public static final int CPC_NATIONAL_SECURITY_EMERGENCY_PREPAREDNESS_CALL = 0xE2; 	
}


