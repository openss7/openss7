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
 *  File Name     : EndIndEvent.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap.event;

import java.io.*;
import java.util.*;
import java.lang.*;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.inap.datatype.*;


/** 
 This class represents the EndIndEvent class. 
 This event will be passed from the Listener to the Provider to indicate:
<UL>
<LI> Operations in a TCAP TC_END Ind
</UL> */
public final class EndIndEvent extends DialogueIndEvent
{

  private boolean dialogueEnd = false;

  private Vector inapMessages;
  private boolean isInapMessages = false;
    

/**
Constructs a new EndIndEvent .
*/
  public EndIndEvent(java.lang.Object source,TransportAddress originationAddress, int callID, byte[] applicationContext,boolean dialogueEnd)
  {
  		super(source);
		dialoguePrimitiveType= DialoguePrimitiveType.END;
		setOriginationAddress(originationAddress);
		setCallID(callID);
		setApplicationContext(applicationContext);
		setDialogueEnd(dialogueEnd);

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
Gets the Origination  Address  
*/
  
    public TransportAddress getOriginationAddress()
    {
        return originationAddress;
    }

/**
Sets the Origination  Address 
*/


    public void setOriginationAddress(TransportAddress originationAddress)
    {
        super.originationAddress=originationAddress;
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
        super.applicationContext = applicationContext;
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
        super.callID=callID;
    }

//---------------------------------------------	

/**
Sets a reference of the message objects for the event.
*/
	public void setInapMessages(Vector inapMessages)
	{
		this.inapMessages = inapMessages;
		this.isInapMessages = true;
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
		this.isInapMessages = true;
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
/**
Sets the type of termination (True = basic end).
*/
	public void setDialogueEnd(boolean dialogueEnd)
	{
		this.dialogueEnd = dialogueEnd;
	}

/**
Gets the type of termination (True = basic end).
*/
	public boolean getDialogueEnd()
	{
		return dialogueEnd;
	}

//---------------------------------------------

}  

