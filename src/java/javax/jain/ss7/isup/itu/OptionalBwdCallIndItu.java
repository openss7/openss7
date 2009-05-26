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
* MODULE NAME  : $RCSfile: OptionalBwdCallIndItu.java,v $
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

public class OptionalBwdCallIndItu extends OptionalBwdCallInd{

	public OptionalBwdCallIndItu(){
	
	}

	public OptionalBwdCallIndItu(boolean in_inbandInfoInd,
                             boolean in_callDivInd,
                             boolean in_segInd,
                             boolean in_mlppUserInd){
		super(in_inbandInfoInd);
		callDivInd  = in_callDivInd;
		mlppUserInd = in_mlppUserInd;
		segInd      = in_segInd; 
	}

	public boolean getCallDiversionInd(){
		return callDivInd;
	}	

	public void setCallDiversionInd(boolean aCallDiversionMayOccurIndicator){
		callDivInd = aCallDiversionMayOccurIndicator;
	}

	public boolean getMLPPUserInd(){
		return mlppUserInd;
	}

	public void setMLPPUserInd(boolean aMLPPUserIndicator){
		mlppUserInd = aMLPPUserIndicator;
	}

	public boolean getSegmentationInd(){
		return segInd;
	}

	public void setSegmentationInd(boolean aSimpleSegmentation){
		segInd = aSimpleSegmentation;
	}

	public void putOptionalBwdCallIndItu(byte[] arr, int index, byte par_len)
	{
		super.putOptionalBwdCallInd(arr,index,par_len);

		if((byte)((arr[index]>>1) & IsupMacros.L_BIT1_MASK) == 1)
			callDivInd = true;
		if((byte)((arr[index]>>2) & IsupMacros.L_BIT1_MASK) == 1)
			segInd = true;
		if((byte)((arr[index]>>3) & IsupMacros.L_BIT1_MASK) == 1)
			mlppUserInd = true;
	}

	public byte flatOptionalBwdCallIndItu()
	{

		byte rc = 0;
		byte callDiv = 0, mlppUser = 0, seg = 0;

		if(callDivInd == true)
			callDiv = 1;
		if(mlppUserInd == true)
			mlppUser = 1;
		if(segInd == true)
			seg = 1;
		
		rc = super.flatOptionalBwdCallInd();

		rc = (byte)(rc | (byte)(((callDiv & IsupMacros.L_BIT1_MASK) << 1) |
						((seg & IsupMacros.L_BIT1_MASK) << 2) |
						((mlppUser & IsupMacros.L_BIT1_MASK) << 3)));				

		return rc;
	}	

	/**
    * String representation of class OptionalBwdCallIndItu
    *
    * @return    String provides description of class OptionalBwdCallIndItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncallDivInd = ");
				buffer.append(callDivInd);	
				buffer.append("\nmlppUserInd = ");
				buffer.append(mlppUserInd);
				buffer.append("\nsegInd = ");
				buffer.append(segInd);
				return buffer.toString();
		
		}

	
	boolean callDivInd;
	boolean mlppUserInd;
	boolean segInd;

	public static final boolean CDI_NO_INDICATION = false; 
	public static final boolean CDI_CALL_DIVERSION_MAY_OCCUR = true; 
	public static final boolean SSI_NO_ADDITIONAL_INFORMATION = false; 
	public static final boolean SSI_ADDITIONAL_INFORMATION_SENT_IN_SEGMENTATION = true; 
	public static final boolean MUI_NO_INDICATION = false; 
	public static final boolean MUI_MLPP_USER = true; 


}




