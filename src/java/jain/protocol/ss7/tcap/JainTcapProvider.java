/*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyrights:
 *
 * Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 * 901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 * This product and related documentation are protected by copyright and
 * distributed under licenses restricting its use, copying, distribution, and
 * decompilation. No part of this product or related documentation may be
 * reproduced in any form by any means without prior written authorization of
 * Sun and its licensors, if any.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 * States Government is subject to the restrictions set forth in DFARS
 * 252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 * The product described in this manual may be protected by one or more U.S.
 * patents, foreign patents, or pending applications.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Author:
 *
 * AePONA Limited, Interpoint Building
 * 20-24 York Street, Belfast BT15 1AQ
 * N. Ireland.
 *
 *
 * Module Name   : JAIN TCAP API
 * File Name     : JainTcapProvider.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 *                 Eugene Bell [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.0     19/03/99   AMcG, CH             Initial version
 * 1.0d    15/8/2000  Eugene Bell          Final Public Release
 * 1.1     26/4/2001  Eugene Bell          Maintenance Release 1.1
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.*;
import java.util.TooManyListenersException;
import jain.protocol.ss7.*;
import jain.protocol.ss7.SccpUserAddress;

/**
 * This interface must be implemented by any Object representing the JAIN TCAP
 * layer of any SS7 stack that interacts directly with a proprietary (stack
 * vendor specific) implementation of the TCAP layer. This interface defines
 * the methods that will be used by any registered TCAP User application
 * implementing the <a href="JainTcapListener.html">JainTcapListener</a>
 * interface to send component primitives and dialogue handling primitives. It
 * must be noted that any object that implements the:
 * <UL>
 *    <LI> JainTcapListener Interface is referred to as JainTcapListenerImpl.
 *
 *    <LI> JainTcapProvider Interface is referred to as JainTcapProviderImpl.
 *
 *    <LI> JainTcapStack Interface is referred to as JainTcapStackImpl.
 * </UL>
 * A TCAP User application may send a number of Component Request primitives as
 * <a href="ComponentReqEvent.html">ComponentReqEvents</a> to a Provider
 * object. When the TCAP User application (JainTcapListenerImpl) sends a
 * Dialogue handling request primitive as a <a href="DialogueReqEvent.html">
 * DialogueReqEvent</a> this will trigger the JainTcapProviderImpl object to
 * send of all of the associated Component requests that have been previously
 * passed to the JainTcapProviderImpl. Since the same JainTcapProviderImpl will
 * be used to handle a particular transaction, Dialogue Request Events with the
 * same Originating Transaction Id must be sent to the same
 * JainTcapProviderImpl. <p>
 *
 * This interface defines methods for:
 * <UL>
 *    <LI> Sending <a href="ComponentReqEvent.html">ComponentReqEvents</a> .
 *    Numerous Component requests may be sent to a JainTcapProviderImpl before
 *    they are transmitted to the remote JainTcapProviderImpl.
 *    <LI> Sending <a href="DialogueReqEvent.html">DialogueReqEvents</a> . This
 *    will trigger the transmission of the Dialogue request primitive along with
 *    any associated Component request primitives to the destination node
 *    <LI> Adding a <a href="JainTcapListener.html">JainTcapListener</a> to its
 *    list of registered Event Listeners. The JainTcapListenerImpl will register
 *    with the JainTcapProviderImpl with an User Address and become an Event
 *    Listener of that JainTcapProviderImpl object. Once registered as an Event
 *    Listener, a JainTcapListenerImpl will receive any Dialogue Indication
 *    primitives, and Component Indication primitives addressed to its User
 *    Address as Events.
 *    <LI> Removing a <a href="JainTcapListener.html">JainTcapListener</a> from
 *    the JainTcapProviderImpl's list of Event Listeners. Once a
 *    JainTcapListenerImpl is de-registered, it will no longer receive any
 *    Events from that JainTcapProviderImpl.
 *    <LI> Retrieving a new Dialogue Id from TCAP.
 *    <LI> Releasing a Dialogue Id back into TCAP.
 * </UL>
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 * @see        JainTcapListener
 * @see        JainTcapStack
 */
public interface JainTcapProvider extends java.util.EventListener {

    /**
    *  Returns a unique Dialogue Id to initiate a dialogue with another TCAP user.
    *
    * @return the new Dialogue Id returned by the underlying TCAP layer
    * @exception  IdNotAvailableException  if a new Dialogue Id is not available
    */
    public int getNewDialogueId() throws IdNotAvailableException;

    /**
    * Release the dialogue Id back to the system. <br>
    * <b>Note:</b> In some SS7 stacks the TCAP Dialogue Id is automatically
    * released following the end of a TCAP transaction. In such instances the
    * implementation of this method may be left null.
    *
    * @param  dialogueId  the dialogue Id supplied to the method
    * @exception  IdNotAvailableException  if a new Dialogue Id is not available
    */
    public void releaseDialogueId(int dialogueId) throws IdNotAvailableException;

    /**
    * Returns a unique Invoke Id for identifying invoke requests within the
    * dialogue identified by the supplied Dialogue Id. Each dialogue between two
    * <CODE>JainTcapListeners</CODE> is identified by a unique Dialogue Id. Note
    * that the returned Invoke Id will be unique for a particular Dialogue Id.
    *
    * @param  dialogueId the dialogue Id supplied to the method
    * @return an unused unique Invoke Id
    * @exception  IdNotAvailableException  if an invoke Id is not available to the
    * specified dialogue Id
    */
    public int getNewInvokeId(int dialogueId) throws IdNotAvailableException;

