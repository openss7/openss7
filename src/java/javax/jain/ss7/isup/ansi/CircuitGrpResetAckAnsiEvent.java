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
* DATE         : $Date: 2008/05/16 12:24:09 $
* 
* MODULE NAME  : $RCSfile: CircuitGrpResetAckAnsiEvent.java,v $
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

public class CircuitGrpResetAckAnsiEvent extends CircuitGrpResetAckEvent{


	public CircuitGrpResetAckAnsiEvent(java.lang.Object source,
                                   SignalingPointCode dpc,
                                   SignalingPointCode opc,
                                   byte sls,
                                   int cic,
                                   byte congestionPriority,
                                   RangeAndStatus in_rangeAndStatus)
                            throws ParameterRangeInvalidException{
		super(source,dpc,opc,sls,cic,congestionPriority,in_rangeAndStatus);
	
	}

	public CircuitAssignMapAnsi getCircuitAssignMap()
                                         throws ParameterNotSetException{
		if(isCircuitAssignMapAnsi == true)
			return cktAssignMapAnsi;
		else throw new ParameterNotSetException();
	}

	public void setCircuitAssignMap(CircuitAssignMapAnsi circuitAssignMapAnsi){

		cktAssignMapAnsi       = circuitAssignMapAnsi;
		isCircuitAssignMapAnsi = true;

	}

	public boolean isCircuitAssignMapPresent(){
		return isCircuitAssignMapAnsi;
	}
	
	/**
    * String representation of class CircuitGrpResetAckAnsiEvent
    *
    * @return    String provides description of class CircuitGrpResetAckAnsiEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisCircuitAssignMapAnsi = ");
				buffer.append(isCircuitAssignMapAnsi);
				buffer.append("\ncktAssignMapAnsi = ");
				buffer.append(cktAssignMapAnsi);
				return buffer.toString();
		
		}
	
	CircuitAssignMapAnsi cktAssignMapAnsi;
	boolean              isCircuitAssignMapAnsi;

}


