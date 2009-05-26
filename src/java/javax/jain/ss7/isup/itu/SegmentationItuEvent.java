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
* DATE         : $Date: 2008/05/16 12:23:59 $
* 
* MODULE NAME  : $RCSfile: SegmentationItuEvent.java,v $
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

public class SegmentationItuEvent extends IsupEvent{

	public SegmentationItuEvent(java.lang.Object source,
                            SignalingPointCode dpc,
                            SignalingPointCode opc,
                            byte sls,
                            int cic,
                            byte congestionPriority)
                     throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
				
	}

	public int getIsupPrimitive(){

		return IsupConstants.ISUP_PRIMITIVE_SEGMENTATION; 
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

	public GenericDigits getGenericDigits()
                               throws ParameterNotSetException{
		if(isGenericDigits == true)
			return genericDigits;
		else throw new ParameterNotSetException();

	}

	public void setGenericDigits(GenericDigits gd){
		genericDigits   = gd;
		isGenericDigits = true;
	}

	public boolean isGenericDigitsPresent(){
		return isGenericDigits;
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

	public NotificationInd getNotificationInd()
                                   throws ParameterNotSetException{

		if(isNotificationInd == true)
			return notificationInd;
		else throw new ParameterNotSetException();
	}

	public void setNotificationInd(NotificationInd noi){
		
		notificationInd   = noi;
		isNotificationInd = true;
	}

	public boolean isNotificationIndPresent(){
		return isNotificationInd;
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
    * String representation of class SegmentationItuEvent
    *
    * @return    String provides description of class SegmentationItuEvent
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
				buffer.append("\n\nisGenericDigits = ");
				buffer.append(isGenericDigits);	
				buffer.append("\ngenericDigits = ");
				buffer.append(genericDigits);
				buffer.append("\n\nisGenericNumberItu = ");
				buffer.append(isGenericNumberItu);	
				buffer.append("\ngenericNumberItu = ");
				buffer.append(genericNumberItu);
				buffer.append("\n\nisMessageCompInfoItu = ");
				buffer.append(isMessageCompInfoItu);	
				buffer.append("\nmessageCompInfoItu  = ");
				buffer.append(messageCompInfoItu);
				buffer.append("\n\nisNotificationInd = ");
				buffer.append(isNotificationInd);	
				buffer.append("\nnotificationInd = ");
				buffer.append(notificationInd);
				buffer.append("\n\nisUserToUserInformation = ");
				buffer.append(isUserToUserInformation);	
				if(isUserToUserInformation == true){
					buffer.append("\nuserToUserInformation = ");
					for(i=0;i<userToUserInformation.length;i++)
						buffer.append(" "+Integer.toHexString((int)(userToUserInformation[i] & 0xFF)));				
				}
				return buffer.toString();
		
		}

	byte[]                      accessTransport;
	GenericDigits               genericDigits;
	GenericNumberItu            genericNumberItu;
	boolean                     isAccessTransport;
	boolean                     isGenericDigits;
	boolean                     isGenericNumberItu;
	boolean                     isMessageCompInfoItu;
	boolean                     isNotificationInd;
	boolean                     isUserToUserInformation;
	MessageCompatibilityInfoItu messageCompInfoItu;
	NotificationInd             notificationInd;
	byte[]                      userToUserInformation;

    
}


