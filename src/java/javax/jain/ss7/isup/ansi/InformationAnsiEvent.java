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
* DATE         : $Date: 2008/05/16 12:24:10 $
* 
* MODULE NAME  : $RCSfile: InformationAnsiEvent.java,v $
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


package javax.jain.ss7.isup.ansi;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class InformationAnsiEvent extends InformationEvent{

	public InformationAnsiEvent(java.lang.Object source,
                            SignalingPointCode dpc,
                            SignalingPointCode opc,
                            byte sls,
                            int cic,
                            byte congestionPriority,
                            InfoIndAnsi in_infoInd)
                     throws ParameterRangeInvalidException{
	


		super(source,dpc,opc,sls,cic,congestionPriority,in_infoInd);
		

	}

	public byte[] getAccessTransport()
                          throws ParameterNotSetException{
		if(isAccessTransport == true)
			return accessTransport;
		else throw new ParameterNotSetException();
	}

	public void setAccessTransport(byte[] in_accessTransport){
		accessTransport = in_accessTransport;
		isAccessTransport  = true;
	}

	public boolean isAccessTransportPresent(){
		return isAccessTransport;
	}

	public BusinessGroupAnsi getBusinessGroup()
                                   throws ParameterNotSetException{
		if(isBusinessGroupAnsi == true)
			return businessGroupAnsi;
		else throw new ParameterNotSetException();
	}
	public void setBusinessGroup(BusinessGroupAnsi bgp){
		businessGroupAnsi = bgp;
		isBusinessGroupAnsi = true;
	}
	public boolean isBusinessGroupPresent(){
		return isBusinessGroupAnsi;
	}
	
	public ChargeNumberAnsi getChargeNumber()
                                 throws ParameterNotSetException{
		if(isChargeNumberAnsi == true)
			return chargeNumberAnsi;
		else throw new ParameterNotSetException();
	}

	public void setChargeNumber(ChargeNumberAnsi chargeNumAnsi){
		chargeNumberAnsi   = chargeNumAnsi;
		isChargeNumberAnsi = true;
	}

	public boolean isChargeNumberPresent(){
		return isChargeNumberAnsi;
	}

	public byte getOrigLineInfo()
                     throws ParameterNotSetException{
		if(isOrigLineInfo == true)
			return origLineInfo;
		else throw new ParameterNotSetException();
	}

	public void setOrigLineInfo(byte oli)
                     throws ParameterRangeInvalidException{
		origLineInfo   = oli;
		isOrigLineInfo = true;
	}

	public boolean isOrigLineInfoPresent(){
		return isOrigLineInfo;
	}

	public RedirectingNumber getRedirectingNumber()
                                       throws ParameterNotSetException{
		if(isRedirectingNumber == true)
			return redirectingNumber;
		else throw new ParameterNotSetException();
	}

	public void setRedirectingNumber(RedirectingNumber rn){
		redirectingNumber   = rn;
		isRedirectingNumber = true;
	}

	public boolean isRedirectingNumberPresent(){
		return isRedirectingNumber;
	}


	public RedirectionInfo getRedirectionInfo()
                                   throws ParameterNotSetException{
		if(isRedirectionInfo == true)
			return redirectionInfo;
		else throw new ParameterNotSetException();
	}

	public void setRedirectionInfo(RedirectionInfo ri){
		redirectionInfo   = ri;
		isRedirectionInfo = true;
	}

	public boolean isRedirectionInfoPresent(){
		return isRedirectionInfo;
	}

	public byte[] getUserToUserInformation()
                                throws ParameterNotSetException{
		if(isUserToUserInformation == true)
			return userToUserInformation;
		else throw new ParameterNotSetException();
	}

	public void setUserToUserInformation(byte[] userToUserInfo){
		userToUserInformation   = userToUserInfo;
		isUserToUserInformation = true;
	}

	public boolean isUserToUserInformationPresent(){
		return isUserToUserInformation;
	}


	/**
    * String representation of class InformationAnsiEvent
    *
    * @return    String provides description of class InformationAnsiEvent
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
				buffer.append("\n\nisBusinessGroupAnsi = ");
				buffer.append(isBusinessGroupAnsi);
				buffer.append("\nbusinessGroupAnsi = ");
				buffer.append(businessGroupAnsi);
				buffer.append("\n\nisChargeNumberAnsi = ");
				buffer.append(isChargeNumberAnsi);
				buffer.append("\nchargeNumberAnsi = ");
				buffer.append(chargeNumberAnsi);
				buffer.append("\n\nisOrigLineInfo = ");
				buffer.append(isOrigLineInfo);
				buffer.append("\norigLineInfo = ");
				buffer.append(origLineInfo);
				buffer.append("\n\nisRedirectingNumber = ");
				buffer.append(isRedirectingNumber);
				buffer.append("\nredirectingNumber = ");
				buffer.append(redirectingNumber);
				buffer.append("\n\nisRedirectionInfo = ");
				buffer.append(isRedirectionInfo);
				buffer.append("\nredirectionInfo = ");
				buffer.append(redirectionInfo);
				buffer.append("\n\nisUserToUserInformation = ");
				buffer.append(isUserToUserInformation);
				if(isUserToUserInformation == true){
					buffer.append("\nuserToUserInformation = ");
					for(i=0;i<userToUserInformation.length;i++)
						buffer.append(" "+Integer.toHexString((int)(userToUserInformation[i] & 0xFF)));
				}
				return buffer.toString();
		
		}


	byte[]            accessTransport;
	BusinessGroupAnsi businessGroupAnsi;
	ChargeNumberAnsi  chargeNumberAnsi;
	boolean           isAccessTransport;
	boolean           isBusinessGroupAnsi;
	boolean           isChargeNumberAnsi;
	boolean           isOrigLineInfo;
	boolean           isRedirectingNumber;
	boolean           isRedirectionInfo;
	boolean           isUserToUserInformation;
	byte              origLineInfo;
	RedirectingNumber redirectingNumber;
	RedirectionInfo   redirectionInfo;
	byte[]            userToUserInformation;

	
}	


