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
 *  File Name     : OperationCode.java
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
This class specifies the Operation Code of INAP operations.
*/

public class OperationCode {


	/**
	* Internal constant  available for switch statements 
	*/
	
	
	public static final int M_INITIAL_DP=0;
    public static final int M_ORIGINATION_ATTEMPT_AUTHORIZED=1;
    public static final int M_COLLECTED_INFORMATION=2;
    public static final int M_ANALYSED_INFORMATION=3;
    public static final int M_ROUTE_SELECT_FAILURE=4;
    public static final int M_O_CALLED_PARTY_BUSY=5;
    public static final int M_O_NO_ANSWER=6;
    public static final int M_O_ANSWER=7;
    public static final int M_O_DISCONNECT=8;
    public static final int M_TERM_ATTEMPT_AUTHORIZED=9;
    public static final int M_T_BUSY=10;
    public static final int M_T_NO_ANSWER=11;
    public static final int M_T_ANSWER=12;
    public static final int M_T_DISCONNECT=13;
    public static final int M_O_MID_CALL=14;
    public static final int M_T_MID_CALL=15;
    public static final int M_ASSIST_REQUEST_INSTRUCTIONS=16;
    public static final int M_ESTABLISH_TEMPORARY_CONNECTION=17;
    public static final int M_DISCONNECT_FORWARD_CONNECTION=18;
    public static final int M_CONNECT_TO_RESOURCE=19;
    public static final int M_CONNECT=20;
    public static final int M_HOLD_CALL_IN_NETWORK=21;
    public static final int M_RELEASE_CALL=22;
    public static final int M_REQUEST_REPORT_BCSM_EVENT=23;
    public static final int M_EVENT_REPORT_BCSM=24;
    public static final int M_REQUEST_NOTIFICATION_CHARGING_EVENT=25;
    public static final int M_EVENT_NOTIFICATION_CHARGING=26;
    public static final int M_COLLECT_INFORMATION=27;
    public static final int M_ANALYSE_INFORMATION=28;
    public static final int M_SELECT_ROUTE=29;
    public static final int M_SELECT_FACILITY=30;
    public static final int M_CONTINUE=31;
    public static final int M_INITIATE_CALL_ATTEMPT=32;
    public static final int M_RESET_TIMER=33;
    public static final int M_FURNISH_CHARGING_INFORMATION=34;
    public static final int M_APPLY_CHARGING=35;
    public static final int M_APPLY_CHARGING_REPORT=36;
    public static final int M_REQUEST_CURRENT_STATUS_REPORT=37;
    public static final int M_REQUEST_EVERY_STATUS_CHANGE_REPORT=38;
    public static final int M_REQUEST_FIRST_STATUS_MATCH_REPORT=39;
    public static final int M_STATUS_REPORT=40;
    public static final int M_CALL_GAP=41;
    public static final int M_ACTIVATE_SERVICE_FILTERING=42;
    public static final int M_SERVICE_FILTERING_RESPONSE=43;
    public static final int M_CALL_INFORMATION_REPORT=44;
    public static final int M_CALL_INFORMATION_REQUEST=45;
    public static final int M_SEND_CHARGING_INFORMATION=46;
    public static final int M_PLAY_ANNOUNCEMENT=47;
    public static final int M_PROMPT_AND_COLLECT_USER_INFORMATION=48;
    public static final int M_SPECIALIZED_RESOURCE_REPORT	=49;
    public static final int M_CANCEL=53;
    public static final int M_CANCEL_STATUS_REPORT_REQUEST=54;
    public static final int M_ACTIVITY_TEST	=55;
    public static final int M_FACILITY_SELECTED_AND_AVAILABLE=80;
    public static final int M_ORIGINATION_ATTEMPT=81;
    public static final int M_TERMINATION_ATTEMPT=82;
    public static final int M_O_ABANDON=83;
    public static final int M_O_SUSPENDED=84;
    public static final int M_T_SUSPENDED=85;
    public static final int M_DFC_WITH_ARGUMENT=86;
    public static final int M_AUTHORIZE_TERMINATION=87;
    public static final int M_CONTINUE_WITH_ARGUMENT=88;
    public static final int M_CREATE_CALL_SEGMENT_ASSOCIATION=89;
    public static final int M_DISCONNECT_LEG=90;
    public static final int M_MERGE_CALL_SEGMENTS=91;
    public static final int M_MOVE_CALL_SEGMENTS=92;
    public static final int M_MOVE_LEG=93;
    public static final int M_RECONNECT=94;
    public static final int M_SPLIT_LEG=95;
    public static final int M_ENTITY_RELEASED=96;
    public static final int M_MANAGE_TRIGGER_DATA=97;
    public static final int M_REQUEST_REPORT_UTSI=98;
    public static final int M_SEND_STUI=100;
    public static final int M_REPORT_UTSI=101;
    public static final int M_SEND_FACILITY_INFORMATION=102;
    public static final int M_REQUEST_REPORT_FACILITY_EVENT=103;
    public static final int M_EVENT_REPORT_FACILITY	=104;
    public static final int M_PROMPT_AND_RECEIVE_MESSAGE=107;
    
   
   
