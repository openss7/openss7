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

package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

/**
  * The JainMtpProvider interface models the services available from an
  * JainMtpProvider implementatoin.  Each vendor-specific JainMtpProviderImpl
  * class will implement the JainMtpProvider interface. <p>
  *
  * This interface provides the implementation object with the ability
  * to offer services to JainMtpListener implementation objects
  * (applications). <p>
  *
  * This interface must be implemented by any Object representing the
  * Jain Mtp layer of any SS7 stack that interacts directly with a
  * proprietary (stack vendor specific) implementation of the MTP layer.
  * This interface defines the methods that will be used by any
  * registered MTP User application implementing the JainMtpListener
  * interface to send transfer primitives.  It must be noted that any
  * object that implements the JainMtpListener interface is referred to
  * as JainMtpListenerImpl; JainMtpProvider, as JainMtpProviderImpl and
  * JainMtpStack as JainMtpStackImpl. <p>
  *
  * A MTP User application may send a number of transfer request
  * primitives as MtpEvent events to a provider object.  This interface
  * defines methods for: <p>
  *
  * <ul>
  * <li>Sending MtpEvent events.  This will trigger the sending of MTP
  * transfer request primitives to the destinationnode.
  * <li>Adding a JainMtpListener to its list of registered Event Listeners.
  * The JainMtpListenerImpl will register with  a User Address and becomes
  * an Event Listener of that JainMtpProviderImpl object.  Once registered
  * as an Event Listener, an JainMtpListenerImpl will receive any transfer
  * indication or management indication primitives address to its User
  * Address as Events.
  * <li>Removing a JainMtpListener from the JainMtpProviderImpl's list of Event
  * Listeners.  Once a JainMtpListenerImpl is de-registered, it will no
  * longer receive any Events from that JainMtpProviderImpl.
  * </ul>
  *
  * @author     Brian Bidulock
  * @version    1.2.2
  * @see        JainMtpListener
  * @see        JainMtpStack
  */
public interface JainMtpProvider extends java.util.EventListener {
    /** Adds a JainMtpListener to the list of registered Event
      * Listeners of this JainMtpProviderImpl.
      * This is also used to add new MTP User Addresses being
      * handled by an MTP Listener so that a Listener can
      * register for more than one MTP User Address by
      * repeatedly calling this method, provided that the
      * implementation maximum number of listeners for a given
      * provider would not be exceeded.
      * @param listener
      * The feature to be added in the JainMtpListener attribute
      * @param userAddress
      * The feature to the added to the JainMtpListener
      * attribute.
      * @exception java.util.TooManyListenersException
      * Thrown if a limit is placed on the allowable number of
      * registered JainMtpListeners, and this limit has been
      * reached.
      * @exception javax.jain.ListenerAlreadyRegisteredException
      * Thrown if the listener supplied is already registered
      * for the specified userAddress with this
      * JainMtpProviderimpl.
      * @exception javax.jain.ss7.isup.InvalidListenerException
      * Thrown if the Listener is null.
      * @exception javax.jain.InvalidAddressException
      * Thrown if the userAddress supplied is not a valid
      * address.
      * @exception javax.jain.ss7.isup.SendStackException
      * Thrown if the registration could not be sent to the
      * underlying stack.  */
    public void addMtpListener(JainMtpListener listener, MtpUserAddress userAddress)
        throws java.util.TooManyListenersException,
            javax.jain.ListenerAlreadyRegisteredException,
            javax.jain.ss7.isup.InvalidListenerException,
            javax.jain.InvalidAddressException,
            javax.jain.ss7.isup.SendStackException;
    /** Removes a JainMtpListener from the list of registered
      * JainMtpListeners of thie JainMtpProviderImpl.
      * This removes a JainMtpListener from the list of
      * registered Listeners being services by this Provider,
      * if it is the last User Address being handled by the
      * MTP Listener.
      * @param listener
      * The listener to be removed from this provider.
      * @param userAddress
      * The MTP User Adress handled by the JainMtpListener
      * object.
      * @exception javax.jain.ListenerNotRegisteredException
      * Thrown when the JainMtpListener object to be removed is
      * not registered as an Event Listener of this
      * JainMtpProvider object.
      * @exception javax.jain.ss7.isup.InvalidListenerException
      * Thrown if the listener is null.
      * @exception javax.jain.InvalidAddressException
      * Thrown if the userAddress has an invalid format.  */
    public void removeMtpListener(JainMtpListener listener, MtpUserAddress userAddress)
        throws javax.jain.ListenerNotRegisteredException,
            javax.jain.ss7.isup.InvalidListenerException,
            javax.jain.InvalidAddressException;
    /** Sends a Transfer Request primitive to the MTP layer of
      * the SS7 protocol stack.
      * This will trigger the transmission to the destination
      * node of the transfer request primtive along with any
      * asociated data.
      *
      * @param event
      * The event to be transmitted.
      * @exception javax.jain.MandatoryParameterNotSetException
      * Thrown if all of the mandatory parameters required by
      * this JainMtpProvider object to send the MTP message
      * out to the network are not set.  Note that different
      * implementations of this JainMtpProvider interface will
      * mandate that a different set of parameters be set for
      * each of the MtpEvents.  It is recommended that the
      * detail message returned in the
      * MandatoryParameterNotSetException should be a String of
      * the form, <p> <center><code>"Parameter: not
      * set"</code></center><p>
      * @exception javax.jain.ss7.isup.SendStackException
      * Thrown if the event could not be sent to the underlying
      * stack.
      * @exception javax.jain.ss7.isup.ParameterRangeInvalidException
      * Thrown if the event being sent has a Circuit
      * Identification Code or a Destination Point Code that
      * has not been registered for by the Listener
      * application.
      * @exception javax.jain.VersionNotSupportedException
      * Thrown if the event is a variant specific event that is
      * different from the underlying stack variant.  */
    public void sendMtpEvent(MtpEvent event)
        throws javax.jain.MandatoryParameterNotSetException,
            javax.jain.ss7.isup.SendStackException,
            javax.jain.ss7.isup.ParameterRangeInvalidException,
            javax.jain.VersionNotSupportedException;
    /** Gets the list of MTP listeners.
      * @return
      * Array of MTP listeners. */
    public JainMtpListener[] getMtpListeners();
    /** Returns the JainMtpStackImpl to which this
      * JainMtpProviderImpl is attached.
      * @return
      * The attached JainMtpStack.  */
    public JainMtpStack getMtpStack();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
