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
 *  File Name     : PromptAndReceiveMessage.java
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
This class represents the PromptAndReceiveMessage Operation.
*/

public class PromptAndReceiveMessage  extends Operation   implements java.io.Serializable
{
	private OperationCode operationCode;

    private boolean disconnectFromIPForbidden = true;

    private InformationToSend informationToSend;
	 private boolean isInformationToSend = false ;   
    
	 private DigitsGenericNumber subscriberID;
	 private boolean isSubscriberIDRecord = false ;
	 
	 private java.lang.String mailBoxID;
	 private boolean isMailBoxID =false ;
	 
    private InformationToRecord informationToRecord;
	 

	 private Media media = Media.VOICE_MAIL;
	 private boolean isMedia = false ;
	 
	 private int callSegmentID;
	 private boolean isCallSegmentID =false ;
	 
	private ExtensionField extensions[];
  private boolean isExtensions =false ;  	 


/**
Constructor For PromptAndReceiveMessage
*/
	public PromptAndReceiveMessage(InformationToRecord informationToRecord)
	{
		operationCode = OperationCode.PROMPT_AND_RECEIVE_MESSAGE;
		setInformationToRecord(informationToRecord);
	}
	
//--------------------------------------	
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
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
Gets Subcriber ID
*/
    public DigitsGenericNumber getSubscriberID() throws ParameterNotSetException
    {
        if(isSubscriberIDPresent())
             return subscriberID;
        else
            throw new ParameterNotSetException();
        
    }
    
/**
Sets Subscriber ID
*/
    public void setSubscriberID(DigitsGenericNumber subscriberID)
    {
        this.subscriberID = subscriberID;
        isSubscriberIDRecord = true;
    }

/**
Indicates if the Subscriber ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isSubscriberIDPresent()
    {
        return isSubscriberIDRecord;
    }

//----------------------------

/**
Gets MailBox ID
*/
    public java.lang.String getMailBoxID() throws ParameterNotSetException
    {
        if(isMailBoxIDPresent())
            return mailBoxID;
        else
            throw new ParameterNotSetException();

    }
    
/**
Sets MailBox ID
*/
    public void setMailBoxID(java.lang.String mailBoxID)
    {
        this.mailBoxID = mailBoxID;
        isMailBoxID = true;
    }

/**
Indicates if the MailBox ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isMailBoxIDPresent()
    {
        return isMailBoxID;
    }

//----------------------------

/**
Gets Information To Record
*/
    public InformationToRecord getInformationToRecord() 
    {
        return informationToRecord;
    }
    
/**
Sets Information To Record
*/
    public void setInformationToRecord(InformationToRecord informationToRecord) 
    {
    	
        this.informationToRecord = informationToRecord;
      
    }

//-----------------------

/**<P>
Gets Media Type.
<DT> This parameter gets the type of media for recording.
<LI>VOICE_MAIL
<LI>FAX_GROUP_3	
<LI>FAX_GROUP_4
<P>
*/


    public Media getMedia() throws ParameterNotSetException
    {
        if(isMediaPresent())
            return media;
        else
            throw new ParameterNotSetException();

    }
    
/**
Sets Media Type
*/
    public void setMedia(Media media) 
         {
         this.media	 = media;
           isMedia = true;

    }
/**
Indicates if the Media optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isMediaPresent()
    {
        return isMedia;
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
