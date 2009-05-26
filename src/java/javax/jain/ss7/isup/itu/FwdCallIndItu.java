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
* MODULE NAME  : $RCSfile: FwdCallIndItu.java,v $
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

public class FwdCallIndItu extends FwdCallInd{

	public FwdCallIndItu(){

		super();
		m_endToEndInformationIndicator = false;
	}
	
	public FwdCallIndItu(byte endToEndMethodIndicator,
                     boolean interworkingIndicator,
                     boolean isdnUserPartIndicator,
                     byte isdnUserPartPreferenceIndicator,
                     boolean isdnAccessIndicator,
                     byte sccpMethodIndicator,
                     boolean typeOfCallIndicator,
                     boolean endToEndInformationIndicator)
              throws ParameterRangeInvalidException{

		super(	endToEndMethodIndicator,
				interworkingIndicator,
				isdnUserPartIndicator,
				isdnUserPartPreferenceIndicator,
				isdnAccessIndicator,
				sccpMethodIndicator,
				typeOfCallIndicator);
		
		m_endToEndInformationIndicator = endToEndInformationIndicator;

	}

	public boolean getEndToEndInfoInd(){
		return m_endToEndInformationIndicator;
	}

	public void setEndToEndInfoInd(boolean endToEndInformationIndicator){
		m_endToEndInformationIndicator = endToEndInformationIndicator;
	}

	public byte[] flatFwdCallIndItu(){

		byte[] fwdcallind = null;		
		byte endtoendinf = 0;		

		fwdcallind = super.flatFwdCallInd();
		
		if(	m_endToEndInformationIndicator == true)
			endtoendinf = 1;
		
		fwdcallind[0] = (byte)(fwdcallind[0]  | ((endtoendinf << 4)&IsupMacros.L_BIT5_MASK));
										
		return fwdcallind;
	}

	public void  putFwdCallIndItu(byte[] arr, int index, byte par_len){
		
		super.putFwdCallInd(arr, index, par_len);
		
		if((byte)((arr[index]>>4) & IsupMacros.L_BIT1_MASK) == 1)
			m_endToEndInformationIndicator = true;

	}

	/**
    * String representation of class FwdCallIndItu
    *
    * @return    String provides description of class FwdCallIndItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nm_endToEndInformationIndicator = ");
				buffer.append(m_endToEndInformationIndicator);	
				return buffer.toString();
		
		}

	boolean m_endToEndInformationIndicator;

	public static final boolean EII_NO_END_TO_END_INFORMATION_AVAILABLE = false;
	public static final boolean EII_END_TO_END_INFORMATION_AVAILABLE=true;
	
}




