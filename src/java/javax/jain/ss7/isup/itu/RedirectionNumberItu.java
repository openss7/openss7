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
* DATE         : $Date: 2008/05/16 12:23:58 $
* 
* MODULE NAME  : $RCSfile: RedirectionNumberItu.java,v $
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

public class RedirectionNumberItu extends NumberParameter{

	public RedirectionNumberItu(){

	}

	public RedirectionNumberItu(byte natureOfAddressIndicator,
                            byte numberingPlanIndicator,
                            byte[] addressSignal,
                            boolean internalNetworkNumberIndicator)
                     throws ParameterRangeInvalidException{
		
		super(natureOfAddressIndicator,
              numberingPlanIndicator,
              addressSignal);
		m_internalNetworkNumberIndicator = internalNetworkNumberIndicator;
	}
	
	public boolean getIntNwNumInd(){
		return m_internalNetworkNumberIndicator;
	}

	public void setIntNwNumInd(boolean internalNetworkNumberIndicator){
		m_internalNetworkNumberIndicator = internalNetworkNumberIndicator;
	}
	
	public void  putRedirectionNumberItu(byte[] arr, int index, byte par_len){
		
			super.putNumberParameter(arr, index, par_len);
			if((byte)((arr[index+1] >> 7) & IsupMacros.L_BIT1_MASK) == 1)
				m_internalNetworkNumberIndicator = true;
	}

	public byte[] flatRedirectionNumberItu()
	{
		byte[] rc;
		byte in = 0;			

		if(m_internalNetworkNumberIndicator == true)
			in = 1;


		rc = super.flatNumberParameter();			

		rc[1] = (byte) (rc[1] | ((in << 7) & IsupMacros.L_BIT8_MASK ));
				

		return rc;			
			
	}
		
	/**
    * String representation of class RedirectionNumberItu
    *
    * @return    String provides description of class RedirectionNumberItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nm_internalNetworkNumberIndicator  = ");
				buffer.append(m_internalNetworkNumberIndicator);	
				return buffer.toString();
		
		}

	boolean m_internalNetworkNumberIndicator;

	public static final boolean INNI_ROUTING_TO_INTERNAL_NETWORK_NUMBER_ALLOWED = false; 
	public static final boolean INNI_ROUTING_TO_INTERNAL_NETWORK_NUMBER_NOT_ALLOWED = true; 

	
}




