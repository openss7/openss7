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
 *  File Name     : SelectFacility.java
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
This class represents the Select Facility Operation.
*/

public class SelectFacility  extends Operation   implements java.io.Serializable
{
	

	private java.lang.String alertingPattern;
	private boolean isAlertingPattern = false ;
	
    private CalledPartyNumber destinationNumberRoutingAddress ;
    private boolean isDestinationNumberRoutingAddress = false ;

    private java.lang.String iSDNAccessRelatedInformation;
    private boolean isISDNAccessRelatedInformation = false ;
    
    private FacilityGroup calledFacilityGroup ;
    private boolean isCalledFacilityGroup= false ;
    
    private int calledFacilityGroupMember ;
    private boolean isCalledFacilityGroupMember = false ;

    private OriginalCalledPartyID originalCalledPartyID;
    private boolean isOriginalCalledPartyID = false ;
    
    private java.lang.String displayInformation;
    private boolean isDisplayInformation = false ;
    
    private java.lang.String serviceInteractionIndicators;
    private boolean isServiceInteractionIndicators = false ;
    
    private Entry iNServiceCompatibilityResponse;
    private boolean isINServiceCompatibilityResponse = false ;

    private ForwardGVNS forwardGVNS;
    private boolean isForwardGVNS = false; 

    private BackwardGVNS backwardGVNS;
    private boolean isBackwardGVNS = false ;
    
    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private boolean isServiceInteractionIndicatorsTwo  = false ;

    private DigitsGenericDigits correlationID;
    private boolean isCorrelationID = false ;

    private ScfID scfID;
    private boolean isScfID = false ;
    
    private int callSegmentID;
    private boolean isCallSegmentID = false ;

    private LegID legToBeCreated ;
    private boolean isLegToBeCreated = false ;
    
	private ExtensionField extensions[];
    private boolean isExtensions = false ;     
    
    
/**
Constructor For SelectFacility
*/

