/*
 * Copyright (c) 1997 Sun Microsystems, Inc. All Rights Reserved.
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
import  javax.telephony.capabilities.CallCapabilities;

/**
 * A <CODE>Call</CODE> object models a telephone call.  
 *
 * <H4>Introduction</H4>
 *
 * A Call can have zero or more {@link Connection}s.
 * A two-party call has two Connections, and a conference call
 * has three or  more Connections. Each Connection models the relationship
 * between a Call and an Address, where an Address identifies a particular
 * party or set of parties on a Call.
 *
 * <H4>Creating Call Objects</H4>
 *
 * Applications create instances of a Call object with the 
 * <CODE>Provider.createCall()</CODE> method, which returns a Call object that
 * has zero Connections and is in the <CODE>Call.IDLE state</CODE>.  The Call
 * maintains a  reference to its Provider for the life of that Call object.
 * This Provider object instance does not change throughout the lifetime of
 * the Call object. The Provider associated with a Call is obtained via the
 * <CODE>Call.getProvider()</CODE> method.
 *
 * <H4>Call States</H4>
 *
 * A Call has a <EM>state</EM> which is obtained via the
 * <CODE>Call.getState()</CODE> method. This state describes the current
 * progress of a telephone call, where is it in its life cycle, and how many
 * Connections exist on the Call. The Call state may be one of three values:
 * <CODE>Call.IDLE</CODE>, <CODE>Call.ACTIVE</CODE>, or
 * <CODE>Call.INVALID</CODE>. The following is a description of each state:
 *
 * <TABLE>
 * <TR>
 * <TD WIDTH="20%"><CODE>Call.IDLE</CODE></TD>
 * <TD WIDTH="80%">
 * This is the initial state for all Calls. In this state, the Call has zero
 * Connections, that is <CODE>Call.getConnections()</CODE> <EM>must</EM>
 * return null.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>Call.ACTIVE</CODE></TD>
 * <TD WIDTH="80%">
 * A Call with some current ongoing activity is in this state. Calls with one
 * or more associated Connections must be in this state. If a Call is in this
 * state, the <CODE>Call.getConnections()</CODE> method must return an array
 * of size at least one.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>Call.INVALID</CODE></TD>
 * <TD WIDTH="80%">
 * This is the final state for all Calls. Call objects which lose all of their
 * Connections objects (via a transition of the Connection object into the
 * <CODE>Connection.DISCONNECTED</CODE> state) moves into this state. Calls
 * in this state have zero Connections and these Call objects may not be used
 * for any future action. In this state, the <CODE>Call.getConnections()</CODE>
 * <EM>must</EM> return null.
 * </TD>
 * </TR>
 * </TABLE>
 *
 * <H4>Call State Transitions</H4>
 *
 * The possible Call state transitions are given in the diagram below:
 * <p>
 * <IMG SRC="doc-files/core-callstates.gif">
 *
 * <H4>Calls and Connections</H4>
 *
 * A Call maintain a list of the Connections on that Call. Applications obtain
 * an array of Connections associated with the Call via the
 * <CODE>Call.getConnections()</CODE> method. A Call retains a reference to a
 * Connection only if it is not in the <CODE>Connection.DISCONNECTED</CODE>
 * state. Therefore, if a Call has a reference to a Connection, then that
 * Connection must not be in the <CODE>Connection.DISCONNECTED</CODE> state.
 * When a Connection moves into the <CODE>Connection.DISCONNECTED</CODE> state
 * (e.g. when a party hangs up), the Call loses its reference to that
 * Connection which is no longer reported via the
 * <CODE>Call.getConnections()</CODE> method.
 *
 * <H4>The <CODE>Call.connect()</CODE> method</H4>
 *
 * The primary method on this interface is the <CODE>Call.connect()</CODE>
 * method. Applications use this method to place a telephone call from an
 * originating endpoint to a destination address string. The result of this
 * method on the call model is to create an originating and destination
 * Connection and move the Call into the <CODE>Call.ACTIVE</CODE> state. As the
 * new telephone call progresses during its lifetime, the state of various
 * objects associated with the Call may change and new objects may be created
 * and associated with the Call. See the specification of the
 * <CODE>Call.connect()</CODE> method below for more details.
 *
 * <H4>Listeners and Observers</H4>
 *
 * In JTAPI 1.2 and earlier versions, the way a JTAPI application
 * arranged to receive events about telephony activity was to
 * register an application object as an <CODE>Observer</CODE> of one or more
 * JTAPI objects.
 *<P> 
 * Starting with JTAPI 1.3, the primary way to arrange for a
 * provider to report events is to use Listeners, following the
 * pattern established in Java release 1.1
 * (see <CODE>Event</CODE> for more details).
 * The Observer model is supported but its use is deprecated with
 * this release.
 * 
 * <H4>Listeners and Events</H4>
 *
 * The <CODE>CallListener</CODE> interface reports all events pertaining to
 * the Call object. Events delivered to this interface must extend the
 * <CODE>CallEvent</CODE> interface. Applications add listeners to a Call object
 * via the <CODE>Call.addCallListener()</CODE> method.
 * <p>
 * Connection-related and TerminalConnection-related events are also reported
 * via the <CODE>CallListener</CODE> interface. These events include the
 * creation of these objects and their state changes. Events which are reported
 * via the <CODE>CallListener</CODE> interface pertaining to Connections and
 * TerminalConnections extend the <CODE>ConnectionEvent</CODE> interface and the
 * <CODE>TerminalConnectionEvent</CODE> interface, respectively.
 * <p>
 * An event is delivered to the application whenever the state of the Call
 * changes. The event interfaces corresponding to Call state changes are:
 * <CODE>CallActiveEventID</CODE> and <CODE>CallInvalidEventID</CODE>.
 *
 * <H5>When Call Event Transmission Ends</H5>
 *
 * At times it may become impossible for the implementation to
 * report events to an application.  In this case,
 * a CALL_EVENT_TRANSMISSION_ENDED is delivered to
 * an object registered as a <CODE>CallListener</CODE>
 * (or an extension of that interface).
 * <p>
 * This is the final event receives by the Listener, and it is no
 * longer reported via the <CODE>Call.getCallListeners()</CODE> method.
 *
 * <H5>Event Granularity</H5>
 *
 * An application may control the granularity of events which are
 * reported.  Registering for the highest level interface
 * (<CODE>CallListener</CODE>) will direct the implementation to
 * send only <CODE>Call</CODE>-related events (including
 * <CODE>MetaEvents</CODE>).  To register as a Listener at a lower
 * level interfaces (<CODE>ConnectionListener</CODE> or
 * <CODE>TerminalConnectionListener</CODE>) directs the
 * implementation to provide successively more detailed events.
 *
 * <H5>Registering Call Listeners via Address and Terminal</H5>
 *
 * Applications may receive events about a Call
 * by adding an Listener via the Address or
 * Terminal objects using the <CODE>Address.addCallListener()</CODE> and
 * <CODE>Terminal.addCallListener()</CODE> (and related) methods.
 * These methods provide the
 * ability for an application to receive Call-related events when a Call
 * contains a particular Address or Terminal.
 * In particular, methods exist to add a <CODE>CallListener</CODE>,
 * <CODE>ConnectionListener</CODE> and
 * <CODE>TerminalConnectionListener</CODE> via Address and
 * Terminal.
 * See the specifications for
 * Address and Terminal for more details.
 * <p>
 * @see javax.telephony.CallListener
 * @see javax.telephony.ConnectionListener
 * @see javax.telephony.TerminalConnectionListener
 * @see javax.telephony.Connection
 * @see javax.telephony.Address
 * @see javax.telephony.Terminal
 * @see javax.telephony.TerminalConnection
 * @see javax.telephony.CallEvent
 * @version 10/22/99 1.62
 */

