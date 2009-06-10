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
 *  File Name     : SendFacilityInformation.java
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
This class represents the Send Facility Information Operation.
*/


public class SendFacilityInformation  extends Operation   implements java.io.Serializable
{
    
    private ComponentType componentType;
    
    private LegID legID;
    private boolean isLegID = false ;
    
    private int componentCorrelationID;
    private boolean isComponentCorrelationID= false ;
    
    private CallProcessingOperationCorrelationID callProcessingOperationCorrelationID;
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ;  
    
    private Component component; 
       
 /**
Constructor For SelectFacilityInformation
*/

	public SendFacilityInformation() 
	{
		operationCode = OperationCode.SEND_FACILITY_INFORMATION;
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
    public ComponentType getComponentType() 
    {
        return componentType;
    }

/**
Sets Component Type
*/
    public void setComponentType(ComponentType componentType) 

    
    {
    this.componentType  = componentType;
//    	Runtime.getRuntime().gc();
    }
/**

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

//-----------------------------------    
    
/**<P>
Gets Call Processing Operation Correlation ID. 
<DT> Returns one of the following messages:
<LI>ALERTING	
<LI>SETUP	
<LI>CONNECT
<LI>DISCONNECT	
<LI>RELEASE	
<LI>RELEASECOMPLETE
<LI>FACILITY	
<P>
*/

    public CallProcessingOperationCorrelationID getCallProcessingOperationCorrelationID() 
    {
        return callProcessingOperationCorrelationID;
    }

/**
Sets Call Processing Operation Correlation ID 
*/
    public void setCallProcessingOperationCorrelationID(CallProcessingOperationCorrelationID callProcessingOperationCorrelationID) 

    
    {
    this.callProcessingOperationCorrelationID	 = callProcessingOperationCorrelationID;

    }

//----------------------------------------


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


/**
Gets Component
*/

    public Component getComponent() 
    {
        return component;
    }


/**
Sets Component
*/

    public void  setComponent (Component component)
    {
        this.component = component;
    }

//-----------------------
} 



