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
* MODULE NAME  : $RCSfile: FwdCallInd.java,v $
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

public abstract class FwdCallInd extends java.lang.Object implements java.io.Serializable{

	public FwdCallInd(){

	}

	public FwdCallInd(byte endToEndMethodIndicator,
                  boolean interworkingIndicator,
                  boolean isdnUserPartIndicator,
                  byte isdnUserPartPreferenceIndicator,
                  boolean isdnAccessIndicator,
                  byte sccpMethodIndicator,
                  boolean typeOfCallInd)
           throws ParameterRangeInvalidException{

		 if ((endToEndMethodIndicator >= EMI_PASS_ALONG_METHOD_AVAILABLE) &&
		  (endToEndMethodIndicator <= EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE)) {
			m_endToEndMethodIndicator         = endToEndMethodIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((isdnUserPartPreferenceIndicator >= ISUPPI_PREFERRED) &&
		  (isdnUserPartPreferenceIndicator <= ISUPPI_REQUIRED)) {
			m_isdnUserPartPreferenceIndicator = isdnUserPartPreferenceIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((sccpMethodIndicator >= SMI_SCCP_METHOD_NO_IND) &&
		  (sccpMethodIndicator <= SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE)) {
			m_sccpMethodIndicator = sccpMethodIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		m_interworkingIndicator           = interworkingIndicator;
		m_isdnUserPartIndicator           = isdnUserPartIndicator;
		m_isdnAccessIndicator             = isdnAccessIndicator;
		m_typeOfCallInd                   = typeOfCallInd;    
		
	}

	public byte getETEMethodInd(){
		
		return m_endToEndMethodIndicator;
	}

	public boolean getInterworkingInd(){
		return m_interworkingIndicator;
	}

	public boolean getISDNAccessInd(){
		return m_isdnAccessIndicator;
	}

	public boolean getISDNUserPartInd(){
		return m_isdnUserPartIndicator;
	}

	public byte getISUPPreferenceInd(){
		return m_isdnUserPartPreferenceIndicator;
	}

	public byte getSCCPMethodInd(){
		return m_sccpMethodIndicator; 
	}

	public boolean getTypeOfCallInd(){
		return m_typeOfCallInd;
	}

	public void setETEMethodInd(byte endToEndMethodIndicator)
                     throws ParameterRangeInvalidException{
		 if ((endToEndMethodIndicator >= EMI_PASS_ALONG_METHOD_AVAILABLE) &&
		  (endToEndMethodIndicator <= EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE)) {
			m_endToEndMethodIndicator = endToEndMethodIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void setInterworkingInd(boolean interworkingIndicator){
		m_interworkingIndicator = interworkingIndicator;
	}

	public void setISDNAccessInd(boolean isdnAccessIndicator){
		m_isdnAccessIndicator = isdnAccessIndicator;
	}

	public void setISDNUserPartInd(boolean isdnUserPartIndicator){
		m_isdnUserPartIndicator = isdnUserPartIndicator;
	}

	public void setISUPPreferenceInd(byte isdnUserPartPreferenceIndicator)
                          throws ParameterRangeInvalidException{
		 if ((isdnUserPartPreferenceIndicator >= ISUPPI_PREFERRED) &&
		  (isdnUserPartPreferenceIndicator <= ISUPPI_REQUIRED)) {
			m_isdnUserPartPreferenceIndicator = isdnUserPartPreferenceIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void setSCCPMethodInd(byte sccpMethodIndicator)
                      throws ParameterRangeInvalidException{
		
		 if ((sccpMethodIndicator >= SMI_SCCP_METHOD_NO_IND) &&
		  (sccpMethodIndicator <= SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE)) {
			m_sccpMethodIndicator = sccpMethodIndicator;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void setTypeOfCallInd(boolean typeOfCallInd){
		m_typeOfCallInd = typeOfCallInd;
	}
		
	public byte[] flatFwdCallInd(){
		
		byte[] fwdcallind = ByteArray.getByteArray(2);	
		byte interworking = 0, isupind = 0,isupaccessind = 0, typeofcall = 0;		

		for(int i=0;i<2;i++)
			fwdcallind[i] = 0;
		
		if(m_interworkingIndicator == true)
			interworking = 1;

		if(m_isdnUserPartIndicator == true)
			isupind = 1;		
		
		if(m_isdnAccessIndicator == true)
			isupaccessind = 1;		

		if(m_typeOfCallInd == true)
			typeofcall = 1;						

		fwdcallind[0] = (byte)(typeofcall |
		((m_endToEndMethodIndicator << 1)&  IsupMacros.L_bits32_MASK)|
		((interworking << 3) & IsupMacros.L_BIT4_MASK) |
		((isupind << 5)  & IsupMacros.L_BIT6_MASK)|
		((m_isdnUserPartPreferenceIndicator << 6) &IsupMacros.L_bits87_MASK));

		fwdcallind[1] = (byte)(isupaccessind |
						((m_sccpMethodIndicator << 1) &IsupMacros.L_bits32_MASK));		
		
		return fwdcallind;
	}

	public void  putFwdCallInd(byte[] arr, int index, byte par_len){

		m_endToEndMethodIndicator = (byte)((arr[index]>>1) & IsupMacros.L_bits21_MASK);
		m_isdnUserPartPreferenceIndicator = (byte)((arr[index]>>6) & IsupMacros.L_bits21_MASK);
		m_sccpMethodIndicator = (byte)((arr[index+1]>>1) & IsupMacros.L_bits21_MASK);
		
		if((byte)((arr[index]>>3) & IsupMacros.L_BIT1_MASK) == 1)
			m_interworkingIndicator = true;
		
		if((byte)(arr[index+1] & IsupMacros.L_BIT1_MASK) == 1)
			m_isdnAccessIndicator = true;
		
		if((byte)((arr[index]>>5) & IsupMacros.L_BIT1_MASK) == 1)
			m_isdnUserPartIndicator = true;

		if((byte)(arr[index] & IsupMacros.L_BIT1_MASK) == 1)
			m_typeOfCallInd = true;
	
	}

	/**
    * String representation of class FwdCallInd
    *
    * @return    String provides description of class FwdCallInd
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nendToEndMethodIndicator = ");
				buffer.append(m_endToEndMethodIndicator);
				buffer.append("\ninterworkingIndicator  = ");
				buffer.append(m_interworkingIndicator);
				buffer.append("\nisdnAccessIndicator = ");
				buffer.append(m_isdnAccessIndicator);
				buffer.append("\nisdnUserPartIndicator = ");
				buffer.append(m_isdnUserPartIndicator);
				buffer.append("\nisdnUserPartPreferenceIndicator = ");
				buffer.append(m_isdnUserPartPreferenceIndicator);
				buffer.append("\nsccpMethodIndicator = ");
				buffer.append(m_sccpMethodIndicator);
				buffer.append("\ntypeOfCallInd = ");
				buffer.append(m_typeOfCallInd);				
				return buffer.toString();
		
		}

	byte    m_endToEndMethodIndicator;
	boolean m_interworkingIndicator;
	boolean m_isdnAccessIndicator;
	boolean m_isdnUserPartIndicator;
	byte    m_isdnUserPartPreferenceIndicator;
	byte    m_sccpMethodIndicator;
	boolean m_typeOfCallInd;
		

	public static final byte EMI_NO_ETE_METHOD_IND = 0x00; 
	public static final byte EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE = 0x03; 
	public static final byte EMI_PASS_ALONG_METHOD_AVAILABLE = 0x01;
	public static final byte EMI_SCCP_METHOD_AVAILABLE = 0x02; 
	public static final boolean IAI_ORIGINATING_ACCESS_ISDN = true;
	public static final boolean IAI_ORIGINATING_ACCESS_NON_ISDN = false;
	public static final boolean II_INTERWORKING_ENCOUNTERED = true;
	public static final boolean II_NO_INTERWORKING_IND =  false;
	public static final boolean ISUPI_ISUP_NOT_USED = false;
	public static final boolean ISUPI_ISUP_USED = true; 
	public static final byte ISUPPI_NOT_REQUIRED = 0x01; 
	public static final byte ISUPPI_PREFERRED = 0x00; 
	public static final byte ISUPPI_REQUIRED = 0x02; 
	public static final boolean NICI_INTERNATIONAL_CALL = true;
	public static final boolean NICI_NATIONAL_CALL = false; 
	public static final byte SMI_CONNECTION_METHOD_AVAILABLE = 0x02;
	public static final byte SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE = 0x03; 
	public static final byte SMI_CONNECTIONLESS_METHOD_AVAILABLE = 0x01;
	public static final byte SMI_SCCP_METHOD_NO_IND = 0x00; 
	
}




