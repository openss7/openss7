package javax.telephony.callcontrol;

import  javax.telephony.*;

/**
 * The <CODE>CallControlCallEvent</CODE> interface is the base interface for 
 * all call control package Call-related events. All events which pertain to the
 * <CODE>CallControlCall</CODE> interface must extend this interface. 
 * Events which extend this interface are reported via the 
 * <CODE>CallControlCallListener</CODE> interface. This interface
 * extends both the <CODE>CallControlEvent</CODE> and core <CODE>CallEvent</CODE>
 * interfaces.
 * <p>
 * The <CODE>CallControlConnectionEvent</CODE> and 
 * <CODE>CallControlTerminalConnectionEvent</CODE> events extend this interface. 
 * <p>
 * <H4>Additional Call Information</H4>
 *
 * This interface supports methods which return additional information
 * regarding the telephone call. Specifically, it returns the <EM>calling
 * address</EM>, <EM>calling terminal</EM>, <EM>called address</EM>, and
 * <EM>last redirected address</EM> information. This information is returned
 * by the <CODE>getCallingAddress()</CODE>, <CODE>getCallingTerminal()</CODE>,
 * <CODE>getCalledAddress()</CODE>, and <CODE>getLastRedirectedAddress()</CODE>
 * methods on this interface, respectively.
 * <p>
 *
 * @see javax.telephony.Call
 * @see javax.telephony.CallEvent
 * @see javax.telephony.callcontrol.CallControlCall
 * @see javax.telephony.callcontrol.CallControlConnectionEvent
 * @see javax.telephony.callcontrol.CallControlTerminalConnectionEvent
 * @see javax.telephony.callcontrol.CallControlCallListener
 * @version 07/01/01 1.4
 * @since 1.4
 */

public interface CallControlCallEvent extends CallControlEvent, CallEvent {


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

}

