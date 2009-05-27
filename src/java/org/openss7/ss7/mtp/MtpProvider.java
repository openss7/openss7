
package org.openss7.ss7.mtp;

import org.openss7.ss7.*;

/**
  * The MtpProvider interface models the services available from an
  * MtpProvider implementatoin.  Each vendor-specific MtpProviderImpl
  * class will implement the MtpProvider interface. <p>
  *
  * This interface provides the implementation object with the ability
  * to offer services to MtpListener implementation objects
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
  * <li>Adding a MtpListener to its list of registered Event Listeners.
  * The MtpListenerImpl will register with  a User Address and becomes
  * an Event Listener of that MtpProviderImpl object.  Once registered
  * as an Event Listener, an MtpListenerImpl will receive any transfer
  * indication or management indication primitives address to its User
  * Address as Events.
  * <li>Removing a MtpListener from the MtpProviderImpl's list of Event
  * Listeners.  Once a MtpListenerImpl is de-registered, it will no
  * longer receive any Events from that MtpProviderImpl.
  * </ul>
  *
  * @author     Brian Bidulock
  * @version    1.2.2
  * @see        MtpListener
  * @see        MtpStack
  */
public interface MtpProvider extends java.util.EventListener {
    /**
      * Sends a Transfer Request primitive to the MTP layer of the SS7
      * protocol stack.  This will trigger the transmission to the
      * destination node of the transfer request primtive along with any
      * asociated data.
      *
      * @param event The event to be transmitted.
      * @exception MandatoryParameterNotSetException  Thrown if all of
      * the mandatory parameters required by this MtpProviderImpl to
      * send the MtpEvent are not set.
      */
    public void sendMtpEvent(MtpEvent event)
        throws MandatoryParameterNotSetException;
    /**
      * Adds a MtpListener to the list of registered Event Listeners of
      * this MtpProviderImpl.
      *
      * @param listener The feature to be added in the MtpListener
      * attribute
      * @param userAddress The feaeture to the added to the MtpListener
      * attribute.
      * @exception TooManyListenersException  Thrown if a limit is
      * placed on the allowable number of registered MtpListeners, and
      * this limit has been reached.
      * @exception ListenerAlreadyRegisteredException  Thrown if the
      * listener supplied is already registered for the specified
      * userAddress with this MtpProviderimpl.
      * @exception InvalidUserAddressException  Thrown if the
      * userAddress supplied is not a valid address.
      */
    public void addMtpListener(MtpListener listener, MtpUserAddress userAddress)
        throws TooManyListenersException, ListenerAlreadyRegisteredException, InvalidUserAddressException;
    /**
      * Removes a MtpListener from the list of registered MtpListeners
      * of thie MtpProviderImpl.
      *
      * @param listener  The listener to be removed from this provider.
      * @exception ListenerNotRegisteredException  Thrown if there is no
      * such listener registered with this provider.
      */
    public void removeMtpListener(MtpListener listener)
        throws ListenerNotRegisteredException;
    public MtpListener[] getMtpListeners();
    /**
      * Returns the MtpStackImpl to which this MtpProviderImpl is
      * attached.
      *
      * @return The attached MtpStack.
      */
    public MtpStack getMtpStack();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
