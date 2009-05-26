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
* DATE         : $Date: 2008/05/16 12:23:55 $
* 
* MODULE NAME  : $RCSfile: ConnectedNumberItu.java,v $
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


package javax.jain.ss7.isup.itu;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class ConnectedNumberItu extends NumberParameter{

	
	public ConnectedNumberItu(){
	}

	public ConnectedNumberItu(byte natureOfAddressIndicator,
                          byte numberingPlanIndicator,
                          byte addressPresentationRestrictedIndicator,
                          byte screeningIndicator,
                          byte[] addressSignal)
                   throws ParameterRangeInvalidException{

		super(	natureOfAddressIndicator,
				numberingPlanIndicator,
				addressSignal);
		if ((addressPresentationRestrictedIndicator >= APRI_PRESENTATION_ALLOWED) && 
		(addressPresentationRestrictedIndicator <= APRI_ADDRESS_NOT_AVAILABLE)) {
			m_addressPresentationRestrictedIndicator  = addressPresentationRestrictedIndicator;

		}
		else 
			throw new ParameterRangeInvalidException();
			
		if ((screeningIndicator >= SI_USER_PROVIDED_NOT_VERIFIED) && 
		(screeningIndicator <= SI_NETWORK_PROVIDED)) {
			m_screeningIndicator = screeningIndicator;

		}
		else 
			throw new ParameterRangeInvalidException();
	}
	
	public byte getAddrPresRestInd(){
		return m_addressPresentationRestrictedIndicator ;
	}

	public void setAddrPresRestInd(byte addressPresentationRestrictedIndicator)
                        throws ParameterRangeInvalidException{
		if ((addressPresentationRestrictedIndicator >= APRI_PRESENTATION_ALLOWED) && 
		(addressPresentationRestrictedIndicator <= APRI_ADDRESS_NOT_AVAILABLE)) {
			m_addressPresentationRestrictedIndicator  = addressPresentationRestrictedIndicator;

		}
		else 
			throw new ParameterRangeInvalidException();
	}

	public byte getScreeningInd(){
		return m_screeningIndicator;
	}

	public void setScreeningInd(byte screeningIndicator)
                     throws ParameterRangeInvalidException{
		if ((screeningIndicator >= SI_USER_PROVIDED_NOT_VERIFIED) && 
		(screeningIndicator <= SI_NETWORK_PROVIDED)) {
			m_screeningIndicator = screeningIndicator;

		}
		else 
			throw new ParameterRangeInvalidException();
	}

	public void  putConnectedNumberItu(byte[] arr, int index, byte par_len){
		
		super.putNumberParameter(arr,index,par_len);
		m_addressPresentationRestrictedIndicator = (byte)((arr[index+1]>>2)&
														IsupMacros.L_bits21_MASK);
		m_screeningIndicator = (byte)(arr[index+1] & IsupMacros.L_bits21_MASK);
	}

	public byte[] flatConnectedNumberItu()
	{
			byte[] rc;
			
			rc = super.flatNumberParameter();

			rc[1] = (byte) (rc[1] |
				((m_addressPresentationRestrictedIndicator << 2) & IsupMacros.L_bits43_MASK)|
				(m_screeningIndicator & IsupMacros.L_bits21_MASK));

			return rc;

	}
		
	/**
    * String representation of class ConnectedNumberItu
    *
    * @return    String provides description of class ConnectedNumberItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nm_addressPresentationRestrictedIndicator = ");
				buffer.append(m_addressPresentationRestrictedIndicator);	
				buffer.append("\nm_screeningIndicator = ");
				buffer.append(m_screeningIndicator);
				return buffer.toString();
		
		}


	byte m_addressPresentationRestrictedIndicator;
	byte m_screeningIndicator;

	public static final byte APRI_PRESENTATION_ALLOWED = 0x00; 
	public static final byte APRI_PRESENTATION_RESTRICTED = 0x01; 
	public static final byte APRI_ADDRESS_NOT_AVAILABLE = 0x02; 
	public static final byte SI_USER_PROVIDED_NOT_VERIFIED = 0x00; 
	public static final byte SI_USER_PROVIDED_VERIFIED_PASSED = 0x01; 
	public static final byte SI_NETWORK_PROVIDED = 0x03; 
	


	
	
}




