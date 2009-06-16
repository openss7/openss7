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
 *  File Name     : CollectedInformation.java
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
 This class represents the CollectedInformation Operation.
*/

public class CollectedInformation   extends Operation implements java.io.Serializable
{


	 private DPSpecificCommonParameters dPSpecificCommonParameters;
	 
    private CalledPartyNumber dialledDigits;
	 private boolean isDialledDigits = false ;
	     
    private java.lang.String callingPartyBusinessGroupID;
    private boolean isCallingPartyBusinessGroupID = false ;
	 
	 private CallingPartySubaddress callingPartySubaddress;
    private boolean isCallingPartySubaddress = false ;
	 
	 private FacilityGroup callingFacilityGroup;
    private boolean isCallingFacilityGroup = false ;
	 
	 private int callingFacilityGroupMember;
    private boolean isCallingFacilityGroupMember = false ; 
	 
	 private OriginalCalledPartyID originalCalledPartyID;
    private boolean isOriginalCalledPartyID = false ;
	 
	 private DigitsGenericDigits prefix;
    private boolean isPrefix = false ;	
	 
	 private RedirectingPartyID redirectingPartyID;
    private boolean isRedirectingPartyID = false ;
	 
	 private RedirectionInformation redirectionInformation;
    private boolean isRedirectionInformation = false ;
	 
	 private LocationNumber travellingClassMark;
    private boolean isTravellingClassMark = false ;
	 
	 private LocationNumber featureCode;
    private boolean isFeatureCode = false ;
	 
	 private LocationNumber accessCode;
    private boolean isAccessCode = false ;
	 
	 private Carrier carrier;
    private boolean isCarrier = false ;
	 
	 private ComponentType componentType;
    private boolean isComponentType = false ;

	 private int componentCorrelationID;
	 private boolean isComponentCorrelationID = false ;
	 
	private ExtensionField extensions[];
  private boolean isExtensions = false ;  
    