   // internal variable to store the constant value
   
   private int operationCode;
   
   	
/**
Constructor for OperationCode datatype
*/
    
   private OperationCode(int operationCode) 
   {
    	
		this.operationCode=operationCode;
	
   }
   
   
/**
*  OperationCode  :INITIAL_DP
*
*/

public static final OperationCode  INITIAL_DP = new OperationCode (M_INITIAL_DP);

   


   
/**
*  OperationCode  :ORIGINATION_ATTEMPT_AUTHORIZED
*
*/

public static final OperationCode  ORIGINATION_ATTEMPT_AUTHORIZED = new OperationCode (M_ORIGINATION_ATTEMPT_AUTHORIZED);


/**
*  OperationCode  :COLLECTED_INFORMATION
*
*/

public static final OperationCode  COLLECTED_INFORMATION = new OperationCode (M_COLLECTED_INFORMATION);

		
    	
    	/**
    	*  OperationCode  :ANALYSED_INFORMATION
    	*
    	*/
    	
    	public static final OperationCode   ANALYSED_INFORMATION= new OperationCode (M_ANALYSED_INFORMATION);

    	
    	/**
    	*  OperationCode  :ROUTE_SELECT_FAILURE
    	*
    	*/
    	
    	public static final OperationCode  ROUTE_SELECT_FAILURE = new OperationCode (M_ROUTE_SELECT_FAILURE);

    	
    	/**
    	*  OperationCode  :O_CALLED_PARTY_BUSY
    	*
    	*/
    	
    	public static final OperationCode  O_CALLED_PARTY_BUSY = new OperationCode (M_O_CALLED_PARTY_BUSY);

    	
    	/**
    	*  OperationCode  :O_NO_ANSWER
    	*
    	*/
    	
    	public static final OperationCode O_NO_ANSWER  = new OperationCode (M_O_NO_ANSWER);

 	
		
    	
    	/**
    	*  OperationCode  :O_ANSWER
    	*
    	*/
    	
    	public static final OperationCode  O_ANSWER = new OperationCode (M_O_ANSWER);

    	
    	/**
    	*  OperationCode  :O_DISCONNECT
    	*
    	*/
    	
    	public static final OperationCode  O_DISCONNECT = new OperationCode (M_O_DISCONNECT);

    	
    	/**
    	*  OperationCode  :TERM_ATTEMPT_AUTHORIZED
    	*
    	*/
    	
    	public static final OperationCode  TERM_ATTEMPT_AUTHORIZED = new OperationCode (M_TERM_ATTEMPT_AUTHORIZED);

    	
    	/**
    	*  OperationCode  :T_BUSY
    	*
    	*/
    	
    	public static final OperationCode  T_BUSY = new OperationCode (M_T_BUSY);
	
    	
    	/**
    	*  OperationCode  :T_NO_ANSWER
    	*
    	*/
    	
    	public static final OperationCode T_NO_ANSWER  = new OperationCode (M_T_NO_ANSWER);
	
    	
    	/**
    	*  OperationCode  :T_ANSWER
    	*
    	*/
    	