    /**
    * Releases the unique Invoke Id, allowing it to be reused within the dialogue
    * identified by the supplied Dialogue Id.
    *
    * @param  invokeId    the invoke Id to be released
    * @param  dialogueId  the dialogue Id from which to release the dialogue Id
    * @exception  IdNotAvailableException  if an invoke Id is not available to the
    * specified dialogue Id
    */
    public void releaseInvokeId(int invokeId, int dialogueId) throws IdNotAvailableException;

    /**
    * Sends a Component Request primitive into the TCAP layer of the SS7 protocol
    * stack.
    *
    * @param  event the new component event supplied to the method
    * @exception  MandatoryParameterNotSetException  thrown if all of the mandatory
    * parameters required by this JainTcapProviderImpl, to send the Component
    * Request are not set. <BR>
    * Note that different implementations of this JainTcapProvider interface
    * will mandate that different parameters must be set for each <CODE>
    * ComponentReqEvent</CODE> . It is recommended that the message detail
    * returned in the <CODE>MandatoryParameterNotSetException</CODE> should
    * be a <CODE>String</CODE> of the form: <br>
    * <CENTER><B>"Parameter: <parameterName> not set"</B></CENTER>
    */
    public void sendComponentReqEvent(ComponentReqEvent event) throws MandatoryParameterNotSetException;

    /**
    * Sends a Dialogue Request primitive into the TCAP layer of the SS7 protocol
    * stack. This will trigger the transmission to the destination node of the
    * Dialogue request primitive along with any associated Component request
    * primitives that have previously been passed to this JainTcapProviderImpl.
    * Since the same JainTcapProviderImpl will be used to handle a particular
    * transaction, Dialogue Request Events with the same Originating Transaction
    * Id must be sent to the same JainTcapProviderImpl.
    *
    * @param  event the new dialogue event supplied to the method
    * @exception  MandatoryParameterNotSetException  thrown if all of the mandatory
    * parameters required by this JainTcapProviderImpl to send the Dialogue
    * Request are not set. <p>
    *
    * <b>Note to developers</b> :- different implementations of this
    * JainTcapProvider interface will mandate that different parameters must
    * be set for each <CODE>DialogueReqEvent</CODE> . It is recommended that
    * the detail message returned in the <CODE>
    * MandatoryParameterNotSetException</CODE> should be a <CODE>String
    * </CODE>of the form: <P>
    * <CENTER><B>"Parameter: <parameterName> not set"</B></CENTER>
    */
    public void sendDialogueReqEvent(DialogueReqEvent event) throws MandatoryParameterNotSetException;

    /**
    * Adds a <a href="JainTcapListener.html">JainTcapListener</a> to the list of
    * registered Event Listeners of this JainTcapProviderImpl.
    *
    * @param  listener the feature to be added to the JainTcapListener attribute
    * @param  userAddress the feature to be added to the JainTcapListener attribute
    * @exception  TooManyListenersException thrown if a limit is
    * placed on the allowable number of registered JainTcapListeners, and
    * this limit is exceeded.
    * @exception  ListenerAlreadyRegisteredException thrown if the listener
    * listener supplied is already registered
    * @exception  InvalidUserAddressException thrown if the user address
    * supplied is not a valid address
    *
    * @deprecated As of JAIN TCAP version 1.1. This method is replaced by the
    * {@link #addJainTcapListener(JainTcapListener, SccpUserAddress)} method.
    */
    public void addJainTcapListener(JainTcapListener listener, TcapUserAddress userAddress)
                			 throws TooManyListenersException, ListenerAlreadyRegisteredException,
		                	 InvalidUserAddressException;

    /**
    * Adds a <a href="JainTcapListener.html">JainTcapListener</a> to the list of
    * registered Event Listeners of this JainTcapProviderImpl.
    *
    * @param  listener the feature to be added to the JainTcapListener attribute
    * @param  userAddress the feature to be added to the JainTcapListener attribute
    * @exception  TooManyListenersException thrown if a limit is
    * placed on the allowable number of registered JainTcapListeners, and
    * this limit is exceeded.
    * @exception  ListenerAlreadyRegisteredException thrown if the listener
    * listener supplied is already registered
    * @exception  InvalidAddressException thrown if the user address
    * supplied is not a valid address
    */
    public void addJainTcapListener(JainTcapListener listener, SccpUserAddress userAddress)
			            throws TooManyListenersException, ListenerAlreadyRegisteredException,
			            InvalidAddressException;

    /**
    * Removes a <a href="JainTcapListener.html">JainTcapListener</a> from the
    * list of registered JainTcapListeners of this JainTcapProviderImpl.
    *
    * @param  listener the listener to be removed from this provider
    * @exception  ListenerNotRegisteredException thrown if there is no such
    * listener registered with this provider
    */
    public void removeJainTcapListener(JainTcapListener listener) throws ListenerNotRegisteredException;

    /**
    * Returns the JainTcapStackImpl that this JainTcapProviderImpl is attached to.
    *
    * @return the attached JainTcapStack.
    * @deprecated As of JAIN TCAP v1.1. This class is no longer needed as a result
    * of the addition of the {@link jain.protocol.ss7.tcap.JainTcapProvider#getStack} class.
    * The reason for deprecating this method is that the provider is attached implicitly.
    */
    public JainTcapStack getAttachedStack();

    /**
    * Returns the JainTcapStackImpl that this JainTcapProviderImpl is attached to.
    *
    * @return the attached JainTcapStack.
    * @since version 1.1
    */
    public JainTcapStack getStack();

    /**
    * @deprecated    As of JAIN TCAP v1.1. No replacement, the JainTcapProvider is
    * attached implicitly within the <a href = "JainTcapStack.html#createProvider()">createProvider</a>
    * method call in the <code>JainTcapStack</code> Interface.
    */
    public boolean isAttached();
}

