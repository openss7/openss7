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
* DATE         : $Date: 2008/05/16 12:23:53 $
* 
* MODULE NAME  : $RCSfile: CUGInterlockCodeItu.java,v $
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


public class CUGInterlockCodeItu extends java.lang.Object implements java.io.Serializable{


	public CUGInterlockCodeItu(){

	}

	public CUGInterlockCodeItu(int in_binaryCode,
                           java.lang.String in_networkIdentity)
                    throws ParameterRangeInvalidException{
		binaryCode      = in_binaryCode;
		networkIdentity = in_networkIdentity;
	}
	
	public int getBinaryCode(){
		return binaryCode;
	}

	public void setBinaryCode(int in_binaryCode)
                   throws ParameterRangeInvalidException{
		binaryCode = in_binaryCode;
	}

	public java.lang.String getNetworkIdentity(){
		return networkIdentity;
	}	

	public void setNetworkIdentity(java.lang.String in_networkIdentity)
                        throws ParameterRangeInvalidException{

		networkIdentity = in_networkIdentity;
	}
	
	public byte[] flatCUGInterlockCodeItu()
	{
		byte[] rc = ByteArray.getByteArray(4);

		byte[] nID = networkIdentity.getBytes();
		
		System.arraycopy(nID,0,rc,0,2);				

		rc[2] = (byte)((binaryCode >> 8) & 0xFF);
		rc[3] = (byte)(binaryCode & 0xFF);
	
		return rc;
		
	}		

	public void  putCUGInterlockCodeItu(byte[] arr, int index, byte par_len){
		
		networkIdentity = new String(arr,index,2);
		
		binaryCode = (arr[index+2] << 8) | arr[index+3];

	}

	/**
    * String representation of class CUGInterlockCodeItu
    *
    * @return    String provides description of class CUGInterlockCodeItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nbinaryCode = ");
				buffer.append(binaryCode);	
				buffer.append("\nnetworkIdentity = ");
				buffer.append(networkIdentity);
				return buffer.toString();
		
		}


	int					binaryCode;
	java.lang.String	networkIdentity;

	
	
}




