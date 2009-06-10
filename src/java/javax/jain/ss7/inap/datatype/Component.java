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
 *  File Name     : Component.java
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
This class represents the Component DataType
*/

public class Component implements java.io.Serializable
{
	
	private String componentInfo;


	private String relayedComponent;


	private ComponentChoice componentChoice;

/**
Constructor For Component 
*/
	public Component(java.lang.String componentData, ComponentID componentID)
	{
		if(componentID.getComponentID() == ComponentID.COMPONENT_INFO.getComponentID())
			setComponentInfo(componentData);
		else if(componentID.getComponentID() ==ComponentID.RELAYED_COMPONENT.getComponentID())
			setRelayedComponent(componentData);
	}
//---------------------------------------------	

/**
Gets ComponentChoice
*/
	public ComponentChoice getComponentChoice()
		{ 
			return componentChoice;
		}
	
/**
Gets Component Info
*/
	public java.lang.String getComponentInfo() throws ParameterNotSetException
    	{
        	if(componentChoice.getComponentChoice() ==ComponentChoice.COMPONENT_INFO.getComponentChoice())
			{	
                 return componentInfo;
            }
			else
			{
                 throw new ParameterNotSetException();
            }      
    	}
/**
Sets Component Info
*/
	public void setComponentInfo(java.lang.String componentInfo)
    	{
        	this.componentInfo = componentInfo;
            componentChoice =ComponentChoice.COMPONENT_INFO;
    	}


/**
Gets Relayed Component
*/
	public java.lang.String getRelayedComponent() throws ParameterNotSetException
    	{
        		if(componentChoice.getComponentChoice() ==ComponentChoice.RELAYED_COMPONENT.getComponentChoice())
				{
                	return relayedComponent;
                }
				else
				{
                    throw new ParameterNotSetException();
                } 
    	}
/**
Sets Relayed Component
*/
	public void setRelayedComponent(java.lang.String relayedComponent)
    	{
			this.relayedComponent = relayedComponent;
            componentChoice =ComponentChoice.RELAYED_COMPONENT;
    	}

//-----------------------

}
