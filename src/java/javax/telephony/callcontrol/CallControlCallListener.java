package javax.telephony.callcontrol;

import  javax.telephony.CallListener;

/**
 * The <CODE>CallControlCallListener</CODE> interface reports all changes to 
 * the <CODE>Call</CODE> object. This interface extends the 
 * <CODE>CallListener</CODE> interfaces. The 
 * <CODE>CallControlConnectionListener</CODE> and 
 * <CODE>CallControlTerminalConnectionListener</CODE> interfaces extend this
 * interface.
 * <p>
 * This interface supports no additional methods.
 * 
 * @see javax.telephony.Call
 * @see javax.telephony.CallEvent
 * @see javax.telephony.callcontrol.CallControlCall
 * @see javax.telephony.callcontrol.CallControlCallEvent
 * @see javax.telephony.callcontrol.CallControlConnectionListener
 * @see javax.telephony.callcontrol.CallControlTerminalConnectionListener
 * @version 07/01/01 1.4
 * @since 1.4
 */
public interface CallControlCallListener extends CallListener {
}

