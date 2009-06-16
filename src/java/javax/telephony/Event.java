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

/**
 * The <CODE>Event</CODE> interface is the parent of all JTAPI event interfaces.
 *
 * <H4>Introduction</H4>
 *
 * All JTAPI event interfaces extend this interface, either directly or
 * indirectly. Event interfaces within each JTAPI package are organized in a
 * hierarchical fashion. The architecture of the core package event hierarchy
 * is described later.
 * <p>
 * The JTAPI event system notifies applications when changes in various JTAPI
 * object occur. Each individual change in an object is represented by an
 * event sent to the appropriate Listener.
 *<p>
 * In the spirit of the
 * <A HREF="http://java.sun.com/products/jdk/1.2/docs/guide/awt/designspec/events.html">
 * JDK 1.1 Event model changes
 * </A>,
 * JTAPI now has Listeners instead of Observers.
 * In addition to the JDK 1.1 change in terminology,
 * JTAPI event handling has been modified to make programming easier,
 * by multiplexing specific events out to specific methods and
 * by reducing the number of event objects involved.
 * In addition, the memory footprint of the API has been lessened by
 * reducing the number of interfaces defined, in order to accomodate smaller platforms.
 *<p>
 * JTAPI events will be delivered to a Listener one at a time, unlike
 * the Observer model, where they are delivered in an array.
 *<p>
 * With the old observer event model,
 * large scale activities such as 3rd party call transfer and conference,
 * are awkward to detect and process.
 * <p>
 * With the new Listener model and meta events (see <CODE>MetaEvent</CODE>),
 * an application has a listener interface with methods and events which convey,
 * directly and with necessary details,
 * what applications need to know to respond to higher-level events.
 * <p>
 * <b>Note:</b>
 * Classes that <i>implement</i> this interface shall follow the 
 * EventListener design pattern by extending java.util.EventObject.
 * 
 * <H4>Event Identification</H4>
 *
 *<p>
 * Event objects correspond to the JTAPI object which is undergoing
 * a state change; the specific state change is conveyed to the application
 * in two ways.
 * <p>
 * First, the provider reports the event to a particular method
 * in a particular Listener interface to a listening object; generally
 * the method corresponds to a particular state change.
 * <p>
 * Second, the event that is presented to the method has an identification
 * integer which indicates the specific state change.
 * <p>
 * The <CODE>Event.getID()</CODE> method returns this identification number for each
 * event.
 * The actual event identification integer values that may be conveyed
 * by the individual event object are defined in each of the
 * specific event interfaces.
 *<p>
 *
 * <H4>Cause Codes</H4>
 *
 * Each events carries a <EM>cause</EM> or a reason why the event happened. The
 * <CODE>Event.getCause()</CODE> method returns this cause value. The different
 * types of cause values are also defined in this interface.
 *
 * <H4>Core Package Event Hierarchy</H4>
 *
 * The core package defines a hierarchy of event interfaces. The base of this
 * hierarchy is the <CODE>Event</CODE> interface. Directly extending this
 * interface are those events interfaces for each object which supports an
 * Listener: <CODE>ProviderEvent</CODE>, <CODE>CallEvent</CODE>, <CODE>AddressEvent</CODE>,
 * and <CODE>TerminalEvent</CODE>.
 * <p>
 * Since Connection and TerminalConnection events are reported via the
 * <CODE>CallListener</CODE> interface, the <CODE>ConnectionEvent</CODE> and
 * <CODE>TerminalConnectionEvent</CODE> interfaces extends the <CODE>CallEvent</CODE>
 * interface.
 * <p>
 * The following diagram illustrates the complete core package event structure.
 * <p>
 * <IMG SRC="doc-files/core-events.gif" ALIGN="center">
 *
 * <H4>JTAPI 1.2, Observer Model Meta Events</H4>
 *
 * With JTAPI 1.2 & the observer model, "meta information" mechanisms (see Event.java) -
 * event batches,
 * meta codes, and
 * the Event.isNewMetaEvent() method -
 * were used
 * <TABLE>
 * <TR> <TD> A. to indicate what, and which, higher-level action
 * an event belongs to </TD> </TR>
 * <TR> <TD> B. to identify which events are associated with
 * a particular update to the call model (implying
 * synchronization points at which queries are reasonable) </TD> </TR>
 * <TR> <TD> C. to convey, implicitly, multi-call event information. </TD> </TR>
 * </TABLE>
 *<p>
 * Determining what and which higher-level action is happening with the observer mechanism
 * is indirect - it must
 * be concluded by looking at meta codes and possibly multiple events. Determining
 * when the JTAPI events imply a consistent call state is conveyed by a change between events
 * in the return value of the method "Event.isNewMetaEvent". Conveying
 * information about multiple calls is done implicitly, across multiple events.
 *<p>
 * For applications to respond to higher-level actions, and to understand
 * the scope of those actions, in the current JTAPI API, they must scan multiple
 * events, inspect meta codes, inspect "isNewMetaEvent" "flag" values and,
 * ultimately, guess that events are related.
 *<p>
 * Applications would be much simpler if they were given
 * <TABLE>
 * <TR> <TD> A. direct notification of high-level actions </TD> </TR>
 * <TR> <TD>  B. explicit descriptions of the scope of these actions
 * (calls involved and events included) </TD> </TR>
 * </TABLE>
 *
 * <H4>JTAPI 1.3, Listener Model Meta Events</H4>
 *
 *<p>
 * This semantic event proposal addresses these needs. It provides an interface
 * with methods and events which convey, directly and with necessary details,
 * what applications need to know to respond to higher-level events.
 * <PRE>
 * MetaEvent
 *   MultiCallMetaEvent extends MetaEvent
 *     constants
 *       MultiCallMetaMergeStart
 *       MultiCallMetaMergeEnd
 *       MultiCallMetaTransferStart
 *       MultiCallMetaTransferEnd
 *     methods
 *       Call[] getOldCalls()
 *       Call   getNewCall()
 * </PRE>
 *<p>
 * As for Listening for these events, we would add to CallListener
 * <PRE>
 * CallListener
 *   methods
 *     MultiCallMetaMergeStarted ( MetaEvent ev )
 *     MultiCallMetaMergeEnded ( MetaEvent ev )
 *     MultiCallMetaTransferStarted ( MetaEvent ev )
 *     MultiCallMetaTransferEnded ( MetaEvent ev )
 *</PRE>
 *<p>
 * and, to be able to map an event back to a meta-event, we'd add
 *<PRE>
 * CallEvent
 *   method
 *     MetaEvent getMetaEvent() 
 *</PRE>
 *<p>
 * which would return null if there were no meta-event associated with this event.
 *<p>
 * In addition to these interfaces and methods, the following operational
 * principles would apply (currently just one):
 *<PRE>
 *    meta events are reported before any corresponding normal events.
 *</PRE>
 *<p>
 * These changes are reflected in the current API.
 * @version 04/06/99 1.12
 *
 */
