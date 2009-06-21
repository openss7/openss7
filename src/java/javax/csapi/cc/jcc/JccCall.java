/*
 @(#) $RCSfile: JccCall.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:33 $ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date: 2009-06-21 11:34:33 $ by $Author: brian $
 */

package javax.csapi.cc.jcc;

/** A JccCall is a transient association of (zero or more) addresses for the purposes of engaging in a real-time
  * communications interchange.  The call and its associated connection and address objects describe the control and
  * media flows taking place in some underlying "real world" communication network. Other parties involved in the call
  * may also exert control over it, thus the membership and state of the endpoints may change without explicit request
  * by the Jcc application. The {@link JccProvider} adjusts the call, address and connection objects to reflect the
  * results of these combined command actions.  <p>
  *
  * <h4>Introduction</h4>
  * A JccCall can have zero or more {@link JccConnection}s.  A two-party call has two JccConnections, and a conference
  * call has three or more JccConnections. Each JccConnection models the relationship between a JccCall and an
  * JccAddress, where an JccAddress identifies a particular party or set of parties on a call. <p>
  *
  * <h4>Creating JccCall Objects</h4>
  * Applications create instances of a JccCall object with the {@link JccProvider#createCall()} method, which returns a
  * JccCall object that has zero Connections and is in the {@link JccCall#IDLE} state.  The JccCall maintains a
  * reference to its JccProvider for the life of that JccCall object.  The JccProvider object instance does not change
  * throughout the lifetime of the JccCall object. The JccProvider associated with a JccCall is obtained via the
  * {@link #getProvider()} method. <p>
  *
  * <h4>JccCall States</h4>
  * A JccCall has a <em>state</em> which is obtained via the {@link #getState()} method. This state describes the
  * current progress of a telephone call, where is it in its life cycle, and how many connections exist on the call. The
  * JccCall state may be one of three values: {@link #IDLE}, {@link #ACTIVE}, or {@link #INVALID}. The following is a
  * description of each state: <p>
  * <table>
  * <tr>
  *     <td width="20%">{@link #IDLE}</td>
  *     <td width="80%"> This is the initial state for all calls. In this state, the JccCall has zero connections, that
  *     is {@link #getConnections()} <em>must</em> return null.  </td>
  * </tr>
  * <tr>
  *     <td width="20%">{@link #ACTIVE}</td>
  *     <td width="80%"> A call with some current ongoing activity is in this state. JccCalls with one or more
  *     associated JccConnections must be in this state. If a JccCall is in this state, the
  *     <code>getConnections()</code> method must return an array of size at least one.  </td>
  * </tr>
  * <tr>
  *     <td width="20%">{@link #INVALID}</td>
  *     <td width="80%"> This is the final state for all calls. JccCall objects which lose all of their JccConnection
  *     objects (via a transition of the JccConnection object into the {@link JccConnection#DISCONNECTED} state) moves
  *     into this state. Calls in this state have zero JccConnections and these JccCall objects may not be used for any
  *     future action. In this state, the <code>getConnections()</code> <em>must</em> return null.  </td>
  * </tr>
  * </table> <p>
  *
  * <h4>JccCall State Transitions</h4>
  * Consider the transitions given below as the ones that would be visible to the application developer in the package
  * considered. This may be either because there is a method in that class/interface that provides that behavior (read
  * transition) or because of events in the underlying platform. Note that we are not making any statements to the
  * effect that the other transitions (ones not shown) are disallowed. The application developer would have to keep in
  * mind the possibility that the "invisible" transitions can surface (become visible) in inherited classes/interfaces.
  * <p>
  *
  * <center> <h1>JccCall FSM</h1> <img src="doc-files/callstates.gif"></center>
  *
  * <h4>JccCall and JccConnection objects</h4>
  * A JccCall maintains a list of the JccConnections on that JccCall.  Applications obtain an array of JccConnections
  * associated with the JccCall via the <code>getConnections()</code> method. A JccCall retains a reference to a
  * JccConnection only if it is not in the {@link JccConnection#DISCONNECTED} state. Therefore, if a JccCall has a
  * reference to a JccConnection, then that JccConnection must not be in the <code>JccConnection.DISCONNECTED</code>
  * state.  When a JccConnection moves into the <code>JccConnection.DISCONNECTED</code> state (e.g. when a party hangs
  * up), the JccCall loses its reference to that JccConnection which is no longer reported via the
  * <code>JccCall.getConnections()</code> method.
  *
  * <h4>Listeners and Events</h4>
  * The {@link JccCallListener} or {@link JccConnectionListener} interface reports all events pertaining to the JccCall
  * object.  Events delivered to this interface must implement or extend the {@link JccCallEvent} interface.
  * Applications can add listeners to a JccCall object via the {@link #addCallListener(JccCallListener)} method.  <p>
  *
  * Connection-related events can be reported via the <code>JccConnectionListener</code> interface.  These events
  * include the creation of these objects and their state changes. Events which are reported via the
  * <code>JccConnectionListener</code> interface pertaining to JccConnections or JccCalls extend the
  * {@link JccConnectionEvent} interface.  <p>
  *
  * <h5><A name="event_snapshots">Event Snapshots</A></h5>
  * By default, when an listener is added to a telephone call, the first batch of events may be a "snapshot".  That is,
  * if the listener was added after state changes in the Call, the first batch of events will inform the application of
  * the current state of the Call. Note that these snapshot events do NOT provide a history of all events on the Call,
  * rather they provide the minimum necessary information to bring the application up-to-date with the current state of
  * the Call.  <p>
  *
  * <h5>When Event Transmission Ends</h5>
  * At times it may become impossible for the implementation to report events to an application.  In this case, a
  * {@link JccCallEvent#CALL_EVENT_TRANSMISSION_ENDED} is delivered to an object registered as a
  * <code>JccCallListener</code> (or an extension of that interface).  This is the final event received by the Listener
  * from the call. <p>
  * @version 1.2.2
  * @author Monavacon Limited
  * @since 1.0 */
