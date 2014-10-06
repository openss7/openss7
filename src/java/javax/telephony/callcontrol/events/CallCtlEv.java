/*
#pragma ident "%W%      %E%     SMI"

 * Copyright (c) 1996 Sun Microsystems, Inc. All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for NON-COMMERCIAL purposes and without
 * fee is hereby granted provided that this copyright notice
 * appears in all copies. Please refer to the file "copyright.html"
 * for further important copyright and licensing information.
 *
 * SUN MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
 * THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NON-INFRINGEMENT. SUN SHALL NOT BE LIABLE FOR
 * ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR
 * DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
 */

package javax.telephony.callcontrol.events;
import  javax.telephony.events.Ev;

/**
 * The <CODE>CallCtlEv</CODE> is the base event for all events in the call
 * control package. Each event in this package must extend this interface. This
 * interface extends the core <CODE>Ev</CODE> interface.
 * <p>
 * This interface contains the <CODE>getCallControlCause()</CODE> method which
 * returns a call control package specific cause for the event. Cause codes
 * pertaining to this package are defined in this interface as well.
 * <p>
 * In the call control package, this interface is extended by the following
 * interfaces: <CODE>CallCtlCallEv</CODE>, <CODE>CallCtlAddrEv</CODE>, and
 * <CODE>CallCtlTermEv</CODE>.
 * <p>
 * @see javax.telephony.events.Ev
 * @see javax.telephony.callcontrol.events.CallCtlCallEv
 * @see javax.telephony.callcontrol.events.CallCtlAddrEv
 * @see javax.telephony.callcontrol.events.CallCtlTermEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcontrol.CallControlEvent}
 */

public interface CallCtlEv extends Ev {

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
   * may also returns the <CODE>Ev.CAUSE_NORMAL</CODE> and the
   * <CODE>Ev.CAUSE_UNKNOWN</CODE> values as defined in the core package.
   * <p>
   * @return The call control package cause of the event.
   */
  public int getCallControlCause();
}
