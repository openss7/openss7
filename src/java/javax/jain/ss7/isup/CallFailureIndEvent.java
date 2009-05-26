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
* DATE         : $Date: 2008/05/16 12:23:51 $
* 
* MODULE NAME  : $RCSfile: CallFailureIndEvent.java,v $
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

public class CallFailureIndEvent extends IsupEvent{

	public CallFailureIndEvent(java.lang.Object source,
                           SignalingPointCode dpc,
                           SignalingPointCode opc,
                           byte sls,
                           int cic,
                           byte congestionPriority,
                           int callFailureReason)
                    throws ParameterRangeInvalidException{
		super(source,dpc,opc,sls,cic,congestionPriority);
		callFailReason = callFailureReason;
	}

	public int getCallFailureReason(){
		return callFailReason;
	}

	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_CALL_FAILURE_IND;
	}

	public void setCallFailureReason(int callFailureReason){
		callFailReason = callFailureReason;
	}

	/**
    * String representation of class CallFailureEvent
    *
    * @return    String provides description of class CallFailureEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncallFailReason = ");
				buffer.append(callFailReason);				
				return buffer.toString();
		
		}


	int callFailReason;
    
	public static final int ISUP_CFR_ADDRESS_COMPLETE_TIMEOUT = 7;/* T7 timer*/
	public static final int ISUP_CFR_ANSWER_TIMEOUT = 9;/* T9 timer*/
	public static final int ISUP_CFR_INFORMATION_TIMEOUT = 33; /* T33 timer*/
	public static final int ISUP_CFR_CIRCUIT_RESERVATION_TIMEOUT = 43;/* TCRM timer*/
	public static final int ISUP_CFR_CIRCUIT_RESERVATION_ACK_TIMEOUT = 42;/* TCRA timer*/
	public static final int ISUP_CFR_RESUME_TIMEOUT = 2;/* T2 timer */
	public static final int ISUP_CFR_CONTINUITY_CHECK_RESPONSE_TIMEOUT = 8;/* T8 timer */
	public static final int ISUP_CFR_CHARGE_INFO_TIMEOUT = 0;/* no timer associated with it */

}


