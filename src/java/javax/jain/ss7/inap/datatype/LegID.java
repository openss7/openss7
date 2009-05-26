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
 *  File Name     : LegID.java
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
This class defines the LegID Datatype.
*/


public class LegID implements Serializable
{

    private String sendingSideID;


    private String receivingSideID;


	private LegIDChoice legIDChoice;

/**
Constructor For LegID
*/
	public LegID(String sendReceive, LegIDChoice legIDChoice)
	{
		if(legIDChoice.getLegIDChoice()== LegIDChoice.SENDING_SIDE_ID.getLegIDChoice())
			setSendingSideID( sendReceive);
		else if(legIDChoice.getLegIDChoice()== LegIDChoice.RECEIVING_SIDE_ID.getLegIDChoice())
			setReceivingSideID( sendReceive);
	}

//-----------------------------------
/**
Gets leg IDChoice Choice
*/

public LegIDChoice getLegIDChoice()
	{ 
		return legIDChoice;
	}

/**
Gets the Sending Side ID
*/
    public String getSendingSideID() throws ParameterNotSetException
    {
        if(legIDChoice==LegIDChoice.SENDING_SIDE_ID)
		{
        	return sendingSideID;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }

/**
Sets the Sending Side ID
*/

    public void setSendingSideID( String sendingSideID)
    {
        this.sendingSideID=sendingSideID;
        legIDChoice=LegIDChoice.SENDING_SIDE_ID;
    }


//-------------------------------------------

/**
Gets the Receiving Side ID
*/

    public String getReceivingSideID() throws ParameterNotSetException
    {
        if(legIDChoice==LegIDChoice.RECEIVING_SIDE_ID)
		{
        	return receivingSideID;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }

/**
Sets the Receving Side ID
*/

    public void setReceivingSideID( String receivingSideID)
    {
        this.receivingSideID=receivingSideID;
        legIDChoice=LegIDChoice.RECEIVING_SIDE_ID;
    }



}