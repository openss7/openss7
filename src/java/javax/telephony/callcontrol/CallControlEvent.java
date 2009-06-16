package javax.telephony.callcontrol;

import  javax.telephony.Event;

/**
 * The <CODE>CallControlEvent</CODE> is the base event for all events in the 
 * <CODE>callcontrol</CODE> package. Each event in this package extends this 
 * interface. This interface extends the core <CODE>Event</CODE> interface.
 * <p>
 * This interface contains the <CODE>getCallControlCause()</CODE> method which
 * returns a call control package specific cause for the event. Cause codes
 * pertaining to this package are defined in this interface as well.
 * <p>
 * This interface is extended by the following interfaces: 
 * <CODE>CallControlCallEvent</CODE>, <CODE>CallControlAddressEvent</CODE>, and
 * <CODE>CallControlTerminalEvent</CODE>.
 *
 * @see javax.telephony.Event
 * @see javax.telephony.callcontrol.CallControlCallEvent
 * @see javax.telephony.callcontrol.CallControlAddressEvent
 * @see javax.telephony.callcontrol.CallControlTerminalEvent
 * @version 07/01/01 1.4
 * @since 1.4
 */

public interface CallControlEvent extends Event {
 
  /**
   * Cause code indicating that the call was put on hold and another retrieved
   * in an atomic operation, typically on single line telephones.
   */
  public static final int CAUSE_ALTERNATE = 202;


  /**
   * Cause code indicating that the call encountered a busy endpoint.
   */
  public static final int CAUSE_BUSY = 203;


  /**
   * Cause code indicating the event is related to the callback feature.
   */
  public static final int CAUSE_CALL_BACK = 204;


  /**
   * Cause code indicating that the call was not answered before a timer
   * elapsed.
   */
  public static final int CAUSE_CALL_NOT_ANSWERED = 205;


  /**
   * Cause code indicating that the call was redirected by the call pickup
   * feature.
   */
  public static final int CAUSE_CALL_PICKUP = 206;


  /**
   * Cause code indicating the event is related to the conference feature.
   */
  public static final int CAUSE_CONFERENCE = 207;


  /**
   * Cause code indicating the event is related to the do not disturb feature.
   */
  public static final int CAUSE_DO_NOT_DISTURB = 208;


  /**
   * Cause code indicating the event is related to the park feature.
   */
  public static final int CAUSE_PARK = 209;


  /**
   * Cause code indicating the event is related to the redirect feature.
   */
  public static final int CAUSE_REDIRECTED = 210;


  /**
   * Cause code indicating that the call encountered a reorder tone.
   */
  public static final int CAUSE_REORDER_TONE = 211;


  /**
   * Cause code indicating the event is related to the transfer feature.
   */
  public static final int CAUSE_TRANSFER = 212;


  /**
   * Cause code indicating that the call encountered the "all trunks busy"
   * condition.
   */
  public static final int CAUSE_TRUNKS_BUSY = 213;


  /**
   * Cause code indicating the event is related to the unhold feature.
   */
  public static final int CAUSE_UNHOLD = 214;


  /**
   * Returns the call control package cause associated with the event. The
   * cause values are integer constants defined in this interface. This method
   * may also returns the {@link javax.telephony.Event#CAUSE_NORMAL  
   * Event.CAUSE_NORMAL } and the {@link 
   * javax.telephony.Event#CAUSE_UNKNOWN Event.CAUSE_UNKNOWN} values as defined
   * in the core package.
   * <p>
   * @return The call control package cause of the event.
   */
  public int getCallControlCause();  
    
}