	public SelectFacility() 
	{
		operationCode = OperationCode.SELECT_FACILITY;
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
Gets Alerting Pattern
*/
    public java.lang.String getAlertingPattern() throws ParameterNotSetException
    {
        if(isAlertingPattern()){
        return alertingPattern;
         }else{
               throw new ParameterNotSetException();
            }  
    }

/**
Sets Alerting Pattern
*/
    public void setAlertingPattern(java.lang.String alertingPattern)
    {
        this.alertingPattern = alertingPattern;
        isAlertingPattern=true;
    }

/**
Indicates if the AlertingPattern optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isAlertingPattern()
    {
        return isAlertingPattern;
    }

//-----------------------


/**
Gets DestinationNumberRoutingAddress
*/
    public CalledPartyNumber getDestinationNumberRoutingAddress() throws ParameterNotSetException
    {
        if(isDestinationNumberRoutingAddress()){
        return destinationNumberRoutingAddress;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Destination Number Routing Address
*/
    public void setDestinationNumberRoutingAddress(CalledPartyNumber destinationNumberRoutingAddress)
    {
        this.destinationNumberRoutingAddress = destinationNumberRoutingAddress;
         isDestinationNumberRoutingAddress=true;
    }
/**
Indicates if the DestinationNumberRoutingAddress optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isDestinationNumberRoutingAddress()
    {
        return isDestinationNumberRoutingAddress;
    }

//-----------------------

/**
Gets ISDN Access Related Information
*/
    public java.lang.String getISDNAccessRelatedInformation() throws ParameterNotSetException
    {
        if(isISDNAccessRelatedInformationPresent()){
        return iSDNAccessRelatedInformation;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets ISDN Access Related Information
*/
    public void setISDNAccessRelatedInformation(java.lang.String iSDNAccessRelatedInformation)
    {
        this.iSDNAccessRelatedInformation = iSDNAccessRelatedInformation;
        isISDNAccessRelatedInformation=true;
    }
/**
Indicates if the ISDN Access Related Information optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isISDNAccessRelatedInformationPresent()
    {
        return isISDNAccessRelatedInformation;
    }

//-----------------------


/**
Gets Called Facility Group
*/
    public FacilityGroup getCalledFacilityGroup() throws ParameterNotSetException
    {
        if(isCalledFacilityGroupPresent()){
         return calledFacilityGroup;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Called Facility Group
*/
    public void setCalledFacilityGroup(FacilityGroup calledFacilityGroup)
    {
        this.calledFacilityGroup = calledFacilityGroup;
         isCalledFacilityGroup=true;
    }
/**
Indicates if the Called Facility Group optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCalledFacilityGroupPresent()
    {
        return isCalledFacilityGroup;
    }

//-----------------------
/**
Gets Called Facility Group Member
*/
    public int getCalledFacilityGroupMember() throws ParameterNotSetException
    {
        if(isCalledFacilityGroupMemberPresent()){
        return calledFacilityGroupMember;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Called Facility Group Member
*/
    public void setCalledFacilityGroupMember(int calledFacilityGroupMember)
    {
        this.calledFacilityGroupMember = calledFacilityGroupMember;
        isCalledFacilityGroupMember=true;
    }
/**
Indicates if the Called Facility Group Member optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCalledFacilityGroupMemberPresent()
    {
        return isCalledFacilityGroupMember;
    }

//-----------------------------------


/**
Gets Original Called Party ID
*/
    public OriginalCalledPartyID getOriginalCalledPartyID() throws ParameterNotSetException
    {
        if(isOriginalCalledPartyIDPresent()){
        return originalCalledPartyID;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Original Called Party ID
*/
    public void setOriginalCalledPartyID(OriginalCalledPartyID originalCalledPartyID)
    {
        this.originalCalledPartyID = originalCalledPartyID;
        isOriginalCalledPartyID=true;
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
Gets Display Information
*/
    public java.lang.String getDisplayInformation() throws ParameterNotSetException
    {
       if(isDisplayInformationPresent()){ 
       return displayInformation;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Display Information
*/
    public void setDisplayInformation(java.lang.String displayInformation)
    {
        this.displayInformation = displayInformation;
        isDisplayInformation=true;
    }
/**
Indicates if the Display Information optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isDisplayInformationPresent()
    {
        return isDisplayInformation;
    }

//-----------------------


/**
Gets Service Interaction Indicators
*/
    public java.lang.String getServiceInteractionIndicators() throws ParameterNotSetException
    {
        if(isServiceInteractionIndicatorsPresent()){
        return serviceInteractionIndicators;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Service Interaction Indicators
*/
    public void setServiceInteractionIndicators(java.lang.String serviceInteractionIndicators)
    {
        this.serviceInteractionIndicators = serviceInteractionIndicators;
        isServiceInteractionIndicators=true;
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
        if(isINServiceCompatibilityResponsePresent()){
         return iNServiceCompatibilityResponse;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets IN Service Compatibility Response
*/
    public void setINServiceCompatibilityResponse(Entry iNServiceCompatibilityResponse)
    {
        this.iNServiceCompatibilityResponse = iNServiceCompatibilityResponse;
        isINServiceCompatibilityResponse=true;
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
        if(isForwardGVNSPresent()){
        return forwardGVNS;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Forward Global Virtual Network Services
*/
    public void setForwardGVNS(ForwardGVNS forwardGVNS)
    {
        this.forwardGVNS = forwardGVNS;
        isForwardGVNS=true;
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
        if(isBackwardGVNSPresent()){
         return backwardGVNS;
         }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Backward Global Virtual Network Services
*/
    public void setBackwardGVNS(BackwardGVNS backwardGVNS)
    {
        this.backwardGVNS = backwardGVNS;
         isBackwardGVNS=true;
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
        if(isServiceInteractionIndicatorsTwoPresent()){
        return serviceInteractionIndicatorsTwo;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Service Interaction Indicators Two
*/
    public void setServiceInteractionIndicatorsTwo(ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo)
    {
        this.serviceInteractionIndicatorsTwo = serviceInteractionIndicatorsTwo;
        isServiceInteractionIndicatorsTwo=true;
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
Gets Correlation ID
*/
    public DigitsGenericDigits getCorrelationID() throws ParameterNotSetException
    {
        if(isCorrelationIDPresent()){
         return correlationID;
         }else{
               throw new ParameterNotSetException();
            }  
    }

/**
Sets Correlation ID
*/
    public void setCorrelationID(DigitsGenericDigits correlationID)
    {
        this.correlationID = correlationID;
        isCorrelationID=true;
    }

/**
Indicates if the Correlation ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCorrelationIDPresent()
    {
        return isCorrelationID;
    }

//-----------------------

/**
Gets SCF ID
*/
    public ScfID getScfID() throws ParameterNotSetException
    {
        if(isScfIDPresent()){
          return scfID;
        }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets SCF ID
*/
    public void setScfID(ScfID scfID)
    {
        this.scfID = scfID;
        isScfID=true;
    }
/**
Indicates if the SCF ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isScfIDPresent()
    {
        return isScfID;
    }

//-----------------------

/**
Gets Call Segment ID
*/
    public int getCallSegmentID() throws ParameterNotSetException
    {
        if(isCallSegmentIDPresent()){
         return callSegmentID;
         }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Call Segment ID
*/
    public void setCallSegmentID(int callSegmentID)
    {
        this.callSegmentID = callSegmentID;
        isCallSegmentID=true;
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
        if(isLegToBeCreatedPresent()){
         return legToBeCreated;
         }else{
               throw new ParameterNotSetException();
            }  
    }
/**
Sets Leg To Be Created
*/
    public void setLegToBeCreated(LegID legToBeCreated)
    {
        this.legToBeCreated = legToBeCreated;
        isLegToBeCreated=true;
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
