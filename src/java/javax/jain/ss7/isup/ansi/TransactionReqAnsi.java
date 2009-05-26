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
* MODULE NAME  : $RCSfile: TransactionReqAnsi.java,v $
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

public class TransactionReqAnsi extends java.lang.Object implements java.io.Serializable{

	public TransactionReqAnsi(){
	}
		
	public TransactionReqAnsi(long in_transId,
                          byte[] in_sccpAddr)
                   throws ParameterRangeInvalidException{
		sccpAddr = in_sccpAddr;
		transId  = in_transId;
	}
	
	public byte[] getSCCPAddress(){
		return sccpAddr;
	}

	public void setSCCPAddress(byte[] aSCCPAddress){
		sccpAddr = aSCCPAddress;
	}

	public long getTransactionId(){
		return transId;
	}

	public void setTransactionId(long aTransactionId)
                      throws ParameterRangeInvalidException{
		transId  = aTransactionId;
	}
			
	public void  putTransactionReqAnsi(byte[] arr,int index,byte par_len){
		
		int i;

		transId = (arr[index+3]<< 24) + (arr[index+2]<< 16) + (arr[index+1] << 8) + arr[index];
		
		sccpAddr = new byte[par_len - 4];
		
		for(i=4;i<par_len;i++)
          sccpAddr[i-4] = arr[index+i];
		
	}

	public byte[] flatTransactionReqAnsi(){

		byte[] rc = ByteArray.getByteArray(sccpAddr.length+4);	
		
		int i;	

		for(i=0;i<4;i++)
		    rc[i] = (byte)((transId >> (8*i)) & 0x000000FF);

		for(i=0;i<sccpAddr.length;i++)
			rc[i+4] = sccpAddr[i];
							
		return rc;
	}
	/**
    * String representation of class TransactionReqAnsi
    *
    * @return    String provides description of class TransactionReqAnsi
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				if(sccpAddr != null){
					buffer.append("\nsccpAddr = ");
					for(i=0;i<sccpAddr.length;i++)
						buffer.append(" "+Integer.toHexString((int)(sccpAddr[i] & 0xFF)));
				}		
				buffer.append("\ntransId = ");
				buffer.append(transId);
				return buffer.toString();
		
		}

	byte[] sccpAddr;
	long   transId;


}


