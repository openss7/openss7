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
* DATE         : $Date: 2008/05/16 12:24:07 $
* 
* MODULE NAME  : $RCSfile: RemoteOperations.java,v $
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

public class RemoteOperations extends java.lang.Object implements java.io.Serializable{

	
	public RemoteOperations(){

	}

	public RemoteOperations(byte in_protoProfile,
                        boolean in_isComp,
                        byte[] in_comp)
                 throws ParameterRangeInvalidException{
		
		m_protoProfile = in_protoProfile;
		m_isComp       = in_isComp;
		m_comp         = in_comp;
	}
	

	public boolean getComponentInd(){
		return m_isComp;
	}
	public byte[] getComponents(){
		return m_comp;
	}
	public byte getProtocolProfile(){
		return m_protoProfile;
	}
	public void setComponentInd(boolean componentFlag){
		m_isComp = componentFlag;
	}
	public void setComponents(byte[] components){
		m_comp = components;
	}
	public void setProtocolProfile(byte aProtocolProfile)
                        throws ParameterRangeInvalidException{
		/* there is no range as such for this parameter content */
		m_protoProfile = aProtocolProfile;
	}

	public void  putRemoteOperations(byte[] arr, int index, byte par_len){
		
		int i;

		if(par_len > 1)
			m_isComp = true;
		m_protoProfile = (byte)(arr[index] & IsupMacros.L_bits41_MASK);

		m_comp =  new byte[par_len-1];

		for(i=0;i<par_len-1;i++)
			m_comp[i] = (byte)arr[index+i+1];
		
	}
		
	public byte[] flatRemoteOperations(){
		
		int i=0;
		byte[] rc = ByteArray.getByteArray(1+m_comp.length);
				
		rc[0] = (byte)(m_protoProfile & IsupMacros.L_bits51_MASK); 

		for(i=0;i<m_comp.length;i++)
			rc[i+1] = m_comp[i];
		
		return rc;
	}
		
	/**
    * String representation of class RemoteOperations
    *
    * @return    String provides description of class RemoteOperations
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				if(m_comp != null){
					buffer.append("\ncomp = ");
					for(i=0;i<m_comp.length;i++)
						buffer.append(" "+Integer.toHexString((int)(m_comp[i] & 0xFF)));
				}				
				buffer.append("\nisComp = ");
				buffer.append(m_isComp);
				buffer.append("\nprotoProfile = ");
				buffer.append(m_protoProfile);
				return buffer.toString();
		
		}		
	
	
	byte[]  m_comp;
	boolean m_isComp;
	byte    m_protoProfile;

	public static final byte PPF_REMOTE_OPERATIONS_PROTOCOL = 0x11; 
	public static final boolean EI_NEXT_OCTET_EXIST = false; 
	public static final boolean EI_LAST_OCTET = true; 


}




