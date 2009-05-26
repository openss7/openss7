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
* DATE         : $Date: 2008/05/16 12:24:07 $
* 
* MODULE NAME  : $RCSfile: RedirectionInfo.java,v $
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
import javax.jain.ss7.*;

public class RedirectionInfo extends java.lang.Object implements java.io.Serializable{

	public RedirectionInfo(){

	}

	public RedirectionInfo(byte redirectingIndicator,
                       byte originalRedirectingReason,
                       byte redirectionCounter,
                       byte redirectingReason)
                throws ParameterRangeInvalidException{

		 if ((redirectingIndicator >= RI_NO_REDIRECTION) &&
		  (redirectingIndicator <= RI_CALL_DIVERSION_REDIRECTION_NUMBER_PRESENTATION_RESTRICTED)) {
			m_redirectingIndicator = redirectingIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((originalRedirectingReason >= ORR_UNKNOWN_UNAVAILABLE) &&
		  (originalRedirectingReason <= ORR_UNCONDITIONAL)) {
			m_originalRedirectingReason = originalRedirectingReason;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((redirectingReason >= RR_UNKNOWN_UNAVAILABLE) &&
		  (redirectingReason <= RR_MOBILE_SUBSCRIBER_NOT_REACHABLE)) {
			m_redirectingReason = redirectingReason;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		m_redirectionCounter        = redirectionCounter;
	}

	public byte getOriginalRedirectingReason(){
		return m_originalRedirectingReason;
	}

	public byte getRedirectingInd(){
		return m_redirectingIndicator;
	}

	public byte getRedirectingReason(){
		return m_redirectingReason;
	}

	public byte getRedirectionCounter(){
		return m_redirectionCounter;
	}

	public void setOriginalRedirectingReason(byte originalRedirectingReason)
                                  throws ParameterRangeInvalidException{
		 if ((originalRedirectingReason >= ORR_UNKNOWN_UNAVAILABLE) &&
		  (originalRedirectingReason <= ORR_UNCONDITIONAL)) {
			m_originalRedirectingReason = originalRedirectingReason;
		 }
		 else 
			throw new ParameterRangeInvalidException();

	}

	public void setRedirectingInd(byte redirectingIndicator)
                       throws ParameterRangeInvalidException{
		 if ((redirectingIndicator >= RI_NO_REDIRECTION) &&
		  (redirectingIndicator <= RI_CALL_DIVERSION_REDIRECTION_NUMBER_PRESENTATION_RESTRICTED)) {
			m_redirectingIndicator = redirectingIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void setRedirectingReason(byte redirectingReason)
                          throws ParameterRangeInvalidException{
		 if ((redirectingReason >= RR_UNKNOWN_UNAVAILABLE) &&
		  (redirectingReason <= RR_MOBILE_SUBSCRIBER_NOT_REACHABLE)) {
			m_redirectingReason = redirectingReason;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void setRedirectionCounter(byte redirectionCounter)
                           throws ParameterRangeInvalidException{
		m_redirectionCounter = redirectionCounter;
	}

	public void  putRedirectionInfo(byte[] arr, int index, byte par_len){

		m_originalRedirectingReason = (byte)((arr[index]>>4)&IsupMacros.L_bits41_MASK);
		m_redirectingIndicator      = (byte)(arr[index] & IsupMacros.L_bits31_MASK);
		m_redirectingReason         = (byte)((arr[index+1]>>4)&IsupMacros.L_bits41_MASK);
		m_redirectionCounter        = (byte)(arr[index+1] & IsupMacros.L_bits31_MASK);
	}

	public byte[] flatRedirectionInfo(){
		
		byte[] rc = ByteArray.getByteArray(2);	
		

		for(int i=0;i<2;i++)
			rc[i] = 0;
				
		rc[0] = (byte)((m_redirectingIndicator & IsupMacros.L_bits31_MASK) |
						((m_originalRedirectingReason << 4) & IsupMacros.L_bits85_MASK));

		rc[1] = (byte)(( m_redirectionCounter & IsupMacros.L_bits31_MASK)|
						((m_redirectingReason << 4) & IsupMacros.L_bits85_MASK));		
		
		return rc;
	}

	/**
    * String representation of class RedirectionInfo
    *
    * @return    String provides description of class RedirectionInfo
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\noriginalRedirectingReason = ");
				buffer.append(m_originalRedirectingReason);
				buffer.append("\nredirectingIndicator = ");
				buffer.append(m_redirectingIndicator);
				buffer.append("\nredirectingReason = ");
				buffer.append(m_redirectingReason);
				buffer.append("\nredirectionCounter = ");
				buffer.append(m_redirectionCounter);
				return buffer.toString();
		
		}		
	
	
	byte m_originalRedirectingReason;
	byte m_redirectingIndicator;
	byte m_redirectingReason;
	byte m_redirectionCounter;

	public static final byte RI_NO_REDIRECTION = 0x00; 
	public static final byte RI_CALL_ROUTED = 0x01; 
	public static final byte RI_CALL_REROUTED = 0x02; 
	public static final byte RI_CALL_DIVERSION = 0x03; 
	public static final byte RI_CALL_DIVERSION_ALL_REDIRECTION_INFORMATION_PRESENTATION_RESTRICTED = 0x04; 
	public static final byte RI_CALL_REROUTED_REDIRECTION_NUMBER_PRESENTATION_RESTRICTED = 0x05; 
	public static final byte RI_CALL_DIVERSION_REDIRECTION_NUMBER_PRESENTATION_RESTRICTED = 0x06; 
	public static final byte ORR_UNKNOWN_UNAVAILABLE = 0x00; 
	public static final byte ORR_USER_BUSY = 0x01; 
	public static final byte ORR_NO_REPLY = 0x02; 
	public static final byte ORR_UNCONDITIONAL = 0x03; 
	public static final byte RR_UNKNOWN_UNAVAILABLE = 0x00; 
	public static final byte RR_USER_BUSY = 0x01; 
	public static final byte RR_NO_REPLY = 0x02; 
	public static final byte RR_UNCONDITIONAL = 0x03; 
	public static final byte RR_DEFLECTION_DURING_ALERTING = 0x04; 
	public static final byte RR_DEFLECTION_IMMEDIATE_RESPONSE = 0x05; 
	public static final byte RR_MOBILE_SUBSCRIBER_NOT_REACHABLE = 0x06; 


}




