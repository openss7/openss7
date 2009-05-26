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
 *  File Name     : MoveCallSegments.java
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
This class represents the MoveCallSegments Operation.
*/

public class MoveCallSegments  extends Operation   implements Serializable
{
	 

	 private int targetCallSegmentAssociation;
	 
    private CallSegment callSegments[];

    private Leg legS[];
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ;      
    
/**
Constructor For MoveCallSegments
*/
	public MoveCallSegments(int targetCallSegmentAssociation,
							CallSegment callSegments[],
							Leg legs[])
	{
		operationCode = OperationCode.MOVE_CALL_SEGMENTS;
		setTargetCallSegmentAssociation(targetCallSegmentAssociation);
		setCallSegments (callSegments);
		setLegs(legs);
	}

//--------------------------------------
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------

/**
Gets Target Call Segment Association
*/

    public int getTargetCallSegmentAssociation() 
    {
        return targetCallSegmentAssociation;
    }

/**
Sets Target Call Segment Association
*/

    public void  setTargetCallSegmentAssociation (int targetCallSegmentAssociation)
    {
        this.targetCallSegmentAssociation = targetCallSegmentAssociation;
    }

//----------------------------------------------

/**
Gets Call Segments
*/

    public CallSegment[] getCallSegments()
    {
        return callSegments;
    }

/**
Gets a particular Call Segment
*/

    public CallSegment getCallSegment(int index) 
    {
        return callSegments[index];
    }

/**
Sets Call Segments*/

    public void  setCallSegments (CallSegment callSegments[])
    {
        this.callSegments = callSegments;
    }


/**
Sets a particular Call Segment
*/

    public void  setCallSegment(int index ,CallSegment callSegment)
    {
        callSegments[index] = callSegment;
    }

//----------------------------------------------

/**
Gets Legs
*/

    public Leg[] getLegs() 
    {
        return legS;
    }

/**
Gets a particular Leg
*/

    public Leg getLeg(int index) 
    {
        return legS[index];
    }

/**
Sets Legs*/

    public void  setLegs (Leg legs[])
    {
        legS = legs;
    }


/**
Sets a particular Leg
*/

    public void  setLeg(int index ,Leg leg)
    {
        legS[index] = leg;
    }
//---------------

/**
Gets Extensions Parameter
*/

    public ExtensionField[] getExtensions() throws ParameterNotSetException
    {
        if(isExtensionsPresent())
             return extensions;
        else 
             throw new ParameterNotSetException();
    }

/**
Gets a particular  Extension Parameter
*/

    public ExtensionField getExtension(int index) 
    {
        return extensions[index];
    }

/**
Sets Extensions Parameter
*/

    public void  setExtensions (ExtensionField extensions[])
    {
        this.extensions = extensions ;
        isExtensions = true;  
    }

/**
Sets a particular Extensions Parameter
*/

    public void  setExtension (int index , ExtensionField extension)
    {
        this.extensions[index] = extension ;
    }

/**
Indicates if the Extensions optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isExtensionsPresent()
    {
        return isExtensions;
    }
    
//-----------------------

}

