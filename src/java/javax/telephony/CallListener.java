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
import  javax.telephony.CallEvent;

/**
 * The <CODE>CallListener</CODE> interface reports all changes to the Call object.
 * <H4>Introduction</H4>
 * <H5> Call Events and Call MetaEvents</H5>
 * JTAPI generally provides a common model
 * and standard interface to telephony applications,
 * to enable the application to portably represent and control calls.
 * Again generally there is a telephony platform on the other side of the API,
 * which internally represents the objects and operations differently -
 * often at a different level of granularity.
 * <p>
 * Since some individual changes to a call
 * as represented by the telephony platform result
 * in many changes in state in the corresponding JTAPI objects,
 * the telephony platform's JTAPI implementation needs a way
 * to mark a group of JTAPI events,
 * calling them related,
 * all originating from a single change in the telephony platform
 * and its calls.
 * <p>
 * Applications may care because they may draw incorrect conclusions about
 * the state of Calls after having processed only a fraction of a set of events
 * generated in response to a single provider change of state.
 * <p>
 * In JTAPI 1.2, in order to indicate that a collection of JTAPI events all were in response
 * to a single common telephony platform event,
 * the convention was to deliver JTAPI events in <i>batches.</i>
 * JTAPI 1.2 events delivered in a single batch were produced
 * in response to a single
 * telephony platform change of state.
 * <p>
 * JTAPI 1.3 introduces
 * the Java 1.2 Event Model pattern of <i>Event Listeners,</i>
 * which expect to receive events one at a time;
 * hence batches are no longer used.
 * Since the related events do not arrive as a group,
 * JTAPI 1.3 provides new events, called MetaEvents,
 * which show when such a group begins and ends.
 * <p>
 * In addition to helping the application know how the events are grouped,
 * MetaEvents also convey information about larger granularity events.
 * Some specifically carry information about call merging and transferring
 * (see MultiCallMetaEvent).
 * 
 *   <H5>Registering as a Listener</H5>
 * 
 * Therefore the CallListener interface conveys JTAPI Events and MetaEvents
 * which indicate changes to the Call and related Calls.
 * Applications which need to know about changes to a Call,
 * may register as a listener to that Call;
 * in order to do so the listener must implement this (CallListener) interface.
 * <p>
 * In order to register as a listener for events reported for all calls
 * involving a specific Address, as soon as the Calls are created,
 * an application may implement the CallListener interface and
 * then register with the Address,
 * via the <CODE>Address.addCallListener</CODE> method.
 * 
 *  <H5>Connection Event and TerminalConnection Event listeners</H5>
 *
 * The above discussion describes how an application may arrange
 * to be notified of Call related state changes.
 * An application may be notified, in addition, of Connection and TerminalConnection events,
 * by registering as a listener for extensions of this interface -
 * that is, as a ConnectionListener or as a TerminalConnectionListener.
 * <p>
 * In addition to the CallEvents and MetaEvents described above,
 * listeners to these interfaces receive Connection events (ConnectionListener) or
 * Connection and TerminalConnection events (TerminalConnectionListener).
 * <p> 
 * These other interfaces extend the CallListener interface, to guarantee a Listener
 * a synchronous stream of Meta, Call, Connection and TerminalConnection events
 * in the expected order. To have a seperate Connection Listener might leave
 * an application open to race conditions, where events coming on seperate listener threads
 * could come in an unexpected order.
 * <p>
 * The cost to an application which cares only about Connection or TerminalConnection events
 * is small, in that relatively few other events are reported on those interfaces;
 * in addition, the programming burden is mitigated by convenient adapters
 * (<STRONG> naturally we need to provide adapters here! like 
 * <CODE>CallListenerAdapter</CODE>).</STRONG>
 * <p>
 * State changes are reported as events
 * to the Listener method corresponding
 * to the type of event.
 *
 * <H4>Adding a Listener to a Call</H4>
 *
 * Applications must
 * instantiate an object which implements the appropriate listener interface
 * and then register the Call Listener using one of several mechanisms described below to
 * receive all future events associated with the Call and (as requested) its Connections and
 * TerminalConnections.
 *
 * Applications may add a listener to a Call via one of several mechanisms.
 * They may directly add a listener via the <CODE>Call.addCallListener</CODE>
 * method. Applications may also add listeners to Calls indirectly via the
 * <CODE>Address.addCallListener</CODE> and
 * <CODE>Terminal.addCallListener</CODE> methods. These methods add the given
 * listener to the Call when the Call comes to the Address or Terminal. See
 * the specifications for Call, Address, and Terminal for more information.
 *
 *
 * <H4>Call State Changes</H4>
 * In the core package, an event is delivered whenever the Call changes state.
 * The methods which are called with an event to indicate when
 * these state changes occur, are: 
 * <CODE>Call.callActiveEvent</CODE> and <CODE>Call.callInvalidEvent</CODE>.
 *
 *
 * <H4>Meta Events</H4>
 *
 * Meta events (<CODE>MetaEvent</CODE>) pertaining to this Call are reported on this
 * interface. In the core package, a meta event is delivered to this interface
 * to provide higher-level context for a sequence of JTAPI "regular" events.
 * The Meta events indicate appropriate times for an application to query the JTAPI model,
 * and convey details about higher-level activities and operations
 * (see <CODE>MetaEvent</CODE> for more information).
 *
 * <H4>Call Listener Ending</H4>
 *
 * <p>
 * At various times, the underlying implementation may not be able to provide
 * events noting Call state changes.
 * In these instances, the CallListener is sent an event via the
 * <CODE>callEventTransmissionEnded</CODE> method.
 * This indicates that the application will not
 * receive further events associated with the Call object. This listener is
 * no longer reported via the <CODE>Call.getCallListeners</CODE> method.
 *
 * @see javax.telephony.Call
 * @see javax.telephony.CallEvent
 * @see javax.telephony.ConnectionEvent
 * @see javax.telephony.TerminalConnectionEvent
 * @see javax.telephony.MetaEvent
 * @see javax.telephony.MultiCallMetaEvent
 * @see javax.telephony.SingleCallMetaEvent
 * @version 05/19/99 1.10
 */

