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
* DATE         : $Date: 2008/05/16 12:23:51 $
* 
* MODULE NAME  : $RCSfile: BwdCallInd.java,v $
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


public abstract class BwdCallInd extends java.lang.Object implements
java.io.Serializable {


		protected BwdCallInd(){
		}
		
		protected BwdCallInd(byte in_chargeInd,
                     byte in_calledStatusInd,
                     byte in_calledCateInd,
                     byte in_eteMethodInd,
                     boolean in_iwInd,
                     boolean in_isupInd,
                     boolean in_holdInd,
                     boolean in_isdnAccessInd,
                     boolean in_echoDevInd,
                     byte in_sccpMethodInd)
              throws ParameterRangeInvalidException
		
		 {

			 if ((in_calledCateInd == CPCI_ORDINARY_SUBSCRIBER) ||
			  (in_calledCateInd == CPCI_PAYPHONE)) {
				this.m_calledCatInd    = in_calledCateInd;
			 }
			 else 
				throw new ParameterRangeInvalidException();
				

			 if ((in_calledStatusInd >= CPSI_SUBSCRIBER_FREE) &&
			  (in_calledStatusInd <= CPSI_EXCESSIVE_DELAY)) {
				this.m_calledStatusInd = in_calledStatusInd;
			 }
			 else 
				throw new ParameterRangeInvalidException();
				
			 if ((in_chargeInd >= CI_NO_CHARGE) &&
			  (in_chargeInd <= CI_CHARGE)) {
				this.m_chargeInd       = in_chargeInd;
			 }
			 else 
				throw new ParameterRangeInvalidException();

			 if ((in_eteMethodInd >= EMI_PASS_ALONG_METHOD_AVAILABLE) &&
			  (in_eteMethodInd <= EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE)) {
				this.m_eteMethodInd    = in_eteMethodInd;
			 }
			 else 
				throw new ParameterRangeInvalidException();

			 if ((in_sccpMethodInd >= SMI_SCCP_METHOD_NO_IND) &&
			  (in_sccpMethodInd <= SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE)) {
				this.m_sccpMethodInd   = in_sccpMethodInd;
			 }
			 else 
				throw new ParameterRangeInvalidException();
								
			this.m_iwInd           = in_iwInd;
			this.m_isupInd         = in_isupInd;
			this.m_holdInd         = in_holdInd;
			this.m_isdnAccessInd   = in_isdnAccessInd;
			this.m_echoDevInd      = in_echoDevInd;

		}

		public byte getCalledCategoryInd(){
			return 	m_calledCatInd;				
		}

		public byte getCalledStatusInd(){

			return m_calledStatusInd;
		}


		public byte getChargeInd(){
			return m_chargeInd;
		}

		public boolean getEchoControlDevInd() {

			return m_echoDevInd;
		}

		public byte getETEMethodInd() {

			return 	m_eteMethodInd;
		}

		public boolean getHoldingInd() {
				
			return m_holdInd;
		}

		public boolean getInterworkingInd(){

			return m_iwInd;
		}

		public boolean getISDNAccessInd() {

			return  m_isdnAccessInd;
		}

		public boolean getISUPInd() {

			return m_isupInd;
		}

		public byte getSCCPMethodInd(){

			return 	m_sccpMethodInd;
		}

		public void setCalledCategoryInd(byte aCalledPartysCategoryIndicator)
                          throws ParameterRangeInvalidException {
			
			 if ((aCalledPartysCategoryIndicator == CPCI_ORDINARY_SUBSCRIBER) ||
			  (aCalledPartysCategoryIndicator == CPCI_PAYPHONE)) {
				 m_calledCatInd = aCalledPartysCategoryIndicator;
			 }
			 else 
				throw new ParameterRangeInvalidException();
		}

		public void setCalledStatusInd(byte aCalledPartysStatusIndicator)
                        throws ParameterRangeInvalidException {

			 if ((aCalledPartysStatusIndicator >= CPSI_SUBSCRIBER_FREE) &&
			  (aCalledPartysStatusIndicator <= CPSI_EXCESSIVE_DELAY)) {
				 m_calledStatusInd = aCalledPartysStatusIndicator;
			 }
			 else 
				throw new ParameterRangeInvalidException();
		}

		public void setChargeInd(byte aChargeInd)
                  throws ParameterRangeInvalidException {
							
			 if ((aChargeInd >= CI_NO_CHARGE) &&
			  (aChargeInd <= CI_CHARGE)) {
				m_chargeInd = aChargeInd;
			 }
			 else 
				throw new ParameterRangeInvalidException();
		}

		public void setEchoControlDevInd(boolean aEchoControlDeviceIndicator){
				
			m_echoDevInd = aEchoControlDeviceIndicator;
		}

		public void setETEMethodInd(byte aEndToEndMethodIndicator)
                     throws ParameterRangeInvalidException {
			
			 if ((aEndToEndMethodIndicator >= EMI_PASS_ALONG_METHOD_AVAILABLE) &&
			  (aEndToEndMethodIndicator <= EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE)) {
				m_eteMethodInd = aEndToEndMethodIndicator;
			 }
			 else 
				throw new ParameterRangeInvalidException();
		}

		public void setHoldingInd(boolean aHoldingIndicator){

			m_holdInd = aHoldingIndicator;
		}

		public void setInterworkingInd(boolean aInterworkingIndicator){

			m_iwInd = aInterworkingIndicator;
		}

		public void setISDNAccessInd(boolean aISDNAccessIndicator){
		
			m_isdnAccessInd = aISDNAccessIndicator;
		}

		public void setISUPInd(boolean aISDNUserPartIndicator){
		
			m_isupInd =  aISDNUserPartIndicator;
		}

		public void setSCCPMethodInd(byte aSCCPMethodIndicator)
                      throws ParameterRangeInvalidException {
			
			 if ((aSCCPMethodIndicator >= SMI_SCCP_METHOD_NO_IND) &&
			  (aSCCPMethodIndicator <= SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE)) {
				m_sccpMethodInd = aSCCPMethodIndicator;
			 }
			 else 
				throw new ParameterRangeInvalidException();
		}


	/**
    * String representation of class BwdCallInd
    *
    * @return    String provides description of class BwdCallInd
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncalledCatInd = ");
				buffer.append(m_calledCatInd);
				buffer.append("\ncalledStatusInd  = ");
				buffer.append(m_calledStatusInd);
				buffer.append("\nchargeInd = ");
				buffer.append(m_chargeInd);
				buffer.append("\nechoDevInd = ");
				buffer.append(m_echoDevInd);
				buffer.append("\neteMethodInd = ");
				buffer.append(m_eteMethodInd);
				buffer.append("\nholdInd = ");
				buffer.append(m_holdInd);
				buffer.append("\nisdnAccessInd = ");
				buffer.append(m_isdnAccessInd);
				buffer.append("\nisupInd = ");
				buffer.append(m_isupInd);
				buffer.append("\niwInd = ");
				buffer.append(m_iwInd);
				buffer.append("\nsccpMethodInd = ");
				buffer.append(m_sccpMethodInd);
				return buffer.toString();
		
		}

		public void  putBwdCallInd(byte[] arr, int index, byte par_len){
		
			m_chargeInd       = (byte)(arr[index] & IsupMacros.L_bits21_MASK);
			m_calledStatusInd = (byte)((arr[index] >> 2)& IsupMacros.L_bits21_MASK);
			m_calledCatInd    = (byte)((arr[index] >> 4)& IsupMacros.L_bits21_MASK);
			m_eteMethodInd    = (byte)((arr[index] >> 6)& IsupMacros.L_bits21_MASK);
			if((arr[index+1] & IsupMacros.L_BIT1_MASK) == 1)
				m_iwInd = true;
			if(((arr[index+1] >> 2) & IsupMacros.L_BIT1_MASK) == 1)
				m_isupInd = true;
			if(((arr[index+1] >> 3) & IsupMacros.L_BIT1_MASK) == 1)
				m_holdInd = true;
			if(((arr[index+1] >> 4) & IsupMacros.L_BIT1_MASK) == 1)
				m_isdnAccessInd = true;
			if(((arr[index+1] >> 5) & IsupMacros.L_BIT1_MASK) == 1)
				m_echoDevInd = true;	
			m_sccpMethodInd = (byte)((arr[index+1] >> 6)&IsupMacros.L_bits21_MASK);
								
		}

		public byte[] flatBwdCallInd(){
		
			byte[] rc = ByteArray.getByteArray(2);	
			byte echoDevInd = 0, holdInd = 0, isdnAccessInd = 0,
				isupInd = 0, iwInd = 0;
		
			if(m_echoDevInd == true)
				echoDevInd = 1;
			if(m_holdInd == true)
				holdInd = 1;
			if(m_isdnAccessInd == true)
				isdnAccessInd = 1;
			if(m_isupInd == true)
				isupInd = 1;
			if(m_iwInd == true)
				iwInd = 1;
							

			rc[0] = (byte)((m_chargeInd & IsupMacros.L_bits21_MASK)|
						(( m_calledStatusInd << 2) & IsupMacros.L_bits43_MASK)|
						(( m_calledCatInd << 4) & IsupMacros.L_bits65_MASK)|						
						(( m_eteMethodInd << 6) & IsupMacros.L_bits87_MASK));

			rc[1] = (byte)((iwInd & IsupMacros.L_BIT1_MASK) |
							((isupInd << 2) & IsupMacros.L_BIT3_MASK)|
							((holdInd << 3) & IsupMacros.L_BIT4_MASK)|
							((isdnAccessInd << 4) & IsupMacros.L_BIT5_MASK)|
							((echoDevInd << 5) & IsupMacros.L_BIT6_MASK)|
							((m_sccpMethodInd << 6)&IsupMacros.L_bits87_MASK));
		
			return rc;
		}
		

		byte m_calledCatInd = 0;

		byte m_calledStatusInd = 0;

		byte m_chargeInd = 0;

		boolean m_echoDevInd = false;
		
		byte m_eteMethodInd = 0;

		boolean m_holdInd = false;

		boolean m_isdnAccessInd = false;

		boolean m_isupInd = false;

		boolean m_iwInd = false;

		byte m_sccpMethodInd = 0;


		public static final byte CI_NO_CHARGE_IND = 0;

		public static final byte CI_NO_CHARGE = 1;
 
		public static final byte CI_CHARGE = 2;

		public static final byte CPSI_NO_CALLED_STATUS_IND = 0;
		
		public static final byte CPSI_SUBSCRIBER_FREE = 1;

		public static final byte CPSI_CONNECT_WHEN_FREE = 2;

		public static final byte CPSI_EXCESSIVE_DELAY = 3;

		public static final byte CPCI_NO_CALLED_CATEGORY_IND = 0;

		public static final byte CPCI_ORDINARY_SUBSCRIBER = 1;

		public static final byte CPCI_PAYPHONE = 2;

		public static final byte EMI_NO_ETE_METHOD_IND = 0;

		public static final byte EMI_PASS_ALONG_METHOD_AVAILABLE = 1;

		public static final byte EMI_SCCP_METHOD_AVAILABLE = 2;
		
		public static final byte EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE = 3;

		public static final boolean II_NO_INTERWORKING_IND = false;

		public static final boolean II_INTERWORKING_ENCOUNTERED = false; 

		public static final boolean ISUPI_ISUP_NOT_USED = false;

		public static final boolean ISUPI_ISUP_USED = false;

		public static final boolean HI_HOLDING_NOT_REQUESTED = false;

		public static final boolean HI_HOLDING_REQUESTED = false;

		public static final boolean IAI_TERMINATING_ACCESS_NON_ISDN = false;

		public static final boolean IAI_TERMINATING_ACCESS_ISDN = false;

		public static final boolean ECDI_INCOMING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED = false;

		public static final boolean ECDI_INCOMING_HALF_ECHO_CONTROL_DEVICE_INCLUDED = false;

		public static final byte SMI_SCCP_METHOD_NO_IND = 0;

		public static final byte SMI_CONNECTIONLESS_METHOD_AVAILABLE = 1;

		public static final byte SMI_CONNECTION_METHOD_AVAILABLE = 2;
		
		public static final byte SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE = 3;


}




