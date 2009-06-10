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
 *  File Name     : PromptAndCollectUserInformation.java
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
This class represents the PromptAndCollectUserInformation Operation.
*/

public class PromptAndCollectUserInformation  extends Operation   implements java.io.Serializable
{
   

	 private CollectedInfo collectedInfo;
	 
    private boolean disconnectFromIPForbidden = true;

    private InformationToSend informationToSend;
	 private boolean isInformationToSend = false ;   
    
	 private int callSegmentID;
	 private boolean isCallSegmentID = false ;
	 
	private ExtensionField extensions[];
  private boolean isExtensions = false ;  	 

/**
Constructor For PromptAndCollectUserInformation
*/
	
	public PromptAndCollectUserInformation(CollectedInfo collectedInfo, boolean disconnectFromIPForbidden)
	{
		operationCode = OperationCode.PROMPT_AND_COLLECT_USER_INFORMATION;
		setCollectedInfo(collectedInfo);
		setDisconnectFromIPForbidden(disconnectFromIPForbidden);
		
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

/**
Gets Collected Info
*/
    public CollectedInfo getCollectedInfo()
    {
        return collectedInfo;
    }
    
/**
Sets Collected Info
*/
    public void setCollectedInfo(CollectedInfo collectedInfo)
    {
        this.collectedInfo = collectedInfo;
    }

//-----------------------

/**
Gets Disconnect From IP Forbidden
*/
    public boolean getDisconnectFromIPForbidden() 
    {
        return disconnectFromIPForbidden;
    }
/**
Sets Disconnect From IP Forbidden
*/
    public void setDisconnectFromIPForbidden(boolean disconnectFromIPForbidden)
    {
        this.disconnectFromIPForbidden = disconnectFromIPForbidden;
    }    

//-----------------------

/**
Gets Information To Send
*/
    public InformationToSend getInformationToSend() throws ParameterNotSetException
    {
        if(isInformationToSendPresent()) 
              return informationToSend;
        else
             throw new ParameterNotSetException();
    }
    
/**
Sets Information To Send
*/
    public void setInformationToSend(InformationToSend informationToSend)
    {
        this.informationToSend = informationToSend;
        isInformationToSend = true;
    }

/**
Indicates if the Information To Send optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isInformationToSendPresent()
    {
        return isInformationToSend;
    }

//-----------------------

/**
Gets Call Segment ID
*/
    public int getCallSegmentID() throws ParameterNotSetException
    {
        if(isCallSegmentIDPresent())
             return callSegmentID;
        else
             throw new ParameterNotSetException();

    }
    
/**
Sets Call Segment ID
*/
    public void setCallSegmentID(int callSegmentID)
    {
        this.callSegmentID = callSegmentID;
        isCallSegmentID = true;
    }

/**
Indicates if the Call Segment ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCallSegmentIDPresent()
    {
        return isCallSegmentID;
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
