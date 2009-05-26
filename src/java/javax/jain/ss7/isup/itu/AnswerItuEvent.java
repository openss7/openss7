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
* DATE         : $Date: 2008/05/16 12:23:53 $
* 
* MODULE NAME  : $RCSfile: AnswerItuEvent.java,v $
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

public class AnswerItuEvent extends AnswerEvent{


	public AnswerItuEvent(java.lang.Object source,
                      SignalingPointCode dpc,
                      SignalingPointCode opc,
                      byte sls,
                      int cic,
                      byte congestionPriority)
               throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		
		
	}

	public byte getAccessDeliveryInfo()
                           throws ParameterNotSetException{
		if(isAccessDeliveryInfo == true)
			return accessDeliveryInfo;
		else throw new ParameterNotSetException();
	}
	
	public void setAccessDeliveryInfo(byte adi){
		accessDeliveryInfo   = adi; 
		isAccessDeliveryInfo = true;
	}

	public boolean isAccessDeliveryInfoPresent(){
		return isAccessDeliveryInfo;
	}
	
	public long getCallHistoryInfo()
                        throws ParameterNotSetException{
		if(isCallHistoryInfo == true)
			return callHistoryInfo;
		else throw new ParameterNotSetException();
	}

	public void setCallHistoryInfo(long callHistInfo){
		callHistoryInfo   = callHistInfo;
		isCallHistoryInfo = true;
	}

	public boolean isCallHistoryInfoPresent(){
		return isCallHistoryInfo;
	}

	public ConnectedNumberItu getConnectedNumber()
                                      throws ParameterNotSetException{
		if(isConnectedNumberItu == true)
			return connectedNumberItu;
		else throw new ParameterNotSetException();
	}

	public void setConnectedNumber(ConnectedNumberItu connNumItu){
		connectedNumberItu   = connNumItu;
		isConnectedNumberItu = true;
	}

	public boolean isConnectedNumberPresent(){
		return isConnectedNumberItu;
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

	public GenericNumberItu getGenericNumber()
                                  throws ParameterNotSetException{
		if(isGenericNumberItu == true)
			return 	genericNumberItu;
		else throw new ParameterNotSetException();
	}

	public void setGenericNumber(GenericNumberItu genNumItu){
		genericNumberItu   = genNumItu;
		isGenericNumberItu = true;
	}

	public boolean isGenericNumberPresent(){
		return isGenericNumberItu;
	}

	public NwSpecificFacItu getNwSpecificFac()
                                  throws ParameterNotSetException{
		if(isNwSpecificFacItu == true)
			return nwSpecificFacItu;
		else throw new ParameterNotSetException();
	}
	
	public void setNwSpecificFac(NwSpecificFacItu nsf){
		nwSpecificFacItu   = nsf;
		isNwSpecificFacItu = true;
	}

	public boolean isNwSpecificFacPresent(){
		return isNwSpecificFacItu;
	}

	public ParamCompatibilityInfoItu getParamCompatibilityInfo()
                                        throws ParameterNotSetException{
		if(isParamCompatibilityInfoItu == true)
			return paramCompatibilityInfoItu;
		else throw new ParameterNotSetException();
	}

	public void setParamCompatibilityInfo(ParamCompatibilityInfoItu pci){
		paramCompatibilityInfoItu   = pci;
		isParamCompatibilityInfoItu = true;
	}

	public boolean isParamCompatibilityInfoPresent(){
		return isParamCompatibilityInfoItu;
	}

	public RedirectionNumberItu getRedirectionNumber()
                                          throws ParameterNotSetException{
		if(isRedirectionNumberItu == true)
			return redirectionNumberItu;
		else throw new ParameterNotSetException();
	}

	public void setRedirectionNumber(RedirectionNumberItu rn){
		redirectionNumberItu   = rn;
		isRedirectionNumberItu = true;
	}

	public boolean isRedirectionNumberPresent(){
		return isRedirectionNumberItu;
	}
		
	public byte getRedirectionNumberRest()
                              throws ParameterNotSetException{
		if(isRedirectionNumberRest == true)
			return redirectionNumberRest;
		else throw new ParameterNotSetException();
		
	}

	public void setRedirectionNumberRest(byte rnr){
		redirectionNumberRest   = rnr;
		isRedirectionNumberRest =  true;
	}

	public boolean isRedirectionNumberRestPresent(){
		return isRedirectionNumberRest;
	}
	


	/**
    * String representation of class AnswerItuEvent
    *
    * @return    String provides description of class AnswerItuEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisAccessDeliveryInfo = ");
				buffer.append(isAccessDeliveryInfo);
				buffer.append("\naccessDeliveryInfo = ");
				buffer.append(accessDeliveryInfo);
				buffer.append("\n\nisCallHistoryInfo = ");
				buffer.append(isCallHistoryInfo);
				buffer.append("\ncallHistoryInfo  = ");
				buffer.append(callHistoryInfo);
				buffer.append("\n\nisConnectedNumberItu = ");
				buffer.append(isConnectedNumberItu);
				buffer.append("\nconnectedNumberItu = ");
				buffer.append(connectedNumberItu);
				buffer.append("\n\nisEchoControlInfoItu = ");
				buffer.append(isEchoControlInfoItu);
				buffer.append("\nechoControlInfoItu = ");
				buffer.append(echoControlInfoItu);
				buffer.append("\n\nisGenericNumberItu = ");
				buffer.append(isGenericNumberItu);
				buffer.append("\ngenericNumberItu = ");
				buffer.append(genericNumberItu);
				buffer.append("\n\nisNwSpecificFacItu = ");
				buffer.append(isNwSpecificFacItu);
				buffer.append("\nnwSpecificFacItu = ");
				buffer.append(nwSpecificFacItu);
				buffer.append("\n\nisParamCompatibilityInfoItu = ");
				buffer.append(isParamCompatibilityInfoItu);
				buffer.append("\nparamCompatibilityInfoItu = ");
				buffer.append(paramCompatibilityInfoItu);	
				buffer.append("\n\nisRedirectionNumberItu = ");
				buffer.append(isRedirectionNumberItu);
				buffer.append("\nredirectionNumberItu = ");
				buffer.append(redirectionNumberItu);
				buffer.append("\n\nisRedirectionNumberRest = ");
				buffer.append(isRedirectionNumberRest);
				buffer.append("\nredirectionNumberRest = ");
				buffer.append(redirectionNumberRest);						
				return buffer.toString();
		
		}

	byte                      accessDeliveryInfo;
	long                      callHistoryInfo;
	ConnectedNumberItu        connectedNumberItu;
	EchoControlInfoItu        echoControlInfoItu;
	GenericNumberItu          genericNumberItu;
	boolean                   isAccessDeliveryInfo;
	boolean                   isCallHistoryInfo;
	boolean                   isConnectedNumberItu;
	boolean                   isEchoControlInfoItu;
	boolean                   isGenericNumberItu;
	boolean                   isNwSpecificFacItu;
	boolean                   isParamCompatibilityInfoItu;
	boolean                   isRedirectionNumberItu;
	boolean                   isRedirectionNumberRest;
	NwSpecificFacItu          nwSpecificFacItu;
	ParamCompatibilityInfoItu paramCompatibilityInfoItu;
	RedirectionNumberItu      redirectionNumberItu;
	byte                      redirectionNumberRest;




    
}