public interface Call {

  /**
   * The <CODE>Call&#46IDLE</CODE> state indicates the Call has zero Connections.
   * It is the initial state of all Call objects.
   */
  public static final int IDLE = 0x20;


  /**
   * The <CODE>Call&#46ACTIVE</CODE> state indicates the Call has one or more 
   * Connections, none of which are in the <CODE>Connection.DISCONNECTED</CODE>
   * state.
   */
  public static final int ACTIVE = 0x21;


  /**
   * The <CODE>Call&#46INVALID</CODE> state indicates the Call has lost all of 
   * its connections - that is, all of its associated Connection objects have moved
   * into the <CODE>Connection.DISCONNECTED</CODE> state and are no longer
   * associated with the Call. A Call in this state cannot be used for future
   * actions.
   */
  public static final int INVALID = 0x22;


  /**
   * Returns an array of Connections associated with this call. Note that none
   * of the Connections returned will be in the
   * <CODE>Connection.DISCONNECTED</CODE> state. Also, if the Call is in the
   * <CODE>Call.IDLE</CODE> state or the <CODE>Call.INVALID</CODE> state, this
   * method returns null. Otherwise, it returns one or more Connection objects.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let Connection[] conn = Call.getConnections()
   * <LI>if this.getState() == Call.IDLE then conn = null
   * <LI>if this.getState() == Call.INVALID then conn = null
   * <LI>if this.getState() == Call.ACTIVE then conn.length >= 1
   * <LI>For all i, conn[i].getState() != Connection.DISCONNECTED
   * </OL>
   * @return An array of the Connections associated with the call.
   */
  public Connection[] getConnections();


