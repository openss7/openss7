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
* DATE         : $Date: 2008/05/16 12:24:05 $
* 
* MODULE NAME  : $RCSfile: NwStatusIndEvent.java,v $
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

public class NwStatusIndEvent extends IsupEvent{

	public NwStatusIndEvent(java.lang.Object source,
                        SignalingPointCode dpc,
                        SignalingPointCode opc,
                        byte sls,
                        int cic,
                        byte congestionPriority,
                        int primitive,
                        SignalingPointCode spc)
                 throws PrimitiveInvalidException,
                        ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		if((primitive == IsupConstants.ISUP_PRIMITIVE_MTP_PAUSE) ||
			(primitive == IsupConstants.ISUP_PRIMITIVE_MTP_RESUME) ||
			(primitive == IsupConstants.ISUP_PRIMITIVE_MTP_STATUS_CONGESTION )||
			(primitive == IsupConstants.ISUP_PRIMITIVE_MTP_STATUS_USER_PART )){
				myPrimitive = primitive;
				isMyPrimitive = true;
				affectedPC = spc;
		}
				
		else throw new PrimitiveInvalidException();		
		
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		
	}
		
	public int getCongLevel()
                 throws ParameterNotSetException{
		if(isCongLevel == true)
				return congLevel;
		else throw new ParameterNotSetException();
	}

	public boolean isCongLevelPresent(){
		return isCongLevel;
	}

	public void setCongLevel(int congestionLevel){
		congLevel = congestionLevel;
		isCongLevel = true;
	}

	public int getIsupPrimitive(){
		if(isMyPrimitive == true)
				return myPrimitive;  
		else return IsupConstants.ISUP_PRIMITIVE_UNSET;
	}	

	public SignalingPointCode getSignalingPointCode(){
		return affectedPC;
	}

	public void setSignalingPointCode(SignalingPointCode spc)
                           throws ParameterRangeInvalidException{
		affectedPC = spc;
	}

	public int getUserPartStatus()
                      throws ParameterNotSetException{
		if(isUpStatus == true)
				return upStatus;
		else throw new ParameterNotSetException();
	}

	public boolean isUserPartStatusPresent(){
		return isUpStatus;
	}

	public void setUserPartStatus(int userPartStatus){
		upStatus = userPartStatus;
		isUpStatus = true;
	}
	
	public void setIsupPrimitive(int i_isupPrimitive)
                      throws PrimitiveInvalidException{
		if((i_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_MTP_PAUSE) ||
		   (i_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_MTP_RESUME) ||
		   (i_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_MTP_STATUS_CONGESTION )||
		   (i_isupPrimitive ==
						IsupConstants.ISUP_PRIMITIVE_MTP_STATUS_USER_PART )){
				myPrimitive = i_isupPrimitive;
				isMyPrimitive = true;
		}
		else throw new PrimitiveInvalidException();
	}
	
	
	/**
    * String representation of class NwStatusIndEvent
    *
    * @return    String provides description of class NwStatusIndEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\naffectedPC = ");
				buffer.append(affectedPC);
				buffer.append("\n\nisCongLevel = ");
				buffer.append(isCongLevel);
				buffer.append("\ncongLevel = ");
				buffer.append(congLevel);
				buffer.append("\n\nisMyPrimitive = ");
				buffer.append(isMyPrimitive);
				buffer.append("\nmyPrimitive = ");
				buffer.append(myPrimitive);
				buffer.append("\n\nisUpStatus = ");
				buffer.append(isUpStatus);
				buffer.append("\nupStatus = ");
				buffer.append(upStatus);
				return buffer.toString();
		
		}
	
	SignalingPointCode affectedPC;
	int                congLevel;
	boolean            isCongLevel;
	boolean            isMyPrimitive;
	boolean            isUpStatus;
	int                myPrimitive;
	int                upStatus;

	public static final int ISUP_UP_USER_UNEQUIPPED = 0x01; 
	public static final int ISUP_UP_USER_INACCESSIBLE = 0x02; 
	public static final int ISUP_UP_UNKNOWN = 0x00; 

}


