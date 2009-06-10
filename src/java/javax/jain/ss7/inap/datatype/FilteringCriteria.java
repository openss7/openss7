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
 *  File Name     : FilteringCriteria.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.*;

/**
This class represents the FilteringCriteria DataType 
*/

public class FilteringCriteria implements java.io.Serializable
{

	
	private DigitsGenericDigits dialledNumber;


	private DigitsGenericDigits callingLineID;

	
	private int serviceKey;


	private AddressAndService addressAndService;

	
	private FilteringCriteriaChoice filteringCriteriaChoice;
	private FilterCriteriaID filterCriteriaID;
/**
Constructor For FilteringCriteria
*/
	public FilteringCriteria(DigitsGenericDigits number, FilterCriteriaID filterCriteriaID)	
	{
		if(filterCriteriaID.getFilterCriteriaID() == FilterCriteriaID.DIALLED_NUMBER.getFilterCriteriaID())
			setDialledNumber(number);
		else if(filterCriteriaID.getFilterCriteriaID() ==FilterCriteriaID.CALLING_LINE_ID.getFilterCriteriaID())
			setCallingLineID(number);
	}
	
	public FilteringCriteria(AddressAndService addressAndService)
	{
		setAddressAndService(addressAndService);
	}
	
	public FilteringCriteria(int serviceKey) throws IllegalArgumentException
	{
		setServiceKey(serviceKey);
	}

//-----------------------------------
	
/**
Gets Filtering Criteria Choice
*/

public FilteringCriteriaChoice getFilteringCriteriaChoice()
	{ 
		return filteringCriteriaChoice;
	}

/**
Gets Dialled Number
*/
	public DigitsGenericDigits getDialledNumber() throws ParameterNotSetException
    	{               
        		if(filteringCriteriaChoice ==FilteringCriteriaChoice.DIALLED_NUMBER)
				{
                	return dialledNumber;
                }
				else
				{
                     throw new ParameterNotSetException();
                }
    	}
/**
Sets Dialled Number
*/
	public void setDialledNumber(DigitsGenericDigits dialledNumber)
    	{
        		this.dialledNumber = dialledNumber;
              	filteringCriteriaChoice=FilteringCriteriaChoice.DIALLED_NUMBER;
    	}


//-----------------------

/**
Gets Calling Line ID
*/
	public DigitsGenericDigits getCallingLineID() throws ParameterNotSetException
    	{
        		if(filteringCriteriaChoice.getFilteringCriteriaChoice() ==FilteringCriteriaChoice.CALLING_LINE_ID.getFilteringCriteriaChoice())
				{
                	return callingLineID;
                }
				else
				{
                    throw new ParameterNotSetException();
                }
    	}
/**
Sets Calling Line ID
*/
	public void setCallingLineID(DigitsGenericDigits callingLineID)
    	{
        		this.callingLineID = callingLineID;
                filteringCriteriaChoice =FilteringCriteriaChoice.CALLING_LINE_ID;
    	}
//-----------------------

/**
Gets Service Key
*/
	public int getServiceKey() throws ParameterNotSetException
    	{
        		if(filteringCriteriaChoice.getFilteringCriteriaChoice() ==FilteringCriteriaChoice.SERVICE_KEY.getFilteringCriteriaChoice())
				{
                      return serviceKey;
                }
				else
				{
                      throw new ParameterNotSetException();
                }
    	}
/**
Sets Service Key
*/
	public void setServiceKey(int serviceKey) throws IllegalArgumentException
    	{
    		if((serviceKey>=0)&&(serviceKey<=2147483647))
    		 {
    		  	this.serviceKey = serviceKey;
                filteringCriteriaChoice =FilteringCriteriaChoice.SERVICE_KEY;
    		 }
           	else
           	{  
            	throw new IllegalArgumentException("ParameterOutOfRange");
           	}
    	}


//-----------------------    

/**
Gets Address And Service
*/
	public AddressAndService getAddressAndService() throws ParameterNotSetException
    	{
        		if(filteringCriteriaChoice.getFilteringCriteriaChoice() ==FilteringCriteriaChoice.ADDRESS_AND_SERVICE.getFilteringCriteriaChoice())
				{
                         return addressAndService;
                }
				else
				{
                         throw new ParameterNotSetException();
                }
    	}
/**
Sets Address And Service
*/
	public void setAddressAndService(AddressAndService addressAndService)
    	{
        		this.addressAndService = addressAndService;
                filteringCriteriaChoice =FilteringCriteriaChoice.ADDRESS_AND_SERVICE;
    	}
//-----------------------
}
