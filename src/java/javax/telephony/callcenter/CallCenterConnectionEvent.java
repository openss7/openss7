package javax.telephony.callcenter;

import  javax.telephony.ConnectionEvent;

/**
 * The <CODE>CallCenterConnectionEvent</CODE> interface is the interface exposed for all events
 * reported for the <CODE>CallCenterConnection</CODE> interface.
 * This interface extends
 * the <CODE>CallCenterCallEvent</CODE> interface and the core <CODE>ConnectionEvent</CODE>
 * interface.
 * <p>
 * All <CODE>CallCenterConnectionEvents</CODE> are reported via
 * the <CODE>CallCenterCallListener</CODE> interface,
 * to objects registered as Listeners to a <CODE>CallCenterCall</CODE>.
 * Objects register as Listeners of <CODE>CallCenterConnectionEvents</CODE> by implementing
 * the <CODE>CallCenterCallListener</CODE> interface and invoking <CODE>Call.addListener</CODE>.
 * <p>
 * @see javax.telephony.Connection
 * @see javax.telephony.ConnectionEvent
 * @see javax.telephony.callcenter.CallCenterCallEvent
 * @since 1.4
 */
public interface CallCenterConnectionEvent extends CallCenterCallEvent, 
                                                   ConnectionEvent 
{
   /**
    * This event ID
    * indicates that <CODE>Connection</CODE> has moved into the <CODE>Connection.INPROGRESS</CODE>
    * state.
    */
    public static final int CALLCENT_CONNECTION_IN_PROGRESS = 319;
 
}
