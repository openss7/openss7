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

package org.openss7.ss7.sccp;

import org.openss7.ss7.*;

/**
  * The SccpProvider interface models the services available from an
  * SccpProvider implementation.  Ecah vendor-sepcific SccpProviderImpl
  * class will implement the SccpProvider interface. <p>
  *
  * This interface provides the implementation object with the abilith
  * to offer services to SccpListener implementation objects
  * (application). <p>
  *
  * This interface must be implemented by any Object representing the
  * SCCP layer of any SS7 stack that interacts directly with a
  * proprietary (stack vendor specific) implementation of the SCCP
  * layer.  This interface defines the methods that will be used by any
  * registered SCCP User application implementing the SccpListener
  * interface to send connection-oriented or connectionless primitives.
  * It must be noted that any object that implements the SccpListener
  * interface is referred to as SccpListenerImpl; SccpProvider, as
  * SccpProviderImpl and SccpStack as SccpStackImpl. <p>
  *
  * A SCCP User application may send a number of transfer request
  * primitives as SccpEvent events to a provider object.  This interface
  * defines methods for: <p>
  *
  * <ul>
  * <li>Sending SccpEvent events.  This will trigger the sending of SCCP
  * connnectionless or connection-oriented service primitives to the
  * destination node.
  * <li>Adding a SccpListener to its list of registered Event Listeners.
  * The SccpListenerImpl will register with a User Address and become an
  * Event Listener of that SccpProviderImpl object.  Once registered as
  * an Event Listener, an SccpListenerImpl will receive any
  * connectionless or connection-oriented or management indication
  * primitives addressed to its User Address as Events.
  * <li>Removing a SccpListener from the SccpProviderImpl's list of
  * Event Listeners.  Once an SccpListener is de-registered, it will no
  * longer receive any Events from that SccpProviderImpl.
  * </ul>
  *
  * @author     Brian Bidulock
  * @version    1.2.2
  * @see        SccpListener
  * @see        SccpStack
  */
public interface SccpProvider extends java.util.EventListener {
    /**
      * Sends a Transfer Request primitive to the SCCP layer of the SS7
      * protocol stack.  This will trigger the transmission to the
      * destination node of the transfer request primtive along with any
      * asociated data.
      *
      * @param event The event to be transmitted.
      * @exception javax.jain.MandatoryParameterNotSetException  Thrown if all of
      * the mandatory parameters required by this SccpProviderImpl to
      * send the SccpEvent are not set.
      */
    public void sendSccpEvent(SccpEvent event)
        throws javax.jain.MandatoryParameterNotSetException;
    /**
      * Adds a SccpListener to the list of registered Event Listeners of
      * this SccpProviderImpl.
      *
      * @param listener The feature to be added in the SccpListener
      * attribute
      * @param userAddress The feaeture to the added to the SccpListener
      * attribute.
      * @exception java.util.TooManyListenersException  Thrown if a limit is
      * placed on the allowable number of registered SccpListeners, and
      * this limit has been reached.
      * @exception javax.jain.ListenerAlreadyRegisteredException  Thrown if the
      * listener supplied is already registered for the specified
      * userAddress with this SccpProviderimpl.
      * @exception javax.jain.InvalidUserAddressException  Thrown if the
      * userAddress supplied is not a valid address.
      */
    public void addSccpListener(SccpListener listener, SccpUserAddress userAddress)
        throws java.util.TooManyListenersException, javax.jain.ListenerAlreadyRegisteredException, javax.jain.InvalidUserAddressException;
    /**
      * Removes a SccpListener from the list of registered SccpListeners
      * of thie SccpProviderImpl.
      *
      * @param listener  The listener to be removed from this provider.
      * @exception javax.jain.ListenerNotRegisteredException  Thrown if there is no
      * such listener registered with this provider.
      */
    public void removeSccpListener(SccpListener listener)
        throws javax.jain.ListenerNotRegisteredException;
    public SccpListeners[] getSccpListeners();
    /**
      * Returns the SccpStackImpl to which this SccpProviderImpl is
      * attached.
      *
      * @return The attached SccpStack.
      */
    public SccpStack getSccpStack();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
