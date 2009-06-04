/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package javax.jcat;

import javax.csapi.cc.jcc.*;

/**
  * A JcatCall object extends the JccCall object . It provides advanced
  * features such as transfer, conference etc. The JcatCall object has
  * the same finite state machine as the JccCall object.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  * @since 1.0
  */
public interface JcatCall extends JccCall {
    /**
      * Adds a JcatTerminalConnectionListener to the call.
      *
      * @param termconnlistener  The JcatTerminalConnectionListener to
      * be added to the call.
      *
      * @exception MethodNotSupportedException  The implementation does
      * not support this method.
      *
      * @exception ResourceUnavailableException  The resource limit for
      * the number of listeners has been exceeded.
      */
    public void addTerminalConnectionListener(JcatTerminalConnectionListener termconnlistener)
        throws MethodNotSupportedException, ResourceUnavailableException;
    /**
      * This method transfers all participants currently on this Call,
      * with the exception of the transfer controller participant, to
      * another telephone address. This is often called a "single-step
      * transfer" because the transfer feature places another telephone
      * call and performs the transfer at one time. The telephone
      * address string given as the argument to this method must be
      * valid and complete.
      *
      * <h5>The Transfer Controller</h5>
      *
      * The transfer controller for this version of this method
      * represents the participant on this JcatCall around which the
      * transfer is taking place and who drops off the call once the
      * transfer feature has completed. The transfer controller is a
      * JcatTerminalConnection which must be in the
      * JcatTerminalConnection.TALKING state. Applications may control
      * which JcatTerminalConnection acts as the transfer controller via
      * the setTransferController(JcatTerminalConnection) method.  If no
      * transfer controller is set, the implementation chooses a
      * suitable JcatTerminalConnection when the transfer feature is
      * invoked. When the transfer feature is invoked, the transfer
      * controller moves into the JcatTerminalConnection.DROPPED state.
      * If it is the only JcatTerminalConnection associated with its
      * JcatConnection, then its JcatConnection moves into the
      * JccConnection.DISCONNECTED state as well.
      *
      * <h5>The New Connection</h5>
      *
      * This method creates and returns a new JcatConnection
      * representing the party to which the JcatCall was transferred.
      * If the JcatConnection state progressed beyond JccConnection.IDLE
      * before this method returns, appropriate events should reflect
      * this. This new JcatConnection will progress as any normal
      * destination JcatConnection on a telephone call. <p>
      *
      * Pre-Conditions: <ol>
      *
      * <li>Let tc be the transfer controller on this Call
      * <li>(this.getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>this.getState() == JccCall.ACTIVE
      * <li>tc.getState() == JcatTerminalConnection.TALKING </ol>
      *
      * Post-Conditions: </ol>
      *
      * <li>Let newconnection be the JcatConnection created and returned
      * <li>Let connection = tc.getConnection()
      * <li>(this.getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>this.getState() == JccCall.ACTIVE
      * <li>tc.getState() == JcatTerminalConnection.DROPPED
      * <li>If connection.getTerminalConnections().size() == 1, then
      *     connection.getState() == JccConnection.DISCONNECTED
      * <li>newconnection is an element of this.getConnections().
      * <li>newconnection.getState() at least JccConnection.IDLE.
      * <li>JccConnectionEvent.CONNECTION_CREATED is delivered for newconnection
      * <li>JcatTerminalConnectionEevent.TERMINAL_DROPPED is delivered for tc </ol>
      *
      * @param dialledDigits
      * The destination telephone address string to where the Call is
      * being transferred.
      *
      * @return
      * The new JcatConnection associated with the destination.
      *
      * @exception InvalidArgumentException
      * The JcatTerminalConnection provided as controlling the transfer
      * is not valid or part of this JcatCall.
      *
      * @exception InvalidStateException
      * Either the JcatProvider is not JccProvider.IN_SERVICE, the
      * JcatCall is not JccCall.ACTIVE, or the transfer controller is
      * not JcatTerminalConnection.TALKING.
      *
      * @exception InvalidPartyException
      * The destination address is not valid and/or complete.
      *
      * @exception MethodNotSupportedException
      * This method is not supported by the implementation.
      *
      * @exception PrivilegeViolationException
      * The application does not have the proper authority to invoke
      * this method.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for the successful invocation of
      * this method is not available.
      */
    public JcatConnection blindTransfer(java.lang.String dialledDigits)
        throws InvalidArgumentException, InvalidStateException,
                          InvalidPartyException,
                          MethodNotSupportedException,
                          PrivilegeViolationException,
                          ResourceUnavailableException;
    /**
      * Merges two JcatCalls together, resulting in the union of the
      * participants of both calls being placed on a single JcatCall.
      * This method takes a call as an argument, referred to hereafter
      * as the "second" call. All of the participants from the second
      * call are moved to the call on which this method is invoked.
      *
      * <h5>The Conference Controller</h5>
      *
      * In order for the conferencing feature to happen, there must be a
      * common participant to both calls, as represented by a single
      * JcatTerminal and two JcatTerminalConnections, one on each of the
      * two JcatCalls. These two JcatTerminalConnections are known as
      * the conference controllers. In the real-world, one of the two
      * calls must be on hold with respect to the controlling Terminal,
      * and hence, the JcatTerminalConnection on the second JcatCall
      * must be in the JcatTerminalConnection.HELD state. The two
      * conference controlling JcatTerminalConnections are merged into
      * one as a result of this method. Applications may control which
      * JcatTerminalConnection acts as the conference controller via the
      * setConferenceController(JcatTerminalConnection) method.  If no
      * conference controller is set, the implementation chooses a
      * suitable JcatTerminalConnection when the conferencing feature is
      * invoked.
      *
      * <h5>The Telephone Call Argument</h5>
      *
      * All of the participants from the second JcatCall, passed as the
      * argument to this method, are "moved" to the JcatCall on which
      * this method was invoked. That is, new JcatConnections and
      * JcatTerminalConnections are created on this JcatCall which are
      * found on the second JcatCall. Those JcatConnections and
      * JcatTerminalConnections on the second JcatCall are removed from
      * the JcatCall and the JcatCall moves into the JccCall.INVALID
      * state. The conference controller terminalConnections are merged
      * into one on this JcatCall. That is, the existing
      * terminalConnection controller on this JcatCall is left
      * unchanged, while the terminalConnection on the second JcatCall
      * is removed from that JcatCall.
      *
      * <h5>Other Shared Participant</h5>
      *
      * There may exist JcatAddress and JcatTerminals which are part of
      * both telephone calls in addition to the designated conference
      * controller. In these instances, those participants which are
      * shared between both JcatCalls are merged into one. That is, the
      * JcatConnections and JcatTerminalConnections on this JcatCall are
      * left unchanged. The corresponding JcatConnections and
      * JcatTerminalConnections on the second JcatCall are removed from
      * that JcatCall. <p>
      *
      * Pre-Conditions: <ol>
      *
      * <li>Let tc1 be the conference controller on this Call
      * <li>Let connection1 = tc1.getConnection()
      * <li>Let tc2 be the conference controller on otherCall
      * <li>(this.getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>this.getState() == JccCall.ACTIVE
      * <li>tc1.getTerminal() == tc2.getTerminal()
      * <li>tc1.getState() == JcatTerminalConnection.TALKING
      * <li>tc2.getState() == JcatTerminalConnection.HELD
      * <li>this != otherCall </ol>
      *
      * Post-Conditions: <ol>
      *
      * <li>(this.getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>this.getState() == JccCall.ACTIVE
      * <li>otherCall.getState() == JccCall.INVALID
      * <li>Let c[] be the JccConnections to be merged from otherCall
      * <li>Let tc[] be the JcatTerminalConnections to be merged from * otherCall
      * <li>Let new_c be the set of new JccConnections created on this * Call
      * <li>Let new_tc be the set of new JcatTerminalConnections created * on this Call
      * <li>new_c element of this.getConnections()
      * <li>new_c.getState() == c.getState()
      * <li>new_tc element of * (this.getConnections()).getTerminalConnections()
      * <li>new_tc.getState() == tc.getState()
      * <li>c[i].getState() == JccConnection.DISCONNECTED for all i
      * <li>tc[i].getState() == JcatTerminalConnection.DROPPED for all i
      * <li>Appropriate events are delivered for all new_c and new_tc </ol>
      *
      * @param othercall The JcatCall which is to be merged with the
      * existing JcatCall.
      *
      * @exception InvalidArgumentException
      * The JcatTerminalConnection provided (implicitly) as controlling
      * the transfer is not valid or part of this Call.
      *
      * @exception InvalidStateException
      * One of the objects is not in the prescribed state.
      *
      * @exception MethodNotSupportedException
      * This method is not supported by the implementation.
      *
      * @exception PrivilegeViolationException
      * The application does not have the proper authority to invoke
      * this method.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for the successful invocation of
      * this method is not available.
      */
    public void conference(JcatCall othercall)
        throws InvalidArgumentException, InvalidStateException,
                          MethodNotSupportedException,
                          PrivilegeViolationException,
                          ResourceUnavailableException;
    /**
      * Places a telephone call from an originating endpoint to a
      * destination address string. The call must be in the JccCall.IDLE
      * state (and therefore have no existing associated connections.
      * The successful effect of this method is to place the telephone
      * call and create and return at most two connections associated
      * with this call.
      *
      * <h5>Method input Arguments</h5>
      *
      * This method has three arguments. The first input argument is the
      * originating terminal for the telephone call. The second argument
      * is the originating address for the telephone Call.  This
      * terminal/address pair must reference one another. That is, the
      * originating address must appear on the terminal (via
      * JcatAddress.getTerminals() and the originating terminal must
      * appear on the address (via JcatTerminal.getAddresses()). If not,
      * an InvalidArgumentException is thrown. The third argument is a
      * destination string whose value represents the address to which
      * the telephone call is placed.
      *
      * @param term
      * Originating terminal for the telephone call.
      *
      * @param addr
      * The originating Address for this call.
      *
      * @param dialedDigits
      * The destination address string for this call.
      *
      * @return
      * An array of JcatConnection.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for placing the call is
      * unavailable.
      *
      * @exception PrivilegeViolationException
      * The application does not have the proper authority to place a
      * call.
      *
      * @exception InvalidPartyException
      * Either the originator or the destination does not represent a
      * valid party required to place a call.
      *
      * @exception InvalidStateException
      * Some object required by this method is not in a valid state as
      * designated by the pre-conditions for this method.
      *
      * @exception MethodNotSupportedException
      * The implementation does not support this method.
      */
    public JcatConnection[] connect(JcatTerminal term, JcatAddress addr, java.lang.String dialedDigits)
        throws ResourceUnavailableException,
                          PrivilegeViolationException,
                          InvalidPartyException, InvalidStateException,
                          MethodNotSupportedException;
    /**
      * Creates a consultation between this idle JcatCall and an active
      * JcatCall. A consultation JcatCall must be in the JccCall.IDLE
      * state which is associated with a particular existing JcatCall
      * and often created for a particular purpose. For example, the
      * consultation JcatCall may be used simply to "consult" with
      * another party or to conference or transfer with its associated
      * JcatCall. This method establishes a special relationship between
      * the two JcatCalls which extends down to the telephony platform
      * level. Most often, this feature is directly provided by the
      * underlying telephony platform.
      *
      * <h5>This Call Object</h5>
      *
      * The instance on which this method is invoked is used as the
      * JcatCall on which the consultation takes place and must be in
      * the JccCall.IDLE state.
      *
      * <h5>The TerminalConnection Argument</h5>
      *
      * The JcatTerminalConnection argument provides two pieces of
      * information. The first piece of information is the other active
      * JcatCall to which this idle JcatCall is associated. The call
      * associated with the JcatTerminalConnection argument must be in
      * the JccCall.ACTIVE state. The second piece of information given
      * by the JcatTerminalConnection argument is the originating
      * endpoint from which to place a telephone call on this idle
      * JcatCall. In other words, the JcatAddress and JcatTerminal
      * associated with the JcatTerminalConnection argument are used as
      * the originating endpoint for the telephone call. The state of
      * the JcatTerminalConnection must be
      * JcatTerminalConnection.TALKING and this method first moves it
      * into the JcatTerminalConnection.HELD in order to place a
      * telephone call on this idle call.
      *
      * <h5>The Destination Address String</h5>
      *
      * A telephone call is placed to the destination telephone address
      * string given as the second argument to this method.
      *
      * <h5>The Telephone Call</h5>
      *
      * A telephone call is placed on this JcatCall and an array of two
      * JcatConnections are returned representing the originating and
      * destination participants of the JcatCall. The JcatCall
      * progresses in the same way as if the JcatCall was placed using
      * the connect(JcatTerminal,JcatAddress,String) method.
      *
      * @param termconn
      * Originating terminal for the telephone call.
      *
      * @param dialedDigits
      * The destination address string for this call.
      *
      * @return
      * An array of JcatConnection.
      *
      * @exception InvalidArgumentException
      * The JcatTerminalConnection provided as controlling the transfer
      * is not valid or part of this call.
      *
      * @exception InvalidStateException
      * Some object required by this method is not in a valid state as
      * designated by the pre-conditions for this method.
      * InvalidPartyException
      *
      * @exception MethodNotSupportedException
      * This method is not supported by the implementation.
      *
      * @exception PrivilegeViolationException
      * The application does not have the proper authority to invoke
    * this method.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for the successful invocation of
      * this method is not available.
      */
    public JcatConnection[] consult(JcatTerminalConnection termconn, java.lang.String dialedDigits)
    throws InvalidArgumentException, InvalidPartyException,
                      InvalidStateException,
                      MethodNotSupportedException,
                      PrivilegeViolationException,
                      ResourceUnavailableException;
    /**
      * This method moves all participants from one JcatCall to another,
      * with the exception of a selected common participant.  This
      * method takes a JcatCall as an argument, referred to hereafter as
      * the "second" JcatCall. All of the participants, with the
      * exception for the selected common participant, from the second
      * call are moved to the JcatCall on which this method is invoked.
      *
      * <h5>The Transfer Controller</h5>
      *
      * In order for the transfer feature to happen, there must be a
      * participant that acts as the transfer controller. The transfer
      * controller is a JcatTerminalConnection around which the transfer
      * is placed. This transfer controller must be present on each of
      * the two JcatCalls and share a common JcatTerminal. The transfer
      * controller participant is no longer part of any JcatCall once
      * this transfer feature is complete. The transfer controllers on
      * each of the two JcatCalls must be in either of the
      * JcatTerminalConnection.TALKING or JcatTerminalConnection.HELD
      * state. Applications may control which JcatTerminalConnection
      * acts as the transfer controller via the
      * setTransferController(JcatTerminalConnection) method.  If no
      * transfer controller is set, the implementation chooses a
      * suitable JcatTerminalConnection when the transfer feature is
      * invoked.
      *
      * <h5>The Telephone Call Argument</h5>
      *
      * All of the participants from the second JcatCall, passed as the
      * argument to this method, are "moved" to the JcatCall on which
      * this method is invoked, with the exception of the transfer
      * controller. That is, new connections and terminal connections
      * are created on this JcatCall which are found on the second
      * JcatCall. Those JcatConnections and JcatTerminalConnections on
      * the second JcatCall are removed from the JcatCall and the
      * JcatCall moves into the JccCall.INVALID state. The transfer
      * controller JcatTerminalConnections are dropped from both Calls.
      * They move into the JcatTerminalConnection.DROPPED state.
      *
      * <h5>Other Shared Participants</h5>
      *
      * There may exist JcatAddress and JcatTerminals which are part of
      * both JcatCalls in addition to the designated transfer
      * controller. In these instances, those participants which are
      * shared between both JcatCalls are merged into one. That is, the
      * JcatConnections and JcatTerminalConnections on this JcatCall are
      * left unchanged. The corresponding JcatConnections and
      * JcatTerminalConnections on the second JcatCall are removed from
      * that JcatCall. <p>
      *
      * Pre-Conditions: <ol>
      *
      * <li>JcatTerminalConnection tc1=this.getTransferController()
      * <li>JcatTerminalConnection tc2=otherCall.getTransferController()
      * <li>this != otherCall
      * <li>(this.getProvider()).getState() == JccProvider.IN_SERVICE
      * <li>this.getState() == JcatCall.ACTIVE
      * <li>otherCall.getState() == JcatCall.ACTIVE
      * <li>tc1.getState() == JcatTerminalConnection.TALKING or JcatTerminalConnection.HELD
      * <li>tc2.getState() == JcatTerminalConnection.TALKING or JcatTerminalConnection.HELD </ol>
      *
      * Post-Conditions: <ol>
      *
      * <li>(this.getProvider()).getState() == JcatProvider.IN_SERVICE
      *
      * <li>this.getState() == JcatCall.ACTIVE
      *
      * <li>otherCall.getState() == JcatCall.INVALID
      *
      * <li>tc1.getState() == JcatTerminalConnection.DROPPED
      *
      * <li>tc2.getState() == JcatTerminalConnection.DROPPED
      *
      * <li>Let c[] be the JcatConnections to be transferred from
      *     otherCall
      *
      * <li>Let tc[] be the JcatTerminalConnections to be transferred
      *     from otherCall
      *
      * <li>Let new_c[] be the new JcatConnections created on this
      *     JcatCall and for all i, new_c[i].getAddress() ==
      *     c[i].getAddress()
      *
      * <li>Let new_tc[] be the new JcatTerminalConnections created on
      *     this JcatCall and for all i, new_tc[i].getName() ==
      *     tc[i].getName()
      *
      * <li>for all i, new_c[i].getCall().getState() ==
      *     c[i].getCall().getState()
      *
      * <li>for all i, new_tc[i].getConnection().getCall().getState() ==
      *     tc[i].getConnection().getCall().getState()
      *
      * <li>for all i, c[i].getState() == JccConnection.DISCONNECTED
      *
      * <li>for all i, tc[i].getState() ==
      *     JcatTerminalConnection.DROPPED
      *
      * <li>let JccCallEvent ce be delivered, where ce.getID() ==
      *     JccCallEvent.CALL_INVALID and ce.getCall() == otherCall
      *
      * <li>let JcatTerminalConnectionEvent tce1,tce2 be delivered,
      *     where tce1.getID() == tce2.getID() ==
      *     JcatTerminalConnectionEvent.TERMINALCONNECTION_DROPPED
      *     and tce1.getTerminalConnection() == tc1 and
      *     tce2.getTerminalConnection() == tc2
      *
      * <li>for all i, new_c[i].getState() == JccConnection.IDLE or
      *     beyond
      *
      * <li>for all i, new_tc[i].getState() ==
      *     JcatTerminalConnection.IDLE or beyond </ol>
      *
      * @param otherCall
      * The other JcatCall which to transfer to this JcatCall.
      *
      * @exception InvalidArgumentException
      * The JcatTerminalConnection controlling the transfer is not valid
      * or does not exist or the other call is not valid.
      *
      * @exception InvalidStateException
      * Either the provider is not "in service", the Call is not
      * "active", or the transfer controllers are not "talking" or
      * "held".
      *
      * @exception InvalidPartyException
      * The other Call given as the argument is not a valid Call to
      * conference with.
      *
      * @exception MethodNotSupportedException
      * This method is not supported by the implementation.
      *
      * @exception PrivilegeViolationException
      * The application does not have the proper authority to invoke
      * this method.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for the successful invocation of
      * this method is not available.
      */
    public void consultTransfer(JcatCall otherCall)
        throws InvalidArgumentException, InvalidPartyException,
                          InvalidStateException,
                          MethodNotSupportedException,
                          PrivilegeViolationException,
                          ResourceUnavailableException;
    /**
      * Returns the JcatTerminalConnection which currently acts as the
      * conference controller. The conference controller represents the
      * participant in the telephone around which a conference takes
      * place. <p>
      *
      * When a JcatCall is initially created, the conference controller
      * is set to null. This method returns non-null only if the
      * application has previously set the conference controller. If the
      * current conference controller leaves the JcatCall, the
      * conference controller is reset to null.
      *
      * @return
      * JcatTerminalConnection which acts as the conference controller.
      */
    public JcatTerminalConnection getConferenceController();
    /**
      * Return true if conferencing is enabled, false otherwise.
      * Applications may use this method initially to obtain the default
      * value set by the implementation and may attempt to change this
      * value using the setConferenceEnable(boolean) method.
      *
      * @return
      * true/false depending on whether conferencing is enabled.
      */
    public boolean getConferenceEnable();
    /**
      * Returns the JcatTerminalConnection which currently acts as the
      * transfer controller. The transfer controller represents the
      * participant in the JcatCall around which a transfer takes place.
      * <p>
      *
      * When a JcatCall is initially created, the transfer controller is
      * set to null. This method returns non-null only if the
      * application has previously set the transfer controller. If the
      * current transfer controller leaves the telephone call, the
      * transfer controller is reset to null.
      *
      * @return
      * The JcatTerminalConnection which acts as the transfer controller.
      */
    public JcatTerminalConnection getTransferController();
    /**
      * Return true if transferring is enabled, false otherwise.
      * Applications may use this method initially to obtain the default
      * value set by the implementation and may attempt to change this
      * value using the JcatCall.setTransferEnable() method.
      *
      * @return
      * true/false depending on whether transfer is enabled.
      */
    public boolean getTransferEnable();
    /**
      * Removes the terminal connection listener. Removes a
      * TerminalConnectionlistener from this call. If the listener is
      * not part of the Call for the given address(es), then this method
      * fails
      * silently.JcatTerminalConnectionEvent.TERMINALCONNECTION_EVENT_TRANSMISSION_ENDED
      * is the last event sent on this listener.
      *
      * @param terminalConnectionListener
      * Listener to be added.
      */
    public void removeTerminalConnectionListener(JcatTerminalConnectionListener terminalConnectionListener);
    /**
      * Sets the JcatTerminalConnection which acts as the conference
      * controller for the JcatCall. The conference controller
      * represents the participant in the JcatCall around which a
      * conference takes place. Typically, when two JcatCalls are
      * conferenced together, a single participant is part of both
      * JcatCalls. This participant is represented by a
      * JcatTerminalConnection on each JcatCall, each of which shares
      * the same JcatTerminal. If the designated JcatTerminalConnection
      * is not part of this JcatCall, exception InvalidArgumentException
      * is thrown. If the JcatTerminalConnection leaves the JcatCall in
      * the future, the implementation resets the conference controller
      * to null.
      *
      * @param tc
      * The JcatTerminalConnection to set as the conference controller.
      *
      * @exception InvalidArgumentException
      * The TerminalConnection controlling the transfer is not valid or
      * does not exist or the other Call is not valid.
      *
      * @exception InvalidStateException
      * Either the Provider is not "in service", the Call is not
      * "active", or the transfer controllers are not "talking" or
      * "held".
      *
      * @exception MethodNotSupportedException
      * This method is not supported by the implementation.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for the successful invocation of
      * this method is not available.
      */
    public void setConferenceController(JcatTerminalConnection tc)
        throws InvalidArgumentException, InvalidStateException,
                          MethodNotSupportedException,
                          ResourceUnavailableException;
    /**
      * Controls whether the JcatCall is permitted or able to perform
      * the conferencing feature. The boolean argument provided
      * indicates whether conferencing should be turned on (true) or off
      * (false). This method throws an exception if the boolean argument
      * is true and the implementation does not support the conferencing
      * feature. This method must be invoked when the JcatCall is in the
      * JccCall.IDLE state. <p>
      *
      * Setting this parameter is a "request"; the call will be routed
      * such that a resource capable of conferencing is part of the
      * path. Note that this does not mean that three party calls are
      * not supported if ConferenceEnable=false since we consider a
      * conference to be call with more than three parties.
      *
      * @param enabled
      * Set to true or false depending on whether conferencing feature
      * is enabled or not.
      *
      * @exception InvalidArgumentException
      * The TerminalConnection controlling the transfer is not valid or
      * does not exist or the other Call is not valid.
      *
      * @exception InvalidStateException
      * Either the Provider is not "in service", the Call is not
      * "active", or the transfer controllers are not "talking" or
      * "held".
      *
      * @exception MethodNotSupportedException
      * This method is not supported by the implementation.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for the successful invocation of
      * this method is not available.
      */
    public void setConferenceEnable(boolean enabled)
        throws InvalidArgumentException, InvalidStateException,
                          MethodNotSupportedException,
                          ResourceUnavailableException;
    /**
      * Sets the JcatTerminalConnection which acts as the transfer
      * controller for the JcatCall. The transfer controller represents
      * the participant in the JcatCall around which a transfer takes
      * place. If the designated JcatTerminalConnection is not part of
      * this JcatCall, an exception is thrown. If the
      * JcatTerminalConnection leaves the JcatCall in the future, the
      * implementation resets the transfer controller to null.
      *
      * @param termconn
      * JcatTerminalConnection to set as the transfer controller.
      *
      * @exception InvalidArgumentException
      * The TerminalConnection controlling the transfer is not valid or
      * does not exist or the other Call is not valid.
      *
      * @exception InvalidStateException
      * Either the Provider is not "in service", the Call is not
      * "active", or the transfer controllers are not "talking" or
      * "held".
      *
      * @exception MethodNotSupportedException
      * This method is not supported by the implementation.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for the successful invocation of
      * this method is not available.
      */
    public void setTransferController(JcatTerminalConnection termconn)
        throws InvalidArgumentException, InvalidStateException,
                          MethodNotSupportedException,
                          ResourceUnavailableException;
    /**
      * Controls whether the JcatCall is permitted or able to perform
      * the transferring feature. The boolean argument provided
      * indicates whether transferring should be turned on (true) or off
      * (false). This method throws an exception if the boolean argument
      * is true and the implementation does not support the transferring
      * feature. This method must be invoked when the JcatCall is in the
      * JccCall.IDLE state.
      *
      * @param enable
      * true/false depending on whether transferring feature is enabled.
      *
      * @exception InvalidArgumentException
      * The TerminalConnection controlling the transfer is not valid or
      * does not exist or the other Call is not valid.
      *
      * @exception InvalidStateException
      * Either the Provider is not "in service", the Call is not
      * "active", or the transfer controllers are not "talking" or
      * "held".
      *
      * @exception MethodNotSupportedException
      * This method is not supported by the implementation.
      *
      * @exception ResourceUnavailableException
      * An internal resource necessary for the successful invocation of
      * this method is not available.
      */
    public void setTransferEnable(boolean enable)
        throws InvalidArgumentException, InvalidStateException,
                          MethodNotSupportedException,
                          ResourceUnavailableException;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

