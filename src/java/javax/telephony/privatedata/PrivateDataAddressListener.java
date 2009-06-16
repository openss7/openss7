package javax.telephony.privatedata;

import javax.telephony.AddressListener;

/**
 * The <CODE>PrivateDataAddressListener</CODE> interface reports the receipt
 * of platform-specific address information. This interface extends the 
 * <CODE>AddressListener</CODE> interfaces.
 *
 * @see javax.telephony.AddressListener
 * @see javax.telephony.privatedata.PrivateData
 * @see javax.telephony.privatedata.PrivateDataEvent
 * @since 1.4
 */
public interface PrivateDataAddressListener extends AddressListener {
    
    /**
     * The <CODE>addressPrivateData</CODE> method is called to report 
     * that address platform-specific information has been sent.
     * <p>
     * @param event A PrivateDataEvent with eventID 
     * <CODE>PrivateDataEvent.PRIVATE_DATA_ADDRESS_EVENT</CODE>.
     *
     * @see PrivateDataEvent#PRIVATE_DATA_ADDRESS_EVENT
     * @see PrivateData
     */
    public void addressPrivateData(PrivateDataEvent event);
}