    	public static final OperationCode  T_ANSWER = new OperationCode (M_T_ANSWER);
	
    	
    	/**
    	*  OperationCode  :T_DISCONNECT
    	*
    	*/
    	
    	public static final OperationCode T_DISCONNECT  = new OperationCode (M_T_DISCONNECT);
		
    	
    	/**
    	*  OperationCode  :O_MID_CALL
    	*
    	*/
    	
    	public static final OperationCode   O_MID_CALL= new OperationCode (M_O_MID_CALL);
		
    	
    	/**
    	*  OperationCode  :T_MID_CALL
    	*
    	*/
    	
    	public static final OperationCode  T_MID_CALL = new OperationCode (M_T_MID_CALL);
		
 
    	
    	/**
    	*  OperationCode  :ASSIST_REQUEST_INSTRUCTIONS
    	*
    	*/
    	
    	public static final OperationCode  ASSIST_REQUEST_INSTRUCTIONS = new OperationCode (M_ASSIST_REQUEST_INSTRUCTIONS);
		
    	
    	/**
    	*  OperationCode  :ESTABLISH_TEMPORARY_CONNECTION
    	*
    	*/
    	
    	public static final OperationCode  ESTABLISH_TEMPORARY_CONNECTION = new OperationCode (M_ESTABLISH_TEMPORARY_CONNECTION);
		
    	
    	/**
    	*  OperationCode  :DISCONNECT_FORWARD_CONNECTION
    	*
    	*/
    	
    	public static final OperationCode  DISCONNECT_FORWARD_CONNECTION = new OperationCode (M_DISCONNECT_FORWARD_CONNECTION);
		
		
    	
    	/**
    	*  OperationCode  :CONNECT_TO_RESOURCE
    	*
    	*/
    	
    	public static final OperationCode CONNECT_TO_RESOURCE  = new OperationCode (M_CONNECT_TO_RESOURCE);
		
    	
    	/**
    	*  OperationCode  :CONNECT
    	*
    	*/
    	
    	public static final OperationCode  CONNECT = new OperationCode (M_CONNECT);
		
    	
    	/**
    	*  OperationCode  :HOLD_CALL_IN_NETWORK
    	*
    	*/
    	
    	public static final OperationCode  HOLD_CALL_IN_NETWORK = new OperationCode (M_HOLD_CALL_IN_NETWORK);
		
    	
    	/**
    	*  OperationCode  :RELEASE_CALL
    	*
    	*/
    	
    	public static final OperationCode  RELEASE_CALL = new OperationCode (M_RELEASE_CALL);
		
    	
    	/**
    	*  OperationCode  :REQUEST_REPORT_BCSM_EVENT
    	*
    	*/
    	
    	public static final OperationCode  REQUEST_REPORT_BCSM_EVENT = new OperationCode (M_REQUEST_REPORT_BCSM_EVENT);
		
    	
    	/**
    	*  OperationCode  :EVENT_REPORT_BCSM
    	*
    	*/
    	
    	public static final OperationCode EVENT_REPORT_BCSM  = new OperationCode (M_EVENT_REPORT_BCSM);
	
		
    	
    	/**
    	*  OperationCode  :REQUEST_NOTIFICATION_CHARGING_EVENT
    	*
    	*/
    	
    	public static final OperationCode  REQUEST_NOTIFICATION_CHARGING_EVENT = new OperationCode (M_REQUEST_NOTIFICATION_CHARGING_EVENT);
		
    	
    	/**
    	*  OperationCode  :EVENT_NOTIFICATION_CHARGING
    	*
    	*/
    	
    	public static final OperationCode EVENT_NOTIFICATION_CHARGING  = new OperationCode (M_EVENT_NOTIFICATION_CHARGING);
		
    	
    	/**
    	*  OperationCode  :COLLECT_INFORMATION
    	*
    	*/
    	
    	public static final OperationCode COLLECT_INFORMATION  = new OperationCode (M_COLLECT_INFORMATION);
		
    	
    	/**
    	*  OperationCode  :ANALYSE_INFORMATION
    	*
    	*/
    	
    	public static final OperationCode ANALYSE_INFORMATION  = new OperationCode (M_ANALYSE_INFORMATION);
		
    	
    	/**
    	*  OperationCode  :SELECT_ROUTE
    	*
    	*/
    	
