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
* MODULE NAME  : $RCSfile: InfoIndAnsi.java,v $
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

public class InfoIndAnsi extends InfoInd{



	public InfoIndAnsi(){
		multiLocBGInfoRespInd = false;
	}

	public InfoIndAnsi(byte in_callingAddrRespInd,
                   boolean in_holdInd,
                   boolean in_callingCatResInd,
                   boolean in_chargeInfoResInd,
                   boolean in_solInfoInd,
                   boolean in_multiLocBGInfoRespInd){

		super(in_callingAddrRespInd,
				in_holdInd,
				in_callingCatResInd,
				in_chargeInfoResInd,
				in_solInfoInd);


		multiLocBGInfoRespInd = in_multiLocBGInfoRespInd;

	}

	public boolean getMultiLocBusinessGroupInfoRespInd(){
		return multiLocBGInfoRespInd;
	}

	public void setMultiLocBusinessGroupInfoRespInd(boolean aMultiLocationBGInfoRespIndicator){
		multiLocBGInfoRespInd = aMultiLocationBGInfoRespIndicator;
	}
		
	public void  putInfoIndAnsi(byte[] arr, int index, byte par_len){

			if(((arr[index+1] >> 7) & IsupMacros.L_BIT1_MASK) == 1)
				multiLocBGInfoRespInd = true;

			super.putInfoInd(arr,index,par_len);											
								
	}
	
	public byte[] flatInfoIndAnsi(){
		
			byte[] rc;
			byte multiLocBGInfoResp = 0;

			rc = super.flatInfoInd();

			if(multiLocBGInfoRespInd == true)
				multiLocBGInfoResp = 1;
			
			rc[1] = (byte)(rc[1] | (( multiLocBGInfoResp  << 7 ) & IsupMacros.L_BIT8_MASK));
		
			return rc;
		}		


	/**
    * String representation of class InfoIndAnsi
    *
    * @return    String provides description of class InfoIndAnsi
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nmultiLocBGInfoRespInd = ");
				buffer.append(multiLocBGInfoRespInd);
				return buffer.toString();
		
		}

	boolean multiLocBGInfoRespInd;
		
	public static final boolean MLBGIRI_MULTI_LOCATION_BUSINESS_GROUP_INFORMATION_NOT_INCLUDED = false; 
	public static final boolean MLBGIRI_MULTI_LOCATION_BUSINESS_GROUP_INFORMATION_INCLUDED = true; 


    
}


