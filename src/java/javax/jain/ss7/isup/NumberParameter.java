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
* MODULE NAME  : $RCSfile: NumberParameter.java,v $
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

public class NumberParameter extends java.lang.Object
								implements java.io.Serializable{


	
	public NumberParameter(){

	}

	public NumberParameter(byte natureOfAddressIndicator,
                       byte numberingPlanIndicator,
                       byte[] addressSignal)
                throws ParameterRangeInvalidException {

		this.m_addressSignal = addressSignal;

		 if ((natureOfAddressIndicator >= NAI_SUBSCRIBER_NUMBER) &&
		  (natureOfAddressIndicator <= NAI_TEST_LINE_TEST_CODE)) {
			this.m_natureOfAddressIndicator = natureOfAddressIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((numberingPlanIndicator >= NPI_ISDN_NUMBERING_PLAN) &&
		  (numberingPlanIndicator <= NPI_PRIVATE_NUMBERING_PLAN)) {
			this.m_numberingPlanIndicator = numberingPlanIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();

	}

	public byte[] getAddressSignal(){

		return m_addressSignal;
	}

	public byte getNatureOfAddressInd() {

		return m_natureOfAddressIndicator;
	}

	public byte getNumberingPlanInd() {

		return m_numberingPlanIndicator;
	}

	public void setAddressSignal(byte[] addressSignal)
                      throws ParameterRangeInvalidException {

			this.m_addressSignal = addressSignal;
	}

	public void setNatureOfAddressInd(byte natureOfAddressIndicator)
                           throws ParameterRangeInvalidException {

		 if ((natureOfAddressIndicator >= NAI_SUBSCRIBER_NUMBER) &&
		  (natureOfAddressIndicator <= NAI_TEST_LINE_TEST_CODE)) {
			this.m_natureOfAddressIndicator = natureOfAddressIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}


	public void setNumberingPlanInd(byte numberingPlanIndicator)
                         throws ParameterRangeInvalidException {
		
		 if ((numberingPlanIndicator >= NPI_ISDN_NUMBERING_PLAN) &&
		  (numberingPlanIndicator <= NPI_PRIVATE_NUMBERING_PLAN)) {
			this.m_numberingPlanIndicator = numberingPlanIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte[] flatNumberParameter(){
		
		byte[] rc = ByteArray.getByteArray(m_addressSignal.length+2);		
		int i=0;

		rc[0] = (byte)(m_natureOfAddressIndicator & 0x7F);
		rc[1] =  (byte)((m_numberingPlanIndicator << 4) & IsupMacros.L_bits75_MASK);
		for(i=0;i<m_addressSignal.length;i++)
			rc[i+2] =  m_addressSignal[i];
				
		return rc;

	}

	public void  putNumberParameter(byte[] arr, int index, byte par_len){

		int i;
		int j = 2;

		m_natureOfAddressIndicator = (byte)(arr[index] & IsupMacros.L_bits71_MASK);
		m_numberingPlanIndicator   = (byte)((arr[index+1]	>>4) & IsupMacros.L_bits41_MASK);

		m_addressSignal = new byte[(par_len-2)*2];

		for(i=0;i<(par_len-2)*2;i++){
			m_addressSignal[i++]   = (byte)(arr[index+j] & IsupMacros.L_bits41_MASK);				
			m_addressSignal[i]     = (byte)((arr[index+j]>>4) & IsupMacros.L_bits41_MASK);
			j++;
		}

	}
	/**
    * String representation of class NumberParameter
    *
    * @return    String provides description of class NumberParameter
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				if(m_addressSignal != null){
					buffer.append("\naddressSignal = ");
					for(i=0;i<m_addressSignal.length;i++)
						buffer.append(" "+Integer.toHexString((int)(m_addressSignal[i] & 0xFF)));
				}
				buffer.append("\nnatureOfAddressIndicator  = ");
				buffer.append(m_natureOfAddressIndicator);
				buffer.append("\nnumberingPlanIndicator = ");
				buffer.append(m_numberingPlanIndicator);				
				return buffer.toString();
		
		}

	byte[] m_addressSignal = null;
	byte m_natureOfAddressIndicator = 0;
	byte m_numberingPlanIndicator = 0;

	public static final byte NAI_ABBREVIATED_NUMBER = 0x06; 
	public static final byte NAI_CALLED_PARTY_NATIONAL_NUMBER = 0x07;
	public static final byte NAI_CALLED_PARTY_NO_NUMBER = 0x06;
	public static final byte NAI_CALLED_PARTY_SUBSCRIBER_NUMBER = 0x05;
	public static final byte NAI_INTERNATIONAL_NUMBER = 0x04;
	public static final byte NAI_NATIONAL_NUMBER = 0x03;
	public static final byte NAI_NON_UNIQUE_INTERNATIONAL_NUMBER = 0x74;
	public static final byte NAI_NON_UNIQUE_NATIONAL_NUMBER = 0x73;
	public static final byte NAI_NON_UNIQUE_SUBSCRIBER_NUMBER = 0x71;	
	public static final byte NAI_SUBSCRIBER_NUMBER = 0x01;	
	public static final byte NAI_TEST_LINE_TEST_CODE = 0x77;
	public static final byte NAI_UNKOWN = 0x02;
	public static final byte NPI_DATA_NUMBERING_PLAN = 0x03;		
	public static final byte NPI_ISDN_NUMBERING_PLAN = 0x01;
	public static final byte NPI_PRIVATE_NUMBERING_PLAN = 0x05;
	public static final byte NPI_TELEX_NUMBERING_PLAN = 0x04;
	
}

