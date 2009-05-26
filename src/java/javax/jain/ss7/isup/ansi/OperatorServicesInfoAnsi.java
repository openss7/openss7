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
* DATE         : $Date: 2008/05/16 12:24:11 $
* 
* MODULE NAME  : $RCSfile: OperatorServicesInfoAnsi.java,v $
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


package javax.jain.ss7.isup.ansi;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class OperatorServicesInfoAnsi extends java.lang.Object implements java.io.Serializable{

	public OperatorServicesInfoAnsi(){

	}
		
	public OperatorServicesInfoAnsi(byte in_infoType,
                                byte in_infoValu)
                         throws ParameterRangeInvalidException{
		if((in_infoType >=IT_ORIGINAL_ACCESS_PREFIX ) && (in_infoType <= IT_ACCESS_SIGNALING) &&
			(in_infoValu >= IV_IT1_UNKNOWN) && ( in_infoValu <= IV_IT2_ENTRY_AUTOMATED_BY_SPOKEN_INPUT_PERSON_HANDELING)){
			infoType   = in_infoType;
			infoValue  = in_infoValu;
		}else throw new ParameterRangeInvalidException();
			

	}

	public byte getInfoType(){
		return infoType;
	}

	public void setInfoType(byte in_infoType)
                 throws ParameterRangeInvalidException{
		if((in_infoType >=IT_ORIGINAL_ACCESS_PREFIX ) && (in_infoType <= IT_ACCESS_SIGNALING))
			infoType   = in_infoType;
		else throw new ParameterRangeInvalidException();
	}

	public byte getInfoValue(){
		return infoValue;
	}

	public void setInfoValue(byte in_infoValue)
                  throws ParameterRangeInvalidException{
		if((in_infoValue >= IV_IT1_UNKNOWN) && ( in_infoValue <= IV_IT2_ENTRY_AUTOMATED_BY_SPOKEN_INPUT_PERSON_HANDELING))
			infoValue  = in_infoValue;
		else throw new ParameterRangeInvalidException();
	}

	public void  putOperatorServicesInfoAnsi(byte[] arr,int index,byte par_len){

		infoType = (byte)((arr[index] >> 4) & IsupMacros.L_bits41_MASK);
		infoValue = (byte)(arr[index] & IsupMacros.L_bits41_MASK);
				
	}

	public byte flatOperatorServicesInfoAnsi(){

		byte rc = 0;	
		
		rc = (byte)(((infoType & IsupMacros.L_bits41_MASK) << 4) |
					(infoValue & IsupMacros.L_bits41_MASK));
	
		return rc;
	}
		

	/**
    * String representation of class OperatorServicesInfoAnsi
    *
    * @return    String provides description of class OperatorServicesInfoAnsi
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\ninfoType = ");
				buffer.append(infoType);
				buffer.append("\ninfoValue = ");
				buffer.append(infoValue);
				return buffer.toString();
		
		}

	byte infoType;
	byte infoValue;

	public static final byte IT_ORIGINAL_ACCESS_PREFIX = 0x00; 
	public static final byte IT_BILL_TO_INFORMATION_ENTRY_HANDELING = 0x02; 
	public static final byte IT_BILL_TO_TYPE = 0x03; 
	public static final byte IT_BILL_TO_SPECIFIC_INFORMATION = 0x04; 
	public static final byte IT_SPECIAL_HANDELING = 0x05; 
	public static final byte IT_ACCESS_SIGNALING = 0x07; 
	public static final byte IV_IT1_UNKNOWN = 0x00; 
	public static final byte IV_IT1_1_001 = 0x01; 
	public static final byte IV_IT1_0_01 = 0x02; 
	public static final byte IV_IT1_0 = 0x03; 
	public static final byte IV_IT2_ENTRY_UNKNOWN_HANDELING_UNKNOWN = 0x00; 
	public static final byte IV_IT2_ENTRY_MANUAL_BY_OPERATOR_STATION_HANDELING = 0x01; 
	public static final byte IV_IT2_ENTRY_MANUAL_BY_OPERATOR_PERSON_HANDELING = 0x02; 
	public static final byte IV_IT2_ENTRY_AUTOMATED_BY_TONE_INPUT_STATION_HANDELING = 0x03; 
	public static final byte IV_IT2_ENTRY_UNKNOWN_STATION_HANDELING = 0x04; 
	public static final byte IV_IT2_ENTRY_UNKNOWN_PERSON_HANDELING = 0x05; 
	public static final byte IV_IT2_ENTRY_MANUAL_BY_OPERATOR_UNKNOWN_HANDELING = 0x06; 
	public static final byte IV_IT2_ENTRY_AUTOMATED_BY_TONE_INPUT_UNKNOWN_HANDELING = 0x07; 
	public static final byte IV_IT2_ENTRY_AUTOMATED_BY_TONE_INPUT_PERSON_HANDELING = 0x08; 
	public static final byte IV_IT2_ENTRY_AUTOMATED_BY_SPOKEN_INPUT_UNKNOWN_HANDELING = 0x09; 
	public static final byte IV_IT2_ENTRY_AUTOMATED_BY_SPOKEN_INPUT_STATION_HANDELING = 0x0A; 
	public static final byte IV_IT2_ENTRY_AUTOMATED_BY_SPOKEN_INPUT_PERSON_HANDELING = 0x0B; 
	public static final byte IV_IT3_UNKNOWN = 0x00; 
	public static final byte IV_IT3_CARD_14_DIGIT_FORMAT = 0x01; 
	public static final byte IV_IT3_CARD_89C_FORMAT = 0x02; 
	public static final byte IV_IT3_CARD_OTHER_FORMAT = 0x03; 
	public static final byte IV_IT3_COLLECT = 0x04; 
	public static final byte IV_IT3_THIRD_NUMBER = 0x05; 
	public static final byte IV_IT3_SENT_PAID = 0x06; 
	public static final byte IV_IT4_NIDB_AUTHORIZES = 0x00; 
	public static final byte IV_IT4_NIDB_REPORTS_VERIFY_BY_AUTOMATED_MEANS = 0x02; 
	public static final byte IV_IT4_NIDB_REPORTS_VERIFY_BY_OPERATOR = 0x03; 
	public static final byte IV_IT4_NO_NIDB_QUERY = 0x04; 
	public static final byte IV_IT4_NO_NIDB_RESPONSE = 0x05; 
	public static final byte IV_IT4_NIDB_REPORTS_UNAVAILABLE = 0x06; 
	public static final byte IV_IT4_NO_NIDB_RESPONSE_TIMEOUT = 0x07; 
	public static final byte IV_IT4_NO_NIDB_RESPONSE_REJECT_COMPONENT = 0x08; 
	public static final byte IV_IT4_NO_NIDB_RESPONSE_ACG_IN_EFECT = 0x09; 
	public static final byte IV_IT4_NO_NIDB_RESPONSE_SCCP_FAILURE = 0x0A; 
	public static final byte IV_IT5_UNKNOWN = 0x00; 
	public static final byte IV_IT5_CALL_COMPLETION = 0x01; 
	public static final byte IV_IT5_RATE_INFORMATION = 0x02; 
	public static final byte IV_IT5_TROUBLE_REPORTING = 0x03; 
	public static final byte IV_IT5_TIME_AND_CHANGES = 0x04; 
	public static final byte IV_IT5_CREDIT_REPORTING = 0x05; 
	public static final byte IV_IT5_GENERAL_ASSISTANCE = 0x06; 
	public static final byte IV_IT7_UNKNOWN = 0x00; 
	public static final byte IV_IT7_DIAL_PULSE = 0x01; 
	public static final byte IV_IT7_DUAL_TONE_MULTIFREQUENCY = 0x02; 


}


