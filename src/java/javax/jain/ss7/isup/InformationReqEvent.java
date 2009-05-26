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
* MODULE NAME  : $RCSfile: InformationReqEvent.java,v $
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

public abstract class InformationReqEvent extends IsupEvent{


	protected InformationReqEvent(java.lang.Object source,
                              SignalingPointCode dpc,
                              SignalingPointCode opc,
                              byte sls,
                              int cic,
                              byte congestionPriority,
                              InfoReqInd in_infoReqInd)
                       throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		infoReqInd = in_infoReqInd;
		m_infoReqIndPresent = true;
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		if(m_infoReqIndPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();
		
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

	public InfoReqInd getInfoReqInd()
                         throws MandatoryParameterNotSetException{
		if(m_infoReqIndPresent == true)
			return infoReqInd;
		else throw new MandatoryParameterNotSetException();
	}

	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_INFORMATION_REQ;
	}

	public void setInfoReqInd(InfoReqInd infomationReqInd){
		infoReqInd = infomationReqInd;
		m_infoReqIndPresent = true;
	}

	/**
    * String representation of class InformationReqEvent
    *
    * @return    String provides description of class InformationReqEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisCallReference = ");
				buffer.append(isCallReference);
				buffer.append("\ncallReference = ");
				buffer.append(callReference);
				buffer.append("\n\ninfoReqInd = ");
				buffer.append(infoReqInd);
				return buffer.toString();
		
		}

	CallReference callReference;
	InfoReqInd    infoReqInd;
	boolean       isCallReference;

	private boolean m_infoReqIndPresent = false;

	
}


