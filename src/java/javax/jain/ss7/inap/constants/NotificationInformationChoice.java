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
 *  File Name     : NotificationInformationChoice.java
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
This class indicates the NotificationInformation choice
*/


public class NotificationInformationChoice {

	/*
	* Internal constant available for switch statements 
	*/
	
    public static final int M_USER_ABANDON_SPECIFIC_INFO =0;
    public static final int M_CALL_FAILURE_SPECIFIC_INFO =1;
    public static final int M_NO_REPLY_SPECIFIC_INFO =2;
    public static final int M_CALL_RELEASE_SPECIFIC_INFO=3;
    public static final int M_SS_INVOCATION_SPECIFIC_INFO =4;
    public static final int M_CREDIT_LIMIT_REACHED_SPECIFIC_INFO =5;
    public static final int M_CALL_DURATION_SPECIFIC_INFO =6;
    public static final int M_CALLED_NUMBER_SPECIFIC_INFO=7;
	public static final int M_ANSWERED_CALL_SPECIFIC_INFO=8;


	// internal variable to store the constant value 
	
	
	private int notificationInformationChoice;
	
/*
* Constructor for NotificationInformationChoice data type 
*
*/

	private NotificationInformationChoice ( int notificationInformationChoice)
	{
		this.notificationInformationChoice=notificationInformationChoice;
	
	}

/*
* NotificationInformationChoice  : USER_ABANDON_SPECIFIC_INFO
*
*/


public static final NotificationInformationChoice   USER_ABANDON_SPECIFIC_INFO = new NotificationInformationChoice (M_USER_ABANDON_SPECIFIC_INFO);


/*
* NotificationInformationChoice  :CALL_FAILURE_SPECIFIC_INFO 
*
*/


public static final NotificationInformationChoice  CALL_FAILURE_SPECIFIC_INFO   = new NotificationInformationChoice (M_CALL_FAILURE_SPECIFIC_INFO );

/*
* NotificationInformationChoice  :NO_REPLY_SPECIFIC_INFO
*
*/


public static final NotificationInformationChoice  NO_REPLY_SPECIFIC_INFO  = new NotificationInformationChoice (M_NO_REPLY_SPECIFIC_INFO);


/*
* NotificationInformationChoice  :CALL_RELEASE_SPECIFIC_INFO
*
*/


public static final NotificationInformationChoice  CALL_RELEASE_SPECIFIC_INFO  = new NotificationInformationChoice (M_CALL_RELEASE_SPECIFIC_INFO);


/*
* NotificationInformationChoice  :SS_INVOCATION_SPECIFIC_INFO
*
*/


public static final NotificationInformationChoice    SS_INVOCATION_SPECIFIC_INFO= new NotificationInformationChoice (M_SS_INVOCATION_SPECIFIC_INFO);


/*
* NotificationInformationChoice  :CREDIT_LIMIT_REACHED_SPECIFIC_INFO 
*
*/


public static final NotificationInformationChoice  CREDIT_LIMIT_REACHED_SPECIFIC_INFO   = new NotificationInformationChoice (M_CREDIT_LIMIT_REACHED_SPECIFIC_INFO );


/*
* NotificationInformationChoice  :CALL_DURATION_SPECIFIC_INFO
*
*/


public static final NotificationInformationChoice   CALL_DURATION_SPECIFIC_INFO = new NotificationInformationChoice (M_CALL_DURATION_SPECIFIC_INFO);


/*
* NotificationInformationChoice  :CALLED_NUMBER_SPECIFIC_INFO
*
*/


public static final NotificationInformationChoice CALLED_NUMBER_SPECIFIC_INFO   = new NotificationInformationChoice (M_CALLED_NUMBER_SPECIFIC_INFO);


/*
* NotificationInformationChoice  :ANSWERED_CALL_SPECIFIC_INFO
*
*/


public static final NotificationInformationChoice  ANSWERED_CALL_SPECIFIC_INFO  = new NotificationInformationChoice (M_ANSWERED_CALL_SPECIFIC_INFO);





	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	
	public int getNotificationInformationChoice()
	{
		return  notificationInformationChoice;
	}
	
		
}