    	public static final OperationCode SELECT_ROUTE  = new OperationCode (M_SELECT_ROUTE);
		
    	
    	/**
    	*  OperationCode  :SELECT_FACILITY
    	*
    	*/
    	
    	public static final OperationCode  SELECT_FACILITY = new OperationCode (M_SELECT_FACILITY);

    	
    	/**
    	*  OperationCode  :CONTINUE
    	*
    	*/
    	
    	public static final OperationCode CONTINUE  = new OperationCode (M_CONTINUE);
		
    	
    	/**
    	*  OperationCode  :INITIATE_CALL_ATTEMPT
    	*
    	*/
    	
    	public static final OperationCode INITIATE_CALL_ATTEMPT  = new OperationCode (M_INITIATE_CALL_ATTEMPT);
		
    	
    	/**
    	*  OperationCode  :RESET_TIMER
    	*
    	*/
    	
    	public static final OperationCode RESET_TIMER  = new OperationCode (M_RESET_TIMER);
		
    	
    	/**
    	*  OperationCode  :FURNISH_CHARGING_INFORMATION
    	*
    	*/
    	
    	public static final OperationCode  FURNISH_CHARGING_INFORMATION = new OperationCode (M_FURNISH_CHARGING_INFORMATION);
		
    	
    	/**
    	*  OperationCode  :APPLY_CHARGING
    	*
    	*/
    	
    	public static final OperationCode APPLY_CHARGING  = new OperationCode (M_APPLY_CHARGING);
		
    	
    	/**
    	*  OperationCode  :APPLY_CHARGING_REPORT
    	*
    	*/
    	
    	public static final OperationCode  APPLY_CHARGING_REPORT = new OperationCode (M_APPLY_CHARGING_REPORT);
		
    	
    	/**
    	*  OperationCode  :REQUEST_CURRENT_STATUS_REPORT
    	*
    	*/
    	
    	public static final OperationCode  REQUEST_CURRENT_STATUS_REPORT = new OperationCode (M_REQUEST_CURRENT_STATUS_REPORT);
		
    	
    	/**
    	*  OperationCode  :REQUEST_EVERY_STATUS_CHANGE_REPORT
    	*
    	*/
    	
    	public static final OperationCode  REQUEST_EVERY_STATUS_CHANGE_REPORT = new OperationCode (M_REQUEST_EVERY_STATUS_CHANGE_REPORT);
		
    	
    	/**
    	*  OperationCode  :REQUEST_FIRST_STATUS_MATCH_REPORT
    	*
    	*/
    	
    	public static final OperationCode  REQUEST_FIRST_STATUS_MATCH_REPORT = new OperationCode (M_REQUEST_FIRST_STATUS_MATCH_REPORT);
		
    	
    	/**
    	*  OperationCode  :STATUS_REPORT
    	*
    	*/
    	
    	public static final OperationCode  STATUS_REPORT = new OperationCode (M_STATUS_REPORT);
		
    	
    	/**
    	*  OperationCode  :CALL_GAP
    	*
    	*/
    	
    	public static final OperationCode  CALL_GAP = new OperationCode (M_CALL_GAP);
		
    	
    	/**
    	*  OperationCode  :ACTIVATE_SERVICE_FILTERING
    	*
    	*/
    	
    	public static final OperationCode  ACTIVATE_SERVICE_FILTERING = new OperationCode (M_ACTIVATE_SERVICE_FILTERING);
		
    	
    	/**
    	*  OperationCode  :SERVICE_FILTERING_RESPONSE
    	*
    	*/
    	
    	public static final OperationCode SERVICE_FILTERING_RESPONSE  = new OperationCode (M_SERVICE_FILTERING_RESPONSE);
		
    	
    	/**
    	*  OperationCode  :CALL_INFORMATION_REPORT
    	*
    	*/
    	
    	public static final OperationCode CALL_INFORMATION_REPORT  = new OperationCode (M_CALL_INFORMATION_REPORT);
		
    	
    	/**
    	*  OperationCode  :CALL_INFORMATION_REQUEST
    	*
    	*/
    	
