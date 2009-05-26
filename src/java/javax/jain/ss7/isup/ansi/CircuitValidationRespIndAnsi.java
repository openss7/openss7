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
* MODULE NAME  : $RCSfile: CircuitValidationRespIndAnsi.java,v $
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

public class CircuitValidationRespIndAnsi extends java.lang.Object implements java.io.Serializable{

	public CircuitValidationRespIndAnsi(){

	}

	public CircuitValidationRespIndAnsi(byte in_cktValRespInd)
                             throws ParameterRangeInvalidException{
		if((in_cktValRespInd >= 0) && (in_cktValRespInd <= 255))
			cktValRespInd = in_cktValRespInd;
		else throw new ParameterRangeInvalidException();
	}

	public short getCircuitValidationRespInd(){

		return cktValRespInd;
	}

	public void setCircuitValidationRespInd(short in_cktValRespInd)
                                 throws ParameterRangeInvalidException{
		if((in_cktValRespInd >= 0) && (in_cktValRespInd <= 255))
			cktValRespInd = in_cktValRespInd;
		else throw new ParameterRangeInvalidException();
	}

	public void  putCircuitValidationRespIndAnsi(byte[] arr,int index,byte par_len){

		cktValRespInd = arr[index];
	
		
	}

	public byte flatCircuitValidationRespIndAnsi(){

		byte rc = 0;
    		
		rc = (byte)cktValRespInd;

		return rc;

	}		
	
	/**
    * String representation of class CircuitValidationRespIndAnsi
    *
    * @return    String provides description of class CircuitValidationRespIndAnsi
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncktValRespInd = ");
				buffer.append(cktValRespInd);
				return buffer.toString();
		
		}



	short cktValRespInd;

	public static final short CVRI_SUCCESSFUL = 0x00; 
	public static final short CVRI_FAILURE    = 0x01; 

}	


