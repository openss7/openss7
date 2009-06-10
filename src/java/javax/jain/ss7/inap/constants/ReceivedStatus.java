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
 *  File Name     : ReceivedStatus.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*; 
import java.util.*;
/**
This class indicates the result of the recording.
MessageComplete – the recording was successfully completed.
MessageInterrupted – the user has abandoned the recording    
MessageTimeOut – the maximum recording time has been exceeded.

*/

public class ReceivedStatus  {

    
	
	
	/**
	*  Internal Constant for switch statements 
	*/
		
	public static final int M_MESSAGE_COMPLETE=0;
    public static final int M_MESSAGE_INTERRUPTED=1;
    public static final int M_MESSAGE_TIMEOUT=2;
    
	// Internal variable to store the constant value 
	
	private int receivedStatus;
	
	
	
/**
Constructor for Received Status datatype
*/
    
    private ReceivedStatus(int receivedStatus ) 
	{
    	
		this.receivedStatus=receivedStatus ;
		
	}
	
	/**
	* ReceivedStatus :MESSAGE_COMPLETE
	*
	*/
	
	public static final ReceivedStatus  MESSAGE_COMPLETE= new ReceivedStatus (M_MESSAGE_COMPLETE);
	
	
	
	/**
	* ReceivedStatus :MESSAGE_INTERRUPTED
	*
	*/
	
	public static final ReceivedStatus  MESSAGE_INTERRUPTED= new ReceivedStatus (M_MESSAGE_INTERRUPTED);
	
	
	
	/**
	* ReceivedStatus :MESSAGE_TIMEOUT
	*
	*/
	
	public static final ReceivedStatus MESSAGE_TIMEOUT = new ReceivedStatus (M_MESSAGE_TIMEOUT);
	
	
	
		/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	
	public int getReceivedStatus ()
	{
	
		return receivedStatus ;
		
	}
	
		   
}
