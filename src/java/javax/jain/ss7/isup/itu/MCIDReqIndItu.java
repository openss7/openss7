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
* MODULE NAME  : $RCSfile: MCIDReqIndItu.java,v $
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

public class MCIDReqIndItu extends java.lang.Object implements java.io.Serializable{

	public MCIDReqIndItu(){

	}

	public MCIDReqIndItu(boolean in_mcidReqInd,
                     boolean in_holdingInd){
		
		holdingInd = in_holdingInd;
		mcidReqInd = in_mcidReqInd;
	}
	
	public void setMCIDReqInd(boolean in_mcidReqInd){
		mcidReqInd = in_mcidReqInd;
	}

	public boolean getMCIDReqInd(){
		return mcidReqInd;
	}

	public boolean getHoldingInd(){
		return holdingInd;
	}

	public void setHoldingInd(boolean in_holdingInd){
		holdingInd = in_holdingInd;
	}	

	public void  putMCIDReqIndItu(byte[] arr, int index, byte par_len){
				
		if((byte)((arr[index]) & IsupMacros.L_BIT1_MASK) == 1)
			mcidReqInd = true;
		if((byte)((arr[index] >> 1) & IsupMacros.L_BIT1_MASK) == 1)
			holdingInd = true;

	}

	public byte flatMCIDReqIndItu()
	{

		byte rc = 0;
		byte mcidReq=0, holdInd=0;

		if(mcidReqInd == true)
			mcidReq = 1;
		if(holdingInd == true)
			holdInd = 1;
		
		rc = (byte)((mcidReq & IsupMacros.L_BIT1_MASK) | 
					((holdInd & IsupMacros.L_BIT1_MASK) << 1));
					
		return rc;
	}			

	/**
    * String representation of class MCIDReqIndItu
    *
    * @return    String provides description of class MCIDReqIndItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nholdingInd = ");
				buffer.append(holdingInd);	
				buffer.append("\nmcidReqInd = ");
				buffer.append(mcidReqInd);
				return buffer.toString();
		
		}

	boolean holdingInd;
	boolean mcidReqInd;
	
	public static final boolean MRI_MCID_NOT_REQUESTED = false; 
	public static final boolean MRI_MCID_REQUESTED = true; 
	public static final boolean HI_HOLDING_NOT_REQUESTED = false; 
	public static final boolean HI_HOLDING_REQUESTED = true; 

}




