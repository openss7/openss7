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
 *  File Name     : InitiateCallAttempt.java
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
This class represents the InitiateCallAttempt Operation.
*/

public class InitiateCallAttempt   extends Operation   implements java.io.Serializable
{
    
    private DestinationRoutingAddress destinationRoutingAddress;
    
    private String alertingPattern;
    private boolean isAlertingPattern = false ;
    
    private String iSDNAccessRelatedInformation;
    private boolean isISDNAccessRelatedInformation = false ;
    
    private LocationNumber travellingClassMark;
    private boolean isTravellingClassMark = false ;
    
    private String serviceInteractionIndicators;
    private boolean isServiceInteractionIndicators = false ;
    
    private CallingPartyNumber callingPartyNumber;
    private boolean isCallingPartyNumber = false ;
    
    private LegID legToBeCreated ;
    private boolean isLegToBeCreated = false ;
    
    private int newCallSegment;
    private boolean isNewCallSegment=  false ;
    
    private Entry iNServiceCompatibilityResponse;
    private boolean isINServiceCompatibilityResponse = false ;
    
    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private boolean isServiceInteractionIndicatorsTwo = false ;
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ;      

/**
Constructor For InitiateCallAttempt
*/
	public InitiateCallAttempt(DestinationRoutingAddress destinationRoutingAddress)
	{
		operationCode = OperationCode.INITIATE_CALL_ATTEMPT;
		setDestinationRoutingAddress(destinationRoutingAddress);
	}

//----------------------------------------------    
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------

/**
Gets Destination Routing Address
*/
    public DestinationRoutingAddress getDestinationRoutingAddress()
   {
        return destinationRoutingAddress;
    }

/**
Sets Destination Routing Address
*/
    public void setDestinationRoutingAddress(DestinationRoutingAddress destinationRoutingAddress)
    {
        this.destinationRoutingAddress = destinationRoutingAddress;
    }

//----------------------------------------------    

/**
Gets Alerting Pattern
*/
    public java.lang.String getAlertingPattern() throws ParameterNotSetException
    {
        if(isAlertingPatternPresent()) 
            return alertingPattern;
        else
            throw new ParameterNotSetException();  
    }


/**
Sets Alerting Pattern
*/
    public void setAlertingPattern(java.lang.String alertingPattern)
    {
        this.alertingPattern = alertingPattern;
        isAlertingPattern = true;
    }

/**
Indicates if the Alerting Pattern optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isAlertingPatternPresent()
    {
        return isAlertingPattern;
    }

//----------------------------------------------    

/**
Gets ISDN Access Related Information
*/
    public java.lang.String getISDNAccessRelatedInformation() throws ParameterNotSetException
    {
        if(isISDNAccessRelatedInformationPresent())
             return iSDNAccessRelatedInformation;
        else
            throw new ParameterNotSetException();  

    }

/**
Sets ISDN Access Related Information
*/
    public void setISDNAccessRelatedInformation(java.lang.String iSDNAccessRelatedInformation)
    {
        this.iSDNAccessRelatedInformation = iSDNAccessRelatedInformation;
        isISDNAccessRelatedInformation = true;
    }

/**
Indicates if the ISDN Access Related Information optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isISDNAccessRelatedInformationPresent()
    {
        return isISDNAccessRelatedInformation;
    }

//----------------------------------------------    

/**
Gets Travelling Class Mark
*/
    public LocationNumber getTravellingClassMark() throws ParameterNotSetException
    {
        if(isTravellingClassMarkPresent())
            return travellingClassMark;
        else
            throw new ParameterNotSetException();  

    }

/**
Sets Travelling Class Mark
*/
    public void setTravellingClassMark(LocationNumber travellingClassMark)
    {
        this.travellingClassMark = travellingClassMark;
        isTravellingClassMark = true; 
    }

/**
Indicates if the Travelling Class Mark optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isTravellingClassMarkPresent()
    {
        return isTravellingClassMark;
    }

//----------------------------------------------    

/**
Gets Service Interaction Indicators
*/
    public java.lang.String getServiceInteractionIndicators() throws ParameterNotSetException
    {
        if(isServiceInteractionIndicatorsPresent())
            return serviceInteractionIndicators;
        else
            throw new ParameterNotSetException();  

    }

/**
Sets Service Interaction Indicators
*/
    public void setServiceInteractionIndicators(java.lang.String serviceInteractionIndicators)
    {
        this.serviceInteractionIndicators = serviceInteractionIndicators;
        isServiceInteractionIndicators = true;
    }

/**
Indicates if the Service Interaction Indicators optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isServiceInteractionIndicatorsPresent()
    {
        return isServiceInteractionIndicators;
    }

//----------------------------------------------    

/**
Gets Calling Party Number
*/
    public CallingPartyNumber getCallingPartyNumber() throws ParameterNotSetException
    {
        if(isCallingPartyNumberPresent())
             return callingPartyNumber;
        else
            throw new ParameterNotSetException();  

    }

/**
Sets Calling Party Number
*/
    public void setCallingPartyNumber(CallingPartyNumber callingPartyNumber)
    {
        this.callingPartyNumber = callingPartyNumber;
        isCallingPartyNumber = true;
    }

/**
Indicates if the Calling Party Number optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCallingPartyNumberPresent()
    {
        return isCallingPartyNumber;
    }

//----------------------------------------------    

/**
Gets Leg To Be Created
*/
    public LegID getLegToBeCreated() throws ParameterNotSetException
    {
        if(isLegToBeCreatedPresent())
            return legToBeCreated;
        else
            throw new ParameterNotSetException();  

    }

/**
Sets Leg To Be Created
*/
    public void setLegToBeCreated(LegID legToBeCreated)
    {
        this.legToBeCreated = legToBeCreated;
        isLegToBeCreated = true; 
    }

/**
Indicates if the Leg To Be Created optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isLegToBeCreatedPresent()
    {
        return isLegToBeCreated;
    }

//----------------------------------------------    

/**
Gets Call Segment ID
*/
    public int getNewCallSegment() throws ParameterNotSetException
    {
        if(isNewCallSegmentPresent())
            return newCallSegment;
        else
            throw new ParameterNotSetException();  

    }

/**
Sets Call Segment ID
*/
    public void setNewCallSegment(int newCallSegment)
    {
        this.newCallSegment = newCallSegment;
        isNewCallSegment = true;
    }

/**
Indicates if the Call Segment ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isNewCallSegmentPresent()
    {
        return isNewCallSegment;
    }

//----------------------------------------------    

/**
Gets IN Service Compatibility Response
*/
    public Entry getINServiceCompatibilityResponse() throws ParameterNotSetException
    {
        if(isINServiceCompatibilityResponsePresent())
            return iNServiceCompatibilityResponse;
        else
            throw new ParameterNotSetException();  
        
    }

/**
Sets IN Service Compatibility Response
*/
    public void setINServiceCompatibilityResponse(Entry iNServiceCompatibilityResponse)
    {
        this.iNServiceCompatibilityResponse = iNServiceCompatibilityResponse;
        isINServiceCompatibilityResponse = true; 
    }

/**
Indicates if the IN Service Compatibility Response optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isINServiceCompatibilityResponsePresent()
    {
        return isINServiceCompatibilityResponse;
    }

//----------------------------------------------    

/**
Gets Service Interaction Indicators Two
*/
    public ServiceInteractionIndicatorsTwo getServiceInteractionIndicatorsTwo() throws ParameterNotSetException
    {
        if(isServiceInteractionIndicatorsTwoPresent())
            return serviceInteractionIndicatorsTwo;
        else
            throw new ParameterNotSetException();  

    }

/**
Sets Service Interaction Indicators Two
*/
    public void setServiceInteractionIndicatorsTwo(ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo)
    {
        this.serviceInteractionIndicatorsTwo = serviceInteractionIndicatorsTwo;
        isServiceInteractionIndicatorsTwo = true;
    }

/**
Indicates if the Service Interaction Indicators Two optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isServiceInteractionIndicatorsTwoPresent()
    {
        return isServiceInteractionIndicatorsTwo;
    }

//----------------------------------------------    


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
