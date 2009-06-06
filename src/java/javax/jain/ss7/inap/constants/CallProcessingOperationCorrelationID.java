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
 *  File Name     : CallProcessingOperationCorrelationID.java
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
This class Call Processing Operation Correlation ID indicates what message should be used to
deliver the specified UNI APDU with component. Default is FACILITY and in this case the APDU is delivered
during the BCSM suspended at a DP.
*/

public class CallProcessingOperationCorrelationID {

    public static final int M_ALERTING=1;
    public static final int M_SETUP=5;
    public static final int M_CONNECT=7;
    public static final int M_DISCONNECT=69;
    public static final int M_RELEASE=77;
    public static final int M_RELEASE_COMPLETE=90;
    public static final int M_FACILITY=98;
    
	
	// Internal variable to store Constant value
	
	private int callProcessingOperationCorrelationID;
	
	
	
	
/**
Constructor for CallProcessingOperationCorrelationID datatype
*/
    private CallProcessingOperationCorrelationID(int callProcessingOperationCorrelationID) 
	
	{
	
		this.callProcessingOperationCorrelationID=callProcessingOperationCorrelationID;
					
	 }  
	 
	 
	 /*
	 * callProcessingOperationCorrelationID:ALERTING
	 *
	 */
	 
	 public static final CallProcessingOperationCorrelationID ALERTING =new CallProcessingOperationCorrelationID (M_ALERTING);
	 
	 
	 
	 
	 /*
	 * callProcessingOperationCorrelationID:SETUP
	 *
	 */
	 
	 public static final CallProcessingOperationCorrelationID SETUP =new CallProcessingOperationCorrelationID (M_SETUP);
	 
	 
	 
	 
	 /*
	 * callProcessingOperationCorrelationID:CONNECT
	 *
	 */
	 
	 public static final CallProcessingOperationCorrelationID CONNECT =new CallProcessingOperationCorrelationID (M_CONNECT);
	 
	 
	 
	 /*
	 * callProcessingOperationCorrelationID:DISCONNECT
	 *
	 */
	 
	 public static final CallProcessingOperationCorrelationID DISCONNECT=new CallProcessingOperationCorrelationID (M_DISCONNECT);
	 
	 
	 
	 
	 /*
	 * callProcessingOperationCorrelationID:RELEASE
	 *
	 */
	 
	 public static final CallProcessingOperationCorrelationID RELEASE =new CallProcessingOperationCorrelationID (M_RELEASE);
	 
	 
	 
	 /*
	 * callProcessingOperationCorrelationID:RELEASE_COMPLETE
	 *
	 */
	 
	 public static final CallProcessingOperationCorrelationID RELEASE_COMPLETE =new CallProcessingOperationCorrelationID (M_RELEASE_COMPLETE);
	 
	 
	 
	 /*
	 * callProcessingOperationCorrelationID:FACILITY
	 *
	 */
	 
	 public static final CallProcessingOperationCorrelationID FACILITY =new CallProcessingOperationCorrelationID (M_FACILITY);
	 
	 /**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	 
	 	 	 
	 public int getCallProcessingOperationCorrelationID()
	 
	 {
	 
	 	return callProcessingOperationCorrelationID;
		
	 }
	 
	 
	   
}
