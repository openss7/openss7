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
* DATE         : $Date: 2008/05/16 12:23:55 $
* 
* MODULE NAME  : $RCSfile: EchoControlInfoItu.java,v $
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

public class EchoControlInfoItu extends java.lang.Object implements java.io.Serializable{


	public EchoControlInfoItu(){
	}

	public EchoControlInfoItu(byte in_ogHalfEchoControlDevRespInd,
                          byte in_icHalfEchoControlDevRespInd,
                          byte in_ogHalfEchoControlDevReqInd,
                          byte in_icHalfEchoControlDevReqInd)
                   throws ParameterRangeInvalidException{
		
		if ((in_icHalfEchoControlDevReqInd >= IRQI_NO_INFORMATION) && 
		(in_icHalfEchoControlDevReqInd <= IRQI_INCOMING_HALF_ECHO_CONTROL_DEVICE_DEACTIVATION_REQUEST)) {
			icHalfEchoControlDevReqInd = in_icHalfEchoControlDevReqInd;

		}
		else 
			throw new ParameterRangeInvalidException();

		if ((in_icHalfEchoControlDevRespInd >= IRPI_NO_INFORMATION) && 
		(in_icHalfEchoControlDevRespInd <= IRPI_INCOMING_HALF_ECHO_CONTROL_DEVICE_INCLUDED)) {
			icHalfEchoControlDevRespInd = in_icHalfEchoControlDevRespInd;

		}
		else 
			throw new ParameterRangeInvalidException();

		if ((in_ogHalfEchoControlDevReqInd >= ORQI_NO_INFORMATION) && 
		(in_ogHalfEchoControlDevReqInd <= ORQI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_DEACTIVATION_REQUEST)) {
			ogHalfEchoControlDevReqInd = in_ogHalfEchoControlDevReqInd;

		}
		else 
			throw new ParameterRangeInvalidException();

		if ((in_ogHalfEchoControlDevRespInd >= ORPI_NO_INFORMATION) && 
		(in_ogHalfEchoControlDevRespInd <= ORPI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_INCLUDED)) {
			ogHalfEchoControlDevRespInd = in_ogHalfEchoControlDevRespInd;

		}
		else 
			throw new ParameterRangeInvalidException();

	}

	public byte getIcHalfEchoControlDevReqInd(){
		return icHalfEchoControlDevReqInd;
	}

	public void setIcHalfEchoControlDevReqInd(byte aIcHalfEchoControlDevReqInd)
                                   throws ParameterRangeInvalidException{
		if ((aIcHalfEchoControlDevReqInd >= IRQI_NO_INFORMATION) && 
		(aIcHalfEchoControlDevReqInd <= IRQI_INCOMING_HALF_ECHO_CONTROL_DEVICE_DEACTIVATION_REQUEST)) {
			icHalfEchoControlDevReqInd = aIcHalfEchoControlDevReqInd;

		}
		else 
			throw new ParameterRangeInvalidException();
	}

	public byte getIcHalfEchoControlDevRespInd(){
		return icHalfEchoControlDevRespInd;
	}

	public void setIcHalfEchoControlDevRespInd(byte aIcHalfEchoDevRespInd)
                                    throws ParameterRangeInvalidException{
		if ((aIcHalfEchoDevRespInd >= IRPI_NO_INFORMATION) && 
		(aIcHalfEchoDevRespInd <= IRPI_INCOMING_HALF_ECHO_CONTROL_DEVICE_INCLUDED)) {
			icHalfEchoControlDevRespInd = aIcHalfEchoDevRespInd;

		}
		else 
			throw new ParameterRangeInvalidException();
	}

	public byte getOgHalfEchoControlDevReqInd(){
		return ogHalfEchoControlDevReqInd;
	}

	public void setOgHalfEchoControlDevReqInd(byte aOgHalfEchoDevReqInd)
                                   throws ParameterRangeInvalidException{
		if ((aOgHalfEchoDevReqInd >= ORQI_NO_INFORMATION) && 
		(aOgHalfEchoDevReqInd <= ORQI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_DEACTIVATION_REQUEST)) {
			ogHalfEchoControlDevReqInd = aOgHalfEchoDevReqInd;

		}
		else 
			throw new ParameterRangeInvalidException();
	}
	
