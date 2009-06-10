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
 *  File Name     : FacilitySelectedAndAvailable.java
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
This class represents the FacilitySelectedAndAvailable Operation.
*/

public class FacilitySelectedAndAvailable  extends Operation   implements java.io.Serializable {

    
    private DPSpecificCommonParameters dPSpecificCommonParameters;

    private String calledPartyBusinessGroupID;
    private boolean isCalledPartyBusinessGroupID;

    private CalledPartySubaddress calledPartySubaddress;
    private boolean isCalledPartySubaddress;

    private String callingPartyBusinessGroupID;
    private boolean isCallingPartyBusinessGroupID;

    private CallingPartyNumber callingPartyNumber;
    private boolean isCallingPartyNumber;

    private OriginalCalledPartyID originalCalledPartyID;
    private boolean isOriginalCalledPartyID;

    private RedirectingPartyID redirectingPartyID;
    private boolean isRedirectingPartyID;

    private RedirectionInformation redirectionInformation;
    private boolean isRedirectionInformation;

    private RouteList routeList;
    private boolean isRouteList;

    private LocationNumber travellingClassMark;
    private boolean isTravellingClassMark;
    private ComponentType componentType;
    private boolean isComponentType;

    private int componentCorrelationID;
    private boolean isComponentCorrelationID;
    
	private ExtensionField extensions[];
  private boolean isExtensions;  
    
    private Component component;
    private boolean isComponent;     
    
/**
Constructor For FacilitySelectedAndAvailable
*/
	public FacilitySelectedAndAvailable(DPSpecificCommonParameters dPSpecificCommonParameters)
	{
		operationCode = OperationCode.FACILITY_SELECTED_AND_AVAILABLE;
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
 


/** Gets Dp Specific Common Parameters */


    public DPSpecificCommonParameters getDPSpecificCommonParameters() 
    {
        return dPSpecificCommonParameters;
    }

/** Sets Dp Speicifc Common Parameters  */


    public void setDPSpecificCommonParameters( DPSpecificCommonParameters  dPSpecificCommonParameters)
    {
        this.dPSpecificCommonParameters = dPSpecificCommonParameters;
    }

//---------------------------------------------------------------------------------


/** Gets Called Party Business Group ID */


    public java.lang.String getCalledPartyBusinessGroupID() throws ParameterNotSetException
    {
        if(isCalledPartyBusinessGroupIDPresent())
             return calledPartyBusinessGroupID;
        else
             throw new ParameterNotSetException();
    }

/** Sets Called Party Business Group ID */


    public void setCalledPartyBusinessGroupID( String  calledPartyBusinessGroupID)
    {
        this.calledPartyBusinessGroupID=calledPartyBusinessGroupID;
        isCalledPartyBusinessGroupID = true;
    }

/**
 Indicates if the Called Party Business Group ID optional parameter is present .
    Returns: TRUE if present , FALSE otherwise.
*/
    public boolean isCalledPartyBusinessGroupIDPresent()
    {
        return isCalledPartyBusinessGroupID;
    }

//------------------------------------------------------------------------


/** Gets Called Party Subaddress */


    public CalledPartySubaddress getCalledPartySubaddress() throws ParameterNotSetException
    {
        if(isCalledPartySubaddressPresent())
                return calledPartySubaddress;
        else
             throw new ParameterNotSetException();

    }

/** Sets Called Party Subaddress */


    public void setCalledPartySubaddress( CalledPartySubaddress  calledPartySubaddress)
    {
          this.calledPartySubaddress=calledPartySubaddress;
          isCalledPartySubaddress = true;
    }

/**
 Indicates if the Called Party Subaddress optional parameter is present .
    Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isCalledPartySubaddressPresent()
    {
        return isCalledPartySubaddress;
    }


//-------------------------------------------------------------------------------------

/** Gets Calling Party Business Group ID */


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


/** Gets Calling Party Number */


    public CallingPartyNumber getCallingPartyNumber() throws ParameterNotSetException
    {
        if(isCallingPartyNumberPresent())
               return callingPartyNumber;
        else
             throw new ParameterNotSetException();

    }

/** Sets Calling Party Number*/


    public void setCallingPartyNumber( CallingPartyNumber  callingPartyNumber)
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

//-----------------------------------------------------------------------------------------



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
     Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isRedirectionInformationPresent()
    {
        return isRedirectionInformation;

    }


//----------------------------------------------------------------------------------------


/**
Gets Route List
*/
    public RouteList getRouteList() throws ParameterNotSetException
    {
        if(isRouteListPresent())
              return routeList;
        else
             throw new ParameterNotSetException();

    }
/**
Sets Route List
*/
    public void setRouteList(RouteList routeList)
    {
        this.routeList = routeList;
        isRouteList = true;
    }
/**
Indicates if the Route List optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isRouteListPresent()
    {
        return isRouteList;
    }


//-----------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------

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

    public void setComponentType( ComponentType componenttype ) 

        {
	      this.componentType  = componentType;
           isComponentType = true;
    }


/**
 Indicates if the Component Type optional parameter is present .
     Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isComponentTypePresent()
    {
        return isComponentType;

    }

//------------------------------------------------------------------------------------


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
     Returns:TRUE if present , FALSE otherwise.
*/

    public boolean isComponentCorrelationIDPresent()
    {
        return isComponentCorrelationID;

    }
    
//------------------------------------------------------------------------------------


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

