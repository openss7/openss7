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
* MODULE NAME  : $RCSfile: GenericDigits.java,v $
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

public class GenericDigits extends java.lang.Object implements java.io.Serializable{


	public GenericDigits(){

	}

	public GenericDigits(byte typeOfDigits,
                     byte encodingScheme,
                     byte[] digits)
              throws ParameterRangeInvalidException{
		
		 if ((encodingScheme >= ES_BCD_EVEN) &&
		  (encodingScheme <= ES_BINARY_CODED)) {
			m_encodingScheme = encodingScheme;
		 }
		 else 
			throw new ParameterRangeInvalidException();
		 if ((typeOfDigits >= TOD_ACCOUNT_CODE) &&
		  (typeOfDigits <= TOD_BILL_TO_NUMBER)) {
			m_typeOfDigits = typeOfDigits;
		 }
		 else 
			throw new ParameterRangeInvalidException();
					
		m_digits         = digits;
	}

	public byte[] getDigits(){
		return m_digits;
	}

	public byte getEncodingScheme(){
		return m_encodingScheme;
	}

	public byte getTypeOfDigits(){
		return m_typeOfDigits;
	}

	public void setDigits(byte[] digits){
		m_digits = digits;
	}

	public void setEncodingScheme(byte encodingScheme)
                       throws ParameterRangeInvalidException{
		 if ((encodingScheme >= ES_BCD_EVEN) &&
		  (encodingScheme <= ES_BINARY_CODED)) {
			m_encodingScheme = encodingScheme;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void setTypeOfDigits(byte typeOfDigits)
                     throws ParameterRangeInvalidException{
		 if ((typeOfDigits >= TOD_ACCOUNT_CODE) &&
		  (typeOfDigits <= TOD_BILL_TO_NUMBER)) {
			m_typeOfDigits = typeOfDigits;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void  putGenericDigits(byte[] arr, int index, byte par_len){
		
		int i;		

		m_encodingScheme = (byte)((arr[index] >> 5) & IsupMacros.L_bits31_MASK);
		m_typeOfDigits   = (byte)(arr[index]  & IsupMacros.L_bits51_MASK);
	
		m_digits  = new byte[par_len-1];
		for(i=0;i<par_len-1;i++)
			m_digits[i] = (byte)arr[index+i+1];
			
	}

	public byte[] flatGenericDigits()
	{
			byte[] rc = ByteArray.getByteArray(m_digits.length+2);
		
			int i;
			
			rc[0] = (byte)(((m_encodingScheme & IsupMacros.L_bits31_MASK) << 5) |
					(m_typeOfDigits & IsupMacros.L_bits51_MASK));
			
			rc[1] = (byte)m_digits.length;
			
			for(i=0;i<m_digits.length;i++)
				rc[i+2] = 	m_digits[i];
			
			return rc;
	}
		

	/**
    * String representation of class GenericDigits
    *
    * @return    String provides description of class GenericDigits
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				if(m_digits != null){
					buffer.append("\ndigits = ");
					for(i=0;i<m_digits.length;i++)
						buffer.append(" "+Integer.toHexString((int)(m_digits[i] & 0xFF)));
				}
				buffer.append("\nencodingScheme  = ");
				buffer.append(m_encodingScheme);
				buffer.append("\ntypeOfDigits  = ");
				buffer.append(m_typeOfDigits);					
				return buffer.toString();
		
		}		
	
	byte[] m_digits;
	byte   m_encodingScheme;
	byte   m_typeOfDigits;

	public static final byte ES_BCD_EVEN = 0x00; 
	public static final byte ES_BCD_ODD = 0x01; 
	public static final byte ES_BINARY_CODED = 0x03; 
	public static final byte ES_IA5_CHARACTER = 0x02; 
	public static final byte TOD_ACCOUNT_CODE = 0x00; 
	public static final byte TOD_AUTHORIZATION_CODE = 0x01; 
	public static final byte TOD_BILL_TO_NUMBER = 0x0F; 
	public static final byte TOD_BUSINESS_COMMUNICATION_GROUP_IDENTITY = 0x03; 
	public static final byte TOD_PRIVATE_NETWORK_TRAVELLING_CLASS_MARK = 0x02;
}




