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

package javax.telephony.events;

/**
 * The parent of all JTAPI event interfaces.
 *
 * <H4>Introduction</H4>
 *
 * The <CODE>Ev</CODE> interface is the parent of all JTAPI event interfaces.
 * All JTAPI event interfaces extend this interface, either directly or
 * indirectly. Event interfaces within each JTAPI package are organized in a
 * hierarchical fashion. The architecture of the core package event hierarchy
 * is described later.
 * <p>
 * The JTAPI event system notifies applications when changes in various JTAPI
 * object occur. Each individual change in an object is represented by an
 * event sent to the appropriate observer. Because several changes may happen
 * to an object at once, events are delivered as a <EM>batch</EM>. A batch of
 * events represents a series of events and changes to the call model which
 * happened exactly at the same time. For this reason, events are delivered
 * to observers as arrays.
 *
 * <H4>Event IDs</H4>
 *
 * Each event carries a corresponding identification integer. The
 * <CODE>Ev.getID()</CODE> method returns this identification number for each
 * event. The actual event identification integer is defined in each of the
 * specific event interfaces. Each event interface must carry a unique id.
 *
 * <H4>Cause Codes</H4>
 *
 * Each events carries a <EM>cause</EM> or a reason why the event happened. The
 * <CODE>Ev.getCause()</CODE> method returns this cause value. The different
 * types of cause values are also defined in this interface.
 *
 * <H4>Core Package Event Hierarchy</H4>
 *
 * The core package defines a hierarchy of event interfaces. The base of this
 * hierarchy is the <CODE>Ev</CODE> interface. Directly extending this
 * interface are those events interfaces for each object which supports an
 * observer: <CODE>ProvEv</CODE>, <CODE>CallEv</CODE>, <CODE>AddrEv</CODE>,
 * and <CODE>TermEv</CODE>.
 * <p>
 * Since Connection and TerminalConnection events are reported via the
 * <CODE>CallObserver</CODE> interface, the <CODE>ConnEv</CODE> and
 * <CODE>TermConnEv</CODE> interfaces extends the <CODE>CallEv</CODE>
 * interface.
 * <p>
 * The following diagram illustrates the complete core package event structure.
 * <p>
 * <IMG SRC="doc-files/core-events.gif" ALIGN="center">
 *
 * <H4>Meta Codes</H4>
 *
 * The <CODE>Ev.getMetaCode()</CODE> method returns the <EM>meta code</EM> for
 * the event. Events are grouped together using meta codes to provide a
 * higher-level description of an update to the call model. Since events
 * represent singular changes in one particular object in the call model, it
 * may be difficult for the application to infer a higher-level interpretation
 * of several of these singular events. Meta codes exist on events to assist
 * the application in this regard.
 * <p>
 * Events which belong to the same higher-level action and contain the same
 * meta code are reported consecutively in an event batch sent to an observer.
 * In fact, multiple meta code grouping of events may exist in a single
 * event batch. In that case, the <CODE>Ev.isNewMetaEvent()</CODE> method is
 * used to indicate the beginning of a new meta code event grouping. This
 * method also indicates whether a meta code grouping exists across event
 * batch boundaries. That is, events belonging to the same meta code grouping
 * may be delivered in two contiguous event batches.
 * <p>
 * There are five types of meta codes which pertain to individual calls,
 * and two which pertain to a mutli-call action, and two miscellaneous meta
 * codes. The five meta codes which pertain to individual calls are:
 * <p>
 * <TABLE>
 * <TR>
 * <TD WIDTH="20%">Ev.META_CALL_STARTING</TD>
 * <TD WIDTH="80%">
 * Indicates that a new active call has been presented to the application,
 * either by an application creating a call and performing an action on it, or
 * by an incoming call to an object being observed by the application.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%">Ev.META_CALL_PROGRESS</TD>
 * <TD WIDTH="80%">
 * Indicates that the objects belonging to a call have changed state, with the
 * exception of Connections moving to <CODE>Connection.DISCONNECTED</CODE>. For
 * example, when a remote party answers a telephone call and the corresponding
 * Connection moves into the <CODE>Connection.CONNECTED</CODE> state, this is
 * the meta code associated with the resulting batch of events.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%">Ev.META_CALL_ADDING_PARTY</TD>
 * <TD WIDTH="80%">
 * Indicates that a party has been added to the call. A "party" corresponds
 * to a Connection being added. Note that if a TerminalConnection is added, it
 * carries a meta code of <CODE>Ev.META_CALL_PROGRESS</CODE>.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%">Ev.META_CALL_REMOVING_PARTY</TD>
 * <TD WIDTH="80%">
 * Indicates that a party (i.e. Connection) has been removed from the call by
 * moving into the <CODE>Connection.DISCONNECTED</CODE> state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%">Ev.META_CALL_ENDING</TD>
 * <TD WIDTH="80%">
 * Indicates that an entire telephone call has ended, which implies the call
 * has moved into the <CODE>Call.INVALID</CODE> state and all of its
 * Connections have moved into the <CODE>Connection.DISCONNECTED</CODE> state.
 * </TD>
 * </TR>
 * </TABLE>
 * <BR>
 * The two meta codes pertaining to a mutli-call actions are as follows:
 *
 * <TABLE>
 * <TR>
 * <TD WIDTH="20%">Ev.META_CALL_MERGING</TD>
 * <TD WIDTH="80%"> 
 * Indicates that a party has moved from one call to another as part of the
 * two calls merging. A common example is when two telephone calls are
 * conferenced.
 * </TD>
 * <TR>
 *
 * <TR>
 * <TD WIDTH="20%">Ev.META_CALL_TRANSFERRING</TD>
 * <TD WIDTH="80%">
 * Indicates that a party has moved from one call to another as part of one
 * call being transferred to another. The differs from
 * <CODE>Ev.META_CALL_MERGING</CODE> because a common party leaves both calls.
 * </TD>
 * </TR>
 * </TABLE>
 *
 * <BR>
 * The two miscellaneous meta codes are as follows:
 *
 * <TABLE>
 * <TR>
 * <TD WIDTH="20%">Ev.META_SNAPSHOT</TD>
 * <TD WIDTH="80%">
 * Indicates that the sequence of events are part of a "snapshot" given to
 * the application to bring it up-to-date with the current state of the call
 * model.
 * </TD>
 * <TR>
 *
 * <TR>
 * <TD WIDTH="20%">Ev.META_UNKNOWN</TD>
 * <TD WIDTH="80%">
 * Indicates that the meta code is unknown for the event.
 * </TD>
 * </TR>
 * </TABLE>
 */

