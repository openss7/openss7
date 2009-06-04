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
  * This interface must be implemented by any Object representing the
  * JAIN TCAP layer of any SS7 stack that interacts directly with a
  * proprietary (stack vendor specific) implementation of the TCAP
  * layer. This interface defines the methods that will be used by any
  * registered TCAP User application implementing the
  * {@link JainTcapListener} interface to send component primitives and
  * dialogue handling primitives. <p>
  *
  * It must be noted that any object that implements the: <ul>
  *
  * <li>JainTcapListener Interface is referred to as JainTcapListenerImpl;
  *
  * <li>JainTcapProvider Interface is referred to as JainTcapProviderImpl;
  *
  * <li>JainTcapStack Interface is referred to as JainTcapStackImpl. </ul>
  *
  * A TCAP User application may send a number of Component Request
  * primitives as {@link ComponentReqEvent ComponentReqEvents} to a
  * Provider object.
  * When the TCAP User application (JainTcapListenerImpl) sends a
  * Dialogue handling request primitive as a
  * {@link DialogueReqEvent DialogueReqEvent} this will trigger the
  * JainTcapProviderImpl object to send of all of the associated
  * Component requests that have been previously passed to the
  * JainTcapProviderImpl.
  * Since the same JainTcapProviderImpl will be used to handle a
  * particular transaction, Dialogue Request Events with the same
  * Originating Transaction Id must be sent to the same
  * JainTcapProviderImpl. <p>
  *
  * This interface defines methods for: <ul>
  *
  * <li>Sending {@link ComponentReqEvent ComponentReqEvents}.  Numerous
  * Component requests may be sent to a JainTcapProviderImpl before they
  * are transmitted to the remote JainTcapProviderImpl.
  *
  * <li>Sending {@link DialogueReqEvent DialogueReqEvents}.
  * This will trigger the transmission of the Dialogue request primitive
  * along with any associated Component request
  * primitives to the destination node.
  *
  * <li>Adding a {@link JainTcapListener JainTcapListener} to its list
  * of registered Event Listeners.  The JainTcapListenerImpl will
  * register with the JainTcapProviderImpl with a User Address and
  * become an Event Listener of that JainTcapProviderImpl object. Once
  * registered as an Event Listener, a JainTcapListenerImpl will receive
  * any Dialogue Indication primitives, and Component Indication
  * primitives addressed to its User Address as Events.
  *
  * <li>Removing a {@link JainTcapListener JainTcapListener} from the
  * JainTcapProviderImpl's list of Event Listeners. Once a
  * JainTcapListenerImpl is de-registered, it will no longer receive any
  * Events from that JainTcapProviderImpl.
  *
  * <li>Retrieving a new Dialogue Id from TCAP.
  *
  * <li>Releasing a Dialogue Id back into TCAP. </ul>
  *
  * @author     Sun Microsystems Inc.
  * @version    1.1
  * @see        JainTcapListener
  * @see        JainTcapStack
  */
public interface JainTcapProvider extends java.util.EventListener {

    /**
      * Returns a unique Dialogue Id to initiate a dialogue with another
      * TCAP user.
      *
      * @return
      * The new Dialogue Id returned by the underlying TCAP layer.
      *
      * @exception IdNotAvailableException
      * If a new Dialogue Id is not available.
      */
    public int getNewDialogueId()
      throws IdNotAvailableException;

    /**
      * Release the dialogue Id back to the system. <p>
      * <b>Note:</b> In some SS7 stacks the TCAP Dialogue Id is
      * automatically released following the end of a TCAP transaction.
      * In such instances the implementation of this method may be left
      * null.
      *
      * @param  dialogueId
      * The dialogue Id supplied to the method.
      *
      * @exception IdNotAvailableException
      * If a new Dialogue Id is not available.
      */
    public void releaseDialogueId(int dialogueId)
        throws IdNotAvailableException;

    /**
      * Returns a unique Invoke Id for identifying invoke requests
      * within the dialogue identified by the supplied Dialogue Id.
      * Each dialogue between two <code>JainTcapListeners</code> is
      * identified by a unique Dialogue Id. Note that the returned
      * Invoke Id will be unique for a particular Dialogue Id.
      *
      * @param dialogueId
      * The dialogue Id supplied to the method.
      *
      * @return
      * An unused unique Invoke Id.
      *
      * @exception IdNotAvailableException
      * If the invoke Id is not available to the specified dialogue Id.
      */
    public int getNewInvokeId(int dialogueId)
        throws IdNotAvailableException;

    /**
      * Releases the unique Invoke Id, allowing it to be reused within
      * the dialogue identified by the supplied Dialogue Id.
      *
      * @param invokeId
      * The invoke Id to be released.
      *
      * @param dialogueId
      * The dialogue Id from which to release the dialogue Id.
      *
      * @exception IdNotAvailableException
      * If the invoke Id is not available to the specified dialogue Id.
      */
    public void releaseInvokeId(int invokeId, int dialogueId)
        throws IdNotAvailableException;