    	public static final OperationCode CALL_INFORMATION_REQUEST  = new OperationCode (M_CALL_INFORMATION_REQUEST);
		
    	
    	/**
    	*  OperationCode  :SEND_CHARGING_INFORMATION
    	*
    	*/
    	
    	public static final OperationCode  SEND_CHARGING_INFORMATION = new OperationCode (M_SEND_CHARGING_INFORMATION);
		
    	
    	/**
    	*  OperationCode  :PLAY_ANNOUNCEMENT
    	*
    	*/
    	
    	public static final OperationCode   PLAY_ANNOUNCEMENT= new OperationCode (M_PLAY_ANNOUNCEMENT);
		
    	
    	/**
    	*  OperationCode  :PROMPT_AND_COLLECT_USER_INFORMATION
    	*
    	*/
    	
    	public static final OperationCode  PROMPT_AND_COLLECT_USER_INFORMATION = new OperationCode (M_PROMPT_AND_COLLECT_USER_INFORMATION);
		
    	
    	/**
    	*  OperationCode  :SPECIALIZED_RESOURCE_REPORT
    	*
    	*/
    	
    	public static final OperationCode SPECIALIZED_RESOURCE_REPORT  = new OperationCode (M_SPECIALIZED_RESOURCE_REPORT);
    	
    	/**
    	*  OperationCode  :CANCEL
    	*
    	*/
    	
    	public static final OperationCode  CANCEL = new OperationCode (M_CANCEL);
		
    	
    	/**
    	*  OperationCode  :CANCEL_STATUS_REPORT_REQUEST
    	*
    	*/
    	
    	public static final OperationCode  CANCEL_STATUS_REPORT_REQUEST = new OperationCode (M_CANCEL_STATUS_REPORT_REQUEST);
		
    	
    	/**
    	*  OperationCode  :ACTIVITY_TEST
    	*
    	*/
    	
    	public static final OperationCode ACTIVITY_TEST  = new OperationCode (M_ACTIVITY_TEST);

    	
    	/**
    	*  OperationCode  :FACILITY_SELECTED_AND_AVAILABLE
    	*
    	*/
    	
    	public static final OperationCode FACILITY_SELECTED_AND_AVAILABLE  = new OperationCode (M_FACILITY_SELECTED_AND_AVAILABLE);
		
    	
    	/**
    	*  OperationCode  :ORIGINATION_ATTEMPT
    	*
    	*/
    	
    	public static final OperationCode ORIGINATION_ATTEMPT   = new OperationCode (M_ORIGINATION_ATTEMPT);
		
    	
    	/**
    	*  OperationCode  :TERMINATION_ATTEMPT
    	*
    	*/
    	
    	public static final OperationCode  TERMINATION_ATTEMPT = new OperationCode (M_TERMINATION_ATTEMPT);
		
	
    	/**
    	*  OperationCode  :O_ABANDON
    	*
    	*/
    	
    	public static final OperationCode O_ABANDON  = new OperationCode (M_O_ABANDON);
		
    	
    	/**
    	*  OperationCode  :O_SUSPENDED
    	*
    	*/
    	
    	public static final OperationCode   O_SUSPENDED= new OperationCode (M_O_SUSPENDED);
		
    	
    	/**
    	*  OperationCode  :T_SUSPENDED
    	*
    	*/
    	
    	public static final OperationCode T_SUSPENDED  = new OperationCode (M_T_SUSPENDED);
		
    	
    	/**
    	*  OperationCode  :DFC_WITH_ARGUMENT
    	*
    	*/
    	
    	public static final OperationCode  DFC_WITH_ARGUMENT = new OperationCode (M_DFC_WITH_ARGUMENT);
		
    	
    	/**
    	*  OperationCode  :AUTHORIZE_TERMINATION
    	*
    	*/
    	
    	public static final OperationCode   AUTHORIZE_TERMINATION= new OperationCode (M_AUTHORIZE_TERMINATION);
		
    	
    	/**
    	*  OperationCode  :CONTINUE_WITH_ARGUMENT
    	*
    	*/
    	
