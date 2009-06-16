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
 * The <CODE>MultiCallMetaEvent</CODE> interface is the base interface for
 * all multiple-call Call Meta events (multi-call MetaEvent).
 * All multi-call MetaEvent's must extend this interface.
 * Events which extend this interface are reported via the
 * <CODE>CallListener</CODE> interface.
 * <p>
 * An individual <CODE>MultiCallMetaEvent</CODE>
 * conveys, directly and with necessary details,
 * what an application needs to know to respond to
 * a multiple-call higher-level call event.
 * The specific MultiCallMetaEvent event is indicated by
 * the <CODE>Event.getID()</CODE>
 * value returned by the event.
 * <p>
 * The core package defines events which are reported when
 * high-level actions occur. The event IDs
 * (as returned by <CODE>Event.getID()</CODE>) are:
 * <CODE>MULTICALL_META_MERGE_STARTED</CODE>,
 * <CODE>MULTICALL_META_MERGE_ENDED</CODE>,
 * <CODE>MULTICALL_META_TRANSFER_STARTED</CODE> and
 * <CODE>MULTICALL_META_TRANSFER_ENDED</CODE>.
 * <p>
 * @see javax.telephony.CallEvent
 * @see javax.telephony.MetaEvent
 * @see javax.telephony.CallListener
 * @see javax.telephony.Call
 */
public interface MultiCallMetaEvent extends MetaEvent {

  /**
   * Returns the Call object associated with the result of the multiple-call operation
   * reported by this MultiCallMetaEvent event.
   * <p>
   * @return The result Call associated with this event.
   */
  public Call getNewCall();

  /**
   * Returns an array of Call objects which were
   * considered input to the multiple-call operation
   * reported by this MultiCallMetaEvent event.
   * <p>
   * @return An array of old Calls associated with this event's operation.
   */
  public Call[] getOldCalls();

/**
 * Event id codes returned for this interface
 */

/**
 * The <CODE>MULTICALL_META_MERGE_STARTED</CODE> event indicates that
 * calls are merging, and events will follow which indicate the changes to those calls.
 * The former calls involved are returned by <CODE>MultiCallMetaEvent.getOldCalls()</CODE>
 * and the resulting call is returned by <CODE>MultiCallMetaEvent.getNewCall()</CODE>.
 *<p>
 * This constant indicates a specific event passed via a <CODE>MultiCallMetaEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int MULTICALL_META_MERGE_STARTED = 200;

/**
 * The <CODE>MULTICALL_META_MERGE_ENDED</CODE> event indicates that
 * calls have merged, and that all state change events resulting from
 * this merge have been reported.
 *<p>
 * The former calls involved are returned by <CODE>MultiCallMetaEvent.getOldCalls()</CODE>
 * and the resulting call is returned by <CODE>MultiCallMetaEvent.getNewCall()</CODE>.
 *<p>
 * This constant indicates a specific event passed via a <CODE>MultiCallMetaEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int MULTICALL_META_MERGE_ENDED = 201;


/**
 * The <CODE>MULTICALL_META_TRANSFER_STARTED</CODE> event indicates that
 * a transfer is occurring, and events will follow which indicate the changes to the affected calls.
 * The former calls involved are returned by <CODE>MultiCallMetaEvent.getOldCalls()</CODE>
 * and the resulting call is returned by <CODE>MultiCallMetaEvent.getNewCall()</CODE>.
 *<p>
 * This constant indicates a specific event passed via a <CODE>MultiCallMetaEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int MULTICALL_META_TRANSFER_STARTED = 202;


/**
 * The <CODE>MULTICALL_META_TRANSFER_ENDED</CODE> event indicates that
 * a transfer has completed, and that all state change events resulting from
 * this transfer have been reported.
 *<p>
 * The former calls involved are returned by <CODE>MultiCallMetaEvent.getOldCalls()</CODE>
 * and the resulting call is returned by <CODE>MultiCallMetaEvent.getNewCall()</CODE>.
 *<p>
 * This constant indicates a specific event passed via a <CODE>MultiCallMetaEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int MULTICALL_META_TRANSFER_ENDED = 203;

}

