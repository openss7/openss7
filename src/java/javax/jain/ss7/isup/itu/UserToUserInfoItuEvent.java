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
* DATE         : $Date: 2008/05/16 12:24:00 $
* 
* MODULE NAME  : $RCSfile: UserToUserInfoItuEvent.java,v $
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

public class UserToUserInfoItuEvent extends IsupEvent{

	public UserToUserInfoItuEvent(java.lang.Object source,
                              SignalingPointCode dpc,
                              SignalingPointCode opc,
                              byte sls,
                              int cic,
                              byte congestionPriority,
                              byte[] in_userToUserInfo)
                       throws ParameterRangeInvalidException{
	
		super(source,dpc,opc,sls,cic,congestionPriority);
		userToUserInformation = in_userToUserInfo;
		m_userToUserInformationPresent = true;
		
	}
		
	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		if(m_userToUserInformationPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();

	}
		
	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_USER_TO_USER_INFO;
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
	
	public byte[] getUserToUserInformation()
                                throws MandatoryParameterNotSetException{
		if(m_userToUserInformationPresent == true)
			return userToUserInformation;
		else throw new MandatoryParameterNotSetException();
	}

	public void setUserToUserInformation(byte[] userToUserInfo){
		userToUserInformation = userToUserInfo;
		m_userToUserInformationPresent = true;
	}

	/**
    * String representation of class UserToUserInfoItuEvent
    *
    * @return    String provides description of class UserToUserInfoItuEvent
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
				buffer.append("\n\nuserToUserInformation = ");
				for(i=0;i<userToUserInformation.length;i++)
					buffer.append(" "+Integer.toHexString((int)(userToUserInformation[i] & 0xFF)));		
				return buffer.toString();
		
		}


	byte[]  accessTransport;
	boolean isAccessTransport;
	byte[]  userToUserInformation;

	private boolean m_userToUserInformationPresent = false;
	    
}


