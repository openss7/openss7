package javax.telephony.privatedata;

import javax.telephony.CallListener;

/**
 * The <CODE>PrivateDataCallListener</CODE> interface reports the receipt
 * of platform-specific call information. This interface extends the 
 * <CODE>CallListener</CODE> interfaces.
 *
 * @see javax.telephony.CallListener
 * @see javax.telephony.privatedata.PrivateData
 * @see javax.telephony.privatedata.PrivateDataEvent
 * @since 1.4
 */
public interface PrivateDataCallListener extends CallListener {
    
    /**
     * The <CODE>callPrivateData</CODE> method is called to report 
     * that call platform-specific information has sent.
     * <p>
     * @param event A PrivateDataEvent with eventID 
     * <CODE>PrivateDataEvent.PRIVATE_DATA_CALL_EVENT</CODE>.
     *
     * @see PrivateDataEvent#PRIVATE_DATA_CALL_EVENT
     * @see PrivateData
     */
    public void callPrivateData(PrivateDataEvent event);
}
