package javax.telephony.callcontrol;

import javax.telephony.ConnectionEvent;

/**
 * The <CODE>CallControlConnectionEvent</CODE> interface is the base interface 
 * for all <CODE>callcontrol</CODE> package Connection-related events. All 
 * events which pertain to the <CODE>CallControlConnection</CODE> interface 
 * must extend this interface. This interface extends both the 
 * <CODE>CallControlCallEvent</CODE> and <CODE>ConnectionEvent</CODE> 
 * interfaces.
 * <p>
 * The <CODE>callcontrol</CODE> package defines events which are reported when
 * the <CODE>CallControlConnection</CODE> state changes. These events have the
 * following IDs:
 * <CODE>CALLCTL_CONNECTION_ALERTING</CODE>, 
 * <CODE>CALLCTL_CONNECTION_DIALING</CODE>,
 * <CODE>CALLCTL_CONNECTION_DISCONNECTED</CODE>, 
 * <CODE>CALLCTL_CONNECTION_ESTABLISHED</CODE>, 
 * <CODE>CALLCTL_CONNECTION_FAILED</CODE>, 
 * <CODE>CALLCTL_CONNECTION_INITIATED</CODE>, 
 * <CODE>CALLCTL_CONNECTION_NETWORK_ALERTING</CODE>, 
 * <CODE>CALLCTL_CONNECTION_NETWORK_REACHED</CODE>, 
 * <CODE>CALLCTL_CONNECTION_OFFERED</CODE>,
 * <CODE>CALLCTL_CONNECTION_QUEUED</CODE> and 
 * <CODE>CALLCTL_CONNECTION_UNKNOWN</CODE>. 
 * <p>
 * Each of these events is reported to the corresponding method for 
 * an application registered as a <CODE>CallControlConnectionListener</CODE>
 * or <CODE>CallControlTerminalConnectionListener</CODE>.
 * 
 * @see javax.telephony.Connection
 * @see javax.telephony.CallListener
 * @see javax.telephony.ConnectionEvent
 * @see javax.telephony.callcontrol.CallControlConnection
 * @see javax.telephony.callcontrol.CallControlCall
 * @see javax.telephony.callcontrol.CallControlCallEvent
 * @see javax.telephony.callcontrol.CallControlCallListener
 * @see javax.telephony.callcontrol.CallControlConnectionListener
 * @see javax.telephony.callcontrol.CallControlTerminalConnectionListener
 * @version 07/01/01 1.4
 * @since 1.4
 */
 
