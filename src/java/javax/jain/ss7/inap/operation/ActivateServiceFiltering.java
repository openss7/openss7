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
 *  File Name     : ActivateServiceFiltering.java
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
This class represents the ActivateServiceFiltering Operation.
*/

public class ActivateServiceFiltering extends Operation implements Serializable 
{
 
    private FilteredCallTreatment filteredCallTreatment;
    
    private FilteringCharacteristics filteringCharacteristics;
    
    private FilteringTimeOut filteringTimeOut;
    
    private FilteringCriteria filteringCriteria;
    
    private String startTime;
    private boolean isStartTime = false ;
	
    private ExtensionField extensions[];
    private boolean isExtensions = false ;   

/**
Constructor For ActivateServiceFiltering
*/

	public ActivateServiceFiltering(FilteredCallTreatment filteredCallTreatment,
									FilteringCharacteristics filteringCharacteristics,
									FilteringTimeOut filteringTimeout,
									FilteringCriteria filteringCriteria) 
	{
		operationCode = OperationCode.ACTIVATE_SERVICE_FILTERING;
		setFilteredCallTreatment(filteredCallTreatment);
		setFilteringCharacteristics(filteringCharacteristics);
		setFilteringTimeOut(filteringTimeout);
		setFilteringCriteria(filteringCriteria);
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
Gets Filtered Call Treatment
*/
    public FilteredCallTreatment getFilteredCallTreatment() 
    {
        return filteredCallTreatment;
    }

/**
Sets Filtered Call Treatment*/
    public void setFilteredCallTreatment(FilteredCallTreatment filteredCallTreatment)
    {
        this.filteredCallTreatment = filteredCallTreatment;
    }

//-----------------------

/**
Gets Filtering Characteristics
*/
    public FilteringCharacteristics getFilteringCharacteristics() 
    {
        return filteringCharacteristics;
    }
/**
Sets Filtering Characteristics
*/
    public void setFilteringCharacteristics(FilteringCharacteristics filteringCharacteristics) 
    {
                        	
         this.filteringCharacteristics = filteringCharacteristics;
          
    }

//-----------------------

/**
Gets Filtering Timeout
*/
    public FilteringTimeOut getFilteringTimeOut() 
    {
        return filteringTimeOut;
    }
/**
Sets Filtering TimeOut
*/
    public void setFilteringTimeOut(FilteringTimeOut filteringTimeout)
    {
        this.filteringTimeOut = filteringTimeout;
    }

//-----------------------

/**
Gets Filtering Criteria
*/
    public FilteringCriteria getFilteringCriteria() 
    {
        return filteringCriteria;
    }
/**
Sets Filtering Criteria
*/
    public void setFilteringCriteria(FilteringCriteria filteringCriteria)
    {
        this.filteringCriteria = filteringCriteria;
    }

//-----------------------
/**
Gets Start Time
*/
    public String getStartTime() throws ParameterNotSetException
    {
      if(isStartTimePresent()) 
         return startTime;
      else 
         throw new ParameterNotSetException(); 
       
    }

/**
Sets Start Time
*/
    public void setStartTime(String startTime)
    {
        this.startTime = startTime;
        isStartTime = true;
    }
/**
Indicates if the Start Time optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isStartTimePresent()
    {
        return isStartTime;
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