  /**
   * Returns the Provider associated with the Call. This Provider reference
   * remains valid throughout the lifetime of the Call object, despite the
   * state of the Call object. This Provider reference does not change once the
   * Call object has been created.
   * <p>
   * @return The Provider associated with the call.
   */
  public Provider getProvider();


  /**
   * Returns the current state of the telephone call. The state will be
   * either <CODE>Call.IDLE</CODE>, <CODE>Call.ACTIVE</CODE>, or
   * <CODE>Call.INVALID</CODE>.
   * <p>
   * @return The current state of the call.
   */
  public int getState();


  /**
   * Places a telephone call from an originating endpoint to a destination
   * address string.
   * <p>
   * The Call must be in the <CODE>Call.IDLE</CODE> state (and therefore have
   * no existing associated Connections and the Provider must be in the
   * <CODE>Provider.IN_SERVICE</CODE> state. The successful effect of this
   * method is to place the telephone call and create and return two
   * Connections associated with this Call.
   *
   * <H5>Method Arguments</H5>
   *
   * This method has three arguments. The first argument is the originating
   * Terminal for the telephone call. The second argument is the originating
   * Address for the telephone Call. This Terminal/Address pair must reference
   * one another. That is, the originating Address must appear on the Terminal
   * (via <CODE>Address.getTerminals()</CODE> and the originating Terminal
   * must appear on the Address (via <CODE>Terminal.getAddress()</CODE>). If
   * not, an InvalidArgumentException is thrown.
   * <p>
   * The third argument is a destination string whose value represents the
   * address to which the telephone call is placed. This destination address
   * must be valid and complete.
   *
   * <H5>Method Post-conditions</H5>
   *
   * This method returns successfully when the Provider can successfully
   * initiate the placing of the telephone call. As a result, when the
   * <CODE>Call.connect()</CODE> method returns, the Call will be in the
   * <CODE>Call.ACTIVE</CODE> state and exactly two Connections will be
   * created and returned. The Connection associated with the originating
   * endpoint is the first Connection in the returned array, and the Connection
   * associated with the destination endpoint is the second Connection in the
   * returned array. These two Connections must at least be in the
   * <CODE>Connection.IDLE</CODE> state. That is, if one of the Connections
   * progresses beyond the <CODE>Connection.IDLE</CODE> state while this
   * method is completing, this Connection may be in a state other than the
   * <CODE>Connection.IDLE</CODE>. This state must be reflected by an event
   * sent to the application.
   *
   * <H5>Call Scenarios or Flows</H5>
   *
   * As a telephone call progresses during its lifetime, the various objects
   * associated with the Call may change state and new objects may be created
   * an associated with the Call. These changes typically occur after this
   * method has successfully returned.
   * <p>
   * How the Call and its associated objects progress depends upon real-world
   * conditions. There is a large but finite number of ways in which the state
   * of a Call may progress. It is difficult to enumerate each possible way
   * in which the state of a Call may progress. Instead, several illustrative
   * <EM>scenarios</EM> (also called <EM>flows</EM>) are presented for common
   * real-world conditions. These scenarios obey the valid state transitions
   * as defined by the Call, Connection, and TerminalConnection objects.
   * <p>
   * Two common scenarios are presented below. Note that there may exist
   * additional scenarios very similar to these which may only differ in a
   * single step or state change. Any implementation which adheres to the
   * rules outlined by the Call, Connection, and TerminalConnection objects
   * is considered a proper implementations with respect to call flows.
   *
   * <H5>Normal <CODE>Call.connect()</CODE> Scenario</H5>
   *
   * This this scenario, a telephone call is placed to a telephone number
   * address. This address is outside the Provider's domain. The destination
   * party answers the telephone call.
   *
   * <OL>
   * <p>
   * <LI>The <CODE>Call.connect()</CODE> method is invoked with the given
   * arguments. Two Connection objects are created and returned, each in the
   * <CODE>Connection.IDLE</CODE> state.
   * <p>
   * <B>Events delivered to the application:</B> a CallActivEv and two
   * ConnectionCreatedEventID, one for each Connection.
   * <p>
   * <LI>Once the Provider begins to place the telephone call, the originating
   * Connection moves from the <CODE>Connection.IDLE</CODE> state into 
   * the <CODE>Connection.CONNECTED</CODE> state. A TerminalConnection is
   * created in the <CODE>TerminalConnection.IDLE</CODE> state and moves
   * into the to model the <CODE>TerminalConnection.ACTIVE</CODE>
   * relationship between the originating Terminal and the Connection.
   * <p>
   * <B>Events delivered to the application:</B> a ConnectionEvent.CONNECTION_CONNECTED for the
   * originating Connection, a TerminalConnectionEvent.TERMINAL_CONNECTION_CREATED and a TerminalConnection.TERMINAL_CONNECTION_ACTIVE for
   * the new TerminalConnection.
   * <p>
   * <B>Note:</B> Depending upon the configuration of the switch, additional
   * TerminalConnection objects associated with the originating Connection may
   * be created. If the originating Address has more than on Terminal, these
   * additional Terminals may be involved in the telephone call. For each
   * TerminalConnection created a TerminalConnectionEvent.TERMINAL_CONNECTION_CREATED is delivered. Typically,
   * these TerminalConnections will be in the
   * <CODE>TerminalConnection.PASSIVE</CODE> state and a TerminalConnectionEvent.TERMINAL_CONNECTION_PASSIVE
   * is delivered for each.
   * <p>
   * <LI>The destination Connection into the
   * <CODE>Connection.INPROGRESS</CODE> state as the Call proceeds.
   * <p>
   * <B>Events delivered to the application:</B> a ConnectionEvent.CONNECTION_IN_PROGRESS for the
   * destination Connection.
   * <p>
   * <LI>The destination Connection moves into the
   * <CODE>Connection.ALERTING</CODE> state as the destination is alerted to
   * the telephone call. TerminalConnection object may be created to model
   * the relationship between any known destination Terminals associated with
   * the Call, each in the <CODE>TerminalConnection.RINGING</CODE> state. If
   * the destination Terminals are unknown, then no TerminalConnections are
   * created.
   * <p>
   * <B>Events delivered to the application:</B> a ConnectionEvent.CONNECTION_ALERTING for the
   * destination Connection, a TerminalConnectionEvent.TERMINAL_CONNECTION_CREATED 
   * and TerminalConnectionEvent.TERMINAL_CONNECTION_RINGING for
   * any destination TerminalConnections created.
   * <p>
   * <LI>The destination Connection moves into the
   * <CODE>Connection.CONNECTED</CODE> state when the called party answers the
   * telephone call. If the destination Terminals are known, the answering
   * TerminalConnection moves into the <CODE>TerminalConnection.ACTIVE</CODE>
   * state.
   * <p>
   * <B>Events delivered to the application:</B> a ConnectionEvent.CONNECTION_CONNECTED for the
   * destination Connection and a TerminalConnectionEvent.TERMINAL_CONNECTION_ACTIVE for the answering
   * TerminalConnection, if known.
   * <p>
   * <B>Note:</B>
   * For all other non-answering destination TerminalConnections known, either
   * one of two state changes will occur depending upon the configuration of
   * the switch. These TerminalConnections will either move into the
   * <CODE>TerminalConnection.PASSIVE</CODE> state or the
   * <CODE>TerminalConnection.DROPPED</CODE> state, depending upon whether or
   * not these Terminals continue as part of the telephone call. For each,
   * either a TerminalConnectionEvent.TERMINAL_CONNECTION_PASSIVE or TerminalConnectionEvent.TERMINAL_CONNECTION_DROPPED is delivered.
   * </OL>
   * <p>
   * At the conclusion of this scenario, the Call will be in the
   * <CODE>Call.ACTIVE</CODE> state, both Connections will be in the
   * <CODE>Connection.CONNECTED</CODE> state, and the originating
   * TerminalConnection will be in the <CODE>TerminalConnection.ACTIVE</CODE>
   * state.
   *
   * <H5>Failure <CODE>Call.connect()</CODE> Scenario</H5>
   *
   * In this scenario, a telephone call is placed to a destination address.
   * The destination cannot be reached, perhaps because the destination address
   * is busy.
   * <p>
   * The first three steps of the first scenario are the same as in this
   * scenario. They are not repeated here for brevity. The fourth and final
   * step of this scenario is:
   * <p>
   * <OL>
   * <LI>The destination Connection moves into the
   * <CODE>Connection.FAILED</CODE> because the destination party could not
   * be reached. (e.g. busy)
   * <p>
   * <B>Events delivered to the application:</B> a ConnectionEvent.CONNECTION_FAILED is delivered
   * for the destination Connection.
   * </OL>
   * <p>
   * At the conclusion of this scenario, the Call will be in the
   * <CODE>Call.ACTIVE</CODE> state, the originating Connection will be in the
   * <CODE>Connection.CONNECTED</CODE> state, the destination Connection will
   * be in the <CODE>Connection.FAILED</CODE> state, and the originating
   * TerminalConnection will be in the <CODE>TerminalConnection.ACTIVE</CODE>
   * state.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.IDLE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>Let Connection c[] = this.getConnections()
   * <LI>c.length == 2
   * <LI>c[0].getState() == Connection.IDLE (at least)
   * <LI>c[1].getState() == Connection.IDLE (at least)
   * </OL>
   * @see javax.telephony.CallEvent
   * @see javax.telephony.ConnectionEvent
   * @see javax.telephony.TerminalConnectionEvent
   * @param origterm The originating Terminal for this telephone call.
   * @param origaddr The originating Address for this telephone call.
   * @param dialedDigits The destination address string for this telephone call.
   * @exception ResourceUnavailableException An internal resource necessary
   * for placing the phone call is unavailable.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to place a telephone call.
   * @exception InvalidPartyException Either the originator or the destination
   * does not represent a valid party required to place a telephone call.
   * @exception InvalidArgumentException An argument provided is not valid
   * either by not providing enough information for this method or is
   * inconsistent with another argument.
   * @exception InvalidStateException Some object required by this method is
   * not in a valid state as designated by the pre-conditions for this method.
   * @exception MethodNotSupportedException The implementation does not
   * support this method.
   * @return array of Connections
   */
  public Connection[] connect(Terminal origterm, Address origaddr,
			      String dialedDigits)
    throws ResourceUnavailableException, PrivilegeViolationException,
      InvalidPartyException, InvalidArgumentException, InvalidStateException,
      MethodNotSupportedException;


