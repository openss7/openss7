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
 *  File Name     : EstablishTemporaryConnection.java
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
This class represents the EstablishTemporaryConnection Operation.
*/

public class EstablishTemporaryConnection  extends Operation   implements java.io.Serializable {

	 

    private DigitsGenericNumber assistingSSPIPRoutingAddress;
    
    private DigitsGenericNumber correlationID;
    private boolean isCorrelationID = false ;
	 
    private PartyToConnect partyToConnect;
    private boolean isPartyToConnect = false ;
    
    private ScfID   scfID;
    private boolean isScfID = false ;
    
    private Carrier carrier;
    private boolean isCarrier = false ;
    
    private java.lang.String serviceInteractionIndicators;
    private boolean isServiceInteractionIndicators = false ;
    
    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private boolean isServiceInteractionIndicatorsTwo = false ;
    
	private ExtensionField extensions[];
      private boolean isExtensions = false ;      
    
/**
Constructor For EstablishTemporaryConnection
*/
	public EstablishTemporaryConnection(DigitsGenericNumber assistingSSPIPRoutingAddress)
	{
		operationCode = OperationCode.ESTABLISH_TEMPORARY_CONNECTION;
		setAssistingSSPIPRoutingAddress(assistingSSPIPRoutingAddress);
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
Gets Assisting SSP IP Routing Address
*/

    public DigitsGenericNumber getAssistingSSPIPRoutingAddress() 
    {
        return assistingSSPIPRoutingAddress;
    }

/**
Sets Assisting SSP IP Routing Address
*/


    public void setAssistingSSPIPRoutingAddress (DigitsGenericNumber assistingSSPIPRoutingAddress)
    {
        this.assistingSSPIPRoutingAddress = assistingSSPIPRoutingAddress;
    }

//-----------------------

/**
Gets Correlation ID
*/

    public DigitsGenericNumber getCorrelationID() throws ParameterNotSetException
    {
        if(isCorrelationIDPresent ())
             return correlationID;
        else
             throw new ParameterNotSetException();
    }

/**
Sets Correlation ID
*/


    public void setCorrelationID (DigitsGenericNumber correlationID)
    {
        this.correlationID = correlationID;
        isCorrelationID = true;
    }

/**
 Indicates if the Correlation ID optional parameter is present .
 Returns: TRUE if present , FALSE otherwise.
*/
    public boolean  isCorrelationIDPresent ()
    {
        return isCorrelationID;
    }

//-----------------------

/**
Gets Party To Connect
*/

    public PartyToConnect getPartyToConnect() throws ParameterNotSetException
    {
        if(isPartyToConnectPresent ())
              return partyToConnect;
        else
             throw new ParameterNotSetException();
      
    }

/**
Sets Party To Connect
*/


    public void setPartyToConnect (PartyToConnect partyToConnect)
    {
        this.partyToConnect = partyToConnect;
        isPartyToConnect = true;
    }

/**
 Indicates if the Party To Connect optional parameter is present .
 Returns: TRUE if present , FALSE otherwise.
*/

    public boolean  isPartyToConnectPresent ()
    {
        return isPartyToConnect;
    }

//-----------------------

/**
Gets Scf ID
*/

    public ScfID getScfID() throws ParameterNotSetException
    {
        if(isScfIDPresent ())
             return scfID;
        else
             throw new ParameterNotSetException();

    }

/**
Sets Scf ID
*/


    public void setScfID (ScfID scfID)
    {
        this.scfID = scfID;
        isScfID = true;
    }

/**
 Indicates if the Scf ID optional parameter is present .
 Returns:TRUE if present , FALSE otherwise.
*/

    public boolean  isScfIDPresent ()
    {
        return isScfID;
    }

//-----------------------

/**
Gets Carrier
*/

    public Carrier getCarrier() throws ParameterNotSetException
    {
        if(isCarrierPresent ())
              return carrier;
        else
             throw new ParameterNotSetException();

    }

/**
Sets Carrier
*/


    public void setCarrier (Carrier carrier)
    {
        this.carrier = carrier;
        isCarrier = true;
    }

/**
 Indicates if the Carrier optional parameter is present .
 Returns: TRUE if present , FALSE otherwise.
*/

    public boolean  isCarrierPresent ()
    {
        return isCarrier;
    }
    
//-----------------------

/**
Gets Service Interaction Indicators (CS-1)
*/

    public java.lang.String getServiceInteractionIndicators() throws ParameterNotSetException
    {
        if(isServiceInteractionIndicatorsPresent ()) 
                  return serviceInteractionIndicators;
        else
             throw new ParameterNotSetException();

    }

/**
Sets Service Interaction Indicators (CS-1)
*/


    public void setServiceInteractionIndicators(java.lang.String serviceInteractionIndicators)
    {
        this.serviceInteractionIndicators = serviceInteractionIndicators;
        isServiceInteractionIndicators = true;
    }


/**
 Indicates if the Service Interaction Indicators (CS-1) optional parameter is present .
 Returns: TRUE if present , FALSE otherwise.
*/
    
    public boolean  isServiceInteractionIndicatorsPresent ()
    {
        return isServiceInteractionIndicators;
    }

//-----------------------

/**
Gets Service Interaction Indicators Two (CS-2)
*/

    public ServiceInteractionIndicatorsTwo getServiceInteractionIndicatorsTwo() throws ParameterNotSetException
    {
         if(isServiceInteractionIndicatorsTwoPresent ())
             return serviceInteractionIndicatorsTwo;
        else
             throw new ParameterNotSetException();

    }

/**
Sets Service Interaction Indicators (CS-2)
*/


    public void setServiceInteractionIndicators(ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo)
    {
        this.serviceInteractionIndicatorsTwo = serviceInteractionIndicatorsTwo;
        isServiceInteractionIndicatorsTwo = true;
    }

/**
 Indicates if the Service Interaction Indicators Two (CS-2) optional parameter is present .
 Returns: TRUE if present , FALSE otherwise.
*/

    public boolean  isServiceInteractionIndicatorsTwoPresent ()
    {
        return isServiceInteractionIndicatorsTwo;
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
