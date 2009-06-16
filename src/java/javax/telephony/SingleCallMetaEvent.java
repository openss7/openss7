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
 * The <CODE>SingleCallMetaEvent</CODE> interface is the base interface for
 * all single-call Call Meta events.
 * All single-call MetaEvent's must extend this interface.
 * Events which extend this interface are reported via the
 * <CODE>CallListener</CODE> interface.
 * <p>
 * An individual <CODE>SingleCallMetaEvent</CODE>
 * conveys, directly and with necessary details,
 * what an application needs to know to respond to
 * a higher-level call event that only effects a single call.
 * The specific SingleCallMetaEvent event is indicated by
 * the <CODE>Event.getID()</CODE>
 * value returned by the event.
 * <p>
 * Since all these Meta events relate to a single JTAPI Call entity,
 * this interface provides the method getCall to grant the application access
 * to the affected Call.
 * <p>
 * The core package defines events which are reported when
 * high-level actions occur. The event IDs
 * (as returned by <CODE>Event.getID()</CODE>) are:
 * <CODE>SINGLECALL_META_PROGRESS_STARTED</CODE>,
 * <CODE>SINGLECALL_META_PROGRESS_ENDED</CODE>,
 * <CODE>MULTICALL_META_TRANSFER_STARTED</CODE> and
 * <CODE>MULTICALL_META_TRANSFER_ENDED</CODE>.
 * <p>
 * @see javax.telephony.CallListener
 * @see javax.telephony.Call
 * @version 04/06/99 1.2
 */
public interface SingleCallMetaEvent extends MetaEvent {

  /**
   * Returns the Call object associated with the higher-level operation
   * reported by this SingleCallMetaEvent event.
   * <p>
   * @return The result Call associated with this event.
   */
  public Call getCall();

/**
 * Event id codes returned for this interface
 */

/**
 * The <CODE>SINGLECALL_META_PROGRESS_STARTED</CODE> event indicates that
 * the current call in the telephony platform has changed state,
 * and events will follow which indicate the changes to this call.
 *<p>
 * This constant indicates a specific event passed via a <CODE>SingleCallMetaEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
  public static final int SINGLECALL_META_PROGRESS_STARTED = 210;

/**
 * The <CODE>SINGLECALL_META_PROGRESS_ENDED</CODE> event indicates that
 * the current call in the telephony platform has changed state,
 * and all the events that were associated with that change have now been reported.
 *<p>
 * This constant indicates a specific event passed via a <CODE>SingleCallMetaEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
  public static final int SINGLECALL_META_PROGRESS_ENDED = 211;

/**
 * The <CODE>SINGLECALL_META_SNAPSHOT_STARTED</CODE> event indicates that
 * the JTAPI implementation is reporting to the application the current state of the call
 * on the associated telephony platform, by reporting a set of simulated state changes
 * that in effect construct the current state of the call.
 * The events which follow convey that current state.
 *<p>
 * This constant indicates a specific event passed via a <CODE>SingleCallMetaEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
  public static final int SINGLECALL_META_SNAPSHOT_STARTED = 212;

/**
 * The <CODE>SINGLECALL_META_SNAPSHOT_ENDED</CODE> event indicates that
 * the JTAPI implementation has finished reporting a set of simulated state changes
 * that in effect construct the current state of the call.
 *<p>
 * This constant indicates a specific event passed via a <CODE>SingleCallMetaEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
  public static final int SINGLECALL_META_SNAPSHOT_ENDED = 213;

}