public interface JccCall {
    /** Add a connection listener to all connections under this call.  This method applies to any JccConnection that
      * already exists when the method is called as well as any JccConnections that will be added in the future to this
      * JccCall object.  This method behaves similar to the addCallListener(conListener, filter) method on this
      * interface where conListener is in fact a JccConnectionListener.  When conListener is not a JccConnectionListener
      * but is only a JccCallListener, a similar behavior as addCallListener(conListener, newfilter) can be obtained
      * with this method using a JccConnectionListener and a different EventFilter which filters all the JccConnection
      * related events in addition to the events being filtered by newfilter.  Note though that using this method only
      * JccConnectionListeners can be added.  <p>
      *
      * Registering a single listener twice will result in the last filter being used for the purposes of consultation
      * to determine the events that the application is interested in.  Note that registering for the same event
      * multiple times should not result in multiple notifications being sent to an application for the same event. <p>
      *
      * <b>Post-Conditions:</b> <ol>
      * <li>A snapshot of events is delivered to the listener, if appropriate.  </ol>
      * @param cl
      * JccConnectionListener object that receives the specified events.
      * @param filter
      * EventFilter determines if the event is to be delivered to the specified listener. 
      * @throws MethodNotSupportedException
      * The listener cannot be added at this time.
      * @throws ResourceUnavailableException
      * The resource limit for the number of listeners has been exceeded.  */
    public void addConnectionListener(JccConnectionListener cl, EventFilter filter)
        throws ResourceUnavailableException, MethodNotSupportedException;
    /** Removes the connection listener from all connections under this call. Note that if the listener is currently not
      * registered then this method fails silently. 
      * @param cl
      * JccConnectionListener object that was registered using a corresponding addConnectionListener method.  */
    public void removeConnectionListener(JccConnectionListener cl);
    /** This method requests the release of the call object and associated connection objects. Thus this method is
      * equivalent to using the {@link JccConnection#release(int)} method on each JccConnection which is part of the
      * Call. Typically each JccConnection associated with this call will move into the {@link
      * JccConnection#DISCONNECTED} state.  The call will also be terminated in the network. If the application has
      * registered as a listener then it receives the {@link JccCallEvent#CALL_EVENT_TRANSMISSION_ENDED} event.  <p>
      *
      * Valid cause codes (prefixed by <code>CAUSE_</code>) for the integer that is named causeCode are defined in
      * {@link JccEvent} and {@link JccCallEvent}.  <p>
      *
      * <b>Pre-conditions:</b> <ol>
      * <li>(this.getProvider()).getState() == IN_SERVICE
      * <li> this.getState() == ACTIVE </ol>
      *
      * <b>Post-conditions:</b> <ol>
      * <li>(this.getProvider()).getState() == IN_SERVICE
      * <li> this.getState() == INVALID
      * <li>CALL_EVENT_TRANSMISSION_ENDED event delivered to the valid Calllisteners. 
      * <li>Appropriate ConnectionEvents are also delivered to the ConnectionListeners.  </ol>
      *
      * @param causeCode
      * An integer that represents a cause code.  Valid values are defined in {@link JccEvent} and {@link JccCallEvent},
      * they are typically prefixed by <code>CAUSE_</code>.
      * @throws PrivilegeViolationException
      * The application does not have the authority or permission to disconnect the Call. For example, an  Address
      * associated with this Call may not be controllable in the Provider's domain. 
      * @throws ResourceUnavailableException
      * An internal resource required to drop a connection is not available. 
      * @throws InvalidStateException
      * Some object required for the successful invocation of this method is not in the proper state as given by this
      * method's pre-conditions. 
      * @throws InvalidArgumentException
      * The given release cause code is invalid.
      * @version 1.2.2
      * @author Monavacon Limited
      * @since 1.0a */
    public void release(int causeCode)
        throws PrivilegeViolationException, ResourceUnavailableException, InvalidStateException,
                          InvalidArgumentException;

