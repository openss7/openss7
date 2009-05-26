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
* MODULE NAME  : $RCSfile: OptionalBwdCallInd.java,v $
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

public abstract class OptionalBwdCallInd extends java.lang.Object 
										implements java.io.Serializable{
		
	protected OptionalBwdCallInd(){

	}

	protected OptionalBwdCallInd(boolean in_inbandInfoInd){
		m_inbandInfoInd = in_inbandInfoInd;
	}

	public boolean getInBandInfoInd(){

		return m_inbandInfoInd;
	}

	public void setInBandInfoInd(boolean aInBandInformationIndicator){
		m_inbandInfoInd = aInBandInformationIndicator;
	}
		
	public void  putOptionalBwdCallInd(byte[] arr, int index, byte par_len){
		
		if((byte)(arr[index] & IsupMacros.L_BIT1_MASK) == 1)
				m_inbandInfoInd = true;				
	}	

	public byte flatOptionalBwdCallInd()
	{

		byte rc = 0;
		byte inbandInfoInd = 0;
		
		if(m_inbandInfoInd == true)
			inbandInfoInd = 1;

		rc = (byte)(inbandInfoInd & IsupMacros.L_BIT1_MASK);
					
		return rc;
	}	

	/**
    * String representation of class OptionalBwdCallInd
    *
    * @return    String provides description of class OptionalBwdCallInd
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ninbandInfoInd = ");
				buffer.append(m_inbandInfoInd);
				return buffer.toString();
		
		}

	
	
	boolean m_inbandInfoInd;
		
	public static final boolean III_NO_INDICATION = false; 
	public static final boolean III_INBAND_INFORMATION_OR_PATTERN_AVAILABLE = true; 

}




