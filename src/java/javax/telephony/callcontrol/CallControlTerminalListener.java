package javax.telephony.callcontrol;

import  javax.telephony.TerminalListener;

/**
 * The <CODE>CallControlTerminalListener</CODE> interface reports all changes 
 * that happen to the <CODE>Terminal</CODE> object. This interface extends the 
 * <CODE>TerminalListener</CODE> interfaces.
 *
 * @see javax.telephony.Terminal
 * @see javax.telephony.TerminalListener
 * @see javax.telephony.callcontrol.CallControlTerminalEvent
 * @since 1.4
 */

public interface CallControlTerminalListener extends TerminalListener {
    
    /**
     * The <CODE>terminalDoNotDisturb</CODE> method is called to report that 
     * the state of the do not disturb feature has changed for the 
     * <CODE>Terminal</CODE>. The new state of the do not disturb 
     * feature can be get by the  <CODE>getDoNotDisturb()</CODE> method on
     * the <CODE>CallControlTerminal</CODE>
     * <p>
     * @param event A <CODE>CallControlTerminalEvent</CODE> with eventID 
     * <CODE>CallControlTerminalEvent.CALLCTL_TERMINAL_EVENT_DO_NOT_DISTURB</CODE>.
     *
     * @see CallControlTerminalEvent#CALLCTL_TERMINAL_EVENT_DO_NOT_DISTURB
     * @see CallControlTerminal#getDoNotDisturb
     */
    public void terminalDoNotDisturb( CallControlTerminalEvent event );
}

