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
 *  File Name     : RequestReportFacilityEvent.java
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
import javax.jain.ss7.inap.exception.*;
import javax.jain.*;


/**
This class represents the RequestReportFacilityEvent Operation.
*/


public class RequestReportFacilityEvent  extends Operation   implements java.io.Serializable
{
    

    private ComponentType componentTypes[]= new ComponentType[3];
    
    private LegID legID;
    private boolean isLegID = false ;
    
    private int componentCorrelationID;
    private boolean isComponentCorrelationID = false ;
    
    private int monitorDuration;
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ;    

   
/**
Constructors For RequestReportFacilityEvent
*/

	public RequestReportFacilityEvent(int monitorDuration) throws IllegalArgumentException
	{
		operationCode = OperationCode.REQUEST_REPORT_FACILITY_EVENT;
		for(int i=0;i<componentTypes.length;i++)
		{
			componentTypes[i] = ComponentType.ANY;
		}

		setMonitorDuration(monitorDuration);
	}
//-----------------------------------    
 /**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
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
    public ComponentType[] getComponentTypes()
    {
        return componentTypes;
    }

/**
Gets a Particular Component Type
*/
    public ComponentType getComponentType(int index)
    {
        return componentTypes[index];
    }

/**
Sets Component Type
*/
    public void setComponentTypes(ComponentType componentTypes[])
    {
        componentTypes = componentTypes;
    }

/**
Sets a Particular Component Type
*/

    public void  setComponentType(int index, ComponentType componentType)
    {
        componentTypes[index] = componentType;
    }
    
//-----------------------------------    

/**
Gets the LegID 
*/
    public LegID getLegID() throws ParameterNotSetException
    {
        if(isLegIDPresent()){
        return legID;
         }else{
               throw new ParameterNotSetException();
            }  
    }

/**
Sets the LegID
*/
    public void setLegID(LegID legID)
    {
        this.legID = legID;
        isLegID=true;
    }

/**
Indicates if the LegID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isLegIDPresent()
    {
        return isLegID;
    }
    
//-----------------------------------    
    
/**
Gets the Monitor Duration
*/
    public int getMonitorDuration()
    {
        return monitorDuration;
         
    }

/**
Sets the Monitor Duration
*/
    public void setMonitorDuration(int monitorDuration) throws IllegalArgumentException

    { 
      if(monitorDuration>=-2 || monitorDuration <=86400 )
       {    	 
        this.monitorDuration = monitorDuration;
       }
       else
        {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }  
    }

//-----------------------------------    

/**
Gets Component Correlation ID
*/
    public int getComponentCorrelationID() throws ParameterNotSetException
    {
        if(isComponentCorrelationIDPresent()){
        return componentCorrelationID;
        }else{
               throw new ParameterNotSetException();
            }  
    }

/**
Sets Component Correlation ID
*/
    public void setComponentCorrelationID(int componentCorrelationID)
    {
        this.componentCorrelationID = componentCorrelationID;
         isComponentCorrelationID=true;
    }

/**
Indicates if the Component Correlation ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isComponentCorrelationIDPresent()
    {
        return isComponentCorrelationID;
    }

//-------------

/**
Gets Extensions Parameter
*/

    public ExtensionField[] getExtensions() throws ParameterNotSetException
    {
        if(isExtensionsPresent()){
        return extensions;
        }else{
               throw new ParameterNotSetException();
            }  
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
        isExtensions=true;
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



