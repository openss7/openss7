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
 *  File Name     : OriginationAttemptAuthorized.java
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
This class represents the OriginationAttemptAuthorized Operation.
*/

public class OriginationAttemptAuthorized  extends Operation   implements java.io.Serializable {

    

    private DPSpecificCommonParameters dPSpecificCommonParameters;
    
    private CalledPartyNumber dialledDigits;
    private boolean isDialledDigits = false;   
    
    private String callingPartyBusinessGroupID;
    private boolean isCallingPartyBusinessGroupID = false ;
    
    private CallingPartySubaddress callingPartySubaddress;
    private boolean isCallingPartySubaddress = false ;
    
    private FacilityGroup callingFacilityGroup;
    private boolean isCallingFacilityGroup = false ;
    
    private int facilityGroupMember;
    private boolean isFacilityGroupMember =false;
    
    private LocationNumber travellingClassMark;
    private boolean isTravellingClassMark = false ;

    private Carrier carrier;
    private boolean isCarrier = false ;
  
    private ComponentType componentType;
    private boolean isComponentType = false ;
    
    private int componentCorrelationID;
    private boolean isComponentCorrelationID =false ;
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ;  
    
    private Component component;
    private boolean isComponent =false ;     

/**
Constructor For OriginationAttemptAuthorised
*/
	public OriginationAttemptAuthorized(DPSpecificCommonParameters dPSpecificCommonParameters)
	{
		operationCode = OperationCode.ORIGINATION_ATTEMPT_AUTHORIZED;
		setDPSpecificCommonParameters(dPSpecificCommonParameters);
	}

//-------------------------------
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------

   
/** Gets Dp Specific Common Parameters */


    public DPSpecificCommonParameters getDPSpecificCommonParameters()
    {
        return dPSpecificCommonParameters;
    }

/** Sets Dp Specific Common Parameters  */


    public void setDPSpecificCommonParameters( DPSpecificCommonParameters  dPSpecificCommonParameters)
    {
        this.dPSpecificCommonParameters=dPSpecificCommonParameters;
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
Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isDialledDigitsPresent()
    {
        return isDialledDigits;

    }

//---------------------------------------------------------------------------------------

/** Gets Calling Party BusinessGroupID */


    public java.lang.String getCallingPartyBusinessGroupID() throws ParameterNotSetException
    {
        if(isCallingPartyBusinessGroupIDPresent())
             return callingPartyBusinessGroupID;
        else
            throw new ParameterNotSetException();

    }

/** Sets Calling Party Business Group ID */


    public void setCallingPartyBusinessGroupID( String  callingPartyBusinessGroupID)
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


    public void setCallingPartySubaddress( CallingPartySubaddress  callingPartySubaddress)
    {
        this.callingPartySubaddress=callingPartySubaddress;
        isCallingPartySubaddress = true;
    }

/**
Indicates if the Calling Party Subaddress  optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isCallingPartySubaddressPresent()
    {
        return isCallingPartySubaddress;
    }

//----------------------------------------------------------------------------------------


/** Gets Calling Facility Group */


    public FacilityGroup getCallingFacilityGroup() throws ParameterNotSetException
    {
        if(isCallingFacilityGroupPresent())
             return callingFacilityGroup;
        else
            throw new ParameterNotSetException();

    }

/** Sets Calling Facility Group */


    public void setCallingFacilityGroup( FacilityGroup  callingFacilityGroup)
    {
        this.callingFacilityGroup=callingFacilityGroup;
        isCallingFacilityGroup = true;
    }

/**
Indicates if the Calling Facility Group optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isCallingFacilityGroupPresent()
    {
        return isCallingFacilityGroup;
    }

//------------------------------------------------------------------------------------------


/** Gets Facility Group Member  */


    public int getFacilityGroupMember() throws ParameterNotSetException
    {
        if(isFacilityGroupMemberPresent())
             return facilityGroupMember;
        else
            throw new ParameterNotSetException();

    }

/** Sets Facility Group Member  */

    public void setFacilityGroupMember( int facilityGroupMember )
    {
        this.facilityGroupMember=facilityGroupMember;
        isFacilityGroupMember = true; 
    }


/**
Indicates if the Facility Group Member optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isFacilityGroupMemberPresent()
    {
        return isFacilityGroupMember;

    }

//--------------------------------------------------------------------------------

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


    public void setCarrier(Carrier carrier)
    {
        this.carrier= carrier;
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

//---------------------------------------------------------------------------------------


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

//----------------------------------------------------------------------------------

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
        this.componentType  = componentType;
           isComponentType = true;
    }


/**
Indicates if the Component Type optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isComponentTypePresent()
    {
        return isComponentType;

    }
    
//--------------------------------------------------------------------------------

/** Gets Component Correlation ID */


    public  int getComponentCorrelationID() throws ParameterNotSetException
    {
        if(isComponentCorrelationIDPresent())
             return componentCorrelationID;
        else
            throw new ParameterNotSetException();

    }

/** Sets Component Correlation ID */

    public void setComponentCorrelationID( int componentCorrelationID)
    {
       this.componentCorrelationID=componentCorrelationID;
       isComponentCorrelationID = true; 
    }


/**
Indicates if the Component Correlation ID optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isComponentCorrelationIDPresent()
    {
        return isComponentCorrelationID;

    }

//--------------------------------------------------------------------------------

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

