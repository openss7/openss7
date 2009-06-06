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
 *  File Name     : RequestedInformationType.java
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
The class Requested Information Type specifies a list of specific items of 
information which is requested.*/


public class RequestedInformationType  {

/*
* Internal constants for switch statements 
*/


	public static final int M_CALL_ATTEMPT_ELAPSED_TIME=0;
	public static final int M_CALL_STOP_TIME =1;
	public static final int M_CALL_CONNECTED_ELAPSED_TIME =2;
	public static final int M_CALLED_ADDRESS =3;
	public static final int M_RELEASE_CAUSE = 30;

	// internal variable to store the constant value 
	
	private int requestedInformationType;
	
	
/**
Constructor for RequestedInformationType datatype
*/
    
    private RequestedInformationType(int requestedInformationType )
	{
    	
		this.requestedInformationType =requestedInformationType;
	
	}
	
	/*
	* RequestedInformationType  :CALL_ATTEMPT_ELAPSED_TIME 
	*
	*/
	
	public static final RequestedInformationType CALL_ATTEMPT_ELAPSED_TIME = new RequestedInformationType (M_CALL_ATTEMPT_ELAPSED_TIME);
	
	
	
	/*
	* RequestedInformationType  : CALL_STOP_TIME
	*
	*/
	
	public static final RequestedInformationType CALL_STOP_TIME = new RequestedInformationType (M_CALL_STOP_TIME);
	
	

	/*
	* RequestedInformationType  : CALL_CONNECTED_ELAPSED_TIME
	*
	*/
	
	public static final RequestedInformationType CALL_CONNECTED_ELAPSED_TIME = new RequestedInformationType (M_CALL_CONNECTED_ELAPSED_TIME);
	
	

	/*
	* RequestedInformationType  : CALLED_ADDRESS
	*
	*/
	
	public static final RequestedInformationType CALLED_ADDRESS = new RequestedInformationType (M_CALLED_ADDRESS);
	
	

	/*
	* RequestedInformationType  : RELEASE_CAUSE
	*
	*/
	
	public static final RequestedInformationType RELEASE_CAUSE = new RequestedInformationType (M_RELEASE_CAUSE);
	



	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getRequestedInformationType ()
	{
	
		return requestedInformationType ;
		
	}
	
}

