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
* DATE         : $Date: 2008/05/16 12:24:06 $
* 
* MODULE NAME  : $RCSfile: RangeAndStatus.java,v $
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

public class RangeAndStatus extends java.lang.Object implements java.io.Serializable{


	public RangeAndStatus(){

	}

	public RangeAndStatus(byte in_range,
                      byte[] in_status)
               throws ParameterRangeInvalidException{
		
		m_range  = in_range; 
		if(in_status != null){
			m_status = in_status;		
			m_isStatus = true;
		}
	}
		
	public byte getRange(){
		return m_range;
	}

	public byte[] getStatus(){
		return m_status;	
	}

	public boolean isStatusPresent(){
		if(m_isStatus == true)
				return true;
		else
				return false;
	}
	
	public void setRange(byte in_range)
			      throws ParameterRangeInvalidException{
		m_range = in_range;
	}

	public void setStatus(byte[] in_status){
		m_status = in_status;
		m_isStatus = true;
	}

	public void  putRangeAndStatus(byte[] arr, int index, byte par_len){	
		
		int i;

		if(par_len > 1)
			m_isStatus = true;

		m_range = (byte)arr[index];

		if(m_isStatus == true){
			m_status = new byte[par_len-1];
			for(i=0;i<par_len-1;i++)
				m_status[i] = (byte)arr[index+1+i];
		}
	}	

	public byte[] flatRangeAndStatus(){
		
			int len=0;

			if(m_status != null)
				len = m_status.length;
			
			byte[] rc = ByteArray.getByteArray(len+1);	
			int i;			

			rc[0] = m_range;
			if(m_isStatus == true){
				for(i=0;i<m_status.length;i++)
					rc[i+1] = m_status[i];
			}
			return rc;
	}

	/**
    * String representation of class RangeAndStatus
    *
    * @return    String provides description of class RangeAndStatus
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nisStatus = ");
				buffer.append(m_isStatus);
				buffer.append("\nrange  = ");
				buffer.append(m_range);
				if(m_status != null){
					buffer.append("\nstatus = ");
					for(i=0;i<m_status.length;i++)
						buffer.append(" "+Integer.toHexString((int)(m_status[i] & 0xFF)));
				}
				return buffer.toString();
		
		}		
	
	
	boolean m_isStatus;
	byte    m_range;
	byte[]  m_status;



}




