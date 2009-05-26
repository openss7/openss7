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
 *  File Name     : ForwardServiceInteractionInd.java
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
This Class defines ForwardServiceInteractionInd Datatype
*/

public class ForwardServiceInteractionInd implements Serializable 
{

    private String conferenceTreatmentIndicator;
    private boolean isConferenceTreatmentIndicator = false ;
    
    private String callDiversionTreatmentIndicator;
    private boolean isCallDiversionTreatmentIndicator = false ;

    private String callOfferingTreatmentIndicator;
    private boolean isCallOfferingTreatmentIndicator = false ;

/**
Gets Conference Treatment Indicator
*/
    public String getConferenceTreatmentIndicator() throws ParameterNotSetException
    {
        if(isConferenceTreatmentIndicatorPresent())
		{
        	return conferenceTreatmentIndicator;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }
/**
Sets Conference Treatment Indicator
*/

    public void setConferenceTreatmentIndicator(String conferenceTreatmentIndicator)
    {
        this.conferenceTreatmentIndicator=conferenceTreatmentIndicator;
        isConferenceTreatmentIndicator=true;
    }
/**
Indicates if the Conference Treatment Indicator optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isConferenceTreatmentIndicatorPresent()
    	{
        		return isConferenceTreatmentIndicator;
    	}


//--------------------------------------

/**
Gets Call Diversion Treatment Indicator
*/

    public String getCallDiversionTreatmentIndicator() throws ParameterNotSetException
    {
        if(isCallDiversionTreatmentIndicatorPresent())
		{
        	return callDiversionTreatmentIndicator;
        }
		else
		{
            throw new ParameterNotSetException();
        }  
    }
/**
Sets Call Diversion Treatment Indicator
*/

    public void setCallDiversionTreatmentIndicator(String callDiversionTreatmentIndicator)
    {
        this.callDiversionTreatmentIndicator=callDiversionTreatmentIndicator;
        isCallDiversionTreatmentIndicator=true;
    }
/**
Indicates if the Call Diversion Treatment Indicator optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isCallDiversionTreatmentIndicatorPresent()
    	{
        		return isCallDiversionTreatmentIndicator;
    	}


//--------------------------------------

/**
Gets Call Offering Treatment Indicator
*/

    public String getCallOfferingTreatmentIndicator() throws ParameterNotSetException
    {
        if(isCallOfferingTreatmentIndicatorPresent())
		{
	        return callOfferingTreatmentIndicator;
        }
		else
		{
            throw new ParameterNotSetException();
        }  
    }

/**
Sets Call Offering Treatment Indicator
*/

    public void setCallOfferingTreatmentIndicator(String callOfferingTreatmentIndicator)
    {
        this.callOfferingTreatmentIndicator=callOfferingTreatmentIndicator;
        isCallOfferingTreatmentIndicator=true;
    }

/**
Indicates if the Call Offering Treatment Indicator optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isCallOfferingTreatmentIndicatorPresent()
    	{
        		return isCallOfferingTreatmentIndicator;
    	}


//--------------------------------------

}

