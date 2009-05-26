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
* MODULE NAME  : $RCSfile: CalledPartyNumber.java,v $
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

public class CalledPartyNumber extends NumberParameter{


		public CalledPartyNumber(){

		}
		
		public CalledPartyNumber(byte natureOfAddressIndicator,
                         byte numberingPlanIndicator,
                         byte[] addressSignal)
                  throws ParameterRangeInvalidException{

				m_natureOfAddressIndicator = natureOfAddressIndicator;
				m_numberingPlanIndicator   = numberingPlanIndicator;
				m_addressSignal            = addressSignal;

		}

		public byte[] flatCalledPartyNumber()
		{
			return super.flatNumberParameter();
			
		}

		public void  putCalledPartyNumber(byte[] arr, int index, byte par_len){

			super.putNumberParameter(arr,index,par_len);
		}
	
	/**
    * String representation of class CalledPartyNumber
    *
    * @return    String provides description of class CalledPartyNumber
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				return buffer.toString();
		
		}		

}






















