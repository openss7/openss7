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
 *  File Name     : EventReportBCSM.java
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
import javax.jain.ss7.inap.exception.*;
import javax.jain.*;

/**
This class represents Event Report BCSM Operation.
*/

public class EventReportBCSM  extends Operation   implements Serializable
{
    
    private EventTypeBCSM eventTypeBCSM;

    private DigitsGenericDigits bcsmEventCorrelationID;
    private boolean isBcsmEventCorrelationID;

    private EventSpecificInformationBCSM eventSpecificInformationBCSM;
    private boolean isEventSpecificInformationBCSM;

    private LegID legID;
    private boolean isLegID;

    private MiscCallInfo miscCallInfo;

    private ComponentType componentType;
    private boolean isComponentType;

    private int componentCorrelationID;
    private boolean isComponentCorrelationID;
    
	private ExtensionField extensions[];
  private boolean isExtensions;  
    
    private Component component;
    private boolean isComponent;     

/**
Constructor For EventReportBCSM
*/
	public EventReportBCSM(EventTypeBCSM eventTypeBCSM,MiscCallInfo miscCallInfo) 
	{
		operationCode = OperationCode.EVENT_REPORT_BCSM;
		setEventTypeBCSM(eventTypeBCSM);
		setMiscCallInfo(miscCallInfo);
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


/**<P>
Gets Event Type BCSM.
<DT> This parameter gets the BCSM detection point event. 
<LI>ORIG_ATTEMPT_AUTHORIZED
<LI>COLLECTED_INFO	
<LI>ANALYSED_INFORMATION
<LI>ROUTE_SELECT_FAILURE
<LI>O_CALLED_PARTY_BUSY	
<LI>O_NO_ANSWER
<LI>O_ANSWER
<LI>O_MID_CALL	
<LI>O_DISCONNECT
<LI>O_ABANDON
<LI>TERM_ATTEMPT_AUTHORIZED	
<LI>T_BUSY
<LI>T_NO_ANSWER
<LI>T_ANSWER	
<LI>T_MID_CALL
<LI>T_DISCONNECT
<LI>T_ABANDON	
<LI>O_TERM_SEIZED
<LI>O_SUSPENDED	
<LI>T_SUSPENDED
<LI>ORIG_ATTEMPT	
<LI>TERM_ATTEMPT
<LI>O_RE_ANSWER	
<LI>T_RE_ANSWER
<LI>FACILITY_SELECTED_AND_AVAILABLE	
<LI>CALL_ACCPETED
<P>
*/
    public EventTypeBCSM getEventTypeBCSM()
    {
        return eventTypeBCSM;
    }
/**
Sets Event Type BCSM
*/
    public void setEventTypeBCSM (EventTypeBCSM eventTypeBCSM) 

        {
        this.eventTypeBCSM = eventTypeBCSM;
    }

//-----------------------

/**
Gets BCSM Event Correlation ID
*/
    public DigitsGenericDigits getBcsmEventCorrelationID() throws ParameterNotSetException
    {
        if(isBcsmEventCorrelationIDPresent())
              return bcsmEventCorrelationID;
        else
              throw new ParameterNotSetException();           
    }
/**
Sets BCSM Event Correlation ID
*/
    public void setBcsmEventCorrelationID (DigitsGenericDigits bcsmEventCorrelationID)
    {
        this.bcsmEventCorrelationID = bcsmEventCorrelationID;
        isBcsmEventCorrelationID = true;
    }
/**
Indicates if the BCSM Event Correlation ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isBcsmEventCorrelationIDPresent()
    {
        return isBcsmEventCorrelationID;
    }

//-----------------------

/**
Gets Event Specific Information BCSM
*/
    public EventSpecificInformationBCSM getEventSpecificInformationBCSM() throws ParameterNotSetException
    {
        if(isEventSpecificInformationBCSMPresent())
              return eventSpecificInformationBCSM;
        else
              throw new ParameterNotSetException();           

    }
/**
Sets Event Specific Information BCSM
*/
    public void setEventSpecificInformationBCSM (EventSpecificInformationBCSM eventSpecificInformationBCSM) throws IllegalArgumentException

    {
    	
        this.eventSpecificInformationBCSM = eventSpecificInformationBCSM;
        isEventSpecificInformationBCSM = true;
		
    }
/**
Indicates if the Event Specific Information BCSM optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isEventSpecificInformationBCSMPresent()
    {
        return isEventSpecificInformationBCSM;
    }

//-----------------------

/**
Gets Leg ID
*/
    public LegID getLegID() throws ParameterNotSetException
    {
        if(isLegIDPresent())
              return legID;
        else
              throw new ParameterNotSetException();           
   
    }
/**
Sets Leg ID
*/
    public void setLegID (LegID legID)
    {
        this.legID = legID;
        isLegID = true;
    }
/**
Indicates if the Leg ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isLegIDPresent()
    {
        return isLegID;
    }

//-----------------------

/**
Gets Misc Call Info
*/
    public MiscCallInfo getMiscCallInfo() 
    {
        
        return miscCallInfo;
    }
/**
Sets Misc Call Info
*/
    public void setMiscCallInfo (MiscCallInfo miscCallInfo)
    {
        this.miscCallInfo = miscCallInfo;
    }

//-----------------------


/**<P>
Gets ComponentType.
<DT> This parameter indicates the type of event that is reported to the SSF: 
<LI>ANY
<LI>INVOKE	
<LI>R_RESULT
<LI>R_ERROR
<LI>R_REJECT	
<LI>R_RESULT_NOT_LAST	
<P>
*/
    public ComponentType getComponentType() throws ParameterNotSetException
    {
        if(isComponentTypePresent()) 
              return componentType;
        else
              throw new ParameterNotSetException();           

    }
/**
Sets Component Type
*/
    public void setComponentType(ComponentType componentType)

        {
        this.componentType  = componentType;
           isComponentType = true;
//    	Runtime.getRuntime().gc();
    }
/**
Indicates if the Component Type optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isComponentTypePresent()
    {
        return isComponentType;
    }

//-----------------------

/**
Gets Component Correlation ID
*/
    public int getComponentCorrelationID() throws ParameterNotSetException
    {
        if(isComponentCorrelationIDPresent())
              return componentCorrelationID;
        else
              throw new ParameterNotSetException();           

    }
/**
Sets Component Correlation ID
*/
    public void setComponentCorrelationID(int componentCorrelationID)
    {
        this.componentCorrelationID = componentCorrelationID;
        isComponentCorrelationID = true;
    }
/**
Indicates if the Component Correlation ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isComponentCorrelationIDPresent()
    {
        return isComponentCorrelationID;
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


/**
Gets Component
*/

    public Component getComponent() throws ParameterNotSetException
    {
        if(isComponentPresent()) 
              return component;
        else
              throw new ParameterNotSetException();           

    }


/**
Sets Component
*/

    public void  setComponent (Component component)
    {
        this.component = component;
        isComponent = true;
    }

//-----------------------

/**
Indicates if the Component optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isComponentPresent()
    {
        return isComponent;
    }
    

}