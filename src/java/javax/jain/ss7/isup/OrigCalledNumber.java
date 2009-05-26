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
* DATE         : $Date: 2008/05/16 12:24:05 $
* 
* MODULE NAME  : $RCSfile: OrigCalledNumber.java,v $
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

public class OrigCalledNumber extends NumberParameter{

	
	public OrigCalledNumber(){

	}
		
	public OrigCalledNumber(byte natureOfAddressIndicator,
                        byte numberingPlanIndicator,
                        byte addressPresentationRestrictedIndicator,
                        byte[] addressSignal)
                 throws ParameterRangeInvalidException{

		m_natureOfAddressIndicator = natureOfAddressIndicator;
		m_numberingPlanIndicator   = numberingPlanIndicator;
		m_addressSignal            = addressSignal;

		 if ((addressPresentationRestrictedIndicator >= APRI_PRESENTATION_ALLOWED) &&
		  (addressPresentationRestrictedIndicator <= APRI_ADDRESS_NOT_AVAILABLE)) {
			m_addressPresentationRestrictedIndicator = addressPresentationRestrictedIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte getAddrPresRestInd(){
		return m_addressPresentationRestrictedIndicator;
	}

	public void setAddrPresRestInd(byte addressPresentationRestrictedIndicator)
                        throws ParameterRangeInvalidException{
		 if ((addressPresentationRestrictedIndicator >= APRI_PRESENTATION_ALLOWED) &&
		  (addressPresentationRestrictedIndicator <= APRI_ADDRESS_NOT_AVAILABLE)) {
			m_addressPresentationRestrictedIndicator = addressPresentationRestrictedIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}
	
	public void  putOrigCalledNumber(byte[] arr, int index, byte par_len){

		super.putNumberParameter(arr,index,par_len);
		m_addressPresentationRestrictedIndicator = (byte)((arr[index+1]>>2)&IsupMacros.L_bits21_MASK);

	}
		
	public byte[] flatOrigCalledNumber()
	{
			byte[] rc;
			
			rc = super.flatNumberParameter();
			
			rc[1] = (byte) (rc[1] |
				((m_addressPresentationRestrictedIndicator << 2) & IsupMacros.L_bits43_MASK));
				
			return rc;

	}
	
	/**
    * String representation of class OrigCalledNumber
    *
    * @return    String provides description of class OrigCalledNumber
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\naddressPresentationRestrictedIndicator = ");
				buffer.append(m_addressPresentationRestrictedIndicator);		
				return buffer.toString();
		
		}		

		byte m_addressPresentationRestrictedIndicator;

		public static final byte APRI_PRESENTATION_ALLOWED = 0x00; 
		public static final byte APRI_PRESENTATION_RESTRICTED = 0x01; 
		public static final byte APRI_ADDRESS_NOT_AVAILABLE = 0x02; 

}