    /** The application calls this method to supervise a call. The application can set a granted connection time for
      * this call. If an application calls this function before it calls a {@link
      * #routeCall(java.lang.String,java.lang.String,java.lang.String,java.lang.String)}, the timer measurement will start as soon as the call is answered by
      * the called party.  A call supervision event is also sent if the call is terminated before the supervision event
      * occurs.  <p>
      *
      * The call supervision events are then reported to the listener registered using this method.  The events related
      * to call supervision are reported only to the listeners registered using this method.  <p>
      *
      * <em> Note </em> that a policy object may be designed to define the policy to be implemented by the platform as a
      * result of this method instead of defining the policy through the given parameters. This might be designed in the
      * future specifications. 
      * @param calllistener
      * JccCallListener object that receives the specified events.
      * @param time
      * Specifies the granted time in milliseconds for the connection.  When specified as 0, volume based supervision is
      * applied. Either bytes(volume) or time should be specified.
      * @param treatment
      * Defines the treatment of the call by the call control service when the call supervision timer expires. The
      * values which may be combined using a logical OR function are: <ol>
      * <li><code>0x01</code> to release the call when the call supervision timer expires.
      * <li><code>0x02</code> to notify the application when the call supervision timer expires.
      * <li><code>0x04</code> to send a warning tone to the controlling party when a call supervision timer expires. If
      * call release is requested, then the call will be released following the tone after an administered time period.
      * </ol>
      * @throws MethodNotSupportedException
      * Thrown when the implementation does not support this method. 
      * @since 1.0b */
    public void superviseCall(JccCallListener calllistener,double time,int treatment) throws 
    MethodNotSupportedException;
    /** Places a call from an originating address to a destination address string.  <p>
      *
      * The Call must be in the {@link JccCall#IDLE} state (and therefore have no existing associated JccConnections and
      * the Provider must be in the {@link JccProvider#IN_SERVICE} state.  The successful effect of this method is to
      * place the call and create and return two JccConnections associated with this Call.  <p>
      *
      * <h5>Method Arguments</h5>
      * This method has two arguments. The first argument is the originating Address for the Call. The second argument
      * is a destination string whose value represents the address to which the call is placed. This destination address
      * must be valid and complete.  <p>
      *
      * <h5>Method Post-conditions</h5>
      * This method returns successfully when the Provider can successfully initiate the placing of the call. As a
      * result, when the JccCall.connect() method returns, the JccCall will be in the {@link JccCall#ACTIVE} state and
      * exactly two JccConnections will be created and returned. The JccConnection associated with the originating
      * endpoint is the first JccConnection in the returned array.  This JccConnection will execute the originating
      * JccConnection's Final State Diagram (see <a href="package-summary.html#TOConnections">table 3</a>). The
      * JccConnection associated with the destination endpoint is the second JccConnection in the returned array and
      * will execute the terminating JccConnection's Final State Diagram. These two JccConnections must at least be in
      * the {@link JccConnection#IDLE} state. That is, if one of the Connections progresses beyond the IDLE state while
      * this method is completing, this Connection may be in a state other than the IDLE. This state must be reflected
      * by an event sent to the application.  <p>
      *
      * <b>Pre-Conditions:</b> <ol>
      * <li>(this.getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>this.getState() == JccCall.IDLE </ol>
      *
      * <b>Post-Conditions:</b> <ol>
      * <li>(this.getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>this.getState() == JccCall.ACTIVE 
      * <li>Let Connection c[] = this.getConnections() 
      * <li>c.length == 2 
      * <li>c[0].getState() == JccConnection.IDLE (at least) 
      * <li>c[1].getState() == JccConnection.IDLE (at least) </ol>
      *
      * @param origaddr
      * The originating Address for this call.
      * @param dialedDigits
      * The destination address string for this call.
      * @return
      * Array of Connections 
      * @throws ResourceUnavailableException
      * An internal resource necessary for placing the call is unavailable.
      * @throws PrivilegeViolationException
      * The application does not have the proper authority to place a call.
      * @throws InvalidPartyException
      * Either the originator or the destination does not represent a valid party required to place a call.
      * @throws InvalidStateException
      * Some object required by this method is not in a valid state as designated by the pre-conditions for this method.
      * @throws MethodNotSupportedException
      * The implementation does not support this method.
      * @since 1.0a */
    public JccConnection[] connect(JccAddress origaddr, java.lang.String dialedDigits)
        throws ResourceUnavailableException, PrivilegeViolationException, InvalidPartyException, InvalidStateException,
                          MethodNotSupportedException;
    /** Creates a new JccConnection and attaches it to this JccCall. The JccConnection object is associated with an
      * JccAddress object corresponding to the string given as an input parameter. Note that following this operation
      * the returned JccConnection object must still be routed to the JccAddress which can be accomplished using the
      * {@link JccConnection#routeConnection(boolean)}.  <p>
      *
      * <h5><A NAME="createConnectionSemantics">Examples</a></h5> <ul>
      * <li></code>createConnection("B","A",null,null)</code> will result in  third party setup of a Connection.  A
      * reference to the  created connection i.e "B"'s connection will be returned.  A getAddress() method invocation on
      * the returned JccConnection reference object results in the JccAddress corresponding to party "B" being returned.
      * Party "B" will have to be routed to in this case.	    
      * <li><code>createConnection("C","A","B","B")</code> will be applied in case the number of party "B" is translated
      * into the number of party "C", while party "A" dialed party "B".  <code>createConnection("C","A","B",null)</code>
      * results in the same behavior.   Note that the connection with address "B" is automatically moved to
      * {@link JccConnection#DISCONNECTED DISCONNECTED} with cause code
      * {@link JccCallEvent#CAUSE_REDIRECTED CAUSE_REDIRECTED} after the connection to "C" is routed.  A reference to
      * the  created connection i.e "C"'s connection will be returned.  A getAddress() method invocation on the returned
      * JccConnection reference object results in the JccAddress corresponding to party "C" being returned.  Party "C"
      * will have to be routed to in this case.	    
      * <li><code>createConnection("C","A","B","D")</code> will be applied in case the number of party "D" is translated
      * into the number of party "C", while party "A" dialed party "B".  A reference to the  created connection i.e
      * "C"'s connection will be returned.  A getAddress() method invocation on the returned JccConnection reference
      * object results in the JccAddress corresponding to party "C" being returned.  Party "C" will have to be routed to
      * in this case. Note that the connection with address "B" is automatically moved to
      * {@link JccConnection#DISCONNECTED DISCONNECTED} with cause code
      * {@link JccCallEvent#CAUSE_REDIRECTED CAUSE_REDIRECTED} after the connection to "C" is routed.  </ul> <p>
      *
      * <b>Pre-conditions:</b> <ol>
      * <li>(this.getProvider()).getState() == IN_SERVICE
      * <li> this.getState() != INVALID </ol> <p>
      *
      * <b>Post-conditions:</b> <ol>
      * <li> let conn = createConnection(..);
      * <li> conn.getState() == IDLE
      * <li> this.getState() == ACTIVE
      * <li>(this.getProvider()).getState() == IN_SERVICE </ol>
      *
      * @return
      * The created JccConnection object
      * @param targetAddress
      * Specifies the address to which the call should be routed.
      * @param originatingAddress
      * Specifies the address of the originating (calling) party for this leg of the call. This is optional and can be
      * set to null. 
      * @param originalCalledAddress
      * Specifies the initial destination address to which this leg of the call was initiated. This is optional and can
      * be set to null.
      * @param redirectingAddress
      * Specifies the last address from which this leg of the call was redirected. This is optional and can be set to
      * null. 
      * @throws InvalidArgumentException
      * An InvalidArgumentException indicates an argument passed to the method is invalid.
      * @throws InvalidStateException
      * Some object required by this method is not in a valid state as designated by the pre-conditions for this method.
      * @throws ResourceUnavailableException
      * An internal resource necessary for creating the Connection object is unavailable. 
      * @throws PrivilegeViolationException
      * The application does not have the proper authority to create the Connection.
      * @throws MethodNotSupportedException
      * The implementation does not support this method
      * @throws InvalidPartyException
      * In case one of the parties given is invalid.  */
    public JccConnection createConnection(java.lang.String targetAddress, java.lang.String originatingAddress, java.lang.String originalCalledAddress, java.lang.String redirectingAddress)
        throws InvalidStateException, ResourceUnavailableException, PrivilegeViolationException, MethodNotSupportedException, InvalidArgumentException, InvalidPartyException;
    /** This method requests routing of a call to the targetAddress given as an input parameter.  Creates a new
      * JccConnection and attaches it to this JccCall. The JccConnection object is associated with an JccAddress object
      * corresponding to the string given as an input parameter.  Note that the addresses are passed as strings. The
      * implementation is expected to find the JccAddress object corresponding to the string assuming that the
      * JccAddress is local to the JccProvider.  The given string may not correspond to any JccAddress object in the
      * JccProvider's domain which would be the case for a call to a remote Address.  This method is equivalent to the
      * {@link #createConnection(java.lang.String,java.lang.String,java.lang.String,java.lang.String)},
      * {@link JccConnection#routeConnection(boolean) JccConnection.routeConnection(FALSE)} and
      * {@link JccConnection#attachMedia()} or is also equivalent to
      * {@link #createConnection(java.lang.String,java.lang.String,java.lang.String,java.lang.String)} and
      * {@link JccConnection#routeConnection(boolean) JccConnection.routeConnection(TRUE)}.
      * <h5><a name="routeCallSemantics">Examples</a></h5> <ul>
      *
      * <li><code>routeCall("B","A",null,null)</code> will result in third party setup of a Connection.  A reference to
      * the created connection i.e "B"'s connection will be returned.  A getAddress() method invocation on the returned
      * JccConnection reference object results in the JccAddress corresponding to party "B" being returned.
      *
      * <li><code>routeCall("C","A","B","B")</code> will be applied in case the number of party "B" is translated into
      * the number of party "C", while party "A" dialed party "B".  <code>routeCall("C","A",null,"B")</code> and
      * <code>routeCall("C","A","B",null)</code> result in the same behavior.  Note that in all three cases the
      * connection with address "B" is automatically moved to {@link JccConnection#DISCONNECTED DISCONNECTED} with cause
      * code {@link JccCallEvent#CAUSE_REDIRECTED CAUSE_REDIRECTED}.  A reference to the  created connection i.e "C"'s
      * connection will be returned.  A getAddress() method invocation on the returned JccConnection reference object
      * results in the JccAddress corresponding to party "C" being returned.
      *
      * <li><code>routeCall("D","A","B","C")</code> will be applied in case the number of party "C" is translated into
      * the number of party "D", while party "A" dialed party B.  A reference to the created connection, i.e. "D"'s
      * connection, will be returned.  A getAddress() method invocation on the returned JccConnection reference object
      * results in the JccAddress corresponding to party "D" being returned.  Note that the connection with address "C"
      * is automatically moved to {@link JccConnection#DISCONNECTED DISCONNECTED} with cause code
      * {@link JccCallEvent#CAUSE_REDIRECTED CAUSE_REDIRECTED}.  </ul>
      *
      * <b>Pre-conditions:</b> <ol>
      * <li>(this.getProvider()).getState() == IN_SERVICE
      * <li> this.getState() != INVALID </ol>
      *
      * <b>Post-conditions:</b> <ol>
      * <li> let conn = routeCall(..);
      * <li>conn.getState() == IDLE (at least)
      * <li>this.getState() == ACTIVE
      * <li>(this.getProvider()).getState() == IN_SERVICE </ol>
      *
      * @param targetAddress
      * Specifies the address to which the call should be routed.
      * @param originatingAddress
      * Specifies the address of the originating (calling) party for this leg of the call. This is optional and can be
      * set to null.
      * @param originalCalledAddress
      * Specifies the initial destination address to which this leg of the call was initiated. This is optional and can
      * be set to null.
      * @param redirectingAddress
      * Specifies the last address from which this leg of the call was redirected. This is optional and can be set to
      * null. 
      * @throws InvalidArgumentException
      * An InvalidArgumentException indicates an argument passed to the method is invalid.
      * @throws InvalidStateException
      * Some object required by this method is not in a valid state as designated by the pre-conditions for this method.
      * @throws ResourceUnavailableException
      * An internal resource necessary for creating the Connection object is unavailable. 
      * @throws PrivilegeViolationException
      * The application does not have the proper authority to create the Connection.
      * @throws MethodNotSupportedException
      * The implementation does not support this method
      * @throws InvalidPartyException
      * In case one of the parties given is invalid. 
      * @return
      * The created JccConnection object */
    public JccConnection routeCall(java.lang.String targetAddress, java.lang.String originatingAddress, java.lang.String originalDestinationAddress, java.lang.String redirectingAddress)
        throws InvalidStateException, ResourceUnavailableException, PrivilegeViolationException,
                          MethodNotSupportedException, InvalidPartyException, InvalidArgumentException;  
	