  /**
   * Adds an observer to the Call. The <CODE>CallObserver</CODE> reports all
   * Call-related events. This includes changes in the state of the Call and
   * all Connection-related and TerminalConnection-related events. The observer
   * added with this method will report events on the call for as long as the
   * implementation can observer the Call. In the case that the implementation
   * can no longer observe the Call, the applications receives a
   * CallObservationEndedEv. The observer receives no more events after it
   * receives the CallObservationEndedEv and is no longer reported via the
   * <CODE>Call.getObservers()</CODE> method.
   *
   * <H5>Observer Lifetime</H5>
   *
   * The <CODE>CallObserver</CODE> will receive events until one of the
   * following occurs, whereupon the observer receives a CallObservationEndedEv
   * and the observer is no longer reported via the 
   * <CODE>Call.getObservers()</CODE> method.
   * <p>
   * <OL>
   * <LI>The observer is removed by the application with 
   * <CODE>Call.removeObserver()</CODE>.
   * <LI>The implementation can no longer monitor the call.
   * <LI>The Call has completed and moved into the 
   * <CODE>Call.INVALID</CODE> state.
   * </OL>
   *
   * <H5>Event Snapshots</H5>
   *
   * By default, when an observer is added to a telephone call, the first
   * batch of events may be a "snapshot". That is, if the observer was added
   * after state changes in the Call, the first batch of events will inform
   * the application of the current state of the Call. Note that these
   * snapshot events do NOT provide a history of all events on the Call, rather
   * they provide the minimum necessary information to bring the application
   * up-to-date with the current state of the Call. The meta code for all of
   * these events will be <CODE>Ev.META_SNAPSHOT</CODE>.
   *
   * <H5>CallObservers from Addresses and Terminals</H5>
   *
   * There may be additional call observers on the call which were not added
   * by this method. These observers may have become part of the call via
   * the <CODE>Address.addCallObserver()</CODE> and 
   * <CODE>Terminal.addCallObserver()</CODE> methods. See the specifications
   * for these methods for more information.
   *
   * <H5>Multiple Invocations</H5>
   *
   * If an application attempts to add an instance of an observer already
   * present on this Call, there are two possible outcomes:
   * <p>
   * <OL>
   * <LI>If the observer was added by the application using this method,
   * then a repeated invocation will silently fail, i.e. multiple instances
   * of an observer are not added and no exception will be thrown.
   * <LI>If the observer is part of the call because an application invoked
   * <CODE>Address.addCallObserver()</CODE> or 
   * <CODE>Terminal.addCallObserver()</CODE>, either of these methods
   * modifies the behavior of that observer as if it were added via this
   * method instead. That is, the observer is no longer removed when the call
   * leaves the Address or Terminal. The observer now receives events until
   * one of the conditions in  "Observer Lifetime" is met.
   * </OL>
   * <p>
   * <B>Post-Conditions:</B>
   * <OL>
   * <LI>observer is an element of <CODE>this.getObservers()</CODE>
   * <LI>A snapshot of events is delivered to the observer, if appropriate.
   * </OL>
   * @param observer The observer being added.
   * @exception MethodNotSupportedException The observer cannot be added at this time
   * @exception ResourceUnavailableException The resource limit for the
   * numbers of observers has been exceeded.
   */
  public void addObserver(CallObserver observer)
    throws ResourceUnavailableException, MethodNotSupportedException;


