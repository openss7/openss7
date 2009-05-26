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
 *  File Name     : UabortReqEvent.java
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
 This class represents the UAbortReqEvent class. 
 This event will be passed from the Listener to the Provider to indicate:
<UL>
<LI> Operations in a TCAP TC_U_ABORT Req
</UL> */
public final class UAbortReqEvent extends DialogueReqEvent
{

  private UAbortReason abortReason;
   

/**
Constructs a new UabortReqEvent .
*/
  public UAbortReqEvent(java.lang.Object source,TransportAddress destinationAddress, int callID, byte[] applicationContext,UAbortReason abortReason)throws IllegalArgumentException
  {
  		super(source);
		dialoguePrimitiveType= DialoguePrimitiveType.UABORT;
		setDestinationAddress(destinationAddress);
		setCallID(callID);
		setApplicationContext(applicationContext);
		setAbortReason(abortReason);


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
Gets the Destination  Address (eg SSP1, IP2) for the event 
*/
  
    public TransportAddress getDestinationAddress()
    {
        return destinationAddress;
    }

/**
Sets the Destination  Address (eg SSP1, IP2) for the event 
*/


    public void setDestinationAddress(TransportAddress destinationAddress)
    {
        super.destinationAddress=destinationAddress;
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
Sets the reason for aborting the transaction.
*/
	public void setAbortReason(UAbortReason abortReason) 
	{
				this.abortReason = abortReason;
	}

/**
Gets the reason for aborting the transaction.
*/
	public UAbortReason getAbortReason()
	{
		return abortReason;
	}

//---------------------------------------------	


}  

