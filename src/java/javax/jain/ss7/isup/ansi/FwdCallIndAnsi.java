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
* DATE         : $Date: 2008/05/16 12:24:10 $
* 
* MODULE NAME  : $RCSfile: FwdCallIndAnsi.java,v $
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


package javax.jain.ss7.isup.ansi;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class FwdCallIndAnsi extends FwdCallInd{

	public FwdCallIndAnsi(){

	}

	public FwdCallIndAnsi(byte endToEndMethodIndicator,
                      boolean interworkingIndicator,
                      boolean isdnUserPartIndicator,
                      byte isdnUserPartPreferenceIndicator,
                      boolean isdnAccessIndicator,
                      byte sccpMethodIndicator,
                      boolean iamSegmentationIndicator,
                      boolean typeOfCallInd)
               throws ParameterRangeInvalidException{

		super(endToEndMethodIndicator,
                  	interworkingIndicator,
                  	isdnUserPartIndicator,
                  	isdnUserPartPreferenceIndicator,
                  	isdnAccessIndicator,
                  	sccpMethodIndicator,
                  	typeOfCallInd);
		
		m_iamSegmentationIndicator = iamSegmentationIndicator;
	}

	public boolean getIAMSegInd(){
		return m_iamSegmentationIndicator;
	}

	public void setIAMSegInd(boolean iamSegmentationIndicator){
		m_iamSegmentationIndicator = iamSegmentationIndicator;
	}

	public byte[] flatFwdCallIndAnsi(){

		byte[] fwdcallind = null;		
		byte iamSegmentationIndicator = 0;		

		fwdcallind = super.flatFwdCallInd();
		
		if(	m_iamSegmentationIndicator == true)
			iamSegmentationIndicator = 1;
		
		fwdcallind[0] = (byte)(fwdcallind[0]  | ((iamSegmentationIndicator << 4)&IsupMacros.L_BIT5_MASK));
										
		return fwdcallind;
	}

	public void  putFwdCallIndAnsi(byte[] arr, int index, byte par_len){
		
		super.putFwdCallInd(arr, index, par_len);
		
		if((byte)((arr[index]>>4) & IsupMacros.L_BIT1_MASK) == 1)
			m_iamSegmentationIndicator = true;

	}	

	/**
    * String representation of class FwdCallIndAnsi
    *
    * @return    String provides description of class FwdCallIndAnsi
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nm_iamSegmentationIndicator = ");
				buffer.append(m_iamSegmentationIndicator);
				return buffer.toString();
		
		}

	boolean m_iamSegmentationIndicator;

	public static final boolean ISI_NO_IAM_SEGMENTATION_IND = false; 
	public static final boolean ISI_IAM_SEGMENTATION_USED = true; 

    
}


