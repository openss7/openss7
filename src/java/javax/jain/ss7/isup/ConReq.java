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
* DATE         : $Date: 2008/05/16 12:24:00 $
* 
* MODULE NAME  : $RCSfile: ConReq.java,v $
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

public class ConReq extends java.lang.Object implements java.io.Serializable{
		
		public ConReq(){
		}

		public ConReq(int in_localRef,
              SignalingPointCode in_spc,
              short in_protoClass,
              short in_credit)
       throws ParameterRangeInvalidException{
				if((in_localRef >= 0) && (in_localRef <= 0xffffff))
						m_localRef   = in_localRef;
				else throw new ParameterRangeInvalidException();								
				m_spc        = in_spc;
				if((in_protoClass >=0) && (in_protoClass <=255))
						m_protoClass = in_protoClass;
				else throw new ParameterRangeInvalidException();	
				if((in_credit >=0) && (in_credit <=255))
						m_credit     = in_credit;
				else throw new ParameterRangeInvalidException();
				
		}

		public short getCredit(){
				return m_credit;
		}

		public int getLocalReference(){
				return m_localRef;
		}

		public short getProtocolClass(){
				return m_protoClass;
		}

		public SignalingPointCode getSignalingPointCode(){
				return m_spc;
		}

		public void setCredit(short aCredit)
			throws ParameterRangeInvalidException{
			    if((aCredit >=0) && (aCredit <=255))
				    m_credit = aCredit;
			    else throw new ParameterRangeInvalidException();
		}

		public void setLocalReference(int aLocalReference)
                       throws ParameterRangeInvalidException{
			    if((aLocalReference >= 0) && (aLocalReference <= 0xffffff))
				    m_localRef = aLocalReference;
			    else throw new ParameterRangeInvalidException();
		}

		public void setProtocolClass(short aProtocolClass)
                      throws ParameterRangeInvalidException{
			    if((aProtocolClass >=0) && (aProtocolClass <=255))
				    m_protoClass = aProtocolClass;

			    else throw new ParameterRangeInvalidException();
		}

		public void setSignalingPointCode(SignalingPointCode aPointCode)
                           throws ParameterRangeInvalidException{
				m_spc = aPointCode;
		}	
		
	public void  putConReq(byte[] arr, int index, byte par_len,int pcsize){

		int cluster; 		
		
		m_localRef = (arr[index+2] & 0x0000FF)|
					 (arr[index+1]<<8 & 0x00FF00)|
					 (arr[index] <<16 & 0xFF0000);

		if(pcsize == PC_SIZE_14){

			cluster = (arr[index+3] >> 3 & IsupMacros.L_bits51_MASK) |
					  ((arr[index+4] & IsupMacros.L_bits31_MASK)<<5);

			m_spc = new SignalingPointCode((arr[index+3] & IsupMacros.L_bits31_MASK),
											cluster,
											((arr[index+4] >> 3) & IsupMacros.L_bits31_MASK));
			m_protoClass = arr[index+5];
			m_credit     = arr[index+6];

		}
		else{
			m_spc = new SignalingPointCode(arr[index+5],
											arr[index+4],
											arr[index+3]);

			m_protoClass = arr[index+6];
			m_credit     = arr[index+7];
		}
	}	

	public byte[] flatConReq()
		{
			byte[] rc = ByteArray.getByteArray(11);
	
			int i=0;

			rc[0] = (byte)((m_localRef & 0xFF0000) >> 16);
			rc[1] = (byte)((m_localRef & 0x00FF00) >> 8);
			rc[2] = (byte)((m_localRef & 0x0000FF));
			rc[3] = 0;

			try{
				rc[4] = (byte)(m_spc.getMember() & 0x000000FF);
				rc[5] = (byte)(m_spc.getCluster() & 0x000000FF);
				rc[6] = (byte)(m_spc.getZone() & 0x000000FF);

			} catch (Exception exception) {};														

			for(i=0;i<2;i++)
				rc[i+7] =  (byte)((m_protoClass >> (8*i)) & 0x00FF);
					
			for(i=0;i<2;i++)
				rc[i+9] =  (byte)((m_credit >> (8*i)) & 0x00FF);
			return rc;
		}
		
	/**
    * String representation of class ConReq
    *
    * @return    String provides description of class ConReq
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncredit = ");
				buffer.append(m_credit);
				buffer.append("\nlocalRef  = ");
				buffer.append(m_localRef);
				buffer.append("\nprotoClass  = ");
				buffer.append(m_protoClass);	
				buffer.append("\nspc  = ");
				buffer.append(m_spc);					
				return buffer.toString();
		
		}		
	
	
	short              m_credit;
	int                m_localRef;
	short              m_protoClass;
	SignalingPointCode m_spc;
		
	public static final byte PC_SIZE_14 = 1;
	public static final byte PC_SIZE_24 = 2;	
}