    /**
      * Sends a Component Request primitive into the TCAP layer of the
      * SS7 protocol stack.
      *
      * @param event
      * The new component event supplied to the method.
      *
      * @exception MandatoryParameterNotSetException
      * Thrown if all of the mandatory parameters required by this
      * JainTcapProviderImpl, to send the Component Request are not set.
      * <p>
      *
      * Note that different implementations of this JainTcapProvider
      * interface will mandate that different parameters must be set for
      * each <code>ComponentReqEvent</code> . It is recommended that the
      * message detail returned in the
      * <code>MandatoryParameterNotSetException</code> should be a
      * <code>String</code> of the form: <p>
      * <center><B>"Parameter: <parameterName> not set"</B></center>
      */
    public void sendComponentReqEvent(ComponentReqEvent event)
        throws MandatoryParameterNotSetException;

    /**
      * Sends a Dialogue Request primitive into the TCAP layer of the
      * SS7 protocol stack. This will trigger the transmission to the
      * destination node of the Dialogue request primitive along with
      * any associated Component request primitives that have previously
      * been passed to this JainTcapProviderImpl.
      * Since the same JainTcapProviderImpl will be used to handle a
      * particular transaction, Dialogue Request Events with the same
      * Originating Transaction Id must be sent to the same
      * JainTcapProviderImpl.
      *
      * @param event
      * The new dialogue event supplied to the method.
      *
      * @exception MandatoryParameterNotSetException
      * Thrown if all of the mandatory parameters required by this
      * JainTcapProviderImpl to send the Dialogue Request are not set.
      * <p>
      *
      * <b>Note to developers</b> :- different implementations of this
      * JainTcapProvider interface will mandate that different
      * parameters must be set for each <code>DialogueReqEvent</code>.
      * It is recommended that the detail message returned in the
      * <code>MandatoryParameterNotSetException</code> should be a
      * <code>String</code> of the form: <p>
      * <center><B>"Parameter: <parameterName> not set"</B></center>
      */
    public void sendDialogueReqEvent(DialogueReqEvent event)
        throws MandatoryParameterNotSetException;

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
        throws TooManyListenersException, ListenerAlreadyRegisteredException, InvalidUserAddressException;

    /**
      * Adds a {@link JainTcapListener} to the list of registered Event
      * Listeners of this JainTcapProviderImpl.
      *
      * @param listener
      * The feature to be added to the JainTcapListener attribute.
      *
      * @param  userAddress
      * The feature to be added to the JainTcapListener attribute.
      *
      * @exception TooManyListenersException
      * Thrown if a limit is placed on the allowable number of
      * registered JainTcapListeners, and this limit has been reached.
      *
      * @exception ListenerAlreadyRegisteredException
      * Thrown if the listener supplied is already registered for the
      * specified userAddress with this JainSccpProviderImpl.
      *
      * @exception InvalidAddressException
      * Thrown if the userAddress supplied is not a valid address.
      */
    public void addJainTcapListener(JainTcapListener listener, SccpUserAddress userAddress)
        throws TooManyListenersException, ListenerAlreadyRegisteredException, InvalidAddressException;

    /**
      * Removes a {@link JainTcapListener} from the list of registered
      * JainTcapListeners of this JainTcapProviderImpl.
      *
      * @param listener
      * The listener to be removed from this provider.
      *
      * @exception ListenerNotRegisteredException
      * Thrown if there is no such listener registered with this
      * provider.
      */
    public void removeJainTcapListener(JainTcapListener listener)
        throws ListenerNotRegisteredException;

    /**
      * Returns the JainTcapStackImpl to which this JainTcapProviderImpl
      * is attached.
      *
      * @return
      * The attached JainTcapStack.
      *
      * @deprecated
      * As of JAIN TCAP v1.1. This class is no longer needed as a result
      * of the addition of the
      * {@link jain.protocol.ss7.tcap.JainTcapProvider#getStack()}
      * method.  The reason for deprecating this method is that the
      * provider is attached implicitly.
      */
    public JainTcapStack getAttachedStack();

    /**
      * Returns the JainTcapStackImpl to which this JainTcapProviderImpl
      * is attached.
      *
      * @return
      * The attached JainTcapStack.
      * @since 1.1
      */
    public JainTcapStack getStack();

    /**
      * @deprecated
      * As of JAIN TCAP v1.1. No replacement, the JainTcapProvider is
      * attached implicitly within the
      * {@link JainTcapStack#createProvider() createProvider} method
      * call in the <code>JainTcapStack</code> Interface.
      */
    public boolean isAttached();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