    	public static final OperationCode  CONTINUE_WITH_ARGUMENT = new OperationCode (M_CONTINUE_WITH_ARGUMENT);
		

    	
    	/**
    	*  OperationCode  :CREATE_CALL_SEGMENT_ASSOCIATION
    	*
    	*/
    	
    	public static final OperationCode CREATE_CALL_SEGMENT_ASSOCIATION  = new OperationCode (M_CREATE_CALL_SEGMENT_ASSOCIATION);
		
    	
    	/**
    	*  OperationCode  :DISCONNECT_LEG
    	*
    	*/
    	
    	public static final OperationCode DISCONNECT_LEG  = new OperationCode (M_DISCONNECT_LEG);
		
    	
    	/**
    	*  OperationCode  :MERGE_CALL_SEGMENTS
    	*
    	*/
    	
    	public static final OperationCode MERGE_CALL_SEGMENTS  = new OperationCode (M_MERGE_CALL_SEGMENTS);
		
		
     	
    	/**
    	*  OperationCode  :MOVE_CALL_SEGMENTS
    	*
    	*/
    	
    	public static final OperationCode  MOVE_CALL_SEGMENTS = new OperationCode (M_MOVE_CALL_SEGMENTS);
		
    	
    	/**
    	*  OperationCode  :MOVE_LEG
    	*
    	*/
    	
    	public static final OperationCode MOVE_LEG  = new OperationCode (M_MOVE_LEG);
		
    	
    	/**
    	*  OperationCode  :RECONNECT
    	*
    	*/
    	
    	public static final OperationCode  RECONNECT = new OperationCode (M_RECONNECT);
		
		
    	/**
    	*  OperationCode  :SPLIT_LEG
    	*
    	*/
    	
    	public static final OperationCode  SPLIT_LEG = new OperationCode (M_SPLIT_LEG);
		
    	
    	/**
    	*  OperationCode  :ENTITY_RELEASED
    	*
    	*/
    	
    	public static final OperationCode ENTITY_RELEASED  = new OperationCode (M_ENTITY_RELEASED);
		
    	
    	/**
    	*  OperationCode  :MANAGE_TRIGGER_DATA
    	*
    	*/
    	
    	public static final OperationCode  MANAGE_TRIGGER_DATA = new OperationCode (M_MANAGE_TRIGGER_DATA);

    	/**
    	*  OperationCode  :REQUEST_REPORT_UTSI
    	*
    	*/
    	
    	public static final OperationCode  REQUEST_REPORT_UTSI = new OperationCode (M_REQUEST_REPORT_UTSI);
		
    	
    	/**
    	*  OperationCode  :SEND_STUI
    	*
    	*/
    	
    	public static final OperationCode  SEND_STUI = new OperationCode (M_SEND_STUI);
		
    	
    	/**
    	*  OperationCode  :REPORT_UTSI
    	*
    	*/
    	
    	public static final OperationCode REPORT_UTSI  = new OperationCode (M_REPORT_UTSI);
		
      	
    	/**
    	*  OperationCode  :SEND_FACILITY_INFORMATION
    	*
    	*/
    	
    	public static final OperationCode SEND_FACILITY_INFORMATION  = new OperationCode (M_SEND_FACILITY_INFORMATION);
		
    	
    	/**
    	*  OperationCode  :REQUEST_REPORT_FACILITY_EVENT
    	*
    	*/
    	
    	public static final OperationCode  REQUEST_REPORT_FACILITY_EVENT = new OperationCode (M_REQUEST_REPORT_FACILITY_EVENT);
		
    	
    	/**
    	*  OperationCode  :EVENT_REPORT_FACILITY
    	*
    	*/
    	
    	public static final OperationCode EVENT_REPORT_FACILITY  = new OperationCode (M_EVENT_REPORT_FACILITY);
		
		
    	
    	/**
    	*  OperationCode  :PROMPT_AND_RECEIVE_MESSAGE
    	*
    	*/
    	
    	public static final OperationCode PROMPT_AND_RECEIVE_MESSAGE  = new OperationCode (M_PROMPT_AND_RECEIVE_MESSAGE);
		
 
 
 
 
 
 
 
 
 
 	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getOperationCode()
	{
	
		return operationCode;
		
	}    

}// end class 
