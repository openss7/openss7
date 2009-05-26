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
* DATE         : $Date: 2008/05/16 12:24:14 $
* 
* MODULE NAME  : $RCSfile: UserToUserIndicators.java,v $
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

public class UserToUserIndicators extends java.lang.Object implements java.io.Serializable{

	public UserToUserIndicators(){

	}

	public UserToUserIndicators(boolean in_type,
                            boolean in_nwDiscardInd){
		m_nwDiscardInd = in_nwDiscardInd;
		m_type         = in_type;
	}
	
	public boolean getNwDiscardInd(){
		return m_nwDiscardInd;
	}

	public boolean getType(){	
		return m_type;
	}

	public void setNwDiscardInd(boolean aNwDiscardInd){
		m_nwDiscardInd = aNwDiscardInd;
	}

	public void setType(boolean aType){
		m_type =  aType;
	}
	
	public byte flatUserToUserIndicators()
	{
		byte rc = 0;
		byte nwdis=0, type=0;
		
		if(m_type == true)
			type = 1;
		if(m_nwDiscardInd == true)
			nwdis = 1;
		
		rc = (byte)((type & IsupMacros.L_BIT1_MASK) |
					((nwdis << 7) & IsupMacros.L_BIT8_MASK));

		return rc;

	}
		
	public void  putUserToUserIndicators(byte[] arr, int index, byte par_len){

		if((byte)(arr[index] & IsupMacros.L_BIT1_MASK) == 1)
			 m_type = true;
		if((byte)((arr[index] >> 7)  & IsupMacros.L_BIT1_MASK) == 1)
			m_nwDiscardInd = true;
	}	
		
	/**
    * String representation of class UserServiceInfoPrime
    *
    * @return    String provides description of class UserServiceInfoPrime
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		       buffer.append(super.toString());
				buffer.append("\nnwDiscardInd = ");
				buffer.append(m_nwDiscardInd);
				buffer.append("\ntype = ");
				buffer.append(m_type);
				return buffer.toString(); 
		
		}		
	
	boolean m_nwDiscardInd;
	boolean m_type;

	public static final boolean TYPE_REQUEST = false; 
	public static final boolean TYPE_RESPONSE = true; 
	public static final boolean NDI_NO_INFORMATION = false; 
	public static final boolean NDI_USER_TO_USER_INFORMATION_DISCARDED_BY_NETWORK = true; 

}




