/*
#pragma ident "@(#)CallControlCall.java	1.29      97/02/02     SMI"

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

package javax.telephony.callcontrol;
import  javax.telephony.*;

/**
 * <H4>Introduction</H4>
 *
 * The <CODE>CallControlCall</CODE> interface extends the core
 * <CODE>Call</CODE> interface. This interface provides additional methods on
 * a Call.
 *
 * <H4>Additional Call Information</H4>
 *
 * This interface supports methods which return additional information
 * regarding the Call. Specifically, it returns the <EM>calling address</EM>,
 * <EM>calling terminal</EM>, <EM>called address</EM>, and <EM>last redirected
 * address</EM> information.
 * <p>
 * The calling address, as returned by the
 * <CODE>CallControlCall.getCallingAddress()</CODE> method is the Address which
 * originally placed the Call. The calling terminal, as returned by the
 * <CODE>CallControlCall.getCallingTerminal()</CODE> method is the Terminal
 * which originally placed the Call. The called Address, as returned by the
 * <CODE>CallControlCall.getCalledAddress()</CODE> method is the Address to
 * which the Call was originally placed. The last redirected address, as
 * returned by the <CODE>CallControlCall.getLastRedirectedAddress()</CODE>
 * method is the Address to which this Call was placed before the current
 * destination Address. For example, if a Call was forwarded from one Address
 * to another, then the first Address is the last redirected Address for this
 * call.
 * <p>
 * Each of these methods returns <CODE>null</CODE> if their values are
 * unknown at the present time.  During the lifetime of a Call, an
 * implementation may learn this additional information, and return different
 * values for some or all of these methods as a result.
 *
 * <H4>Conferencing Telephone Calls</H4>
 *
 * The conferencing feature supported by this interface permits two telephone
 * calls to be "merged". That is, the two Calls are merged into a single Call
 * with the union of all of the participants of the two Calls being placed on
 * the single Call.
 * <p>
 * Applications invoke the <CODE>CallControlCall.conference()</CODE> method to
 * perform the conferencing feature. This method is given the "second"
 * Call as an argument. All participants are moved from the second Call to the
 * Call on which the method is invoked. The second Call moves into the
 * <CODE>Call.INVALID</CODE> state as a result.
 * <p>
 * In order for the conferencing feature to happen, there must be a common
 * participant to both Calls, as represented by a single Terminal and two
 * TerminalConnections, one on each of the two Calls. These two
 * TerminalConnections are known as the <EM>conference controllers</EM>. In
 * the real-world, one of the two telephone calls must be on hold with respect
 * to the controlling Terminal, and hence, the TerminalConnection on the
 * second Call must be in the <CODE>CallControlTerminalConnection.HELD</CODE>
 * state. The two conference controlling TerminalConnections are merged into
 * one as a result of this method.
 * <p>
 * Applications may control which TerminalConnection acts as the conference
 * controller via the <CODE>CallControlCall.setConferenceController()</CODE>
 * method. The <CODE>CallControlCall.getConferenceController()</CODE> method
 * returns the current conference controller, <CODE>null</CODE> if there is
 * none. If no conference controller is set, the implementation chooses a
 * suitable TerminalConnection when the conferencing feature is invoked.
 *
 * <H4>Transferring Telephone Calls</H4>
 *
 * The transfer feature supported by this interface permits one Call to be
 * "moved" to another Call. That is, all of the participants from one Call are
 * moved to another Call, except for the transferring participant which drops
 * off from both Calls.
 * <p>
 * Applications invoke the <CODE>CallControlCall.transfer()</CODE> method to
 * perform the transfer feature. There are two overloaded versions of this
 * method. The first method takes a second Call as an argument. This method
 * acts similarly to <CODE>CallControlCall.conference()</CODE>, except the two
 * TerminalConnections on each Call with a common Terminal are removed from
 * both Calls. The second version takes a string telephone address as an
 * argument. This method removes the transfer controller participant while
 * placing the telephone call to the designated address. This latter version
 * of the transfer feature is often known as a <EM>single-step transfer</EM>.
 * <p>
 * In order for the transfer feature to happen, there must be a participant
 * which acts as the <EM>transfer controller</EM>. The transfer controller is a
 * TerminalConnection around which the transfer is performed. In the first
 * version of the <CODE>CallControlCall.transfer()</CODE> method, the transfer
 * controller must be present on each of the two Calls and share a common
 * Terminal. In the second version, the transfer controller only applies to
 * the Call object on which the method is invoked (since there is no second
 * Call involved). In both cases, the transfer controller participant is no
 * longer part of any Call once the transfer feature is complete.
 * <p>
 * Applications may control which TerminalConnection acts as the transfer
 * controller via the <CODE>CallControlCall.setTransferController()</CODE>
 * method. The <CODE>CallControlCall.getTransferController()</CODE> method
 * returns the current transfer controller, <CODE>null</CODE> if there is
 * none. If no transfer controller is set, the implementation chooses a
 * suitable TerminalConnection when the conferencing feature is invoked.
 * 
 * <H4>Consultation Calls</H4>
 *
 * <EM>Consultation Calls</EM> are special types of telephony calls created
 * (often temporarily) for a specific purpose. Consultation calls are created
 * if a user wants to "consult" with another party briefly while currently on
 * a Call, or are created for the purpose of conferencing or transferring with
 * a Call. Consequently, consultation calls are always associated with another
 * existing Call.
 * <p>
 * Applications invoke the <CODE>CallControlCall.consult()</CODE> method to
 * perform the consultation feature. The instance on which the method is
 * invoke is always the "idle" Call on which the consultation takes place.
 * There are two overloaded versions of this method. The first method takes a
 * TerminalConnection and a string telephone address as arguments. This
 * consultation telephone call is associated with the Call of the
 * TerminalConnection argument. This method places a telephone call from the
 * same originating endpoint specified by the TerminalConnection argument to
 * the designated telephone address string. The second version of this method
 * only takes a TerminalConnection as an argument, and permits applications to
 * use the <CODE>CallControlConnection.addToAddress()</CODE> method to dial
 * the destination address string.
 *
 * <H4>Additional CallControlCall Methods</H4>
 *
 * The <CODE>CallControlCall.addParty()</CODE> method adds a single party to
 * a Call given some telephone address string. The
 * <CODE>CallControlCall.drop()</CODE> disconnects all parties from the
 * Call and moves it into the <CODE>Call.INVALID</CODE> state. The
 * <CODE>CallControlCall.offHook()</CODE> method takes an originating Address
 * and Terminal pair "off hook" and permits applications to dial destination
 * address digits one-by-one.
 *
 * <H4>Observers and Events</H4>
 *
 * All events pertaining to the <CODE>CallControlCall</CODE> interface are
 * reported via the <CODE>CallObserver.callChangedEvent()</CODE> method. The
 * application observer object must also implement the
 * <CODE>CallControlCallObserver</CODE> interface to express interest in the
 * call control package events. Applications receive events pertaining to the
 * <CODE>CallControlConnection</CODE> and
 * <CODE>CallControlTerminalConnection</CODE> interfaces via this observer as
 * well.
 * <p>
 * All <CODE>CallControlCall</CODE>-related events must extend the
 * <CODE>CallCtlCallEv</CODE> interface. There are no specific events
 * pertaining to the <CODE>CallControlCall</CODE> interface, however.
 * <p>
 * @see javax.telephony.Call
 * @see javax.telephony.CallObserver
 * @see javax.telephony.callcontrol.CallControlCallObserver
 * @see javax.telephony.callcontrol.events.CallCtlCallEv
 */

