
package org.openss7.ss7.mtp;

/**
  * This interface defines the methods required by all MTP user
  * applications to receive and process JAIN MAP Events that are emitted
  * by an object implementing the
  * {@link org.openss7.ss7.mtp.MtpProvider} interface.
  * It must be noted that any object than implements the MtpListener
  * interface is referred to as a MtpListenerImpl; the MtpProvider
  * interface, a MtpProviderImpl.  The events emitted by MtpProviderImpl
  * may be {@linke org.openss7.ss7.mtp.MtpEvent} events.  These events
  * correspond to MTP indication primitives emitted as events by the
  * MtpProviderImpl.  MTP indication events represent MTP service
  * primtives that are sent from MTP to the MTP User. <p>
  *
  * A MTP User application will only receive indication events once it
  * has registered as an EventListener of a MtpProviderImpl.  The user
  * application will register with the MtpProviderImpl with a User
  * Address by invoking the ProviderImpl's {@link
  * org.openss7.ss7.mtp.MtpProvider#addMtpListener} method.  Any events
  * address to a User Address belonging to a User Application will be
  * passed to that User application. <p>
  *
  * The User application will use the MtpProviderImpl to send MTP
  * messages by send {@link org.openss7.ss7.mtp.MtpEvent} events to that
  * MtpProviderImpl's
  * {@link org.openss7.ss7.mtp.MtpProvider#sendMtpEvent} method.
  *
  * @author     Monavacon Limited
  * @version    1.2.2
  * @see        MtpProvider
  */
public interface MtpListener extends java.util.EventListener {
    /**
      * Process an MTP Event emitted by the MtpProvider.  Indication
      * and Confirmation events can only be passed from MTP (the
      * MtpProviderImpl) to the MTP User (MtpListenerImpl) and is
      * enforced by the JAIN MTP Event hierarchy and error checking
      * within the specification, to ensure MTP like processing.
      *
      * @param event  The new even supplied by the method.
      */
    public void processMtpEvent(MtpEvent event);
    /**
      * Adds a {@link org.openss7.ss7.mtp.MtpUserAddress} class to the
      * list of user addresses used by a MtpListenerImpl.
      * A MtpListenerImpl may choose to register as an Event Listener of
      * a MtpProvider for all of its user addreses.  Any MTP messages
      * addressed to any of this MtpListenerImpl's user addresses will
      * be passed to this MtpListenerImpl as events by the
      * MtpProviderImpl.
      *
      * @param userAddress  The feature to be added to the UserAddress
      * attribute.
      */
    public void addUserAddress(MtpUserAddress userAddress)
	throws UserAddressLimitException;
    /**
      * Removes the {@link org.openss7.ss7.mtp.MtpUserAddress} class
      * form the list of user addresses held by a MtpListenerImpl.
      *
      * @param userAddress  The user address to be removed.
      */
    public void removeUserAddress(MtpUserAddress userAddress)
	throws InvalidAddressException;
    /**
      * Returns the list of {@link org.openss7.ss7.mtp.MtpUserAddress}
      * used by this MtpListenerImpl.
      *
      * @return The MtpUserAddress[] of user addresses used by this
      * MtpListenerImpl.
      */
    public MtpUserAddress[] getUserAddressList()
	throws UserAddressEmptyException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
