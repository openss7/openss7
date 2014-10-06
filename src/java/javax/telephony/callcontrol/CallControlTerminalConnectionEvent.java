package javax.telephony.callcontrol;

import  javax.telephony.TerminalConnectionEvent;

/**
 * The <CODE>CallControlTerminalConnectionEvent</CODE> interface is the base 
 * event interface for all <CODE>callcontrol</CODE> package 
 * TerminalConnection-related events. All events which
 * pertain to the <CODE>CallControlTerminalConnection</CODE> interface must
 * extend this interface. This interface extends both 
 * the <CODE>CallControlCallEvent</CODE> and 
 * <CODE>TerminalConnectionEvent</CODE> interface.
 * <p>
 * The <CODE>callcontrol</CODE> package defines events which are reported when
 * the <CODE>CallControlTerminalConnection</CODE> state changes. These events
 * have following IDs:
 * <CODE>CALLCTL_TERMINAL_CONNECTION_BRIDGED</CODE>,
 * <CODE>CALLCTL_TERMINAL_CONNECTION_DROPPED</CODE>,
 * <CODE>CALLCTL_TERMINAL_CONNECTION_HELD</CODE>,
 * <CODE>CALLCTL_TERMINAL_CONNECTION_IN_USE</CODE>,
 * <CODE>CALLCTL_TERMINAL_CONNECTION_RINGING</CODE>,
 * <CODE>CALLCTL_TERMINAL_CONNECTION_TALKING</CODE> and
 * <CODE>CALLCTL_TERMINAL_CONNECTION_UNKNOWN</CODE>.
 * <p>
 * Each of these events is reported to the corresponding method for 
 * an application registered as a 
 * <CODE>CallControlTerminalConnectionListener</CODE>.
 * 
 * @see javax.telephony.TerminalConnection
 * @see javax.telephony.CallListener
 * @see javax.telephony.TerminalConnectionEvent
 * @see javax.telephony.callcontrol.CallControlTerminalConnection
 * @see javax.telephony.callcontrol.CallControlCall
 * @see javax.telephony.callcontrol.CallControlCallEvent
 * @see javax.telephony.callcontrol.CallControlCallListener
 * @see javax.telephony.callcontrol.CallControlTerminalConnectionListener
 * @version 07/01/01 1.4
 * @since 1.4
 */
 
public interface CallControlTerminalConnectionEvent 
                    extends CallControlCallEvent, TerminalConnectionEvent {
                        
    /**
     * The <CODE>CALLCTL_TERMINAL_CONNECTION_BRIDGED</CODE> event indicates 
     * that the state of the <CODE>CallControlTerminalConnection</CODE> object 
     * has changed to <CODE>CallControlTerminalConnection.BRIDGED</CODE>.
     * <p>
     * This constant indicates a specific event passed via
     * <CODE>CallControlTerminalConnectionEvent</CODE> event, and is reported
     * on the <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlTerminalConnectionListener#terminalConnectionBridged( 
     * CallControlTerminalConnectionEvent )} 
     * method.
     */
    public static final int CALLCTL_TERMINAL_CONNECTION_BRIDGED = 364;
    
    /**
     * The <CODE>CALLCTL_TERMINAL_CONNECTION_DROPPED</CODE> event indicates 
     * that the state of the <CODE>CallControlTerminalConnection</CODE> object 
     * has changed to <CODE>CallControlTerminalConnection.DROPPED</CODE>.
     * <p>
     * This constant indicates a specific event passed via
     * <CODE>CallControlTerminalConnectionEvent</CODE> event, and is reported
     * on the <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlTerminalConnectionListener#terminalConnectionDropped(
     * CallControlTerminalConnectionEvent )} 
     * method.
     */
    public static final int CALLCTL_TERMINAL_CONNECTION_DROPPED = 365;
    
    /**
     * The <CODE>CALLCTL_TERMINAL_CONNECTION_HELD</CODE> event indicates 
     * that the state of the <CODE>CallControlTerminalConnection</CODE> object
     * has changed to <CODE>CallControlTerminalConnection.HELD</CODE>.
     * <p>
     * This constant indicates a specific event passed via
     * <CODE>CallControlTerminalConnectionEvent</CODE> event, and is reported
     * on the <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlTerminalConnectionListener#terminalConnectionHeld(
     * CallControlTerminalConnectionEvent )} 
     * method.
     */
    public static final int CALLCTL_TERMINAL_CONNECTION_HELD    = 366;
    
    /**
     * The <CODE>CALLCTL_TERMINAL_CONNECTION_IN_USE</CODE> event indicates 
     * that the state of the <CODE>CallControlTerminalConnection</CODE> object 
     * has changed to <CODE>CallControlTerminalConnection.INUSE</CODE>.
     * <p>
     * This constant indicates a specific event passed via
     * <CODE>CallControlTerminalConnectionEvent</CODE> event, and is reported
     * on the <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlTerminalConnectionListener#terminalConnectionInUse(
     * CallControlTerminalConnectionEvent )} 
     * method.
     */
    public static final int CALLCTL_TERMINAL_CONNECTION_IN_USE  = 367;
    
    /**
     * The <CODE>CALLCTL_TERMINAL_CONNECTION_RINGING</CODE> event indicates 
     * that the state of the <CODE>CallControlTerminalConnection</CODE> object 
     * has changed to <CODE>CallControlTerminalConnection.RINGING</CODE>.
     * <p>
     * This constant indicates a specific event passed via
     * <CODE>CallControlTerminalConnectionEvent</CODE> event, and is reported
     * on the <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlTerminalConnectionListener#terminalConnectionRinging(
     * CallControlTerminalConnectionEvent )} 
     * method.
     */
    public static final int CALLCTL_TERMINAL_CONNECTION_RINGING = 368;
    
    /**
     * The <CODE>CALLCTL_TERMINAL_CONNECTION_TALKING</CODE> event indicates 
     * that the state of the <CODE>CallControlTerminalConnection</CODE> object
     * has changed to <CODE>CallControlTerminalConnection.TALKING</CODE>.
     * <p>
     * This constant indicates a specific event passed via
     * <CODE>CallControlTerminalConnectionEvent</CODE> event, and is reported
     * on the <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlTerminalConnectionListener#terminalConnectionTalking(
     * CallControlTerminalConnectionEvent )}
     * method.
     */
    public static final int CALLCTL_TERMINAL_CONNECTION_TALKING = 369;
    
    /**
     * The <CODE>CALLCTL_TERMINAL_CONNECTION_UNKNOWN</CODE> event indicates 
     * that the state of the <CODE>CallControlTerminalConnection</CODE> object
     * has changed to <CODE>CallControlTerminalConnection.UNKNOWN</CODE>.
     * <p>
     * This constant indicates a specific event passed via
     * <CODE>CallControlTerminalConnectionEvent</CODE> event, and is reported
     * on the <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlTerminalConnectionListener#terminalConnectionUnknown(
     * CallControlTerminalConnectionEvent )}
     * method.
     */
    public static final int CALLCTL_TERMINAL_CONNECTION_UNKNOWN = 370;
}

