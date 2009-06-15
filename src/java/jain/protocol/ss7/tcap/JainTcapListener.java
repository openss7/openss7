/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date$ by $Author$
 */

package jain.protocol.ss7.tcap;

import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the methods required by all TCAP user
  * applications to receive and process JAIN TCAP Events that are
  * emitted by an object implementing the JainTcapProvider interface. It
  * must be noted that any object that implements the: <ul>
  * <li>JainTcapListener Interface is referred to as
  * JainTcapListenerImpl.
  * <li>JainTcapProvider Interface is referred to as
  * JainTcapProviderImpl. </ul>
  * The Events emitted by a JainTcapProviderImpl may be one of two types: <ul>
  * <li>Component Indication Events - these are component indication
  * primitives emitted as events by the JainTcapProviderImpl.  Component
  * Indication Events represent TCAP Components that are sent from TCAP
  * to the TCAP User.
  * <li>Dialogue Indication Events - these are dialogue handling
  * indication primitives emitted as events by the JainTcapProviderImpl.
  * Dialogue Indication Events represent TCAP Dialogue handling
  * primitives that are sent from TCAP to the TCAP User. </ul>
  * A TCAP User application will only receive Dialogue and Component
  * handling indication Events once it has registered as an
  * EventListener of a JainTcapProviderImpl. The User application will
  * register with the JainTcapProviderImpl with an User Address by
  * invoking the ProviderImpl's addJainTcapListener() method . Any
  * Events addressed to a User Address belonging to an User Application
  * will be passed to that User application. <p>
  * The User application will use the JainTcapProviderImpl to send TCAP
  * messages by sending Dialogue Request Events and Component Request
  * Events to that JainTcapProviderImpl. These Events will be handled by
  * the JainTcapProviderImpl's sendComponentReqEvent() and
  * sendDialogueReqEvent() methods. The User application must send
  * Dialogue Request Events with the same Dialogue Id to the same
  * JainTcapProviderImpl.
  * @version 1.2.2
  * @author Monavacon Limited
  * @see JainTcapProvider
  */
public interface JainTcapListener extends java.util.EventListener {
    /** Processes a Component Indication Event emitted by the
      * JainTcapProvider . Similar to TCAP the restriction that
      * Indications events can only passed from TCAP (the
      * JainTcapProviderImpl) to the Tcap User (JainTcapListenerImpl),
      * is enforced by the JAIN TCAP Event hierarchy and error checking
      * within the specification, to ensure TCAP like processing.
      * @param event
      * The new event supplied to the method.  */
    public void processComponentIndEvent(ComponentIndEvent event);
    /** Processes a Dialogue Indication Event emitted by the
      * JainTcapProvider . Similar to TCAP the restriction that
      * Indications events can only passed from TCAP (the
      * JainTcapProviderImpl) to the Tcap User (JainTcapListenerImpl),
      * is enforced by the JAIN TCAP Event hierarchy.
      * @param event
      * The new event supplied to the method.  */
    public void processDialogueIndEvent(DialogueIndEvent event);
    /** Processes an error Event emitted by a JainTcapProvider . A
      * provider will fire this event to all registered
      * JainTcapListeners whenever it encounters an error that cannot be
      * handled through regular TCAP error handling mechanisms.
      * @param error
      * The new error supplied to the method.  */
    public void processTcapError(TcapErrorEvent error);
    /** Adds a SccpUserAddress class to the List of User Addresses used
      * by a JainTcapListenerImpl. A JainTcapListenerImpl may choose to
      * register as an Event Listener of a JainTcapProvider for all of
      * its User Addresses. Any Tcap messages addressed to any of this
      * JainTcapListenerImpls' User Addresses will be passed to this
      * JainTcapListenerImpl as events by the JainTcapProviderImpl.
      * @param userAddress
      * The feature to be added to the UserAddress attribute.  */
    public void addUserAddress(SccpUserAddress userAddress) throws UserAddressLimitException;
    /** Removes a SccpUserAddress class from the list of User Addresses
      * held by a JainTcapListenerImpl.
      * @param userAddress
      * The user address to be removed.  */
    public void removeUserAddress(SccpUserAddress userAddress) throws InvalidAddressException;
    /** @deprecated
      * As of JAIN TCAP v1.1. This method is no longer needed as a
      * result of the addition of the addUserAddress(SccpUserAddress)
      * method.
      *
      * Adds a TcapUserAddress to the vector of User Addresses used by a
      * JainTcapListenerImpl. A JainTcapListenerImpl may choose to
      * register as an Event Listener of a JainTcapProvider for all of
      * its User Addresses. Any Tcap messages addressed to any of this
      * JainTcapListenerImpls' User Addresses will be passed to this
      * JainTcapListenerImpl as events by the JainTcapProviderImpl.
      * @param userAddress
      * The User Address to be added to this JainTcapListenerImpl's
      * vector of User Addresses.  */
    public void addUserAddress(TcapUserAddress userAddress);
    /** @deprecated
      * As of JAIN TCAP v1.1. This method is no longer needed as a
      * result of the addition of the removeUserAddress(SccpUserAddress)
      * method.
      *
      * Removes a TcapUserAddress from the list of User Addresses used
      * by a JainTcapListenerImpl.
      * @param userAddress
      * The User Address to be removed from this JainTcapListenerImpl's
      * vector of User Addresses.
      */
    public void removeUserAddress(TcapUserAddress userAddress);
    /** Update v1.1: Changed return type of User Address List from
      * Vector to SccpUserAddress Array. Returns the list of
      * SccpUserAddress used by this JainTcapListenerImpl.
      * @return
      * The SccpUserAddress[] of User Addresses used by this
      * JainTcapListenerImpl.  */
    public SccpUserAddress[] getUserAddressList() throws UserAddressEmptyException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
