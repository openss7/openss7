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
* MODULE NAME  : $RCSfile: GenericNumber.java,v $
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

public class GenericNumber extends NumberParameter{


	public GenericNumber(){

	}

	public GenericNumber(short numberQualifierIndicator,
                     byte natureOfAddressIndicator,
                     byte numberingPlanIndicator,
                     byte addressPresentationRestrictedIndicator,
                     byte[] addressSignal)
              throws ParameterRangeInvalidException{

		 if ((addressPresentationRestrictedIndicator >= APRI_PRESENTATION_ALLOWED) &&
		  (addressPresentationRestrictedIndicator <= APRI_ADDRESS_NOT_AVAILABLE)) {
			m_addressPresentationRestrictedIndicator = addressPresentationRestrictedIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((numberQualifierIndicator >= NQI_DIALED_DIGITS) &&
		  (numberQualifierIndicator <= NQI_TRANSFER_NUMBER_1)) {
			m_numberQualifierIndicator = numberQualifierIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		m_natureOfAddressIndicator = natureOfAddressIndicator;
		m_numberingPlanIndicator   = numberingPlanIndicator;
		m_addressSignal = addressSignal;				
	}
	
	public byte getAddrPresRestInd(){
		return m_addressPresentationRestrictedIndicator;

	}
	public short getNumberQualifierIndicator(){
		return m_numberQualifierIndicator;
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

	public void setNumberQualifierIndicator(short numberQualifierIndicator)
                                 throws ParameterRangeInvalidException{
		
		 if ((numberQualifierIndicator >= NQI_DIALED_DIGITS) &&
		  (numberQualifierIndicator <= NQI_TRANSFER_NUMBER_1)) {
			m_numberQualifierIndicator = numberQualifierIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}	

	public void  putGenericNumber(byte[] arr, int index, byte par_len){

		super.putNumberParameter(arr,index,par_len);
		m_numberQualifierIndicator               = (byte) arr[index];
		m_addressPresentationRestrictedIndicator = (byte)((arr[index+2]>>2)&IsupMacros.L_bits21_MASK);


	}

	
	public byte[] flatGenericNumber()
	{
			byte[] rc;
						
			rc = super.flatNumberParameter();

			byte[] tmp = ByteArray.getByteArray(rc.length+1);		

			System.arraycopy(rc,0,
								tmp,1,rc.length);

			tmp[0] = (byte)m_numberQualifierIndicator;
			
			tmp[2] = (byte) (tmp[2] |
				((m_addressPresentationRestrictedIndicator << 2) & IsupMacros.L_bits43_MASK));
				
			ByteArray.returnByteArray(rc);
	
			return tmp;

	}

	/**
    * String representation of class GenericNumber
    *
    * @return    String provides description of class GenericNumber
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\naddressPresentationRestrictedIndicator = ");
				buffer.append(m_addressPresentationRestrictedIndicator);
				buffer.append("\nnumberQualifierIndicator  = ");
				buffer.append(m_numberQualifierIndicator);								
				return buffer.toString();
		
		}		
	
	byte  m_addressPresentationRestrictedIndicator;
	short m_numberQualifierIndicator;

	public static final byte APRI_ADDRESS_NOT_AVAILABLE = 0x02;
	public static final byte APRI_PRESENTATION_ALLOWED = 0x00; 
	public static final byte APRI_PRESENTATION_RESTRICTED = 0x01;
	public static final short NQI_ADDITIONAL_CALLED_NUMBER = 0x01;
	public static final short NQI_ADDITIONAL_CALLING_PARTY_NUMBER = 0x06;
	public static final short NQI_ADDITIONAL_CONNECTED_NUMBER = 0x05;
	public static final short NQI_ADDITIONAL_ORIGINAL_CALLED_NUMBER =0x07; 
	public static final short NQI_ADDITIONAL_REDIRECTING_NUMBER = 0x08;
	public static final short NQI_ADDITIONAL_REDIRECTION_NUMBER = 0x09;

	public static final short NQI_CALLER_EMERGENCY_SERVICE_IDENTIFICATION = 0xFE; 
	public static final short NQI_DIALED_DIGITS = 0x00; 
	public static final short NQI_REDIRECTING_TERMINATING_NUMBER = 0x04;
	public static final short NQI_SUPPLEMENTAL_CALLING_NUMBER_FAILED_NETWORK_SCREENING = 0x02;
	public static final short NQI_SUPPLEMENTAL_CALLING_NUMBER_NOT_SCREENED = 0x03; 
	public static final short NQI_TRANSFER_NUMBER_1 = 0xFD; 
	public static final short NQI_TRANSFER_NUMBER_2 = 0xFC; 
	public static final short NQI_TRANSFER_NUMBER_3 = 0xFB; 
	public static final short NQI_TRANSFER_NUMBER_4 = 0xFA; 
	public static final short NQI_TRANSFER_NUMBER_5 = 0xF9; 
	public static final short NQI_TRANSFER_NUMBER_6 = 0xF8; 

}




