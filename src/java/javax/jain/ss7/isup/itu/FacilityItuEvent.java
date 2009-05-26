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
* DATE         : $Date: 2008/05/16 12:23:55 $
* 
* MODULE NAME  : $RCSfile: FacilityItuEvent.java,v $
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

public class FacilityItuEvent extends FacilityEvent{

	public FacilityItuEvent(java.lang.Object source,
                        SignalingPointCode dpc,
                        SignalingPointCode opc,
                        byte sls,
                        int cic,
                        byte congestionPriority)
                 throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
	}

	public MessageCompatibilityInfoItu getMessageCompatibilityInfo()
                                                        throws ParameterNotSetException{
		if(	isMessageCompInfoItu == true)
			return messageCompInfoItu;
		else throw new ParameterNotSetException();
	}
	public void setMessageCompatibilityInfo(
		MessageCompatibilityInfoItu in_MessageCompInfoItu){

		messageCompInfoItu   = in_MessageCompInfoItu;
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
    * String representation of class FacilityItuEvent
    *
    * @return    String provides description of class FacilityItuEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisMessageCompInfoItu = ");
				buffer.append(isMessageCompInfoItu);
				buffer.append("\nmessageCompInfoItu = ");
				buffer.append(messageCompInfoItu);
				buffer.append("\n\nisParamCompInfoItu = ");
				buffer.append(isParamCompInfoItu);
				buffer.append("\nparamCompInfoItu  = ");
				buffer.append(paramCompInfoItu);			
				return buffer.toString();
		
		}

	


	boolean                     isMessageCompInfoItu;
	boolean                     isParamCompInfoItu;
	MessageCompatibilityInfoItu messageCompInfoItu;
	ParamCompatibilityInfoItu   paramCompInfoItu;


    
}


