package javax.telephony.callcenter;

import javax.telephony.AddressEvent;

/**
 * The <CODE>ACDAddressEvent</CODE> interface is the interface exposed for all events
 * reported for the <CODE>ACDAddress</CODE> interface.
 * This interface extends
 * the <CODE>CallCenterEvent</CODE> interface and the core <CODE>AddressEvent</CODE>
 * interface.
 * <p>
 * All <CODE>ACDAddressEvents</CODE> are reported via
 * the <CODE>ACDAddressListener</CODE> interface,
 * to objects registered as Listeners for the <CODE>ACDAddress</CODE> associated with the Agent.
 * Objects register as Listeners of <CODE>ACDAddressEvents</CODE> by implementing
 * the <CODE>ACDAddressListener</CODE> interface and invoking <CODE>Address.addListener</CODE>.
 * <p>
 * The call center package reports state changes
 * in <CODE>Agents</CODE> which are associated
 * with <CODE>ACDAddresses</CODE>.
 * <p>
 * Each state change is reported via 
 * an <CODE>ACDAddressEvent</CODE>.
 * An <CODE>ACDAddressEvent</CODE> carries an event ID which indicates 
 * which specific <CODE>Agent</CODE> state change occurred.
 * <p>
 * The state change indicated by a particular <CODE>ACDAddressEvent</CODE>,
 * indicated in turn by the ID value,
 * may be determined by calling
 * the <CODE>Event.getID</CODE> method on the event.
 * <p>
 * @see javax.telephony.Address
 * @see javax.telephony.AddressEvent
 * @see javax.telephony.callcenter.Agent
 * @see javax.telephony.callcenter.ACDAddress
 * @since 1.4
 */
public interface ACDAddressEvent extends CallCenterEvent, AddressEvent 
{
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.BUSY</CODE> state.
    * @see Agent#BUSY
    */
    public static final int ACD_ADDRESS_BUSY = 300;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.LOG_OUT</CODE> state.
    * @see Agent#LOG_OUT
    */
    public static final int ACD_ADDRESS_LOGGED_OFF = 301;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.LOG_IN</CODE> state.
    * @see Agent#LOG_IN
    */
    public static final int ACD_ADDRESS_LOGGED_ON = 302;

   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.NOT_READY</CODE> state.
    * @see Agent#NOT_READY
    */
    public static final int ACD_ADDRESS_NOT_READY = 303;

   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.READY</CODE> state.
    * @see Agent#READY
    */
    public static final int ACD_ADDRESS_READY = 304;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.UNKNOWN</CODE> state.
    * @see Agent#UNKNOWN
    */
    public static final int ACD_ADDRESS_UNKNOWN = 305;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.WORK_NOT_READY</CODE> state.
    * @see Agent#WORK_NOT_READY
    */
    public static final int ACD_ADDRESS_WORK_NOT_READY = 306;
    
   /**
    * This event ID
    * indicates that an Agent has moved into the <CODE>Agent.WORK_READY</CODE> state.
    * @see Agent#WORK_READY
    */
    public static final int ACD_ADDRESS_WORK_READY = 307;
   
    /**
     * Returns the <CODE>Agent</CODE> associated with the
     * <CODE>ACDAddress</CODE>.
     * <p>
     * @return The associated <CODE>Agent</CODE>.
     */
    public Agent getAgent();

 
}
