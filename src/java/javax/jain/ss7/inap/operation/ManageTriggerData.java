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
 *  File Name     : ManageTriggerData.java
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
This class represents the ManageTriggerData Operation
*/

public class ManageTriggerData  extends Operation   implements Serializable
{
    
    private ActionIndicator actionIndicator;

    private TriggerDataIdentifier triggerDataIdentifier;

	 private String registratorIdentifier;
	 private boolean isRegistratorIdentifier = false ;
	 
	private ExtensionField extensions[];
  private boolean isExtensions = false ;  	 
	 
/**
Constructor For ManageTriggerData
*/
	public ManageTriggerData(ActionIndicator actionIndicator, TriggerDataIdentifier triggerDataIdentifier) 
	{
		operationCode = OperationCode.MANAGE_TRIGGER_DATA;
		setActionIndicator(actionIndicator);
		setTriggerDataIdentifier(triggerDataIdentifier);
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

/**<P>
Gets Action Indicator.
<DT> Returns one of the following:
<LI>ACTIVATE	- gets an activated  TDP
<LI>DEACTIVATE	- gets a deactivated TDP
<LI>RETRIEVE	- retrieves the status of a TDP
<P>
*/
    public ActionIndicator getActionIndicator() 
    { 
        return actionIndicator;
    }
    
/**
Sets Action Indicator
*/
    public void setActionIndicator(ActionIndicator actionIndicator)
    
        {
       this.actionIndicator	 = actionIndicator;
    }
    

//----------------------------------------------

/**
Gets Trigger Data Identifier
*/
    public TriggerDataIdentifier getTriggerDataIdentifier()
    {
        return triggerDataIdentifier;
    }
    
/**
Sets Trigger Data Identifier
*/
    public void setTriggerDataIdentifier(TriggerDataIdentifier triggerDataIdentifier)
    {
        this.triggerDataIdentifier = triggerDataIdentifier;
    }

//----------------------------------------------

/**
Gets Registrator Identifier
*/
    public String getRegistratorIdentifier() throws ParameterNotSetException
    {
        if(isRegistratorIdentifierPresent())
              return registratorIdentifier;
        else 
              throw new ParameterNotSetException();
    }
    
/**
Sets Registrator Identifier
*/
    public void setRegistratorIdentifier(String registratorIdentifier)
    {
        this.registratorIdentifier = registratorIdentifier;
        isRegistratorIdentifier = true;
    }
    
/**
Indicates if the Registrator Identifier optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isRegistratorIdentifierPresent()
    {
        return isRegistratorIdentifier;
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

}