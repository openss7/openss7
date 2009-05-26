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
* DATE         : $Date: 2008/05/16 12:23:53 $
* 
* MODULE NAME  : $RCSfile: CircuitGrpQueryEvent.java,v $
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

public class CircuitGrpQueryEvent extends CircuitGrpCommonEvent{

	public CircuitGrpQueryEvent(java.lang.Object source,
                            SignalingPointCode dpc,
                            SignalingPointCode opc,
                            byte sls,
                            int cic,
                            byte congestionPriority,
                            int primitive,
                            RangeAndStatus in_rangeAndStatus)
                     throws PrimitiveInvalidException,
                            ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority,in_rangeAndStatus);
		if((primitive ==
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY) ||
			(primitive ==
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY)){
						myPrimitive = primitive;
						isMyPrimitive = true;
		}
		else throw new PrimitiveInvalidException();
		/* Check range for ITU and ANSI */
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		super.checkMandatoryParameters();
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
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY) ||
			(i_isupPrimitive ==
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY)){
				
				myPrimitive = i_isupPrimitive;
				isMyPrimitive = true;
		}

		else throw new PrimitiveInvalidException();
				
	}
	
	/**
    * String representation of class CircuitGrpQueryEvent
    *
    * @return    String provides description of class CircuitGrpQueryEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisMyPrimitive = ");
				buffer.append(isMyPrimitive);
				buffer.append("\nmyPrimitive = ");
				buffer.append(myPrimitive);
				return buffer.toString();
		
		}

	boolean isMyPrimitive;
	int     myPrimitive;

	
}