public interface CallControlCall extends Call {

  /**
   * Returns the calling Address associated with this call. The calling
   * Address is the Address which originally placed the telephone call. If the
   * calling address is unknown or not yet known, this method returns null.
   * <p>
   * @return The calling Address.
   */
  public Address getCallingAddress();


  /**
   * Returns the calling Terminal associated with this Call. The calling
   * Terminal is the Terminal which originally placed the telephone call. If
   * the calling Terminal is unknown or not yet known, this method returns
   * null.
   * <p>
   * @return The calling Terminal.
   */
  public Terminal getCallingTerminal();


  /**
   * Returns the called Address associated with this Call. The called
   * Address is the Address to which the call had been originally placed. If
   * the called address is unknown or not yet known, this method returns null.
   * <p>
   * @return The called Address.
   */
  public Address getCalledAddress();


  /**
   * Returns the last redirected Address associated with this Call.
   * The last redirected Address is the Address at which the current Call
   * was placed immediately before the current Address. This is common if a
   * Call is forwarded to several Addresses before being answered. If the last
   * redirected address is unknown or not yet known, this method returns null.
   * <p>
   * @return The last redirected Address for this telephone Call.
   */
  public Address getLastRedirectedAddress();


  /**
   * Adds an additional party to an existing Call. This is sometimes called a
   * "single-step conference" because a party is conferenced into a Call
   * directly. The telephone address string provided as the argument must be
   * complete and valid.
   *
   * <H5>States of the Existing Connections</H5>
   *
   * The Call must  have at least two Connections in the
   * <CODE>CallControlConnection.ESTABLISHED</CODE> state. An additional
   * restriction requires that at most one other Connection may be in either
   * the <CODE>CallControlConnection.QUEUED</CODE>,
   * <CODE>CallControlConnection.OFFERED</CODE>, or
   * <CODE>CallControlConnection.ALERTING</CODE> state.
   * <p>
   * Some telephony platforms impose restrictions on the number of Connections
   * in a particular state. For instance, it is common to restrict the number
   * of "alerting" Connections to at most one. As a result, this method
   * requires that at most one other Connections is in the "queued",
   * "offering", or "alerting" state. (Note that the first two states
   * correspond to the core Connection "in progress" state). Although some
   * systems may not enforce this requirement, for consistency, JTAPI specifies
   * implementations must uphold the conservative requirement.
   *
   * <H5>The New Connection</H5>
   *
   * This method creates and returns a new Connection representing the new
   * party. This Connection must at least be in the
   * <CODE>CallControlConnection.IDLE</CODE> state. Its state may have
   * progressed beyond "idle" before this method returns, and should be
   * reflected by an event. This new Connection will progress as any normal
   * destination Connection on a Call. Typical scenarios for this Connection
   * are described by the <CODE>Call.connect()</CODE> method.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>Let c[] = call.getConnections() where c.length >= 2
   * <LI>c[i].getCallControlState() == CallControlConnection.ESTABLISHED for
   * at least two i
   * <LI>c[j].getCallControlState() == CallControlConnection.QUEUED,
   * CallControlConnection.OFFERED, or CallControlConnection.ALERTING for at
   * most one c[j]
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let connection be the Connection created and returned
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>connection.getCallControlState() at least CallControlConnection.IDLE
   * <LI>ConnCreatedEv is delivered for connection
   * </OL>
   * @see javax.telephony.events.ConnCreatedEv
   * @param newParty The telephone address of the party to be added.
   * @return The new Connection associated with the added party.
   * @exception InvalidStateException Either the Provider is not "in service",
   * the Call is not "active" or the proper conditions on the Connections does
   * not exist.
   * as designated by the pre-conditions for this method.
   * @exception InvalidPartyException The destination address string is not
   * valid and/or complete.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public Connection addParty(String newParty)
    throws InvalidStateException, InvalidPartyException,
           MethodNotSupportedException, PrivilegeViolationException,
           ResourceUnavailableException;


  /**
   * Drops the entire Call. This method is equivalent to using the
   * <CODE>Connection.disconnect()</CODE> method on each Connection which is
   * part of the Call. Typically, each Connection on the Call will move into
   * the <CODE>CallControlConnection.DISCONNECTED</CODE> state, each
   * TerminalConnection will move into the
   * <CODE>CallControlTerminalConnection.DROPPED</CODE> state, and the Call
   * will move into the <CODE>Call.INVALID</CODE> state.
   * <p>
   * There are some Connections for which the application does not possess the
   * proper authority to disconnect. In this case, this method performs no
   * action on these Connections. These Connections may disconnect naturally
   * as a result of disconnecting other Connections, however. This method
   * returns when it can successfully disconnect as many methods as it can.
   * The application is notified via events whether the entire Call was
   * successfully dropped.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>Let c[] = this.getConnections() before this method is invoked
   * <LI>If c[i].getCallControlState == CallControlConnection.DISCONNECTED, for
   * all i, then this.getState() == Call.INVALID
   * <LI>CallCtlConnDisconnectedEv/ConnDisconnectedEv is delivered for all
   * disconnected Connections
   * <LI>CallCtlTermConnDroppedEv/TermConnDroppedEv is delivered for all
   * dropped TerminalConnections
   * <LI>CallInvalidEv is delivered if all Connections were disconnected
   * </OL>
   * @see javax.telephony.events.ConnDisconnectedEv
   * @see javax.telephony.events.TermConnDroppedEv
   * @see javax.telephony.events.CallInvalidEv
   * @see javax.telephony.callcontrol.events.CallCtlConnDisconnectedEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnDroppedEv
   * @exception InvalidStateException Either the Provider was not "in service"
   * or the Call was not "active".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method and it can drop none of the
   * Connections.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public void drop()
      throws InvalidStateException, MethodNotSupportedException,
	    PrivilegeViolationException, ResourceUnavailableException;


  /**
   * Takes the originating end of a Call off-hook. This method permits
   * applications to simply take the originating terminal of a Call off-hook so
   * that users may manually dial telephone number digits or applications may
   * supply digits with the <CODE>CallControlConnection.addToAddress()</CODE>
   * method. This is in contrast to the <CODE>Call.connect()</CODE> method
   * which requires the complete destination address string.
   * <p>
   * This method takes the originating Address and Terminal as arguments. This
   * Call must be in the <CODE>Call.IDLE</CODE> state. This method creates and
   * returns a Connection to the originating Address in the
   * <CODE>CallControlConnection.INITIATED</CODE> state. This method also
   * creates a TerminalConnection in the
   * <CODE>CallControlTerminalConnection.TALKING</CODE> state and associated
   * with the new Connection and originating Terminal.
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
   * <LI>Let connection be the Connection created and returned
   * <LI>Let terminalconnection be the TerminalConnection created
   * <LI>connection.getCallControlState() == CallControlConnection.INITIATED
   * <LI>Let tc[] = c.getTerminalConnections() where tc.length == 1
   * <LI>tc[0] == terminalconnection
   * <LI>tc[0].getCallControlState() == CallControlTerminalConnection.TALKING
   * <LI>tc[0] element of origterminal.getTerminalConnections()
   * <LI>connection element of origaddress.getConnections()
   * <LI>connection element of this.getConnections()
   * <LI>ConnCreatedEv is delivered for connection
   * <LI>TermConnCreatedEv is delivered for terminalconnection
   * <LI>CallActiveEv is delivered for this Call
   * <LI>CallCtlConnInitiatedEv/ConnConnectedEv is delivered for connection
   * <LI>CallCtlTermConnTalkingEv/TermConnActiveEv is delivered for
   * terminalconnection
   * </OL>
   * @see javax.telephony.events.ConnCreatedEv
   * @see javax.telephony.events.TermConnCreatedEv
   * @see javax.telephony.events.CallActiveEv
   * @see javax.telephony.callcontrol.events.CallCtlConnInitiatedEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnTalkingEv
   * @param origaddress The originating Address object.
   * @param origterminal The originating Terminal object.
   * @return The Connection associated with the originating end of the
   * telephone call.
   * @exception InvalidStateException Either the Provider was not "in service"
   * or the Call was not "idle".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public Connection offHook(Address origaddress, Terminal origterminal)
    throws InvalidStateException, MethodNotSupportedException,
      PrivilegeViolationException, ResourceUnavailableException;


  /**
   * Merges two Calls together, resulting in the union of the participants
   * of both Calls being placed on a single Call. This method takes a Call
   * as an argument, referred to hereafter as the "second" Call. All of the
   * participants from the second call are moved to the Call on which this
   * method is invoked.
   *
   * <H5>The Conference Controller</H5>
   *
   * In order for the conferencing feature to happen, there must be a common
   * participant to both Calls, as represented by a single Terminal and two
   * TerminalConnections, one on each of the two Calls. These two
   * TerminalConnections are known as the <EM>conference controllers</EM>. In
   * the real-world, one of the two Calls must be on hold with respect to the
   * controlling Terminal, and hence, the TerminalConnection on the second
   * Call must be in the <CODE>CallControlTerminalConnection.HELD</CODE> state.
   * The two conference controlling TerminalConnections are merged into one as
   * a result of this method.
   * <p>
   * Applications may control which TerminalConnection acts as the conference
   * controller via the <CODE>CallControlCall.setConferenceController()</CODE>
   * method. The <CODE>CallControlCall.getConferenceController()</CODE> method
   * returns the current conference controller, <CODE>null</CODE> if there is
   * none. If no conference controller is set, the implementation chooses a
   * suitable TerminalConnection when the conferencing feature is invoked.
   * 
   * <H5>The Telephone Call Argument</H5>
   *
   * All of the participants from the second Call, passed as the argument to
   * this method, are "moved" to the Call on which this method was invoked.
   * That is, new Connections and TerminalConnections are created on this Call
   * which are found on the second Call. Those Connections and
   * TerminalConnections on the second Call are removed from the Call and the
   * Call moves into the <CODE>Call.INVALID</CODE> state.
   * <p>
   * The conference controller TerminalConnections are merged into one on this
   * Call. That is, the existing TerminalConnection controller on this Call is
   * left unchanged, while the TerminalConnection on the second Call is removed
   * from that Call.
   *
   * <H5>Other Shared Participants</H5>
   *
   * There may exist Address and Terminals which are part of both telephone
   * call in addition to the designated conference controller. In these
   * instances, those participants which are shared between both Calls are
   * merged into one. That is, the Connections and TerminalConnections on this
   * Call are left unchanged. The corresponding Connections and
   * TerminalConnections on the second Call are removed from that Call.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>Let tc1 be the conference controller on this Call
   * <LI>Let connection1 = tc1.getConnection()
   * <LI>Let tc2 be the conference controller on otherCall
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>tc1.getTerminal() == tc2.getTerminal()
   * <LI>tc1.getCallControlState() == CallControlTerminalConnection.TALKING
   * <LI>tc2.getCallControlState() == CallControlTerminalConnection.HELD
   * <LI>this != otherCall
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>otherCall.getState() == INVALID
   * <LI>Let c[] be the Connections to be merged from otherCall
   * <LI>Let tc[] be the TerminalConnections to be merged from otherCall
   * <LI>Let new(c) be the set of new Connections created on this Call
   * <LI>Let new(tc) be the set of new TerminalConnections created on this Call
   * <LI>new(c) element of this.getConnections()
   * <LI>new(c).getCallState() == c.getCallState()
   * <LI>new(tc) element of (this.getConnections()).getTerminalConnections()
   * <LI>new(tc).getCallState() == tc.getCallState()
   * <LI>c[i].getCallControlState() == CallControlConnection.DISCONNECTED for
   * all i
   * <LI>tc[i].getCallControlState() == CallControlTerminalConnection.DROPPED
   * for all i
   * <LI>CallInvalidEv is delivered for otherCall
   * <LI>CallCtlConnDisconnectedEv/ConnDisconnectedEv is delivered for all c[i]
   * <LI>CallCtlTermConnDroppedEv/TermConnDroppedEv is delivered for all tc[i]
   * <LI>ConnCreatedEv is delivered for all new(c)
   * <LI>TermConnCreatedEv is delivered for all new(tc)
   * <LI>Appropriate events are delivered for all new(c) and new(tc)
   * </OL>
   * @see javax.telephony.events.ConnCreatedEv
   * @see javax.telephony.events.TermConnCreatedEv
   * @see javax.telephony.events.ConnDisconnectedEv
   * @see javax.telephony.events.TermConnDroppedEv
   * @see javax.telephony.events.CallInvalidEv
   * @see javax.telephony.callcontrol.events.CallCtlConnDisconnectedEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnDroppedEv
   * @param otherCall The second Call which to merge with this Call object.
   * @exception InvalidArgumentException The Call object provided is
   * not valid for the conference
   * @exception InvalidStateException Either the Provider is not "in service",
   * the Call is not "active", or the conference controllers are not in the
   * proper state.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public void conference(Call otherCall)
    throws InvalidStateException, InvalidArgumentException,
           MethodNotSupportedException, PrivilegeViolationException,
           ResourceUnavailableException;


  /**
   * This method moves all participants from one Call to another, with the
   * exception of a selected common participant. This method takes a Call as
   * an argument, referred to hereafter as the "second" Call. All of the
   * participants, with the exception for the selected common participant, from
   * the second call are moved to the Call on which this method is invoked.
   *
   * <H5>The Transfer Controller</H5>
   *
   * In order for the transfer feature to happen, there must be a participant
   * which acts as the transfer controller. The transfer controller is a
   * TerminalConnection around which the transfer is placed. This transfer
   * controller must be present on each of the two Calls and share a common
   * Terminal. The transfer controller participant is no longer part of any
   * Call once this transfer feature is complete. The transfer controllers on
   * each of the two Calls must be in either of the
   * <CODE>CallControlTerminalConnection.TALKING</CODE> or
   * <CODE>CallControlTerminalConnection.HELD</CODE> state.
   * <p>
   * Applications may control which TerminalConnection acts as the transfer
   * controller via the <CODE>CallControlCall.setTransferController()</CODE>
   * method. The <CODE>CallControlCall.getTransferController()</CODE> method
   * returns the current transfer controller, <CODE>null</CODE> if there is
   * none. If no transfer controller is set, the implementation chooses a
   * suitable TerminalConnection when the transfer feature is invoked.
   *
   * <H5>The Telephone Call Argument</H5>
   *
   * All of the participants from the second Call, passed as the argument to
   * this method, are "moved" to the Call on which this method is invoked, with
   * the exception of the transfer controller. That is, new Connections and
   * TerminalConnections are created on this Call which are found on the
   * second Call. Those Connections and TerminalConnections on the second Call
   * are removed from the Call and the Call moves into the
   * <CODE>Call.INVALID</CODE> state.
   * <p>
   * The transfer controller TerminalConnections are dropped from both Calls.
   * They move into the <CODE>CallControlTerminalConnection.DROPPED</CODE>
   * state.
   *
   * <H5>Other Shared Participants</H5>
   *
   * There may exist Address and Terminals which are part of both Calls in
   * addition to the designated transfer controller. In these instances, those
   * participants which are shared between both Calls are merged into one.
   * That is, the Connections and TerminalConnections on this Call are left
   * unchanged. The corresponding Connections and TerminalConnections on the
   * second Call are removed from that Call.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>Let tc1 be the transfer controller on this Call
   * <LI>Let tc2 be the transfer controller on otherCall
   * <LI>this != otherCall
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>otherCall.getState() == Call.ACTIVE
   * <LI>tc1.getCallControlState() == CallControlTerminalConnection.TALKING or
   * CallControlTerminalConnection.HELD
   * <LI>tc2.getCallControlState() == CallControlTerminalConnection.TALKING or
   * CallControlTerminalConnection.HELD
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>otherCall.getState() == Call.INVALID
   * <LI>tc1.getCallControlState() == CallControlTerminalConnection.DROPPED
   * <LI>tc2.getCallControlState() == CallControlTerminalConnection.DROPPED
   * <LI>Let c[] be the Connections to be transferred from otherCall
   * <LI>Let tc[] be the TerminalConnections to be transferred from otherCall
   * <LI>Let new(c) be the set of new Connections created on this Call
   * <LI>Let new(tc) be the set of new TerminalConnections created on this Call
   * <LI>new(c) element of this.getConnections()
   * <LI>new(c).getCallState() == c.getCallState()
   * <LI>new(tc) element of (this.getConnections()).getTerminalConnections()
   * <LI>new(tc).getCallState() == tc.getCallState()
   * <LI>c[i].getCallControlState() == CallControlConnection.DISCONNECTED for
   * all i
   * <LI>tc[i].getCallControlState() == CallControlTerminalConnection.DROPPED
   * for all i
   * <LI>CallInvalidEv is delivered for otherCall
   * <LI>CallCtlTermConnDroppedEv/TermConnDroppedEv is delivered for tc1, tc2
   * <LI>CallCtlConnDisconnectedEv/ConnDisconnectedEv is delivered for all c[i]
   * <LI>CallCtlTermConnDroppedEv/TermConnDroppedEv is delivered for all tc[i]
   * <LI>ConnCreatedEv is delivered for all new(c)
   * <LI>TermConnCreatedEv is delivered for all new(tc)
   * <LI>Appropriate events are delivered for all new(c) and new(tc)
   * </OL>
   * @see javax.telephony.events.ConnCreatedEv
   * @see javax.telephony.events.TermConnCreatedEv
   * @see javax.telephony.events.ConnDisconnectedEv
   * @see javax.telephony.events.TermConnDroppedEv
   * @see javax.telephony.events.CallInvalidEv
   * @see javax.telephony.callcontrol.events.CallCtlConnDisconnectedEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnDroppedEv
   * @param otherCall The other Call which to transfer to this Call.
   * @exception InvalidArgumentException The TerminalConnection
   * controlling the transfer is not valid or does not exist or the other Call
   * is not valid.
   * @exception InvalidStateException Either the Provider is not "in service",
   * the Call are not "active", or the transfer controllers are not "talking"
   * or "held".
   * @exception InvalidPartyException The other Call given as the argument is
   * not a valid Call to conference with.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public void transfer(Call otherCall)
    throws InvalidStateException, InvalidArgumentException,
      InvalidPartyException, MethodNotSupportedException,
      PrivilegeViolationException, ResourceUnavailableException;


  /**
   * This overloaded version of this method transfers all participants
   * currently on this Call, with the exception of the transfer controller
   * participant, to another telephone address. This is often called a
   * "single-step transfer" because the transfer feature places another
   * telephone call and performs the transfer at one time. The telephone
   * address string given as the argument to this method must be valid and
   * complete.
   *
   * <H5>The Transfer Controller</H5>
   *
   * The transfer controller for this version of this method represents the
   * participant on this Call around which the transfer is taking place and
   * who drops off the Call once the transfer feature has completed. The
   * transfer controller is a TerminalConnection which must be in the
   * <CODE>CallControlTerminalConnection.TALKING</CODE> state.
   * <p>
   * Applications may control which TerminalConnection acts as the transfer
   * controller via the <CODE>CallControlCall.setTransferController()</CODE>
   * method. The <CODE>CallControlCall.getTransferController()</CODE> method
   * returns the current transfer controller, <CODE>null</CODE> if there is
   * none. If no transfer controller is set, the implementation chooses a
   * suitable TerminalConnection when the transfer feature is invoked.
   * <p>
   * When the transfer feature is invoked, the transfer controller moves into
   * the <CODE>CallControlTerminalConnection.DROPPED</CODE> state. If it is
   * the only TerminalConnection associated with its Connection, then its
   * Connection moves into the <CODE>CallControlConnection.DISCONNECTED</CODE>
   * state as well.
   *
   * <H5>The New Connection</H5>
   *
   * This method creates and returns a new Connection representing the party
   * to which the Call was transferred. Note that this Connections may be
   * <CODE>null</CODE> in the case the Call has been transferred outside of
   * the Provider's domain and can no longer be tracked. This Connection must
   * at least be in the <CODE>CallControlConnection.IDLE</CODE> state. Its
   * state may have progressed beyond "idle" before this method returns, and
   * should be reflected by an event. This new Connection will progress as any
   * normal destination Connection on a telephone call. Typical scenarios for
   * this Connection are described by the <CODE>Call.connect()</CODE> method.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>Let tc be the transfer controller on this Call
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>tc.getCallControlState() == CallControlTerminalConnection.TALKING
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let newconnection be the Connection created and returned
   * <LI>Let connection == tc.getConnection()
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>tc.getCallControlState() == CallControlTerminalConnection.DROPPED
   * <LI>If connection.getTerminalConnections().length == 1, then
   * connection.getCallControlState() == CallControlConnection.DISCONNECTED
   * <LI>newconnection is an element of this.getConnections(), if not null.
   * <LI>newconnection.getCallControlState() at least
   * CallControlConnection.IDLE, if not null.
   * <LI>ConnCreatedEv is delivered for newconnection
   * <LI>CallCtlTermConnDroppedEv/TermConnDroppedEv is delivered for tc
   * <LI>CallCtlConnDisconnectedEv/ConnDisconnectedEv is delivered for
   * connection if no other TerminalConnections
   * </OL>
   * @see javax.telephony.events.ConnCreatedEv
   * @see javax.telephony.events.ConnDisconnectedEv
   * @see javax.telephony.events.TermConnDroppedEv
   * @see javax.telephony.callcontrol.events.CallCtlConnDisconnectedEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnDroppedEv
   * @param address The destination telephone address string to where the
   * Call is being transferred.
   * @return The new Connection associated with the destination or null.
   * @exception InvalidArgumentException The TerminalConnection provided as
   * controlling the transfer is not valid or part of this Call.
   * @exception InvalidStateException Either the Provider is not "in service",
   * the Call is not "active", or the transfer controller is not "talking".
   * @exception InvalidPartyException The destination address is not valid
   * and/or complete.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public Connection transfer(String address)
    throws InvalidArgumentException, InvalidStateException,
      InvalidPartyException, MethodNotSupportedException,
      PrivilegeViolationException, ResourceUnavailableException;


  /**
   * Sets the TerminalConnection which acts as the conference controller for
   * the Call. The conference controller represents the participant in the
   * Call around which a conference takes place.
   * <p>
   * Typically, when two Calls are conferenced together, a single participant
   * is part of both Calls. This participant is represented by a
   * TerminalConnection on each Call, each of which shares the same Terminal.
   * <p>
   * If the designated TerminalConnection is not part of this Call, an
   * exception is thrown. If the TerminalConnection leaves the Call in the
   * future, the implementation resets the conference controller to
   * <CODE>null</CODE>.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>Let connections[] = this.getConnections()
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>tc element of connections[i].getTerminalConnections() for all i
   * <LI>tc.getCallControlState() != CallControlTerminalConnection.DROPPED
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>Let connections[] = this.getConnections()
   * <LI>tc element of connections[i].getTerminalConnections() for all i
   * <LI>tc.getCallControlState() != CallControlTerminalConnection.DROPPED
   * <LI>tc == this.getConferenceController()
   * </OL>
   * @param tc The TerminalConnection to use as the conference controller
   * @exception InvalidArgumentException The TerminalConnection provided is not
   * associated with this Call.
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Call is not "active".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public void setConferenceController(TerminalConnection tc)
    throws InvalidArgumentException, InvalidStateException,
      MethodNotSupportedException, ResourceUnavailableException;


  /**
   * Returns the TerminalConnection which currently acts as the conference
   * controller. The conference controller represents the participant in the
   * telephone around which a conference takes place.
   * <p>
   * When a Call is initially created, the conference controller is set
   * to <CODE>null</CODE>. This method returns non-null only if the
   * application has previously set the conference controller. If the current
   * conference controller leaves the Call, the conference controller is reset
   * to <CODE>null</CODE>.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() != Call.INVALID
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() != Call.INVALID
   * <LI>Let tc = this.getConferenceController()
   * <LI>Let connections[] = this.getConnections()
   * <LI>tc element of connections[i].getTerminalConnections() for all i, if
   * tc is not null
   * <LI>tc.getCallControlState() != CallControlTerminalConnection.DROPPED, if
   * tc is not null
   * </OL>
   * <p>
   * @return The current TerminalConnection acting as the conference
   * controller, null if one is not set.
   */
  public TerminalConnection getConferenceController();


