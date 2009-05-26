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
* DATE         : $Date: 2008/05/16 12:23:59 $
* 
* MODULE NAME  : $RCSfile: UserPartTestInfoItuEvent.java,v $
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

public class UserPartTestInfoItuEvent extends IsupEvent{

	public UserPartTestInfoItuEvent(java.lang.Object source,
                                SignalingPointCode dpc,
                                SignalingPointCode opc,
                                byte sls,
                                int cic,
                                byte congestionPriority,
                                int primitive)
                         throws PrimitiveInvalidException,
                                ParameterRangeInvalidException{

	
		super(source,dpc,opc,sls,cic,congestionPriority);
		myPrimitive = primitive;
		m_myPrimitivePresent = true;

	}
		
	 public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
                
		if(m_myPrimitivePresent == true)
			return;
		else throw new MandatoryParameterNotSetException();             

     }

	 public int getIsupPrimitive(){
                if (isMyPrimitive == true)
                                return myPrimitive; 
                else
                        return IsupConstants.ISUP_PRIMITIVE_UNSET;
     }

	 public void setIsupPrimitive(int i_isupPrimitive)
                      throws PrimitiveInvalidException{

                if((i_isupPrimitive ==
                                IsupConstants.ISUP_PRIMITIVE_USER_PART_TEST ) ||
                        (i_isupPrimitive ==
                                IsupConstants.ISUP_PRIMITIVE_USER_PART_AVAILABLE)){

                                myPrimitive = i_isupPrimitive;
                                isMyPrimitive = true;
                }

                else throw new PrimitiveInvalidException();
	}	
		
	public ParamCompatibilityInfoItu getParamCompatibilityInfo()
                                                    throws ParameterNotSetException{
		if(isParamCompInfoItu == true)
			return paramCompInfoItu;
		else throw new ParameterNotSetException();

	}

	public void setParamCompatibilityInfo(ParamCompatibilityInfoItu in_paramCompInfo){
		paramCompInfoItu   = in_paramCompInfo;
		isParamCompInfoItu = true;
	}

	public boolean isParamCompatibilityInfoPresent(){
		return isParamCompInfoItu;
	}

	/**
    * String representation of class UserPartTestInfoItuEvent
    *
    * @return    String provides description of class UserPartTestInfoItuEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisMyPrimitive = ");
				buffer.append(isMyPrimitive);
				buffer.append("\nmyPrimitive = ");
				buffer.append(myPrimitive);
				buffer.append("\n\nisParamCompInfoItu = ");
				buffer.append(isParamCompInfoItu);
				buffer.append("\n paramCompInfoItu = ");
				buffer.append(paramCompInfoItu);		
				return buffer.toString();
		
		}


	boolean                   isMyPrimitive;
	boolean                   isParamCompInfoItu;
	int                       myPrimitive;
	ParamCompatibilityInfoItu paramCompInfoItu;

	private boolean m_myPrimitivePresent = false;
    
}


