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
 *  File Name     : CallSegment.java
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

/**
This class defines the CallSegment Datatype
*/

public class CallSegment implements Serializable
{
    private int sourceCallSegment = 1;

    private int newCallSegment;

/**
Constructor For CallSegment
*/
	public CallSegment(int newCallSegment)
	{
		setNewCallSegment(newCallSegment);
	}

//--------------------------------------

/**
Gets Source Call Segment
*/
    public int getSourceCallSegment()
    {
        return sourceCallSegment;
    }
/**
Sets Source Call Segment
*/
    public void setSourceCallSegment(int sourceCallSegment)
    {
        this.sourceCallSegment = sourceCallSegment;
    }

//----------------------------------------------

/**
Gets New Call Segment
*/
    public int getNewCallSegment()
    {
        return newCallSegment;
    }
/**
Sets New Call Segment
*/
    public void setNewCallSegment(int newCallSegment)
    {
        this.newCallSegment = newCallSegment;
    }

//----------------------------------------------

}
