package javax.telephony.callcenter;

import  javax.telephony.*;
import  javax.telephony.CallEvent;

/**
 * The <CODE>CallCenterCallEvent</CODE> interface is the interface exposed for all events
 * reported for the <CODE>CallCenterCall</CODE> interface.
 * This interface extends
 * the <CODE>CallCenterEvent</CODE> interface and the core <CODE>CallEvent</CODE>
 * interface.
 * <p>
 * All <CODE>CallCenterCallEvents</CODE> are reported via
 * the <CODE>CallCenterCallListener</CODE> interface,
 * to objects registered as Listeners on a <CODE>CallCenterCall</CODE>.
 * Objects register as Listeners of <CODE>CallCenterCallEvents</CODE> by implementing
 * the <CODE>CallCenterCallListener</CODE> interface and invoking <CODE>Call.addListener</CODE>.
 * <p>
 * @see javax.telephony.Call
 * @see javax.telephony.CallEvent
 * @see javax.telephony.callcenter.CallCenterCall
 * @since 1.4
 */
public interface CallCenterCallEvent extends CallCenterEvent, CallEvent 
{

  /**
   * Returns the calling Address associated with this call. The calling
   * Address is defined as the Address which placed the telephone call.
   * <p>
   * If the calling address is unknown or not yet known, this method returns
   * null.
   * <p>
   * @return The calling Address.
   */
  public Address getCallingAddress();


  /**
   * Returns the calling Terminal associated with this Call. The calling
   * Terminal is defined as the Terminal which placed the telephone call.
   * <p>
   * If the calling Terminal is unknown or not yet know, this method returns
   * null.
   * <p>
   * @return The calling Terminal.
   */
  public Terminal getCallingTerminal();


  /**
   * Returns the called Address associated with this Call. The called
   * Address is defined as the Address to which the call has been originally
   * placed.
   * <p>
   * If the called address is unknown or not yet known, this method returns
   * null.
   * <p>
   * @return The called Address.
   */
  public Address getCalledAddress();


  /**
   * Returns the last redirected Address associated with this Call.
   * The last redirected Address is the Address at which the current telephone
   * call was placed immediately before the current Address. This is common
   * if a Call is forwarded to several Addresses before being answered.
   * <p>
   * If the the last redirected address is unknown or not yet known, this
   * method returns null.
   * <p>
   * @return The last redirected Address for this telephone Call.
   */
  public Address getLastRedirectedAddress();
 
 
   /**
    * This event ID
    * indicates that the application data associated with the Call has changed.
    */
    public static final int CALLCENT_CALL_APPLICATION_DATA_EVENT = 316;

 
   /**
    * Returns the new application data for this call. This method returns null
    * if the application data has been cleared from the call.
    * <p>
    * @return The data object, null if it has been cleared.
    */
   public Object getApplicationData();
}
