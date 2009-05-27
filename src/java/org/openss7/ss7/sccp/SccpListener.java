
package org.openss7.ss7.sccp;

/** This interface defines the methods required by all and any SCCP user
  * application to receive and process JAIN SCCP Events that are emitted
  * by an object implementing the
  * {@link org.openss7.ss7.sccp.SccpProvider} interface.
  * It must be noted that any object that implements the SccpListener
  * interface is referred to as a SccpListenerImpl; the SccpProvider, a
  * SccpProviderImpl.  The events emitted by SccpProviderImpl may be
  * {@link org.openss7.ss7.sccp.SccpEvent} events.  These events
  * correspond to SCCP indication and confirmation primitives emitted as
  * events by the SccpProviderImpl.  SCCP indication and confirmation
  * events represent SCCP service primitives that are sent from SCCP to
  * the SCCP User. <p>
  *
  * A SCCP User application will only receive indication events once it
  * has registered as an EventListener of a SccpProviderImpl.  The user
  * application will register with the SccpProviderImpl with a user
  * address by invoking the SccpProviderImpl's {@link
  * org.openss7.ss7.sccp.SccpProvider#addSccpListener} method.  Any
  * events addressed to a user address belonging to a user application
  * will be passed to that user application. <p>
  *
  * The user application will use the SccpProviderImpl to send SCCP
  * messages by sending {@link org.openss7.ss7.sccp.SccpEvent} events to
  * that SccpProviderImpl's {@link
  * org.openss7.ss7.sccp.SccpProvider#sendSccpEvent} method.
  *
  * @author     Monavacon Limited
  * @version    1.2.2
  * @see        SccpProvider
  */
public interface SccpListener extends java.util.EventListener {
    /** 
      * Process an SCCP Event emitted by the SccpProvider.
      * Indication events can only be passed from SCCP (the
      * SccpProviderImpl) to the SCCP User (SccpListenerImpl) and is
      * enforced by the JAIN SCCP Event hierarchy and error checking
      * within the specification, to ensure SCCP like processing.
      * @param event  The new event supplied by the method.
      */
    public void processSccpEvent(SccpEvent event);
    /** 
      * Adds a {@link org.openss7.ss7.sccp.SccpUserAddress} to the list
      * of user addresses used by a SccpListenerImpl.
      * A SccpListenerImpl may choose to register as an Event Listener of
      * a SccpProvider for all of its user addresses.  Any Sccp messages
      * addressed to any of this SccpListenerImpl's user addresses will
      * be passed to this SccpListenerImpl as events by the
      * SccpProviderImpl.
      */
    public void addUserAddress(SccpUserAddress userAddress)
        throws UserAddressLimitException;
    /** 
      * Removes the {@link org.openss7.ss7.sccp.SccpUserAddress} from the
      * list of user addresses held by a SccpListenerImpl. */
    public void removeUserAddress(SccpUserAddress userAddress)
        throws InvalidAddressException;
    /** 
      * Returns the list of {@link org.openss7.ss7.sccp.SccpUserAddress}
      * used by this SccpListenerImpl.
      * @return The SccpUserAddress[] of user addresses used by this
      * SccpListenerImpl.
      */
    public SccpUserAddress[] getUserAddressList()
        throws UserAddressEmptyException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
