/*
 @(#) src/java/javax/jcat/JcatTerminalConnection.java <p>
 
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

package javax.jcat;

import javax.csapi.cc.jcc.*;

/**
  * A JcatTerminalConnection object maintains a state that reflects the relationship
  * between a JcatTerminal and a JcatConnection. A JcatTerminalConnection object state is
  * distinct from the JcatConnection object states. The JcatConnection object states
  * describe the relationship between an entire JcatAddress endpoint and a JcatCall,
  * whereas the JcatTerminalConnection state describes the relationship between one of the
  * JcatTerminal objects at the endpoint JcatAddress on the JcatCall with respect to its
  * JcatConnection.  Different JcatTerminals on a JcatCall, which are associated with the
  * same, JcatConnection may be in different states. Furthermore, the state of the
  * JcatTerminalConnection has a dependency and specific relationship to the state of its
  * JcatConnection. <p>
  *
  * The TerminalConnection object has the following states: <dl>
  *
  * <dt>IDLE
  * <dd>This state is the initial state for a JcatTerminalConnection object. A
  * JcatTerminalConnection object does not stay in the IDLE state for long, quickly
  * transitioning to other states.
  *
  * <dt>RINGING
  * <dd>This state indicates the JcatTerminal is ringing and that the JcatTerminal has an
  * incoming call.
  *
  * <dt>DROPPED
  * <dd>This state indicates that a particular JcatTerminal has permanently left the call.
  * When a JcatTerminalConnection object moves into the DROPPED state, it is no longer
  * associated with its JcatConnection and JcatTerminal objects. That is, both of these
  * objects lose their references to the JcatTerminalConnection. However, the
  * JcatTerminalConnection still maintains its references to the JcatConnection and
  * JcatTerminal object for application reference.
  *
  * <dt>BRIDGED
  * <dd>This state indicates that a JcatTerminal is currently bridged into a Call. A
  * JcatTerminal may typically join a JcatCall when it is bridged. A bridged JcatTerminal
  * is part of the call, but not active.  Typically, some hardware resource is occupied
  * while a JcatTerminal is bridged into a JcatCall.
  *
  * <dt>TALKING
  * <dd>This state indicates that the JcatTerminal is actively part of a JcatCall, is
  * typically "off-hook", and the party is communicating on the call.
  *
  * <dt>INUSE
  * <dd>This state indicates that a JcatTerminal hardware resource is currently in use.
  * The terminal is not available for the JcatCall associated with this
  * JcatTerminalConnection, that is the JcatTerminal may not join the call. This state is
  * similar to the BRIDGED state except that the JcatTerminal may not join the JcatCall.
  *
  * <dt>HELD
  * <dd>This state indicates that a JcatTerminal is part of a JcatCall, but is on hold.
  * Other JcatTerminal objects, which are on the same JcatCall and associated with the
  * same JcatConnection may or may not also be in this state. </dl>
  *
  * <h5>TerminalConnection object state transition diagram </h5>
  *
  * The figure below illustrates the finite-state diagram for a JcatTerminalConnection
  * object. The finite-states describe the allowable state transitions of a
  * JcatTerminalConnection object. The API must guarantee these state transitions. The
  * specifications of operations, which affect the state of the JcatTerminalConnections,
  * also obey these state transitions. The JcatTerminalConnection object may transition
  * into the DROPPED state from any state. The JcatTerminalConnection object in the IDLE
  * or RINGING state can transition into any other state. <p>
  *
  * <center>JcatTerminalConnection FSM</center><br>
  * <center>[jcat_termconnection.gif]</center><p>
  *
  * The following table defines the specific relationship between JcatConnection states
  * and JcatTerminalConnection states.
  *
  * <table><tr>
  * <td>JcatConnection State</td>
  * <td>JcatTerminalConnectionState</td></tr><tr>
  * <td>JccConnection.IDLE</td>
  * <td>Zero or more JcatTerminalConnection objects may exist for this JcatConnection, and
  *     each must be in the IDLE state.  JccConnection.AUTHORIZE_CALL_ATTEMPT,
  *     JccConnection.ADDRESS_COLLECT, JccConnection.ADDRESS_ANALYZE,
  *     JccConnection.CALL_DELIVERY Zero or more JcatTerminalConnection objects may exist
  *     for this JcatConnection, and each must be in the IDLE state.</td></tr><tr>
  * <td>JccConnection.ALERTING</td>
  * <td>Zero or more JcatTerminalConnections may exist for this JcatConnection, and each
  *     must be in the RINGING state.</td></tr><tr>
  * <td>JccConnection.CONNECTED</td>
  * <td>Zero or more JcatTerminalConnections may exist for this JcatConnection, and each
  *     must be in the BRIDGED or INUSE state (passive) or in the TALKING or HELD (active)
  *     state. Note that when JcatTerminalConnections transition into the DROPPED state
  *     they are no longer associated with the JcatConnection.</td></tr><tr>
  * <td>JccConnection.FAILED</td>
  * <td>No TerminalConnections may exist for this Connection. Note that all
  *     TerminalConnections previously associated with this Connection will move into the
  *     DROPPED state.</td></tr><tr>
  * <td>JcatConnection.SUSPENDED</td>
  * <td>Zero or more JcatTerminalConnections may exist for this JcatConnection, and each
  *     must be in the HELD state.</td></tr><tr>
  * <td>JccConnection.DISCONNECTED</td>
  * <td>Zero or more JcatTerminalConnections may exist for this JcatConnection, and each
  *     must be in the DROPPED state.</td></tr></table>
  *
  * @since 1.0
  * @author Monavacon Limited
  * @version 1.2.2
  */
