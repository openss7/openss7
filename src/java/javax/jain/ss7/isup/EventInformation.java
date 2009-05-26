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
* DATE         : $Date: 2008/05/16 12:24:01 $
* 
* MODULE NAME  : $RCSfile: EventInformation.java,v $
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

public class EventInformation extends java.lang.Object implements java.io.Serializable{
		
	public EventInformation(){

	}

	public EventInformation(byte in_eventInd,
                        boolean in_eventPresRestInd)
                 throws ParameterRangeInvalidException{

		 if ((in_eventInd >= EI_ALERTING) &&
		  (in_eventInd <= EI_CALL_FORWARDED_UNCONDITIONAL)) {
			m_eventInd = in_eventInd;
		 }
		 else 
			throw new ParameterRangeInvalidException();
			
		m_eventPresRestInd = in_eventPresRestInd;
	}
					
	public byte getEventInd(){
		return m_eventInd;
	}

	public boolean getEventPresRestInd(){
		return m_eventPresRestInd;
	}

	public void setEventInd(byte in_eventInd)
                 throws ParameterRangeInvalidException{
		 if ((in_eventInd >= EI_ALERTING) &&
		  (in_eventInd <= EI_CALL_FORWARDED_UNCONDITIONAL)) {
			m_eventInd = in_eventInd;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void setEventPresRestInd(boolean in_eventPresRestInd){
		m_eventPresRestInd = in_eventPresRestInd;
	}

	public void  putEventInformation(byte[] arr, int index, byte par_len){
		
			m_eventInd = (byte)(arr[index] & IsupMacros.L_bits71_MASK);
			
			if((byte)((arr[index]>>7) & IsupMacros.L_BIT1_MASK) == 1)
				m_eventPresRestInd = true;	
	}

	public byte flatEventInformation()
	{
		byte rc = 0;
		byte eventPresRestInd = 0;
		
		if(m_eventPresRestInd == true)
			 eventPresRestInd= 1;

		rc = (byte)((m_eventInd & IsupMacros.L_bits71_MASK) |
					((eventPresRestInd & IsupMacros.L_BIT1_MASK) << 7));
					
		return rc;
	}	

	/**
    * String representation of class EventInformation
    *
    * @return    String provides description of class EventInformation
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\neventInd = ");
				buffer.append(m_eventInd);
				buffer.append("\neventPresRestInd  = ");
				buffer.append(m_eventPresRestInd);								
				return buffer.toString();
		
		}		

	byte    m_eventInd;
	boolean m_eventPresRestInd;

	public static final byte EI_ALERTING = 0x01;
	public static final byte EI_CALL_FORWARDED_ON_BUSY = 0x04; 
	public static final byte EI_CALL_FORWARDED_ON_NO_REPLY = 0x05;
	public static final byte EI_CALL_FORWARDED_UNCONDITIONAL = 0x06;
	public static final byte EI_INBAND_INFORMATION_OR_PATTERN_AVAILABLE = 0x03;
	public static final byte EI_PROGRESS = 0x02; 
	public static final boolean EPRI_NO_INDICATION = false; 
	public static final boolean EPRI_PRESENTATION_RESTRICTED = true; 				
}




