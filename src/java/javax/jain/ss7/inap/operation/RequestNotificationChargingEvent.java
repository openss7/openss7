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
 *  File Name     : RequestNotificationChargingEvent.java
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

/**
 This class represents the RequestNotificationChargingEvent Operation.
*/


public class RequestNotificationChargingEvent  extends Operation   implements Serializable
{
    

    private ChargingEvent chargingEvents[];

/**
Constructor For RequestNotificationChargingEvent
*/
	public RequestNotificationChargingEvent(ChargingEvent chargingEvents[])
	{
		operationCode = OperationCode.REQUEST_NOTIFICATION_CHARGING_EVENT;
		setChargingEvents(chargingEvents);
	}

//---------------------------------------
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------

/**
Gets Charging Event
*/
    public ChargingEvent[] getChargingEvents()
    {
        return chargingEvents;
    }

/**
Gets a particular  Charging Event
*/
    public ChargingEvent getChargingEvent(int index)
    {
        return chargingEvents[index];
    }

/**
Sets Charging Event
*/
    public void setChargingEvents(ChargingEvent chargingEvents[])
    {
        this.chargingEvents = chargingEvents;
    }
/**

/**
Sets a particular Agreements Parameter
*/

    public void  setChargingEvent(int index ,ChargingEvent chargingEvent)
    {
        chargingEvents[index] = chargingEvent ;
    }

//-----------------------------------    

}