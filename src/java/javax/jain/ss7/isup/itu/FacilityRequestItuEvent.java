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
* DATE         : $Date: 2008/05/16 12:23:55 $
* 
* MODULE NAME  : $RCSfile: FacilityRequestItuEvent.java,v $
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


package javax.jain.ss7.isup.itu;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class FacilityRequestItuEvent extends IsupEvent{

	public FacilityRequestItuEvent(java.lang.Object source,
                              SignalingPointCode dpc,
                              SignalingPointCode opc,
                              byte sls,
                              int cic,
                              byte congestionPriority,
                              short facilityInd)
                       throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		facInd   = facilityInd;
	}


	public int getIsupPrimitive(){

		return IsupConstants.ISUP_PRIMITIVE_FACILITY_REQUEST; 

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
	
	public ConReq getConReq()
                 throws ParameterNotSetException{
		if(isConReq == true)
			return conReq;
		else throw new ParameterNotSetException();
	}
	
	public void setConReq(ConReq connectionReq){
		conReq   = connectionReq;
		isConReq = true;
	}

	public boolean isConReqPresent(){
		return isConReq;
	}

	public short getFacilityInd(){
		return facInd;
	}

	public void setFacilityInd(short in_facInd)
                    throws ParameterRangeInvalidException{
		if((in_facInd >=0) && (in_facInd<=255))
			facInd = in_facInd;		
		else throw new ParameterRangeInvalidException();
	}
	
	public ParamCompatibilityInfoItu getParamCompatibilityInfo()
                                                    throws ParameterNotSetException{
		if(isParamCompatibilityInfoItu == true)
			return paramCompatibilityInfoItu;
		else throw new ParameterNotSetException();
	}

	public void setParamCompatibilityInfo(ParamCompatibilityInfoItu pci){
		paramCompatibilityInfoItu   = pci;
		isParamCompatibilityInfoItu = true;
	}

	public boolean isParamCompatibilityInfoPresent(){
		return isParamCompatibilityInfoItu;
	}
	
	public UserToUserIndicatorsItu getUserToUserIndicators()
                                                throws ParameterNotSetException{
		if(isUserToUserIndicatorsItu == true)
			return userToUserIndicatorsItu;
		else throw new ParameterNotSetException();
	}
		
	public void setUserToUserIndicators(UserToUserIndicatorsItu uui){

		userToUserIndicatorsItu   = uui; 
		isUserToUserIndicatorsItu = true;
	}

	public boolean isUserToUserIndicatorsPresent(){
		return  isUserToUserIndicatorsItu;
	}

	/**
    * String representation of class FacilityRequestItuEvent
    *
    * @return    String provides description of class FacilityRequestItuEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisCallReference = ");
				buffer.append(isCallReference);
				buffer.append("\ncallReference = ");
				buffer.append(callReference);
				buffer.append("\n\nisConReq = ");
				buffer.append(isConReq);
				buffer.append("\nconReq  = ");
				buffer.append(conReq);	
				buffer.append("\n\nfacInd  = ");
				buffer.append(facInd);			
				buffer.append("\n\nisParamCompatibilityInfoItu = ");
				buffer.append(isParamCompatibilityInfoItu);
				buffer.append("\nparamCompatibilityInfoItu = ");
				buffer.append(paramCompatibilityInfoItu);
				buffer.append("\n\nisUserToUserIndicatorsItu = ");
				buffer.append(isUserToUserIndicatorsItu);
				buffer.append("\nuserToUserIndicatorsItu  = ");
				buffer.append(userToUserIndicatorsItu);	
				return buffer.toString();
		
		}

	


	CallReference             callReference;
	ConReq                    conReq;
	short                     facInd;
	boolean                   isCallReference;
	boolean                   isConReq;
	boolean                   isParamCompatibilityInfoItu;
	boolean                   isUserToUserIndicatorsItu;	
	ParamCompatibilityInfoItu paramCompatibilityInfoItu;
	UserToUserIndicatorsItu   userToUserIndicatorsItu;

    
}


