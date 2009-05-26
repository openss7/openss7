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
* DATE         : $Date: 2008/05/16 12:24:13 $
* 
* MODULE NAME  : $RCSfile: TransitNwSel.java,v $
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

public class TransitNwSel extends java.lang.Object implements java.io.Serializable{

	public TransitNwSel(){

	}

	public TransitNwSel(byte in_nwIdPlan,
                    byte in_typeOfNwId,
                    byte[] in_nwId)
             throws ParameterRangeInvalidException{
		
		 if ((in_nwIdPlan >= NIP_CCITT_UNKNOWN) &&
		  (in_nwIdPlan <= NIP_CCITT_PUBLIC_LAND_MOBILE_NETWORK_IDENTIFICATION_CODE)) {

			m_nwIdPlan   = in_nwIdPlan;
		 }
		 else 
			throw new ParameterRangeInvalidException();
			
		m_nwId       = in_nwId;

		 if ((in_typeOfNwId >= TNI_CCITT_IDENTIFICATION) &&
		  (in_typeOfNwId <= TNI_CCITT_NATIONAL_NETWORK_IDENTIFICATION)) {
			m_typeOfNwId = in_typeOfNwId;

		 }
		 else 
			throw new ParameterRangeInvalidException();

	}

	public byte getNetworkIdPlan(){
		return m_nwIdPlan;
	}

	public byte[] getNwId(){
		return m_nwId;
	}

	public byte getTypeOfNetworkId(){
		return m_typeOfNwId;
	}

	public void setNetworkIdPlan(byte aNetworkIdentificationPlan)
                      throws ParameterRangeInvalidException{
		 if ((aNetworkIdentificationPlan >= NIP_CCITT_UNKNOWN) &&
		  (aNetworkIdentificationPlan <= NIP_CCITT_PUBLIC_LAND_MOBILE_NETWORK_IDENTIFICATION_CODE)) {
			m_nwIdPlan = aNetworkIdentificationPlan; 

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void setNwId(byte[] aNetworkIdentification){
		m_nwId = aNetworkIdentification;
	}

	public void setTypeOfNetworkId(byte aTypeOfNetworkIdentification)
                        throws ParameterRangeInvalidException{
		 if ((aTypeOfNetworkIdentification >= TNI_CCITT_IDENTIFICATION) &&
		  (aTypeOfNetworkIdentification <= TNI_CCITT_NATIONAL_NETWORK_IDENTIFICATION)) {
			m_typeOfNwId = aTypeOfNetworkIdentification;

		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void  putTransitNwSel(byte[] arr, int index, byte par_len){

		int i;

		m_nwIdPlan   = (byte)(arr[index] & IsupMacros.L_bits41_MASK);
		m_typeOfNwId = (byte)((arr[index] >> 4) & IsupMacros.L_bits31_MASK);
 
		m_nwId =  new byte[par_len-1];

		for(i=0;i<par_len-1;i++)
			
			m_nwId[i] = (byte)arr[index+i+1];
					
	}

	public byte[] flatTransitNwSel(){
		
		byte[] rc = ByteArray.getByteArray(m_nwId.length+1);	
		int i;		
				
		rc[0] = (byte)(((m_typeOfNwId << 4) & IsupMacros.L_bits75_MASK) |
						(m_nwIdPlan  & IsupMacros.L_bits41_MASK));

		for(i=0;i<m_nwId.length;i++)
			rc[i+1] = m_nwId[i];

		return rc;
	}
		

	/**
    * String representation of class TransitNwSel
    *
    * @return    String provides description of class TransitNwSel
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				if(m_nwId != null){
					buffer.append("\nnwId = ");
					for(i=0;i<m_nwId.length;i++)
						buffer.append(" "+Integer.toHexString((int)(m_nwId[i] & 0xFF)));
				}	
				buffer.append("\nnwIdPlan = ");
				buffer.append(m_nwIdPlan);
				buffer.append("\ntypeOfNwId = ");
				buffer.append(m_typeOfNwId);
				return buffer.toString();
		
		}		
	
	
	byte[] m_nwId;
	byte   m_nwIdPlan;
	byte   m_typeOfNwId;


	public static final byte TNI_CCITT_IDENTIFICATION = 0x00; 
	public static final byte TNI_CCITT_NATIONAL_NETWORK_IDENTIFICATION = 0x02; 
	public static final byte NIP_CCITT_UNKNOWN = 0x00; 
	public static final byte NIP_CCITT_PUBLIC_DATA_NETWORK_IDENTIFICATION_CODE = 0x03; 
	public static final byte NIP_CCITT_PUBLIC_LAND_MOBILE_NETWORK_IDENTIFICATION_CODE = 0x06; 
	public static final byte NIP_NATIONAL_UNKNOWN = 0x00; 
	public static final byte NIP_NATIONAL_3_DIGIT_CARRIER_IDENTIFICATION_WITH_CIRCUIT_CODE = 0x01; 
	public static final byte NIP_NATIONAL_4_DIGIT_CARRIER_IDENTIFICATION_WITH_CIRCUIT_CODE =  0x02; 

}




