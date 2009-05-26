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
* MODULE NAME  : $RCSfile: MessageCompatibilityInfoItu.java,v $
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

public class MessageCompatibilityInfoItu extends java.lang.Object implements java.io.Serializable{

	public MessageCompatibilityInfoItu(){

	}

	public MessageCompatibilityInfoItu(byte[] in_messageCompInfo){
		messageCompInfo = in_messageCompInfo;
	}

	public byte[] getMessageCompatibilityInfo(){
		return messageCompInfo;
	}

	public void setMessageCompatibilityInfo(byte[] in_msgCompatibilityInfo){
		messageCompInfo = in_msgCompatibilityInfo;
	}
	
	
	public void putMessageCompatibilityInfoItu(byte[] arr, int index, byte par_len)
	{
		int i;

		messageCompInfo = new byte[par_len];
		for(i=0;i<par_len;i++)
			messageCompInfo[i] = (byte)arr[index+i];		
		
	}

	public byte[] flatMessageCompatibilityInfoItu(){
		
		byte[] rc = ByteArray.getByteArray(messageCompInfo.length);
		int i;
			
		for(i=0;i<messageCompInfo.length;i++)
			rc[i] = messageCompInfo[i];

		return rc;

	}

	/**
    * String representation of class MessageCompatibilityInfoItu
    *
    * @return    String provides description of class MessageCompatibilityInfoItu
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				if(messageCompInfo != null){
					buffer.append("\nmessageCompInfo = ");
					for(i=0;i<messageCompInfo.length;i++)
						buffer.append(" "+Integer.toHexString((int)(messageCompInfo[i] & 0xFF)));
				}	
				return buffer.toString();
		
		}

	byte[] messageCompInfo;
		
	
}