	public byte getOgHalfEchoControlDevRespInd(){
		return ogHalfEchoControlDevRespInd;
	}

	public void setOgHalfEchoControlDevRespInd(byte aOgHalfEchoDevRespInd)
                                    throws ParameterRangeInvalidException{
		if ((aOgHalfEchoDevRespInd >= ORPI_NO_INFORMATION) && 
		(aOgHalfEchoDevRespInd <= ORPI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_INCLUDED)) {
			ogHalfEchoControlDevRespInd = aOgHalfEchoDevRespInd;

		}
		else 
			throw new ParameterRangeInvalidException();
	}

	public void putEchoControlInfoItu(byte[] arr, int index, byte par_len)
	{
		ogHalfEchoControlDevRespInd = (byte)(arr[index] & IsupMacros.L_bits21_MASK);
		icHalfEchoControlDevRespInd = (byte)((arr[index]>>2) &IsupMacros.L_bits21_MASK);
		ogHalfEchoControlDevReqInd  = (byte)((arr[index]>>4) &IsupMacros.L_bits21_MASK);
		icHalfEchoControlDevReqInd  = (byte)((arr[index]>>6) &IsupMacros.L_bits21_MASK);

	}		

	public byte flatEchoControlInfoItu()
	{

		byte rc = 0;
		
		rc = (byte)((ogHalfEchoControlDevRespInd & IsupMacros.L_bits21_MASK) | 
			((icHalfEchoControlDevRespInd & IsupMacros.L_bits21_MASK)<< 2) |
			((ogHalfEchoControlDevReqInd & IsupMacros.L_bits21_MASK)<< 4)|
			((icHalfEchoControlDevReqInd & IsupMacros.L_bits21_MASK)<< 6));
		
		return rc;
	}


	/**
    * String representation of class EchoControlInfoItu
    *
    * @return    String provides description of class EchoControlInfoItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nicHalfEchoControlDevReqInd  = ");
				buffer.append(icHalfEchoControlDevReqInd);	
				buffer.append("\nicHalfEchoControlDevRespInd  = ");
				buffer.append(icHalfEchoControlDevRespInd);	
				buffer.append("\nogHalfEchoControlDevReqInd  = ");
				buffer.append(ogHalfEchoControlDevReqInd);	
				buffer.append("\nogHalfEchoControlDevRespInd  = ");
				buffer.append(ogHalfEchoControlDevRespInd);	
				return buffer.toString();
		
		}


	byte icHalfEchoControlDevReqInd;
	byte icHalfEchoControlDevRespInd;
	byte ogHalfEchoControlDevReqInd;
	byte ogHalfEchoControlDevRespInd;
	
	public static final byte ORPI_NO_INFORMATION = 0x00; 
	public static final byte ORPI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED = 0x01; 
	public static final byte ORPI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_INCLUDED = 0x02; 
	public static final byte IRPI_NO_INFORMATION = 0x00; 
	public static final byte IRPI_INCOMING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED = 0x01; 
	public static final byte IRPI_INCOMING_HALF_ECHO_CONTROL_DEVICE_INCLUDED = 0x02; 
	public static final byte ORQI_NO_INFORMATION = 0x00; 
	public static final byte ORQI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_ACTIVATION_REQUEST = 0x01; 
	public static final byte ORQI_OUTGOING_HALF_ECHO_CONTROL_DEVICE_DEACTIVATION_REQUEST = 0x02; 
	public static final byte IRQI_NO_INFORMATION = 0x00; 
	public static final byte IRQI_INCOMING_HALF_ECHO_CONTROL_DEVICE_ACTIVATION_REQUEST = 0x01; 
	public static final byte IRQI_INCOMING_HALF_ECHO_CONTROL_DEVICE_DEACTIVATION_REQUEST = 0x02; 

	
}