  /**
   * Returns an array of all <CODE>CallObservers</CODE> on this Call. If no
   * observers are on this Call object, then this method returns null. This
   * method returns all observers on this call no matter how they were added
   * to the Call. Call observers may be added to this call in one of three
   * ways:
   * <OL>
   * <LI>The application added the observer via
   * <CODE>Call.addObserver()</CODE>.
   * <LI>The application added the observer via
   * <CODE>Address.addCallObserver()</CODE> and the call came to that Address.
   * <LI>The application added the observer via
   * <CODE>Terminal.addCallObserver()</CODE> and the call came to that
   * Terminal.
   * </OL>
   * <p>
   * An instance of a CallObserver object will only appear once in this list.
   * <p>
   * <B>Post-Conditions:</B>
   * <OL>
   * <LI>Let CallObserver[] obs = this.getObservers()
   * <LI>obs == null or obs.length >= 1
   * </OL>
   * @return An array of CallObserver objects associated with this Call.
   */
  public CallObserver[] getObservers();


  /**
   * Removes the given observer from the Call. If successful, the observer will
   * receive a CallObservationEndedEv as the last event it receives and will
   * no longer be reported via the <CODE>Call.getObservers()</CODE> method.
   * <p>
   * This method has different effects depending upon how the observer was
   * added to the Call, as follows:
   * <p>
   * <OL>
   * <LI>If the observer was added via <CODE>Call.addObserver()</CODE>, this
   * method removes the observer until it is re-applied by the application.
   * <LI>If the observer was added via <CODE>Address.addCallObserver()</CODE>
   * or <CODE>Terminal.addCallObserver()</CODE>, this method removes the
   * observer for this call only. It does not affect whether this observer
   * will be added to future calls which come to that Address. See
   * <CODE>Address.addCallObserver()</CODE> and
   * <CODE>Terminal.addCallObserver()</CODE> for more details.
   * </OL>
   * <p>
   * If an observer is not part of the Call, then this method fails silently,
   * i.e. no observer is removed and no exception is thrown.
   * <p>
   * <B>Post-Conditions:</B>
   * <OL>
   * <LI>observer is not an element of this.getObservers()
   * <LI>CallObservationEndedEv is delivered to the application
   * </OL>
   * @param observer The observer which is being removed.
   */
  public void removeObserver(CallObserver observer);

  
 /**
  * Returns the dynamic capabilities for the instance of the Call object.
  * Dynamic capabilities tell the application which actions are possible at
  * the time this method is invoked based upon the implementations knowledge
  * of its ability to successfully perform the action. This determination may
  * be based upon argument passed to this method, the current state of the
  * call model, or some implementation-specific knowledge. These indications
  * do not guarantee that a particular method can be successfully invoked,
  * however.
  * <p>
  * The dynamic call capabilities are based upon a Terminal/Address pair as
  * well as the instance of the Call object. These parameters are used to
  * determine whether certain call actions are possible at the present. For
  * example, the <CODE>CallCapabilities.canConnect()</CODE> method will
  * indicate whether a telephone call can be placed using the Terminal/Address
  * pair as the originating endpoint.
  * <p>
  * @param terminal Dynamic capabilities are with respect to this Terminal.
  * @param address Dynamic capabilities are with respect to this Address.
  * @return The dynamic Call capabilities.
  * @exception InvalidArgumentException Indicates the Terminal and/or Address
  * argument provided was not valid.
  */
  public CallCapabilities getCapabilities(Terminal terminal, Address address)
    throws InvalidArgumentException;

