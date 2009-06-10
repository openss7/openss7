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
 *  File Name     : AddressAndService.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;

/**
This class represents the AddressAndService DataType
*/

public class AddressAndService implements java.io.Serializable
{
	
	private DigitsGenericNumber calledAddressValue;

	private int serviceKey;

	private DigitsGenericNumber callingAddressValue;
	private boolean isCallingAddressValue =false;

	private LocationNumber locationNumber;
	private boolean isLocationNumber = false ;

/**
Constructor For AddressAndService
*/
	public AddressAndService(DigitsGenericNumber calledAddressValue, int serviceKey)
	{
		setCalledAddressValue(calledAddressValue);
		setServiceKey(serviceKey);
	}
	
//---------------------------------------------	

/**
Gets Called Address Value
*/
	public DigitsGenericNumber getCalledAddressValue()
    	{
        		return calledAddressValue;
    	}
/**
Sets Called Address Value
*/
	public void setCalledAddressValue(DigitsGenericNumber calledAddressValue)
    	{
        		this.calledAddressValue = calledAddressValue;
    	}

//-----------------------

/**
Gets Service Key
*/
	public int getServiceKey()
    	{
        		return serviceKey;
    	}
/**
Sets Service Key
*/
	public void setServiceKey(int serviceKey)
    	{
        		this.serviceKey = serviceKey;
    	}

//-----------------------

/**
Gets Calling Address Value
*/
	public DigitsGenericNumber getCallingAddressValue() throws ParameterNotSetException
    	{
        		
             	if (isCallingAddressValuePresent())
				{		
               		return callingAddressValue;
              	}
				else
				{
                   throw new ParameterNotSetException();
              	}    

    	}
/**
Sets Calling Address Value
*/
	public void setCallingAddressValue(DigitsGenericNumber callingAddressValue)
    	{
        		this.callingAddressValue = callingAddressValue;
                        isCallingAddressValue=true;  
    	}

/**
Indicates if the Calling Address Value optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isCallingAddressValuePresent() 
    	{ 
        		return isCallingAddressValue; 
    	} 



//-----------------------    

/**
Gets Location Number
*/
	public LocationNumber getLocationNumber() throws ParameterNotSetException
    	{
        	if(isLocationNumberPresent())
			{	
               return locationNumber;
            }
			else
			{
                   throw new ParameterNotSetException();
            }    
    	}
/**
Sets Location Number
*/
	public void setLocationNumber(LocationNumber locationNumber)
    	{
        		this.locationNumber = locationNumber;
                isLocationNumber=true;
    	}
/**
Indicates if the Location Number optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isLocationNumberPresent()
    	{
        		return isLocationNumber;
    	}


//-----------------------
}
