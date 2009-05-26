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
* MODULE NAME  : $RCSfile: FacilityEvent.java,v $
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

public class FacilityEvent extends IsupEvent{

	public FacilityEvent(java.lang.Object source,
                     SignalingPointCode dpc,
                     SignalingPointCode opc,
                     byte sls,
                     int cic,
                     byte congestionPriority)
              throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
	}

	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_FACILITY;
	}

	public RemoteOperations getRemoteOperations()
                                     throws ParameterNotSetException{
		if(isRemoteOperations == true)
				return remoteOperations;
		else throw new ParameterNotSetException();
	}

	public byte[] getServiceActivation()
                            throws ParameterNotSetException{
		if(isServiceActivation == true)
				return serviceActivation;
		else throw new ParameterNotSetException();
	}

	public boolean isRemoteOperationsPresent(){
		return isRemoteOperations;
	}

	public boolean isServiceActivationPresent(){
		return isServiceActivation;
	}

	public void setRemoteOperations(RemoteOperations remoteOp){
		remoteOperations = remoteOp;
		isRemoteOperations = true;
	}

	public void setServiceActivation(byte[] servAct){
		serviceActivation = servAct;
		isServiceActivation = true;
	}
		
	/**
    * String representation of class FacilityEvent
    *
    * @return    String provides description of class FacilityEvent
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\n isRemoteOperations= ");
				buffer.append(isRemoteOperations);
				buffer.append("\nremoteOperations = ");
				buffer.append(remoteOperations);
				buffer.append("\n\nisServiceActivation = ");
				buffer.append(isServiceActivation);
				if(isServiceActivation == true){
					buffer.append("\nserviceActivation = ");
					for(i=0;i<serviceActivation.length;i++)
						buffer.append(" "+Integer.toHexString((int)(serviceActivation[i] & 0xFF)));
				}
				return buffer.toString();
		
		}

	
	boolean isRemoteOperations;
	boolean isServiceActivation;
	RemoteOperations remoteOperations;
	byte[] serviceActivation;
}


