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
* MODULE NAME  : $RCSfile: GenericNameAnsi.java,v $
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

public class GenericNameAnsi extends java.lang.Object implements java.io.Serializable{

	public GenericNameAnsi(){

	}

	public GenericNameAnsi(byte in_pres,
                       boolean in_isAvail,
                       byte in_typeOfName,
                       java.lang.String in_genName){

		genName     = in_genName;
		isAvail     = in_isAvail;
		pres        = in_pres;
		typeOfName  = in_typeOfName;

	}

	public boolean getAvailability(){
		return isAvail;
	}

	public void setAvailability(boolean aAvailability){
		isAvail     = aAvailability;
	}

	public java.lang.String getName(){
		return genName;
	}

	public void setName(java.lang.String aName)
             throws ParameterRangeInvalidException{
		genName     = aName;
	}

	public byte getPresentation(){
		return pres;
	}

	public void setPresentation(byte aPresentation)
         	      throws ParameterRangeInvalidException{
		if((aPresentation >= PRESENTATION_ALLOWED) && (aPresentation <= PRESENTATION_NO_INDICATION))
			pres        = aPresentation;
		else throw new ParameterRangeInvalidException();
	}

	public byte getTypeOfName(){
		return typeOfName;
	}

	public void setTypeOfName(byte aTypeOfName)
                   throws ParameterRangeInvalidException{
		if((aTypeOfName >= TN_CALLING_NAME) && (aTypeOfName <= TN_CONNECTED_NAME))
			typeOfName  = aTypeOfName;
		else throw new ParameterRangeInvalidException();
	}

	public void  putGenericNameAnsi(byte[] arr,int index,byte par_len){

		pres = (byte)(arr[index] & IsupMacros.L_bits21_MASK);
		
		if(((arr[index] >> 4) & IsupMacros.L_BIT1_MASK) == 1)
				isAvail = true;		

		typeOfName = (byte)((arr[index] >> 5) & IsupMacros.L_bits31_MASK);

		genName = new String(arr,index+1,par_len-1);
		
	}

	public byte[] flatGenericNameAnsi(){

		
		byte Avail = 0; 		
		byte[] str = null;
		
		str = genName.getBytes();

		byte[] rc = ByteArray.getByteArray(str.length+1);	

		if(isAvail == true)
			Avail = 1;
				
		rc[0] = (byte)((pres & IsupMacros.L_bits21_MASK)|
						((Avail & IsupMacros.L_BIT1_MASK)<<4)| 
						((typeOfName & IsupMacros.L_bits31_MASK)<<5));
						
		
	
		System.arraycopy(str,0,rc,1,str.length);		
		

		return rc;
	}

	/**
    * String representation of class GenericNameAnsi
    *
    * @return    String provides description of class GenericNameAnsi
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ngenName = ");
				buffer.append(genName);
				buffer.append("\nisAvail = ");
				buffer.append(isAvail);
				buffer.append("\npres = ");
				buffer.append(pres);
				buffer.append("\ntypeOfName = ");
				buffer.append(typeOfName);
				return buffer.toString();
		
		}


	java.lang.String genName;
	boolean          isAvail;
	byte             pres;
	byte             typeOfName;


	public static final byte PRESENTATION_ALLOWED = 0x00; 
	public static final byte PRESENTATION_RESTRICTED = 0x01; 
	public static final byte PRESENTATION_BLOCKING_TOGGLE = 0x02; 
	public static final byte PRESENTATION_NO_INDICATION = 0x03; 
	public static final boolean AVAILABILITY_NAME_AVAILABLE_UNKNOWN = false; 
	public static final boolean AVAILABILITY_NAME_UNAVAILABLE = true; 
	public static final byte TN_CALLING_NAME = 0x01; 
	public static final byte TN_ORIGINAL_CALLED_NAME = 0x02; 
	public static final byte TN_REDIRECTING_NAME = 0x03; 
	public static final byte TN_CONNECTED_NAME = 0x04; 
   
}


