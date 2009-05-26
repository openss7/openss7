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
* DATE         : $Date: 2008/05/16 12:24:09 $
* 
* MODULE NAME  : $RCSfile: CircuitIdNameAnsi.java,v $
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


public class CircuitIdNameAnsi extends java.lang.Object implements java.io.Serializable{

	public CircuitIdNameAnsi(){

	}

	public CircuitIdNameAnsi(long in_trunkNumber,
                         CommonLangLocationIdAnsi in_clliCodeA,
                         CommonLangLocationIdAnsi in_clliCodeB)
                  throws ParameterRangeInvalidException{
		
		clliCodeA      = in_clliCodeA;
		clliCodeB      = in_clliCodeB;
		if((in_trunkNumber >=0 ) && (in_trunkNumber <= 32))
			trunkNumber    = in_trunkNumber;
		else throw new ParameterRangeInvalidException();
	
	}
	

	public CommonLangLocationIdAnsi getCLLICodeA(){
		return clliCodeA;
	}
	
	public void setCLLICodeA(CommonLangLocationIdAnsi in_clliCodeA){
		clliCodeA = in_clliCodeA;
	}

	public CommonLangLocationIdAnsi getCLLICodeB(){
		return clliCodeB;
	}

	public void setCLLICodeB(CommonLangLocationIdAnsi in_clliCodeB){
		clliCodeB = in_clliCodeB;
	}

	public long getTrunkNumber(){
		return trunkNumber;
	}

	public void setTrunkNumber(long in_trunkNumber)
                    throws ParameterRangeInvalidException{

		if((in_trunkNumber >=0 ) && (in_trunkNumber <= 32))
			trunkNumber    = in_trunkNumber;
		else throw new ParameterRangeInvalidException();
	}
	
	
	
	public void  putCircuitIdNameAnsi(byte[] arr,int index,byte par_len){
		
		trunkNumber = ((arr[index]<< 24)&0xFF000000) + 
						((arr[index+1]<< 16)&0x00FF0000) + 
						((arr[index+2] << 8) & 0x0000FF00) + 
						arr[index+3];

		clliCodeA = new CommonLangLocationIdAnsi();

		clliCodeA.putCommonLangLocationIdAnsi(arr,index+4,(byte)0);
				
		clliCodeB = new CommonLangLocationIdAnsi();

		clliCodeB.putCommonLangLocationIdAnsi(arr,index+15,(byte)0);
		
	}

	public byte[] flatCircuitIdNameAnsi(){

		byte[] rc = ByteArray.getByteArray(26);	
		
		int i;	
		
		rc[0] = (byte)((trunkNumber >> 24) & 0x000000FF);
		rc[1] = (byte)((trunkNumber >> 16) & 0x000000FF);
		rc[2] = (byte)((trunkNumber >> 8) & 0x000000FF);
		rc[3] = (byte)((trunkNumber) & 0x000000FF);
		
		System.arraycopy(clliCodeA.flatCommonLangLocationIdAnsi(),0,rc,4,11);
		
		System.arraycopy(clliCodeB.flatCommonLangLocationIdAnsi(),0,rc,15,11);			
						
		return rc;
	}


	/**
    * String representation of class CircuitIdNameAnsi
    *
    * @return    String provides description of class CircuitIdNameAnsi
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nclliCodeA = ");
				buffer.append(clliCodeA);
				buffer.append("\nclliCodeB = ");
				buffer.append(clliCodeB);
				buffer.append("\ntrunkNumber = ");
				buffer.append(trunkNumber);
				return buffer.toString();
		
		}

	
	CommonLangLocationIdAnsi clliCodeA;
	CommonLangLocationIdAnsi clliCodeB;
	long                     trunkNumber;

    
}