    /** Retrieves the state of the call. The state will be either {@link #IDLE}, {@link #ACTIVE} or {@link #INVALID}. 
      * @return Integer representing the state of the call.  */
    public int getState();

    /** Add a listener to this call. This also reports all state changes in the state of the Call and Connection
      * objects. The listener added with this method will report events on the call for as long as the implementation
      * can listen to the Call. In the case that the call becomes unobservable the listener receives a {@link
      * JccCallEvent#CALL_EVENT_TRANSMISSION_ENDED} event.
      *
      * <h5>CallListeners from Provider</h5> 
      * here may be additional call listeners on the call which were not added by this method. These listeners may have
      * become part of the call via the {@link javax.csapi.cc.jcc.JccProvider#addCallListener(JccCallListener)} or
      * similar methods.
      *
      * <h5>ConnectionListeners</h5> 
      * ince JccConnectionListener inherits from the JccCallListener, it is also possible to add a JccConnectionListener
      * using this method. In such a case, connection events  would also have to be reported to the registered listener
      * in addition to the call events.  Hence, it is expected that the JCC implementation uses instanceof checks in
      * order to decide if only call events or both call and connection events have to be delivered to the listener.
      * Note that a listener added by this method is expecting all the events without any filtering involved. 
      *
      * <h5>Multiple Invocations</h5>
      * If an application attempts to add an instance of a listener already present on this Call, there are two possible
      * outcomes: <ol>
      *
      * <li>If the listener was added by the application using this method, then a repeated invocation will silently
      * fail, i.e.  multiple instances of an listener are not added and no exception will be thrown. 
      *
      * <li>If the listener is part of the call because an application invoked
      * <code>JccProvider.addCallListener()</code> either of these methods modifies the behavior of that listener as if
      * it were added via this method instead.  </ol>
      *
      * <b>Post-Conditions:</b> <ol>
      * <li>A snapshot of events is delivered to the listener, if appropriate.  </ol>
      * @param calllistener
      * JccCallListener object that receives the specified events.
      * @throws MethodNotSupportedException
      * The listener cannot be added at this time.
      * @throws ResourceUnavailableException
      * The resource limit for the number of listeners has been exceeded.  */
    public void addCallListener(JccCallListener calllistener)
        throws ResourceUnavailableException, MethodNotSupportedException;
    /** Removes a listener from this call. If the listener is not part of the Call for the given address(es), then this
      * method fails silently, i.e. no listener is removed and no exception is thrown.  Note that the listeners will
      * stop receiving events for existing calls also.  <p>
      *
      * This method has different effects depending upon how the listener was added to the Call, as follows: <ol>
      *
      * <li>If the listener was added via {@link JccCall#addCallListener(JccCallListener)}, this method removes the
      * listener until it is re-applied by the application. 
      *
      * <li>If the listener was added via {@link javax.csapi.cc.jcc.JccProvider#addCallListener(JccCallListener)} or
      * simular methods, this method removes the listener for this call only. It does not affect whether this listener
      * will be added to future calls coming in to the JccProvider.  </ol>
      * @param calllistener
      * JccCall Listener object.  */
    public void removeCallListener( JccCallListener calllistener);
    /** Retrieves the provider handling this call object. The Provider reference does not change once the Call object
      * has been created, despite the state of the Call object. 
      * @return JccProvider
      * Object managing this call.  */
    public JccProvider getProvider();
    /** Retrieves an array of  connections asssociated with this call. None of the Connections returned will be in the
      * {@link JccConnection#DISCONNECTED} state. Further, if the Call is in the {@link #IDLE} or {@link #INVALID}
      * state, this method returns null. <p>
      *
      * <b>Post-Conditions:</b> <ol>
      * <li>JccConnection[] conn = getConnections()<br>
      * <li>if this.getState() == IDLE then conn=null <br>
      * <li>if this.getState() == INVALID then conn=null <br>
      * <li>if this.getState() == ACTIVE then conn.length &ge;1 <br>
      * <li>For all i, conn[i].getState() != DISCONNECTED<br> </ol>
      * @return
      * Array of Connections for this call.  */
    public JccConnection[] getConnections();
    /** JccCall.IDLE state indicates the Call has zero Connections. This is the initial state of all Call objects.  */
    public static final int IDLE = 1;
    /** JccCall.ACTIVE state indicates the Call has one or more Connections none of which is in the {@link
      * JccConnection#DISCONNECTED} state.  The Call object transitions into this state from the {@link #IDLE} state
      * only.  */
    public static final int ACTIVE = 2;
    /** The JccCall.INVALID state indicates that the Call has lost all of its connections, that is, all of its
      * Connection objects have moved into the {@link JccConnection#DISCONNECTED} state and are no longer associated
      * with the Call.  A Call in this state cannot be used for future actions.  */
    public static final int INVALID  = 3;
}
 
// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