public interface JcatTerminalConnection {
    /** This state is the initial state for a JcatTerminalConnection object.  */
    public static final int IDLE = 1;
    /** This state indicates the JcatTerminal is ringing and that the JcatTerminal has an
      * incoming JcatCall.  */
    public static final int RINGING = 2;
    /** This state indicates that a particular JcatTerminal has permanently left the call.
      * */
    public static final int DROPPED = 3;
    /** This state indicates that a JcatTerminal is currently bridged into a Call.  */
    public static final int BRIDGED = 4;
    /** This state indicates that the JcatTerminal is actively part of a JcatCall.  */
    public static final int TALKING = 5;
    /** This state indicates that a JcatTerminal hardware resource is currently in use.
      * */
    public static final int INUSE = 6;
    /** This state indicates that a JcatTerminal is part of a JcatCall, but is on hold.
      * */
    public static final int HELD = 7;
    /**
      * Answers an incoming telephone call on this Terminal Connection.  This operation
      * waits (i.e. the invoking thread blocks) until the telephone call has been answered
      * at the endpoint before returning. When this operation returns successfully, the
      * state of this Terminal Connection object is TALKING.
      *
      * <h5>Allowable Terminal Connection States</h5>
      *
      * The Terminal Connection must be in the RINGING state when this operation is
      * invoked. This implies that the associated Connection object is in the
      * JccConnection.ALERTING state. There may be more than one Terminal Connection on
      * the Connection which are in the RINGING state.
      *
      * <h5>Multiple Terminal Connections</h5>
      *
      * The underlying telephone hardware determines the resulting state of other Terminal
      * Connection objects after the telephone call has been answered by one of the
      * Terminal Connections. The other Terminal Connection object may either move into
      * the BRIDGED or INUSE state or the DROPPED state. If a Terminal Connection moves
      * into the BRIDGED or INUSE state, it remains part of the telephone call, but not
      * actively so. It may have the ability to join the call in the future. If a Terminal
      * Connection moves into the DROPPED state, it is removed from the telephone call.
      * The appropriate events are delivered to the application indicates into which of
      * these two states the other Terminal Connection objects have moved.
      *
      * <h5>Events</h5>
      *
      * The following events are reported to applications via the
      * JcatTerminalConnectionListener and JccConnectionListener interface as a result of
      * the successful outcome of this operation: <ol>
      *
      * <li>JcatTerminalConnectionEvent.TERMINALCONNECTION_TALKING for the Terminal
      *     Connection which invoked this operation.
      *
      * <li>JccConnectionEvent.CONNECTION_CONNECTED for the Connection associated with the
      *     Terminal Connection.
      *
      * <li>JcatTerminalConnectionEvent.TERMINALCONNECTION_BRIDGED,
      *     JcatTerminalConnectionEvent.TERMINALCONNECTION_INUSE or
      *     JcatTerminalConnectionEvent.TERMINALCONNECTION_DROPPED for other Terminal
      *     Connections associated with the Connection. </ol>
      */
    public void answer();
    /**
      * Returns the JcatConnection object associated with this JcatTerminalConnection. A
      * JcatTerminalConnection's reference to the JcatConnection remains valid throughout
      * the lifetime of the JcatTerminalConnection. Also, this reference does not change
      * once the JcatTerminalConnection object has been created.
      *
      * @return JcatConnection object associated with this JcatTerminalConnection.
      */
    public JcatConnection getConnection();
    /**
      * Returns the state of the JcatTerminalConnection object.
      *
      * @return state of the Terminal Connection object.
      */
    public int getState();
    /**
      * Returns the JcatTerminal associated with this JcatTerminalConnection object. A
      * terminal connection's reference to its terminal remains valid for the lifetime of
      * the object, even if the terminal loses its references to this terminal connection
      * object. Also, this reference does not change once the JcatTerminalConnection
      * object has been created.
      *
      * @return JcatTerminal associated with this JcatTerminalConnection object.
      */
    public JcatTerminal getTerminal();
    /**
      * Places a JcatTerminalConnection on hold with respect to the JcatCall of which it
      * is a part. Many JcatTerminals may be on the same JcatCall and associated with the
      * same JcatConnection.  Any one of them may go "on hold" at any time, provided they
      * are "active" in the Call. The JcatTerminalConnection must be in the TALKING state.
      * This operation returns when the Terminal Connection has moved to the HELD state,
      * or until an error occurs and an exception is thrown.
      *
      * @exception InvalidStateException
      * @exception MethodNotSupportedException
      * @exception PrivilegeViolationException
      * @exception ResourceUnavailableException
      *
      * @see JcatCall
      */
    public void hold()
        throws InvalidStateException, MethodNotSupportedException,
                          PrivilegeViolationException, ResourceUnavailableException;
    /**
      * Makes a currently bridged JcatTerminalConnection active on a JcatCall. Other
      * terminals, which share the same JcatAddress as this JcatTerminal, may be active on
      * the same JcatCall. This situation is known as bridging. The JcatTerminalConnection
      * must be in the BRIDGED state. This operation returns when the
      * JcatTerminalConnection has moved to the TALKING state or until an error occurs and
      * an exception is thrown.
      *
      * @exception InvalidStateException
      * @exception MethodNotSupportedException
      * @exception PrivilegeViolationException
      * @exception ResourceUnavailableException
      *
      * @see JcatCall
      */
    public void join()
        throws InvalidStateException, MethodNotSupportedException,
                          PrivilegeViolationException, ResourceUnavailableException;
    /**
      * Places a currently active JcatTerminalConnection in a bridged state on a JcatCall.
      * Other terminals, which share the same JcatAddress as this JcatTerminal, may remain
      * active on the same JcatCall. This situation where JcatTerminals share a
      * JcatAddress on a call is known as bridging. The terminal connection on which this
      * operation is invoked must be in the TALKING state. There are two possible outcomes
      * of this operation depending upon the number of remaining, active
      * JcatTerminalConnection objects on the Call. If there are other active terminal
      * connections, then this JcatTerminalConnection moves into the BRIDGED state and
      * this operation returns. If there are no other active JcatTerminalConnection
      * objects, then this JcatTerminalConnection moves into the DROPPED state. Its
      * associated JcatConnection moves into the JccConnection.DISCONNECTED state, i.e.
      * the entire endpoint leaves the telephone call. This operation waits until one of
      * these two outcomes occurs or until an error occurs and an exception is thrown.
      *
      * @exception InvalidStateException
      * @exception MethodNotSupportedException
      * @exception PrivilegeViolationException
      * @exception ResourceUnavailableException
      */
    public void leave()
        throws InvalidStateException, MethodNotSupportedException,
                          PrivilegeViolationException, ResourceUnavailableException;
    /**
      * Takes a JcatTerminalConnection off hold with respect to the JcatCall of which it
      * is a part. Many JcatTerminal objects may be on the same JcatCall and associated
      * with the same JcatConnection. Any one of them may go "on hold" at any time,
      * provided they are active in the JcatCall. The JcatTerminalConnection must be in
      * the HELD state. This operation returns successfully when the
      * JcatTerminalConnection moves into the TALKING state or until an error occurs and
      * an exception is thrown.
      *
      * @exception InvalidStateException
      * @exception MethodNotSupportedException
      * @exception PrivilegeViolationException
      * @exception ResourceUnavailableException
      */
    public void unhold()
        throws InvalidStateException, MethodNotSupportedException,
                          PrivilegeViolationException, ResourceUnavailableException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
