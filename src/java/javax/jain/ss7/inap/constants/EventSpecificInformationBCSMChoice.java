
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
 *  File Name     : EventSpecificInformationBCSMChoice.java
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
This class indicates the EventSpecificInformationBCSM choice
*/


public class EventSpecificInformationBCSMChoice {

    public static final int M_COLLECTED_INFO_SPECIFIC_INFO =0;
    public static final int M_ANALYSED_INFO_SPECIFIC_INFO =1;
    public static final int M_ROUTE_SELECT_FAILURE_SPECIFIC_INFO =2;
    public static final int M_O_CALLED_PARTY_BUSY_SPECIFIC_INFO =3;   
    public static final int M_O_NO_ANSWER_SPECIFIC_INFO =4;
    public static final int M_O_ANSWER_SPECIFIC_INFO =5;
	public static final int M_O_MID_CALL_SPECIFIC_INFO =6;
    public static final int M_O_DISCONNECT_SPECIFIC_INFO =7;
    public static final int M_T_BUSY_SPECIFIC_INFO =8;
    public static final int M_T_NO_ANSWER_SPECIFIC_INFO =9;
    public static final int M_T_ANSWER_SPECIFIC_INFO= 10;
    public static final int M_T_MID_CALL_SPECIFIC_INFO =11;   
    public static final int M_T_DISCONNECT_SPECIFIC_INFO =12;
    public static final int M_O_TERM_SEIZED_SPECIFIC_INFO =13;
	public static final int M_O_SUSPENDED =14;
    public static final int M_T_SUSPENDED =15;
	public static final int M_ORIG_ATTEMPT_AUTHORIZED =16;
    public static final int M_O_RE_ANSWER =17;
    public static final int M_T_RE_ANSWER =18;
    public static final int M_FACILITY_SELECTED_AND_AVAILABLE =19;   
    public static final int M_CALL_ACCEPTED =20;
    public static final int M_O_ABANDON =21;
	public static final int M_T_ABANDON =22;
    
	
	// internal variable to store the constant value
	
	private int eventSpecificInformationBCSMChoice;
	
/**
* constructor for EventSpecificInformationBCSMChoice 
*
*/

