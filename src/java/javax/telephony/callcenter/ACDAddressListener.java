package javax.telephony.callcenter;

import javax.telephony.AddressListener;

/**
 * The <CODE>ACDAddressListener</CODE> interface extends the event
 * reporting of of the core <CODE>AddressListener</CODE> to include call center
 * related events. Applications instantiate an object which implements this
 * interface and use the <CODE>Address.addListener()</CODE> to request delivery
 * of events to this Listener object. Events will be delivered to this
 * interface only if the Provider is in the <CODE>Provider.IN_SERVICE</CODE>
 * state. All events which are reported via this interface must extend the
 * <CODE>ACDAddressEvent</CODE> interface.
 * <p>
 * With JTAPI 1.2, events were reported to Observers who registered to receive events.
 * With JTAPI 1.3, the Listener style of event reporting was introduced to the JTAPI Core, Media and Mobile packages.
 * With JTAPI 1.4, Listener style event reporting is being introduced to the remaining JTAPI packages,
 * and the Observer style interfaces are being deprecated.
 * <p>
 * State changes (to the application data and the any <CODE>CallCenterCall</CODE>-associated <CODE>CallCenterTrunk</CODE>s)
 * are reported as events to the Listener methods corresponding to the type of each event. 
 * <p>
 * Note that the <CODE>Agent</CODE> state changes are also reported via
 * the <CODE>AgentTerminalListener</CODE> for the <CODE>AgentTerminal</CODE> where the
 * <CODE>Agent</CODE> is logged in.
 * <p>
 * @since 1.4
 */


/**
 * The <CODE>ACDAddressListener</CODE> interface reports all changes 
 * that happen to the <CODE>ACDAddress</CODE> object. This interface 
 * extends the <CODE>AddressListener</CODE> interface.
 *
 * @see javax.telephony.Address
 * @see javax.telephony.AddressListener
 * @see javax.telephony.callcenter.ACDAddressEvent
 * @since 1.4
 */
public interface ACDAddressListener extends AddressListener
{
    /**
     * The <CODE>acdAddressBusy</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>ACDAddress</CODE> object 
     * has changed to <CODE>Agent.BUSY</CODE>.
     * <p>
     * @param event An <CODE>ACDAddressEvent</CODE> with eventID 
     * <CODE>ACDAddressEvent.ACD_ADDRESS_BUSY</CODE>.
     *
     * @see ACDAddressEvent#ACD_ADDRESS_BUSY
     */
    public void acdAddressBusy(ACDAddressEvent event);
    
    /**
     * The <CODE>acdAddressLoggedOff</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>ACDAddress</CODE> object 
     * has changed to <CODE>Agent.LOG_OFF</CODE>.
     * <p>
     * @param event An <CODE>ACDAddressEvent</CODE> with eventID 
     * <CODE>ACDAddressEvent.ACD_ADDRESS_LOGGED_OFF</CODE>.
     *
     * @see ACDAddressEvent#ACD_ADDRESS_LOGGED_OFF
     */
    public void acdAddressLoggedOff(ACDAddressEvent event);
    
    /**
     * The <CODE>acdAddressLoggedOn</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>ACDAddress</CODE> object 
     * has changed to <CODE>Agent.LOG_ON</CODE>.
     * <p>
     * @param event An <CODE>ACDAddressEvent</CODE> with eventID 
     * <CODE>ACDAddressEvent.ACD_ADDRESS_LOGGED_ON</CODE>.
     *
     * @see ACDAddressEvent#ACD_ADDRESS_LOGGED_ON
     */
    public void acdAddressLoggedOn(ACDAddressEvent event);
    
    /**
     * The <CODE>acdAddressNotReady</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>ACDAddress</CODE> object 
     * has changed to <CODE>Agent.NOT_READY</CODE>.
     * <p>
     * @param event An <CODE>ACDAddressEvent</CODE> with eventID 
     * <CODE>ACDAddressEvent.ACD_ADDRESS_NOT_READY</CODE>.
     *
     * @see ACDAddressEvent#ACD_ADDRESS_NOT_READY
     */
    public void acdAddressNotReady(ACDAddressEvent event);
    
    /**
     * The <CODE>acdAddressReady</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>ACDAddress</CODE> object 
     * has changed to <CODE>Agent.READY</CODE>.
     * <p>
     * @param event An <CODE>ACDAddressEvent</CODE> with eventID 
     * <CODE>ACDAddressEvent.ACD_ADDRESS_READY</CODE>.
     *
     * @see ACDAddressEvent#ACD_ADDRESS_READY
     */
    public void acdAddressReady(ACDAddressEvent event);
    
    /**
     * The <CODE>acdAddressUnknown</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>ACDAddress</CODE> object 
     * has changed to <CODE>Agent.UNKNOWN</CODE>.
     * <p>
     * @param event An <CODE>ACDAddressEvent</CODE> with eventID 
     * <CODE>ACDAddressEvent.ACD_ADDRESS_UNKNOWN</CODE>.
     *
     * @see ACDAddressEvent#ACD_ADDRESS_UNKNOWN
     */
    public void acdAddressUnknown(ACDAddressEvent event);
    
    /**
     * The <CODE>acdAddressWorkNotReady</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>ACDAddress</CODE> object 
     * has changed to <CODE>Agent.WORK_NOT_READY</CODE>.
     * <p>
     * @param event An <CODE>ACDAddressEvent</CODE> with eventID 
     * <CODE>ACDAddressEvent.ACD_ADDRESS_WORK_NOT_READY</CODE>.
     *
     * @see ACDAddressEvent#ACD_ADDRESS_WORK_NOT_READY
     */
    public void acdAddressWorkNotReady(ACDAddressEvent event);
    
    /**
     * The <CODE>acdAddressWorkReady</CODE> method is called to report that 
     * the state of an <CODE>Agent</CODE> associated with the <CODE>ACDAddress</CODE> object 
     * has changed to <CODE>Agent.WORK_READY</CODE>.
     * <p>
     * @param event An <CODE>ACDAddressEvent</CODE> with eventID 
     * <CODE>ACDAddressEvent.ACD_ADDRESS_WORK_READY</CODE>.
     *
     * @see ACDAddressEvent#ACD_ADDRESS_WORK_READY
     */
    public void acdAddressWorkReady(ACDAddressEvent event);
}
