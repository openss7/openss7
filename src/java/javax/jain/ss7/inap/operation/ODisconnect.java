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
 *  File Name     : ODisconnect.java
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
This class represents the ODisconnect Operation.
*/

public class ODisconnect  extends Operation   implements java.io.Serializable
{
    

    private DPSpecificCommonParameters dPSpecificCommonParameters;
    
    private String callingPartyBusinessGroupID;
    private boolean isCallingPartyBusinessGroupID = false ;
    
    private CallingPartySubaddress callingPartySubaddress;
    private boolean isCallingPartySubaddress = false ;
    
    private FacilityGroup callingFacilityGroup;
    private boolean isCallingFacilityGroup = false ;
    
    private int callingFacilityGroupMember;
    private boolean isCallingFacilityGroupMember = false ;
    
    private Cause releaseCause;
    private boolean isReleaseCause = false ;	
    
    private RouteList routeList;
    private boolean isRouteList = false ;
    
    private Carrier carrier;
    private boolean isCarrier = false ;
    
    private int connectTime;
    private boolean isConnectTime = false ;
    
    private ComponentType componentType;
    private boolean isComponentType =false ;
    
    private int componentCorrelationID;
    private boolean isComponentCorrelationID = false ;

	private ExtensionField extensions[];
  private boolean isExtensions = false ;  
    
    private Component component;
    private boolean isComponent = false ; 

//----------------------

/**
Constructor For ODisconnect
*/
	public ODisconnect(DPSpecificCommonParameters dPSpecificCommonParameters)
	{
		operationCode = OperationCode.O_DISCONNECT;
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
    public void setCallingPartyBusinessGroupID(java.lang.String callingPartyBusinessGroupID)
    {
        this.callingPartyBusinessGroupID = callingPartyBusinessGroupID;
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

//----------------------------------------------  

/**
Gets Calling Party Sub-Address 
*/
    public CallingPartySubaddress getCallingPartySubaddress() throws ParameterNotSetException
    {
        if(isCallingPartySubaddressPresent())
                return callingPartySubaddress;
        else
                throw new ParameterNotSetException();  

    }

/**
Sets Calling Party Sub-Address 
*/
    public void setCallingPartySubaddress(CallingPartySubaddress callingPartySubaddress)
    {
        this.callingPartySubaddress = callingPartySubaddress;
        isCallingPartySubaddress = true;
    }

/**
Indicates if the Calling Party Sub-Address optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCallingPartySubaddressPresent()
    {
        return isCallingPartySubaddress;
    }

//----------------------------------------------  

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
Sets Calling Facility Group parameter
*/
    public void setCallingFacilityGroup(FacilityGroup callingFacilityGroup)
    {
        this.callingFacilityGroup = callingFacilityGroup;
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

//----------------------------------------------  

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
    public void setCallingFacilityGroupMember(int callingFacilityGroupMember) throws IllegalArgumentException
    { 
    if((callingFacilityGroupMember >=0) &&( callingFacilityGroupMember <=2147483647) )
      {
        this.callingFacilityGroupMember = callingFacilityGroupMember;
        isCallingFacilityGroupMember = true; 
      }
    else
       {  
        throw new IllegalArgumentException("ParameterOutOfRange");
       }    
    }
/**
Indicates if the Calling Facility Group Member optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCallingFacilityGroupMemberPresent()
    {
        return isCallingFacilityGroupMember;
    }

//----------------------------------------------  

/**
Gets Release Cause 
*/
    public Cause getReleaseCause() throws ParameterNotSetException
    {
        if(isReleaseCausePresent())
                return releaseCause;
        else
                throw new ParameterNotSetException();  

    }
    
/**
Sets Release Cause  
*/
    public void setReleaseCause(Cause releaseCause)
    {
        this.releaseCause = releaseCause;
        isReleaseCause = true;
    }
    
/**
Indicates if the Release Cause  optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isReleaseCausePresent()
    {
        return isReleaseCause;
    }

//----------------------------------------------  

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

//----------------------------------------------  

/**
Gets Carrier
*/
    public Carrier getCarrier() throws ParameterNotSetException
    {
        if(isCarrierPresent())
                return carrier;
        else
                throw new ParameterNotSetException();  

    }
/**
Sets Carrier
*/
    public void setCarrier(Carrier carrier)
    {
        this.carrier = carrier;
        isCarrier = true; 
    }
/**
Indicates if the  Carrier optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCarrierPresent()
    {
        return isCarrier;
    }


//----------------------------------------------  

/**
Gets Connect Time
*/
    public int getConnectTime() throws ParameterNotSetException
    {
        if(isConnectTimePresent())
                return connectTime;
        else
                throw new ParameterNotSetException();  

    }

/**
Sets Connect Time 
*/
    public void setConnectTime(int connectTime) throws IllegalArgumentException
    {
    	 if ((connectTime>=0) &&(connectTime <=2147483647))
    	 {
         this.connectTime = connectTime;
         isConnectTime = true; 
         }
        else
        {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        } 
    }

/**
Indicates if the Connect Time optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isConnectTimePresent()
    {
        return isConnectTime;
    }


//----------------------------------------------  

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
Sets Component Type
*/
    public void setComponentType(ComponentType componentType) 
    
        {
        this.componentType  = componentType;
           isComponentType = true;
    }

/**
Indicates if the Component Type optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isComponentTypePresent()
    {
        return isComponentType;
    }

//----------------------------------------------  

/**
Gets Component Correlation ID
*/
    public int getComponentCorrelationID() throws ParameterNotSetException
    {
        if(isComponentCorrelationIDPresent()) 
                return componentCorrelationID;
        else
                throw new ParameterNotSetException();  

    }

/**
Sets Component Correlation ID
*/
    public void setComponentCorrelationID(int componentCorrelationID) throws IllegalArgumentException
    {
    	if((componentCorrelationID >=0) &&( componentCorrelationID<=2147483647))
    	{
        this.componentCorrelationID = componentCorrelationID;
        isComponentCorrelationID = true;
        }
        else
        {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }

/**
Indicates if the Component Correlation ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isComponentCorrelationIDPresent()
    {
        return isComponentCorrelationID;
    }

//----------------------------------------------  


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