	private EventSpecificInformationBCSMChoice(int eventSpacificInformationBCSMChoice)
	{

		this.eventSpecificInformationBCSMChoice=eventSpecificInformationBCSMChoice;

	}
	
	
/**
* EventSpecificInformationBCSMChoice  :COLLECTED_INFO_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice COLLECTED_INFO_SPECIFIC_INFO  =new EventSpecificInformationBCSMChoice (M_COLLECTED_INFO_SPECIFIC_INFO);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :ANALYSED_INFO_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice ANALYSED_INFO_SPECIFIC_INFO  =new EventSpecificInformationBCSMChoice (M_ANALYSED_INFO_SPECIFIC_INFO);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :ROUTE_SELECT_FAILURE_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice ROUTE_SELECT_FAILURE_SPECIFIC_INFO  =new EventSpecificInformationBCSMChoice (M_ROUTE_SELECT_FAILURE_SPECIFIC_INFO);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :O_CALLED_PARTY_BUSY_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice O_CALLED_PARTY_BUSY_SPECIFIC_INFO =new EventSpecificInformationBCSMChoice (M_O_CALLED_PARTY_BUSY_SPECIFIC_INFO);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :O_NO_ANSWER_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice O_NO_ANSWER_SPECIFIC_INFO  =new EventSpecificInformationBCSMChoice ( M_O_NO_ANSWER_SPECIFIC_INFO );
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :O_ANSWER_SPECIFIC_INFO 
*
*/

public static final EventSpecificInformationBCSMChoice O_ANSWER_SPECIFIC_INFO =new EventSpecificInformationBCSMChoice (M_O_ANSWER_SPECIFIC_INFO );
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :O_MID_CALL_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice O_MID_CALL_SPECIFIC_INFO =new EventSpecificInformationBCSMChoice (M_O_MID_CALL_SPECIFIC_INFO);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :O_DISCONNECT_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice O_DISCONNECT_SPECIFIC_INFO =new EventSpecificInformationBCSMChoice (M_O_DISCONNECT_SPECIFIC_INFO);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :T_BUSY_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice T_BUSY_SPECIFIC_INFO  =new EventSpecificInformationBCSMChoice (M_T_BUSY_SPECIFIC_INFO);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :T_NO_ANSWER_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice T_NO_ANSWER_SPECIFIC_INFO  =new EventSpecificInformationBCSMChoice (M_T_NO_ANSWER_SPECIFIC_INFO);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :T_ANSWER_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice T_ANSWER_SPECIFIC_INFO  =new EventSpecificInformationBCSMChoice (M_T_ANSWER_SPECIFIC_INFO);

	
	
	
/**
* EventSpecificInformationBCSMChoice  :T_MID_CALL_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice T_MID_CALL_SPECIFIC_INFO  =new EventSpecificInformationBCSMChoice (M_T_MID_CALL_SPECIFIC_INFO);

	
	
	
	/**
* EventSpecificInformationBCSMChoice  :T_DISCONNECT_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice T_DISCONNECT_SPECIFIC_INFO  =new EventSpecificInformationBCSMChoice (M_T_DISCONNECT_SPECIFIC_INFO);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :O_TERM_SEIZED_SPECIFIC_INFO
*
*/

public static final EventSpecificInformationBCSMChoice O_TERM_SEIZED_SPECIFIC_INFO =new EventSpecificInformationBCSMChoice (M_O_TERM_SEIZED_SPECIFIC_INFO);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :O_SUSPENDED
*
*/

public static final EventSpecificInformationBCSMChoice O_SUSPENDED  =new EventSpecificInformationBCSMChoice (M_O_SUSPENDED);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :T_SUSPENDED
*
*/

public static final EventSpecificInformationBCSMChoice T_SUSPENDED  =new EventSpecificInformationBCSMChoice (M_T_SUSPENDED);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :ORIG_ATTEMPT_AUTHORIZED
*
*/

public static final EventSpecificInformationBCSMChoice ORIG_ATTEMPT_AUTHORIZED  =new EventSpecificInformationBCSMChoice (M_ORIG_ATTEMPT_AUTHORIZED);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :O_RE_ANSWER
*
*/

public static final EventSpecificInformationBCSMChoice O_RE_ANSWER  =new EventSpecificInformationBCSMChoice (M_O_RE_ANSWER);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :T_RE_ANSWER 
*
*/

public static final EventSpecificInformationBCSMChoice T_RE_ANSWER  =new EventSpecificInformationBCSMChoice (M_T_RE_ANSWER );
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :FACILITY_SELECTED_AND_AVAILABLE
*
*/

public static final EventSpecificInformationBCSMChoice FACILITY_SELECTED_AND_AVAILABLE  =new EventSpecificInformationBCSMChoice (M_FACILITY_SELECTED_AND_AVAILABLE);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :CALL_ACCEPTED
*
*/

public static final EventSpecificInformationBCSMChoice CALL_ACCEPTED  =new EventSpecificInformationBCSMChoice (M_CALL_ACCEPTED);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :O_ABANDON
*
*/

public static final EventSpecificInformationBCSMChoice O_ABANDON  =new EventSpecificInformationBCSMChoice (M_O_ABANDON);
	
	
	
	/**
* EventSpecificInformationBCSMChoice  :T_ABANDON
*
*/

public static final EventSpecificInformationBCSMChoice T_ABANDON  =new EventSpecificInformationBCSMChoice (M_T_ABANDON);
	
	
	
	
	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getEventSpecificInformationBCSMChoice()
	{
	
		return eventSpecificInformationBCSMChoice;
	
	}
		
	
} //end class

