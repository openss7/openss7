/*
 @(#) src/java/javax/jcat/JcatConnection.java <p>
 
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
  * The JcatConnection interface extends the JccConnection interface.  While the
  * JccConnection interface specified the relationship between the JccCall and JccAddress
  * objects, JcatConnection is also associated additionally with a JcatTerminal (through
  * the JcatTerminalConnection object). This is expected to enable advanced call control
  * services which require multiple terminals per address or multiple addresses per
  * terminal. These capabilities would be needed given the potential of devices today such
  * as the PDA (considered as a terminal), which can handle voice calls as VOIP as well as
  * using the mobile voice networks (each of which requires separate addresses). <p>
  *
  * Thus, each JcatConnection object is associated directly with a JcatCall, a JcatAddress
  * and a JcatTerminalConnection object. The JcatConnection object has a finite state
  * machine associated with it. A JcatConnection object maintains a state that reflects
  * the relationship between a JcatCall and a JcatAddress as well as between the JcatCall
  * and the JcatTerminal (through the JcatTerminalConnection object). The JcatConnection
  * state is important to the application because it indicates a logical view to the
  * application. The application can take specific actions on the JcatConnection object
  * when it is in a specific state and specific events have been reported to the
  * application. <p>
  *
  * In each state, the connection object can detect certain events, which can be reported
  * to an application. As in the case of a JccConnection object, the same JcatConnection
  * object may not be used in another call. The existence of a JcatConnection implies that
  * its address is associated with its call in the manner described by the
  * JcatConnection's state. In addition, this also implies that the JcatTerminalConnection
  * is associated with the JcatCall object. <p>
  *
  * While a single FSM is specified for the JcatCall, the states traversed by a particular
  * endpoint depends on the role of the endpoint --whether the endpoint originated the
  * call or whether the endpoint is receiving a call. Based on this, we consider two types
  * of connection objects, namely origintaing connection (O-Connection) and terminating
  * connection (T-Connection) objects. An O-Connection object represents an association
  * between a call object and an originating endpoint represented by a JcatAddress object.
  * A T-Connection object represents an association between a call object and a
  * terminating address object.
  *
  * <h5>Connection object state transition diagram</h5>
  *
  * The figure below illustrates the finite-state diagram for the JcatConnection object.
  * The finite-states describe the allowable state transitions of an O-Connection or
  * T-Connection object. The API must guarantee these state transitions.
  *
  * Note that this state machine is a refinement of the JccConnection FSM.  The CONNECTED
  * state of JccConnection is divided into a CONNECTED state and a SUSPENDED state. Since
  * the states defined in the JcatConnection interface provide more detail to the states
  * defined in the JccConnection interface, each state in the JccConnection interface
  * corresponds to a state defined in the JcatConnection interface.  Conversely, each
  * JcatConnection state corresponds to exactly one JccConnection state. This arrangement
  * permits applications to view either the JccConnection state or the JcatConnection
  * state and still see a consistent view. <p>
  *
  * <center>JcatConnection FSM</center><br>
  * <center>[jcat_connection.gif]</center>
  * @since 1.0
  * @author Monavacon Limited
  * @version 1.2.2
  */
