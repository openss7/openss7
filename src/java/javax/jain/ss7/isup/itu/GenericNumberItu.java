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
* DATE         : $Date: 2008/05/16 12:23:56 $
* 
* MODULE NAME  : $RCSfile: GenericNumberItu.java,v $
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

public class GenericNumberItu extends GenericNumber{

	public GenericNumberItu(){

	}

	public GenericNumberItu(short numberQualifierIndicator,
                        byte natureOfAddressIndicator,
                        byte numberingPlanIndicator,
                        byte addressPresentationRestrictedIndicator,
                        byte[] addressSignal,
                        boolean numberIncompleteIndicator,
                        byte screeningIndicator)
                 throws ParameterRangeInvalidException{
		super(	numberQualifierIndicator,
				natureOfAddressIndicator,
				numberingPlanIndicator,
				addressPresentationRestrictedIndicator,
				addressSignal);
		
		m_numberIncompleteIndicator = numberIncompleteIndicator;

		if((screeningIndicator < SI_USER_PROVIDED_NOT_VERIFIED) || (screeningIndicator > SI_NETWORK_PROVIDED))
			throw new ParameterRangeInvalidException();

		else 
			m_screeningIndicator = screeningIndicator;			
	}
	
	public boolean getNumberIncompleteInd(){
		return m_numberIncompleteIndicator;
	}

	public void setNumberIncompleteInd(boolean numberIncompleteIndicator){
		m_numberIncompleteIndicator =  numberIncompleteIndicator;
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

	public void  putGenericNumberItu(byte[] arr, int index, byte par_len){
		
		super.putGenericNumber(arr, index, par_len);
		if((byte)((arr[index+2] >> 7) & IsupMacros.L_BIT1_MASK) == 1)
			m_numberIncompleteIndicator = true;
		m_screeningIndicator       = (byte)(arr[index+2] & IsupMacros.L_bits21_MASK);
		
	}

	public byte[] flatGenericNumberItu()
	{
			byte[] rc;
			byte ni = 0;

			if(m_numberIncompleteIndicator == true)
				ni = 1;
			
			rc = super.flatGenericNumber();
			
			rc[2] = (byte) (rc[2] |
				((ni << 7) & IsupMacros.L_BIT8_MASK) |
				(m_screeningIndicator & IsupMacros.L_bits21_MASK));
				
			return rc;

	}

	
	/**
    * String representation of class GenericNumberItu
    *
    * @return    String provides description of class GenericNumberItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nm_numberIncompleteIndicator = ");
				buffer.append(m_numberIncompleteIndicator);
				buffer.append("\nm_screeningIndicator  = ");
				buffer.append(m_screeningIndicator);								
				return buffer.toString();
		
		}		
	
	boolean m_numberIncompleteIndicator;
	byte    m_screeningIndicator;

	public static final boolean NII_NUMBER_COMPLETE = false; 
	public static final boolean NII_NUMBER_INCOMPLETE = true; 
	public static final byte SI_USER_PROVIDED_NOT_VERIFIED = 0x00; 
	public static final byte SI_USER_PROVIDED_VERIFIED_PASSED = 0x01; 
	public static final byte SI_USER_PROVIDED_VERIFIED_FAILED = 0x02; 
	public static final byte SI_NETWORK_PROVIDED = 0x03; 


}




