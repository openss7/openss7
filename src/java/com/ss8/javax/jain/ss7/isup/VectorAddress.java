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
* VERSION      : 1.1
* DATE         : 2008/05/16 12:23:21
* 
* MODULE NAME  : VectorAddress.java,v
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
package com.ss8.javax.jain.ss7.isup;

import java.util.*;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class VectorAddress{

	VectorAddress(SignalingPointCode dpc, int startCIC, int endCIC, int idx){
		m_dpc      = dpc;		
		m_startCIC = startCIC;
		m_endCIC   = endCIC; 
		m_idx      = idx;  
	}

	public SignalingPointCode getDPC(){
		return m_dpc;
	}

	public int getstartCIC(){
		return m_startCIC;
	}

	public int getendCIC(){
		return m_endCIC;
	}

	public int getidx(){
		return m_idx;
	}


	public int hashCode(){
		int result = 17;		
		result = 37 * result + m_startCIC + m_endCIC + m_idx; 
		try{
				result = 37 * result + m_dpc.getCluster();
				result = 37 * result + m_dpc.getMember();		
				result = 37 * result + m_dpc.getZone();
		}catch (Exception exc) {};
		return result;

    }

	public boolean equals(Object obj){
		
		if (obj instanceof VectorAddress)      
		{
				VectorAddress addr = (VectorAddress) obj;      

         // Compare attributes of this VectorAddress  and the passed VectorAddress
			try{
				if((this.m_dpc.equals(addr.m_dpc)) &&
						(this.m_startCIC == addr.m_startCIC) &&
						(this.m_endCIC == addr.m_endCIC) &&
						(this.m_idx    == addr.m_idx))
						return true;

			}catch (Exception exc) {};
		}		
		return false;		   

	}

	private SignalingPointCode m_dpc;
	private int				   m_startCIC;
	private int				   m_endCIC;
	private int                m_idx;


}
