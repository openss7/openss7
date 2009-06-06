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
 *  File Name     : ForwardingCondition.java
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
The class for Forwarding Condition indicates the condition that must be met 
to complete the Connect operation.
*/


public class ForwardingCondition {

    public static final int M_BUSY=0;
    public static final int M_NO_ANSWER=1;
    public static final int M_ANY=2;

// internal variable to store the constant value

	private int forwardingCondition;

/**
Constructor for ForwardingCondition datatype
*/
    
    private ForwardingCondition(int forwardingCondition ) 
	{
    	
		this.forwardingCondition=forwardingCondition;		
    }
    

/**
* ForwardingCondition :BUSY
*
*/

public static final ForwardingCondition BUSY  =new ForwardingCondition (M_BUSY);


/**
* ForwardingCondition :N0_ANSWER	
*
*/

public static final ForwardingCondition NO_ANSWER   =new ForwardingCondition (M_NO_ANSWER);


/**
* ForwardingCondition :ANY
*
*/

public static final ForwardingCondition ANY  =new ForwardingCondition (M_ANY);




	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getForwardingCondition()
	{
	
	return forwardingCondition;
	
	}	

}
