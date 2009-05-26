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
* DATE         : $Date: 2008/05/16 12:23:52 $
* 
* MODULE NAME  : $RCSfile: CircuitGrpMaintenanceEvent.java,v $
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

public class CircuitGrpMaintenanceEvent extends CircuitGrpCommonEvent{

	public CircuitGrpMaintenanceEvent(java.lang.Object source,
                                  SignalingPointCode dpc,
                                  SignalingPointCode opc,
                                  byte sls,
                                  int cic,
                                  byte congestionPriority,
                                  int primitive,
                                  RangeAndStatus in_rangeAndStatus,
                                  byte in_circuitGrpSupervisionMsgTypeInd)
                           throws PrimitiveInvalidException,
                                  ParameterRangeInvalidException{
		super(source,dpc,opc,sls,cic,congestionPriority,in_rangeAndStatus);
		if((primitive ==
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING) ||
			(primitive ==
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING) ||
			(primitive ==
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING_ACK) ||
			(primitive ==
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING_ACK)){
				myPrimitive = primitive; 
				isMyPrimitive = true;
		}
		else throw new PrimitiveInvalidException();
		/* Check range for ITU and ANSI */

		if((	in_circuitGrpSupervisionMsgTypeInd == CGSMTI_MAINTENANCE_ORIENTED) ||
			(in_circuitGrpSupervisionMsgTypeInd == CGSMTI_HARDWARE_FAILURE_ORIENTED)){
			
			cktGrpSupervisionMsgTypeInd          = in_circuitGrpSupervisionMsgTypeInd;
			m_cktGrpSupervisionMsgTypeIndPresent = true;
		}	

		else
			throw new ParameterRangeInvalidException();	
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		super.checkMandatoryParameters();
		if(m_cktGrpSupervisionMsgTypeIndPresent == true)
			return;
		else throw new  MandatoryParameterNotSetException();

	}

	public byte getCircuitGrpSupervisionMsgTypeInd() throws MandatoryParameterNotSetException{
		if(m_cktGrpSupervisionMsgTypeIndPresent == true)
			return cktGrpSupervisionMsgTypeInd;
		else throw new  MandatoryParameterNotSetException();
	}

	public int getIsupPrimitive(){
		if(isMyPrimitive == false)
				return IsupConstants.ISUP_PRIMITIVE_UNSET;
		else
				return myPrimitive;
	}

	public void setCircuitGrpSupervisionMsgTypeInd(byte aCircuitGrpSupervisionMsgTypeInd)
                                throws ParameterRangeInvalidException{
			if((	aCircuitGrpSupervisionMsgTypeInd == CGSMTI_MAINTENANCE_ORIENTED) ||
				(aCircuitGrpSupervisionMsgTypeInd == CGSMTI_HARDWARE_FAILURE_ORIENTED)){
				
				cktGrpSupervisionMsgTypeInd = aCircuitGrpSupervisionMsgTypeInd;
				m_cktGrpSupervisionMsgTypeIndPresent = true;	
			}	

			else
				throw new ParameterRangeInvalidException();	

	}

	public void setIsupPrimitive(int i_isupPrimitive)
                      throws PrimitiveInvalidException{
		if((i_isupPrimitive ==
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING ) ||
			(i_isupPrimitive ==
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING  ) ||
			(i_isupPrimitive ==
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING_ACK ) ||
			(i_isupPrimitive ==
				IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING_ACK )){
				myPrimitive = 	i_isupPrimitive;
				isMyPrimitive = true;
		}
		else throw new PrimitiveInvalidException();
				
	}

	/**
    * String representation of class CircuitGrpMaintenanceEvent
    *
    * @return    String provides description of class CircuitGrpMaintenanceEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncktGrpSupervisionMsgTypeInd = ");
				buffer.append(cktGrpSupervisionMsgTypeInd);
				buffer.append("\n\nisMyPrimitive = ");
				buffer.append(isMyPrimitive);
				buffer.append("\nmyPrimitive = ");
				buffer.append(myPrimitive);
				return buffer.toString();
		
		}

	
	byte    cktGrpSupervisionMsgTypeInd;
	boolean isMyPrimitive;
	int     myPrimitive;
		
	protected boolean m_cktGrpSupervisionMsgTypeIndPresent     = false;
	public static final byte CGSMTI_MAINTENANCE_ORIENTED = 0x00; 
	public static final byte CGSMTI_HARDWARE_FAILURE_ORIENTED = 0x01; 
}


