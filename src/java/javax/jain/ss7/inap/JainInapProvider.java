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
 *  File Name     : JainInapProvider.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap;

import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.inap.event.*;
import javax.jain.ss7.*;
import javax.jain.*;


/**
 *  <P>
This interface must be implemented by any Object representing 
 the JAIN INAP layer of any SS7 stack that interacts directly with a
 proprietary (stack vendor specific) implementation of the INAP layer.
 This interface defines the methods that will be 
 used by any registered INAP User application implementing the 
 <a href="JainInapListener.html">JainInapListener</a>
 interface to send Operations, Return Results or Return Errors .
 It must be noted that any object that implements the:
<UL>
<LI>JainInapListener Interface is referred to as JainInapListenerImpl.
<LI>JainInapProvider Interface is referred to as JainInapProviderImpl.
<LI>JainInapStack Interface is referred to as JainInapStackImpl.
</UL>
 A INAP API User may send number of events to a Provider object. 
 This interface defines methods for:
 <UL>
 <LI>Adding a <a href="JainInapListener.html">JainInapListener</a>
 to its list of registered Event Listeners. 
 The JainInapListenerImpl will register with the JainInapProviderImpl with an Application Context and become 
 an Event Listener of that JainInapProviderImpl object.
 Once registered as an Event Listener, a JainInapListenerImpl will receive any
 Indications addressed to its User Address as Events.    
 <LI>Removing a <a href="JainInapListener.html">JainInapListener</a>
 from the JainInapProviderImpl's list of Event Listeners. Once a JainInapListenerImpl
 is de-registered, it will no longer receive any Events from that JainInapProviderImpl.
 <LI>Returning a new Call Id.

 </UL>
<P>

 *
 */	
public interface JainInapProvider extends java.util.EventListener 
{

/**
* Adds a JainInapListener to the list of registered Event
             Listeners of this JainInapProviderImpl.
*/	
	public void addJainInapListener(JainInapListener listener, byte[] applicationContext) throws ListenerAlreadyRegisteredException, TooManyListenersRegisteredException;
			
/**
* Removes a JainInapListener from the list of registered
             JainInapListeners of this JainInapProviderImpl.
*/
	public void removeJainInapListener(JainInapListener listener, byte[] applicationContext) 	throws ListenerNotRegisteredException;

/** 
* Returns the JainInapStackImpl that this
   JainInapProviderImpl is attached to.
*/

	public JainInapStack getStack() ;

/**
* Returns the Call ID to the JAIN INAP API User.
*/
 	public int getCallID()	throws IDNotAvailableException ;
 		

/**
* Sends the DialogueReqEvent into the INAP
             layer of the SS7 protocol stack. The Provider shall return an array of Invoke IDs.
*/ 
	public int[] sendInapReqEvent(DialogueReqEvent event) throws InvalidAddressException, InvalidCallIDException,InvalidApplicationContextException ;

    
}