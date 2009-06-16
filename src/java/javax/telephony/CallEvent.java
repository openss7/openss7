/*
 * Copyright (c) 1999 Sun Microsystems, Inc. All Rights Reserved.
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

package javax.telephony;
import  javax.telephony.Call;

/**
 * The <CODE>CallEvent</CODE> interface is the base interface for
 * all Call-related events.
 * All Call-related events must extend this interface.
 * Events which extend this interface are reported via the
 * <CODE>CallListener</CODE> interface.
 * <p>
 * An individual <CODE>CallEvent</CODE> conveys one of a series of different
 * possible Call state changes;
 * the specific Call state change is indicated by
 * the <CODE>Event.getID()</CODE>
 * value returned by the event.
 * <p>
 * The core package defines events which are reported when the Call changes
 * state. The event IDs (as returned by <CODE>Event.getID()</CODE>) are:
 * <CODE>CALL_ACTIVE</CODE> and
 * <CODE>CALL_INVALID</CODE>. Also, the core package defines the
 * <CODE>CALL_EVENT_TRANSMISSION_ENDED</CODE> event ID which is sent when
 * the the application can no longer listen for Call events.
 * <p>
 * These event IDs are reported via methods in the <CODE>CallListener</CODE>
 * interface (see method <CODE>CallListener.CallInvalid</CODE>)
 *
 * <p>
 * The <CODE>ConnectionEvent</CODE> and <CODE>TerminalConnectionEvent</CODE> interfaces extend this
 * interface. This reflects the fact that all Connection and TerminalConnection
 * events are reported via the <CODE>CallListener</CODE> interface.
 * <p>
 * The <CODE>CallEvent.getCall()</CODE> method on this interface returns the Call
 * associated with the Call event.
 * <p>
 * @see javax.telephony.Event
 * @see javax.telephony.ConnectionEvent
 * @see javax.telephony.TerminalConnectionEvent
 * @see javax.telephony.CallListener
 * @see javax.telephony.Call
 * @version 04/06/99 1.4
 */
public interface CallEvent extends Event {

  /**
   * Returns the Call object associated with this Call event.
   * <p>
   * @return The Call associated with this event.
   */
  public Call getCall();


/**
 * Event id codes returned for this interface
 */

/**
 * The <CODE>CALL_ACTIVE</CODE> event indicates that the state of the
 * Call object has changed to <CODE>Call.ACTIVE</CODE>.
 * <p>
 * This constant corresponds to a specific call state change, is passed via a
 * <CODE>CallEvent</CODE> event, and is reported
 * to the <CODE>CallListener.callActive</CODE> method.
 */
public static final int CALL_ACTIVE = 101;

/**
 * The <CODE>CALL_INVALID</CODE> event indicates that the state of the
 * Call object has changed to <CODE>Call.INVALID</CODE>.
 * <p>
 * This constant corresponds to a specific call state change, is passed via a
 * <CODE>CallEvent</CODE> event, and is reported
 * to the <CODE>CallListener.callInvalid</CODE> method.
 */
public static final int CALL_INVALID = 102;

/**
 * The <CODE>CALL_EVENT_TRANSMISSION_ENDED</CODE> event indicates that the application
 * will no longer receive Call events on the instance of the
 * <CODE>CallListener</CODE>.
 * <p>
 * This constant corresponds to this specific circumstance, is passed via a
 * <CODE>CallEvent</CODE> event, and is reported
 * to the <CODE>CallListener.callEventTransmissionEnded</CODE> method.
 */
public static final int CALL_EVENT_TRANSMISSION_ENDED = 103;

}