public interface CallListener extends java.util.EventListener  {

  /**
   * Interface reporting all events associated with the Call object.
   * <p>
   * The methods in this interface indicate either state changes in objects
   * (either a CallEvent, ConnectionEvent or TerminalConnectionEvent indicating 
   * a state change in the
   * corresponding object), or, with the meta event methods, indicate that
   * a high-level operation is occurring.
   * <p>
   * Each method is used to convey a particular state change or meta operation;
   * hence an application may implement the methods corresponding to the events
   * they wish to have reported, and may stub out those they wish to ignore.
   */

  /*
   * Call event methods.
   */

  /**
   * The <CODE>callActive</CODE> method is called to indicate that
   * the state of the
   * Call object has changed to <CODE>Call.ACTIVE</CODE>.
   * <p>
   * @param event A CallEvent with eventID "Call.CALL_ACTIVE".
   */
	public void callActive(CallEvent event);
	
	
	/**
	 * The <CODE>callInvalid</CODE> method is called to indicate that
	 * the state of the
	 * Call object has changed to <CODE>Call.INVALID</CODE>.
	 * <p>
	 * @param event A CallEvent with eventID "Call.CALL_INVALID".
	 */
	public void callInvalid(CallEvent event);
	
	
	/**
	 * The <CODE>callEventTransmissionEnded</CODE> method is called to indicate that
	 * the the application will no longer receive CallEvent events on the instance
	 * of the <CODE>CallListener</CODE>.
	 * <p>
	 * @param event A CallEvent with eventID "Call.CALL_EVENT_TRANSMISSION_ENDED".
	 */
	public void callEventTransmissionEnded(CallEvent event);


