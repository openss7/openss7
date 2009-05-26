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
* DATE         : $Date: 2008/05/16 12:23:53 $
* 
* MODULE NAME  : $RCSfile: BwdCallIndItu.java,v $
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

public class BwdCallIndItu extends BwdCallInd{

	public BwdCallIndItu(){
		eteInfoInd = false;
	}

	public BwdCallIndItu(byte in_chargeInd,
                     byte in_calledStatusInd,
                     byte in_calledCatInd,
                     byte in_eteMethodInd,
                     boolean in_iwInd,
                     boolean in_isupInd,
                     boolean in_holdInd,
                     boolean in_isdnAccessInd,
                     boolean in_echoDevInd,
                     byte in_sccpMethodInd,
                     boolean in_eteInfoInd)
              throws ParameterRangeInvalidException {

		super(	in_chargeInd,
				in_calledStatusInd,
				in_calledCatInd,
				in_eteMethodInd,
				in_iwInd,
				in_isupInd,
				in_holdInd,
				in_isdnAccessInd,
				in_echoDevInd,
				in_sccpMethodInd);
		
		eteInfoInd = in_eteInfoInd;		

	}

	public boolean getETEInfoInd(){	
	
		return eteInfoInd;
	}

	public void setETEInfoInd(boolean aEndToEndInformationIndicator){
		eteInfoInd = aEndToEndInformationIndicator;
	}

	public void  putBwdCallIndItu(byte[] arr,int index,byte par_len){

		super.putBwdCallInd(arr,index,par_len);
		if(((arr[index+1] >> 1) & IsupMacros.L_BIT1_MASK) == 1)
				eteInfoInd = true;
		
	}

	public byte[] flatBwdCallIndItu(){

		byte[] rc = null;		
		byte  eteInfoind= 0;		

		rc = super.flatBwdCallInd();
		
		if(	eteInfoInd == true)
			eteInfoind = 1;
			
		
		rc[1] = (byte)(rc[1]  | ((eteInfoind << 1)&IsupMacros.L_BIT2_MASK));				
		return rc;
	}

		
	/**
    * String representation of class BwdCallIndItu
    *
    * @return    String provides description of class BwdCallIndItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\neteInfoInd  = ");
				buffer.append(eteInfoInd);	
				return buffer.toString();
		
		}


	boolean eteInfoInd;

	public static final boolean EII_NO_END_TO_END_INFORMATION_AVAILABLE = false;
	public static final boolean EII_END_TO_END_INFORMATION_AVAILABLE =true;

	
}




