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
* DATE         : $Date: 2008/05/16 12:24:03 $
* 
* MODULE NAME  : $RCSfile: IsupErrorIndEvent.java,v $
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

public class IsupErrorIndEvent extends IsupEvent{

	public IsupErrorIndEvent(java.lang.Object source,
                         SignalingPointCode dpc,
                         SignalingPointCode opc,
                         byte sls,
                         int cic,
                         byte congestionPriority,
                         int eCause,
                         java.lang.String eString)
                  throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		errorCause  = eCause;
		errorString = eString;
		m_errorStringPresent = true;
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		if(m_errorStringPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();

	}
		
	public int getErrorCause(){		
		return errorCause;
		
	}

	public java.lang.String getErrorString()
                                throws MandatoryParameterNotSetException{
		if(m_errorStringPresent == true)
			return errorString;
		else throw new MandatoryParameterNotSetException();
	}

	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_ISUP_ERROR_IND;
	}

	public void setErrorCause(int eCause){
		errorCause = eCause;
	}

	public void setErrorString(java.lang.String eString){
		errorString = eString;
		m_errorStringPresent = true;
	}

	/**
    * String representation of class IsupErrorIndEvent
    *
    * @return    String provides description of class IsupErrorIndEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nerrorCause = ");
				buffer.append(errorCause);
				buffer.append("\nerrorString = ");
				buffer.append(errorString);
				return buffer.toString();
		
		}

	int              errorCause;
	java.lang.String errorString;

	public static final int ISUP_ERROR_UNKNOWN_MSGTYPE = 1; 
	public static final int ISUP_ERROR_STACK_INIT_FAILURE = 2; 
	public static final int ISUP_ERROR_INVALID_ISUP_MESSAGE = 3; 

	private boolean m_errorStringPresent = false;
}


