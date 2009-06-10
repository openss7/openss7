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
 *  File Name     : TDisconnect.java
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
This class represents the TDisconnect Operation.
*/

public class TDisconnect extends Operation  implements java.io.Serializable
{
    
    private DPSpecificCommonParameters dPSpecificCommonParameters;

    private String calledPartyBusinessGroupID;
    private boolean isCalledPartyBusinessGroupID=  false ;

    private CalledPartySubaddress calledPartySubaddress;
    private boolean isCalledPartySubaddress = false ;

    private FacilityGroup calledFacilityGroup;
    private boolean isCalledFacilityGroup = false ;

    private int calledFacilityGroupMember;
    private boolean isCalledFacilityGroupMember =false ;

    private Cause releaseCause;
    private boolean isReleaseCause = false;	

    private int connectTime;
    private boolean isConnectTime = false ;

    private ComponentType componentType;
    private boolean isComponentType = false ;

    private int componentCorrelationID;
    private boolean isComponentCorrelationID =false ;
    
	private ExtensionField extensions[];
  private boolean isExtensions =false ;  
    
    private Component component;
    private boolean isComponent = false ;     

/**
Constructor For TDisconnect
*/
	public TDisconnect(DPSpecificCommonParameters dPSpecificCommonParameters)
	{
		operationCode = OperationCode.T_DISCONNECT;
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
Gets Called Party Business Group ID
*/
    public java.lang.String getCalledPartyBusinessGroupID() throws ParameterNotSetException
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
    public void setCalledPartyBusinessGroupID(java.lang.String calledPartyBusinessGroupID)
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
Gets Called Party Sub-Address
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

//----------------------------------------------    

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

//----------------------------------------------    

/**
Gets Release Cause 
*/
    public Cause getReleaseCause() throws ParameterNotSetException
    {
        if(isReleaseCausePresent()){
        return releaseCause;
        }else{
               throw new ParameterNotSetException();
            } 
    }
/**
Sets Release Cause 
*/
    public void setReleaseCause(Cause releaseCause)
    {
        this.releaseCause = releaseCause;
        isReleaseCause=true;
    }
/**
Indicates if the Release Cause optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isReleaseCausePresent()
    {
        return isReleaseCause;
    }

//----------------------------------------------    

/**
Gets Connect Time 
*/
    public int getConnectTime() throws ParameterNotSetException
    {
        if(isConnectTimePresent()){
        return connectTime;
        }else{
               throw new ParameterNotSetException();
            } 
    }
/**
Sets Connect Time 
*/
    public void setConnectTime(int connectTime)
    {
        this.connectTime = connectTime;
        isConnectTime=true;
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
        if(isComponentTypePresent()){
        return componentType;
        }else{
               throw new ParameterNotSetException();
            } 
    }
/**
Sets Component Type
*/
    public void setComponentType(ComponentType componentType) 

       {
       this.componentType  = componentType;
           isComponentType=true;

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
        if(isComponentCorrelationIDPresent()){
        return componentCorrelationID;
        }else{
               throw new ParameterNotSetException();
            } 
    }
/**
Sets Component Correlation ID
*/
    public void setComponentCorrelationID(int componentCorrelationID)
    {
        this.componentCorrelationID = componentCorrelationID;
         isComponentCorrelationID=true;
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


/**
Gets Component
*/

    public Component getComponent() throws ParameterNotSetException
    {
        if(isComponentPresent()){
         return component;
         }else{
               throw new ParameterNotSetException();
            } 
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
