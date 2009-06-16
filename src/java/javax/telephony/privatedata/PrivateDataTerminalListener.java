package javax.telephony.privatedata;

import javax.telephony.TerminalListener;

/**
 * The <CODE>PrivateDataTerminalListener</CODE> interface reports the receipt
 * of platform-specific terminal information. This interface extends the 
 * <CODE>TerminalListener</CODE> interfaces.
 *
 * @see javax.telephony.TerminalListener
 * @see javax.telephony.privatedata.PrivateData
 * @see javax.telephony.privatedata.PrivateDataEvent
 * @since 1.4
 */
public interface PrivateDataTerminalListener extends TerminalListener {
    
    /**
     * The <CODE>terminalPrivateData</CODE> method is called to report 
     * that terminal platform-specific information has sent.
     * <p>
     * @param event A PrivateDataEvent with eventID 
     * <CODE>PrivateDataEvent.PRIVATE_DATA_TERMINAL_EVENT</CODE>.
     *
     * @see PrivateDataEvent#PRIVATE_DATA_TERMINAL_EVENT
     * @see PrivateData
     */
    public void terminalPrivateData(PrivateDataEvent event);
}