 /**
  * Gets the CallCapabilities object with respect to a Terminal and an Address.
  * If null is passed as a Terminal parameter, the general/provider-wide Call 
  * capabilities are returned.
  * <p>
  * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2. The
  * <CODE>Call.getCapabilities()</CODE> method returns the dynamic Call
  * capabilities. This method now should simply invoke the
  * <CODE>Call.getCapabilities()</CODE> method with the given Terminal and
  * Address arguments.
  * <p>
  * @deprecated Since JTAPI v1.2. This method has been replaced by the
  * Call.getCapabilities() method.
  * @param term Dynamic Call capabilities in JTAPI v1.2 are with respect to
  * this Terminal.
  * @param addr Dynamic Call capabilities in JTAPI v1.2 are with respect to
  * this Address.
  * @exception InvalidArgumentException Indicates the Terminal and/or Address
  * argument provided was not valid.
  * @exception PlatformException A platform-specific exception occurred.
  * @return dynamic call capabilities
  */
  public CallCapabilities getCallCapabilities(Terminal term, Address addr)
    throws InvalidArgumentException, PlatformException;

  /**
   * Adds an listener to the Call. The <CODE>CallListener</CODE> reports all
   * Call-related events. <I>This includes changes in the state of the Call and
   * all Connection-related and TerminalConnection-related events.</I>
   * The listener added with this method will report events on the
   * call for as long as the implementation can listener the Call. In
   * the case that the implementation
   * can no longer observe the Call, the applications receives a
   * CALL_EVENT_TRANSMISSION_ENDED. The listener receives no more events after it
   * receives the CALL_EVENT_TRANSMISSION_ENDED and is no longer reported via the
   * <CODE>Call.getCallListeners()</CODE> method.
   *
   * <H5>Listener Lifetime</H5>
   *
   * The <CODE>CallListener</CODE> will receive events until one of the
   * following occurs, whereupon the listener receives a CALL_EVENT_TRANSMISSION_ENDED
   * and the listener is no longer reported via the 
   * <CODE>Call.getCallListeners()</CODE> method.
   * <p>
   * <OL>
   * <LI>The listener is removed by the application with 
   * <CODE>Call.removeCallListener()</CODE>.
   * <LI>The implementation can no longer monitor the call.
   * <LI>The Call has completed and moved into the 
   * <CODE>Call.INVALID</CODE> state.
   * </OL>
   *
   * <H5>Event Snapshots</H5>
   *
   * By default, when an listener is added to a telephone call, the first
   * batch of events may be a "snapshot". That is, if the listener was added
   * after state changes in the Call, the first batch of events will inform
   * the application of the current state of the Call. Note that these
   * snapshot events do NOT provide a history of all events on the Call, rather
   * they provide the minimum necessary information to bring the application
   * up-to-date with the current state of the Call. The meta code for all of
   * these events will be <CODE>Event.META_SNAPSHOT</CODE>.
   *
   * <H5>CallListeners from Addresses and Terminals</H5>
   *
   * There may be additional call listeners on the call which were not added
   * by this method. These listeners may have become part of the call via
   * the <CODE>Address.addCallListener()</CODE> and 
   * <CODE>Terminal.addCallListener()</CODE> (or related) methods.
   * See the specifications
   * for these methods for more information.
   *
   * <H5>Multiple Invocations</H5>
   *
   * If an application attempts to add an instance of an listener already
   * present on this Call, there are two possible outcomes:
   * <p>
   * <OL>
   * <LI>If the listener was added by the application using this method,
   * then a repeated invocation will silently fail, i.e. multiple instances
   * of an listener are not added and no exception will be thrown.
   * <LI>If the listener is part of the call because an application invoked
   * <CODE>Address.addCallListener()</CODE> or 
   * <CODE>Terminal.addCallListener()</CODE>, either of these methods
   * modifies the behavior of that listener as if it were added via this
   * method instead. That is, the listener is no longer removed when the call
   * leaves the Address or Terminal. The listener now receives events until
   * one of the conditions in  "Listener Lifetime" is met.
   * </OL>
   * <p>
   * <B>Post-Conditions:</B>
   * <OL>
   * <LI>listener is an element of <CODE>this.getCallListeners()</CODE>
   * <LI>A snapshot of events is delivered to the listener, if appropriate.
   * </OL>
   * @param listener The listener being added.
   * @exception MethodNotSupportedException The listener cannot be added at this time
   * @exception ResourceUnavailableException The resource limit for the
   * numbers of listeners has been exceeded.
   */
  public void addCallListener(CallListener listener)
    throws ResourceUnavailableException, MethodNotSupportedException;

