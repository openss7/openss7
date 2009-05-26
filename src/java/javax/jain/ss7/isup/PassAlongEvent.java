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
* DATE         : $Date: 2008/05/16 12:24:05 $
* 
* MODULE NAME  : $RCSfile: PassAlongEvent.java,v $
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


package javax.jain.ss7.isup;

import javax.jain.*;
import javax.jain.ss7.*;

public class PassAlongEvent extends IsupEvent{

	public PassAlongEvent(java.lang.Object source,
                      SignalingPointCode dpc,
                      SignalingPointCode opc,
                      byte sls,
                      int cic,
                      byte congestionPriority)
               throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);		
		
	}

	public byte[] getEmbeddedMessage()
				           throws ParameterNotSetException{
		if(isEmbeddedMsg == true)
				return embeddedMsg;
		else throw new ParameterNotSetException();

	}

	public boolean isEmbeddedMessagePresent(){
		return isEmbeddedMsg;
	}

	public void setEmbeddedMessage(byte[] embeddedMessage){
		embeddedMsg = embeddedMessage;
		isEmbeddedMsg = true;
	}


	public int getIsupPrimitive(){
		 return IsupConstants.ISUP_PRIMITIVE_PASS_ALONG;
	}			
	
	/**
    * String representation of class PassAlongEvent
    *
    * @return    String provides description of class PassAlongEvent
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisEmbeddedMsg = ");
				buffer.append(isEmbeddedMsg);
				if(isEmbeddedMsg == true){
					buffer.append("\nembeddedMsg = ");
					for(i=0;i<embeddedMsg.length;i++)
						buffer.append(" "+Integer.toHexString((int)(embeddedMsg[i] & 0xFF)));
				}
				return buffer.toString();
		
		}
	
	byte[]  embeddedMsg;
    boolean isEmbeddedMsg;


}


