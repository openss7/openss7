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
* DATE         : $Date: 2008/05/16 12:24:04 $
* 
* MODULE NAME  : $RCSfile: NotificationInd.java,v $
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

public class NotificationInd extends java.lang.Object implements java.io.Serializable{
		
	
	public NotificationInd(){

	}

	public NotificationInd(byte in_notificationInd,
                       boolean in_extInd)
                throws ParameterRangeInvalidException{
		 if ((in_notificationInd >= NI_USER_SUSPENDED) &&
		  (in_notificationInd <= NI_CALL_IS_DIVERTING)) {
			m_notificationInd = in_notificationInd;
		 }
		 else 
			throw new ParameterRangeInvalidException();
			
		m_extInd          = in_extInd;
	}

	public boolean getExtensionIndicator(){
		return m_extInd;
	}

	public byte getNotificationInd(){
		return m_notificationInd;
	}
	
	public void setExtensionIndicator(boolean aExtensionIndicator){
		m_extInd = aExtensionIndicator;
	}

	public void setNotificationInd(byte aNotificationIndicator)
                        throws ParameterRangeInvalidException{
		 if ((aNotificationIndicator >= NI_USER_SUSPENDED) &&
		  (aNotificationIndicator <= NI_CALL_IS_DIVERTING)) {
			m_notificationInd = aNotificationIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte[] flatNotificationInd()
	{
		byte[] rc = ByteArray.getByteArray(2);

		byte ext = 1;
		
		if(m_extInd == true)
				ext = 0;
		
		rc[0] = ext;
		rc[1] = m_notificationInd;		
	
		return rc;

	}

	public void  putNotificationInd(byte[] arr, int index, byte par_len){

		
		m_notificationInd = (byte)(arr[index] & IsupMacros.L_bits71_MASK);
		if((byte)((arr[index] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
			m_extInd = false;		
	}

	/**
    * String representation of class NotificationInd
    *
    * @return    String provides description of class NotificationConnInd
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nextInd = ");
				buffer.append(m_extInd);
				buffer.append("\nnotificationInd  = ");
				buffer.append(m_notificationInd);		
				return buffer.toString();
		
		}

		
	
	boolean m_extInd;
	byte    m_notificationInd;

	
	public static final byte NI_USER_SUSPENDED = 0x00; 
	public static final byte NI_USER_RESUMED = 0x01; 
	public static final byte NI_BEARER_SERVICE_CHANGE = 0x02; 
	public static final byte NI_DISCRIMINATOR_FOR_EXTENSION_TO_ASN1_ENCODED_COMPONENT = 0x03; 
	public static final byte NI_CALL_COMPLETION_DELAY = 0x04; 
	public static final byte NI_CONFERENCE_ESTABLISHED = 0x42; 
	public static final byte NI_CONFERENCE_DISCONNECTED = 0x43; 
	public static final byte NI_OTHER_PARTY_ADDED = 0x44; 
	public static final byte NI_ISOLATED = 0x45; 
	public static final byte NI_REATTACHED = 0x46; 
	public static final byte NI_OTHER_PARTY_ISOLATED = 0x47; 
	public static final byte NI_OTHER_PARTY_ATTACHED = 0x48; 
	public static final byte NI_OTHER_PARTY_SPLIT = 0x49; 
	public static final byte NI_OTHER_PARTY_DISCONNECTED = 0x4A; 
	public static final byte NI_CONFERENCE_FLOATING = 0x4B; 
	public static final byte NI_CALL_IS_WAITING_CALL = 0x60; 
	public static final byte NI_DIVERSION_ACTIVATED = 0x68; 
	public static final byte NI_CALL_TRANSFER_ALERTING = 0x69; 
	public static final byte NI_CALL_TRANSFER_ACTIVE = 0x6A; 
	public static final byte NI_REMOTE_HOLD = 0x71; 
	public static final byte NI_REMOTE_RETRIEVAL = 0x7A; 
	public static final byte NI_CALL_IS_DIVERTING = 0x7B; 
	public static final boolean EI_NEXT_OCTET_EXIST = false; 
	public static final boolean EI_LAST_OCTET = true; 


}




