package javax.telephony.callcenter;

import javax.telephony.Event;

/**
 * The <CODE>CallCenterEvent</CODE> interface is the base event interface for all
 * call center package events. This interface extends the core {@link javax.telephony.Event}
 * interface. All call center package events must extend this interface.
 * <p>
 * <CODE>CallCenterEvent</CODE> events are reported via
 * the following Listener interfaces:
 *  <CODE>ACDAddressListener</CODE>,
 *  <CODE>AgentTerminalListener</CODE> and
 *  <CODE>CallCenterCallListener</CODE>.
 * <p>
 * This interface contains a single method <CODE>getCallCenterCause()</CODE>
 * which returns the call center package-specific cause value for the event.
 * <p>
 * The call center package defines the following interfaces which directly
 * extend this interface: <CODE>ACDAddressEvent</CODE>, <CODE>AgentTerminalEvent</CODE>,
 * and <CODE>CallCenterCallEvent</CODE>.
 * @see javax.telephony.Event
 * @see javax.telephony.callcenter.ACDAddressEvent
 * @see javax.telephony.callcenter.AgentTerminalEvent
 * @see javax.telephony.callcenter.CallCenterCallEvent
 * @since 1.4
 */
public interface CallCenterEvent extends Event
{
   /**
    * This cause indicates no agents were available to handle the call.
    */
   public static final int CAUSE_NO_AVAILABLE_AGENTS = 302;


  /**
   * Returns the call center package cause value for this event. This method
   * may also return the <CODE>Ev.CAUSE_NORMAL</CODE> constant or the
   * <CODE>Ev.CAUSE_UNKNOWN</CODE> constant.
   * <p>
   * @return The call center package cause for the event.
   */
  public int getCallCenterCause();
}

