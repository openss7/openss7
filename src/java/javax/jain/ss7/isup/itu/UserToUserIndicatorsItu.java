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
* MODULE NAME  : $RCSfile: UserToUserIndicatorsItu.java,v $
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

public class UserToUserIndicatorsItu extends UserToUserIndicators{

	public UserToUserIndicatorsItu(){
		super();
	}

	public UserToUserIndicatorsItu(boolean in_type,
                               boolean in_nwDiscardInd,
                               byte in_service1,
                               byte in_service2,
                               byte in_service3)
                        throws ParameterRangeInvalidException{
		super(in_type,in_nwDiscardInd);

		if((in_service1 >= S1RQ_NO_INFORMATION) && 
		(in_service1 <= S1RQ_REQUEST_ESSENTIAL))
			service1 = in_service1;

		else 
			throw new ParameterRangeInvalidException();

		if((in_service2 >= S2RQ_NO_INFORMATION) && 
		(in_service2 <= S2RQ_REQUEST_ESSENTIAL))
			service2 = in_service2;

		else 
			throw new ParameterRangeInvalidException();

		if((in_service3 >= S3RQ_NO_INFORMATION) && 
		(in_service3 <= S3RQ_REQUEST_ESSENTIAL))
			service3 = in_service3;

		else 
			throw new ParameterRangeInvalidException();

	}

	public byte getService1(){
		return service1;
	}
	
	public void setService1(byte aService1)
                 throws ParameterRangeInvalidException{
		if((aService1 >= S1RQ_NO_INFORMATION) && 
		(aService1 <= S1RQ_REQUEST_ESSENTIAL))
			service1 = aService1;

		else 
			throw new ParameterRangeInvalidException();
	}
	
	public byte getService2(){
		return service2;
	}
	
	public void setService2(byte aService2)
                 throws ParameterRangeInvalidException{
		if((aService2 >= S2RQ_NO_INFORMATION) && 
		(aService2 <= S2RQ_REQUEST_ESSENTIAL))
			service2 = aService2;

		else 
			throw new ParameterRangeInvalidException();
	}

	public byte getService3(){
		return service3;
	}
	
	public void setService3(byte aService3)
                 throws ParameterRangeInvalidException{
		if((aService3 >= S3RQ_NO_INFORMATION) && 
		(aService3 <= S3RQ_REQUEST_ESSENTIAL))
			service3 = aService3;

		else 
			throw new ParameterRangeInvalidException();
	}

	public byte flatUserToUserIndicatorsItu()
	{
		byte rc=0;
		
		rc = super.flatUserToUserIndicators();

		rc = (byte)(rc | 
			 ((service1 << 1) & IsupMacros.L_bits32_MASK) |
			 ((service2 << 3) & IsupMacros.L_bits54_MASK) |
			 ((service3 << 5) & IsupMacros.L_bits76_MASK));

		return rc;
	}

	public void  putUserToUserIndicatorsItu(byte[] arr, int index, byte par_len){
		super.putUserToUserIndicators(arr,index,par_len);
		service1 = (byte)((arr[index] >> 1)&IsupMacros.L_bits21_MASK);
		service2 = (byte)((arr[index] >> 3)&IsupMacros.L_bits21_MASK);		
		service3 = (byte)((arr[index] >> 5)&IsupMacros.L_bits21_MASK);
	}

	/**
    * String representation of class UserToUserIndicatorsItu
    *
    * @return    String provides description of class UserToUserIndicatorsItu
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nservice1 = ");
				buffer.append(service1);	
				buffer.append("\nservice2 = ");
				buffer.append(service2);	
				buffer.append("\nservice3 = ");
				buffer.append(service3);	
				return buffer.toString();
		
		}


	byte service1;
	byte service2;
	byte service3;

	public static final byte S1RQ_NO_INFORMATION = 0x00; 
	public static final byte S1RQ_REQUEST_NOT_ESSENTIAL = 0x02; 
	public static final byte S1RQ_REQUEST_ESSENTIAL = 0x03; 
	public static final byte S2RQ_NO_INFORMATION = 0x00; 
	public static final byte S2RQ_REQUEST_NOT_ESSENTIAL = 0x02; 
	public static final byte S2RQ_REQUEST_ESSENTIAL = 0x03; 
	public static final byte S3RQ_NO_INFORMATION = 0x00; 
	public static final byte S3RQ_REQUEST_NOT_ESSENTIAL = 0x02; 
	public static final byte S3RQ_REQUEST_ESSENTIAL = 0x03; 
	public static final byte S1RP_NO_INFORMATION = 0x00; 
	public static final byte S1RP_NOT_PROVIDED = 0x01; 
	public static final byte S1RP_PROVIDED = 0x02; 
	public static final byte S2RP_NO_INFORMATION = 0x00; 
	public static final byte S2RP_NOT_PROVIDED = 0x01; 
	public static final byte S2RP_PROVIDED = 0x02; 
	public static final byte S3RP_NO_INFORMATION = 0x00; 
	public static final byte S3RP_NOT_PROVIDED = 0x01; 
	public static final byte S3RP_PROVIDED = 0x02; 


	

}




