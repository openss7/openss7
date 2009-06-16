package javax.telephony.callcontrol;

import  javax.telephony.TerminalConnectionListener;

/**
 * The <CODE>CallControlTerminalConnectionListener</CODE> is an extension
 * of the both the <CODE>CallControlConnectionListener</CODE> and the 
 * <CODE>TerminalConnectionListener</CODE> interfaces. Therefore it reports call 
 * control state changes of the Call, its Connections, and its TerminalConnections.
 * 
 * @see javax.telephony.TerminalConnection
 * @see javax.telephony.TerminalConnectionEvent
 * @see javax.telephony.TerminalConnectionListener
 * @see javax.telephony.callcontrol.CallControlTerminalConnection
 * @see javax.telephony.callcontrol.CallControlTerminalConnectionEvent
 * @see javax.telephony.callcontrol.CallControlCallListener
 * @see javax.telephony.callcontrol.CallControlConnectionListener
 * @see javax.telephony.callcontrol.CallControlTerminalConnectionListener
 * @version 07/01/01 1.4
 * @since 1.4
 */
public interface CallControlTerminalConnectionListener 
          extends CallControlConnectionListener, TerminalConnectionListener {
    
    /**
     * The <CODE>terminalConnectionBridged</CODE> method is called to report 
     * that the state of the <CODE>CallControlTerminalConnection</CODE> object
     * has changed to <CODE>CallControlTerminalConnection.BRIDGED</CODE>.
     * <p>
     * @param event A CallControlTerminalConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_TERMINAL_CONNECTION_BRIDGED</CODE>.
     *
     * @see CallControlTerminalConnectionEvent#CALLCTL_TERMINAL_CONNECTION_BRIDGED
     * @see CallControlTerminalConnection#BRIDGED
     */
    public void terminalConnectionBridged( CallControlTerminalConnectionEvent event );
    
    /**
     * The <CODE>terminalConnectionDropped</CODE> method is called to report 
     * that the state of the <CODE>CallControlTerminalConnection</CODE> object 
     * has changed to <CODE>CallControlTerminalConnection.DROPPED</CODE>.
     * <p>
     * @param event A CallControlTerminalConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_TERMINAL_CONNECTION_DROPPED</CODE>.
     *
     * @see CallControlTerminalConnectionEvent#CALLCTL_TERMINAL_CONNECTION_DROPPED
     * @see CallControlTerminalConnection#DROPPED
     */
    public void terminalConnectionDropped( CallControlTerminalConnectionEvent event );
    
    /**
     * The <CODE>terminalConnectionHeld</CODE> method is called to report that 
     * the state of the <CODE>CallControlTerminalConnection</CODE> object has
     * changed to <CODE>CallControlTerminalConnection.HELD</CODE>.
     * <p>
     * @param event A CallControlTerminalConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_TERMINAL_CONNECTION_HELD</CODE>.
     *
     * @see CallControlTerminalConnectionEvent#CALLCTL_TERMINAL_CONNECTION_HELD
     * @see CallControlTerminalConnection#HELD
     */
    public void terminalConnectionHeld( CallControlTerminalConnectionEvent event );
    
    /**
     * The <CODE>terminalConnectionInUse</CODE> method is called to report that 
     * the state of the <CODE>CallControlTerminalConnection</CODE> object has 
     * changed to <CODE>CallControlTerminalConnection.INUSE</CODE>.
     * <p>
     * @param event A CallControlTerminalConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_TERMINAL_CONNECTION_INUSE</CODE>.
     *
     * @see CallControlTerminalConnectionEvent#CALLCTL_TERMINAL_CONNECTION_IN_USE
     * @see CallControlTerminalConnection#INUSE
     */
    public void terminalConnectionInUse( CallControlTerminalConnectionEvent event );
    
    /**
     * The <CODE>terminalConnectionRinging</CODE> method is called to report that 
     * the state of the <CODE>CallControlTerminalConnection</CODE> object has 
     * changed to <CODE>CallControlTerminalConnection.RINGING</CODE>.
     * <p>
     * @param event A CallControlTerminalConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_TERMINAL_CONNECTION_RINGING</CODE>.
     *
     * @see CallControlTerminalConnectionEvent#CALLCTL_TERMINAL_CONNECTION_RINGING
     * @see CallControlTerminalConnection#RINGING
     */
    public void terminalConnectionRinging( CallControlTerminalConnectionEvent event );
    
    /**
     * The <CODE>terminalConnectionTalking</CODE> method is called to report that 
     * the state of the <CODE>CallControlTerminalConnection</CODE> object has 
     * changed to <CODE>CallControlTerminalConnection.TALKING</CODE>.
     * <p>
     * @param event A CallControlTerminalConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_TERMINAL_CONNECTION_TALKING</CODE>.
     *
     * @see CallControlTerminalConnectionEvent#CALLCTL_TERMINAL_CONNECTION_TALKING
     * @see CallControlTerminalConnection#TALKING
     */
    public void terminalConnectionTalking( CallControlTerminalConnectionEvent event );
    
    /**
     * The <CODE>terminalConnectionUnknown</CODE> method is called to report that 
     * the state of the <CODE>CallControlTerminalConnection</CODE> object has 
     * changed to <CODE>CallControlTerminalConnection.UNKNOWN</CODE>.
     * <p>
     * @param event A CallControlTerminalConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_TERMINAL_CONNECTION_UNKNOWN</CODE>.
     *
     * @see CallControlTerminalConnectionEvent#CALLCTL_TERMINAL_CONNECTION_UNKNOWN
     * @see CallControlTerminalConnection#UNKNOWN
     */
    public void terminalConnectionUnknown( CallControlTerminalConnectionEvent event );
}

