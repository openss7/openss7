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
* DATE         : $Date: 2008/05/16 12:23:52 $
* 
* MODULE NAME  : $RCSfile: CallingPartyNumber.java,v $
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

public class CallingPartyNumber extends NumberParameter{

		public CallingPartyNumber(){
		
		}
			
		public CallingPartyNumber(byte natureOfAddressIndicator,
                          byte numberingPlanIndicator,
                          byte addressPresentationRestrictedIndicator,
                          byte screeningIndicator,
                          byte[] addressSignal)
                   throws ParameterRangeInvalidException{		

			 if ((natureOfAddressIndicator >= NAI_SUBSCRIBER_NUMBER) &&
			  (natureOfAddressIndicator <= NAI_TEST_LINE_TEST_CODE)) {
				m_natureOfAddressIndicator = natureOfAddressIndicator;
			 }
			 else 
				throw new ParameterRangeInvalidException();
				
			 if ((numberingPlanIndicator >= NPI_ISDN_NUMBERING_PLAN) &&
			  (numberingPlanIndicator <= NPI_PRIVATE_NUMBERING_PLAN)) {
				m_numberingPlanIndicator   = numberingPlanIndicator;
			 }
			 else 
				throw new ParameterRangeInvalidException();

			 if ((addressPresentationRestrictedIndicator >= APRI_PRESENTATION_ALLOWED) &&
			  (addressPresentationRestrictedIndicator <= APRI_ADDRESS_NOT_AVAILABLE)) {
				m_addressPresentationRestrictedIndicator =addressPresentationRestrictedIndicator;
			 }
			 else 
				throw new ParameterRangeInvalidException();

			 if ((screeningIndicator >= SI_USER_PROVIDED_NOT_VERIFIED) &&
			  (screeningIndicator <= SI_NETWORK_PROVIDED)) {
				m_screeningIndicator       = screeningIndicator;
			 }
			 else 
				throw new ParameterRangeInvalidException();

			m_addressSignal            = addressSignal;
		}

		
		public byte getAddrPresRestInd(){

				return m_addressPresentationRestrictedIndicator;
		}

		public byte getScreeningInd(){
				return m_screeningIndicator;
		}

		public void setAddrPresRestInd(byte addressPresentationRestrictedIndicator)
                        throws ParameterRangeInvalidException{

			 if ((addressPresentationRestrictedIndicator >= APRI_PRESENTATION_ALLOWED) &&
			  (addressPresentationRestrictedIndicator <= APRI_ADDRESS_NOT_AVAILABLE)) {
				m_addressPresentationRestrictedIndicator =addressPresentationRestrictedIndicator;
			 }
			 else 
				throw new ParameterRangeInvalidException();
		}

		public void setScreeningInd(byte screeningIndicator)
                     throws ParameterRangeInvalidException{

			 if ((screeningIndicator >= SI_USER_PROVIDED_NOT_VERIFIED) &&
			  (screeningIndicator <= SI_NETWORK_PROVIDED)) {
				m_screeningIndicator       = screeningIndicator;
			 }
			 else 
				throw new ParameterRangeInvalidException();

		}

		public void  putCallingPartyNumber(byte[] arr, int index, byte par_len){
		
			super.putNumberParameter(arr, index, par_len);
			m_addressPresentationRestrictedIndicator =
						(byte)((arr[index+1]>>2) & IsupMacros.L_bits21_MASK);
			m_screeningIndicator = (byte)(arr[index+1] & IsupMacros.L_bits21_MASK);
			
		}

		public byte[] flatCallingPartyNumber()
		{
			byte[] rc;
			
			rc = super.flatNumberParameter();

			rc[1] = (byte) (rc[1] |
				((m_addressPresentationRestrictedIndicator << 2) & IsupMacros.L_bits43_MASK)|
				(m_screeningIndicator & IsupMacros.L_bits21_MASK));

			return rc;			
			
		}

	/**
    * String representation of class CallingPartyNumber
    *
    * @return    String provides description of class CallingPartyNumber
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\naddressPresentationRestrictedIndicator = ");
				buffer.append(m_addressPresentationRestrictedIndicator);		
				buffer.append("\nscreeningIndicator = ");
				buffer.append(m_screeningIndicator);		
				return buffer.toString();
		
		}		

	byte m_addressPresentationRestrictedIndicator;
	byte m_screeningIndicator;

	public static final byte APRI_ADDRESS_NOT_AVAILABLE=0x02; 
	public static final byte APRI_PRESENTATION_ALLOWED=0x00; 
	public static final byte APRI_PRESENTATION_RESTRICTED=0x01;
	public static final byte SI_USER_PROVIDED_NOT_VERIFIED = 0x00; 
	public static final byte SI_USER_PROVIDED_VERIFIED_PASSED=0x01; 
	public static final byte SI_NETWORK_PROVIDED=0x03; 
	
	public static final byte NAI_SUBSCRIBER_NUMBER = 0x01;	
	public static final byte NAI_TEST_LINE_TEST_CODE = 0x77;
	public static final byte NPI_ISDN_NUMBERING_PLAN = 0x01;
	public static final byte NPI_PRIVATE_NUMBERING_PLAN = 0x05;
	
}




