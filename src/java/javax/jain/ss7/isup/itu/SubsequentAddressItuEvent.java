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
* MODULE NAME  : $RCSfile: SubsequentAddressItuEvent.java,v $
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

public class SubsequentAddressItuEvent extends IsupEvent{
	
	public SubsequentAddressItuEvent(java.lang.Object source,
                                 SignalingPointCode dpc,
                                 SignalingPointCode opc,
                                 byte sls,
                                 int cic,
                                 byte congestionPriority,
                                 byte[] subsequentNumber)
                          throws ParameterRangeInvalidException{
	
		super(source,dpc,opc,sls,cic,congestionPriority);
		subsNumber          = subsequentNumber;
		m_subsNumberPresent = true;
	}
		
	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
        if(m_subsNumberPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();

	}
		
	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_SUBSEQUENT_ADDRESS;
	}

	public byte[] getSubsequentNumber()
                           throws MandatoryParameterNotSetException{
		if(m_subsNumberPresent == true)
			return subsNumber;
		else throw new MandatoryParameterNotSetException();
	}

	public void setSubsequentNumber(byte[] subsequentNumber){
		subsNumber = subsequentNumber;
		m_subsNumberPresent = true;
	}
	

	/**
    * String representation of class SubsequentAddressItuEvent
    *
    * @return    String provides description of class SubsequentAddressItuEvent
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nsubsNumber = ");
				for(i=0;i<subsNumber.length;i++)
					buffer.append(" "+Integer.toHexString((int)(subsNumber[i] & 0xFF)));		
				
				return buffer.toString();
		
		}


	
	byte[] subsNumber;

	protected boolean m_subsNumberPresent     = false;
	    
}