  /**
   * Sets the TerminalConnection which acts as the transfer controller for
   * the Call. The transfer controller represents the participant in the Call
   * around which a transfer takes place.
   * <p>
   * If the designated TerminalConnection is not part of this Call, an
   * exception is thrown. If the TerminalConnection leaves the Call in the
   * future, the implementation resets the transfer controller to
   * <CODE>null</CODE>.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>Let connections[] = this.getConnections()
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>tc element of connections[i].getTerminalConnections() for all i
   * <LI>tc.getCallControlState() != CallControlTerminalConnection.DROPPED
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>Let connections[] = this.getConnections()
   * <LI>tc element of connections[i].getTerminalConnections() for all i
   * <LI>tc.getCallControlState() != CallControlTerminalConnection.DROPPED
   * <LI>tc == this.getTransferController()
   * </OL>
   * @param tc The TerminalConnection to use as the transfer controller
   * @exception InvalidArgumentException The TerminalConnection provided is
   * not associated with this Call.
   * @exception InvalidStateException Either the Provider is not "in service",
   * Call was not "active", or the TerminalConnection argument was "dropped".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public void setTransferController(TerminalConnection tc)
    throws InvalidArgumentException, InvalidStateException,
      MethodNotSupportedException, ResourceUnavailableException;


  /**
   * Returns the TerminalConnection which currently acts as the transfer
   * controller. The transfer controller represents the participant in the
   * Call around which a transfer takes place.
   * <p>
   * When a Call is initially created, the transfer controller is set
   * to <CODE>null</CODE>. This method returns non-null only if the
   * application has previously set the transfer controller. If the current
   * transfer controller leaves the telephone call, the transfer controller
   * is reset to <CODE>null</CODE>.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() != Call.INVALID
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() != Call.INVALID
   * <LI>Let tc = this.getTransferController()
   * <LI>Let connections[] = this.getConnections()
   * <LI>tc element of connections[i].getTerminalConnections() for all i, if
   * tc is not null
   * <LI>tc.getCallControlState() != CallControlTerminalConnection.DROPPED, if
   * tc is not null
   * </OL>
   * <p>
   * @return The current TerminalConnection acting as the transfer
   * controller, null if one is not set.
   */
  public TerminalConnection getTransferController();


