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
 *  File Name     : ActionPerformed.java
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
This class indicates the result of the operation, Manage Trigger Data.
*/

public class ActionPerformed {

    public static final int M_ACTIVATED=1;       
    public static final int M_DEACTIVATED=2;
    public static final int M_ALREADY_ACTIVE=3;
    public static final int M_ALREADY_INACTIVE=4;
    public static final int M_IS_ACTIVE=5;
    public static final int M_IS_INACTIVE=6;
	
	
	//Internal variable to store the constant value
	private int actionPerformed;
	
    
/**
Constructor for ActionPerformed datatype
*/
    
	private  ActionPerformed(int actionPerformed)
	{
			
			this.actionPerformed=actionPerformed;
		
	}
	

	/**
	* ActionPerformed : ACTIVATED
	*/
	
	public static final ActionPerformed ACTIVATED=new ActionPerformed(M_ACTIVATED);
	
	
	
	
	
	/**
	* ActionPerformed : DEACTIVATED
	*/
	
	public static final ActionPerformed DEACTIVATED=new ActionPerformed(M_DEACTIVATED);
	
	
	
	
	/**
	* ActionPerformed : ALREADY_ACTIVE
	*/
	
	public static final ActionPerformed ALREADY_ACTIVE=new ActionPerformed(M_ALREADY_ACTIVE);
	
	
	
	/**
	* ActionPerformed :ALREADY_INACTIVE
	*/
	
	public static final ActionPerformed ALREADY_INACTIVE=new ActionPerformed(M_ALREADY_INACTIVE);
	
	
	
	/**
	* ActionPerformed : IS_ACTIVE
	*/
	
	public static final ActionPerformed IS_ACTIVE=new ActionPerformed(M_IS_ACTIVE);
	
	
	
	
	/**
	* ActionPerformed : IS_INACTIVE
	*/
	
	public static final ActionPerformed IS_INACTIVE=new ActionPerformed(M_IS_INACTIVE);
	
	
	/**
	* Gets the integer String representation of the constant class
	 @ returns Integer provides value of constant 
	 */
	 
	 
	
	public int getActionPerformed()
	{
	
		return actionPerformed;
	
	}
	
	    
}
