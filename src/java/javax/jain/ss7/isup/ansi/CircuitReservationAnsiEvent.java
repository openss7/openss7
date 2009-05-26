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
* MODULE NAME  : $RCSfile: CircuitReservationAnsiEvent.java,v $
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

public class CircuitReservationAnsiEvent extends IsupEvent{

	public CircuitReservationAnsiEvent(java.lang.Object source,
                                   SignalingPointCode dpc,
                                   SignalingPointCode opc,
                                   byte sls,
                                   int cic,
                                   byte congestionPriority,
                                   NatureConnInd in_natureConnInd)
                            throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		natureConnInd = in_natureConnInd;
		m_natureConnIndPresent     = true;
	}

	public void checkMandatoryParameters()
                              throws MandatoryParameterNotSetException{
		if(m_natureConnIndPresent == true) 
			return;
		else throw new MandatoryParameterNotSetException();
	}

	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_CIRCUIT_RESERVATION;
	}
		
	public NatureConnInd getNatureConnInd()
                               throws	MandatoryParameterNotSetException{
		if(m_natureConnIndPresent == true)
			return natureConnInd;
		else throw new MandatoryParameterNotSetException();
		
	}
	
	public void setNatureConnInd(NatureConnInd in_natureConnInd){
		natureConnInd = in_natureConnInd;
		m_natureConnIndPresent = true;
	}
		
	/**
    * String representation of class CircuitReservationAnsiEvent
    *
    * @return    String provides description of class CircuitReservationAnsiEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nnatureConnInd = ");
				buffer.append(natureConnInd);
				return buffer.toString();
		
		}

	NatureConnInd natureConnInd;

	protected boolean m_natureConnIndPresent     = false;
}


