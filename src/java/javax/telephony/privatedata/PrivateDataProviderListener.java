package javax.telephony.privatedata;

import javax.telephony.ProviderListener;

/**
 * The <CODE>PrivateDataProviderListener</CODE> interface reports the receipt
 * of platform-specific provider information. This interface extends the 
 * <CODE>ProviderListener</CODE> interfaces.
 *
 * @see javax.telephony.ProviderListener
 * @see javax.telephony.privatedata.PrivateData
 * @see javax.telephony.privatedata.PrivateDataEvent
 * @since 1.4
 */
public interface PrivateDataProviderListener extends ProviderListener {
    
    /**
     * The <CODE>providerPrivateData</CODE> method is called to report 
     * that terminal platform-specific information has sent.
     * <p>
     * @param event A PrivateDataEvent with eventID 
     * <CODE>PrivateDataEvent.PRIVATE_DATA_PROVIDER_EVENT</CODE>.
     *
     * @see PrivateDataEvent#PRIVATE_DATA_PROVIDER_EVENT
     * @see PrivateData
     */
    public void providerPrivateData(PrivateDataEvent event);
}
