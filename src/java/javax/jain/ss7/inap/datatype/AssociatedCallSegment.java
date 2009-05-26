
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
 *  File Name     : AssociatedCallSegment.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;

/**
This class defines the Associated Call Segment Datatype
*/

public class AssociatedCallSegment implements Serializable
{

    private int callSegment;
    
    private Cause releaseCause;
    private boolean isReleaseCause = false ;

/**
Constructor For AssociatedCallSegment
*/
	public AssociatedCallSegment(int callSegment)
	{
		setCallSegment(callSegment);
	}
	
//---------------------------------------------	

/**
Gets Call Segment
/*/

    public int getCallSegment()
    {
        return callSegment ;
    }

/**
Sets Call Segment
/*/

public void setCallSegment ( int callSegment)
    {
        this.callSegment =callSegment ;
    }

//---------------------------------------------	

/**
Gets Release Cause
*/

    public Cause  getReleaseCause() throws ParameterNotSetException
    {
         if(isReleaseCausePresent())
		 {
         	return releaseCause ;
         }
		 else
		 {
                throw new ParameterNotSetException();
         }  
    }

/**
Sets Release Cause
*/

    public void setReleaseCause( Cause   releaseCause )
    {
        this.releaseCause =releaseCause ;
        isReleaseCause=true;
    }

/**
Indicates if the Release Cause optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isReleaseCausePresent()
    {
        return isReleaseCause;
    }



//---------------------------------------------	

}