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
* DATE         : $Date: 2008/05/16 12:23:58 $
* 
* MODULE NAME  : $RCSfile: NetworkResourceMgmtItuEvent.java,v $
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

public class NetworkResourceMgmtItuEvent extends IsupEvent{

	public NetworkResourceMgmtItuEvent(java.lang.Object source,
                                   SignalingPointCode dpc,
                                   SignalingPointCode opc,
                                   byte sls,
                                   int cic,
                                   byte congestionPriority)
                            throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
				
	}
	

	public int getIsupPrimitive(){

		return IsupConstants.ISUP_PRIMITIVE_NETWORK_RESOURCE_MGMT; 

	}

	public EchoControlInfoItu getEchoControlInfo()
                                      throws ParameterNotSetException{
		if(isEchoControlInfoItu == true)
			return echoControlInfoItu;
		else throw new ParameterNotSetException();

	}

	public void setEchoControlInfo(EchoControlInfoItu eci){
		echoControlInfoItu   = eci;
		isEchoControlInfoItu = true;
	}

	public boolean isEchoControlInfoPresent(){
		return isEchoControlInfoItu;
	}

	public MessageCompatibilityInfoItu getMessageCompatibilityInfo()
                                                        throws ParameterNotSetException{
		if(isMessageCompInfoItu == true)
			return messageCompInfoItu;
		else throw new ParameterNotSetException();
	}

	public void setMessageCompatibilityInfo
								(MessageCompatibilityInfoItu in_MessageCompInfo){
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

	public byte getTransmissionMediumUsed()
                               throws ParameterNotSetException{
		if(isTransmissionMediumUsed  == true)
			return transmissionMediumUsed;
		else throw new ParameterNotSetException();

	}

	public void setTransmissionMediumUsed(byte tmu)
                               throws ParameterRangeInvalidException{
		
		if ((tmu == TMR_SPEECH) || (tmu >= TMR_64_KBPS_UNRESTRICTED) && (tmu <= TMR_1920_KBPS_UNRESTRICTED)) {
			transmissionMediumUsed   = tmu;
			isTransmissionMediumUsed = true;
		}
		else 
			throw new ParameterRangeInvalidException();
	}

	public boolean isTransmissionMediumUsedPresent(){
		return isTransmissionMediumUsed;
	}

	/**
    * String representation of class NetworkResourceMgmtItuEvent 
    *
    * @return    String provides description of class NetworkResourceMgmtItuEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisEchoControlInfoItu = ");
				buffer.append(isEchoControlInfoItu);	
				buffer.append("\nechoControlInfoItu = ");
				buffer.append(echoControlInfoItu);	
				buffer.append("\n\nisMessageCompInfoItu = ");
				buffer.append(isMessageCompInfoItu);	
				buffer.append("\nmessageCompInfoItu = ");
				buffer.append(messageCompInfoItu);
				buffer.append("\n\nisParamCompInfoItu = ");
				buffer.append(isParamCompInfoItu);	
				buffer.append("\nparamCompInfoItu = ");
				buffer.append(paramCompInfoItu);
				buffer.append("\n\nisTransmissionMediumUsed = ");
				buffer.append(isTransmissionMediumUsed);	
				buffer.append("\ntransmissionMediumUsed  = ");
				buffer.append(transmissionMediumUsed);									
				return buffer.toString();
		
		}

	


	EchoControlInfoItu          echoControlInfoItu;
	boolean                     isEchoControlInfoItu;
	boolean                     isMessageCompInfoItu;
	boolean                     isParamCompInfoItu;
	boolean                     isTransmissionMediumUsed;
	MessageCompatibilityInfoItu messageCompInfoItu;
	ParamCompatibilityInfoItu   paramCompInfoItu;
	byte                        transmissionMediumUsed;

	public static final byte TMR_SPEECH = 0x00; 
	public static final byte TMR_64_KBPS_UNRESTRICTED = 0x02; 
	public static final byte TMR_1920_KBPS_UNRESTRICTED = 0x0A;    

    
}


