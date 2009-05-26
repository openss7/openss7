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
* MODULE NAME  : $RCSfile: CauseInd.java,v $
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

public class CauseInd extends java.lang.Object implements java.io.Serializable{

	public CauseInd(){

	}

	public CauseInd(byte location,
                byte codingStandard,
                byte causeValue,
                byte[] diagnostic,
                boolean ext1,
                boolean ext2)
         throws ParameterRangeInvalidException
	{
		 if ((location >= LOCATION_USER) &&
		  (location <= LOCATION_NETWORK_BEYOND_INTERWORKING_POINT)) {
			m_location = location;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		 if ((codingStandard >= CS_ITU_STANDARD) &&
		  (codingStandard <= CS_STANDARD_SPECIFIC_TO_IDENTIFIED_LOCATION)) {
			m_codingStandard = codingStandard;
		 }
		 else 
			throw new ParameterRangeInvalidException();

		m_causeValue     = causeValue; 
		m_diagnostic     = diagnostic;
		m_ext1           = ext1;
		m_ext2           = ext2;		
	}

	public byte getCauseValue(){

		return m_causeValue;
	}

	public byte getCodingStandard(){
		return m_codingStandard;
	}

	public byte[] getDiagnostic(){
		return m_diagnostic;
	}

	public boolean getExt1(){
		return m_ext1;
	}

	public boolean getExt2(){
		return m_ext2;
	}

	public byte getLocation(){
		return m_location;
	}

	public void setCauseValue(byte causeValue)
                   throws ParameterRangeInvalidException{

		/* not throwing an exception for this as the there are many spare values 
		in between in the range of cause value */
		causeValue = causeValue;
	}

	public void setCodingStandard(byte codingStandard)
                       throws ParameterRangeInvalidException{
		
		 if ((codingStandard >= CS_ITU_STANDARD) &&
		  (codingStandard <= CS_STANDARD_SPECIFIC_TO_IDENTIFIED_LOCATION)) {
			m_codingStandard = codingStandard;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void setDiagnostic(byte[] diagnostic){
		m_diagnostic = diagnostic;
	}

	public void setExt1(boolean ext1){
		m_ext1 = ext1;
	}

	public void setExt2(boolean ext2){
		m_ext2 = ext2;
	}

	public void setLocation(byte location)
                 throws ParameterRangeInvalidException{
		 if ((location >= LOCATION_USER) &&
		  (location <= LOCATION_NETWORK_BEYOND_INTERWORKING_POINT)) {
			m_location = location;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public void  putCauseInd(byte[] arr, int index, byte par_len){

		int i;

		if((byte)((arr[index] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
			m_ext1 = false;
		m_location        = (byte)(arr[index] & IsupMacros.L_bits41_MASK);
		m_codingStandard  = (byte)((arr[index]>>5) & IsupMacros.L_bits21_MASK);

		if(m_ext1 == false){				
				m_causeValue	  = (byte)(arr[index+1] & IsupMacros.L_bits71_MASK);
				if((byte)((arr[index+1] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
					m_ext2 = false;		
				m_diagnostic = new byte[par_len-2];
				for(i=2;i<par_len;i++)
					m_diagnostic[i-2] = arr[index+i];
		}
		else{
			m_causeValue	  = (byte)(arr[index+2] & IsupMacros.L_bits71_MASK);
			if((byte)((arr[index+2] >> 7)& IsupMacros.L_BIT1_MASK) == 1 )
					m_ext2 = false;
			m_diagnostic = new byte[par_len-3];	
			for(i=3;i<par_len;i++)
					m_diagnostic[i-3] = arr[index+i];
		}
	}

	public byte[] flatCauseInd()
		{
			byte[] rc = ByteArray.getByteArray(m_diagnostic.length+2);
			byte ext1=1, ext2=1;
			int i=0;
		
			if(m_ext1 == true)
				ext1 = 0;
			if(m_ext2 == true)
				ext2 = 0;

			rc[0] = (byte)((m_location & IsupMacros.L_bits41_MASK)|
							((m_codingStandard & IsupMacros.L_bits21_MASK) << 5) |
							((ext1 & IsupMacros.L_BIT1_MASK) << 7));
								
			rc[1] = (byte)(((ext2 & IsupMacros.L_BIT1_MASK) << 7) |
							(m_causeValue & IsupMacros.L_bits71_MASK));
		
			for(i=0;i<m_diagnostic.length;i++)
				rc[i+2] = m_diagnostic[i];


			return rc;
		}
		
	/**
    * String representation of class CauseInd
    *
    * @return    String provides description of class CauseInd
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ncauseValue = ");
				buffer.append(m_causeValue);
				buffer.append("\ncodingStandard  = ");
				buffer.append(m_codingStandard);
				if(m_diagnostic != null){
					buffer.append("\ndiagnostic  = ");	
					for(i=0;i<m_diagnostic.length;i++)
						buffer.append(" "+Integer.toHexString((int)(m_diagnostic[i] & 0xFF)));
				}
				buffer.append("\next1  = ");
				buffer.append(m_ext1);
				buffer.append("\next2  = ");
				buffer.append(m_ext2);
				buffer.append("\nlocation  = ");
				buffer.append(m_location);		
				return buffer.toString();
		
		}		
	
	
		byte    m_causeValue;
		byte    m_codingStandard;
		byte[]  m_diagnostic;
		boolean m_ext1;
		boolean m_ext2;
		byte    m_location;

		public static final byte CS_ANSI_STANDARD = 0x02; 
		public static final byte CS_ISO_IEC_STANDARD = 0x01; 
		public static final byte CS_ITU_STANDARD = 0x00; 
		public static final byte CS_NATIONAL_STANDARD = 0x02; 
		public static final byte CS_STANDARD_SPECIFIC_TO_IDENTIFIED_LOCATION = 0x03; 
		public static final boolean EI_LAST_OCTET = true; 
		public static final boolean EI_OCTET_CONTINUE_THROUGH_NEXT = false; 
		public static final byte LOCATION_INTERNATIONAL_NETWORK = 0x07;
		public static final byte LOCATION_LOCAL_INTERFACE = 0x06;
		public static final byte LOCATION_LOCAL_LOCAL_NETWORK = 0x02;
		public static final byte LOCATION_LOCAL_PRIVATE_NETWORK = 0x01;
		public static final byte LOCATION_NETWORK_BEYOND_INTERWORKING_POINT = 0x0A; 
		public static final byte LOCATION_REMOTE_LOCAL_NETWORK = 0x04;
		public static final byte LOCATION_REMOTE_PRIVATE_NETWORK = 0x05;
		public static final byte LOCATION_TRANSIT_NETWORK = 0x03;
		public static final byte LOCATION_USER = 0x00;
		
		
			
}
