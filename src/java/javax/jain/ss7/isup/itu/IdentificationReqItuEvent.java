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
* DATE         : $Date: 2008/05/16 12:23:56 $
* 
* MODULE NAME  : $RCSfile: IdentificationReqItuEvent.java,v $
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


package javax.jain.ss7.isup.itu;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class IdentificationReqItuEvent extends IsupEvent{

	public IdentificationReqItuEvent(java.lang.Object source,
                                 SignalingPointCode dpc,
                                 SignalingPointCode opc,
                                 byte sls,
                                 int cic,
                                 byte congestionPriority)
                          throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
				
	}
	

	public int getIsupPrimitive(){

		return IsupConstants.ISUP_PRIMITIVE_IDENTIFICATION_REQ; 

	}

	public MCIDReqIndItu getMCIDReqInd()
                            throws ParameterNotSetException{
		if(isMCIDReqIndItu == true)
			return mcidReqIndItu;
		else throw new ParameterNotSetException();
	}

	public void setMCIDReqInd(MCIDReqIndItu mcidRequestIndicator){
		mcidReqIndItu   = mcidRequestIndicator;
		isMCIDReqIndItu = true;
	}

	public boolean isMCIDReqIndPresent(){
		return isMCIDReqIndItu;
	}

	public MessageCompatibilityInfoItu getMessageCompatibilityInfo()
                                                        throws ParameterNotSetException{
		if(isMessageCompInfoItu == true)
			return messageCompInfoItu;
		else throw new ParameterNotSetException();
	}

	public void setMessageCompatibilityInfo(MessageCompatibilityInfoItu in_MessageCompInfo){
		messageCompInfoItu   = in_MessageCompInfo;
		isMessageCompInfoItu = true;
	}

	public boolean isMessageCompatibilityInfoPresent(){
		return isMessageCompInfoItu;
	}

	public ParamCompatibilityInfoItu getParamCompatibilityInfo()
                                                    throws ParameterNotSetException{

		if(isParamCompInfoItu == true)
			return paramCompInfoItu;
		else throw new ParameterNotSetException();
	}

	public void setParamCompatibilityInfo(ParamCompatibilityInfoItu in_paramCompInfo){
		
		paramCompInfoItu   = in_paramCompInfo;
		isParamCompInfoItu = true;
	}

	public boolean isParamCompatibilityInfoPresent(){
		return isParamCompInfoItu;
	}

	/**
    * String representation of class IdentificationReqItuEvent
    *
    * @return    String provides description of class IdentificationReqItuEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisMCIDReqIndItu = ");
				buffer.append(isMCIDReqIndItu);	
				buffer.append("\nmcidReqIndItu = ");
				buffer.append(mcidReqIndItu);	
				buffer.append("\n\nisMessageCompInfoItu = ");
				buffer.append(isMessageCompInfoItu);	
				buffer.append("\nmessageCompInfoItu = ");
				buffer.append(messageCompInfoItu);
				buffer.append("\n\nparamCompInfoItu = ");
				buffer.append(paramCompInfoItu);					
				return buffer.toString();
		
		}

	

	boolean                     isMCIDReqIndItu;
	boolean                     isMessageCompInfoItu;
	boolean                     isParamCompInfoItu;
	MCIDReqIndItu               mcidReqIndItu;
	MessageCompatibilityInfoItu messageCompInfoItu;
	ParamCompatibilityInfoItu   paramCompInfoItu;
    
}


