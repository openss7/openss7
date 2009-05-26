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
* MODULE NAME  : $RCSfile: InfoReqInd.java,v $
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

public class InfoReqInd extends java.lang.Object implements java.io.Serializable{

	
	public InfoReqInd(){

	}

	public InfoReqInd(boolean in_callingAddrReqInd,
                  boolean in_holdingInd,
                  boolean in_callingCatReqInd,
                  boolean in_chargeInfoReqInd,
                  boolean in_mciReqInd){

		
		m_callingAddrReqInd = in_callingAddrReqInd;
		m_holdInd           = in_holdingInd;
		m_callingCatReqInd  = in_callingCatReqInd;
		m_chargeInfoReqInd  = in_chargeInfoReqInd;
		m_mciReqInd         = in_mciReqInd;
	}

	public boolean getCallingPartyAddressReqInd(){
		return m_callingAddrReqInd;

	}

	public boolean getCallingPartyCatReqInd(){
		return m_callingCatReqInd;
	}

	public boolean getChargeInfoReqInd(){
		return m_chargeInfoReqInd;
	}

	public boolean getHoldingInd(){
		return m_holdInd;
	}

	public boolean getMCIReqInd(){
		return m_mciReqInd;
	}

	public void setCallingPartyAddressReqInd(
				boolean callingPartyAddressReqInd){
		m_callingAddrReqInd = callingPartyAddressReqInd;
	}

	public void setCallingPartyCatReqInd(
				boolean callingPartyCatReqInd){
		m_callingCatReqInd = callingPartyCatReqInd;
	}
	
	public void setChargeInfoReqInd(boolean aChargeInfoReqInd){
		
		m_chargeInfoReqInd = aChargeInfoReqInd;
	}

	public void setHoldingInd(boolean holdingInd){
		m_holdInd = holdingInd;
	}
		
	public void setMCIReqInd(boolean MCIReqInd){
		m_mciReqInd = MCIReqInd;
	}
	
	public void  putInfoReqInd(byte[] arr, int index, byte par_len){

		if(((arr[index]) & IsupMacros.L_BIT1_MASK) == 1)	
			m_callingAddrReqInd = true;
		if(((arr[index] >> 1) & IsupMacros.L_BIT1_MASK) == 1)	
			m_holdInd = true;
		if(((arr[index] >> 3) & IsupMacros.L_BIT1_MASK) == 1)	
			m_callingCatReqInd = true;	
		if(((arr[index] >> 4) & IsupMacros.L_BIT1_MASK) == 1)										
			m_chargeInfoReqInd = true;
		if(((arr[index] >> 7) & IsupMacros.L_BIT1_MASK) == 1)
			m_mciReqInd = true;					
	}

	public byte[] flatInfoReqInd(){
		
			byte[] rc = ByteArray.getByteArray(2);	
			byte callingAddrReqInd=0, callingCatReqInd=0, chargeInfoReqInd=0, holdInd=0, mciReqInd=0;

			if(m_callingAddrReqInd == true)
				callingAddrReqInd = 1;
			if(m_callingCatReqInd == true)
				callingCatReqInd = 1;
			if(m_chargeInfoReqInd == true)
				chargeInfoReqInd = 1;
			if(m_holdInd == true)
				holdInd = 1;
			if(m_mciReqInd == true)
				mciReqInd = 1;
			
							
			rc[0] = (byte)((callingAddrReqInd & IsupMacros.L_BIT1_MASK)|
						(( holdInd << 1) & IsupMacros.L_BIT2_MASK)|
						(( callingCatReqInd << 3) & IsupMacros.L_BIT4_MASK)|
						(( chargeInfoReqInd << 4) & IsupMacros.L_BIT5_MASK)|
						(( mciReqInd << 7) & IsupMacros.L_BIT8_MASK));

			rc[1] = 0;
		
			return rc;
		}

	/**
    * String representation of class InfoReqInd
    *
    * @return    String provides description of class InfoReqInd
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncallingAddrReqInd = ");
				buffer.append(m_callingAddrReqInd);
				buffer.append("\ncallingCatReqInd  = ");
				buffer.append(m_callingCatReqInd);
				buffer.append("\nchargeInfoReqInd = ");
				buffer.append(m_chargeInfoReqInd);
				buffer.append("\nholdInd  = ");
				buffer.append(m_holdInd);
				buffer.append("\nmciReqInd  = ");
				buffer.append(m_mciReqInd);				
				return buffer.toString();
		
		}		
	
	
		
		boolean m_callingAddrReqInd;
		boolean m_callingCatReqInd;
		boolean m_chargeInfoReqInd;
		boolean m_holdInd;
		boolean m_mciReqInd;


		public static final boolean CPARI_CALLING_PARTY_ADDRESS_NOT_REQUESTED = false; 
		public static final boolean CPARI_CALLING_PARTY_ADDRESS_REQUESTED = true; 	
		public static final boolean HPI_HOLD_NOT_REQUESTED = false; 
		public static final boolean HPI_HOLD_REQUESTED = true; 
		public static final boolean CPCRI_CALLING_PARTY_CATEGORY_NOT_REQUESTED = false; 
		public static final boolean CPCRI_CALLING_PARTY_CATEGORY_REQUESTED = true; 	
		public static final boolean CIRI_CHARGE_INFORMATION_NOT_REQUESTED = false; 
		public static final boolean CIRI_CHARGE_INFORMATION_REQUESTED = true; 
		public static final boolean MCIRI_MALICIOUS_CALL_IDENTIFICATION_NOT_REQUESTED = false; 
		public static final boolean MCIRI_RESERVED_FOR_MCIRI_MALICIOUS_CALL_IDENTIFICATION_REQUESTED = true; 


}




