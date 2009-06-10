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
 *  File Name     : CollectInformation.java
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
This class represents the CollectInformation Operation.
*/

public class CollectInformation   extends Operation implements java.io.Serializable
{

    private String alertingPattern;
    private boolean isAlertingPattern = false ;

    private int numberingPlan;
    private boolean isNumberingPlan = false ;

    private OriginalCalledPartyID originalCalledPartyID;
    private boolean isOriginalCalledPartyID = false ;

    private LocationNumber travellingClassMark;
    private boolean isTravellingClassMark = false ;

    private CallingPartyNumber callingPartyNumber;
    private boolean isCallingPartyNumber = false ;

    private CalledPartyNumber dialledDigits;
	 private boolean isDialledDigits = false ;
	 
    private String serviceInteractionIndicators;
    private boolean isServiceInteractionIndicators = false ;

    private Entry iNServiceCompatibilityResponse;
    private boolean isINServiceCompatibilityResponse = false ;

    private ForwardGVNS forwardGVNS;
    private boolean isForwardGVNS = false ;

    private BackwardGVNS backwardGVNS;
    private boolean isBackwardGVNS = false ;

    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private boolean isServiceInteractionIndicatorsTwo = false ;

    private int callSegmentID;
    private boolean isCallSegmentID = false ;

    private LegID legToBeCreated ;
    private boolean isLegToBeCreated = false ;
    
	private ExtensionField extensions[];
  private boolean isExtensions= false ;      

/**
Constructor for CollectInformation
*/

public CollectInformation()
{
		operationCode = OperationCode.COLLECT_INFORMATION;
}

/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------


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

//-----------------------

/**
Gets the Numbering Plan 
*/

    public int getNumberingPlan() throws ParameterNotSetException
    {
        if(isNumberingPlanPresent())
             return numberingPlan;
        else
             throw new ParameterNotSetException();

    }

/**
Sets the Numbering Plan 
*/

    public void  setNumberingPlan(int numberingPlan)
    {
        this.numberingPlan= numberingPlan;
        isNumberingPlan = true;
    }

/**
Indicates if the Numbering Plan optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isNumberingPlanPresent()
    {
        return isNumberingPlan;
    }
    
//-----------------------


/**
Gets Original Called Party ID
*/
    public OriginalCalledPartyID getOriginalCalledPartyID() throws ParameterNotSetException
    {
        if(isOriginalCalledPartyIDPresent())
             return originalCalledPartyID;
        else
             throw new ParameterNotSetException();

    }
/**
Sets Original Called Party ID
*/
    public void setOriginalCalledPartyID(OriginalCalledPartyID originalCalledPartyID)
    {
        this.originalCalledPartyID = originalCalledPartyID;
        isOriginalCalledPartyID = true;
    }
/**
Indicates if the Original Called Party ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isOriginalCalledPartyIDPresent()
    {
        return isOriginalCalledPartyID;
    }

//-----------------------

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

//-----------------------

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

//-----------------------

/**
Gets Dialled Digits
*/
    public CalledPartyNumber getDialledDigits() throws ParameterNotSetException
    {
        if(isDialledDigitsPresent())
             return dialledDigits;
        else
             throw new ParameterNotSetException();

    }
    
/**
Sets Dialled Digits
*/
    public void setDialledDigits(CalledPartyNumber dialledDigits)
    {
        this.dialledDigits = dialledDigits;
        isDialledDigits = true; 
    }
    
/**
Indicates if the Dialled Digits optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isDialledDigitsPresent()
    {
        return isDialledDigits;
    }

//-----------------------

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
    public void setServiceInteractionIndicators(String serviceInteractionIndicators)
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

//-----------------------

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

//-----------------------

/**
Gets Forward Global Virtual Network Services
*/
    public ForwardGVNS getForwardGVNS() throws ParameterNotSetException
    {
        if(isForwardGVNSPresent())
              return forwardGVNS;
        else
              throw new ParameterNotSetException();
 
    }
/**
Sets Forward Global Virtual Network Services
*/
    public void setForwardGVNS(ForwardGVNS forwardGVNS)
    {
        this.forwardGVNS = forwardGVNS;
        isForwardGVNS = true;
    }
/**
Indicates if the Forward Global Virtual Network Services optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isForwardGVNSPresent()
    {
        return isForwardGVNS;
    }

//-----------------------

/**
Gets Backward Global Virtual Network Services
*/
    public BackwardGVNS getBackwardGVNS() throws ParameterNotSetException
    {
        if(isBackwardGVNSPresent()) 
               return backwardGVNS;
        else
               throw new ParameterNotSetException();

    }
/**
Sets Backward Global Virtual Network Services
*/
    public void setBackwardGVNS(BackwardGVNS backwardGVNS)
    {
        this.backwardGVNS = backwardGVNS;
        isBackwardGVNS = true;
    }
/**
Indicates if the Backward Global Virtual Network Services optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isBackwardGVNSPresent()
    {
        return isBackwardGVNS;
    }

//-----------------------

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

//-----------------------

/**
Gets Call Segment ID
*/
    public int getCallSegmentID() throws ParameterNotSetException
    {
        if(isCallSegmentIDPresent())
                return callSegmentID;
        else
                throw new ParameterNotSetException();

    }
/**
Sets Call Segment ID
*/
    public void setCallSegmentID(int callSegmentID)
    {
        this.callSegmentID = callSegmentID;
        isCallSegmentID = true;
    }
/**
Indicates if the Call Segment ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCallSegmentIDPresent()
    {
        return isCallSegmentID;
    }

//-----------------------

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