public interface Event {

  /**
   * Cause code indicating normal operation
   */
  public static final int CAUSE_NORMAL = 100;


  /**
   * Cause code indicating the cause was unknown
   */
  public static final int CAUSE_UNKNOWN = 101;


  /**
   * Cause code indicating the user has terminated call without going on-hook.
   */
  public static final int CAUSE_CALL_CANCELLED = 102;


  /**
   * Cause code indicating the destination is not available.
   */
  public static final int CAUSE_DEST_NOT_OBTAINABLE = 103;


  /**
   * Cause code indicating that a call has encountered an incompatible
   * destination.
   */
  public static final int CAUSE_INCOMPATIBLE_DESTINATION = 104;


  /**
   * Cause code indicating that a call encountered inter-digit timeout while
   * dialing.
   */
  public static final int CAUSE_LOCKOUT = 105;


  /**
   * Cause code indicating that a new call.
   */
  public static final int CAUSE_NEW_CALL = 106;


  /**
   * Cause code indicating resources were not available.
   */
  public static final int CAUSE_RESOURCES_NOT_AVAILABLE = 107;


  /**
   * Cause code indicating call encountered network congestion.
   */
  public static final int CAUSE_NETWORK_CONGESTION = 108;


  /**
   * Cause code indicating call could not reach a destination network.
   */
  public static final int CAUSE_NETWORK_NOT_OBTAINABLE = 109;


  /**
   * Cause code indicating that the event is part of a snapshot of the current
   * state of the call.
   */
  public static final int CAUSE_SNAPSHOT = 110;


  /**
   * Returns the cause associated with this event. Every event has a cause.
   * The various cause values are defined as public static final variables
   * in this interface.
   * <p>
   * @return The cause of the event.
   */
  public int getCause();


  /**
   * Returns the id of event. Every event has an id. The defined id of each
   * event matches the object type of each event. The defined id allows
   * applications to switch on event id rather than having to use multiple "if
   * instanceof" statements.
   * <p>
   * @return The id of the event.
   */
  public int getID();


  /**
   * Returns the event source of the event
   * (see <CODE>java.util.EventObject</CODE>).
   * <p>
   * @return The object sending the event.
   */
  public Object getSource();


  /**
   * Returns the MetaEvent associated with this event, or null.
   * <p>
   * @return The MetaEvent corresponding to this event, or null.
   */
   public MetaEvent getMetaEvent();

}
