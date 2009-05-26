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
* DATE         : $Date: 2008/05/16 12:24:00 $
* 
* MODULE NAME  : $RCSfile: ConfusionEvent.java,v $
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

public class ConfusionEvent extends IsupEvent{

	public ConfusionEvent(java.lang.Object source,
                      SignalingPointCode dpc,
                      SignalingPointCode opc,
                      byte sls,
                      int cic,
                      byte congestionPriority,
                      CauseInd causeIndicator)
               throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		causeInd          = causeIndicator;
		m_causeIndPresent = true;
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		if(m_causeIndPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();

	}
	
	public CauseInd getCauseInd()
                     throws MandatoryParameterNotSetException{
		if(m_causeIndPresent == true)
			return causeInd;
		else throw new MandatoryParameterNotSetException();
	}

	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_CONFUSION;
	}

	public void setCauseInd(CauseInd causeIndicator){
		causeInd          = causeIndicator; 
		m_causeIndPresent = true; 
	}
		
	/**
    * String representation of class ConfusionEvent
    *
    * @return    String provides description of class ConfusionEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncauseInd = ");
				buffer.append(causeInd);
				return buffer.toString();
		
		}

	
	CauseInd causeInd;

	protected boolean m_causeIndPresent     = false;
	
}