  /**
   * Controls whether the Call is permitted or able to perform the conferencing
   * feature. The boolean argument provided indicates whether conferencing
   * should be turned on (true) or off (false). This method throws an exception
   * if the boolean argument is true and the implementation does not support
   * the conferencing feature. This method must be invoked when the Call is
   * in the <CODE>Call.IDLE</CODE> state.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.IDLE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.IDLE
   * <LI>enable = this.getConferenceEnable()
   * </OL>
   * @param enable True turns conferencing on, false turns conferencing off.
   * @exception InvalidArgumentException Conferencing cannot be turned on as
   * requested by a true argument.
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Call is not "idle".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   */
  public void setConferenceEnable(boolean enable)
    throws InvalidArgumentException, InvalidStateException,
      MethodNotSupportedException, PrivilegeViolationException;


  /**
   * Return true if conferencing is enabled, false otherwise. Applications
   * may use this method initially to obtain the default value set by the
   * implementation and may attempt to change this value using the
   * <CODE>CallControlCall.setConferenceEnable()</CODE> method.
   * <p>
   * @return True if conferencing is enabled, false otherwise.
   */
  public boolean getConferenceEnable();


  /**
   * Controls whether the Call is permitted or able to perform the transferring
   * feature. The boolean argument provided indicates whether transferring
   * should be turned on (true) or off (false). This method throws an exception
   * if the boolean argument is true and the implementation does not support
   * the transferring feature. This method must be invoked when the Call is
   * in the <CODE>Call.IDLE</CODE> state.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.IDLE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.IDLE
   * <LI>enable = this.getConferenceEnable()
   * </OL>
   * @param enable True turns transferring on, false turns transferring off.
   * @exception InvalidArgumentException Transferring cannot be turned on as
   * requested by the true argument.
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Call is not "idle".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   */
  public void setTransferEnable(boolean enable)
    throws InvalidArgumentException, InvalidStateException,
      MethodNotSupportedException, PrivilegeViolationException;


