/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Mahindra British Telecom
 *  155 , Bombay - Pune Road 
 *  Pimpri ,
 *  Pune - 411 018.
 *
 *  Module Name   : JAIN INAP API
 *  File Name     : ErrorCode.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;

import java.io.*;
import java.util.*;
import java.lang.*;
/**
This class specifies the Return Error Code. 
*/

public class ErrorCode   {

    public static final int M_CANCELLED=0;
    public static final int M_CANCEL_FAILED=1;
    public static final int M_ETC_FAILED=3;
    public static final int M_IMPROPER_CALLER_RESPONSE=4;
    public static final int M_MISSING_CUSTOMER_RECORD=6;
    public static final int M_MISSING_PARAMETER=7;
    public static final int M_PARAMETER_OUT_OF_RANGE=8;
    public static final int M_REQUESTED_INFO_ERROR=10;
    public static final int M_SYSTEM_FAILURE=11;
    public static final int M_TASK_REFUSED=12;
    public static final int M_UNAVAILABLE_RESOURCE=13;
    public static final int M_UNEXPECTED_COMPONENT_SEQUENCE=14;
    public static final int M_UNEXPECTED_DATA_VALUE=15;
    public static final int M_UNEXPECTED_PARAMETER=16;
    public static final int M_UNKNOWN_LEGID=17;
    public static final int M_UNKNOWN_RESOURCE=18;
    public static final int M_SCF_REFERRAL=21;
    public static final int M_SCF_TASK_REFUSED=22;
    public static final int M_CHAINING_REFUSED=23;
	
	
// internal variable to store the constant value 
	
	private int errorCode;
	
	

    
/**
Constructor for ErrorCode datatype
*/
    
    private ErrorCode(int errorCode )
	{
	
		this.errorCode=errorCode;
	
	}

/**
* ErrorCode :CANCELLED
*
*/

public static final ErrorCode CANCELLED=new ErrorCode (M_CANCELLED);




/**
* ErrorCode :CANCEL_FAILED
*
*/

public static final ErrorCode CANCEL_FAILED=new ErrorCode (M_CANCEL_FAILED);


/**
* ErrorCode :ETC_FAILED
*
*/

public static final ErrorCode ETC_FAILED=new ErrorCode (M_ETC_FAILED);


/**
* ErrorCode :IMPROPER_CALLER_RESPONSE
*
*/

public static final ErrorCode IMPROPER_CALLER_RESPONSE=new ErrorCode (M_IMPROPER_CALLER_RESPONSE);


/**
* ErrorCode :MISSING_CUSTOMER_RECORD
*
*/

public static final ErrorCode MISSING_CUSTOMER_RECORD=new ErrorCode (M_MISSING_CUSTOMER_RECORD);


/**
* ErrorCode :MISSING_PARAMETER	
*
*/

public static final ErrorCode MISSING_PARAMETER=new ErrorCode (M_MISSING_PARAMETER);


/**
* ErrorCode :PARAMETER_OUT_OF_RANGE
*
*/

public static final ErrorCode PARAMETER_OUT_OF_RANGE=new ErrorCode (M_PARAMETER_OUT_OF_RANGE);


/**
* ErrorCode :REQUESTED_INFO_ERROR
*
*/

public static final ErrorCode REQUESTED_INFO_ERROR=new ErrorCode (M_REQUESTED_INFO_ERROR);


/**
* ErrorCode :SYSTEM_FAILURE
*
*/

public static final ErrorCode SYSTEM_FAILURE=new ErrorCode (M_SYSTEM_FAILURE);


/**
* ErrorCode :TASK_REFUSED
*
*/

public static final ErrorCode TASK_REFUSED=new ErrorCode (M_TASK_REFUSED);


/**
* ErrorCode :UNAVAILABLE_RESOURCE
*
*/

public static final ErrorCode UNAVAILABLE_RESOURCE=new ErrorCode (M_UNAVAILABLE_RESOURCE);


/**
* ErrorCode :UNEXPECTED_COMPONENT_SEQUENCE
*
*/

public static final ErrorCode UNEXPECTED_COMPONENT_SEQUENCE=new ErrorCode (M_UNEXPECTED_COMPONENT_SEQUENCE);


/**
* ErrorCode :UNEXPECTED_DATA_VALUE
*
*/

public static final ErrorCode UNEXPECTED_DATA_VALUE=new ErrorCode (M_UNEXPECTED_DATA_VALUE);


/**
* ErrorCode :UNEXPECTED_PARAMETER
*
*/

public static final ErrorCode UNEXPECTED_PARAMETER=new ErrorCode (M_UNEXPECTED_PARAMETER);


/**
* ErrorCode :UNKNOWN_LEGID
*
*/

public static final ErrorCode UNKNOWN_LEGID=new ErrorCode (M_UNKNOWN_LEGID);


/**
* ErrorCode :UNKNOWN_RESOURCE
*
*/

public static final ErrorCode UNKNOWN_RESOURCE=new ErrorCode (M_UNKNOWN_RESOURCE);


/**
* ErrorCode :SCF_REFERRAL
*
*/

public static final ErrorCode SCF_REFERRAL=new ErrorCode (M_SCF_REFERRAL);


/**
* ErrorCode :SCF_TASK_REFUSED
*
*/

public static final ErrorCode SCF_TASK_REFUSED=new ErrorCode (M_SCF_TASK_REFUSED);


/**
* ErrorCode :CHAINING_REFUSED
*
*/

public static final ErrorCode CHAINING_REFUSED=new ErrorCode (M_CHAINING_REFUSED);



	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getErrorCode()
	{
	
		return errorCode;
	}
	
}
// end class
