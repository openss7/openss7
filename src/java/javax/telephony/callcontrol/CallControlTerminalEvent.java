package javax.telephony.callcontrol;

import javax.telephony.TerminalEvent;

/**
 * The <CODE>CallControlTerminalEvent</CODE> interface is the base interface for
 * all <CODE>callcontrol</CODE> package Terminal-related events.
 * All Terminal-related events must extend this interface.
 * Events which extend this interface are reported via the
 * <CODE>CallControlTerminalListener</CODE> interface. This interface
 * extends both the <CODE>CallControlEvent</CODE> and core 
 * <CODE>TerminalEvent</CODE> interfaces.
 * <p>
 * @see javax.telephony.Terminal
 * @see javax.telephony.TerminalEvent
 * @see javax.telephony.TerminalListener
 * @see javax.telephony.callcontrol.CallControlTerminalListener
 * @see javax.telephony.callcontrol.CallControlEvent
 * @version 07/01/01 1.4
 * @since 1.4
 */
public interface CallControlTerminalEvent extends CallControlEvent, TerminalEvent {
    
    /**
     * The <CODE>CALLCTL_TERMINAL_EVENT_DO_NOT_DISTURB</CODE> event indicates 
     * that the state of the do not disturb feature has changed for the 
     * <CODE>CallControlTerminal</CODE>. 
     * <p>
     * This constant indicates a specific event passed via
     * <CODE>CallControlTerminalEvent</CODE> event, and is reported
     * on the <CODE>CallControlTerminalListener</CODE> interface by calling the 
     * {@link CallControlTerminalListener#terminalDoNotDisturb( 
     * CallControlTerminalEvent )} method. The new state of the do not disturb 
     * feature can be gotten by the <CODE>getDoNotDisturb()</CODE> method on the
     * <CODE>CallControlTerminal</CODE>
     */
    public static final int CALLCTL_TERMINAL_EVENT_DO_NOT_DISTURB = 371;
    

  /**
   * Returns true if the do not disturb feature is activated, false otherwise.
   * <p>
   * @return True if the do not disturb feature is activated, false otherwise.
   */
  public boolean getDoNotDisturbState();
    
}

