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
 *  File Name     : TriggerType.java
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
This class indicates the type of trigger which caused call suspension.
*/

public class TriggerType  {


	//Internal constant available  for switch statements
	
    public static final int M_FEATURE_ACTIVATION=0;
    public static final int M_VERTICAL_SERVICE_CODE=1;
    public static final int M_CUSTOMIZED_ACCESS=2;
    public static final int M_CUSTOMIZED_INTERCOM=3;
    public static final int M_EMERGENCY_SERVICE=12;
    public static final int M_AFR=13;
    public static final int M_SHARED_IO_TRUNK=14;
    public static final int M_OFF_HOOK_DELAY=17;
    public static final int M_CHANNEL_SETUP_PRI=18;
    public static final int M_T_NO_ANSWER=25;
    public static final int M_T_BUSY=26;
    public static final int M_O_CALLED_PARTY_BUSY=27;
    public static final int M_O_NO_ANSWER=29;
    public static final int M_ORIGINATION_ATTEMPT_AUTHORIZED=30;
    public static final int M_O_ANSWER=31;
    public static final int M_O_DISCONNECT=32;
    public static final int M_TERM_ATTEMPT_AUTHORIZED=33;
    public static final int M_T_ANSWER=34;
    public static final int M_T_DISCONNECT=35;
    
	
	// internal variable to store the constant value 
	
	private int triggerType;
	
	
/**
Constructor for TriggerType datatype
*/
    
    private TriggerType(int triggerType ) 
	{
    	this.triggerType=triggerType;
		
	}
	
	/**
	* TriggerType  :FEATURE_ACTIVATION
	*
	*/
	
	public static final TriggerType FEATURE_ACTIVATION = new TriggerType (M_FEATURE_ACTIVATION);
	
	
	
	/**
	* TriggerType  :VERTICAL_SERVICE_CODE
	*
	*/
	
	public static final TriggerType  VERTICAL_SERVICE_CODE= new TriggerType (M_VERTICAL_SERVICE_CODE);
	
	
	
	/**
	* TriggerType  :CUSTOMIZED_ACCESS
	*
	*/
	
	public static final TriggerType  CUSTOMIZED_ACCESS= new TriggerType (M_CUSTOMIZED_ACCESS);
	

	/**
	* TriggerType  :CUSTOMIZED_INTERCOM
	*
	*/
	
	public static final TriggerType CUSTOMIZED_INTERCOM = new TriggerType (M_CUSTOMIZED_INTERCOM);
	
	
	/**
	* TriggerType  :EMERGENCY_SERVICE
	*
	*/
	
	public static final TriggerType  EMERGENCY_SERVICE= new TriggerType (M_EMERGENCY_SERVICE);
	
	
	/**
	* TriggerType  :AFR
	*
	*/
	
	public static final TriggerType AFR = new TriggerType (M_AFR);
	
			
	/**
	* TriggerType  :SHARED_IO_TRUNK
	*
	*/
	
	public static final TriggerType SHARED_IO_TRUNK = new TriggerType (M_SHARED_IO_TRUNK);
	
	
	
	/**
	* TriggerType  :OFF_HOOK_DELAY
	*
	*/
	
	public static final TriggerType OFF_HOOK_DELAY = new TriggerType (M_OFF_HOOK_DELAY);
	
	
	
	/**
	* TriggerType  :CHANNEL_SETUP_PRI
	*
	*/
	
	public static final TriggerType CHANNEL_SETUP_PRI = new TriggerType (M_CHANNEL_SETUP_PRI);
	
	
	
	/**
	* TriggerType  :T_NO_ANSWER
	*
	*/
	
	public static final TriggerType  T_NO_ANSWER= new TriggerType (M_T_NO_ANSWER);
	
	
	/**
	* TriggerType  :T_BUSY
	*
	*/
	
	public static final TriggerType T_BUSY = new TriggerType (M_T_BUSY);
	
	
	/**
	* TriggerType  :O_CALLED_PARTY_BUSY
	*
	*/
	
	public static final TriggerType  O_CALLED_PARTY_BUSY= new TriggerType (M_O_CALLED_PARTY_BUSY);
	
	
	
	/**
	* TriggerType  :O_NO_ANSWER
	*
	*/
	
	public static final TriggerType  O_NO_ANSWER= new TriggerType (M_O_NO_ANSWER);
	
	
	
	/**
	* TriggerType  :ORIGINATION_ATTEMPT_AUTHORIZED
	*
	*/
	
	public static final TriggerType ORIGINATION_ATTEMPT_AUTHORIZED = new TriggerType (M_ORIGINATION_ATTEMPT_AUTHORIZED);
	
	
	
	/**
	* TriggerType  :O_ANSWER
	*
	*/
	
	public static final TriggerType O_ANSWER = new TriggerType (M_O_ANSWER);
	
	
	
	/**
	* TriggerType  :O_DISCONNECT
	*
	*/
	
	public static final TriggerType  O_DISCONNECT= new TriggerType (M_O_DISCONNECT);
	
	
	/**
	* TriggerType  :TERM_ATTEMPT_AUTHORIZED
	*
	*/
	
	public static final TriggerType TERM_ATTEMPT_AUTHORIZED = new TriggerType (M_TERM_ATTEMPT_AUTHORIZED);
	
	
	
	/**
	* TriggerType  :T_ANSWER
	*
	*/
	
	public static final TriggerType  T_ANSWER= new TriggerType (M_T_ANSWER);
	
	
	
	/**
	* TriggerType  :T_DISCONNECT
	*
	*/
	
	public static final TriggerType T_DISCONNECT = new TriggerType (M_T_DISCONNECT);
	
		
	
    /**
    * Gets the integer String representation of the Constant class 
    *
    * @ return  Integer provides value of Constant
    *
    */
	
	public int getTriggerType ()
	{
	
		return triggerType ;
		
	}

	   
}// end class 























