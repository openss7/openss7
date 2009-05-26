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
* DATE         : $Date: 2008/05/16 12:24:12 $
* 
* MODULE NAME  : $RCSfile: TransitNwSelAnsi.java,v $
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


public class TransitNwSelAnsi extends TransitNwSel{

	public TransitNwSelAnsi(){
	}
	
	public TransitNwSelAnsi(byte in_nwIdPlan,
                        byte in_typeOfNwId,
                        byte[] in_nwId,
                        byte in_cktCode)
                 throws ParameterRangeInvalidException{
		super(in_nwIdPlan,
				in_typeOfNwId,
				in_nwId);
		if((in_cktCode >=CC_UNSPECIFIED) && (in_cktCode <= CC_INTERNATIONAL_CALL_OPERATOR_REQUESTED))
				cktCode = in_cktCode;
		else throw new ParameterRangeInvalidException();
	}

	public byte getCircuitCode(){
		return cktCode;
	}

	public void setCircuitCode(byte circuitCode)
                    throws ParameterRangeInvalidException{
		if((circuitCode >=CC_UNSPECIFIED) && (circuitCode <= CC_INTERNATIONAL_CALL_OPERATOR_REQUESTED))
				cktCode = circuitCode;
		else throw new ParameterRangeInvalidException();
	}

	public void  putTransitNwSelAnsi(byte[] arr, int index, byte par_len){

		
		super.putTransitNwSel(arr,index,par_len);
		
		cktCode = (byte)((arr[index+3] >> 4) & IsupMacros.L_bits41_MASK);

					
	}

	public byte[] flatTransitNwSelAnsi(){
		
		byte[] rc = null;

		rc = super.flatTransitNwSel();
				
		rc[4] = (byte)(rc[4] | ((cktCode << 4) & IsupMacros.L_bits85_MASK));						
		
		return rc;
	}
			

	/**
    * String representation of class TransitNwSelAnsi
    *
    * @return    String provides description of class TransitNwSelAnsi
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncktCode = ");
				buffer.append(cktCode);
				return buffer.toString();
		
		}

	byte cktCode;

	public static final byte CC_UNSPECIFIED = 0x00; 
	public static final byte CC_INTERNATIONAL_CALL_NO_OPERATOR_REQUESTED = 0x01; 
	public static final byte CC_INTERNATIONAL_CALL_OPERATOR_REQUESTED = 0x02; 



}