  /**
   * Return true if transferring is enabled, false otherwise. Applications
   * may use this method initially to obtain the default value set by the
   * implementation and may attempt to change this value using the
   * <CODE>CallControlCall.setTransferEnable()</CODE> method.
   * <p>
   * @return True if transferring is enabled, false otherwise.
   */
  public boolean getTransferEnable();


  /**
   * Creates a consultation between this Call and an active Call. A
   * consultation Call is a new, idle Call which is associated with a
   * particular existing Call and often created for a particular purpose. For
   * example, the consultation Call may be used simply to "consult" with
   * another party or to conference or transfer with its associated Call. This
   * method establishes a special relationship between the two Calls which
   * extends down to the telephony platform level. Most often, this feature is
   * directly provided by the underlying telephony platform.
   *
   * <H5>This Call Object</H5>
   *
   * The instance on which this method is invoked is used as the Call on which
   * the consultation takes place. This Call must be in <CODE>Call.IDLE</CODE>
   * and is created with the <CODE>Provider.createCall()</CODE> method.
   *
   * <H5>The TerminalConnection Argument</H5>
   *
   * The TerminalConnection argument provides two pieces of information. The
   * first piece of information is the other active Call to which this idle
   * Call is associated. The Call associated with the TerminalConnection
   * argument must be in the <CODE>Call.ACTIVE</CODE> state.
   * <p>
   * The second piece of information given by the TerminalConnection argument
   * is the originating endpoint from which to place a telephone call on this
   * idle Call. That is, the Address and Terminal associated with the
   * TerminalConnection argument are used as the originating endpoint for the
   * telephone call. The state of the TerminalConnection must be
   * <CODE>CallControlTerminalConnection.TALKING</CODE> and this method first
   * moves it into the <CODE>CallControlTerminalConnection.HELD</CODE> in
   * order to place a telephone call on this idle Call.
   *
   * <H5>The Destination Address String</H5>
   *
   * A telephone call is placed to the destination telephone address string
   * given as the second argument to this method. This address string must be
   * valid and complete.
   *
   * <H5>The Consultation Purpose</H5>
   *
   * As mentioned above, the purpose of creating a consultation Call is often
   * to perform a transfer or conference action on these two Calls. If the
   * methods <CODE>CallControlCall.setConferenceEnable()</CODE> and
   * <CODE>CallControlCall.setTransferEnable()</CODE> are supported as
   * indicated by the <CODE>CallControlCallCapabilities</CODE> interface,
   * applications must specify the purpose of the consultation Call by first
   * telling the telephony platform if the intend to perform a transfer and/or
   * conference action.
   * <p>
   * If the <CODE>CallControlCall.setConferenceEnable()</CODE> and the
   * <CODE>CallControlCall.setTransferEnable()</CODE> methods are not supported
   * as indicated by this interface's capabilities, applications permit the
   * telephony platform to use the static, default values reported by the
   * <CODE>CallControlCall.getConferenceEnable()</CODE> and the
   * <CODE>CallControlCall.getTransferEnable()</CODE> methods.
   *
   * <H5>The Telephone Call</H5>
   *
   * A telephone call is placed on this Call and an array of two Connections
   * are returned representing the originating and destination participants of
   * the Call. The Call progresses in the same way as if the Call was placed
   * using the <CODE>Call.connect()</CODE> method. The description of that
   * method describes different scenarios under which the state of the call
   * progresses.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>this.getProvider().getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.IDLE
   * <LI>tc.getCallControlState() == CallControlTerminalConnection.TALKING
   * <LI>(tc.getCall()).getState() == Call.ACTIVE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let connections[] be the two Connections created and returned
   * <LI>this.getProvider().getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.ACTIVE
   * <LI>tc.getCallControlState() == CallControlTerminalConnection.HELD
   * <LI>(tc.getCall()).getState() == Call.ACTIVE
   * <LI>Let c[] = this.getConnections() such that c.length == 2
   * <LI>c[0].getCallControlState() at least CallControlConnection.IDLE
   * <LI>c[1].getCallControlState() at least CallControlConnection.IDLE
   * <LI>c == connection
   * <LI>CallActiveEv is delivered for this Call
   * <LI>ConnCreatedEv are delivered for both connections[i]
   * <LI>CallCtlTermConnHeldEv is delivered for tc
   * </OL>
   * @see javax.telephony.events.CallActiveEv
   * @see javax.telephony.events.ConnCreatedEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnHeldEv
   * @param tc The controlling TerminalConnection for the consultation call.
   * @param dialedDigits The destination telephone address string to which a
   * telephone call is being placed.
   * @return The two new Connections in the Call.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is unavailable.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to place a consultation telephone call.
   * @exception InvalidArgumentException The TerminalConnection given is
   * not a valid originating endpoint for a Call.
   * @exception InvalidPartyException The destination address string is not
   * valid and/or complete.
   * @exception InvalidStateException Either the Provider is not "in service",
   * the Call is not "idle", the other Call is not "active", or the
   * TerminalConnection is not "talking".
   * @exception MethodNotSupportedException The implementation does not
   * support this method.
   */
  public Connection[] consult(TerminalConnection tc, String dialedDigits)
    throws InvalidStateException, InvalidArgumentException,
      MethodNotSupportedException, ResourceUnavailableException,
      PrivilegeViolationException, InvalidPartyException;


