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
 *  File Name     : RRejectIndEvent.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap.event;

import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.inap.constants.*;

/**
 This Class contains information related to the Remote Reject Component being received
 */	
 
public class RRejectIndEvent extends ComponentIndEvent
			implements java.io.Serializable
{

	private int invokeID;
	private boolean isInvokeID = false ;

	private RejectType rejectType;
   private GeneralProblem rejectCode;


/**
Constructs a new RRejectIndEvent. 
*/

	public RRejectIndEvent(Object source, GeneralProblem rejectcode)
	{
		super(source);
		componentIndPrimitiveType=ComponentIndPrimitiveType.L_REJECT;
		rejectType = RejectType.GENERAL_PROBLEM;
		setRejectCode(rejectcode);
	}

/**
Gets the type of component primitive .
*/
	public ComponentIndPrimitiveType getComponentIndPrimitiveType()
	{
		return componentIndPrimitiveType;
	}


//---------------------------------------------	

/**
Sets the Invoke ID.
*/
	public void setInvokeID(int invokeID)
	{
		this.invokeID = invokeID;
		isInvokeID = true;

	}
	

/**
Gets the Invoke ID.
*/
	public int getInvokeID()
	{
		return invokeID;
	}

/*
Indicates if the Invoke ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isInvokeIDPresent()
    {
        return isInvokeID;
    }


//---------------------------------------------	

/**
  Gets Reject Type
*/

    public RejectType getRejectType()
    {
        return rejectType;
    }


//-----------------------
	 

/**
  Gets Reject Code
*/

    public GeneralProblem getRejectCode()
    {
        return rejectCode;
    }

/**
  Sets Reject Code
*/
    public void setRejectCode(GeneralProblem rejectCode) 
    {
        this.rejectCode = rejectCode;
    }
    
//-----------------------


}
