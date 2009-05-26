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
 *  File Name     : InvokeReqEvent.java
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
import javax.jain.ss7.inap.operation.*;

/**
 This Class contains information related to the Invoke Component being sent
 */	
 
public class InvokeReqEvent extends ComponentReqEvent
			implements java.io.Serializable
{

	private Operation operation;

	private int linkedID;
	private boolean isLinkedID = false ;


/**
Constructs a new InvokeReqEvent.
*/

	public InvokeReqEvent(Object source, Operation operation)
	{
		super(source);
		componentReqPrimitiveType= ComponentReqPrimitiveType.INVOKE;
		setOperation(operation);
	}

/**
Gets the type of component primitive .
*/
	public ComponentReqPrimitiveType getComponentReqPrimitiveType()
	{
		return componentReqPrimitiveType;
	}


//---------------------------------------------	

/**
Sets the Linked ID.
*/
	public void setLinkedID(int linkedID)
	{
		this.linkedID = linkedID;
		isLinkedID = true;
	}

/**
Gets the Linked ID.
*/
	public int getLinkedID()
	{
		return linkedID;
	}

/*
Indicates if the Linked ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isLinkedIDPresent()
    {
        return isLinkedID;
    }

//---------------------------------------------	

/**
Sets the INAP message i.e, Operation to be sent.
*/
	public void setOperation(Operation operation)
	{
		this.operation = operation;
	}

/**
Gets the INAP message i.e, Operation to be sent.
*/
	public Operation getOperation()
	{
		return operation;
	}

//---------------------------------------------	


}