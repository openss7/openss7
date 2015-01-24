/*
 @(#) src/java/javax/csapi/cc/jcc/JccConnection.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package javax.csapi.cc.jcc;

import java.lang.*;
import java.util.*;

/** The purpose of a JccConnection object is to describe the
  * relationship between a JccCall object and a JccAddress object. A
  * JccConnection object exists if the JccAddress is a part of the
  * telephone call. Each JccConnection has a state which describes the
  * particular stage of the relationship between the JccCall and
  * JccAddress. These states and their meanings are described below.
  * Applications use the getCall() and getAddress() methods to obtain
  * the JccCall and JccAddress associated with this JccConnection,
  * respectively. <p>
  *
  * From one perspective, an application may view a JccCall only in
  * terms of the JccAddress/JccConnection objects which are part of the
  * JccCall.  This is termed a logical view of the Call. In this logical
  * view, a telephone call is viewed as two or more endpoint addresses
  * in communication. The JccConnection object describes the state of
  * each of these endpoint addresses with respect to the JccCall.
  *
  * <h5>JccCalls and JccAddresses</h5>
  * JccConnection objects are immutable in terms of their JccCall and
  * JccAddress references. In other words, the JccCall and JccAddress
  * object references do not change throughout the lifetime of the
  * JccConnection object instance. The same JccConnection object may not
  * be used in another telephone call. The existence of a JccConnection
  * implies that its JccAddress is associated with its JccCall in the
  * manner described by the JccConnection's state. <p>
  *
  * Although a JccConnection's JccAddress and JccCall references remain
  * valid throughout the lifetime of the JccConnection object, the same
  * is not true for the JccCall and JccAddress object's references to
  * this JccConnection. Particularly, when a JccConnection moves into
  * the DISCONNECTED state, it is no longer listed by the
  * JccCall.getConnections() method. Typically, when a JccConnection
  * moves into the DISCONNECTED state, the application loses its
  * references to it to facilitate its garbage collection.
  *
  * <h5>Connection States</h5>
  * Below is a description of each JccConnection state in real-world
  * terms. These real-world descriptions have no bearing on the
  * specifications of methods, they only serve to provide a more
  * intuitive understanding of what is going on. Several methods in this
  * specification state pre-conditions based upon the state of the
  * Connection. <ul>
  *
  * <li> IDLE This state is the initial state for all new Connections.
  * Connections which are in the IDLE state are not actively part of a
  * telephone call, yet their references to the Call and Address objects
  * are valid. Connections typically do not stay in the IDLE state for
  * long, quickly transitioning to other states.
  *
  * <li> DISCONNECTED This state implies it is no longer part of the
  * telephone call, although its references to Call and Address still
  * remain valid.  A Connection in this state is interpreted as once
  * previously belonging to this telephone call.
  *
  * <li> AUTHORIZE_CALL_ATTEMPT This state implies that the originating
  * or terminating terminal needs to be authorized for the call.
  *
  * <li> ADDRESS_COLLECT This state implies that more information needs
  * to be collected for the call.
  *
  * <li> ADDRESS_ANALYZE This state implies that the address needs to be
  * analyzed to determine the resources needed.
  *
  * <li> CALL_DELIVERY This state implies that the resources (such as
  * network links) needed to deliver signaling messages are being used.
  *
  * <li> ALERTING This state implies that the Address is being notified
  * of an incoming call.
  *
  * <li> CONNECTED This state implies that a Connection and its Address
  * is actively part of a telephone call. In common terms, two people
  * talking to one another are represented by two Connections in the
  * CONNECTED state.
  *
  * <li> FAILED This state indicates that a Connection to that end of
  * the call has failed for some reason. One reason why a Connection
  * would be in the FAILED state is because the party was busy.
  * Connections that are in the FAILED state are still connected to the
  * call. </ul>
  *
  * <h5>Connection State Transitions</h5>
  * Consider the transitions given below as the ones that would be
  * visible to the application developer in the package considered. This
  * may be either because there is a method in that class/interface that
  * provides that behavior (read transition) or because of events in the
  * underlying platform. Note that we are not making any statements to
  * the effect that the other transitions (ones not shown) are
  * disallowed. The application developer would have to keep in mind the
  * possibility that the "invisible" transitions can surface (become
  * visible) in inherited classes/interfaces.
  *
  * <center> JccConnection FSM </center><br>
  * <center> [connection.gif] </center><br>
  *
  * <h5>Events--Blocking and non-Blocking</h5>
  * An event is generated corresponding to every state change in the
  * finite state machine. (FSM). All events pertaining to the
  * JccConnection object are reported via the JccCallListener interface
  * on the JccCall object associated with this JccConnection. <p>
  *
  * All the events on the JccConnection are expected to be blockable. In
  * other words, after sending each event to the listener the
  * implementation can either suspend processing or continue with
  * processing. The implementation suspends processing if the event is
  * to be fired in a blocking mode and the implementation continues with
  * processing if the event is to be fired in a non-blocking mode. When
  * the implementation suspends processing, only the traversal of the
  * finite state machine by the corresponding JccConnection object is
  * suspended. All external events received for the blocked
  * JccConnection object would have to be queued and handled. Thus, the
  * finite state machine state of the corresponding JccConnection object
  * does not change when processing is suspended. <p>
  *
  * In case of a blocking event, the implementation is expected to
  * suspend processing either until the application uses a valid API
  * method call or until a timeout occurs (table 2 contains a column
  * that lists the methods that implicitly resume processing).  An
  * example of a valid API method that implicitly resumes processing is
  * release(int). Typically, the "informational" methods like
  * isBlocked(), getAddress(), etc. do not cause the resumption of call
  * processing if it had been suspended. <p>
  *
  * The listeners are expected to specify the mode in which they are to
  * be notified of the events. Note that the events are sent out only
  * when the state is reached. Hence, when processing is suspended the
  * connection is in some state. The order of event delivery, whether
  * all the notifications to be done before the blocking events are
  * reported to the listener etc, is undefined. <p>
  *
  * <h5>EventFilters</h5>
  * Listeners can be added with and without the use of EventFilters.
  * Listeners added without filters are expected to receive events in
  * the notification mode. Listeners added with filters will receive
  * only those events which are not "discarded" by the EventFilter.
  *
  * <h5>Multiple Listeners</h5>
  * Multiple listeners can also be added for the same event. In case
  * there are multiple listeners registered for an event and all of them
  * request just the notification of an event without suspension of call
  * processing the JCC implementation informs all the registered
  * listeners. On the other hand if some listeners have registered for
  * event notification and the other listeners have requested blocking,
  * the platform informs all listeners about the event while keeping the
  * call processing suspended(without changing any state). In such a
  * case, if one of the registered listeners invokes a valid API method,
  * then this specification offers no restrictions on call processing
  * resumption/suspension. In other words in such a case, the result is
  * highly dependent on the platform.
  *
  * @since 1.0
  * @version 1.2.2
  * @author Monavacon Limited
  */
