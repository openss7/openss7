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
* DATE         : $Date: 2008/05/16 12:23:54 $
* 
* MODULE NAME  : $RCSfile: CallDiversionInfoItu.java,v $
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

public class CallDiversionInfoItu extends java.lang.Object implements java.io.Serializable{



	public CallDiversionInfoItu(){
	}
		
	public CallDiversionInfoItu(byte in_notificationSubsOpt,
                            byte in_redReason)
                     throws ParameterRangeInvalidException{
		if((in_notificationSubsOpt >=NSO_UNKNOWN) && 
		(in_notificationSubsOpt <= NSO_PRESENTATION_ALLOWED_WITHOUT_REDIRECTION_NUMBER))
			notificationSubsOpt = in_notificationSubsOpt;
		else throw new ParameterRangeInvalidException();


		if((in_redReason >=RR_UNKNOWN) && 
		(in_redReason <= RR_MOBILE_SUBSCRIBER_NOT_REACHABLE))
			redReason = in_redReason;
		else throw new ParameterRangeInvalidException();

	}


	public byte getNotificationSubsOpt(){
		return notificationSubsOpt;
	}

	public void setNotificationSubsOpt(byte aNotificationSubscriptionOptions)
                            throws ParameterRangeInvalidException{
		if((aNotificationSubscriptionOptions >=NSO_UNKNOWN) && 
		(aNotificationSubscriptionOptions <= NSO_PRESENTATION_ALLOWED_WITHOUT_REDIRECTION_NUMBER))
			notificationSubsOpt = aNotificationSubscriptionOptions;
		else throw new ParameterRangeInvalidException();
	}

	public byte getRedirectionReason(){
		return redReason;
	}

	public void setRedirectionReason(byte aRedirectionReason)
                          throws ParameterRangeInvalidException{
		if((aRedirectionReason >=RR_UNKNOWN) && 
		(aRedirectionReason <= RR_MOBILE_SUBSCRIBER_NOT_REACHABLE))
			redReason = aRedirectionReason;
		else throw new ParameterRangeInvalidException();
	}

	public void  putCallDiversionInfoItu(byte[] arr, int index, byte par_len){
		
		notificationSubsOpt = 	(byte)(arr[index] & IsupMacros.L_bits31_MASK);
		redReason           =   (byte)((arr[index]>>3) & IsupMacros.L_bits41_MASK);
		
	}	

	public byte flatCallDiversionInfoItu()
	{

		byte rc = 0;
		
		rc = (byte)((notificationSubsOpt &  IsupMacros.L_bits31_MASK) | 
			 ((redReason << 3) & 0x78));
		
		return rc;
	}
	
	/**
    * String representation of class CallDiversionInfoItu
    *
    * @return    String provides description of class CallDiversionInfoItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nnotificationSubsOpt  = ");
				buffer.append(notificationSubsOpt);	
				buffer.append("\nredReason  = ");
				buffer.append(redReason);	
				return buffer.toString();
		
		}

	byte notificationSubsOpt;
	byte redReason;

	public static final byte NSO_UNKNOWN = 0x00; 
	public static final byte NSO_PRESENTATION_NOT_ALLOWED = 0x01; 
	public static final byte NSO_PRESENTATION_ALLOWED_WITH_REDIRECTION_NUMBER = 0x02; 
	public static final byte NSO_PRESENTATION_ALLOWED_WITHOUT_REDIRECTION_NUMBER = 0x03; 
	public static final byte RR_UNKNOWN = 0x00; 
	public static final byte RR_USER_BUSY = 0x01; 
	public static final byte RR_NO_REPLY = 0x02; 
	public static final byte RR_UNCONDITIONAL = 0x03; 
	public static final byte RR_REFLECTION_DURING_ALERTING = 0x04; 
	public static final byte RR_REFLECTION_IMMEDIATE_RESPONSE = 0x05; 
	public static final byte RR_MOBILE_SUBSCRIBER_NOT_REACHABLE = 0x06; 

	
}




