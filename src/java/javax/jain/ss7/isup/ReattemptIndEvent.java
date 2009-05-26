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
* MODULE NAME  : $RCSfile: ReattemptIndEvent.java,v $
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

public class ReattemptIndEvent extends IsupEvent{

public ReattemptIndEvent(java.lang.Object source,
                         SignalingPointCode dpc,
                         SignalingPointCode opc,
                         byte sls,
                         int cic,
                         byte congestionPriority,
                         int callReattemptReason)
                  throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);		
		reattemptReason = callReattemptReason;
	}

	public int getCallReattemptReason(){
		return reattemptReason;
	}
	
	public int getIsupPrimitive(){
		 return IsupConstants.ISUP_PRIMITIVE_REATTEMPT_IND;
	}			
	
	public void setCallReattemptReason(int callReattemptReason){
		reattemptReason = callReattemptReason;
	}
	
	/**
    * String representation of class ReattemptIndEvent
    *
    * @return    String provides description of class ReattemptIndEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nreattemptReason = ");
				buffer.append(reattemptReason);
				return buffer.toString();
		
		}
	
	
	
	int reattemptReason;

	public static final int ISUP_RR_DUAL_SEIZURE = 1; 
	public static final int ISUP_RR_BAD_RESPONSE_FOR_IAM = 2; 


}