public interface JccConnection {
    /** Represents the connection AUTHORIZE_CALL_ATTEMPT state. This
      * state implies that the originating or terminating terminal needs
      * to be authorized for the call. <p>
      *
      * Entry criteria An indication that the originating or terminating
      * terminal needs to be authorized for the call.  Functions: The
      * originating or terminating terminal characteristics should be
      * verified using the calling party's identity and service profile.
      * The authority/ability of the party to place the call with given
      * properties is verified. The types of authorization may vary for
      * different types of originating and terminating resources.  Exit
      * criteria: The JccConnection object exits this state on receiving
      * indication of the success or failure of the authorization
      * process. The originating JccConnection might move to the
      * ADDRESS_COLLECT state while the terminating JccConnection has to
      * move to the CALL_DELIVERY state or beyond.  Thus, the
      * terminating JccConnection cannot be either in the
      * ADDRESS_COLLECT or the ADDRESS_ANALYZE states.  */
    public static final int AUTHORIZE_CALL_ATTEMPT = 2;
    /** Represents the connection ADDRESS_COLLECT state. <p>
      *
      * Entry criteria The JccConnection object enters this state with
      * the originating party having been authorized for this call.
      * Functions: In this state the initial information package is
      * collected from the originating party. Information is examined
      * according to dialing plan to determine the end of collection.
      * No further action may be required if en bloc signaling method is
      * in use. <p>
      *
      * Exit criteria: This state is exited either because the complete
      * initial information package or dialing string has been collected
      * from the originating party or because of failure to collect
      * information or even due to reception of invalid information from
      * the caller. Timeout and abandon indications may also cause the
      * exit from this state.  */
    public static final int ADDRESS_COLLECT = 3;
    /** Represents the connection ADDRESS_ANALYZE state. <p>
      *
      * The entry criteria have changed since JCC 1.0a <p>
      *
      * Entry criteria This state is entered on the availability of
      * complete or partial initial information package/dialing string
      * from the originating party. The request (with
      * JccProvider.createEventFilterMinimunCollectedAddressLength(int,
      * int,int)) to collect a variable number of address digits and
      * report them to the application (through
      * JccConnectionListener.connectionAddressAnalyze(JccConnectionEve
      * nt))) is handled within this state. The collection of more
      * digits as requested and the reporting of received digits to the
      * application (when the digit collect criteria are met) is done in
      * this state. This action is recursive, e.g. the application could
      * ask for 3 digits to be collected and when reported, requests can
      * be done repeatedly, e.g. the application may for example request
      * first for 3 digits to be collected and when reported request
      * further digits. <p>
      *
      * Functions: The information collected is analyzed and/or
      * translated according to a dialing plan to determine routing
      * address and call type (e.g. local exchange call, transit
      * exchange call, international exchange call). <p>
      *
      * Exit criteria: This state is exited on the availability of
      * routing address. Invalid information and Abandon indications
      * also cause transition out of this state. Exception criteria such
      * as abandon, etc. will cause exit from this state.  */
    public static final int ADDRESS_ANALYZE = 4;
    /** Represents the connection CALL_DELIVERY state. <p>
      *
      * Entry criteria:  <ul>
      * <li>This state is entered on the originating side when the
      * routing address and call type are available.
      * <li>On the terminating side this state is entered when the
      * termination attempt to the address is authorized. </ul>
      *
      * Function: <ul>
      * <li>On the originating side this state involves selecting of the
      * route as well as sending an indication of the desire to set up a
      * call to the specified called party. This results in progressing
      * the Terminating Connection object.
      * <li>On the terminating side this state is involves checking the
      * busy/idle status of the terminating access and also informing
      * the terminating terminal of an incoming call. </ul>
      *
      * Exit criteria: <ul>
      * <li>This state is exited on the originating side when the
      * Terminating Connection object has started progressing.
      * <li>This state is exited on the terminating side when the
      * terminating party is being alerted or the call is accepted.
      * </ul> */
    public static final int CALL_DELIVERY = 5;
    /** The JccConnection.DISCONNECTED state implies it is no longer
      * part of the telephone call, although its references to Call and
      * Address still remain valid. <p>
      *
      * Entry criteria: This state is entered when a disconnect
      * indication is received from the corresponding party or the
      * application. <p>
      *
      * Function: The (bearer) connection to the party is disconnected
      * and depending on the incoming network connection, appropriate
      * backward signaling takes place. <p>
      *
      * Exit criteria: */
    public static final int DISCONNECTED = 0;
    /** The JccConnection.IDLE state is the initial state for all new
      * JccConnection objects. A JccConnection object in the IDLE state
      * while not yet actively participating in a call can still
      * reference a JccCall and JccAddress object. <p>
      *
      * Entry criteria Start of a new call. <p>
      *
      * Functions: Interface (line/trunk) is idled. <p>
      *
      * Exit criteria: An indication of the desire to place an outgoing
      * call or when the indication of an incoming call is received.  */
    public static final int IDLE = 1;
    /** The JccConnection.ALERTING state implies that the Address is
      * being notified of an incoming call. <p>
      *
      * Entry criteria: This state is entered when the terminating party
      * is being alerted of an incoming call. <p>
      *
      * Function: An indication is sent to the originating party that
      * the terminating party is being alerted. <p>
      *
      * Exit criteria: This state is exited when the call is accepted
      * and answered by the terminating party. Exception criteria such
      * as callrejected, NoAnswer and Abandon if possible all cause exit
      * from this state.  */
    public static final int ALERTING = 6;
    /** The JccConnection.CONNECTED state implies that originating and
      * terminating connection objects and the associated Address
      * objects are actively part of a call. <p>
      *
      * Entry criteria: <ul>
      * <li>On the originating side this state is entered after the
      * terminating connection object started progressing.
      * <li>On the terminating side this state is entered when the Call
      * is accepted and answered by the terminating party. </ul>
      *
      * Function: In this state several processes related to message
      * accounting/charging, call supervision etc. may be initiated if
      * such a capability is provided by the implementation. <p>
      *
      * Exit criteria: Exception criteria such as disconnect (and
      * suspend for JCC) cause exit from this state.  */
    public static final int CONNECTED = 7;
    /** The JccConnection.FAILED state indicates that a Connection to
      * that end of the call has failed for some reason. One reason why
      * a JccConnection would be in the FAILED state is due to the fact
      * that the party was busy. Connections that are in the FAILED
      * state are still connected to the call. <p>
      *
      * Entry criteria: This state is entered when an exception
      * condition is encountered. <p>
      *
      * Function: Default handling of the exception condition is
      * provided. <p>
      *
      * Exit criteria: Default handling of the exception condition by
      * the implementation is completed.  */
    public static final int FAILED = 9;
    /** Replaces address information onto an existing JccConnection.
      * The address replaced on a connection is what is normally thought
      * of as the destination address. Note that the address (and hence
      * JccAddress) associated with this JccConnection itself is not
      * changed. This method is used when a telephone address string has
      * been dialed and address translation is needed in order to place
      * the telephone call. The translated addressing information is
      * provided as the argument to this method. <p>
      *
      * Pre-Conditions: <ol>
      * <li>this.getState() == ADDRESS_COLLECT or ADDRESS_ANALYZE
      * <li>this.getCall().getState() == ACTIVE </ol>
      *
      * Post-Conditions: <ol>
      * <li>this.getState() == CALL_DELIVERY
      * <li>this.getCall().getState() == ACTIVE </ol>
      *
      * @param address
      * Indicates the java.lang.String representation of the translated addresses.
      * @exception MethodNotSupportedException
      * The implementation does not support this method.
      * @exception InvalidStateException
      * Some object required by this method is not in a valid state as
      * designated by the pre-conditions for this method.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception PrivilegeViolationException
      * The application does not have the proper authority to call this
      * method.
      * @exception InvalidPartyException
      * The given Addresses are not valid.
      */
    public void selectRoute(java.lang.String address)
        throws MethodNotSupportedException, InvalidStateException,
                          ResourceUnavailableException,
                          PrivilegeViolationException,
                          InvalidPartyException;
    /** Drops a JccConnection from an active telephone call. If
      * successful, the associated JccAddress will be released from the
      * call and the JccConnection moves to the DISCONNECTED state
      * following which it may be deleted. Valid cause codes (prefixed
      * by CAUSE_) for the integer that is named causeCode are defined
      * in JccEvent and JccCallEvent. The JccConnection's JccAddress is
      * no longer associated with the telephone call. This method does
      * not necessarily drop the entire telephone call, only the
      * particular JccConnection on the telephone call. This method
      * provides the ability to disconnect a specific party from a
      * telephone call, which is especially useful in telephone calls
      * consisting of three or more parties. Invoking this method may
      * result in the entire telephone call being dropped, which is a
      * permitted outcome of this method. In that case, the appropriate
      * events are delivered to the application, indicating that more
      * than just a single JccConnection has been dropped from the
      * telephone call. As a result of this method returning
      * successfully, a JccConnectionEvent.CONNECTION_DISCONNECTED event
      * for this JccConnection is delivered to the registered
      * listeners.<p>
      *
      * <h5>Dropping Additional Connections</h5>
      * Additional JccConnections may be dropped indirectly as a result
      * of this method. For example, dropping the destination
      * JccConnection of a two-party call may result in the entire
      * telephone call being dropped. It is up to the implementation to
      * determine which JccConnections are dropped as a result of this
      * method. Implementations should not, however, drop additional
      * JccConnections representing additional parties if it does not
      * reflect the natural response of the underlying telephone
      * hardware. <p>
      *
      * Pre-conditions: <ol>
      * <li>this.getState() != IDLE or DISCONNECTED
      * <li>((this.getCall()).getProvider()).getState() == IN_SERVICE
      * <li>(this.getCall()).getState() == ACTIVE</ol>
      *
      * Post-conditions: <ol>
      * <li>this.getState() == DISCONNECTED
      * <li>((this.getCall()).getProvider()).getState() == IN_SERVICE
      * <li>CONNECTION_DISCONNECTED event is delivered for to the
      * registered listeners.
      * <li>CALL_INVALID event is also delivered if all the
      * JccConnections are dropped indirectly as a result of this
      * method.</ol>
      *
      * @param causeCode
      * An integer that represents a cause code.  Valid values are
      * defined in JccEvent and JccCallEvent, they are typically
      * prefixed by CAUSE_.
      * @exception InvalidStateException
      * If either of the JccConnection, JccCall or JccProvider objects
      * is not in the proper states as given by this method's
      * precondition.
      * @exception PrivilegeViolationException
      * The application does not have the authority or permission to
      * disconnect the JccConnection. For example, the JccAddress
      * associated with this JccConnection may not be controllable in
      * the JccProvider's domain.
      * @exception ResourceUnavailableException
      * An internal resource to drop the connection is not available.
      * @exception InvalidArgumentException
      * The given release cause code is invalid.
      * @since 1.0a */
    public void release(int causeCode)
        throws PrivilegeViolationException,
                          ResourceUnavailableException,
                          InvalidStateException,
                          InvalidArgumentException;
    /** This method causes the call to be answered. <p>
      *
      * This method can only be invoked on a terminating connection
      * object. <p>
      *
      * Pre-conditions: <ol>
      * <li>getAddress() != getOriginatingAddress()
      * <li>this.getState() == CALL_DELIVERY or ALERTING
      * <li>((this.getCall()).getProvider()).getState() == IN_SERVICE
      * <li>(this.getCall()).getState() == ACTIVE</ol>
      *
      * Post-conditions: <ol>
      * <li>this.getState() == CONNECTED
      * <li>((this.getCall()).getProvider()).getState() == IN_SERVICE
      * <li>CONNECTION_CONNECTED event is delivered for to the
      * registered listeners.
      * <li>(this.getCall()).getState() == ACTIVE </ol>
      *
      * @exception MethodNotSupportedException
      * The implementation does not support this method.
      * @exception PrivilegeViolationException
      * This could include trying to answer an outgoing call leg in a
      * case where it is leaving the domain of the local call control
      * platform.
      * @exception ResourceUnavailableException
      * An internal resource to answer the connection is not available.
      * @exception InvalidStateException
      * If either of the JccConnection, JccCall or JccProvider objects
      * is not in the proper states as given by this method's
      * precondition.  */
    public void answer()
        throws PrivilegeViolationException,
                          ResourceUnavailableException,
                          InvalidStateException,
                          MethodNotSupportedException;
    /** This method requests the platform to continue processing. The
      * call processing has been suspended due to the firing of a
      * blocking event (trigger) and this method causes the processing
      * to continue. <p>
      *
      * Pre-conditions: <ol>
      * <li>this.isBlocked() == true
      * <li>((this.getCall()).getProvider()).getState() == IN_SERVICE
      * <li>(this.getCall()).getState() == ACTIVE </ol>
      *
      * Post-conditions: <ol>
      * <li>((this.getCall()).getProvider()).getState() == IN_SERVICE
      * <li>(this.getCall()).getState() == ACTIVE </ol>
      *
      * @exception PrivilegeViolationException
      * The application does not have the authority or permission to
      * invoke this methods.
      * @exception ResourceUnavailableException
      * An internal resource is not available.
      * @exception InvalidStateException
      * If either of the JccConnection, JccCall or JccProvider objects
      * is not in the proper states as given by this method's
      * precondition.  */
    public void continueProcessing()
        throws PrivilegeViolationException,
                          ResourceUnavailableException,
                          InvalidStateException;
    /** This method will allow transmission on all associated bearer
      * connections or media channels to and from other parties in the
      * call. The JccConnection object must be in the CONNECTED state
      * for this method to complete successfully. <p>
      *
      * Pre-conditions: <ol>
      * <li>this.getState() == CONNECTED
      * <li>((this.getCall()).getProvider()).getState() == IN_SERVICE
      * <li>(this.getCall()).getState() == ACTIVE </ol>
      *
      * Post-conditions: <ol>
      * <li>this.getState() == CONNECTED
      * <li>((this.getCall()).getProvider()).getState() == IN_SERVICE
      * <li>(this.getCall()).getState() == ACTIVE</ol>
      *
      * @exception PrivilegeViolationException
      * The application does not have the authority or permission to
      * invoke this methods.
      * @exception ResourceUnavailableException
      * An internal resource is not available.
      * @exception InvalidStateException
      * If either of the JccConnection, JccCall or JccProvider objects
      * is not in the proper states as given by this method's
      * precondition.  */
    public void attachMedia()
        throws PrivilegeViolationException,
                          ResourceUnavailableException,
                          InvalidStateException;
    /** This method will detach the JccConnection from the call, i.e.,
      * this will prevent transmission on any associated bearer
      * connections or media channels to and from other parties in the
      * call. The JccConnection object must be in the CONNECTED state
      * for this method to complete successfully. <p>
      *
      * Pre-conditions: <ol>
      * <li>this.getState() == CONNECTED
      * <li>((this.getCall()).getProvider()).getState() == IN_SERVICE
      * <li>(this.getCall()).getState() == ACTIVE</ol>
      *
      * Post-conditions:<ol>
      * <li>this.getState() == CONNECTED
      * <li>((this.getCall()).getProvider()).getState() == IN_SERVICE
      * <li>(this.getCall()).getState() == ACTIVE</ol>
      *
      * @exception PrivilegeViolationException
      * The application does not have the authority or permission to
      * invoke this methods.
      * @exception ResourceUnavailableException
      * An internal resource is not available.
      * @exception InvalidStateException
      * If either of the JccConnection, JccCall or JccProvider objects
      * is not in the proper states as given by this method's
      * precondition.  */
    public void detachMedia()
        throws PrivilegeViolationException,
                          ResourceUnavailableException,
                          InvalidStateException;
    /** Returns a boolean value indicating if the JccConnection is
      * currently blocked due to a blocking event having been fired to a
      * listener registered for that blocking event. The method returns
      * false once a valid API call is made after the firing of a
      * blocking event or until after the expiry of a timeout.
      * @return
      * Boolean indicating if the connection is blocked due to a
      * blocking event.  */
    public boolean isBlocked();
    /** Returns the last redirected address associated with this
      * JccConnection. The last redirected address is the address at
      * which the current JccCall was placed immediately before the
      * current address. This is common if a JccCall is forwarded to
      * several addresses before being answered. If the last redirected
      * address is unknown or not yet known, this method returns null.
      * @return
      * The address to which the call was last associated and
      * redirection on which caused the current Address to be associated
      * with the call through the connection.  */
    public java.lang.String getLastAddress();
    /** Returns the original address associated with this JccConnection.
      * This would be the first address to which the call was placed.
      * The current address might be different from this due to multiple
      * forwardings. If this address is unknown or not yet known, this
      * method returns null.
      * @return
      * The address which was called initially.  */
    public java.lang.String getOriginalAddress();
    /** Returns the address of the destination party. This method will
      * return the address of the destination party when invoked upon an
      * originating connection and only if the connections state
      * (getState()) is either in ADDRESS_COLLECT, ADDRESS_ANALYZE, or
      * CALL_DELIVERY. It will return null otherwise.
      * @return
      * The address of the destination party.  */
    public java.lang.String getDestinationAddress();
    /** Returns the address of the originating party. This method will
      * return the address of the originating party when invoked upon a
      * terminating connection. If there is no originating party the
      * method will return null. If the method is invoked on an
      * originating connection object, the method's return value will
      * equal the return value of getAddress().
      * @return
      * The address of the originating party (if any).  */
    public JccAddress getOriginatingAddress();
    /** Returns the redirected address. Only after the event with id.
      * CONNECTION_DISCONNECTED with cause code CAUSE_REDIRECTED occured
      * and the connection returned by
      * JccConnectionEvent.getConnection() is this and it is a
      * terminating connection, this method will return the address of
      * the party to which this connection is redirected. In all other
      * cases this method returns null.
      * @return
      * The address to which the call is redirected or null if the call
      * is not redirected.
      * @since 1.0a */
    public java.lang.String getRedirectedAddress();
    /** Routes this JccConnection to the target address associated with
      * this JccConnection object. <p>
      *
      * Pre-Conditions:<ol>
      * <li>this.getState() == IDLE or AUTHORIZE_CALL_ATTEMPT
      * <li>this.getCall().getState() == ACTIVE</ol>
      *
      * Post-Conditions:<ol>
      * <li>this.getState() == AUTHORIZE_CALL_ATTEMPT (at least)
      * <li>this.getCall().getState() == ACTIVE</ol>
      *
      * @param attachmedia
      * Indicates if the media has to be attached after the connection
      * is routed. TRUE causes the media to be attached, FALSE causes
      * the media not to be attached in which case a separate call to
      * attachMedia() must be made in order to attach the media to this
      * connection.
      * @exception InvalidStateException
      * Some object required by this method is not in a valid state as
      * designated by the pre-conditions for this method.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception PrivilegeViolationException
      * The application does not have the proper authority to call this
      * method.
      * @exception MethodNotSupportedException
      * The implementation does not support this method.
      * @exception InvalidPartyException
      * The given Addresses are not valid.
      * @exception InvalidArgumentException
      * The provided argument is not valid.  */
    public void routeConnection(boolean attachmedia)
        throws InvalidStateException, ResourceUnavailableException,
                          PrivilegeViolationException,
                          MethodNotSupportedException,
                          InvalidPartyException,
                          InvalidArgumentException;
    /** This methods gives access to service code type and service code
      * value. It can be invoked after the event with id
      * JccConnectionEvent.CONNECTION_MID_CALL occured. <p>
      *
      * Pre-Conditions:<ol>
      * <li>this.getState() == CONNECTED
      * <li>this.getCall().getState() == ACTIVE</ol>
      *
      * Post-Conditions:<ol>
      * <li>this.getState() == CONNECTED
      * <li>this.getCall().getState() == ACTIVE</ol>
      * @return
      * The mid call data; the service code type and service code value.
      * @exception InvalidStateException
      * Some object required by this method is not in a valid state as
      * designated by the pre-conditions for this method.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable,
      * e.g. no mid call data is available at this time.
      * @exception MethodNotSupportedException
      * The implementation does not support this method.  */
    public MidCallData getMidCallData()
        throws InvalidStateException, ResourceUnavailableException,
                          MethodNotSupportedException;
    /** Retrieves the state of the JccConnection object.
      * @return
      * Integer representing the state of the call. See static int's
      * defined in this object.  */
    public int getState();
    /** Retrieves the Jcccall that is associated with this
      * JccConnection. This JccCall reference remains valid throughout
      * the lifetime of the JccConnection object despite the state of
      * the JccConnection object. This JccCall reference does not change
      * once the JccConnection object has been created.
      * @return
      * JccCall object holding this connection.  */
    public JccCall getCall();
    /** Returns the JccAddress associated with this JccConnection. This
      * JccAddress object remains valid throughout the lifetime of the
      * JccConnection object despite the state of the JccConnection
      * object. This JccAddress reference does not change once the
      * JccConnection object has been created.
      * @return
      * JccAddress object associated with this JccConnection object.  */
    public JccAddress getAddress();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
