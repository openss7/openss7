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
 *  File Name     : DialogueReqEvent.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap.event;

import java.util.*;
import java.lang.*;

import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.datatype.*;

/**
 This Class is the Super class of all Dialogue Request Events Classes
 */	
 
public abstract class DialogueReqEvent extends java.util.EventObject implements java.io.Serializable ,java.lang.Cloneable
{

	protected DialoguePrimitiveType dialoguePrimitiveType;
	
	protected TransportAddress destinationAddress;
	
	protected int callID;
	
	protected byte[] applicationContext;
	
/**
Constructs a new DialogueReqEvent 
*/	
	public DialogueReqEvent(java.lang.Object source)
	
	{
		super(source);
	}

/**
Gets the type of dialogue primitive.
*/
	public abstract DialoguePrimitiveType getDialoguePrimitiveType();

//---------------------------------------------	

/**
Gets the Destination Address 
*/
	public abstract TransportAddress getDestinationAddress();


/**
Sets the Destination Alias Address 
*/

   public abstract void setDestinationAddress(TransportAddress destinationAddress);

//---------------------------------------------	

/**
Gets the Call Identifier of the Call.
*/
	public abstract int getCallID();

/** 
Sets the Call Identifier 
*/
   public abstract void setCallID(int callID);
	
//---------------------------------------------	

/**
Gets the Application Context  
*/
  
    public abstract byte[] getApplicationContext();

/** 
Sets the Application Context 
*/

    public abstract void setApplicationContext(byte[] applicationContext);
    
    
//---------------------------------------------	
	

}
