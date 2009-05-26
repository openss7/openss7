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
 *  File Name     : BeginIndEvent.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap.event;

import java.io.*;
import java.util.*;
import java.lang.*;
import java.net.*;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.inap.datatype.*;


/** 
 This class represents the BeginIndEvent class. 
 This event will be passed from the Listener to the Provider to indicate:
<UL>
<LI> Operations in a TCAP TC_Begin Ind
</UL> */
public final class BeginIndEvent extends DialogueIndEvent
{

  private Vector inapMessages;
  

    

/**
Constructs a new BeginIndEvent .
*/
  public BeginIndEvent(java.lang.Object source,TransportAddress originationAddress, int callID, byte[] applicationContext,Vector inapMessages)
  {
  		super(source);
		dialoguePrimitiveType= DialoguePrimitiveType.BEGIN;
		setOriginationAddress(originationAddress);
		setCallID(callID);
		setApplicationContext(applicationContext);
		setInapMessages(inapMessages);
}


//---------------------------------------------	

/**
Gets the type of dialogue primitive.
*/
	public  DialoguePrimitiveType getDialoguePrimitiveType()
	{
	 return dialoguePrimitiveType;
	}

//---------------------------------------------	

/**
Gets the Origination Address for the event 
*/
  
    public TransportAddress getOriginationAddress()
    {
        return originationAddress;
    }

/**
Sets the Origination Address for the event 
*/


    public void setOriginationAddress(TransportAddress originationAddress)
    {
        this.originationAddress=originationAddress;
    }

//---------------------------------------------	

/** 
Gets the Application Context  
*/
  
    public byte[] getApplicationContext()
    {
        return applicationContext;
    }

/** 
Sets the Application Context 
*/

    public void setApplicationContext(byte[] applicationContext)
    {
        this.applicationContext = applicationContext;
    }

//---------------------------------------------	

/** 
Gets the Call Identifier  
*/
  
    public int getCallID()
    {
        return callID;
    }

/** 
Sets the Call Identifier 
*/


    public void setCallID(int callID)
    {
        this.callID=callID;
    }

//---------------------------------------------	

/**
Sets a reference of the message objects for the event.
*/
	public void setInapMessages(Vector inapMessages)
	{
		this.inapMessages = inapMessages;
	}

/**
Adds a message object for the event.
*/
	public void addInapMessage(java.lang.Object inapMessage)
	{
		if (inapMessages == null)
		{
			inapMessages = new Vector();
		}
		
		inapMessages.addElement(inapMessage);
	}
    
/**
Gets the number of message objects for the event.
*/
	public int getNoOfInapMessages()
	{
		return inapMessages.size();
	}

/**
Gets a specific message object for the event.
*/
	public java.lang.Object getInapMessageAt(int index)
	{
		return (inapMessages.elementAt(index));
	}

/**
Gets a reference of the message objects for the event.
*/
   public Vector getInapMessages()
	{
		return inapMessages;
	}

//---------------------------------------------	


}  

