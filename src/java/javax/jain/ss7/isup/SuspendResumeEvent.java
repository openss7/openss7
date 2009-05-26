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
* DATE         : $Date: 2008/05/16 12:24:13 $
* 
* MODULE NAME  : $RCSfile: SuspendResumeEvent.java,v $
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

public class SuspendResumeEvent extends IsupEvent{

	public SuspendResumeEvent(java.lang.Object source,
                          SignalingPointCode dpc,
                          SignalingPointCode opc,
                          byte sls,
                          int cic,
                          byte congestionPriority,
                          int primitive,
                          byte suspendResumeInd)
                   throws PrimitiveInvalidException,
                          ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		if((primitive == IsupConstants.ISUP_PRIMITIVE_SUSPEND) ||
		   (primitive == IsupConstants.ISUP_PRIMITIVE_RESUME))
			myPrimitive = primitive;
		else throw new PrimitiveInvalidException();	
		if((suspendResumeInd == 0) || (suspendResumeInd == 1)){		
				susResInd = suspendResumeInd;
				m_susResIndPresent = true;
		}
		else throw new ParameterRangeInvalidException();
	}
		
	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		if(m_susResIndPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();

	}

	public CallReference getCallReference()
                               throws ParameterNotSetException{
		if(isCallReference == true)
				return callReference;
		else throw new ParameterNotSetException();
	}

	public boolean isCallReferencePresent(){
		return isCallReference;
	}

	public void setCallReference(CallReference callRef){
		callReference = callRef;
		isCallReference = true;
	}

	public int getIsupPrimitive(){
		if((myPrimitive == IsupConstants.ISUP_PRIMITIVE_SUSPEND ) ||
			(myPrimitive == IsupConstants.ISUP_PRIMITIVE_RESUME ))
				return myPrimitive;
		else return IsupConstants.ISUP_PRIMITIVE_UNSET;
	}

	public void setIsupPrimitive(int i_isupPrimitive)
                      throws PrimitiveInvalidException{
		if((i_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_SUSPEND )
||
			(i_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_RESUME )){
				myPrimitive = i_isupPrimitive;
				isMyPrimitive = true;
		}
		else throw new PrimitiveInvalidException();
	}

	public byte getSuspendResumeInd(){
		return susResInd;
	}

	public void setSuspendResumeInd(byte suspendResumeInd)
                         throws ParameterRangeInvalidException{
		if((suspendResumeInd == 0) || (suspendResumeInd == 1)){
			susResInd = suspendResumeInd;
			m_susResIndPresent = true;
		}
		else throw new ParameterRangeInvalidException();
	}

	/**
    * String representation of class SuspendResumeEvent
    *
    * @return    String provides description of class SuspendResumeEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisCallReference = ");
				buffer.append(isCallReference);
				buffer.append("\ncallReference = ");
				buffer.append(callReference);
				buffer.append("\n\nisMyPrimitive = ");
				buffer.append(isMyPrimitive);
				buffer.append("\nmyPrimitive = ");
				buffer.append(myPrimitive);
				buffer.append("\n\nsusResInd = ");
				buffer.append(susResInd);
				return buffer.toString();
		
		}			

	CallReference callReference;
	boolean       isCallReference;
	boolean       isMyPrimitive;
	int           myPrimitive;
	byte          susResInd;

	private boolean m_susResIndPresent = false;

}


