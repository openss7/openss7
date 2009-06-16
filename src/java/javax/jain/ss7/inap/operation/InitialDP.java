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
 *  File Name     : InitialDP.java
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
import javax.jain.ss7.inap.exception.*;
import javax.jain.*;

/**
This class represents the InitialDP Operation.
*/


public class InitialDP  extends Operation   implements java.io.Serializable
{
    

    private int serviceKey;
    
    private CalledPartyNumber dialledDigits;
    private boolean isDialledDigits = false ;
    
    private CalledPartyNumber calledPartyNumber;
    private boolean isCalledPartyNumber = false ;
    
    private CallingPartyNumber callingPartyNumber;
    private boolean isCallingPartyNumber = false ;
    
    private java.lang.String callingPartyBusinessGroupID;
    private boolean isCallingPartyBusinessGroupID = false ;
    
    private java.lang.String callingPartysCategory;
    private boolean isCallingPartysCategory = false ;
    
    private CallingPartySubaddress callingPartySubaddress;
    private boolean isCallingPartySubaddress =false ;
    
    private CGEncountered cGEncountered;
    private boolean isCGEncountered = false ;
    
    private java.lang.String iPSSPCapabilities;
    private boolean isIPSSPCapabilities = false ;
    
    private java.lang.String iPAvailable;
    private boolean isIPAvailable = false ;
    
    private LocationNumber locationNumber;
    private boolean isLocationNumber = false ;
    
    private MiscCallInfo miscCallInfo;
    private boolean isMiscCallInfo = false ;
    
    private OriginalCalledPartyID originalCalledPartyID;
    private boolean isOriginalCalledPartyID = false ;
    
    private java.lang.String serviceProfileIdentifier;
    private boolean isServiceProfileIdentifier = false ;
    
    private TerminalType terminalType;
    private boolean isTerminalType = false ;
    
    private TriggerType triggerType;
    
    private HighLayerCompatibility highLayerCompatibility;
    private boolean isHighLayerCompatibility = false ;
    
    private java.lang.String serviceInteractionIndicators;
    private boolean isServiceInteractionIndicators = false ;
    
    private DigitsGenericNumber additionalCallingPartyNumber;
    private boolean isAdditionalCallingPartyNumber = false ;
    
    private ForwardCallIndicators forwardCallIndicators;
    private boolean isForwardCallIndicators = false ;
    
    private BearerCapability bearerCapability;
    private boolean isBearerCapability = false ;
    
    private EventTypeBCSM eventTypeBCSM;
    private boolean isEventTypeBCSM = false ;
    
    private RedirectingPartyID redirectingPartyID;
    private boolean isRedirectingPartyID = false ;
    
    private RedirectionInformation redirectionInformation;
    private boolean isRedirectionInformation = false ;
    
    private Cause cause;
    private boolean isCause = false ;
    
    private ComponentType componentType;
    private boolean isComponentType = false ;

    private int componentCorrelationID;
    private boolean isComponentCorrelationID = false ;
    
    private java.lang.String iSDNAccessRelatedInformation;
    private boolean isISDNAccessRelatedInformation = false ;
    
    private INServiceCompatibilityIndication iNServiceCompatibilityIndication;
    private boolean isINServiceCompatibilityIndication = false ;
    
    private GenericNumbers genericNumbers;
    private boolean isGenericNumbers = false ;
    
    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private boolean isServiceInteractionIndicatorsTwo = false ;
    
    private ForwardGVNS forwardGVNS;
    private boolean isForwardGVNS = false ;
    
    private int createdCallSegmentAssociation;
    private boolean isCreatedCallSegmentAssociation = false ;
    
    private java.lang.String uSIServiceIndicator;
    private boolean isUSIServiceIndicator = false ;
    
    private java.lang.String uSIInformation;
    private boolean isUSIInformation = false ;
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ;  
    
    private Component component;
    private boolean isComponent = false ; 

  
 /**
Constructor For InitialDP
*/

