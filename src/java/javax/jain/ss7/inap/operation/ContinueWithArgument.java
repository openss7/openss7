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
 *  File Name     : ContinueWithArgument.java
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
This class represents the ContinueWithArgument Operation.
*/

public class ContinueWithArgument   extends Operation implements java.io.Serializable
{
    
    private LegID legID;

    private String alertingPattern;
    private boolean isAlertingPattern = false ;

    private String genericName;
    private boolean isGenericName = false ;

    private Entry iNServiceCompatibilityResponse;
    private boolean isINServiceCompatibilityResponse =false ;

    private ForwardGVNS forwardGVNS;
    private boolean isForwardGVNS = false ;

    private BackwardGVNS backwardGVNS;
    private boolean isBackwardGVNS = false ;

    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private boolean isServiceInteractionIndicatorsTwo = false ;
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ;      

/**
Constructor For ContinueWithArgument
*/
	public ContinueWithArgument(LegID legID)
	{
		operationCode = OperationCode.CONTINUE_WITH_ARGUMENT;
		setLegID(legID);
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
Gets Leg ID
*/
    public LegID getLegID() 
    {
        return legID;
    }
/**
Sets Leg ID
*/
    public void setLegID(LegID legID)
    {
        this.legID = legID;
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
Gets Generic Name
*/
    public java.lang.String getGenericName() throws ParameterNotSetException
    {
        if(isGenericNamePresent())
              return genericName;
        else
              throw new ParameterNotSetException();   

    }
/**
Sets Generic Name
*/
    public void setGenericName(String genericName)
    {
        this.genericName = genericName;
        isGenericName = true;
    }
/**
Indicates if the Generic Name optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isGenericNamePresent()
    {
        return isGenericName;
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
    

}
