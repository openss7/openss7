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
* MODULE NAME  : $RCSfile: InfoReqIndAnsi.java,v $
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

public class InfoReqIndAnsi extends InfoReqInd{

	public InfoReqIndAnsi(){
		multiLocBGInfoReqInd = false;
	}
		
	public InfoReqIndAnsi(boolean in_callingAddrReqInd,
                      boolean in_holdingInd,
                      boolean in_callingCatReqInd,
                      boolean in_chargeInfoReqInd,
                      boolean in_mciReqInd,
                      boolean in_multiLocBGInfoInd){	

		super(in_callingAddrReqInd,
				in_holdingInd,
				in_callingCatReqInd,
				in_chargeInfoReqInd,
				in_mciReqInd);


		 multiLocBGInfoReqInd = in_multiLocBGInfoInd;

	}

	public boolean getMultiLocBusinessGroupInfoReqInd(){
		return multiLocBGInfoReqInd;
	}

	public void setMultiLocBusinessGroupInfoReqInd(boolean multiLocBGInfoInd){
		multiLocBGInfoReqInd = multiLocBGInfoInd;
	}
	
	public void  putInfoReqIndAnsi(byte[] arr, int index, byte par_len){

			if(((arr[index+1] >> 7) & IsupMacros.L_BIT1_MASK) == 1)
				multiLocBGInfoReqInd = true;

			super.putInfoReqInd(arr,index,par_len);											
								
	}
	
	public byte[] flatInfoReqIndAnsi(){
		
			byte[] rc;
			byte multiLocBGInfoReq = 0;

			rc = super.flatInfoReqInd();

			if(multiLocBGInfoReqInd == true)
				multiLocBGInfoReq = 1;
			
			rc[1] = (byte)(rc[1] | ((multiLocBGInfoReq  << 7 ) & IsupMacros.L_BIT8_MASK));
		
			return rc;
		}		

		

	/**
    * String representation of class InfoReqIndAnsi 
    *
    * @return    String provides description of class InfoReqIndAnsi
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nmultiLocBGInfoReqInd = ");
				buffer.append(multiLocBGInfoReqInd);
				return buffer.toString();
		
		}

	boolean multiLocBGInfoReqInd;	

    	public static final boolean MLBGII_MULTI_LOCATION_BUSINESS_GROUP_INFORMATION_NOT_REQUESTED = false; 
	public static final boolean MLBGII_MULTI_LOCATION_BUSINESS_GROUP_INFORMATION_REQUESTED = true; 

}