    private Component component;
    private boolean isComponent = false ; 
	 
	 
/**
Constructor For CollectedInformation
*/
	public CollectedInformation(DPSpecificCommonParameters dPSpecificCommonParameters)
	{
		operationCode = OperationCode.COLLECTED_INFORMATION;
		setDPSpecificCommonParameters(dPSpecificCommonParameters);
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
Gets Dp Specific Common Parameters
*/
	public DPSpecificCommonParameters getDPSpecificCommonParameters() 
    	{
        		return dPSpecificCommonParameters;
    	}
/**
Sets Dp Specific Common Parameters
*/
	public void setDPSpecificCommonParameters( DPSpecificCommonParameters  dPSpecificCommonParameters)
    	{
        		this.dPSpecificCommonParameters=dPSpecificCommonParameters;
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
	public void setDialledDigits (CalledPartyNumber  dialledDigits)
    	{
        		this.dialledDigits=dialledDigits;
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
Gets Calling Party Business Group ID
*/
	public java.lang.String getCallingPartyBusinessGroupID() throws ParameterNotSetException
    	{
               if(isCallingPartyBusinessGroupIDPresent())
        		return callingPartyBusinessGroupID;
               else
                        throw new ParameterNotSetException();
             
    	}
/**
Sets Calling Party Business Group ID
*/
	public void setCallingPartyBusinessGroupID( java.lang.String  callingPartyBusinessGroupID)
    	{
        		this.callingPartyBusinessGroupID=callingPartyBusinessGroupID;
                        isCallingPartyBusinessGroupID = true;
    	}
/**
Indicates if the Calling Party Business Group ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isCallingPartyBusinessGroupIDPresent()
    	{
        		return isCallingPartyBusinessGroupID;
    	}

//-----------------------

/**
Gets Calling Party Subaddress
*/
	public CallingPartySubaddress getCallingPartySubaddress() throws ParameterNotSetException
    	{
               if(isCallingPartySubaddressPresent())
        		return callingPartySubaddress;
               else
                        throw new ParameterNotSetException();

    	}
/**
Sets Calling Party Subaddress
*/
	public void setCallingPartySubaddress( CallingPartySubaddress  callingPartySubaddress)
    	{
        		this.callingPartySubaddress=callingPartySubaddress;
                        isCallingPartySubaddress = true; 
    	}
/**
Indicates if the Calling Party Subaddress optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isCallingPartySubaddressPresent()
    	{
        		return isCallingPartySubaddress;
    	}

//-----------------------

/**
Gets Calling Facility Group
*/
	public FacilityGroup getCallingFacilityGroup() throws ParameterNotSetException
    	{
                if(isCallingFacilityGroupPresent())
        		return callingFacilityGroup;
                else
                        throw new ParameterNotSetException();

    	}
/**
Sets Calling Facility Group
*/
	public void setCallingFacilityGroup( FacilityGroup  callingFacilityGroup)
    	{
        		this.callingFacilityGroup=callingFacilityGroup;
                        isCallingFacilityGroup = true; 
    	}
/**
Indicates if the Calling Facility Group optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isCallingFacilityGroupPresent()
    	{
        		return isCallingFacilityGroup;
    	}

//-----------------------

/**
Gets Calling Facility Group Member
*/
	public int getCallingFacilityGroupMember() throws ParameterNotSetException
    	{
                  if(isCallingFacilityGroupMemberPresent())
        		return callingFacilityGroupMember;
                  else
                         throw new ParameterNotSetException();

    	}
/**
Sets Calling Facility Group Member
*/
	public void setCallingFacilityGroupMember( int callingFacilityGroupMember )
    	{
        		this.callingFacilityGroupMember=callingFacilityGroupMember;
                        isCallingFacilityGroupMember = true;
    	}
/**
Indicates if the Calling Facility Group Member optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isCallingFacilityGroupMemberPresent()
    	{
        		return isCallingFacilityGroupMember;	
    	}

//-----------------------

/**
Gets Original Called Party ID
*/
	public OriginalCalledPartyID  getOriginalCalledPartyID() throws ParameterNotSetException
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
        		this.originalCalledPartyID=originalCalledPartyID;
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
Gets Prefix Digits
*/
	public DigitsGenericDigits  getPrefix() throws ParameterNotSetException
    	{
                if(isPrefixPresent())
        		return prefix;
                else
                        throw new ParameterNotSetException();
                 
    	}
/**
Sets Prefix Digits
*/
	public void setPrefix(DigitsGenericDigits prefix)
    	{
        	this.prefix=prefix;
                        isPrefix = true;
    	}
/**
Indicates if the Prefix Digits optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isPrefixPresent()
    	{
        		return isPrefix;
    	}

//-----------------------

/**
Gets Redirecting Party ID
*/
	public  RedirectingPartyID  getRedirectingPartyID() throws ParameterNotSetException
    	{
                if(isRedirectingPartyIDPresent())
        		return redirectingPartyID;
                else
                        throw new ParameterNotSetException();

    	}
/**
Sets Redirecting Party ID
*/
	public void setRedirectingPartyID( RedirectingPartyID redirectingPartyID)
    	{
       		this.redirectingPartyID=redirectingPartyID;
                isRedirectingPartyID = true;	
    	}
/**
Indicates if the Redirecting Party ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isRedirectingPartyIDPresent()
    	{
        		return isRedirectingPartyID;	
    	}

//-----------------------

/**
Gets Redirection Information
*/
	public  RedirectionInformation  getRedirectionInformation() throws ParameterNotSetException
    	{
              if(isRedirectionInformationPresent())
        		return redirectionInformation;
               else
                        throw new ParameterNotSetException();

    	}
/**
Sets Redirection Information
*/
	public void setRedirectionInformation( RedirectionInformation redirectionInformation)
    	{
       		this.redirectionInformation=redirectionInformation;
                isRedirectionInformation = true;
    	}
/**
Indicates if the Redirection Information optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean isRedirectionInformationPresent()
    	{
        		return isRedirectionInformation;
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
Returns: TRUE if present, FALSE otherwise.
*/
	public boolean  isCarrierPresent ()
    	{
        		return isCarrier;
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
Gets Feature Code
*/
    	public LocationNumber getFeatureCode() throws ParameterNotSetException
    	{
              if(isFeatureCodePresent())
        		return featureCode;
              else
                        throw new ParameterNotSetException();

    	}
/**
Sets Feature Code
*/
    	public void setFeatureCode(LocationNumber featureCode)
    	{
        		this.featureCode = featureCode;
                        isFeatureCode = true; 
    	}
/**
Indicates if the Feature Code optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    	public boolean isFeatureCodePresent()
    	{
        		return isFeatureCode;
    	}

//-----------------------

/**
Gets Access Code
*/
    	public LocationNumber getAccessCode() throws ParameterNotSetException
    	{
              if(isAccessCodePresent())
        		return accessCode;
              else
                        throw new ParameterNotSetException();

    	}
/**
Sets Access Code
*/
    	public void setAccessCode(LocationNumber accessCode)
    	{
        		this.accessCode = accessCode;
                        isAccessCode = true;
    	}
/**
Indicates if the Access Code optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    	public boolean isAccessCodePresent()
    	{
        		return isAccessCode;
    	}

//-----------------------

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
/**
Sets ComponentType
*/
	public void setComponentType( ComponentType componentType ) 
        {
        	this.componentType  = componentType;
    	    isComponentType = true; 
	//    	Runtime.getRuntime().gc();
    }
/**
Indicates if the ComponentType optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    	public boolean isComponentTypePresent()
    	{
        		return isComponentType;
    	}

//-----------------------

/**
Gets Component Correlation ID
*/
	public  int getComponentCorrelationID() throws ParameterNotSetException
    	{
             if(isComponentCorrelationIDPresent())
        		return componentCorrelationID;
             else
                        throw new ParameterNotSetException();

    	}
/**
Sets Component Correlation ID
*/
	public void setComponentCorrelationID( int componentCorrelationID)
    	{
       		this.componentCorrelationID=componentCorrelationID;
                isComponentCorrelationID = true;
    	}
/**
Indicates if the Component Correlation ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    	public boolean isComponentCorrelationIDPresent()
    	{
        		return isComponentCorrelationID;	
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
        isComponent = true; 
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