public interface Ev {

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
   * Meta code description for the initiation or starting of a call. This
   * implies that the call is a new call and in the active state with at least
   * one Connection added to it.
   */
  public static final int META_CALL_STARTING = 0x80;


  /**
   * Meta code description for the progress of a call. This indicates an
   * update in state of certain objects in the call, or the addition of
   * TerminalConnections (but not Connections).
   */
  public static final int META_CALL_PROGRESS = 0x81;


  /**
   * Meta code description for the addition of a party to call. This includes
   * adding a connection to the call.
   */
  public static final int META_CALL_ADDITIONAL_PARTY = 0x82;


  /**
   * Meta code description for a party leaving the call. This includes
   * exactly one Connection moving to the <CODE>Connection.DISCONNECTED</CODE>
   * state.
   */
  public static final int META_CALL_REMOVING_PARTY = 0x83;


  /**
   * Meta code description for the entire call ending. This includes
   * the call going to <CODE>Call.INVALID</CODE>, all of the Connections
   * moving to the <CODE>Connection.DISCONNECTED</CODE> state.
   */
  public static final int META_CALL_ENDING = 0x84;


  /**
   * Meta code description for an action of merging two calls. This
   * involves the removal of one party from one call and the addition
   * of the same party to another call.
   */
  public static final int META_CALL_MERGING = 0x85;


  /**
   * Meta code description for an action of transferring one call to another.
   * This involves the removal of parties from one call and the addition
   * to another call, and the common party dropping off completely.
   */
  public static final int META_CALL_TRANSFERRING = 0x86;
 

  /**
   * Meta code description for a snapshot of events.
   */
  public static final int META_SNAPSHOT = 0x87;


  /**
   * Meta code is unknown.
   */
  public static final int META_UNKNOWN = 0x88;


  /**
   * Returns the cause associated with this event. Every event has a cause.
   * The various cause values are defined as public static final variables
   * in this interface.
   * <p>
   * @return The cause of the event.
   */
  public int getCause();


  /**
   * Returns the meta code associated with this event. The meta code provides
   * a higher-level description of the event.
   * <p>
   * @return The meta code for this event.
   */
  public int getMetaCode();


  /**
   * Returns true when this event is the start of a meta code group. This
   * method is used to distinguish two contiguous groups of events bearing
   * the same meta code.
   * <p>
   * @return True if this event represents a new meta code grouping, false
   * otherwise.
   */
  public boolean isNewMetaEvent();


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
   * Returns the object that is being observed.
   * <p>
   * <STRONG>Note:</STRONG>Implementation need no longer supply this
   * information. The <CODE>CallObsevationEndedEv.getObservedObject()</CODE>
   * method has been added which returns related information. This method may
   * return null in JTAPI v1.2 and later.
   * <p>
   * @deprecated Since JTAPI v1.2 This interface no longer needs to supply this
   * information and may return null.
   * @return The object that is being observed.
   */
   public Object getObserved();
}
