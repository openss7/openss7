package javax.telephony.callcontrol;

import  javax.telephony.ConnectionListener;

/**
 * The <CODE>CallControlConnectionListener</CODE> is an extension of 
 * both the <CODE>CallControlCallListener</CODE> and the 
 * <CODE>ConnectionListener</CODE> interfaces. Therefore it reports call 
 * control state changes both of the Call and its Connections.
 * 
 * @see javax.telephony.Connection
 * @see javax.telephony.ConnectionEvent
 * @see javax.telephony.ConnectionListener
 * @see javax.telephony.callcontrol.CallControlConnection
 * @see javax.telephony.callcontrol.CallControlConnectionEvent
 * @see javax.telephony.callcontrol.CallControlCallListener
 * @version 07/01/01 1.4
 * @since 1.4
 */
public interface CallControlConnectionListener extends CallControlCallListener,
													   ConnectionListener {
    
    /**
     * The <CODE>connectionAlerting</CODE> method is called to report that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.ALERTING</CODE>.
     * <p>
     * @param event A CallControlConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_CONNECTION_ALERTING</CODE>.
     *
     * @see CallControlConnectionEvent#CALLCTL_CONNECTION_ALERTING
     * @see CallControlConnection#ALERTING
     */
    public void connectionAlerting(CallControlConnectionEvent event);
    
    /**
     * The <CODE>connectionDialing</CODE> method is called to report that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.DIALING</CODE>.
     * <p>
     * @param event A CallControlConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_CONNECTION_DIALING</CODE>.
     *
     * @see CallControlConnectionEvent#CALLCTL_CONNECTION_DIALING
     * @see CallControlConnection#DIALING
     */
    public void connectionDialing(CallControlConnectionEvent event);
    
    /**
     * The <CODE>connectionDisconnected</CODE> method is called to report that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.DISCONNECTED</CODE>.
     * <p>
     * @param event A CallControlConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_CONNECTION_DISCONNECTED</CODE>.
     *
     * @see CallControlConnectionEvent#CALLCTL_CONNECTION_DISCONNECTED
     * @see CallControlConnection#DISCONNECTED
     */
    public void connectionDisconnected(CallControlConnectionEvent event);
    
    /**
     * The <CODE>connectionEstablished</CODE> method is called to report that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.ESTABLISHED</CODE>.
     * <p>
     * @param event A CallControlConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_CONNECTION_ESTABLISHED</CODE>.
     *
     * @see CallControlConnectionEvent#CALLCTL_CONNECTION_ESTABLISHED
     * @see CallControlConnection#ESTABLISHED
     */
    public void connectionEstablished(CallControlConnectionEvent event);
    
    /**
     * The <CODE>connectionFailed</CODE> method is called to report that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.FAILED</CODE>.
     * <p>
     * @param event A CallControlConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_CONNECTION_FAILED</CODE>.
     *
     * @see CallControlConnectionEvent#CALLCTL_CONNECTION_FAILED
     * @see CallControlConnection#FAILED
     */
    public void connectionFailed(CallControlConnectionEvent event);
    
    /**
     * The <CODE>connectionInitiated</CODE> method is called to report that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.INITIATED</CODE>.
     * <p>
     * @param event A CallControlConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_CONNECTION_INITIATED</CODE>.
     *
     * @see CallControlConnectionEvent#CALLCTL_CONNECTION_INITIATED
     * @see CallControlConnection#INITIATED
     */
    public void connectionInitiated(CallControlConnectionEvent event);
    
    /**
     * The <CODE>connectionNetworkAlerting</CODE> method is called to report that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.NETWORK_ALERTING</CODE>.
     * <p>
     * @param event A CallControlConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_CONNECTION_NETWORK_ALERTING</CODE>.
     *
     * @see CallControlConnectionEvent#CALLCTL_CONNECTION_NETWORK_ALERTING
     * @see CallControlConnection#NETWORK_ALERTING
     */
    public void connectionNetworkAlerting(CallControlConnectionEvent event);
    
    /**
     * The <CODE>connectionNetworkReached</CODE> method is called to report that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.NETWORK_REACHED</CODE>.
     * <p>
     * @param event A CallControlConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_CONNECTION_NETWORK_REACHED</CODE>.
     *
     * @see CallControlConnectionEvent#CALLCTL_CONNECTION_NETWORK_REACHED
     * @see CallControlConnection#NETWORK_REACHED
     */
    public void connectionNetworkReached(CallControlConnectionEvent event);
    
    /**
     * The <CODE>connectionOffered</CODE> method is called to report that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.OFFERED</CODE>.
     * <p>
     * @param event A CallControlConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_CONNECTION_OFFERED</CODE>.
     *
     * @see CallControlConnectionEvent#CALLCTL_CONNECTION_OFFERED
     * @see CallControlConnection#OFFERED
     */
    public void connectionOffered(CallControlConnectionEvent event);
    
    /**
     * The <CODE>connectionQueued</CODE> method is called to report that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed
     * to <CODE>CallControlConnection.QUEUED</CODE>.
     * <p>
     * @param event A CallControlConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_CONNECTION_QUEUED</CODE>.
     *
     * @see CallControlConnectionEvent#CALLCTL_CONNECTION_QUEUED
     * @see CallControlConnection#QUEUED
     */
    public void connectionQueued(CallControlConnectionEvent event);
    
    /**
     * The <CODE>connectionUnknown</CODE> method is called to report that 
     * the state of the <CODE>CallControlConnection</CODE> object has changed to 
     * <CODE>CallControlConnection.UNKNOWN</CODE>.
     * <p>
     * @param event A CallControlConnectionEvent with eventID 
     * <CODE>CallControlConnectionEvent.CALLCTL_CONNECTION_UNKNOWN</CODE>.
     *
     * @see CallControlConnectionEvent#CALLCTL_CONNECTION_UNKNOWN
     * @see CallControlConnection#UNKNOWN
     */
    public void connectionUnknown(CallControlConnectionEvent event);
}

