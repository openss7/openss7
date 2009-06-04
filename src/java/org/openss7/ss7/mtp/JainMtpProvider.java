
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
    /**
      * Sends a Transfer Request primitive to the MTP layer of the SS7
      * protocol stack.  This will trigger the transmission to the
      * destination node of the transfer request primtive along with any
      * asociated data.
      *
      * @param event The event to be transmitted.
      * @exception MandatoryParameterNotSetException  Thrown if all of
      * the mandatory parameters required by this JainMtpProviderImpl to
      * send the MtpEvent are not set.
      */
    public void sendMtpEvent(MtpEvent event)
        throws MandatoryParameterNotSetException;
    /**
      * Adds a JainMtpListener to the list of registered Event Listeners of
      * this JainMtpProviderImpl.
      *
      * @param listener The feature to be added in the JainMtpListener
      * attribute
      * @param userAddress The feaeture to the added to the JainMtpListener
      * attribute.
      * @exception TooManyListenersException  Thrown if a limit is
      * placed on the allowable number of registered JainMtpListeners, and
      * this limit has been reached.
      * @exception ListenerAlreadyRegisteredException  Thrown if the
      * listener supplied is already registered for the specified
      * userAddress with this JainMtpProviderimpl.
      * @exception InvalidUserAddressException  Thrown if the
      * userAddress supplied is not a valid address.
      */
    public void addMtpListener(JainMtpListener listener, MtpUserAddress userAddress)
        throws TooManyListenersException, ListenerAlreadyRegisteredException, InvalidUserAddressException;
    /**
      * Removes a JainMtpListener from the list of registered JainMtpListeners
      * of thie JainMtpProviderImpl.
      *
      * @param listener  The listener to be removed from this provider.
      * @exception ListenerNotRegisteredException  Thrown if there is no
      * such listener registered with this provider.
      */
    public void removeMtpListener(JainMtpListener listener)
        throws ListenerNotRegisteredException;
    public JainMtpListener[] getMtpListeners();
    /**
      * Returns the JainMtpStackImpl to which this JainMtpProviderImpl is
      * attached.
      *
      * @return The attached JainMtpStack.
      */
    public JainMtpStack getMtpStack();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
