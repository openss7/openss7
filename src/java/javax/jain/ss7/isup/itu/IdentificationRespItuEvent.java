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
* MODULE NAME  : $RCSfile: IdentificationRespItuEvent.java,v $
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

public class IdentificationRespItuEvent extends IsupEvent{
		
	public IdentificationRespItuEvent(java.lang.Object source,
                                  SignalingPointCode dpc,
                                  SignalingPointCode opc,
                                  byte sls,
                                  int cic,
                                  byte congestionPriority)
                           throws ParameterRangeInvalidException{


		super(source,dpc,opc,sls,cic,congestionPriority);
	}

	public int getIsupPrimitive(){

		return IsupConstants.ISUP_PRIMITIVE_IDENTIFICATION_RESP; 

	}
		
	public byte[] getAccessTransport()
                          throws ParameterNotSetException{
		if(isAccessTransport == true)
			return accessTransport;
		else throw new ParameterNotSetException();
	}

	public void setAccessTransport(byte[] accessTrans){
		accessTransport   = accessTrans;
		isAccessTransport = true;
	}

	public boolean isAccessTransportPresent(){
		return isAccessTransport;
	}

	public CallingPartyNumberItu getCallingPartyNumber()
                                            throws ParameterNotSetException{

		if(isCallingPartyNumberItu == true)
			return callingPartyNumberItu;
		else throw new ParameterNotSetException();
	}

	public void setCallingPartyNumber(CallingPartyNumberItu callingPartyNumItu){
		callingPartyNumberItu   = callingPartyNumItu;
		isCallingPartyNumberItu = true;
	}

	public boolean isCallingPartyNumberPresent(){
		return isCallingPartyNumberItu;
	}

	public GenericNumberItu getGenericNumber()
                                  throws ParameterNotSetException{

		if(isGenericNumberItu == true)
			return genericNumberItu;
		else throw new ParameterNotSetException();
	}

	public void setGenericNumber(GenericNumberItu genNum){
		genericNumberItu   = genNum;
		isGenericNumberItu = true;
	}

	public boolean isGenericNumberPresent(){
		return isGenericNumberItu;
	}

	public MCIDRespIndItu getMCIDRespInd()
                              throws ParameterNotSetException{
		if(isMCIDRespIndItu == true)
			return mcidRespIndItu;
		else throw new ParameterNotSetException();
		
	}

	public void setMCIDRespInd(MCIDRespIndItu mcidResponseIndicator){

		mcidRespIndItu   = mcidResponseIndicator;
		isMCIDRespIndItu = true;
	}

	public boolean isMCIDRespIndPresent(){
		return isMCIDRespIndItu;
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
    * String representation of class IdentificationRespItuEvent
    *
    * @return    String provides description of class IdentificationRespItuEvent
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisAccessTransport = ");
				buffer.append(isAccessTransport);
				if(isAccessTransport == true){
					buffer.append("\naccessTransport = ");
					for(i=0;i<accessTransport.length;i++)
						buffer.append(" "+Integer.toHexString((int)(accessTransport[i] & 0xFF)));
				}
				buffer.append("\n\nisCallingPartyNumberItu = ");
				buffer.append(isCallingPartyNumberItu);
				buffer.append("\ncallingPartyNumberItu = ");
				buffer.append(callingPartyNumberItu);
				buffer.append("\n\nisGenericNumberItu = ");
				buffer.append(isGenericNumberItu);
				buffer.append("\ngenericNumberItu = ");
				buffer.append(genericNumberItu);	
				buffer.append("\n\nisMCIDRespIndItu = ");
				buffer.append(isMCIDRespIndItu);
				buffer.append("\nmcidRespIndItu = ");
				buffer.append(mcidRespIndItu);	
				buffer.append("\n\nisMessageCompInfoItu = ");
				buffer.append(isMessageCompInfoItu);
				buffer.append("\nmessageCompInfoItu = ");
				buffer.append(messageCompInfoItu);
				buffer.append("\n\nisParamCompInfoItu = ");
				buffer.append(isParamCompInfoItu);
				buffer.append("\nparamCompInfoItu = ");
				buffer.append(paramCompInfoItu);					
				return buffer.toString();
		
		}

	
	byte[]                      accessTransport;
	CallingPartyNumberItu       callingPartyNumberItu;
	GenericNumberItu            genericNumberItu;
	boolean                     isAccessTransport;
	boolean                     isCallingPartyNumberItu;
	boolean                     isGenericNumberItu;
	boolean                     isMCIDRespIndItu;
	boolean                     isMessageCompInfoItu;
	boolean                     isParamCompInfoItu;
	MCIDRespIndItu              mcidRespIndItu;
	MessageCompatibilityInfoItu messageCompInfoItu;
	ParamCompatibilityInfoItu   paramCompInfoItu;
    
}


