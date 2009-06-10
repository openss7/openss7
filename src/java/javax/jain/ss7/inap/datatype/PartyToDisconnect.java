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
 *  File Name     : PartyToDisconnect.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;

/**
This Class defines the Party to Disconnect Datatype
*/

public class PartyToDisconnect  implements java.io.Serializable
{

    private LegID legID;
 

    private int callSegmentID;
 
   
   private PartyToDisconnectChoice partyToDisconnectChoice;


/**
Constructors For PartyToDisconnect
*/
	public PartyToDisconnect(LegID legID)
	{
		setLegID( legID);
	}

	public PartyToDisconnect(int callSegmentID)
	{
		setCallSegmentID( callSegmentID);
	}

//-----------------------------------

/** 
Gets the Party To disconnect Choice
*/
	public PartyToDisconnectChoice getPartyToDisconnectChoice()
	{
		return partyToDisconnectChoice;
	
	}

/** 
Gets the Leg ID
*/
    public LegID getLegID() throws ParameterNotSetException
    {
        if(partyToDisconnectChoice.getPartyToDisconnectChoice()==PartyToDisconnectChoice.LEG_ID.getPartyToDisconnectChoice())
		{
        	return legID;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }

/**
Sets the Leg ID
*/

    public void setLegID( LegID legID)
    {
        this.legID=legID;
        partyToDisconnectChoice=PartyToDisconnectChoice.LEG_ID;
		//isLegID=true;
		
    }

//-----------------------------------

/**
Gets the Call Segment ID
*/

    public int getCallSegmentID() throws ParameterNotSetException
    {
        if( partyToDisconnectChoice.getPartyToDisconnectChoice()==PartyToDisconnectChoice.CALL_SEGMENT_ID.getPartyToDisconnectChoice())
		{
        	return callSegmentID;
        }
		else
		{
            throw new ParameterNotSetException();
        }

    }

/**
Sets the Call Segment ID
*/

    public void setCallSegmentID( int callSegmentID)
    {
        this.callSegmentID=callSegmentID;
        partyToDisconnectChoice=PartyToDisconnectChoice.CALL_SEGMENT_ID;
		//isCallSegmentID=true;
    }




}
