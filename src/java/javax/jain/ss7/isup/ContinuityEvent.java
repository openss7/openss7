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
* DATE         : $Date: 2008/05/16 12:24:01 $
* 
* MODULE NAME  : $RCSfile: ContinuityEvent.java,v $
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

public class ContinuityEvent extends IsupEvent{

	public ContinuityEvent(java.lang.Object source,
                       SignalingPointCode dpc,
                       SignalingPointCode opc,
                       byte sls,
                       int cic,
                       byte congestionPriority,
                       byte in_continuityInd)
                throws ParameterRangeInvalidException{
		super(source,dpc,opc,sls,cic,congestionPriority);

		if((in_continuityInd == CI_CONTINUITY_CHECK_FAILED) ||
		   (in_continuityInd == CI_CONTINUITY_CHECK_SUCCESSFUL))
				continuityInd = in_continuityInd;
		else throw new  ParameterRangeInvalidException();
	}

	public byte getContinuityInd(){
		return continuityInd;
	}
	
	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_CONTINUITY;
	}

	public void setContinuityInd(byte in_continuityInd)
                      throws ParameterRangeInvalidException{
		if((in_continuityInd == CI_CONTINUITY_CHECK_FAILED) ||
		   (in_continuityInd == CI_CONTINUITY_CHECK_SUCCESSFUL))
				continuityInd = in_continuityInd;
		else throw new  ParameterRangeInvalidException();
	}
		
	/**
    * String representation of class ContinuityEvent
    *
    * @return    String provides description of class ContinuityEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncontinuityInd = ");
				buffer.append(continuityInd);
				return buffer.toString();
		
		}

	
	byte continuityInd;
	public static final byte CI_CONTINUITY_CHECK_FAILED = 0x00; 
	public static final byte CI_CONTINUITY_CHECK_SUCCESSFUL = 0x01; 
}


