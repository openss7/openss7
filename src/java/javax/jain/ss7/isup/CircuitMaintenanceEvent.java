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
* MODULE NAME  : $RCSfile: CircuitMaintenanceEvent.java,v $
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

public class CircuitMaintenanceEvent extends IsupEvent{

	public CircuitMaintenanceEvent(java.lang.Object source,
                               SignalingPointCode dpc,
                               SignalingPointCode opc,
                               byte sls,
                               int cic,
                               byte congestionPriority,
                               int primitive)
                        throws PrimitiveInvalidException,
                               ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		if((primitive == IsupConstants.ISUP_PRIMITIVE_BLOCKING ) ||
		   (primitive == IsupConstants.ISUP_PRIMITIVE_UNBLOCKING) ||
		   (primitive == IsupConstants.ISUP_PRIMITIVE_BLOCKING_ACK) ||
		   (primitive ==
					IsupConstants.ISUP_PRIMITIVE_UNBLOCKING_ACK)||
		   (primitive == IsupConstants.ISUP_PRIMITIVE_RESET_CIRCUIT)||
		   (primitive ==
						IsupConstants.ISUP_PRIMITIVE_CONTINUITY_CHECK_REQ) ||
		   (primitive == IsupConstants.ISUP_PRIMITIVE_OVERLOAD) ||
		   (primitive == IsupConstants.ISUP_PRIMITIVE_LOOPBACK_ACK) ||
		   (primitive ==
						IsupConstants.ISUP_PRIMITIVE_UNEQUIPPED_CIC)||
		   (primitive ==
						IsupConstants.ISUP_PRIMITIVE_CIRCUIT_RESERVATION_ACK) ||
		   (primitive ==
						IsupConstants.ISUP_PRIMITIVE_CIRCUIT_VALIDATION_TEST)){
				
				myPrimitive = primitive;
				isMyPrimitive = true;
		}
		else throw new PrimitiveInvalidException();
		
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{		

	}

	public int getIsupPrimitive(){
		if(isMyPrimitive == true)
				return myPrimitive;
		else return IsupConstants.ISUP_PRIMITIVE_UNSET;
	}
	
	public void setIsupPrimitive(int i_isupPrimitive)
                      throws PrimitiveInvalidException{
		if(( i_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_BLOCKING ) ||
		   ( i_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_UNBLOCKING) ||
		   ( i_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_BLOCKING_ACK) ||
		   ( i_isupPrimitive ==
					IsupConstants.ISUP_PRIMITIVE_UNBLOCKING_ACK)||
		   ( i_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_RESET_CIRCUIT)||
		   ( i_isupPrimitive ==
						IsupConstants.ISUP_PRIMITIVE_CONTINUITY_CHECK_REQ) ||
		   ( i_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_OVERLOAD) ||
		   ( i_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_LOOPBACK_ACK) ||
		   ( i_isupPrimitive ==
						IsupConstants.ISUP_PRIMITIVE_UNEQUIPPED_CIC)||
		   ( i_isupPrimitive ==
						IsupConstants.ISUP_PRIMITIVE_CIRCUIT_RESERVATION_ACK) ||
		   ( i_isupPrimitive ==
						IsupConstants.ISUP_PRIMITIVE_CIRCUIT_VALIDATION_TEST)){
				myPrimitive = i_isupPrimitive;
				isMyPrimitive = true;
		}
		else throw new PrimitiveInvalidException();
		
	}
	
	/**
    * String representation of class CircuitMaintenanceEvent
    *
    * @return    String provides description of class CircuitMaintenanceEvent
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
	int myPrimitive;
	
	
}


