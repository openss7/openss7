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
 * File Name     : JainTcapListener.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     16/10/2000  Phelim O'Doherty    Updated userAddressList from Vector
 *                                         to TcapUserAddress array.
 *                                         Replaced TcapUserAddress with JainAddress
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.InvalidAddressException;
import jain.protocol.ss7.*;
// import java.util.Vector;

/**
 *  This interface defines the methods required by all TCAP user applications to
 *  receive and process JAIN TCAP Events that are emitted by an object
 *  implementing the <a href="JainTcapProvider.html">JainTcapProvider</a>
 *  interface. It must be noted that any object that implements the:
 *  <UL>
 *    <LI> JainTcapListener Interface is referred to as JainTcapListenerImpl.
 *    <LI> JainTcapProvider Interface is referred to as JainTcapProviderImpl.
 *  </UL>
 *  The Events emitted by a JainTcapProviderImpl may be one of two types:
 *  <UL>
 *    <LI> <a href="ComponentIndEvent.html">Component Indication Events</a> -
 *    these are component indication primitives emitted as events by the
 *    JainTcapProviderImpl. Component Indication Events represent TCAP
 *    Components that are sent from TCAP to the TCAP User.
 *    <LI> <a href="DialogueIndEvent.html">Dialogue Indication Events</a> -
 *    these are dialogue handling indication primitives emitted as events by the
 *    JainTcapProviderImpl. Dialogue Indication Events represent TCAP Dialogue
 *    handling primitives that are sent from TCAP to the TCAP User.
 *  </UL>
 *  <p>
 *
 *  A TCAP User application will only receive Dialogue and Component handling
 *  indication Events once it has registered as an EventListener of a
 *  JainTcapProviderImpl. The User application will register with the
 *  JainTcapProviderImpl with an User Address by invoking the ProviderImpl's <a
 *  href="JainTcapProvider.html#addJainTcapListener(jain.protocol.ss7.tcap.JainTcapListener,
 *  , jain.address.JainAddress)">addJainTcapListener() method</a>
 *  . Any Events addressed to a User Address belonging to an User Application
 *  will be passed to that User application. <p>
 *
 *  The User application will use the JainTcapProviderImpl to send TCAP messages
 *  by sending <a href="DialogueReqEvent.html">Dialogue Request Events</a> and
 *  <a href="ComponentReqEvent.html">Component Request Events</a> to that
 *  JainTcapProviderImpl. These Events will be handled by the
 *  JainTcapProviderImpl's <a href="JainTcapProvider.html#sendComponentReqEvent(jain.protocol.ss7.tcap.ComponentReqEvent)">
 *  sendComponentReqEvent()</a> and <a href="JainTcapProvider.html#sendDialogueReqEvent(jain.protocol.ss7.tcap.DialogueReqEvent)">
 *  sendDialogueReqEvent()</a> methods. The User application must send <a
 *  href="DialogueReqEvent.html">Dialogue Request Events</a> with the same
 *  Dialogue Id to the same JainTcapProviderImpl.
 *
 *@author     Sun Microsystems Inc.
 *@version    1.1
 *@see        JainTcapProvider
 */
public interface JainTcapListener extends java.util.EventListener {

    /**
    * Processes a Component Indication Event emitted by the <a
    * href="JainTcapProvider.html">JainTcapProvider</a> . Similar to TCAP the
    * restriction that Indications events can only passed from TCAP (the
    * JainTcapProviderImpl) to the Tcap User (JainTcapListenerImpl), is enforced
    * by the JAIN TCAP Event hierarchy and error checking within the
    * specification, to ensure TCAP like processing.
    *
    * @param  event the new event supplied to the method
    */
    public void processComponentIndEvent(ComponentIndEvent event);

    /**
    * Processes a Dialogue Indication Event emitted by the <a
    * href="JainTcapProvider.html">JainTcapProvider</a> . Similar to TCAP the
    * restriction that Indications events can only passed from TCAP (the
    * JainTcapProviderImpl) to the Tcap User (JainTcapListenerImpl), is enforced
    * by the JAIN TCAP Event hierarchy.
    *
    * @param  event the new event supplied to the method
    */
    public void processDialogueIndEvent(DialogueIndEvent event);

    /**
    * Processes an error Event emitted by a <a href="JainTcapProvider.html">
    * JainTcapProvider</a> . A provider will fire this event to all registered
    * JainTcapListeners whenever it encounters an error that cannot be handled
    * through regular TCAP error handling mechanisms.
    *
    * @param  error the new error supplied to the method
    */
    public void processTcapError(TcapErrorEvent error);

    /**
    * Adds a {@link jain.protocol.ss7.SccpUserAddress} class to the List of
    * User Addresses used by a JainTcapListenerImpl. A JainTcapListenerImpl may
    * choose to register as an Event Listener of a <a
    * href="JainTcapProvider.html">JainTcapProvider</a> for all of its User
    * Addresses. Any Tcap messages addressed to any of this
    * JainTcapListenerImpls' User Addresses will be passed to this
    * JainTcapListenerImpl as events by the JainTcapProviderImpl.
    *
    * @param  userAddress The feature to be added to the UserAddress attribute
    */
    public void addUserAddress(SccpUserAddress userAddress) throws UserAddressLimitException;

    /**
    * Removes a {@link jain.protocol.ss7.SccpUserAddress} class from the list
    * of User Addresses held by a JainTcapListenerImpl.
    *
    * @param  userAddress the user address to be removed
    */
    public void removeUserAddress(SccpUserAddress userAddress) throws InvalidAddressException;

    /**
    * Adds a <a href="TcapUserAddress.html">TcapUserAddress</a>
    * to the vector of User Addresses used by a JainTcapListenerImpl. A
    * JainTcapListenerImpl may choose to register as an Event Listener
    * of a <a href="JainTcapProvider.html">JainTcapProvider</a>
    * for all of its User Addresses. Any Tcap messages addressed to any of this JainTcapListenerImpls'
    * User Addresses will be passed to this JainTcapListenerImpl as events by the JainTcapProviderImpl.
    * @param <var>userAddress</var> the User Address to be added to this JainTcapListenerImpl's vector
    * of User Addresses.
    *
    * @deprecated As of JAIN TCAP v1.1. This method is no longer needed as a result
    * of the addition of the {@link #addUserAddress(SccpUserAddress)} method.
    */
    public void addUserAddress(TcapUserAddress userAddress);

    /**
    * Removes a <a href="TcapUserAddress.html">TcapUserAddress</a>
    * from the list of User Addresses used by a JainTcapListenerImpl.
    * @param <var>userAddress</var> the User Address to be removed from this JainTcapListenerImpl's
    * vector of User Addresses.
    *
    * @deprecated As of JAIN TCAP v1.1. This method is no longer needed as a result
    * of the addition of the {@link #removeUserAddress(SccpUserAddress)} method.
    */
    public void removeUserAddress(TcapUserAddress userAddress);

    /**
    * Update v1.1: Changed return type of User Address List from Vector to SccpUserAddress Array.
    *
    * Returns the list of {@link jain.protocol.ss7.SccpUserAddress}
    * used by this JainTcapListenerImpl.
    * @return  the SccpUserAddress[] of User Addresses used by this JainTcapListenerImpl.
    *
    */
    public SccpUserAddress[] getUserAddressList() throws UserAddressEmptyException;
}
