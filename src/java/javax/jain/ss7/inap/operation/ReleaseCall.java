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
 *  File Name     : ReleaseCall.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap.operation;

import java.io.*;
import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.datatype.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;

/**
This class represents the Release Call Operation.
*/

public class ReleaseCall   extends Operation   implements Serializable {

    
    private Cause initialCallSegment;
    
    private AssociatedCallSegment associatedCallSegment;
    
    private AllCallSegments allCallSegments;
    private boolean isAllCallSegments = false ;
    
/**
Constructor For ReleaseCall
*/
	public ReleaseCall(Cause initialCallSegment, AssociatedCallSegment associatedCallSegment)
	{
		operationCode = OperationCode.RELEASE_CALL;
		setInitialCallSegment(initialCallSegment);
		setAssociatedCallSegment(associatedCallSegment);
	}

//----------------------------------------------    
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------

/**
Gets Initial Call Segment 
*/
    public Cause getInitialCallSegment()
    {
        return initialCallSegment;
    }
/**
Sets Initial Call Segment 
*/
    public void setInitialCallSegment(Cause initialCallSegment)
    {
        this.initialCallSegment = initialCallSegment;
    }

//----------------------------------------------  

/**
Gets Associated Call Segment 
*/
    public AssociatedCallSegment getAssociatedCallSegment()
    {
        return associatedCallSegment;
    }
    
/**
Sets Associated Call Segment 
*/
    public void setAssociatedCallSegment(AssociatedCallSegment associatedCallSegment)
    {
        this.associatedCallSegment = associatedCallSegment;
    }

//----------------------------------------------  

/**
Gets All Call Segments 
*/
    public AllCallSegments getAllCallSegments() throws ParameterNotSetException
    {
        if(isAllCallSegmentsPresent())
             return allCallSegments;
        else 
             throw new ParameterNotSetException();
    }

/**
Sets All Call Segments 
*/
    public void setAllCallSegments(AllCallSegments allCallSegments)
    {
        this.allCallSegments = allCallSegments;
        isAllCallSegments = true; 
    }

/**
Indicates if the All Call Segments optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isAllCallSegmentsPresent()
    {
        return isAllCallSegments;
    }

//----------------------------------------------  

}