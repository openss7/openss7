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
* DATE         : $Date: 2008/05/16 12:24:11 $
* 
* MODULE NAME  : $RCSfile: OptionalBwdCallIndAnsi.java,v $
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

public class OptionalBwdCallIndAnsi extends OptionalBwdCallInd{

	public OptionalBwdCallIndAnsi(){

	}

	public OptionalBwdCallIndAnsi(boolean in_inbandInfoInd,
                              boolean in_callFwdInd,
                              boolean in_nwExcessDelayInd,
                              boolean in_userNwInteractionInd){

		super(in_inbandInfoInd);
		callFwdInd           = in_callFwdInd;
		nwExcessDelayInd     = in_nwExcessDelayInd;
		userNwInteractionInd = in_userNwInteractionInd;
	}
	
	public boolean getCallFwdInd(){
		return callFwdInd;
	}

	public void setCallFwdInd(boolean aCallFwdInd){
		callFwdInd = aCallFwdInd;
	}

	public boolean getNetworkExcessiveDelayInd(){
		return nwExcessDelayInd;
	}

	public void setNetworkExcessiveDelayInd(boolean aNetworkExcessiveDelayIndicator){
		nwExcessDelayInd = aNetworkExcessiveDelayIndicator;
	}

	public boolean getUserNetworkInteractionInd(){
		return userNwInteractionInd;
	}

	public void setUserNetworkInteractionInd(boolean aUserNetworkInteractionIndicator){
		userNwInteractionInd = aUserNetworkInteractionIndicator;
	}
		
	public void putOptionalBwdCallIndAnsi(byte[] arr, int index, byte par_len)
	{
		super.putOptionalBwdCallInd(arr,index,par_len);

		if((byte)((arr[index]>>1) & IsupMacros.L_BIT1_MASK) == 1)
			callFwdInd = true;
		if((byte)((arr[index]>>6) & IsupMacros.L_BIT1_MASK) == 1)
			 nwExcessDelayInd = true;
		if((byte)((arr[index]>>7) & IsupMacros.L_BIT1_MASK) == 1)
			userNwInteractionInd = true;
	}

	public byte flatOptionalBwdCallIndAnsi()
	{

		byte rc = 0;
		byte callFwd = 0, nwExcessDelay = 0, userNwInteraction = 0;

		if( callFwdInd== true)
			callFwd = 1;
		if(nwExcessDelayInd == true)
			nwExcessDelay = 1;
		if(userNwInteractionInd == true)
			userNwInteraction = 1;
		
		rc = super.flatOptionalBwdCallInd();

		rc = (byte)(rc | (byte)(((callFwd & IsupMacros.L_BIT1_MASK) << 1) |
						((nwExcessDelay & IsupMacros.L_BIT1_MASK) << 6) |
						((userNwInteraction & IsupMacros.L_BIT1_MASK) << 7)));				

		return rc;
	}	
					

	/**
    * String representation of class OptionalBwdCallIndAnsi
    *
    * @return    String provides description of class OptionalBwdCallIndAnsi
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncallFwdInd = ");
				buffer.append(callFwdInd);
				buffer.append("\nwExcessDelayIndn = ");
				buffer.append(nwExcessDelayInd);
				buffer.append("\nuserNwInteractionInd = ");
				buffer.append(userNwInteractionInd);
				return buffer.toString();
		
		}

	
	boolean callFwdInd;
	boolean nwExcessDelayInd;
	boolean userNwInteractionInd;

	public static final boolean CFI_NO_INDICATION = false; 
	public static final boolean CFI_CALL_FORWARDING_MAY_OCCUR = true; 
	public static final boolean NEDI_NO_INDICATION = false; 
	public static final boolean NEDI_NETWORK_EXCESSIVE_DELAY_ENCOUNTERED = true; 
	public static final boolean UNII_NO_INDICATION = false; 
	public static final boolean UNII_USER_NETWORK_INTERACTION_OCCURS = true; 


}


