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
* DATE         : $Date: 2008/05/16 12:23:52 $
* 
* MODULE NAME  : $RCSfile: CircuitGrpCommonEvent.java,v $
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

public abstract class CircuitGrpCommonEvent extends IsupEvent{

	protected CircuitGrpCommonEvent(java.lang.Object source,
                                SignalingPointCode dpc,
                                SignalingPointCode opc,
                                byte sls,
                                int cic,
                                byte congestionPriority,
                                RangeAndStatus in_rangeAndStatus)
                         throws ParameterRangeInvalidException{
		super(source,dpc,opc,sls,cic,congestionPriority);
		rangeAndStatus          = in_rangeAndStatus;
		m_rangeAndStatusPresent = true;
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		if(m_rangeAndStatusPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();
	}

	public RangeAndStatus getRangeAndStatus()
                                 throws MandatoryParameterNotSetException{

		if(m_rangeAndStatusPresent == true)
			return rangeAndStatus;
		else throw new MandatoryParameterNotSetException();
		
	}

	public void setRangeAndStatus(RangeAndStatus in_rangeAndStatus){
		rangeAndStatus          = in_rangeAndStatus;
		m_rangeAndStatusPresent = true;
	}
	
	/**
    * String representation of class CircuitGrpCommonEvent
    *
    * @return    String provides description of class CircuitGrpCommonEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nrangeAndStatus = ");
				buffer.append(rangeAndStatus);
				return buffer.toString();
		
		}

	
	RangeAndStatus rangeAndStatus;

	protected boolean m_rangeAndStatusPresent     = false;

}


