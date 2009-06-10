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
 *  File Name     : Cancel.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap.operation;

import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.datatype.*;
import javax.jain.ss7.inap.constants.*;


/**
This class represents the Cancel Operation.
*/

public class Cancel  extends Operation  implements java.io.Serializable
{

    private int invokeID;
	 
    private int allRequests;
	 	 
    private CallSegmentToCancel callSegmentToCancel;
    
    private CancelChoice cancelChoice;


/**
Constructors For Cancel
*/
	public Cancel(int invokeID, CancelID cancelID)
	{
   	operationCode = OperationCode.CANCEL;
		if(cancelID.getCancelID() == CancelID.INVOKE_ID.getCancelID())
			setInvokeID(invokeID);
		else if(cancelID.getCancelID() == CancelID.ALL_REQUESTS.getCancelID())
			setAllRequests(invokeID);
	}

	public Cancel(CallSegmentToCancel callSegmentToCancel)
	{
		operationCode = OperationCode.CANCEL;
		setCallSegmentToCancel(callSegmentToCancel);
	}

//-----------------------------------

/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------

/**
Gets Invoke ID
*/
    public int getInvokeID()
    {
        return invokeID;
    }
    
/**
Sets Invoke ID
*/
    public void setInvokeID(int invokeID)
    {
        this.invokeID = invokeID;
        cancelChoice = CancelChoice.INVOKE_ID;
    }

//-----------------------

/**
Gets All Cancel Requests
*/
    public int getAllRequests()
    {
        return allRequests;
    }
    
/**
Sets All Cancel Requests
*/
    public void setAllRequests(int allRequests)
    {
        this.allRequests = allRequests;
        cancelChoice = CancelChoice.ALL_REQUESTS;

    }


//-----------------------

/**
Gets Call Segment To Cancel
*/
    public CallSegmentToCancel getCallSegmentToCancel() 
    {
        return callSegmentToCancel;
    }
    
/**
Sets Call Segment To Cancel
*/
    public void setCallSegmentToCancel(CallSegmentToCancel callSegmentToCancel)
    {
        this.callSegmentToCancel = callSegmentToCancel;
        cancelChoice = CancelChoice.CALL_SEGMENT;

    }
    
//-----------------------

/**
Gets Cancel Choice
*/
    public CancelChoice getCancelChoice() 
    {
        return cancelChoice;
    }
    
//-----------------------


}
