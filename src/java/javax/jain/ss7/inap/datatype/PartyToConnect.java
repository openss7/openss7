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
 *  File Name     : PartyToConnect.java
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
This Class identifies the Party to Connect Datatype
*/

public class PartyToConnect  implements java.io.Serializable
{

    private LegID legID;


    private int callSegmentID;

	
	private PartyToConnectChoice partyToConnectChoice;
    

/**
Constructors For PartyToConnect
*/
	public PartyToConnect(LegID legID)
	{
		setLegID( legID);
	}
	
	public PartyToConnect(int callSegmentID)
	{
		setCallSegmentID( callSegmentID);
	}

//------------------------------------------	

/** 
Gets the Party To Connect Choice
*/
 public PartyToConnectChoice getPartyToConnectChoice()
 {
 return partyToConnectChoice;
 }


/** 
Gets the Leg ID
*/
    public LegID getLegID() throws ParameterNotSetException
    {
        if(partyToConnectChoice.getPartyToConnectChoice()==PartyToConnectChoice.LEG_ID.getPartyToConnectChoice())
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
		partyToConnectChoice=PartyToConnectChoice.LEG_ID;
        
		//isLegID=true;
    }


//-----------------------------------

/**
Gets the Call Segment ID
*/

    public int getCallSegmentID() throws ParameterNotSetException
    {
        if(partyToConnectChoice.getPartyToConnectChoice()==PartyToConnectChoice.CALL_SEGMENT_ID.getPartyToConnectChoice())
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
       partyToConnectChoice=PartyToConnectChoice.CALL_SEGMENT_ID;

    }



}
