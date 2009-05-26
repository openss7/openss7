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
* MODULE NAME  : $RCSfile: CallReference.java,v $
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

public class CallReference extends java.lang.Object implements java.io.Serializable{



		public CallReference(){

		}
		
		public CallReference(int in_callId,
                     SignalingPointCode in_spc)
              throws ParameterRangeInvalidException{
		
			m_callId = in_callId;
			m_spc    = in_spc;

		}

		public int getCallIdentity(){

				return m_callId;
		}
		
		public SignalingPointCode getSignalingPointCode(){

				return m_spc;
		}

		public void setCallIdentity(int aCallIdentity)
                     throws ParameterRangeInvalidException{

			    /* no valid range specified in the specs to throw an exception */
			    m_callId = aCallIdentity;

		}

		public void setSignalingPointCode(SignalingPointCode aPointCode)
                           throws ParameterRangeInvalidException{

				/* no valid range specified in the specs to throw an exception */
				m_spc = aPointCode;
		}

		public void  putCallReference(byte[] arr,int index,byte par_len,int pcsize){
		
			int cluster; 		
			
			m_callId = (arr[index+2]<< 16) + (arr[index+1] << 8) + arr[index]; 
		
			if(pcsize == PC_SIZE_14){
				cluster = (arr[index+3] >> 3 & IsupMacros.L_bits51_MASK) |
					  ((arr[index+4]& IsupMacros.L_bits31_MASK)<<5);

				
				m_spc = new SignalingPointCode((arr[index+3] & IsupMacros.L_bits31_MASK),
											cluster,
										((arr[index+4] >> 3) & IsupMacros.L_bits31_MASK));
			}
			else
      			m_spc = new SignalingPointCode(arr[index+5],
      											arr[index+4],
      											arr[index+3]);
		}			

		public byte[] flatCallReference()
		{
			byte[] rc = ByteArray.getByteArray(7);
		
			byte ext = 0;
			int i=0;
				
			rc[0] = (byte)((m_callId & 0xFF0000) >> 16);
			rc[1] = (byte)((m_callId & 0x00FF00) >> 8);
			rc[2] = (byte)((m_callId & 0x0000FF));
			rc[3] = 0;

			try{
				rc[4] = (byte)(m_spc.getMember() & 0x000000FF);
				rc[5] = (byte)(m_spc.getCluster() & 0x000000FF);
				rc[6] = (byte)(m_spc.getZone() & 0x000000FF);

			} catch (Exception exception) {};														

			return rc;
		}
		
	/**
    * String representation of class CallReference
    *
    * @return    String provides description of class CallReference
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncallId = ");
				buffer.append(m_callId);
				buffer.append("\nspc  = ");
				buffer.append(m_spc);					
				return buffer.toString();
		
		}		
	
	int m_callId;
	SignalingPointCode m_spc;

	public static final byte PC_SIZE_14 = 1;
    public static final byte PC_SIZE_24 = 2;
	
}




