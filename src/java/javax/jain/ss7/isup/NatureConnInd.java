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
* DATE         : $Date: 2008/05/16 12:24:04 $
* 
* MODULE NAME  : $RCSfile: NatureConnInd.java,v $
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

public class NatureConnInd extends java.lang.Object implements java.io.Serializable{


		
	public NatureConnInd(){

	}

	public NatureConnInd(byte in_satInd,
                     byte in_contCheckInd,
                     boolean in_echoControlDevInd)
              throws ParameterRangeInvalidException{
		
		 if ((in_contCheckInd >= CCI_CONTINUITY_CHECK_NOT_REQUIRED) &&
		  (in_contCheckInd <= CCI_CONTINUITY_CHECK_PERFORMED_ON_PREVIOUS_CIRCUIT)) {
			m_contCheckInd      = in_contCheckInd;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_satInd >= SI_NO_SATELLITE_CIRCUIT) &&
		  (in_satInd <= SI_TWO_SATELLITE_CIRCUIT)) {
			m_satInd            = in_satInd;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		m_echoControlDevInd = in_echoControlDevInd;

	}

	public byte getContCheckInd(){
		return m_contCheckInd;
	}
		
	public boolean getEchoControlDevInd(){
		return m_echoControlDevInd;
	}

	public byte getSatelliteInd(){
		return m_satInd;
	}

	public void setContCheckInd(byte continuityCheckInd)
                     throws ParameterRangeInvalidException{
		 if ((continuityCheckInd >= CCI_CONTINUITY_CHECK_NOT_REQUIRED) &&
		  (continuityCheckInd <= CCI_CONTINUITY_CHECK_PERFORMED_ON_PREVIOUS_CIRCUIT)) {
			m_contCheckInd = continuityCheckInd;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void setEchoControlDevInd(boolean echoDevInd){
		m_echoControlDevInd = echoDevInd;
	}

	public void setSatelliteInd(byte satelliteInd)
                     throws ParameterRangeInvalidException{
		 if ((satelliteInd >= SI_NO_SATELLITE_CIRCUIT) &&
		  (satelliteInd <= SI_TWO_SATELLITE_CIRCUIT)) {
			m_satInd = satelliteInd;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public byte flatNatureConnInd()
	{
		byte rc = 0;
		byte echo = 0;
		
		if(m_echoControlDevInd == true)
			echo = 1;

		rc = (byte) ((m_satInd & IsupMacros.L_bits21_MASK) |  
			((m_contCheckInd<< 2) &  IsupMacros.L_bits43_MASK) |
			((echo << 4) & IsupMacros.L_BIT5_MASK));

		return rc;

	}
	
	public void  putNatureConnInd(byte[] arr, int index, byte par_len){
		
		m_satInd            = (byte)(arr[index] &  IsupMacros.L_bits21_MASK);
		m_contCheckInd      = (byte)((arr[index] >> 2)& IsupMacros.L_bits21_MASK);
		if((byte)((arr[index] >> 4) &  IsupMacros.L_BIT1_MASK) == 1)
				m_echoControlDevInd = true;
	}

	/**
    * String representation of class NatureConnInd
    *
    * @return    String provides description of class NatureConnInd
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncontCheckInd = ");
				buffer.append(m_contCheckInd);
				buffer.append("\nechoControlDevInd  = ");
				buffer.append(m_echoControlDevInd);
				buffer.append("\nsatInd = ");
				buffer.append(m_satInd);				
				return buffer.toString();
		
		}

		
		
		byte    m_contCheckInd;

		boolean m_echoControlDevInd;

		byte    m_satInd;


		public static final byte SI_NO_SATELLITE_CIRCUIT = 0x00; 

		public static final byte SI_ONE_SATELLITE_CIRCUIT = 0x01; 

		public static final byte SI_TWO_SATELLITE_CIRCUIT = 0x02; 

		public static final byte CCI_CONTINUITY_CHECK_NOT_REQUIRED = 0x00; 

		public static final byte CCI_CONTINUITY_CHECK_REQUIRED = 0x01; 

		public static final byte CCI_CONTINUITY_CHECK_PERFORMED_ON_PREVIOUS_CIRCUIT = 0x02; 

		public static final boolean ECDI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED = false; 

		public static final boolean ECDI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_INCLUDED = true; 
		
		public static final byte CI_CONTINUITY_CHECK_FAILED = 0x00; 
		
		public static final byte CI_CONTINUITY_CHECK_SUCCESSFUL = 0x01; 
}






