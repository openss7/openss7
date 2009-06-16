package javax.telephony.callcontrol;

import javax.telephony.AddressEvent;

/**
 * The <CODE>CallControlAddressEvent</CODE> interface is the base interface for
 * all <CODE>callcontrol</CODE> package Address-related events.
 * All CallControlAddress-related events must extend this interface.
 * Events which extend this interface are reported via the
 * <CODE>CallControlAddressListener</CODE> interface. This interface
 * extends both the <CODE>CallControlEvent</CODE> and core 
 * <CODE>AddressEvent</CODE> interfaces.
 * <p>
 * @see javax.telephony.Address
 * @see javax.telephony.AddressEvent
 * @see javax.telephony.AddressListener
 * @see javax.telephony.callcontrol.CallControlAddressListener
 * @see javax.telephony.callcontrol.CallControlEvent
 * @version 07/01/01 1.4
 * @since 1.4
 */
public interface CallControlAddressEvent extends CallControlEvent, AddressEvent {
    
    /**
     * The <CODE>CALLCTL_ADDRESS_EVENT_DO_NOT_DISTURB</CODE> event indicates 
     * that the state of the do not disturb feature has changed for the 
     * <CODE>Address</CODE>. 
     * <p>
     * This constant indicates a specific event passed via the
     * <CODE>CallControlAddressEvent</CODE> event, and is reported
     * on the <CODE>CallControlAddressListener</CODE> interface by calling the 
     * {@link CallControlAddressListener#addressDoNotDisturb( 
     * CallControlAddressEvent )} method. The new state of the do not disturb 
     * feature can be gotten by the <CODE>getDoNotDisturb()</CODE> method on the
     * <CODE>CallControlAddress</CODE>
     */
    public static final int CALLCTL_ADDRESS_EVENT_DO_NOT_DISTURB  = 350;
    
    /**
     * The <CODE>CALLCTL_ADDRESS_EVENT_FORWARD</CODE> event indicates 
     * that the state of the forward feature has changed for the 
     * <CODE>Address</CODE>. 
     * <p>
     * This constant indicates a specific event passed via the
     * <CODE>CallControlAddressEvent</CODE> event, and is reported
     * on the <CODE>CallControlAddressListener</CODE> interface by calling the 
     * {@link CallControlAddressListener#addressForwarded( 
     * CallControlAddressEvent )} method. The new state of the forward 
     * feature can be gotten by the <CODE>getForwarding()</CODE> method on the
     * <CODE>CallControlAddress</CODE>
     */
    public static final int CALLCTL_ADDRESS_EVENT_FORWARD         = 351;
    
    /**
     * The <CODE>CALLCTL_ADDRESS_EVENT_MESSAGE_WAITING</CODE> event indicates 
     * that the state of the message waiting feature has changed for the Address. 
     * <p>
     * This constant indicates a specific event passed via the
     * <CODE>CallControlAddressEvent</CODE> event, and is reported
     * on the <CODE>CallControlAddressListener</CODE> interface by calling the 
     * {@link CallControlAddressListener#addressMessageWaiting( 
     * CallControlAddressEvent )} method. The new state of the message waiting 
     * feature can gotten by the <CODE>getMessageWaiting()</CODE> method on the
     * <CODE>CallControlAddress</CODE>
     */
    public static final int CALLCTL_ADDRESS_EVENT_MESSAGE_WAITING = 352;


  /**
   * Returns true if the do not disturb feature is activated, false otherwise.
   * <p>
   * @return True if the do not disturb feature is activated, false otherwise.
   */
  public boolean getDoNotDisturbState();


  /**
   * Returns the current forwarding on the Address.
   * <p>
   * @return An array of CallControlForwarding objects.
   */
  public CallControlForwarding[] getForwarding();
  

  /**
   * Returns the current message waiting state of the Address.
   * <p>
   * @return The message waiting state of the Address.
   */
  public boolean getMessageWaitingState();
  

}

