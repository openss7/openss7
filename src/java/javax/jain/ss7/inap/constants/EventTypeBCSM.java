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
 *  File Name     : EventTypeBCSM.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*; 
import java.util.*;
/**
This class indicates the BCSM detection point event.
*/

public class EventTypeBCSM  {

    public static final int M_ORIG_ATTEMPT_AUTHORIZED=1;
    public static final int M_COLLECTED_INFO=2;
    public static final int M_ANALYSED_INFORMATION=3;
    public static final int M_ROUTE_SELECT_FAILURE=4;
    public static final int M_O_CALLED_PARTY_BUSY=5;
    public static final int M_O_NO_ANSWER=6;
    public static final int M_O_ANSWER=7;
    public static final int M_O_MID_CALL=8;
    public static final int M_O_DISCONNECT=9;
    public static final int M_O_ABANDON=10;
    public static final int M_TERM_ATTEMPT_AUTHORIZED=12;
    public static final int M_T_BUSY=13;
    public static final int M_T_NO_ANSWER=14;
    public static final int M_T_ANSWER=15;
    public static final int M_T_MID_CALL=16;
    public static final int M_T_DISCONNECT=17;
    public static final int M_T_ABANDON=18;
    public static final int M_O_TERM_SEIZED=19;
    public static final int M_O_SUSPENDED=20;
    public static final int M_T_SUSPENDED=21;
    public static final int M_ORIG_ATTEMPT=22;
    public static final int M_TERM_ATTEMPT=23;
    public static final int M_O_RE_ANSWER=24;
    public static final int M_T_RE_ANSWER=25;
    public static final int M_FACILITY_SELECTED_AND_AVAILABLE=26;
    public static final int M_CALL_ACCPETED=27;



// internal variable to store the constant value

	private int eventTypeBCSM;
	
	    
/**
Constructor for EventTypeBCSM datatype
*/
    
    private EventTypeBCSM(int enventTypeBCSM) 
	{
    	this.eventTypeBCSM=eventTypeBCSM;
		
	}
		

/**
* EventTypeBCSM : ORIG_ATTEMPT_AUTHORIZED
*
*/

public static final EventTypeBCSM  ORIG_ATTEMPT_AUTHORIZED = new EventTypeBCSM (M_ORIG_ATTEMPT_AUTHORIZED);



/**
* EventTypeBCSM :COLLECTED_INFO
*
*/

public static final EventTypeBCSM  COLLECTED_INFO = new EventTypeBCSM (M_COLLECTED_INFO);


/**
* EventTypeBCSM :ANALYSED_INFORMATION
*
*/

public static final EventTypeBCSM ANALYSED_INFORMATION  = new EventTypeBCSM (M_ANALYSED_INFORMATION);

/**
* EventTypeBCSM :ROUTE_SELECT_FAILURE
*
*/

public static final EventTypeBCSM  ROUTE_SELECT_FAILURE = new EventTypeBCSM (M_ROUTE_SELECT_FAILURE);

/**
* EventTypeBCSM :O_CALLED_PARTY_BUSY
*
*/

public static final EventTypeBCSM  O_CALLED_PARTY_BUSY = new EventTypeBCSM (M_O_CALLED_PARTY_BUSY);


/**
* EventTypeBCSM :O_NO_ANSWER
*
*/

public static final EventTypeBCSM  O_NO_ANSWER = new EventTypeBCSM (M_O_NO_ANSWER);

/**
* EventTypeBCSM :O_ANSWER
*
*/

public static final EventTypeBCSM  O_ANSWER = new EventTypeBCSM (M_O_ANSWER);

/**
* EventTypeBCSM :O_MID_CALL
*
*/

public static final EventTypeBCSM O_MID_CALL  = new EventTypeBCSM (M_O_MID_CALL);

/**
* EventTypeBCSM :O_DISCONNECT
*
*/

public static final EventTypeBCSM O_DISCONNECT  = new EventTypeBCSM (M_O_DISCONNECT);

/**
* EventTypeBCSM :O_ABANDON
*
*/

public static final EventTypeBCSM O_ABANDON  = new EventTypeBCSM (M_O_ABANDON);


/**
* EventTypeBCSM :TERM_ATTEMPT_AUTHORIZED
*
*/

public static final EventTypeBCSM  TERM_ATTEMPT_AUTHORIZED = new EventTypeBCSM (M_TERM_ATTEMPT_AUTHORIZED);

/**
* EventTypeBCSM :T_BUSY
*
*/

public static final EventTypeBCSM  T_BUSY = new EventTypeBCSM (M_T_BUSY);

/**
* EventTypeBCSM :T_NO_ANSWER
*
*/

public static final EventTypeBCSM  T_NO_ANSWER = new EventTypeBCSM (M_T_NO_ANSWER);

/**
* EventTypeBCSM :T_ANSWER
*
*/

public static final EventTypeBCSM  T_ANSWER = new EventTypeBCSM (M_T_ANSWER);

/**
* EventTypeBCSM :T_MID_CALL
*
*/

public static final EventTypeBCSM T_MID_CALL  = new EventTypeBCSM (M_T_MID_CALL);

/**
* EventTypeBCSM :T_DISCONNECT
*
*/

public static final EventTypeBCSM  T_DISCONNECT= new EventTypeBCSM (M_T_DISCONNECT);

 
   
   

/**
* EventTypeBCSM :T_ABANDON
*
*/

public static final EventTypeBCSM  T_ABANDON = new EventTypeBCSM (M_T_ABANDON);

/**
* EventTypeBCSM :O_TERM_SEIZED
*
*/

public static final EventTypeBCSM O_TERM_SEIZED  = new EventTypeBCSM (M_O_TERM_SEIZED);

/**
* EventTypeBCSM :O_SUSPENDED
*
*/

public static final EventTypeBCSM  O_SUSPENDED = new EventTypeBCSM (M_O_SUSPENDED);

/**
* EventTypeBCSM :T_SUSPENDED
*
*/

public static final EventTypeBCSM  T_SUSPENDED = new EventTypeBCSM (M_T_SUSPENDED);

/**
* EventTypeBCSM :ORIG_ATTEMPT
*
*/

public static final EventTypeBCSM  ORIG_ATTEMPT = new EventTypeBCSM (M_ORIG_ATTEMPT);

/**
* EventTypeBCSM :TERM_ATTEMPT
*
*/

public static final EventTypeBCSM   TERM_ATTEMPT= new EventTypeBCSM (M_TERM_ATTEMPT);

/**
* EventTypeBCSM :O_RE_ANSWER
*
*/

public static final EventTypeBCSM  O_RE_ANSWER = new EventTypeBCSM (M_O_RE_ANSWER);

/**
* EventTypeBCSM :T_RE_ANSWER
*
*/

public static final EventTypeBCSM T_RE_ANSWER  = new EventTypeBCSM (M_T_RE_ANSWER);

/**
* EventTypeBCSM :FACILITY_SELECTED_AND_AVAILABLE
*
*/

public static final EventTypeBCSM  FACILITY_SELECTED_AND_AVAILABLE = new EventTypeBCSM (M_FACILITY_SELECTED_AND_AVAILABLE);

/**
* EventTypeBCSM :CALL_ACCPETED
*
*/

public static final EventTypeBCSM CALL_ACCPETED  = new EventTypeBCSM (M_CALL_ACCPETED);




	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getEventTypeBCSM()
	{
	
		return eventTypeBCSM;
	}
	
   
}
