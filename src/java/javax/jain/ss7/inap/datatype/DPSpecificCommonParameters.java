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
 *  File Name     : DPSpecificCommonParameters.java
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
This class defines DPSpecificCommonParameters Datatype
*/


public class DPSpecificCommonParameters implements java.io.Serializable 
{


    private ServiceAddressInformation serviceAddressInformation;
    
    private BearerCapability bearerCapability;
    private CalledPartyNumber calledPartyNumber;
    private CallingPartyNumber callingPartyNumber;
    private String callingPartysCategory;
    private String iPSSPCapabilities;
    private String iPAvailable;
    private String iSDNAccessRelatedInformation;
    private CGEncountered cGEncountered;
    private LocationNumber locationNumber;
    private String serviceProfileIdentifier;
    private TerminalType terminalType;
    private LocationNumber chargeNumber;
    private LocationNumber servingAreaID;
    private String serviceInteractionIndicators;
    private INServiceCompatibilityIndication iNServiceCompatibilityIndication;
    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private String uSIServiceIndicator;
    private String uSIInformation;
    private ForwardGVNS forwardGVNS;
    private int createdCallSegmentAssociation;
    private ExtensionField extensions[];
     

    private boolean isBearerCapability = false ;
    private boolean isCalledPartyNumber = false ;
    private boolean isCallingPartyNumber = false ;
    private boolean isCallingPartysCategory = false ;
    private boolean isIPSSPCapabilities = false ;
    private boolean isIPAvailable = false ;
    private boolean isISDNAccessRelatedInformation = false;
    private boolean isCGEncountered =false ;
    private boolean isLocationNumber = false ;
    private boolean isServiceProfileIdentifier = false ;
    private boolean isTerminalType = false ;
    private boolean isChargeNumber = false ;
    private boolean isServingAreaID = false ;
    private boolean isServiceInteractionIndicators =false ;
    private boolean isINServiceCompatibilityIndication = false ;
    private boolean isServiceInteractionIndicatorsTwo = false ;
    private boolean isUSIServiceIndicator = false ;
    private boolean isUSIInformation = false ;
    private boolean isForwardGVNS = false ;
    private boolean isCreatedCallSegmentAssociation = false ;
    private boolean isExtensions = false ; 

/**
Constructor For DPSpecificCommonParameters
*/
	public DPSpecificCommonParameters(ServiceAddressInformation serviceAddressInformation)
	{
		setServiceAddressInformation(serviceAddressInformation);
	}

//---------------------------------------


/** Gets Service Address Information   */


    public ServiceAddressInformation  getServiceAddressInformation()
    {
        return serviceAddressInformation;
    }

 /** Sets Service Address Information   */


    public void setServiceAddressInformation( ServiceAddressInformation serviceAddressInformation)
    {
        this.serviceAddressInformation=serviceAddressInformation;
    }

//-------------------------------------------------------------------------------------

/** Gets Bearer Capability  */


    public BearerCapability getBearerCapability() throws ParameterNotSetException
    {
        if(isBearerCapabilityPresent())
		{
        	return bearerCapability;
        }
		else
		{
            throw new ParameterNotSetException();
        }           
    }

 /** Sets Bearer Capability  */

    public void setBearerCapability( BearerCapability bearerCapability)
    {
        this.bearerCapability=bearerCapability;
        isBearerCapability=true; 
    }

/**
Indicates if the optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isBearerCapabilityPresent()
    {
        return isBearerCapability;

    }



//-----------------------------------------------------------------------------------


/** Gets Called Party Nuber  */


    public CalledPartyNumber getCalledPartyNumber() throws ParameterNotSetException
    {
        if(isCalledPartyNumberPresent())
		{
        	return calledPartyNumber;
        }
		else
		{
            throw new ParameterNotSetException();
        }  
    }

/** Sets Called Party Number  */

    public void setCalledPartyNumber (CalledPartyNumber  calledPartyNumber)
    {
        this.calledPartyNumber=calledPartyNumber;
        isCalledPartyNumber=true;
    }

/**
Indicates if the Called Party Nuber optional parameter is present .
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
		{ 
        	return callingPartyNumber;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }
/** Sets Calling Party Number */

    public void setCallingPartyNumber (CallingPartyNumber  callingPartyNumber)
    {
        this.callingPartyNumber=callingPartyNumber;
        isCallingPartyNumber=true;
    }


/**
Indicates if the Calling Party Number optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isCallingPartyNumberPresent()
    {
        return isCallingPartyNumber;

    }


//--------------------------------------------------------------------------------------



/** Gets Calling Partys Category */


