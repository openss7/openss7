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
* MODULE NAME  : $RCSfile: CircuitGrpResetAckEvent.java,v $
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

public class CircuitGrpResetAckEvent extends CircuitGrpCommonEvent{

	public CircuitGrpResetAckEvent(java.lang.Object source,
                               SignalingPointCode dpc,
                               SignalingPointCode opc,
                               byte sls,
                               int cic,
                               byte congestionPriority,
                               RangeAndStatus in_rangeAndStatus)
                        throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority,in_rangeAndStatus);
		/* Range Check must be done for ITU or ANSI */
	}

	public int getIsupPrimitive(){

		return IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_RESET_ACK;
	}	
	
	/**
    * String representation of class CircuitGrpResetAckEvent
    *
    * @return    String provides description of class CircuitGrpResetAckEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				return buffer.toString();
		
		}
	
}


