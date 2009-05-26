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
* DATE         : $Date: 2008/05/16 12:23:57 $
* 
* MODULE NAME  : $RCSfile: LocationNumberItu.java,v $
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

public class LocationNumberItu extends NumberParameter{

	public LocationNumberItu(){

	}
	
	public LocationNumberItu(byte natureOfAddressIndicator,
                         boolean internalNetworkNumberIndicator,
                         byte numberingPlanIndicator,
                         byte addressPresentationRestrictedIndicator,
                         byte screeningIndicator,
                         byte[] addressSignal)
                  throws ParameterRangeInvalidException{
		
		super(	natureOfAddressIndicator,
				numberingPlanIndicator,
				addressSignal);
		
		if((addressPresentationRestrictedIndicator < APRI_PRESENTATION_ALLOWED) || 
		(addressPresentationRestrictedIndicator > APRI_ADDRESS_NOT_AVAILABLE))
			throw new ParameterRangeInvalidException();

		else 
			m_addressPresentationRestrictedIndicator = addressPresentationRestrictedIndicator;

		m_internalNetworkNumberIndicator         = internalNetworkNumberIndicator;

		if((screeningIndicator < SI_USER_PROVIDED_NOT_VERIFIED) || (screeningIndicator > SI_NETWORK_PROVIDED))
			throw new ParameterRangeInvalidException();

		else 
			m_screeningIndicator = screeningIndicator;
	}

	
	public byte getAddrPresRestInd(){
		return m_addressPresentationRestrictedIndicator;
	}

	public void setAddrPresRestInd(byte addressPresentationRestrictedIndicator)
                        throws ParameterRangeInvalidException{
		if((addressPresentationRestrictedIndicator < APRI_PRESENTATION_ALLOWED) || 
		(addressPresentationRestrictedIndicator > APRI_ADDRESS_NOT_AVAILABLE))
			throw new ParameterRangeInvalidException();

		m_addressPresentationRestrictedIndicator = addressPresentationRestrictedIndicator;
	}

	public boolean getInternalNwNumberInd(){
		return m_internalNetworkNumberIndicator;
	}

	public void setInternalNwNumberInd(boolean internalNetworkNumberIndicator){
		m_internalNetworkNumberIndicator = internalNetworkNumberIndicator;
	}

	public byte getScreeningInd(){
		return m_screeningIndicator;
	}	

	public void setScreeningInd(byte screeningIndicator)
                     throws ParameterRangeInvalidException{
		if((screeningIndicator < SI_USER_PROVIDED_NOT_VERIFIED) || (screeningIndicator > SI_NETWORK_PROVIDED))
			throw new ParameterRangeInvalidException();

		m_screeningIndicator = screeningIndicator;
	}

	public byte[] flatLocationNumberItu()
	{
			byte[] rc;
			byte intNetNumInd = 0;

			if(m_internalNetworkNumberIndicator  == true)
				intNetNumInd  = 1;

			rc = super.flatNumberParameter();

			rc[1] = (byte) (rc[1] | ((intNetNumInd  << 7) & IsupMacros.L_BIT8_MASK));
			rc[1] = (byte) (rc[1] |
				((m_addressPresentationRestrictedIndicator << 2) & IsupMacros.L_bits43_MASK)|
				(m_screeningIndicator & IsupMacros.L_bits21_MASK));

			return rc;

	}

	public void  putLocationNumberItu(byte[] arr, int index, byte par_len){
		
			super.putNumberParameter(arr, index, par_len);

			m_addressPresentationRestrictedIndicator =
						(byte)((arr[index+1]>>2) & IsupMacros.L_bits21_MASK);
			m_screeningIndicator = (byte)(arr[index+1] & IsupMacros.L_bits21_MASK);
			
			if((byte)((arr[index+1]>>7) & IsupMacros.L_BIT1_MASK) == 1)
				m_internalNetworkNumberIndicator = true;	
	}
		

	/**
    * String representation of class LocationNumberItu
    *
    * @return    String provides description of class LocationNumberItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nm_addressPresentationRestrictedIndicator = ");
				buffer.append(m_addressPresentationRestrictedIndicator);	
				buffer.append("\nm_internalNetworkNumberIndicator = ");
				buffer.append(m_internalNetworkNumberIndicator);
				buffer.append("\nm_screeningIndicator = ");
				buffer.append(m_screeningIndicator);
				return buffer.toString();
		
		}



	byte    m_addressPresentationRestrictedIndicator;
	boolean m_internalNetworkNumberIndicator;
	byte    m_screeningIndicator;

	public static final byte APRI_PRESENTATION_ALLOWED = 0x00; 
	public static final byte APRI_PRESENTATION_RESTRICTED = 0x01; 
	public static final byte APRI_ADDRESS_NOT_AVAILABLE = 0x02; 
	public static final byte SI_USER_PROVIDED_NOT_VERIFIED = 0x00; 
	public static final byte SI_USER_PROVIDED_VERIFIED_PASSED = 0x01; 
	public static final byte SI_NETWORK_PROVIDED = 0x03; 
	public static final boolean INNI_ROUTING_TO_INTERNAL_NUMBER_ALLOWED = false; 
	public static final boolean INNI_ROUTING_TO_INTERNAL_NUMBER_NOT_ALLOWED = true; 


	
	
}