	/*
	 * MetaEvent event methods.
	 */

	
	/**
	 * This method indicates that
	 * a state change has occurred for this Call entity, and that related events will now
	 * be reported. See <CODE>SingleCallMetaEvent.SINGLECALL_META_PROGRESS_STARTED</CODE> 
	 * for details.
	 * <p>
	 * @param event A MetaEvent with eventID 
	 * "SingleCallMetaEvent.SINGLECALL_META_PROGRESS_STARTED".
	 */
	public void singleCallMetaProgressStarted(MetaEvent event);
	
	/**
	 * This method indicates that
	 * all events reported to convey the state change have been reported.
	 * See <CODE>SingleCallMetaEvent.SINGLECALL_META_PROGRESS_ENDED</CODE> 
	 * for details.
	 * <p>
	 * @param event A MetaEvent with eventID "SingleCallMetaEvent.SINGLECALL_META_PROGRESS_ENDED".
	 */
	public void singleCallMetaProgressEnded(MetaEvent event);
	
	/**
	 * This method indicates that
	 * JTAPI events will be reported to help the application reconstruct
	 * the current state of this call.
	 * See <CODE>SingleCallMetaEvent.SINGLECALL_META_SNAPSHOT_STARTED</CODE> 
	 * for details.
	 * <p>
	 * @param event A MetaEvent with eventID 
	 * "SingleCallMetaEvent.SINGLECALL_META_SNAPSHOT_STARTED".
	 */
	public void singleCallMetaSnapshotStarted(MetaEvent event);
	
	/**
	 * This method indicates that
	 * the last event necessary to reconstruct the current state of this call has been reported.
	 * See <CODE>SingleCallMetaEvent.SINGLECALL_META_SNAPSHOT_ENDED</CODE> 
	 * for details.
	 * <p>
	 * @param event A MetaEvent with eventID "SingleCallMetaEvent.SINGLECALL_META_SNAPSHOT_ENDED".
	 */
	public void singleCallMetaSnapshotEnded(MetaEvent event);
	
	/**
	 * This method indicates that
	 * a merge operation is starting, and that related events will now
	 * be reported. See <CODE>MultiCallMetaEvent.MULTICALL_META_MERGE_STARTED</CODE> 
	 * for details.
	 * <p>
	 * @param event A MetaEvent with eventID 
	 * "MultiCallMetaEvent.MULTICALL_META_MERGE_STARTED".
	 */
	public void multiCallMetaMergeStarted(MetaEvent event);
	
	/**
	 * This method indicates that
	 * a merge operation has ended, and that no further related events will
	 * be reported. See <CODE>MultiCallMetaEvent.MULTICALL_META_MERGE_ENDED</CODE> 
	 * for details.
	 * <p>
	 * @param event A MetaEvent with eventID "MultiCallMetaEvent.MULTICALL_META_MERGE_ENDED".
	 */
	public void multiCallMetaMergeEnded(MetaEvent event);
	
	/**
	 * This method indicates that
	 * a transfer operation is starting, and that related events will now
	 * be reported. See <CODE>MultiCallMetaEvent.MULTICALL_META_TRANSFER_STARTED</CODE> 
	 * for details.
	 * <p>
	 * @param event A MetaEvent with eventID 
	 * "MultiCallMetaEvent.MULTICALL_META_TRANSFER_STARTED".
	 */
	public void multiCallMetaTransferStarted(MetaEvent event);
	
	/**
	 * This method indicates that
	 * a merge operation has ended, and that no further related events will
	 * be reported. See <CODE>MultiCallMetaEvent.MULTICALL_META_TRANSFER_ENDED</CODE>
	 *  for details.
	 * <p>
	 * @param event A MetaEvent with eventID
	 *  "MultiCallMetaEvent.MULTICALL_META_TRANSFER_ENDED".
	 */
	public void multiCallMetaTransferEnded(MetaEvent event);
}