public interface CallControlConnectionEvent extends CallControlCallEvent, 
                                                    ConnectionEvent {

    /**
     * The <CODE>CALLCTL_CONNECTION_ALERTING</CODE> event indicates that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed
     * to <CODE>CallControlConnection.ALERTING</CODE>.
     * <p>
     * This constant indicates a specific event passed via the 
     * <CODE>CallControlConnectionEvent</CODE> event, and is reported on the 
     * <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlConnectionListener#connectionAlerting(
     * CallControlConnectionEvent )} method.
     */
    public static final int CALLCTL_CONNECTION_ALERTING         = 353;
    
    /**
     * The <CODE>CALLCTL_CONNECTION_DIALING</CODE> event indicates that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed
     * to <CODE>CallControlConnection.DIALING</CODE>.
     * <p>
     * This constant indicates a specific event passed via the 
     * <CODE>CallControlConnectionEvent</CODE> event, and is reported on the 
     * <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlConnectionListener#connectionDialing(
     * CallControlConnectionEvent )} method.
     */
    public static final int CALLCTL_CONNECTION_DIALING          = 354;
    
    /**
     * The <CODE>CALLCTL_CONNECTION_DISCONNECTED</CODE> event indicates that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to
     * <CODE>CallControlConnection.DISCONNECTED</CODE>.
     * <p>
     * This constant indicates a specific event passed via the 
     * <CODE>CallControlConnectionEvent</CODE> event, and is reported on the 
     * <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlConnectionListener#connectionDisconnected(
     * CallControlConnectionEvent )} method.
     */
    public static final int CALLCTL_CONNECTION_DISCONNECTED     = 355;
    
    /**
     * The <CODE>CALLCTL_CONNECTION_ESTABLISHED</CODE> event indicates that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.ESTABLISHED</CODE>.
     * <p>
     * This constant indicates a specific event passed via the 
     * <CODE>CallControlConnectionEvent</CODE> event, and is reported on the 
     * <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlConnectionListener#connectionEstablished(
     * CallControlConnectionEvent )} method.
     */
    public static final int CALLCTL_CONNECTION_ESTABLISHED      = 356;
    
    /**
     * The <CODE>CALLCTL_CONNECTION_FAILED</CODE> event indicates that the 
     * state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.FAILED</CODE>.
     * <p>
     * This constant indicates a specific event passed via the 
     * <CODE>CallControlConnectionEvent</CODE> event, and is reported on the 
     * <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlConnectionListener#connectionFailed(
     * CallControlConnectionEvent )} method.
     */
    public static final int CALLCTL_CONNECTION_FAILED           = 357;
    
    /**
     * The <CODE>CALLCTL_CONNECTION_INITIATED</CODE> event indicates that the 
     * state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.INITIATED</CODE>.
     * <p>
     * This constant indicates a specific event passed via the 
     * <CODE>CallControlConnectionEvent</CODE> event, and is reported on the 
     * <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlConnectionListener#connectionInitiated(
     * CallControlConnectionEvent )} method.
     */
    public static final int CALLCTL_CONNECTION_INITIATED        = 358;
    
    /**
     * The <CODE>CALLCTL_CONNECTION_NETWORK_ALERTING</CODE> event indicates 
     * that the state of the <CODE>CallControlConnection</CODE> object has 
     * changed to <CODE>CallControlConnection.NETWORK_ALERTING</CODE>.
     * <p>
     * This constant indicates a specific event passed via the 
     * <CODE>CallControlConnectionEvent</CODE> event, and is reported on the 
     * <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlConnectionListener#connectionNetworkAlerting(
     * CallControlConnectionEvent )} method.
     */
    public static final int CALLCTL_CONNECTION_NETWORK_ALERTING = 359;
    
    /**
     * The <CODE>CALLCTL_CONNECTION_NETWORK_REACHED</CODE> event indicates that
     * the state of the <CODE>CallControlConnection</CODE> object has changed 
     * to <CODE>CallControlConnection.NETWORK_REACHED</CODE>.
     * <p>
     * This constant indicates a specific event passed via the 
     * <CODE>CallControlConnectionEvent</CODE> event, and is reported on the 
     * <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlConnectionListener#connectionNetworkReached(
     * CallControlConnectionEvent )} method.
     */
    public static final int CALLCTL_CONNECTION_NETWORK_REACHED  = 360;
    
    /**
     * The <CODE>CALLCTL_CONNECTION_OFFERED</CODE> event indicates that the 
     * state of the <CODE>CallControlConnection</CODE> object has changed to
     * <CODE>CallControlConnection.OFFERED</CODE>.
     * <p>
     * This constant indicates a specific event passed via the 
     * <CODE>CallControlConnectionEvent</CODE> event, and is reported on the 
     * <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlConnectionListener#connectionOffered(
     * CallControlConnectionEvent )} method.
     */
    public static final int CALLCTL_CONNECTION_OFFERED          = 361;
    
    /**
     * The <CODE>CALLCTL_CONNECTION_QUEUED</CODE> event indicates that the 
     * state of the <CODE>CallControlConnection</CODE> object has changed to
     * <CODE>CallControlConnection.QUEUED</CODE>.
     * <p>
     * This constant indicates a specific event passed via the 
     * <CODE>CallControlConnectionEvent</CODE> event, and is reported on the 
     * <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlConnectionListener#connectionQueued(
     * CallControlConnectionEvent )} method.
     */
    public static final int CALLCTL_CONNECTION_QUEUED           = 362;
    
    /**
     * The <CODE>CALLCTL_CONNECTION_UNKNOWN</CODE> event indicates that the 
     * state of the <CODE>CallControlConnection</CODE> object has changed to
     * <CODE>CallControlConnection.UNKNOWN</CODE>.
     * <p>
     * This constant indicates a specific event passed via the 
     * <CODE>CallControlConnectionEvent</CODE> event, and is reported on the 
     * <CODE>CallControlCallListener</CODE> interface by calling the 
     * {@link CallControlConnectionListener#connectionUnknown(
     * CallControlConnectionEvent )} method.
     */
    public static final int CALLCTL_CONNECTION_UNKNOWN          = 363;
    
    


  /**
   * Returns the digits that have already been dialed.
   * <p>
   * @return The digits that have already been dialed.
   */
  public String getDigits();
    

  /**
   * Returns the number of Connections which are queued at the Address of this
   * Connection.
   * <p>
   * @return The number of queued Connections.
   */
  public int getNumberInQueue();
    
}

