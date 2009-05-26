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
* MODULE NAME  : $RCSfile: OptionalFwdCallIndItu.java,v $
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

public class OptionalFwdCallIndItu extends java.lang.Object implements java.io.Serializable{

	public OptionalFwdCallIndItu(){

	}

	public OptionalFwdCallIndItu(byte in_cugCallInd,
                             boolean in_segInd,
                             boolean in_colrInd)
                      throws ParameterRangeInvalidException{

		if((in_cugCallInd >= CUGCI_NON_CUG_CALL) && 
		(in_cugCallInd <= CUGCI_CUG_CALL_OUTGOING_ACCESS_NOT_ALLOWED))
			cugCallInd  = in_cugCallInd;

		else 
			throw new ParameterRangeInvalidException();

		colrInd    = in_colrInd;
		segInd     = in_segInd;
	}

	public byte getClosedUserGroupCallInd(){
		return cugCallInd;
	}

	public void setClosedUserGroupCallInd(byte CUGcallInd)
                               throws ParameterRangeInvalidException{
		if((CUGcallInd >= CUGCI_NON_CUG_CALL) && 
		(CUGcallInd <= CUGCI_CUG_CALL_OUTGOING_ACCESS_NOT_ALLOWED))
			cugCallInd  = CUGcallInd;

		else 
			throw new ParameterRangeInvalidException();

	}
	
	public boolean getConnectedLineIdentityReqInd(){
		return colrInd;
	}

	public void setConnectedLineIdentityReqInd(boolean aCOLRInd){
		colrInd  = aCOLRInd;
	}

	public boolean getSegmentationInd(){
		return segInd;
	}

	public void setSegmentationInd(boolean aSimpleSegmentation){
		segInd = aSimpleSegmentation;
	}
	

	public byte flatOptionalFwdCallIndItu()
	{

		byte rc = 0;
		byte col =0, seg =0;

		if(colrInd == true)
			col = 1;
		if(segInd == true)
			seg = 1;

		rc = (byte)((cugCallInd &  IsupMacros.L_bits21_MASK) | 
			 ((seg << 2) & IsupMacros.L_BIT3_MASK) |
			 ((col << 7) & IsupMacros.L_BIT8_MASK));
		
		return rc;
	}

	public void  putOptionalFwdCallIndItu(byte[] arr, int index, byte par_len){

		cugCallInd = (byte)(arr[index] & IsupMacros.L_bits21_MASK);
		if((byte)((arr[index]>>2)&IsupMacros.L_BIT1_MASK) == 1)					
			segInd = true;
		if((byte)((arr[index]>>7)&IsupMacros.L_BIT1_MASK) == 1)					
			colrInd = true;
	}


	/**
    * String representation of class OptionalFwdCallIndItu
    *
    * @return    String provides description of class OptionalFwdCallIndItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncolrInd = ");
				buffer.append(colrInd);	
				buffer.append("\ncugCallInd = ");
				buffer.append(cugCallInd);
				buffer.append("\nsegInd = ");
				buffer.append(segInd);
				return buffer.toString();
		
		}


	boolean colrInd;
	byte    cugCallInd;
	boolean segInd;

	public static final byte CUGCI_NON_CUG_CALL = 0x00; 
	public static final byte CUGCI_CUG_CALL_OUTGOING_ACCESS_ALLOWED = 0x02;  
	public static final byte CUGCI_CUG_CALL_OUTGOING_ACCESS_NOT_ALLOWED = 0x03; 
	public static final boolean SSI_NO_ADDITIONAL_INFORMATION = false;
	public static final boolean SSI_ADDITIONAL_INFORMATION_SENT_IN_SEGMENTATION = true;
	public static final boolean CLIRI_NOT_REQUESTED = false;
	public static final boolean CLIRI_REQUESTED = true;


}




