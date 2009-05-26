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
 *  File Name     : ServiceFilteringResponse.java
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
This class represents the ServiceFilteringResponse Operation.
*/

public class ServiceFilteringResponse extends Operation implements Serializable
{

    

    private CounterAndValue countersValue[];

	 private FilteringCriteria filteringCriteria;
	 
	 private ResponseCondition responseCondition;
	 private boolean isResponseCondition = false ;
	 
	private ExtensionField extensions[];
  private boolean isExtensions = false ;  
	 
/**
Constructor For ServiceFilteringResponse
*/
	public ServiceFilteringResponse(CounterAndValue countersValue[], FilteringCriteria filteringCriteria) 

	{
	    operationCode = OperationCode.SERVICE_FILTERING_RESPONSE;
	    setCountersValue(countersValue);
		setFilteringCriteria(filteringCriteria);
	}
	
//--------------------------------------	
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------
/**
Gets Counters Value
*/

    public CounterAndValue[] getCountersValue() 
    {
        return countersValue;
    }

/**
Gets a particular Counter Value
*/

    public CounterAndValue getCounterValue(int index) 
    {
        return countersValue[index];
    }

/**
Sets Counters Value*/

    public void  setCountersValue(CounterAndValue countersValue[])
    {
        this.countersValue = countersValue;
    }


/**
Sets a particular Counter Value
*/

    public void  setCounterValue(int index ,CounterAndValue countersValue)
    {
        this.countersValue[index] = countersValue;
    }

//----------------------------------------------

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

    public void  setFilteringCriteria(FilteringCriteria filteringCriteria)
    {
        this.filteringCriteria = filteringCriteria;
    }

//----------------------------------------------
 

/**
Gets Response Condition
*/

    public ResponseCondition getResponseCondition() throws ParameterNotSetException
    {
        if(isResponseConditionPresent()){
        return responseCondition;
        }else{
               throw new ParameterNotSetException();
            }    
    }

/**
Sets Response Condition
*/

    public void  setResponseCondition(ResponseCondition responseCondition) 

        {
        this.responseCondition  = responseCondition;
           isResponseCondition=true;

    }

/**
Indicates if the Response Condition optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isResponseConditionPresent()
    {
        return isResponseCondition;
    }

//----------------------------------------------

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

