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
 *  File Name     : FilteredCallTreatment.java
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
import javax.jain.*;

/**
This class represents the FilteredCallTreatment DataType
*/

public class FilteredCallTreatment implements Serializable
{

     private String sFBillingChargingCharacteristics;

	 private InformationToSend informationToSend;
     private boolean isInformationToSend = false ;

	 private int maximumNumberOfCounters;
	 private boolean isMaximumNumberOfCounters = false ;
	 
	 private Cause releaseCause;
	 private boolean isReleaseCause = false ;
	 
/**
Constructor For FilteredCallTreatment
*/
	public FilteredCallTreatment(String sFBillingChargingCharacteristics)
	{
		setSFBillingChargingCharacteristics( sFBillingChargingCharacteristics);
	}
	
//---------------------------------------------	
	 
    
/**
  Gets Service Filtering Information
*/

    public String getSFBillingChargingCharacteristics()
    {
        return sFBillingChargingCharacteristics;
    }
/**
  Sets Service Filtering Information
/*/

	 public void setSFBillingChargingCharacteristics( String sFBillingChargingCharacteristics)
    {
        this.sFBillingChargingCharacteristics = sFBillingChargingCharacteristics;
    }

//-----------------------

/**
  Gets Information To Send 
*/

    public InformationToSend getInformationToSend() throws ParameterNotSetException
    {
        if(isInformationToSendPresent())
		{
        	return informationToSend;
        }
		else
		{
                throw new ParameterNotSetException();
        }
    }

/**
  Sets Information To Send 
*/


    public void setInformationToSend(InformationToSend informationToSend)
    {
        this.informationToSend = informationToSend;
        isInformationToSend=true;
    }
    
/**
Indicates if the Information To Send optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isInformationToSendPresent()
    {
        return isInformationToSend;
    }
    


//-----------------------

/**
  Gets the Maximum Number Of Counters
*/

    public int getMaximumNumberOfCounters() throws ParameterNotSetException
    {
        if(isMaximumNumberOfCountersPresent())
		{
        	return maximumNumberOfCounters;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }

/**
  Sets the Maximum Number Of Counters
*/


    public void setMaximumNumberOfCounters( int maximumNumberOfCounters)
    {
        this.maximumNumberOfCounters = maximumNumberOfCounters;
        isMaximumNumberOfCounters=true;
    }
    
/**
Indicates if the Maximum Number Of Counters optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/

    public boolean isMaximumNumberOfCountersPresent()
    {
        return isMaximumNumberOfCounters;
    }



//-----------------------

/**
  Gets Release Cause 
/*/

    public Cause getReleaseCause() throws ParameterNotSetException
    {
       if(isReleaseCausePresent())
	   { 
       		return releaseCause;
       }
	   else
	   {
            throw new ParameterNotSetException();
       }
    }

/**
  Sets Release Cause
*/

    public void setReleaseCause( Cause releaseCause)
    {
        this.releaseCause = releaseCause;
        isReleaseCause=true;
    }

/**
Indicates if the Release Cause optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/

    public boolean isReleaseCausePresent()
    {
        return isReleaseCause;
    }


//-----------------------
    
}
