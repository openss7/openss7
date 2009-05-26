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
 *  File Name     : RejectType.java
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
This class indicates the type of Return Reject.*/

public class RejectType  {


	/**
	* Internal Constant for switch statements
	*/
	
	
    public static final int M_GENERAL_PROBLEM=1;
    public static final int M_INVOKE_PROBLEM=2;
    public static final int M_RETURN_RESULT_PROBLEM=3;
    public static final int M_RETURN_ERROR_PROBLEM=4;
    
	// Internal variable to store the constant value 
	
	private int rejectType;
	
	

/**
Constructor for RejectType datatype
*/
    
    private RejectType(int rejectType ) 
	{
	
	
	    this.rejectType= rejectType ;
		
	}
	
	
	/**
	* RejectType  :GENERAL_PROBLEM
	*
	*/
	
	public static final  RejectType GENERAL_PROBLEM= new RejectType (M_GENERAL_PROBLEM);
	
	
	/**
	* RejectType  :INVOKE_PROBLEM
	*
	*/
	
	public static final  RejectType INVOKE_PROBLEM= new RejectType (M_INVOKE_PROBLEM);
	
	
	
	/**
	* RejectType  :RETURN_RESULT_PROBLEM
	*
	*/
	
	public static final  RejectType RETURN_RESULT_PROBLEM= new RejectType (M_RETURN_RESULT_PROBLEM);
	
	
	
	/**
	* RejectType  :RETURN_ERROR_PROBLEM
	*
	*/
	
	public static final  RejectType RETURN_ERROR_PROBLEM= new RejectType (M_RETURN_ERROR_PROBLEM);
	
	
    
		/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getRejectType ()
	{
	
		return rejectType ;
	
	}
	
}// end class
