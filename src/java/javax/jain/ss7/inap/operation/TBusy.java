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
 *  File Name     : TBusy.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap.operation;

import java.io.*;
import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.datatype.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;

/**
This class represents the TBusy Operation.
*/

public class TBusy extends Operation implements Serializable
{
    
    private DPSpecificCommonParameters dPSpecificCommonParameters;

    private Cause busyCause;
    private boolean isBusyCause = false ;

    private String calledPartyBusinessGroupID;
    private boolean isCalledPartyBusinessGroupID =false ;

    private CalledPartySubaddress calledPartySubaddress;
    private boolean isCalledPartySubaddress = false ;

    private OriginalCalledPartyID originalCalledPartyID;
    private boolean isOriginalCalledPartyID = false ;

    private RedirectingPartyID redirectingPartyID;
    private boolean isRedirectingPartyID =false ;

    private RedirectionInformation redirectionInformation;
    private boolean isRedirectionInformation =false ;

    private RouteList routeList;
    private boolean isRouteList =false    ;

    private LocationNumber travellingClassMark;
    private boolean isTravellingClassMark = false ;
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ;    

/**
Constructor For TBusy
*/
	public TBusy(DPSpecificCommonParameters dPSpecificCommonParameters)
	{
		operationCode = OperationCode.T_BUSY;
		this.dPSpecificCommonParameters = dPSpecificCommonParameters;
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
Gets DP Specific Common Parameter
*/
    public DPSpecificCommonParameters getDPSpecificCommonParameters() 
    {
        return dPSpecificCommonParameters;
    }

/**
Sets DP Specific Common Parameter
*/
    public void setDPSpecificCommonParameters(DPSpecificCommonParameters dPSpecificCommonParameters)
    {
        this.dPSpecificCommonParameters = dPSpecificCommonParameters;
    }

//----------------------------------------------    

/**
Gets Busy Cause 
*/
    public Cause getBusyCause() throws ParameterNotSetException
    {
        if(isBusyCausePresent()){
        return busyCause;
        }else{
               throw new ParameterNotSetException();
            }   
    }

/**
Sets Busy Cause 
*/
    public void setBusyCause(Cause busyCause)
    {
        this.busyCause = busyCause;
        isBusyCause=true;
    }

/**
Indicates if the Busy Cause optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isBusyCausePresent()
    {
        return isBusyCause;
    }

//----------------------------------------------    

/**
Gets Called Party Business Group ID
*/
    public String getCalledPartyBusinessGroupID() throws ParameterNotSetException
    {
        if(isCalledPartyBusinessGroupIDPresent()){
        return calledPartyBusinessGroupID;
        }else{
               throw new ParameterNotSetException();
            }   
    }

/**
Sets Called Party Business Group ID
*/
    public void setCalledPartyBusinessGroupID(String calledPartyBusinessGroupID)
    {
        this.calledPartyBusinessGroupID = calledPartyBusinessGroupID;
        isCalledPartyBusinessGroupID=true;
    }

/**
Indicates if the Called Party Business Group ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCalledPartyBusinessGroupIDPresent()
    {
        return isCalledPartyBusinessGroupID;
    }

//----------------------------------------------    

/**
Gets Called Party Sub Address 
*/
    public CalledPartySubaddress getCalledPartySubaddress() throws ParameterNotSetException
    {
        if(isCalledPartySubaddressPresent()){
        return calledPartySubaddress;
        }else{
               throw new ParameterNotSetException();
            }  
    }

/**
Sets Called Party Sub-Address 
*/
    public void setCalledPartySubaddress(CalledPartySubaddress calledPartySubaddress)
    {
        this.calledPartySubaddress = calledPartySubaddress;
        isCalledPartySubaddress=true;
    }

/**
Indicates if the Called Party Sub-Address optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCalledPartySubaddressPresent()
    {
        return isCalledPartySubaddress;
    }

//----------------------------------------------    

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
Gets Redirecting Party ID 
*/
    public RedirectingPartyID getRedirectingPartyID() throws ParameterNotSetException
    {
        if(isRedirectingPartyIDPresent()){
        return redirectingPartyID;
        }else{
               throw new ParameterNotSetException();
            }  
    }

/**
Sets Redirecting Party ID 
*/
    public void setRedirectingPartyID(RedirectingPartyID redirectingPartyID)
    {
        this.redirectingPartyID = redirectingPartyID;
        isRedirectingPartyID=true;
    }

/**
Indicates if the Redirecting Party ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isRedirectingPartyIDPresent()
    {
        return isRedirectingPartyID;
    }

//----------------------------------------------    

/**
Gets Redirection Information
*/
    public RedirectionInformation getRedirectionInformation() throws ParameterNotSetException
    {
        if(isRedirectionInformationPresent()){
        return redirectionInformation;
        }else{
               throw new ParameterNotSetException();
            }  
    }

/**
Sets Redirection Information
*/
    public void setRedirectionInformation(RedirectionInformation redirectionInformation)
    {
        this.redirectionInformation = redirectionInformation;
        isRedirectionInformation=true;
    }

/**
Indicates if the Redirection Information optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isRedirectionInformationPresent()
    {
        return isRedirectionInformation;
    }

//----------------------------------------------    

/**
Gets Route list
*/
    public RouteList getRouteList() throws ParameterNotSetException
    {
        if(isRouteListPresent()){
        return routeList;
        }else{
               throw new ParameterNotSetException();
            }  
    }

/**
Sets Route list
*/
    public void setRouteList(RouteList routeList)   
	  {
        this.routeList = routeList;
        isRouteList=true;
    }

/**
Indicates if the Route list optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isRouteListPresent()
    {
        return isRouteList;
    }

//----------------------------------------------    

/**
Gets Travelling Class Mark 
*/
    public LocationNumber getTravellingClassMark() throws ParameterNotSetException
    {
        if(isTravellingClassMarkPresent()){
        return travellingClassMark;
        }else{
               throw new ParameterNotSetException();
            }  
    }

/**
Sets Travelling Class Mark 
*/
    public void setTravellingClassMark(LocationNumber travellingClassMark)
    {
        this.travellingClassMark = travellingClassMark;
        isTravellingClassMark=true; 
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