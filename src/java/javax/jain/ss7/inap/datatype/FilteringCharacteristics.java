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
 *  File Name     : FilteringCharacteristics.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.*;

/**
This class represents the FilteringCharacteristics DataType
*/

public class FilteringCharacteristics implements Serializable{

	 private int interval;

	
 	 private int numberOfCalls;


	 private FilteringCharacteristicsChoice filteringCharacteristicsChoice;
	 
/**
Constructor For FilteringCharacteristics
*/
	public FilteringCharacteristics(int filteringValue, FilteringCharacteristicsChoice filteringCharacteristicsChoice) throws IllegalArgumentException
	{
		if (filteringCharacteristicsChoice.getFilteringCharacteristicsChoice() == FilteringCharacteristicsChoice.NUMBER_OF_CALLS.getFilteringCharacteristicsChoice())
			setNumberOfCalls( filteringValue);
		else if (filteringCharacteristicsChoice.getFilteringCharacteristicsChoice() == FilteringCharacteristicsChoice.INTERVAL.getFilteringCharacteristicsChoice())
			setInterval(filteringValue);
					
	}
	
//---------------------------------------------	

/**
  Gets Filtering Characteristics Choice
 */
 
 	public FilteringCharacteristicsChoice getFilteringCharacteristicsChoice()
	 	{ 
			return filteringCharacteristicsChoice;
		}


/**
  Gets Interval
*/

    public int getInterval() throws ParameterNotSetException
    {
        if(filteringCharacteristicsChoice.getFilteringCharacteristicsChoice() == FilteringCharacteristicsChoice.INTERVAL.getFilteringCharacteristicsChoice())
		{
        	return interval;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }

/**
  Sets Interval
*/

    public void setInterval( int interval) throws IllegalArgumentException
    {  
    if ((interval>=1)&&(interval<=32000))
        {
        	this.interval = interval;
        	filteringCharacteristicsChoice =FilteringCharacteristicsChoice.INTERVAL;
       	}
       	else
       	{  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }
    

//-----------------------

/**
  Gets Number Of Calls
*/

    public int getNumberOfCalls() throws ParameterNotSetException
    {
        if(filteringCharacteristicsChoice.getFilteringCharacteristicsChoice() ==FilteringCharacteristicsChoice.NUMBER_OF_CALLS.getFilteringCharacteristicsChoice())
		{
        	return numberOfCalls;
        }
		else
		{
                throw new ParameterNotSetException();
        }
    }

/**
  Sets Number Of Calls
*/

    public void setNumberOfCalls( int numberOfCalls)
    {
        this.numberOfCalls = numberOfCalls;
        filteringCharacteristicsChoice =FilteringCharacteristicsChoice.NUMBER_OF_CALLS;
    }


//-----------------------
    
}