	public InitialDP(int serviceKey,TriggerType triggerType) 
	{
		operationCode = OperationCode.INITIAL_DP;
		try
		{
		setServiceKey(serviceKey);
		setTriggerType(triggerType);
		}catch(IllegalArgumentException ex)
		{
			System.err.println(ex.getMessage());
		}
		
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

    public int  getServiceKey() 
    {
        
           return serviceKey;
        
    }

/** Sets Service Key  */

    public void setServiceKey( int  serviceKey) throws IllegalArgumentException

    {
    	if((serviceKey>=0)&&(serviceKey<=2147483647))
    	{
        this.serviceKey=serviceKey;
      }
      else
      {  
         throw new IllegalArgumentException("ParameterOutOfRange");
      }
    }

//---------------------------------------------------------------------------------

/** Gets Dialled Digits */


    public CalledPartyNumber getDialledDigits() throws ParameterNotSetException
    {
        if(isDialledDigitsPresent())
           return dialledDigits;
        else
            throw new ParameterNotSetException();      
               
    }

/** Sets Dialled Digits  */

    public void setDialledDigits (CalledPartyNumber  dialledDigits)
    {
        this.dialledDigits=dialledDigits;
        isDialledDigits = true;
    }

/**
Indicates if the Dialled Digits optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isDialledDigitsPresent()
    {
        return isDialledDigits;

    }

//-----------------------------------------------------------------------------------

/** Gets Called Party Number  */


    public CalledPartyNumber getCalledPartyNumber() throws ParameterNotSetException
    {
        if(isCalledPartyNumberPresent())
             return calledPartyNumber;
        else
            throw new ParameterNotSetException();      
           
    }

/** Sets Called Party Number  */

    public void setCalledPartyNumber (CalledPartyNumber  calledPartyNumber)
    {
        this.calledPartyNumber=calledPartyNumber;
        isCalledPartyNumber = true;
    }

/**
Indicates if the Called Party Number optional parameter is present .
Returns:TRUE if present , FALSE otherwise.

*/
    public boolean isCalledPartyNumberPresent()
    {
        return isCalledPartyNumber;

    }


//-------------------------------------------------------------------------------------

/** Gets Calling Party Number */


    public CallingPartyNumber getCallingPartyNumber() throws ParameterNotSetException
    {
        if(isCallingPartyNumberPresent())
            return callingPartyNumber;
        else
            throw new ParameterNotSetException();      
               
    }

/** Sets Calling Party Number */

    public void setCallingPartyNumber (CallingPartyNumber  callingPartyNumber)
    {
        this.callingPartyNumber=callingPartyNumber;
        isCallingPartyNumber = true;

    }

/**
Indicates if the Calling Party Number optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isCallingPartyNumberPresent()
    {
        return isCallingPartyNumber;

    }

//--------------------------------------------------------------------------------------

/** Gets  Calling Party Business Group ID */

    public java.lang.String getCallingPartyBusinessGroupID() throws ParameterNotSetException

    {
        if(isCallingPartyBusinessGroupIDPresent()) 
            return callingPartyBusinessGroupID;
        else
            throw new ParameterNotSetException();      
           
    }

/** Sets  Calling Party Business Group ID */

    public void setCallingPartyBusinessGroupID (java.lang.String  callingPartyBusinessGroupID)
    {
        this.callingPartyBusinessGroupID=callingPartyBusinessGroupID;
        isCallingPartyBusinessGroupID = true;
    }

/**
Indicates if the Calling Party Business Group ID optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isCallingPartyBusinessGroupIDPresent()
    {
        return isCallingPartyBusinessGroupID;

    }

//----------------------------------------------------------------------------------


/** Gets Calling Party's Category */


    public java.lang.String getCallingPartysCategory() throws ParameterNotSetException

    {
        if(isCallingPartysCategoryPresent())
             return callingPartysCategory;
        else
            throw new ParameterNotSetException();      
          
    }

/** Sets Calling Party's Category */

    public void setCallingPartysCategory (java.lang.String  callingPartysCategory)
    {
        this.callingPartysCategory=callingPartysCategory;
        isCallingPartysCategory = true;
    }

/**
Indicates if the Calling Party's Category optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isCallingPartysCategoryPresent()
    {
        return isCallingPartysCategory;

    }

//---------------------------------------------------------------------------------


/** Gets Calling Party Subaddress */


    public CallingPartySubaddress getCallingPartySubaddress() throws ParameterNotSetException

    {
        if(isCallingPartySubaddressPresent())        
            return callingPartySubaddress;
        else
            throw new ParameterNotSetException();      
            
    }

/** Sets Calling Party Subaddress */

    public void setCallingPartySubaddress (CallingPartySubaddress  callingPartySubaddress)
    {
        this.callingPartySubaddress=callingPartySubaddress;
        isCallingPartySubaddress = true;
    }


/**
Indicates if the Calling Party Subaddress optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isCallingPartySubaddressPresent()
    {
        return isCallingPartySubaddress;

    }


//----------------------------------------------------------------

/**<P>
Gets Call Gap Encountered .
<DT> Indicates the type of automatic Call Gapping encountered, if any:
<LI>NO_CG_ENCOUNTERED
<LI>MANUAL_CG_ENCOUNTERED
<LI>SCP_OVERLOAD
<P>
*/

    public CGEncountered getCGEncountered() throws ParameterNotSetException

    {
        if(isCGEncounteredPresent())
           return cGEncountered;
        else
            throw new ParameterNotSetException();      
           
    }

/** Sets Call Gap Encountered */

    public void setCGEncountered( CGEncountered cGEncountered) 

        {
        this.cGEncountered	 = cGEncountered;
	      isCGEncountered = true;
//    	Runtime.getRuntime().gc();
    }


/**
Indicates if the Call Gap Encountered optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isCGEncounteredPresent()
    {
        return isCGEncountered;

    }


//-----------------------------------------------------------------------------------------


/** Gets IP SSP Capabilities */


    public java.lang.String getIPSSPCapabilities() throws ParameterNotSetException

    {
        if(isIPSSPCapabilitiesPresent())
             return iPSSPCapabilities;
        else
            throw new ParameterNotSetException();      
           
    }

/** Sets IP SSP Capabilities */

    public void setIPSSPCapabilities (java.lang.String iPSSPCapabilities)
    {
        this.iPSSPCapabilities=iPSSPCapabilities;
        isIPSSPCapabilities = true; 
    }

/**
Indicates if the IP SSP Capabilities optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isIPSSPCapabilitiesPresent()
    {
        return isIPSSPCapabilities;

    }


//---------------------------------------------------------------------------------------


/** Gets  IPAvailable */


    public java.lang.String getIPAvailable() throws ParameterNotSetException

    {
        if(isIPAvailablePresent()) 
            return iPAvailable;
        else
            throw new ParameterNotSetException();      
           
    }

/** Sets  IPAvailable */

    public void setIPAvailable (java.lang.String iPAvailable)
    {
        this.iPAvailable=iPAvailable;
        isIPAvailable = true;
    }


/**
Indicates if the IPAvailable optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isIPAvailablePresent()
    {
        return isIPAvailable;

    }

//----------------------------------------------------------------------------------

/** Gets Location Number*/


    public LocationNumber getLocationNumber() throws ParameterNotSetException

    {
        if(isLocationNumberPresent()) 
             return locationNumber;
        else
             throw new ParameterNotSetException();      
          
    }

/** Sets Location Number*/

    public void setLocationNumber (LocationNumber locationNumber)
    {
        this.locationNumber=locationNumber;
        isLocationNumber = true;

    }

/**
Indicates if the Location Number optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isLocationNumberPresent()
    {
        return isLocationNumber;

    }

//-----------------------------------------------------------------------------------



/** Gets Misc Call Info */


    public MiscCallInfo getMiscCallInfo() throws ParameterNotSetException

    {
        if(isMiscCallInfoPresent()) 
            return miscCallInfo;
        else
             throw new ParameterNotSetException();      
          
    }
/** Sets Misc Call Info */

    public void setMiscCallInfo( MiscCallInfo miscCallInfo)
    {
        this.miscCallInfo=miscCallInfo;
        isMiscCallInfo = true;

    }

/**
Indicates if the Misc Call Info optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isMiscCallInfoPresent()
    {
        return isMiscCallInfo;

    }


//----------------------------------------------------------------------------------

/** Gets Original Called Party ID*/


    public OriginalCalledPartyID  getOriginalCalledPartyID() throws ParameterNotSetException

    {
        if(isOriginalCalledPartyIDPresent())
           return originalCalledPartyID;
        else
             throw new ParameterNotSetException();      
          
    }

/** Sets Original Called Party ID*/

    public void setOriginalCalledPartyID(OriginalCalledPartyID originalCalledPartyID)
    {
        this.originalCalledPartyID=originalCalledPartyID;
        isOriginalCalledPartyID = true;
    }

/**
Indicates if the Original Called Party ID optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isOriginalCalledPartyIDPresent()
    {
        return isOriginalCalledPartyID;

    }


//---------------------------------------------------------------------------

/** Gets Service Profile Identifier*/


    public  java.lang.String  getServiceProfileIdentifier() throws ParameterNotSetException

    {
        if(isServiceProfileIdentifierPresent())
             return serviceProfileIdentifier;
        else
             throw new ParameterNotSetException();      
          
    }

/** Sets Service Profile Identifier*/

    public void setServiceProfileIdentifier( java.lang.String  serviceProfileIdentifier)
    {
       this.serviceProfileIdentifier=serviceProfileIdentifier;
       isServiceProfileIdentifier = true;
    }

/**
Indicates if the Service Profile Identifier optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isServiceProfileIdentifierPresent()
    {
        return isServiceProfileIdentifier;
    }

//----------------------------------------------------------------------------------

/**<P>
Gets Terminal Type.
<DT> This parameter gets the Terminal Type so that the SCF can specify, to the SRF, 
appropriate type of capability (voice recognition, DTMF, display capability etc.).
<LI>UNKNOWN
<LI>DIAL_PULSE	
<LI>DTMF
<LI>ISDN
<LI>ISDN_NO_DTMF	
<LI>SPARE	
<P>
*/

    public TerminalType getTerminalType() throws ParameterNotSetException

    {
        if(isTerminalTypePresent())
            return terminalType;
        else
             throw new ParameterNotSetException();      
        
    }

/** Sets Terminal Type  */

    public void setTerminalType( TerminalType terminalType) 
    {
    	this.terminalType  = terminalType;
          isTerminalType = true;
    //  	Runtime.getRuntime().gc();
    }

/**
Indicates if the Terminal Type optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isTerminalTypePresent()
    {
        return isTerminalType;

    }

//-------------------------------------------------------------------------------

/** Gets Trigger Type */

	 public TriggerType getTriggerType() 
    {
       return triggerType;
    }
/** Sets Trigger Type */

    public void setTriggerType( TriggerType triggerType) 
    {
        this.triggerType	= triggerType;
    }

//----------------------------------------------------------------------------

/** Gets High Layer Compatibility*/


    public  HighLayerCompatibility  getHighLayerCompatibility() throws ParameterNotSetException

    {
        if(isHighLayerCompatibilityPresent()) 
            return highLayerCompatibility;
        else
             throw new ParameterNotSetException();      
         
    }

/** Sets High Layer Compatibility*/

    public void setHighLayerCompatibility( HighLayerCompatibility highLayerCompatibility)
    {
       this.highLayerCompatibility=highLayerCompatibility;
       isHighLayerCompatibility = true;
    }

/**
Indicates if the High Layer Compatibility optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isHighLayerCompatibilityPresent()
    {
        return isHighLayerCompatibility;

    }

//----------------------------------------------------------------------------------------

/**Gets Service Interaction Indicators*/


    public  java.lang.String   getServiceInteractionIndicators() throws ParameterNotSetException

    {
        if(isServiceInteractionIndicatorsPresent()) 
             return serviceInteractionIndicators;
        else
             throw new ParameterNotSetException();      
       
    }
/**Sets Service Interaction Indicators*/

    public void setServiceInteractionIndicators(java.lang.String   serviceInteractionIndicators)
    {
       this.serviceInteractionIndicators=serviceInteractionIndicators;
       isServiceInteractionIndicators = true;
    }

/**
Indicates if the Service Interaction Indicators optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isServiceInteractionIndicatorsPresent()
    {
        return isServiceInteractionIndicators;

    }

//------------------------------------------------------------------------------------------

/** Gets Additional Calling Party Number*/


    public  DigitsGenericNumber   getAdditionalCallingPartyNumber() throws ParameterNotSetException

    {
        if(isAdditionalCallingPartyNumberPresent()) 
            return additionalCallingPartyNumber;
        else
             throw new ParameterNotSetException();      
          
    }

/** Sets Additional Calling Party Number*/

    public void setAdditionalCallingPartyNumber( DigitsGenericNumber additionalCallingPartyNumber)
    {
       this.additionalCallingPartyNumber=additionalCallingPartyNumber;
       isAdditionalCallingPartyNumber = true;
    }

/**
Indicates if the Additional Calling Party Number optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isAdditionalCallingPartyNumberPresent()
    {
        return isAdditionalCallingPartyNumber;

    }

//---------------------------------------------------------------------------------------

/** Gets Forward Call Indicators*/


    public  ForwardCallIndicators   getForwardCallIndicators() throws ParameterNotSetException

    {
        if(isForwardCallIndicatorsPresent()) 
             return forwardCallIndicators;
        else
             throw new ParameterNotSetException();      
          
    }

/** Sets Forward Call Indicators*/

    public void setForwardCallIndicators( ForwardCallIndicators forwardCallIndicators)
    {
       this.forwardCallIndicators=forwardCallIndicators;
	   isForwardCallIndicators = true;
    }

/**
Indicates if the Forward Call Indicators optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isForwardCallIndicatorsPresent()
    {
        return isForwardCallIndicators;

    }

//------------------------------------------------------------------------------------


/** Gets Bearer Capability  */

    public BearerCapability getBearerCapability() throws ParameterNotSetException

    {
        if(isBearerCapabilityPresent()) 
             return bearerCapability;
        else
             throw new ParameterNotSetException();      
          
    }

/** Sets Bearer Capability  */

    public void setBearerCapability( BearerCapability bearerCapability)
    {
        this.bearerCapability=bearerCapability;
        isBearerCapability = true;
    }

/**
Indicates if the Bearer Capability optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isBearerCapabilityPresent()
    {
        return isBearerCapability;

    }

//-----------------------------------------------------------------------

/**<P>
Gets Event Type BCSM.
<DT> This parameter gets the BCSM detection point event. 
<LI>ORIG_ATTEMPT_AUTHORIZED
<LI>COLLECTED_INFO	
<LI>ANALYSED_INFORMATION
<LI>ROUTE_SELECT_FAILURE
<LI>O_CALLED_PARTY_BUSY	
<LI>O_NO_ANSWER
<LI>O_ANSWER
<LI>O_MID_CALL	
<LI>O_DISCONNECT
<LI>O_ABANDON
<LI>TERM_ATTEMPT_AUTHORIZED	
<LI>T_BUSY
<LI>T_NO_ANSWER
<LI>T_ANSWER	
<LI>T_MID_CALL
<LI>T_DISCONNECT
<LI>T_ABANDON	
<LI>O_TERM_SEIZED
<LI>O_SUSPENDED	
<LI>T_SUSPENDED
<LI>ORIG_ATTEMPT	
<LI>TERM_ATTEMPT
<LI>O_RE_ANSWER	
<LI>T_RE_ANSWER
<LI>FACILITY_SELECTED_AND_AVAILABLE	
<LI>CALL_ACCPETED
<P>
*/

    public EventTypeBCSM getEventTypeBCSM () throws ParameterNotSetException

    {
        if(isEventTypeBCSMPresent()) 
             return eventTypeBCSM ;
        else
             throw new ParameterNotSetException();      
         
    }

/** Sets Event Type BCSM */

    public void setEventTypeBCSM ( EventTypeBCSM eventTypeBCSM )
   {
    this.eventTypeBCSM  = eventTypeBCSM;

	      isEventTypeBCSM = true;
    }


/**
Indicates if the Event Type BCSM optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isEventTypeBCSMPresent()
    {
        return isEventTypeBCSM;

    }


//--------------------------------------------------------------------------------------

/** Gets Redirecting Party ID */


    public  RedirectingPartyID  getRedirectingPartyID() throws ParameterNotSetException

    {
        if(isRedirectingPartyIDPresent()) 
             return redirectingPartyID;
        else
             throw new ParameterNotSetException();      
          
    }

/** Sets Redirecting Party ID */

    public void setRedirectingPartyID( RedirectingPartyID redirectingPartyID)
    {
       this.redirectingPartyID=redirectingPartyID;
       isRedirectingPartyID = true;
    }

/**
Indicates if the Redirecting Party ID optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isRedirectingPartyIDPresent()
    {
        return isRedirectingPartyID;

    }

//----------------------------------------------------------------------------


/** Gets Redirection Information */


    public  RedirectionInformation  getRedirectionInformation() throws ParameterNotSetException

    {
        if(isRedirectionInformationPresent())
             return redirectionInformation;
        else
             throw new ParameterNotSetException();      
           
    }

/** Sets Redirection Information */

    public void setRedirectionInformation( RedirectionInformation redirectionInformation)
    {
       this.redirectionInformation=redirectionInformation;
       isRedirectionInformation = true;
    }

/**
Indicates if the Redirection Information optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isRedirectionInformationPresent()
    {
        return isRedirectionInformation;

    }

//-------------------------------------------------------------------------------------

/** Gets Cause */

    public  Cause  getCause() throws ParameterNotSetException

    {
         if(isCausePresent())
             return cause;
         else
             throw new ParameterNotSetException();      
             
    }
    
/** Sets Cause */

    public void setCause( Cause cause)
    {
       this.cause=cause;
       isCause = true;
    }

/**
Indicates if the Cause optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isCausePresent()
    {
        return isCause;

    }

//-----------------------------------------------------------------------------

/**<P>
Gets ComponentType.
<DT> This parameter indicates the type of event that is reported to the SSF: 
<LI>ANY
<LI>INVOKE	
<LI>R_RESULT
<LI>R_ERROR
<LI>R_REJECT	
<LI>R_RESULT_NOT_LAST	
<P>
*/

    public ComponentType getComponentType() throws ParameterNotSetException

    {
        if(isComponentTypePresent())
             return componentType;
        else
             throw new ParameterNotSetException();      
            
    }

/** Sets Component Type  */

    public void setComponentType( ComponentType componentType )
    {
        this.componentType=componentType;
        isComponentType = true;

    }

/**
Indicates if the Component Type  optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isComponentTypePresent()
    {
        return isComponentType;

    }

//---------------------------------------------------------------------------------

/** Gets Component CorrelationID */


    public  int getComponentCorrelationID() throws ParameterNotSetException

    {
        if(isComponentCorrelationIDPresent())
             return componentCorrelationID;
        else
             throw new ParameterNotSetException();      
           
    }

/** Sets Component CorrelationID */

    public void setComponentCorrelationID( int componentCorrelationID)
    {
       this.componentCorrelationID=componentCorrelationID;
       isComponentCorrelationID = true;
    }

/**
Indicates if the Component CorrelationID optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isComponentCorrelationIDPresent()
    {
        return isComponentCorrelationID;

    }


//-----------------------------------------------------------------------------------

/** Gets ISDN Access Related Information */


    public  java.lang.String getISDNAccessRelatedInformation() throws ParameterNotSetException

    {
        if(isISDNAccessRelatedInformationPresent()) 
             return iSDNAccessRelatedInformation;
        else
             throw new ParameterNotSetException();      
           
    }

/** Sets ISDN Access Related Information */

    public void setISDNAccessRelatedInformation( java.lang.String iSDNAccessRelatedInformation)
    {
       this.iSDNAccessRelatedInformation=iSDNAccessRelatedInformation;
       isISDNAccessRelatedInformation = true;
    }

/**
Indicates if the ISDN Access Related Information optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isISDNAccessRelatedInformationPresent()
    {
        return isISDNAccessRelatedInformation;

    }

//---------------------------------------------------------------------------------

/** Gets IN Service Compatibility Indication */


    public  INServiceCompatibilityIndication getINServiceCompatibilityIndication() throws ParameterNotSetException

    {
        if(isINServiceCompatibilityIndicationPresent())
             return iNServiceCompatibilityIndication;
        else
             throw new ParameterNotSetException();      
           
    }

/** Sets IN Service Compatibility Indication*/

    public void setINServiceCompatibilityIndication( INServiceCompatibilityIndication iNServiceCompatibilityIndication)
    {
       this.iNServiceCompatibilityIndication=iNServiceCompatibilityIndication;
       isINServiceCompatibilityIndication = true;
    }

/**
Indicates if the IN Service Compatibility Indication optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isINServiceCompatibilityIndicationPresent()
    {
        return isINServiceCompatibilityIndication;

    }



//------------------------------------------------------------------------------------------

/** Gets Generic Numbers   */


    public GenericNumbers  getGenericNumbers() throws ParameterNotSetException

    {
        if(isGenericNumbersPresent()) 
             return genericNumbers;
        else
             throw new ParameterNotSetException();      
          
    }

/** Sets Generic Numbers   */

    public void setGenericNumbers( GenericNumbers  genericNumbers)
    {
        this.genericNumbers=genericNumbers;
        isGenericNumbers = true;
    }

/**
Indicates if the Generic Numbers optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isGenericNumbersPresent()
    {
        return isGenericNumbers;

    }


//-------------------------------------------------------------------------------------

/** Gets Service Interaction Indicators Two (CS-2)    */


    public ServiceInteractionIndicatorsTwo  getServiceInteractionIndicatorsTwo() throws ParameterNotSetException

    {
         if(isServiceInteractionIndicatorsTwoPresent())
             return serviceInteractionIndicatorsTwo;
         else
             throw new ParameterNotSetException();      
           
    }

/** Sets Service Interaction Indicators Two (CS-2)    */

    public void setServiceInteractionIndicatorsTwo( ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo)
    {
         this.serviceInteractionIndicatorsTwo=serviceInteractionIndicatorsTwo;
         isServiceInteractionIndicatorsTwo = true;
    }


/**
Indicates if the Service Interaction Indicators Two optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isServiceInteractionIndicatorsTwoPresent()
    {
        return isServiceInteractionIndicatorsTwo;

    }


//-------------------------------------------------------------------------------------


/** Gets Forward GVNS */


    public  ForwardGVNS getForwardGVNS() throws ParameterNotSetException

    {
        if(isForwardGVNSPresent())
            return forwardGVNS;
        else
             throw new ParameterNotSetException(); 
    }
/** Sets Forward GVNS */

    public void setForwardGVNS( ForwardGVNS forwardGVNS)
    {
      this.forwardGVNS=forwardGVNS;
      isForwardGVNS=true;

    }


/**
Indicates if the Forward GVNS optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isForwardGVNSPresent()
    {
        return isForwardGVNS;

    }

//-------------------------------------------------------------------------------------

/** Gets Created Call Segment Association */


    public  int getCreatedCallSegmentAssociation() throws ParameterNotSetException

    {
        if(isCreatedCallSegmentAssociationPresent())

          return createdCallSegmentAssociation;
         else
             throw new ParameterNotSetException(); 
    }

/** Sets Created Call Segment Association */

    public void setCreatedCallSegmentAssociation( int  createdCallSegmentAssociation)
    {
       this.createdCallSegmentAssociation=createdCallSegmentAssociation;
       isCreatedCallSegmentAssociation=true;

    }

/**
Indicates if the Created Call Segment Association optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isCreatedCallSegmentAssociationPresent()
    {
        return isCreatedCallSegmentAssociation;

    }

//------------------------------------------------------------------------------

/**Gets USI Service Indicator*/


    public java.lang.String getUSIServiceIndicator() throws ParameterNotSetException

    {
        if(isUSIServiceIndicatorPresent())

        return uSIServiceIndicator;
        else
             throw new ParameterNotSetException(); 
    }
    
/**Sets USI Service Indicator*/

    public void setUSIServiceIndicator( java.lang.String uSIServiceIndicator)
    {
       this.uSIServiceIndicator=uSIServiceIndicator;
       isUSIServiceIndicator=true;

    }

/**
Indicates if the Sets USI Service Indicator optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isUSIServiceIndicatorPresent()
    {
        return isUSIServiceIndicator;

    }


//----------------------------------------------------------------------------------

/**Gets USI Information*/

    public java.lang.String getUSIInformation() throws ParameterNotSetException

    {
        if(isUSIInformationPresent())

         return uSIInformation;
         else
             throw new ParameterNotSetException();
    }

/**Sets USI Information*/

    public void setUSIInformation( java.lang.String uSIInformation)
    {
       this.uSIInformation=uSIInformation;
       isUSIInformation=true;

    }

/**
Indicates if the USI Information optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isUSIInformationPresent()
    {
        return isUSIInformation;

    }
    
//----------------


/**
Gets Extensions Parameter
*/

    public ExtensionField[] getExtensions()throws ParameterNotSetException

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


/**
Gets Component
*/

    public Component getComponent() throws ParameterNotSetException

    {
        if(isComponentPresent())

        return component;
        else
             throw new ParameterNotSetException();
    }


/**
Sets Component
*/

    public void  setComponent (Component component)
    {
        this.component = component;
        isComponent=true;

    }

//-----------------------

/**
Indicates if the Component optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isComponentPresent()
    {
        return isComponent;
    }
    

}



