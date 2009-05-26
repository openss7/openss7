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
* DATE         : $Date: 2008/05/16 12:24:06 $
* 
* MODULE NAME  : $RCSfile: RawMessageEvent.java,v $
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

public class RawMessageEvent extends IsupEvent{

	public RawMessageEvent(java.lang.Object source,
                       SignalingPointCode dpc,
                       SignalingPointCode opc,
                       byte sls,
                       int cic,
                       byte congestionPriority,
                       int msgType)
                throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);		
		isupMessageType = msgType;  
	}

	public int getIsupMessageType(){
		return isupMessageType;
	}
	
	public int getIsupPrimitive(){
		 return IsupConstants.ISUP_PRIMITIVE_RAW_MESSAGE;
	}			
	
	public void setIsupMessageType(int msgType)
                        throws ParameterRangeInvalidException{
		isupMessageType = msgType;
	}
	
	/**
    * String representation of class RawMessageEvent
    *
    * @return    String provides description of class RawMessageEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nisupMessageType = ");
				buffer.append(isupMessageType);
				return buffer.toString();
		
		}
	
	
	int isupMessageType;


}