  /**
   * Creates a consultation between this Call and an active Call. A
   * consultation call is a new, idle Call which is associated with a
   * particular existing Call and often created for a particular purpose. For
   * example, the consultation call may be used simply to "consult" with
   * another party or to conference or transfer with its associated Call. This
   * method establishes a special relationship between the two Calls which
   * extends down to the telephony platform level. Most often, this feature is
   * directly provided by the underlying telephony platform.
   * <p>
   * This overloaded version of this method has a single difference with the
   * other version of the <CODE>CallControlCall.consult()</CODE> method. This
   * method does not take a destination telephone address string as an
   * argument.
   * <p>
   * This method creates and returns a single Connection which is in the
   * <CODE>CallControlConnection.INITIATED</CODE> state. Applications may use
   * the <CODE>CallControlConnection.addToAddress()</CODE> method to dial the
   * destination address digits.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.IDLE
   * <LI>tc.getCallControlState() == CallControlTerminalConnection.TALKING
   * <LI>(tc.getCall()).getState() == Call.ACTIVE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let connection be the connection created and returned
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Call.IDLE
   * <LI>tc.getCallControlState() == CallControlTerminalConnection.HELD
   * <LI>(tc.getCall()).getState() == Call.ACTIVE
   * <LI>connection == c
   * <LI>Let c = this.getConnections() such that c.length == 1
   * <LI>c[0].getCallControlState() == CallControlConnection.INITIATED
   * <LI>ConnCreatedEv is delivered for connection
   * <LI>CallCtlConnInitiatedEv/ConnConnectedEv is delivered for connection
   * <LI>CallActiveEv is delivered for this Call
   * </OL>
   * @see javax.telephony.events.CallActiveEv
   * @see javax.telephony.events.ConnCreatedEv
   * @see javax.telephony.events.ConnConnectedEv
   * @see javax.telephony.callcontrol.events.CallCtlConnInitiatedEv
   * @param tc The controlling TerminalConnection for the consultation call.
   * @return The Connection in the "initiated" state.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is unavailable.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to place a consultation telephone call.
   * @exception InvalidArgumentException The TerminalConnection given is
   * not a valid originating endpoint for a Call.
   * @exception InvalidStateException Either the Provider is not "in service",
   * the Call is not "idle", the other Call is not "active", or the
   * TerminalConnection is not "talking".
   * @exception MethodNotSupportedException The implementation does not
   * support this method.
   */
  public Connection consult(TerminalConnection tc)
    throws InvalidStateException, InvalidArgumentException,
      MethodNotSupportedException, ResourceUnavailableException,
      PrivilegeViolationException;
}
