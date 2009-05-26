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
* MODULE NAME  : $RCSfile: Precedence.java,v $
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

public class Precedence extends java.lang.Object implements java.io.Serializable{

	
	
	public Precedence(){

	}

	public Precedence(byte in_precLevel,
                  byte in_lfb,
                  byte[] in_nwId,
                  int in_mlppServDomain)
           throws ParameterRangeInvalidException{


		 if ((in_lfb >= LFB_ALLOWED) &&
		  (in_lfb <= LFB_NOT_ALLOWED)) {
			m_lfb            = in_lfb;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((in_precLevel >= PL_FLASH_OVERIDE) &&
		  (in_precLevel <= PL_ROUTINE)) {
			m_precLevel      = in_precLevel;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		m_nwId           = in_nwId;
		m_mlppServDomain = in_mlppServDomain;

	}

	public byte getLookAheadForBusyInd(){
		return m_lfb;
	}

	public int getMLPPServiceDomain(){
		return m_mlppServDomain;
	}

	public byte[] getNetworkId(){
		return m_nwId;
	}

	public byte getPrecedenceLevel(){
		return m_precLevel;
	}

	public void setLookAheadForBusyInd(byte aLookAheadForBusy)
                            throws ParameterRangeInvalidException{
		 if ((aLookAheadForBusy >= LFB_ALLOWED) &&
		  (aLookAheadForBusy <= LFB_NOT_ALLOWED)) {
			m_lfb = aLookAheadForBusy;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void setMLPPServiceDomain(int aMLPPServiceDomain)
                          throws ParameterRangeInvalidException{
		/* no valid range found to theow an exception */
		m_mlppServDomain = aMLPPServiceDomain;
	}

	public void setNetworkId(byte[] networkId){
		m_nwId = networkId;
	}

	public void setPrecedenceLevel(byte aPrecedenceLevel)
                        throws ParameterRangeInvalidException{
		 if ((aPrecedenceLevel >= PL_FLASH_OVERIDE) &&
		  (aPrecedenceLevel <= PL_ROUTINE)) {
			m_precLevel = aPrecedenceLevel;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}
	
	public void  putPrecedence(byte[] arr, int index, byte par_len){
		
		int i;		

		m_lfb      = (byte)((arr[index] >> 5) & IsupMacros.L_bits21_MASK);
		m_precLevel= (byte)(arr[index] & IsupMacros.L_bits41_MASK);
		m_nwId = new byte[2];
		for(i=0;i<2;i++)
			m_nwId[i] = (byte)arr[index+1+i];

		m_mlppServDomain = 	(arr[index+3] << 16) & 0xFF0000 |
							(arr[index+4] << 8) & 0x00FF00 |
							(arr[index+5]) & 0x0000FF;	
			
	}

	public byte[] flatPrecedence()
	{
		byte[] rc = ByteArray.getByteArray(4+m_nwId.length);		
			
		int i=0;
		
			
		rc[0] = (byte)(((m_lfb & IsupMacros.L_bits21_MASK) << 5) |
						(m_precLevel & IsupMacros.L_bits41_MASK));
		for( i=0; i<m_nwId.length; i++)
			rc[i+1] = (byte)(m_nwId[i]);
		rc[m_nwId.length+1] = (byte)((m_mlppServDomain >> 16)&0x0000FF);
		rc[m_nwId.length+2] = (byte)((m_mlppServDomain >> 8)&0x0000FF);
		rc[m_nwId.length+3] = (byte)((m_mlppServDomain)&0x0000FF);
		return rc;

	}
	/**
    * String representation of class Precedence
    *
    * @return    String provides description of class Precedence
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nlfb = ");
				buffer.append(m_lfb);
				buffer.append("\nmlppServDomain  = ");
				buffer.append(m_mlppServDomain);
				if(m_nwId != null){
					buffer.append("\nnwId = ");				
					for(i=0;i<m_nwId.length;i++)
						buffer.append(" "+Integer.toHexString((int)(m_nwId[i] & 0xFF)));
				}
				buffer.append("\nprecLevel  = ");
				buffer.append(m_precLevel);			
				return buffer.toString();
		
		}		
	
	
	
	byte   m_lfb;
	int    m_mlppServDomain;
	byte[] m_nwId;
	byte   m_precLevel;
	
	public static final byte LFB_ALLOWED = 0; 
	public static final byte LFB_PATH_RESERVED = 0x02; 
	public static final byte LFB_NOT_ALLOWED = 0x04; 
	public static final byte PL_FLASH_OVERIDE = 0x00; 
	public static final byte PL_FLASH = 0x01; 
	public static final byte PL_IMMEDIATE = 0x02; 
	public static final byte PL_PRIORITY = 0x03; 
	public static final byte PL_ROUTINE = 0x04; 


}




