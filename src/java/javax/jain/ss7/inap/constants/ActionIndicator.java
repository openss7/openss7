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
 *  File Name     : ActionIndicator.java
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
This class indicates the action to be performed by the Manage Trigger Data operation.
*/

public class ActionIndicator {

    public static final int M_ACTIVATE=1;
    public static final int M_DEACTIVATE=2;
    public static final int M_RETRIEVE=3;
	
	
	//Internal variable to store the constant value ..
	private int actionIndicator;
	

/**
Constructor for ActionIndicator datatype
*/
    private ActionIndicator (int actionIndicator)
    {
	
		this.actionIndicator=actionIndicator;
		
    }
	
	
	/**
	*ActionIndicator :ACTIVATE
	*/
	
	public static final ActionIndicator ACTIVATE=new ActionIndicator (M_ACTIVATE);
	
	
		/**
	*ActionIndicator :DEACTIVATE
	*/
	public static final ActionIndicator DEACTIVATE=new ActionIndicator (M_DEACTIVATE);
	
	
		/**
	*ActionIndicator :	RETRIEVE
	*/
	
	public static final ActionIndicator RETRIEVE=new ActionIndicator (M_RETRIEVE);
	
	/**
	*gets the integer string representationof the constant class and  return Integer provides value of constant
	
	*/
	
	public int getActionIndicator	()
	
	{
	
		return actionIndicator;
		
	}
		
  
    
    
}
