
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
      * @exception MandatoryParameterNotSetException  Thrown if all of
      * the mandatory parameters required by this SccpProviderImpl to
      * send the SccpEvent are not set.
      */
    public void sendSccpEvent(SccpEvent event)
        throws MandatoryParameterNotSetExcpetion;
    /**
      * Adds a SccpListener to the list of registered Event Listeners of
      * this SccpProviderImpl.
      *
      * @param listener The feature to be added in the SccpListener
      * attribute
      * @param userAddress The feaeture to the added to the SccpListener
      * attribute.
      * @exception TooManyListenersException  Thrown if a limit is
      * placed on the allowable number of registered SccpListeners, and
      * this limit has been reached.
      * @exception ListenerAlreadyRegisteredException  Thrown if the
      * listener supplied is already registered for the specified
      * userAddress with this SccpProviderimpl.
      * @exception InvalidUserAddressException  Thrown if the
      * userAddress supplied is not a valid address.
      */
    public void addSccpListener(SccpListener listener, SccpUserAddress userAddress)
        throws TooManyListenersException, LIstenerAlreadyRegisteredException, InvalidUserAddressException;
    /**
      * Removes a SccpListener from the list of registered SccpListeners
      * of thie SccpProviderImpl.
      *
      * @param listener  The listener to be removed from this provider.
      * @exception ListenerNotRegisteredException  Thrown if there is no
      * such listener registered with this provider.
      */
    public void removeSccpListener(SccpListener listener)
        throws ListenerNotRegisteredException;
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
