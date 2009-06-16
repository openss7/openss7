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
 *  File Name     : EventNotificationCharging.java
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
import javax.jain.*;


/**
This class represents the EventNotificationCharging Operation.
*/

public class EventNotificationCharging  extends Operation   implements java.io.Serializable
{
	

	private int monitorMode = 1;
    private boolean isMonitorMode = false ;
    
    private java.lang.String eventTypeCharging;

    private java.lang.String eventSpecificInformationCharging;
    private boolean isEventSpecificInformationCharging = false ;

    private LegID legID;
    private boolean isLegID = false ;
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ;      

/**
Constructor For EventNotificationCharging
*/
	public EventNotificationCharging(java.lang.String eventTypeCharging)
	{
		operationCode = OperationCode.EVENT_NOTIFICATION_CHARGING;
		setEventTypeCharging(eventTypeCharging);
	}
	
//-----------------------							


/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------
	

/**
Gets Event Type Charging
*/
    public java.lang.String getEventTypeCharging() 
    {
        return eventTypeCharging;
    }
/**
Sets Event Type Charging
*/
    public void setEventTypeCharging(java.lang.String eventTypeCharging)
    {
        this.eventTypeCharging = eventTypeCharging;
    }

//-----------------------

/**
Gets Event Specific Information Charging
*/
    public java.lang.String getEventSpecificInformationCharging() throws ParameterNotSetException
    {
        if(isEventSpecificInformationChargingPresent())
              return eventSpecificInformationCharging;
        else
              throw new ParameterNotSetException();
    }
/**
Sets Event Specific Information Charging
*/
    public void setEventSpecificInformationCharging(java.lang.String eventSpecificInformationCharging)
    {
        this.eventSpecificInformationCharging = eventSpecificInformationCharging;
        isEventSpecificInformationCharging = true;
    }
/**
Indicates if the Event Specific Information Charging optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isEventSpecificInformationChargingPresent()
    {
        return isEventSpecificInformationCharging;
    }

//-----------------------

/**<P>
Gets Monitor Mode
<DT> This parameter gets the information of an event being relayed and/or 
processed by the SSP. In the context of charging events, the following explanantions hold good.
<LI>INTERRUPTED - Means that the SSF has notified the SCF of the charging event; 
does not process the event but discards it.
<LI>NOTIFY_AND_CONTINUE	- Means that the SSF has notified the SCF of the charging event 
and continues processing the event without waiting for SCF instructions.
<LI>TRANSPARENT - Means that the SSF does not notify the SCF of the event. Monitoring the charging 
of a previously requested charging event is ended.
<P>
*/
    public int getMonitorMode() throws ParameterNotSetException
    {
        if(isMonitorModePresent())
              return monitorMode;
        else
              throw new ParameterNotSetException();

    }
/**
Sets Monitor Mode
*/
    public void setMonitorMode(int monitorMode)
    {
   
    	  this.monitorMode = monitorMode;
          isMonitorMode = true;
    }
/**
Indicates if the Monitor Mode optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isMonitorModePresent()
    {
        return isMonitorMode;
    }

//-----------------------

/**
Gets Leg ID
*/
    public LegID getLegID() throws ParameterNotSetException
    {
        if(isLegID())
              return legID;
        else
              throw new ParameterNotSetException();

    }

/**
Sets Leg ID
*/
    public void setLegID(LegID legID)
    {
        this.legID = legID;
        isLegID = true;
    }

/**
Indicates if the Leg ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isLegID()
    {
        return isLegID;
    }

//-----------------------

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