public interface JcatConnection extends JccConnection {
    /**
      * The SUSPENDED state implies that an JcatConnection object is suspended from the
      * call, although its references to the call and address objects will still remain
      * valid. <p>
      *
      * Entry criteria: <ul>
      *
      * <li>A suspend indication is received if the terminating party has disconnected
      *     (but disconnect timing has not completed). For example, through on-hook or
      *     through suspendConnection(). </ul>
      *
      * Function: The connections for the originating and terminating party are maintained
      * and depending on the incoming network connection, appropriate backward signaling
      * takes place. <p>
      *
      * According to the received indication the following applies: <ul>
      *
      * <li>A suspend indication is sent to the Connection object.
      * <li>For an SS7-supported trunk or an ISDN interface, when a disconnect indication
      *     (e.g., SS7 RELease message) is received from the terminating party, this state
      *     is immediately exited to the DISCONNECTED state without any action. As an
      *     option, the call can be continued for an appropriate period in order to offer
      *     follow-on initiated by JccConnectionEvent.CONNECTION_MID_CALL. </ul>
      *
      * In the following cases, the timer is started and the call waits for re-answer
      * request from the terminating party: <ul>
      *
      * <li>For an SS7 supported trunk, in case of receiving network initiated suspend
      *     message.
      * <li>For an analog interface, in case of detecting on-hook. </ul>
      *
      * If re-answer request (e.g. off-hook, SS7 RESume message) is received from the
      * terminating party before the timer expires, the originating and terminating
      * parties are reconnected.  Exit criteria:  <ul>
      *
      * <li>The terminating party re-answers or a reconnect() message is received before
      *     the disconnect timer expires. The Connection object returns to the CONNECTED
      *     state.
      * <li>A service feature request is received from the originating party, e.g. hook
      *     flash, ISDN feature activator of facility or a new reply from calling party.
      * <li>A disconnect indication is received from the remaining party in the call.
      * <li>A disconnect indication is received from the suspended party (e.g.
      *     JccConnection.release(int)). </ul>
      */
    public static final int SUSPENDED = 1;
    /**
      * Returns a set of JcatTerminalConnection objects associated with this
      * JcatConnection. JcatTerminalConnection objects represent the relationship between
      * a JcatConnection and a specific JcatTerminal endpoint. There may be zero
      * JcatTerminalConnections associated with this JcatConnection. In that case, this
      * method returns an empty set. JcatConnection objects lose their reference to a
      * JcatTerminalConnection once the JcatTerminalConnection moves into the
      * JcatTerminalConnection.DROPPED state.
      *
      * @return
      * Returns a set with JcatTerminalConnection objects.
      */
    public java.util.Set getTerminalConnections();
    /**
      * Parks a Connection at a destination telephone address. This method is similar to
      * the transfer feature, except the JcatConnection at the new destination Address is
      * in the CALL_DELIVERY state or beyond. Parking a JcatConnection at a destination
      * Address drops the JcatConnection from the JcatCall and creates and returns a new
      * JcatConnection at the specified destination address. <p>
      *
      * The new destination telephone address string is given as an argument to this
      * method and must be a valid and complete telephone address. The new JcatConnection
      * must be in the CONNECTED state. If this method is successful, the old (this)
      * JcatConnection moves to the DISCONNECTED state. All of its associated
      * JcatTerminalConnections move to the DROPPED state. <p>
      *
      * Pre-Conditions: <ol>
      *
      * <li>((this.getCall()).getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>this.getState() == JccConnection.CONNECTED
      * <li>destinationAddress must be a valid and complete telephone address. </ol>
      *
      * Post-Conditions: <ol>
      *
      * <li>Let newconnection be the new JccConnection created and returned
      * <li>((this.getCall()).getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>newconnection element of (this.getCall()).getConnections()
      * <li>newconnection.getState() == JcatConnection.SUSPENDED
      * <li>this.getState() == JccConnection.DISCONNECTED
      * <li>Let tc[] = this.getTerminalConnections() before this method is invoked
      * <li>tc[i].getState() == JcatTerminalConnection.DROPPED, for all i </ol>
      *
      * @param destinationAddress
      * This parameter contains the destination telephone address at which to park a
      * connection.
      *
      * @return
      * The JcatConnection associated with the new incoming call.
      *
      * @exception InvalidPartyException
      * The specified destination address provided is not valid.
      *
      * @exception InvalidStateException
      *
      * @exception MethodNotSupportedException
      * This method is not supported by the implementation.
      *
      * @exception PrivilegeViolationException
      * The application does not have the proper authority to invoke this method.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for the successful invocation of this method is not
      * available.
      */
    public JcatConnection park(java.lang.String destinationAddress)
        throws InvalidPartyException, InvalidStateException, MethodNotSupportedException,
                          PrivilegeViolationException, ResourceUnavailableException;
    /**
      * The application can use the reconnect method in the SUSPENDED state. <p>
      *
      * Pre-Conditions: <ol>
      *
      * <li>((this.getCall()).getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>(this.getCall()).getState() == JccCall.ACTIVE
      * <li>this.getState() == JccConnection.SUSPENDED
      * <li>tc[0].getState == JcatTerminalConnection.HELD </ol>
      *
      * Post-Conditions: <ol>
      *
      * <li>((this.getCall()).getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>(this.getCall()).getState() == JccCall.ACTIVE
      * <li>this.getState() == JccConnection.CONNECTED
      * <li>tc[0].getState == JcatTerminalConnection.TALKING
      * <li>JcatTerminalConnectionEvent.TERMINALCONNECTION_TALKING is delivered for
      * JcatTerminalConnection </ol>
      *
      * @exception InvalidArgumentException
      * The ReconnectParameters provided is not valid.
      *
      * @exception InvalidStateException
      * Either the Provider was not "in service" or the Call was not "idle".
      *
      * @exception MethodNotSupportedException
      * This method is not supported by the implementation.
      *
      * @exception PrivilegeViolationException
      * The application does not have the proper authority to invoke this method.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for the successful invocation of this method is not
      * available.
      *
      */
    public void reconnect()
        throws InvalidArgumentException, InvalidStateException,
                          MethodNotSupportedException, PrivilegeViolationException,
                          ResourceUnavailableException;
    /**
      * This method suspends causes a transition to the SUSPENDED state. <p>
      *
      * Pre-Conditions: <ol>
      *
      * <li>((this.getCall()).getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>(this.getCall()).getState() == JccCall.ACTIVE
      * <li>this.getState() == JccConnection.CONNECTED </ol>
      *
      * Post-Conditions: <ol>
      *
      * <li>((this.getCall()).getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>(this.getCall()).getState() == JccCall.ACTIVE
      * <li>this.getState() == JccConnection.SUSPENDED </ol>
      *
      * @exception InvalidStateException
      * Either the Provider is not "in service", the Call is not "active" or the
      * Connection is not in the "select facility" state.
      *
      * @exception MethodNotSupportedException
      * This method is not supported by the implementation.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for the successful invocation of this method is not
      * available.
      */
    public void suspendConnection()
        throws InvalidStateException, MethodNotSupportedException, ResourceUnavailableException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
