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
 *  File Name     : StatusReport.java
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
import javax.jain.*;
/**
 This class represents the StatusReport Operation.
*/


public class StatusReport extends Operation implements Serializable
{

    

    private ResourceStatus resourceStatus;
    private boolean isResourceStatus = false ;

    private DigitsGenericDigits correlationID;
    private boolean isCorrelationID = false ;

    private ResourceID resourceID;
    private boolean isResourceID = false ;
    
    private ReportCondition reportCondition;
    private boolean isReportCondition =   false ;
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ; 
     
/**
Constructor For StatusReport
*/

	public StatusReport() 
	{
		operationCode = OperationCode.STATUS_REPORT;
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
Gets Resource Status
*/
    public ResourceStatus getResourceStatus() throws ParameterNotSetException
    {
        if(isResourceStatusPresent()){
         return resourceStatus;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Resource Status
*/
    public void setResourceStatus(ResourceStatus resourceStatus)

         {
         this.resourceStatus	 = resourceStatus;
                           isResourceStatus=true;

    }

/**
Indicates if the Resource Status parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isResourceStatusPresent()
    {
        return isResourceStatus;
    }


//--------------------------------------------------


/**
Gets Correlation ID
*/
    public DigitsGenericDigits getCorrelationID() throws ParameterNotSetException
    {
        if(isCorrelationIDPresent()){
        return correlationID;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Correlation ID
*/
    public void setCorrelationID(DigitsGenericDigits correlationID)
    {
        this.correlationID = correlationID;
        isCorrelationID=true;
    }
/**
Indicates if the Correlation ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCorrelationIDPresent()
    {
        return isCorrelationID;
    }

//--------------------------------------------------
    
/**
Gets Resource ID
*/
    public ResourceID getResourceID() throws ParameterNotSetException
    {
        if(isResourceIDPresent()){
         return resourceID;
         }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Resource ID
*/
    public void setResourceID(ResourceID resourceID)
    {
        this.resourceID = resourceID;
        isResourceID=true;
    }

/**
Indicates if the Resource ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isResourceIDPresent()
    {
        return isResourceID;
    }


//--------------------------------------------------


/**<P>
Gets Report Condition
<DT> This parameter gets the reason of issuing the 'Status Report' operation.
<LI>STATUS_REPORT
<LI>TIMER_EXPIRED	
<LI>CANCELLED
<P>
*/

    public ReportCondition getReportCondition() throws ParameterNotSetException
    {
        if(isReportConditionPresent()){
         return reportCondition;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Report Condition
*/
    public void setReportCondition(ReportCondition reportCondition) 
         {
         this.reportCondition  = reportCondition;
           isReportCondition=true;

    }
/**
Indicates if the Report Condition optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isReportConditionPresent()
    {
        return isReportCondition;
    }

//---------------

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



