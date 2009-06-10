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
 *  File Name     : ElapsedTimeValueID.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*;
import java.util.*;

/**
This class indicates the type of the call elapsed time value (attempt / connect ).
CALL_ATTEMPT_ELAPSED_TIME_VALUE: This parameter indicates the duration between the end of INAP processing 
of operations initiating call setup ("Connect", "AnalyseInformation", "CollectInformation", "Continue" and "SelectRoute") 
and the received answer indication from the indicated or default called party side.

CALL_CONNECTED_ELAPSED_TIME_VALUE: This parameter indicates the duration between the received answer indication
 from the indicated or default called party side and the release of that connection.

*/

public class ElapsedTimeValueID {

    public static final int M_CALL_ATTEMPT_ELAPSED_TIME_VALUE=0;
    public static final int M_CALL_CONNECTED_ELAPSED_TIME_VALUE=1;
    

// internal variable to store the constant value 

private int elapsedTimeValueID;


/**
Constructor for ElapsedTimeValueID datatype
*/
    
	private ElapsedTimeValueID(int elapsedTimeValueID) 
	{
		this.elapsedTimeValueID=elapsedTimeValueID;
	
	}
    
/**
* ElapsedTimeValueID : CALL_ATTEMPT_ELAPSED_TIME_VALUE
*
*/

public static final ElapsedTimeValueID CALL_ATTEMPT_ELAPSED_TIME_VALUE=new ElapsedTimeValueID(M_CALL_ATTEMPT_ELAPSED_TIME_VALUE);


/**
* ElapsedTimeValueID : CALL_CONNECTED_ELAPSED_TIME_VALUE
*
*/

public static final ElapsedTimeValueID CALL_CONNECTED_ELAPSED_TIME_VALUE=new ElapsedTimeValueID(M_CALL_CONNECTED_ELAPSED_TIME_VALUE);




	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getElapsedTimeValueID()
	{
		return elapsedTimeValueID;
	
	}
	
}




