package javax.telephony.privatedata;

import javax.telephony.Event;

/**
 * The <CODE>PrivateDataEvent</CODE> interface is the base event for all events
 * in the <CODE>privatedata</CODE> package. Each event in this package must 
 * extend this interface. This event notifies the application via the corresponding
 * listeners that platform-specific information was sent. This interface extends the
 * core <CODE>Event</CODE> interface.
 * <p>
 * This interface contains the <CODE>getPrivateData()</CODE> method which
 * returns a platform-specific information. 
 * 
 * @see PrivateDataAddressListener
 * @see PrivateDataCallListener
 * @see PrivateDataProviderListener
 * @see PrivateDataTerminalListener
 * @see PrivateData
 * @since 1.4
 */
public interface PrivateDataEvent extends Event {

    /**
     * The <CODE>PRIVATE_DATA_ADDRESS_EVENT</CODE> event sends platform-specific
     * information to an <CODE>PrivateDataAddressListener</CODE>.
     *
     * @see PrivateDataAddressListener
     */
    public static final int PRIVATE_DATA_ADDRESS_EVENT = 600;
    
    /**
     * The <CODE>PRIVATE_DATA_ADDRESS_EVENT</CODE> event sends platform-specific
     * information to an <CODE>PrivateDataAddressListener</CODE>.
     *
     * @see PrivateDataCallListener
     */
    public static final int PRIVATE_DATA_CALL_EVENT = 601;
    
    /**
     * The <CODE>PRIVATE_DATA_ADDRESS_EVENT</CODE> event sends platform-specific
     * information to an <CODE>PrivateDataAddressListener</CODE>.
     *
     * @see PrivateDataProviderListener
     */
    public static final int PRIVATE_DATA_PROVIDER_EVENT = 602;
    
    /**
     * The <CODE>PRIVATE_DATA_ADDRESS_EVENT</CODE> event sends platform-specific
     * information to an <CODE>PrivateDataAddressListener</CODE>.
     *
     * @see PrivateDataTerminalListener
     */
    public static final int PRIVATE_DATA_TERMINAL_EVENT = 603;
    
    /**
   * Returns platform-specific information to the application. The format of
   * the data and the action that should be taken upon receipt of the data is
   * platform-dependent.
   * <p>
   * @return The platform-specific data.
   * @see PrivateData
   */
  public Object getPrivateData();
}
