package javax.telephony.callcontrol;

import  javax.telephony.AddressListener;

/**
 * The <CODE>CallControlAddressListener</CODE> interface reports all changes 
 * that happen to the <CODE>CallControlAddress</CODE> object, reported as <CODE>CallControlAddressEvent</CODE>s.
 * This interface extends the <CODE>AddressListener</CODE> interface.
 *
 * @see javax.telephony.Address
 * @see javax.telephony.AddressListener
 * @see javax.telephony.callcontrol.CallControlAddressEvent
 * @since 1.4
 */
public interface CallControlAddressListener extends AddressListener {
    
    /**
     * This method is called to report that 
     * the state of the do not disturb feature has changed for the 
     * <CODE>CallControlAddress</CODE>. The new state of the do not disturb 
     * feature can be gotten by the <CODE>getDoNotDisturb()</CODE> method on 
     * the <CODE>CallControlAddress</CODE>.
     * <p>
     * @param event A <CODE>CallControlAddressEvent</CODE> with eventID 
     * <CODE>CallControlAddressEvent.CALLCTL_ADDRESS_EVENT_DO_NOT_DISTURB</CODE>.
     *
     * @see CallControlAddressEvent#CALLCTL_ADDRESS_EVENT_DO_NOT_DISTURB
     * @see CallControlAddress#getDoNotDisturb
     */
    public void addressDoNotDisturb(CallControlAddressEvent event);
    
    
    /**
     * This method is called to report that 
     * the state of the forward feature has changed for the 
     * <CODE>CallControlAddress</CODE>. The new state of the forward
     * feature can be gotten by the <CODE>getForwarding()</CODE> method on 
     * the <CODE>CallControlAddress</CODE>
     * <p>
     * @param event A <CODE>CallControlAddressEvent</CODE> with eventID 
     * <CODE>CallControlAddressEvent.CALLCTL_ADDRESS_EVENT_FORWARD</CODE>.
     *
     * @see CallControlAddressEvent#CALLCTL_ADDRESS_EVENT_FORWARD
     * @see CallControlAddress#getForwarding
     */
    public void addressForwarded(CallControlAddressEvent event);
    
    /**
     * This method is called to report that 
     * the state of the message waiting feature has changed for 
     * the <CODE>CallControlAddress</CODE>. The new state of the state of the 
     * message waiting  feature can be gotten by the 
     * <CODE>addressMessageWaiting()</CODE> method on the 
     * <CODE>CallControlAddress</CODE>
     * <p>
     * @param event A <CODE>CallControlAddressEvent</CODE> with eventID 
     * <CODE>CallControlAddressEvent.CALLCTL_ADDRESS_EVENT_MESSAGE_WAITING</CODE>.
     *
     * @see CallControlAddressEvent#CALLCTL_ADDRESS_EVENT_MESSAGE_WAITING
     * @see CallControlAddress#getMessageWaiting
     */
    public void addressMessageWaiting(CallControlAddressEvent event);

}