    public java.lang.String getCallingPartysCategory() throws ParameterNotSetException
    {
        if(isCallingPartysCategoryPresent())
		{
        	return callingPartysCategory;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }

/** Sets Calling Partys Category */

    public void setCallingPartysCategory (java.lang.String  callingPartysCategory)
    {
        this.callingPartysCategory=callingPartysCategory;
        isCallingPartysCategory=true;
    }

/**
Indicates if the Calling Partys Category optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/
    public boolean isCallingPartysCategoryPresent()
    {
        return isCallingPartysCategory;

    }


//---------------------------------------------------------------------------------



/** Gets IP SSP Capabilities parameter*/


    public java.lang.String getIPSSPCapabilities() throws ParameterNotSetException
    {
        if( isIPSSPCapabilitiesPresent())
		{
        	return iPSSPCapabilities;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }

/** Sets IP SSP Capabilities parameter*/

    public void setIPSSPCapabilities (java.lang.String iPSSPCapabilities)
    {
        this.iPSSPCapabilities=iPSSPCapabilities;
        isIPSSPCapabilities=true;
    }


/**
Indicates if the IP SSP Capabilities optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isIPSSPCapabilitiesPresent()
    {
        return isIPSSPCapabilities;

    }


//--------------------------------------------------------------------------------

/** Gets IP Available*/


    public java.lang.String getIPAvailable() throws ParameterNotSetException
    {
       if(isIPAvailablePresent())
	   { 
       		return iPAvailable;
       }
	   else
	   {
            throw new ParameterNotSetException();
       } 
    }

/** Sets IP Available*/

    public void setIPAvailable (java.lang.String iPAvailable)
    {
        this.iPAvailable=iPAvailable;
        isIPAvailable=true;
    }


/**
Indicates if the IP Available optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/
    public boolean isIPAvailablePresent()
    {
        return isIPAvailable;

    }



//--------------------------------------------------------------------------------------



/** Gets ISDN Access Related Information */


    public  String getISDNAccessRelatedInformation() throws ParameterNotSetException
    {
        if(isISDNAccessRelatedInformationPresent())
		{
        	return iSDNAccessRelatedInformation;
        }
		else
		{
            throw new ParameterNotSetException();
        }  
    }

/** Sets ISDN Access Related Information */

    public void setISDNAccessRelatedInformation( String iSDNAccessRelatedInformation)
    {
       this.iSDNAccessRelatedInformation=iSDNAccessRelatedInformation;
       isISDNAccessRelatedInformation=true;
    }

/**
Indicates if the ISDN Access Related Information optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/
    public boolean isISDNAccessRelatedInformationPresent()
    {
        return isISDNAccessRelatedInformation;

    }


//-----------------------------------------------------


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
		{
        	return cGEncountered;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }

/** Sets Call Gap Encountered */

    public void setCGEncountered( CGEncountered cGEncountered)

       {
        	this.cGEncountered = cGEncountered;
			isCGEncountered =true;
//    		Runtime.getRuntime().gc();
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

/** Gets Location Number*/


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

/** Sets Location Number*/

    public void setLocationNumber (LocationNumber locationNumber)
    {
        this.locationNumber=locationNumber;
        isLocationNumber=true;
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

/** Gets Service Profile Identifier*/


    public  String  getServiceProfileIdentifier() throws ParameterNotSetException
    {
        if(isServiceProfileIdentifierPresent())
		{
        	return serviceProfileIdentifier;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }

/** Sets Service Profile Identifier*/

    public void setServiceProfileIdentifier( String  serviceProfileIdentifier)
    {
       this.serviceProfileIdentifier = serviceProfileIdentifier;
       isServiceProfileIdentifier=true;
    }


/**
Indicates if the Service Profile Identifier optional parameter is present .
Returns:   TRUE if present , FALSE otherwise.
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
		{
        	return terminalType;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }

/** Sets Terminal Type  */

    public void setTerminalType( TerminalType terminalType) 

        {
        	this.terminalType = terminalType;
			isTerminalType = true;
//    		Runtime.getRuntime().gc();
    }

/**
Indicates if the Terminal Type optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isTerminalTypePresent()
    {
        return isTerminalType;

    }


//---------------------------------------------------------------------------------------


/** Gets Charge Number   */


    public LocationNumber getChargeNumber() throws ParameterNotSetException
    {
        if(isChargeNumberPresent())
		{
        	return chargeNumber;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }

/** Sets Charge Number  */

    public void setChargeNumber( LocationNumber chargeNumber)
    {
        this.chargeNumber=chargeNumber;
        isChargeNumber=true;  
    }

/**
Indicates if the ChargeNumber optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isChargeNumberPresent()
    {
        return isChargeNumber;

    }


//-------------------------------------------------------------------------------


/** Gets Serving Area ID   */


    public LocationNumber getServingAreaID() throws ParameterNotSetException
    {
       if(isServingAreaIDPresent())
	   { 
       		return servingAreaID;
       }
	   else
	   {
            throw new ParameterNotSetException();
       } 
    }

/** Sets Serving AreaID  */

    public void setServingAreaID( LocationNumber servingAreaID)
    {
        this.servingAreaID=servingAreaID;
        isServingAreaID=true; 
    }


/**
Indicates if the Serving AreaID  optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isServingAreaIDPresent()
    {
        return isServingAreaID;

    }



//----------------------------------------------------------------------------


/**Gets Service Interaction Indicators*/


    public  String   getServiceInteractionIndicators() throws ParameterNotSetException
    {
        if(isServiceInteractionIndicatorsPresent())
		{
        	return serviceInteractionIndicators;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }
/**Sets Service Interaction Indicators*/

    public void setServiceInteractionIndicators(java.lang.String   serviceInteractionIndicators)
    {
       this.serviceInteractionIndicators=serviceInteractionIndicators;
       isServiceInteractionIndicators=true; 
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


/** Gets IN Service Compatibility Indication */


    public  INServiceCompatibilityIndication getINServiceCompatibilityIndication() throws ParameterNotSetException
    {
       if( isINServiceCompatibilityIndicationPresent())
	   {
       		return iNServiceCompatibilityIndication;
       }
	   else
	   {
            throw new ParameterNotSetException();
       }  
    }

/** Sets IN Service Compatibility Indication*/

    public void setINServiceCompatibilityIndication( INServiceCompatibilityIndication iNServiceCompatibilityIndication)
    {
       this.iNServiceCompatibilityIndication=iNServiceCompatibilityIndication;
       isINServiceCompatibilityIndication=true;
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

/**
Gets Service Interaction IndicatorsTwo (CS-2)
*/


    public ServiceInteractionIndicatorsTwo  getServiceInteractionIndicatorsTwo() throws ParameterNotSetException
    {
        if(isServiceInteractionIndicatorsTwoPresent())
		{
        	return serviceInteractionIndicatorsTwo;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }

/** Sets Service Interaction IndicatorsTwo (CS-2)    */

    public void setServiceInteractionIndicatorsTwo( ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo)
    {
         this.serviceInteractionIndicatorsTwo=serviceInteractionIndicatorsTwo;
         isServiceInteractionIndicatorsTwo=true;
    }


/**
Indicates if the Service Interaction IndicatorsTwo optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/
    public boolean isServiceInteractionIndicatorsTwoPresent()
    {
        return isServiceInteractionIndicatorsTwo;

    }



//-------------------------------------------------------------------------------------

/**Gets USI Service Indicator*/

    public java.lang.String getUSIServiceIndicator() throws ParameterNotSetException
    {
       if(isUSIServiceIndicatorPresent())
	   { 
       		return uSIServiceIndicator;
       }
	   else
	   {
       	    throw new ParameterNotSetException();
       } 
    }

/**Sets USI Service Indicator*/

    public void setUSIServiceIndicator( String uSIServiceIndicator)
    {
       this.uSIServiceIndicator=uSIServiceIndicator;
       isUSIServiceIndicator=true;
    }

/**
Indicates if the USI Service Indicator optional parameter is present .
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
		{
        	return uSIInformation;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }
    
/**Sets USI Information*/

    public void setUSIInformation( String uSIInformation)
    {
       this.uSIInformation=uSIInformation;
       isUSIInformation=true;
    }


/**
Indicates if the USI Information optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isUSIInformationPresent()
    {
        return isUSIInformation;

    }


// -------------------------------------------------------------------------------


/** Gets Forward GVNS */


    public  ForwardGVNS getForwardGVNS() throws ParameterNotSetException
    {
        if(isForwardGVNSPresent())
		{
        	return forwardGVNS;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
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


//---------------------------------------------------

/** Gets Created Call Segment Association */


    public  int getCreatedCallSegmentAssociation() throws ParameterNotSetException
    {
        if(isCreatedCallSegmentAssociationPresent())
		{
        	return createdCallSegmentAssociation;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
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


//---------------------------------------------------

/**
Gets Extensions Parameter
*/

    public ExtensionField[] getExtensions() throws ParameterNotSetException 
    {
        if(isExtensionsPresent())
		{
        	return extensions;
        }
		else
		{
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
