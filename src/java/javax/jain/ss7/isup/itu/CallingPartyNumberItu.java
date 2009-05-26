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
* DATE         : $Date: 2008/05/16 12:23:54 $
* 
* MODULE NAME  : $RCSfile: CallingPartyNumberItu.java,v $
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

public class CallingPartyNumberItu extends CallingPartyNumber{

		public CallingPartyNumberItu(){
			super();
		}

		public CallingPartyNumberItu(byte natureOfAddressIndicator,
                             byte numberingPlanIndicator,
                             byte addressPresentationRestrictedIndicator,
                             byte screeningIndicator,
                             byte[] addressSignal,
                             boolean numberIncompleteIndicator)
                      throws ParameterRangeInvalidException{
			super(natureOfAddressIndicator,
						numberingPlanIndicator,
						addressPresentationRestrictedIndicator,
                        screeningIndicator,
                        addressSignal);
			m_numberIncompleteIndicator = numberIncompleteIndicator;
		}

		
	public boolean getNumberIncompleteInd(){
		return m_numberIncompleteIndicator;
	}

	public void setNumberIncompleteInd(boolean numberIncompleteIndicator){
		m_numberIncompleteIndicator = numberIncompleteIndicator;
	}

	public byte[] flatCallingPartyNumberItu()
	{
			byte[] rc;
			byte ni = 0;			

			if(m_numberIncompleteIndicator == true)
				ni = 1;

			rc = super.flatCallingPartyNumber();			

			rc[1] = (byte) (rc[1] | ((ni << 7) & IsupMacros.L_BIT8_MASK ));
				

			return rc;			
			
	}

		
	/**
    * String representation of class CallingPartyNumberItu
    *
    * @return    String provides description of class CallingPartyNumberItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nm_numberIncompleteIndicator = ");
				buffer.append(m_numberIncompleteIndicator);	
				return buffer.toString();
		
		}		

	boolean m_numberIncompleteIndicator;

	public static final boolean CPNII_COMPLETE = false; 
	public static final boolean CPNII_INCOMPLETE = true; 

}