  /**
   * Returns an array of all <CODE>CallListeners</CODE> on this Call. If no
   * listeners are on this Call object, then this method returns null. This
   * method returns all listeners on this call no matter how they were added
   * to the Call. Call listeners may be added to this call in one of three
   * ways:
   * <OL>
   * <LI>The application added the listener via
   * <CODE>Call.addCallListener()</CODE>.
   * <LI>The application added the listener via
   * <CODE>Address.addCallListener()</CODE> and the call came to that Address.
   * <LI>The application added the listener via
   * <CODE>Terminal.addCallListener()</CODE> and the call came to that
   * Terminal.
   * </OL>
   * <p>
   * An instance of a CallListener object will only appear once in this list.
   * <p>
   * <B>Post-Conditions:</B>
   * <OL>
   * <LI>Let CallListener[] obs = this.getCallListeners()
   * <LI>obs == null or obs.length >= 1
   * </OL>
   * @return An array of CallListener objects associated with this Call.
   */
  public CallListener[] getCallListeners();

  /**
   * Removes the given listener from the Call. If successful, the listener will
   * receive a CALL_EVENT_TRANSMISSION_ENDED as the last event it receives and will
   * no longer be reported via the <CODE>Call.getCallListeners()</CODE> method.
   * <p>
   * This method has different effects depending upon how the listener was
   * added to the Call, as follows:
   * <p>
   * <OL>
   * <LI>If the listener was added via <CODE>Call.addCallListener()</CODE>, this
   * method removes the listener until it is re-applied by the application.
   * <LI>If the listener was added via <CODE>Address.addCallListener()</CODE>
   * or <CODE>Terminal.addCallListener()</CODE>, this method removes the
   * listener for this call only. It does not affect whether this listener
   * will be added to future calls which come to that Address. See
   * <CODE>Address.addCallListener()</CODE> and
   * <CODE>Terminal.addCallListener()</CODE> for more details.
   * </OL>
   * <p>
   * If an listener is not part of the Call, then this method fails silently,
   * i.e. no listener is removed and no exception is thrown.
   * <p>
   * <B>Post-Conditions:</B>
   * <OL>
   * <LI>listener is not an element of this.getCallListeners()
   * <LI>CALL_EVENT_TRANSMISSION_ENDED is delivered to the application
   * </OL>
   * @param listener The listener which is being removed.
   */
  public void removeCallListener(CallListener listener);

}

