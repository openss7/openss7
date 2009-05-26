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
* DATE         : $Date: 2008/05/16 12:23:58 $
* 
* MODULE NAME  : $RCSfile: NwSpecificFacItu.java,v $
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

public class NwSpecificFacItu extends java.lang.Object implements java.io.Serializable{


	public NwSpecificFacItu(){

	}

	public NwSpecificFacItu(short in_nwIdLen,
                        byte in_nwIdPlan,
                        byte in_typeOfNwId,
                        boolean in_extNwId,
                        byte[] in_nwId,
                        byte[] in_nwSpecificFac)
                 throws ParameterRangeInvalidException{

		nwIdLen       = in_nwIdLen;
		nwIdPlan      = in_nwIdPlan;
		typeOfNwId    = in_typeOfNwId;
		extNwId       = in_extNwId;
		nwId          = in_nwId;
		nwSpecificFac = in_nwSpecificFac;
		
		
	}
	
	public byte[] getNetworkId(){
		return nwId;
	}
	
	public void setNetworkId(byte[] aNetworkIdentification)
                  throws ParameterRangeInvalidException{
		nwId = aNetworkIdentification;
	}

	public boolean getNetworkIdExtensionInd(){
		return extNwId;
	}

	public void setNetworkIdExtensionInd(boolean nwIdExtension){
		extNwId = nwIdExtension;
	}	

	public short getNetworkIdLength(){
		return nwIdLen;
	}

	public void setNetworkIdLength(short networkIdLength)
                        throws ParameterRangeInvalidException{
		nwIdLen = networkIdLength;
	}

	public byte getNetworkIdPlan(){
		return nwIdPlan;
	}

	public void setNetworkIdPlan(byte aNetworkIdentificationPlan)
                      throws ParameterRangeInvalidException{
		nwIdPlan = aNetworkIdentificationPlan;
	}

	public byte[] getNetworkSpecificFac(){
		return nwSpecificFac;
	}

	public void setNetworkSpecificFac(byte[] aNwSpecificFac){
		nwSpecificFac = aNwSpecificFac;
	}
	
	public byte getTypeOfNetworkId(){
		return typeOfNwId;
	}

	public void setTypeOfNetworkId(byte aTypeOfNetworkIdentification)
                        throws ParameterRangeInvalidException{
		typeOfNwId = aTypeOfNetworkIdentification;
	}

	public byte[] flatNwSpecificFacItu()
	{
		int size = 2;
		byte ext = 1;
		int i=0;			

		size+= nwIdLen;

		if(nwSpecificFac != null)
			size += nwSpecificFac.length;

		byte[] rc = ByteArray.getByteArray(size);
			
		if(extNwId == true)
			ext = 0;

		rc[0] = (byte)nwIdLen;
		if(nwIdLen >  0){
			
			rc[1] = (byte)(((ext << 7)&	IsupMacros.L_BIT8_MASK) |
					((typeOfNwId << 4) & IsupMacros.L_bits75_MASK) |
					(nwIdPlan &IsupMacros. L_bits41_MASK));
			if(nwId != null){
				for(i=0;i<nwId.length;i++)
				{		
						
						rc[i+2] = (byte)(nwId[i] & IsupMacros.L_bits71_MASK);			
				}
				i= i+nwId.length+1;
			}
			else
				i+=2;
		}
		else{
				
				i++ ;
		}
		
		
		rc[i++] = (byte)nwSpecificFac.length;

		for(int j=0;j<nwSpecificFac.length;j++)
		{
			rc[i++] = nwSpecificFac[j];
		}

		return rc;
	}

	public void putNwSpecificFacItu(byte[] arr, int index, byte par_len)
	{
		int i;
		int nsf_len = 0;
				
		nwIdLen = arr[index];
		
		nsf_len = arr[index+nwIdLen+1];
							

		nwSpecificFac =  new byte[nsf_len];

		if(nwIdLen > 0){
			nwId =  new byte[nwIdLen-1];	
			if((byte)((arr[index+1]>>7) & IsupMacros.L_BIT1_MASK) == 1)
				extNwId = true;
			typeOfNwId = (byte)((arr[index+1]>>4) & IsupMacros.L_bits31_MASK);
			nwIdPlan   = (byte)(arr[index+1] & IsupMacros.L_bits41_MASK);
		
			for(i=0;i<nwIdLen-1;i++)
				nwId[i] = (byte)(arr[index+i+2] & IsupMacros.L_bits71_MASK);
		}

		for(i=0;i<nsf_len;i++)
			nwSpecificFac[i] = (byte)(arr[index+nwIdLen+2+i]);
		 		
		
	}

	/**
    * String representation of class NwSpecificFacItu
    *
    * @return    String provides description of class NwSpecificFacItu
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nextNwId = ");
				buffer.append(extNwId);
				if(nwId != null){
					buffer.append("\nnwId = ");
					for(i=0;i<nwId.length;i++)
						buffer.append(" "+Integer.toHexString((int)(nwId[i] & 0xFF)));
				}						
				buffer.append("\nnwIdLen = ");
				buffer.append(nwIdLen);
				buffer.append("\nnwIdPlan = ");
				buffer.append(nwIdPlan);	
				if(nwSpecificFac != null){
					buffer.append("\nnwSpecificFac = ");
					for(i=0;i<nwSpecificFac.length;i++)
						buffer.append(" "+Integer.toHexString((int)(nwSpecificFac[i] & 0xFF)));
				}	
				buffer.append("\ntypeOfNwId = ");
				buffer.append(typeOfNwId);
				return buffer.toString();
		
		}


	boolean extNwId;
	byte[]  nwId;
	short   nwIdLen;
	byte    nwIdPlan;
	byte[]  nwSpecificFac;
	byte    typeOfNwId;

	public static final byte TNI_NATIONAL_NETWORK_IDENTIFICATION = 0x02;
	public static final byte TNI_INTERNATIONAL_NETWORK_IDENTIFICATION = 0x03;
	public static final boolean EI_NEXT_OCTET_EXIST = false; 
	public static final boolean EI_LAST_OCTET = true; 				
	
}




