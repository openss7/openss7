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
* DATE         : $Date: 2008/05/16 12:24:11 $
* 
* MODULE NAME  : $RCSfile: ReleaseAnsiEvent.java,v $
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


package javax.jain.ss7.isup.ansi;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class ReleaseAnsiEvent extends ReleaseEvent{

	public ReleaseAnsiEvent(java.lang.Object source,
                        SignalingPointCode dpc,
                        SignalingPointCode opc,
                        byte sls,
                        int cic,
                        byte congestionPriority,
                        CauseInd cai)
                 throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority,cai);		
	}

	public CallReference getCallReference()
                               throws ParameterNotSetException{
		if(isCallReference == true)
			return callReference;
		else throw new ParameterNotSetException();
	}

	public void setCallReference(CallReference callRef){
		callReference   = callRef;
		isCallReference = true;
	}

	public boolean isCallReferencePresent(){
		return isCallReference;
	}

	public ChargeNumberAnsi getChargeNumber()
                                 throws ParameterNotSetException{
		if(isChargeNumberAnsi == true)
			return chargeNumberAnsi;
		else throw new ParameterNotSetException();
	}

	public void setChargeNumber(ChargeNumberAnsi chargeNumAnsi){

		chargeNumberAnsi   = chargeNumAnsi;
		isChargeNumberAnsi = true;
	}

	public boolean isChargeNumberPresent(){
		return isChargeNumberAnsi;
	}

	public GenericNumber getGenericNumber()
                               throws ParameterNotSetException{
		if(isGenericNumber == true)
			return genericNumber;
		else throw new ParameterNotSetException();
	}

	public void setGenericNumber(GenericNumber genericNum){
		genericNumber   = genericNum;
		isGenericNumber = true;
	}

	public boolean isGenericNumberPresent(){
		return isGenericNumber;
	}

	public byte[] getServiceActivation()
                            throws ParameterNotSetException{
		if(isServiceActivation == true)
			return serviceActivation;
		else throw new ParameterNotSetException();
	}
	
	public void setServiceActivation(byte[] servAct){
		serviceActivation   = servAct;
		isServiceActivation = true;
	}

	public boolean isServiceActivationPresent(){
		return isServiceActivation;
	}
	

	/**
    * String representation of class ReleaseAnsiEvent
    *
    * @return    String provides description of class ReleaseAnsiEvent
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisCallReference = ");
				buffer.append(isCallReference);
				buffer.append("\ncallReference = ");
				buffer.append(callReference);
				buffer.append("\n\nisChargeNumberAnsi = ");
				buffer.append(isChargeNumberAnsi);
				buffer.append("\nchargeNumberAnsi = ");
				buffer.append(chargeNumberAnsi);
				buffer.append("\n\nisGenericNumber = ");
				buffer.append(isGenericNumber);
				buffer.append("\ngenericNumber = ");
				buffer.append(genericNumber);
				buffer.append("\n\nisServiceActivation = ");
				buffer.append(isServiceActivation);
				if(isServiceActivation == true){
					buffer.append("\nserviceActivation = ");
					for(i=0;i<serviceActivation.length;i++)
						buffer.append(" "+Integer.toHexString((int)(serviceActivation[i] & 0xFF)));
				}
				return buffer.toString();
		
		}


	CallReference    callReference;
	ChargeNumberAnsi chargeNumberAnsi;
	GenericNumber    genericNumber;
	boolean          isCallReference;
	boolean          isChargeNumberAnsi;
	boolean          isGenericNumber;
	boolean          isServiceActivation;
	byte[]           serviceActivation;

	
	
}	


