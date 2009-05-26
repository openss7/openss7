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
 *  File Name     : PlayAnnouncement.java
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
This class represents the PlayAnnouncement Operation.
*/

public class PlayAnnouncement  extends Operation   implements Serializable
{
    

    private InformationToSend informationToSend;
    
    private boolean disconnectFromIPForbidden = true;
    
	 private boolean requestAnnouncementComplete = true;
	 
	 private ConnectedParty connectedParty;
	 private boolean isConnectedParty  = false ;
	 
	private ExtensionField extensions[];
  private boolean isExtensions =false ;  	 

/**
Constructor For PlayAnnouncement
*/
	public PlayAnnouncement(InformationToSend informationToSend, boolean disconnectFromIPForbidden , boolean requestAnnouncementComplete)
	{
		operationCode = OperationCode.PLAY_ANNOUNCEMENT;
		setInformationToSend(informationToSend);
		setDisconnectFromIPForbidden(disconnectFromIPForbidden);
		setRequestAnnouncementComplete(requestAnnouncementComplete);
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
Gets Information To Send
*/
    public InformationToSend getInformationToSend()
    {
        return informationToSend;
    }
    
/**
Sets Information To Send
*/
    public void setInformationToSend(InformationToSend informationToSend)
    {
        this.informationToSend = informationToSend;
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
Gets Request Announcement Complete
*/
    public boolean getRequestAnnouncementComplete() 
    {
        return requestAnnouncementComplete;
    }
    
/**
Sets Request Announcement Complete
*/
    public void setRequestAnnouncementComplete(boolean requestAnnouncementComplete)
    {
        this.requestAnnouncementComplete = requestAnnouncementComplete;
    }

//-----------------------

/**
Gets Connected Party
*/
    public ConnectedParty getConnectedParty() throws ParameterNotSetException
    {
        if(isConnectedPartyPresent())
             return connectedParty;
        else
             throw new ParameterNotSetException();
    }
    
/**
Sets Connected Party
*/
    public void setConnectedParty(ConnectedParty connectedParty)
    {
        this.connectedParty = connectedParty;
        isConnectedParty = true; 
    }
    
/**
Indicates if the Connected Party optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isConnectedPartyPresent()
    {
        return isConnectedParty;
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
