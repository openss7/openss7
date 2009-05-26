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
* DATE         : $Date: 2008/05/16 12:24:02 $
* 
* MODULE NAME  : $RCSfile: InfoInd.java,v $
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


public class InfoInd extends java.lang.Object implements java.io.Serializable{

	public InfoInd(){

	}

	public InfoInd(byte in_callingAddrRespInd,
               boolean in_holdInd,
               boolean in_callingCatResInd,
               boolean in_chargeInfoResInd,
               boolean in_solInfoInd){

		m_callingAddrRespInd = in_callingAddrRespInd;
		m_callingCatRespInd  = in_callingCatResInd;
		m_chargeInfoRespInd  = in_chargeInfoResInd;
		m_holdInd            = in_holdInd; 
		m_solInfoInd         = in_solInfoInd;
		
	}

	public byte getCallingPartyAddressRespInd(){
		return m_callingAddrRespInd;
	}

	public boolean getCallingPartyCatRespInd(){
		return m_callingCatRespInd;
	}
		
	public boolean getChargeInfoRespInd(){
		return m_chargeInfoRespInd;
	}

	public boolean getHoldProvidedInd(){
		return m_holdInd;
	}

	public boolean getSolicitedInfoInd(){
		return m_solInfoInd;
	}

	public void setCallingPartyAddressRespInd(byte aCallingPartyAddressResponseIndicator)
                                   throws ParameterRangeInvalidException{
		 if ((aCallingPartyAddressResponseIndicator >= CPARI_CALLING_PARTY_ADDRESS_NOT_INCLUDED) &&
		  (aCallingPartyAddressResponseIndicator <= CPARI_CALLING_PARTY_ADDRESS_INCLUDED)) {
			m_callingAddrRespInd = aCallingPartyAddressResponseIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();

	}

	public void setCallingPartyCatRespInd(
		boolean aCallingPartyCategoryResponseIndicator){
		m_callingCatRespInd = aCallingPartyCategoryResponseIndicator;
	}

	public void setChargeInfoRespInd(
				boolean aChargeInformationResponseIndicator){
		m_chargeInfoRespInd = aChargeInformationResponseIndicator;
	}

	public void setHoldProvidedInd(boolean aHoldProvidedIndicator){
		m_holdInd = aHoldProvidedIndicator;
	}
			
	public void setSolicitedInfoInd(
				boolean aSolicitedInformationIndicator){
		m_solInfoInd = aSolicitedInformationIndicator;

	}

	public void  putInfoInd(byte[] arr, int index, byte par_len){

			m_callingAddrRespInd = (byte)(arr[index] & IsupMacros.L_bits21_MASK);
			if(((arr[index] >> 2) & IsupMacros.L_BIT1_MASK) == 1)
				m_holdInd = true;
			if(((arr[index] >> 5) & IsupMacros.L_BIT1_MASK) == 1)
				m_callingCatRespInd = true;
			if(((arr[index] >> 6) & IsupMacros.L_BIT1_MASK) == 1)
				m_chargeInfoRespInd = true;		
			if(((arr[index] >> 7) & IsupMacros.L_BIT1_MASK) == 1)
				m_solInfoInd = true;												
								
	}
	
	public byte[] flatInfoInd(){
		
			byte[] rc = ByteArray.getByteArray(2);	
			byte callingCatRespInd=0, chargeInfoRespInd=0, holdInd=0, solInfoInd=0;

			if(m_callingCatRespInd == true)
				callingCatRespInd = 1;
			if(m_chargeInfoRespInd == true)
				chargeInfoRespInd = 1;
			if(m_holdInd == true)
				holdInd = 1;
			if(m_solInfoInd == true)
				solInfoInd = 1;
							
			rc[0] = (byte)((m_callingAddrRespInd & IsupMacros.L_bits21_MASK)|
						(( holdInd << 2) & IsupMacros.L_BIT3_MASK)|
						(( callingCatRespInd << 5) & IsupMacros.L_BIT6_MASK)|						
						(( chargeInfoRespInd << 6) & IsupMacros.L_BIT7_MASK) |
						(( solInfoInd  << 7 ) & IsupMacros.L_BIT8_MASK));

			rc[1] = 0;
		
			return rc;
		}	


	/**
    * String representation of class InfoInd
    *
    * @return    String provides description of class InfoInd
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncallingAddrRespInd = ");
				buffer.append(m_callingAddrRespInd);
				buffer.append("\ncallingCatRespInd  = ");
				buffer.append(m_callingCatRespInd);
				buffer.append("\nchargeInfoRespInd = ");
				buffer.append(m_chargeInfoRespInd);
				buffer.append("\nholdInd  = ");
				buffer.append(m_holdInd);
				buffer.append("\nsolInfoInd  = ");
				buffer.append(m_solInfoInd);				
				return buffer.toString();
		
		}		
	
	
		byte    m_callingAddrRespInd;
		boolean m_callingCatRespInd;
		boolean m_chargeInfoRespInd;
		boolean m_holdInd;
		boolean m_solInfoInd;


		
		public static final byte CPARI_CALLING_PARTY_ADDRESS_NOT_INCLUDED = 0x00; 
		public static final byte CPARI_CALLING_PARTY_ADDRESS_NOT_AVAILABLE = 0x01;
		public static final byte CPARI_CALLING_PARTY_ADDRESS_INCLUDED = 0x03;
		public static final boolean HPI_HOLD_NOT_PROVIDED = false;
		public static final boolean HPI_HOLD_PROVIDED = true;
		public static final boolean CPCRI_CALLING_PARTY_CATEGORY_NOT_INCLUDED = false;
		public static final boolean CPCRI_CALLING_PARTY_CATEGORY_INCLUDED = true;
		public static final boolean CIRI_CHARGE_INFORMATION_NOT_INCLUDED = false;
		public static final boolean CIRI_CHARGE_INFORMATION_INCLUDED = true;
		public static final boolean SII_SOLICITED = false;
		public static final boolean SII_UNSOLICITED = true;


}




