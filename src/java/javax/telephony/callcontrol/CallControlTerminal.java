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

package javax.telephony.callcontrol;
import  javax.telephony.*;

/**
 * <H4>Introduction</H4>
 *
 * The <CODE>CallControlTerminal</CODE> interface extends the core
 * <CODE>Terminal</CODE> interface with features like the ability to
 * pickup a call at a terminal and the ability to specify that this
 * terminal should not be disturbed.  Applications may query a Terminal object 
 * using the <CODE>instanceof</CODE> operator to see whether it supports this 
 * interface.
 *
 * <H4>Do Not Disturb</H4>
 * 
 * The <CODE>CallControlTerminal</CODE> interface defines the do not disturb
 * attribute. The do not disturb attribute indicates to the telephony platform
 * that this Terminal does not want to be bothered with incoming telephone
 * calls. That is, if this feature is activated, the underlying telephone
 * platform will not ring this Terminal for incoming telephone calls.
 * Applications use the <CODE>CallControlTerminal.setDoNotDisturb()</CODE>
 * method to activate or deactivate this feature and the
 * <CODE>CallControlTerminal.getDoNotDisturb()</CODE> method to return the
 * current state of this attribute.
 * <p>
 * Note that the <CODE>CallControlAddress</CODE> interface also carries the
 * do not disturb attribute. This attribute associated with each are maintained
 * independently. Maintaining a separate do not distrub attribute at terminal
 * and address allows for control over the do not disturb feature at either the
 * terminal or address level.
 *
 * <H4>Picking Up Telephone Calls</H4>
 *
 * The pickup feature permits applications to answer telephone calls which
 * are not ringing at a particular Terminal. This feature is often used when
 * a call is "queued" at an Address or a telephone is ringing at a Terminal
 * across a room. Both the <CODE>pickup()</CODE> and the
 * <CODE>pickupFromGroup()</CODE> methods defined on this interface provide
 * this pickup feature to the application.
 * <p>
 * The <CODE>CallControlTerminal.pickup()</CODE> method has three versions and
 * permits applications to answer a Call at a designated Address or Terminal.
 * The <CODE>CallControlTerminal.pickupFromGroup()</CODE> method permits
 * applications to answer a Call at some other Terminal in the same "pickup
 * group".
 *
 * <H4>Observers and Events</H4>
 *
 * All events pertaining to the <CODE>CallControlTerminal</CODE> interface are
 * reported via the <CODE>TerminalObserver.terminalChangedEvent()</CODE>
 * method. The application observer object must also implement the
 * <CODE>CallControlCallObserver</CODE> interface to express interest in the
 * call control package events.
 * <p>
 * The following are those events associated with this interface:
 * <p>
 * <TABLE CELLPADDING=2>
 * <TR>
 * <TD WIDTH="20%"><CODE>CallCtlTermDoNotDisturbEv</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the do-not-disturb attribute of this Terminal has changed.
 * </TD>
 * </TR>
 * </TABLE>
 * <p>
 * @see javax.telephony.callcontrol.CallControlAddress
 * @see javax.telephony.callcontrol.CallControlTerminalObserver
 * @see javax.telephony.callcontrol.events.CallCtlTermDoNotDisturbEv
 */

public interface CallControlTerminal extends Terminal {

  /**
   * Returns true if the do-not-disturb feature is activated, false otherwise.
   * <p>
   * @return True if do-not-disturb is activated, false if it is deactivated
   * @exception MethodNotSupportedException This method is not supported by the
   * given implementation.
   */
  public boolean getDoNotDisturb()
    throws MethodNotSupportedException;


  /**
   * Specifies whether the do-not-disturb feature should be activated or
   * deactivated for this Terminal. This feature only affects whether or not
   * calls will be accepted at this Terminal. The setting of this attribute
   * does not affect the do-not-disturb attribute associated with all Addresses
   * associated with this Terminal. If 'enable' is true, do-not-disturb is
   * activated if not already so. If 'enable' is false, do-not-disturb is
   * deactivated if not already so.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getDoNotDisturb() == enable
   * <LI>CallCtlTermDoNotDisturbEv is delivered for this Terminal
   * </OL>
   * @see javax.telephony.callcontrol.events.CallCtlTermDoNotDisturbEv
   * @param enable True to activated do-not-disturb, false to deactivate.
   * @exception MethodNotSupportedException This method is not supported by the
   * given implementation.
   * @exception InvalidStateException The Provider is not "in service".
   */
  public void setDoNotDisturb(boolean enable)
    throws MethodNotSupportedException, InvalidStateException;


  /**
   * This method "picks up" a Call at this Terminal. Picking up a Call is
   * analogous to answering a Call at this Terminal
   * (i.e. <CODE>TerminalConnection.answer()</CODE>), except the Call typically
   * is not ringing at this Terminal. For example, this method is used to
   * answer a "queued" Call or a Call which is ringing at another Terminal
   * across the room.
   * <p>
   * This method takes a Connection and an Address as arguments. The Connection
   * argument represents the destination end of the telephone call to be picked
   * up. This Connection must be in either the
   * <CODE>CallControlConnection.QUEUED</CODE> state or the
   * <CODE>CallControlConnection.ALERTING</CODE> state. The Address argument
   * chooses the Address associated with this Terminal on which to pick up the
   * Call. A new TerminalConnection is created and returned which is in the
   * <CODE>CallControlTerminalConnection.TALKING</CODE> state and associated
   * with this Terminal.
   *
   * <H5>The Address and Connection Arguments</H5>
   *
   * The relationship between the Address and Connection arguments affects the
   * resulting behavior of this method. There are two different situations: if
   * the given Connection is associated with the given Address, and if the
   * given Connection is not associated with the given Address (i.e. via the
   * <CODE>Connection.getAddress()</CODE> method).
   * <p>
   * If the given Connection is associated with the given Address, this implies
   * that the Connection was in the <CODE>CallControlConnection.QUEUED</CODE>
   * state, or the Terminal did not ring for some reason even though the
   * Connection is in the <CODE>CallControlConnection.ALERTING</CODE> state. In
   * this case, the Connection moves to the
   * <CODE>CallControlConnection.ESTABLISHED</CODE> state and the new
   * TerminalConnection created is associated with the Connection.
   * <p>
   * If the given Connection is not associated with the given Address, this
   * implies that the call is alerting at an entirely different endpoint from
   * this Terminal. This scenario permits applications to pick up a telephone
   * call which is ringing across the room. In this case, the Connection moves
   * to the <CODE>CallControlConnection.DISCONNECTED</CODE> state. A new
   * Connection is created and associated with the Address argument. It is in
   * the <CODE>CallControlConnection.ESTABLISHED</CODE> state. The new
   * TerminalConnection created is associated with this new Connection.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>(pickupConnection.getCall()).getState() == Call.ACTIVE
   * <LI>pickupConnection.getCallControlState() == CallControlConnection.QUEUED
   * or CallControlConnection.ALERTING
   * <LI>terminaladdress element of this.getAddresses()
   * </OL>
   * <B>Post-conditions Outcome 1:</B>
   * If pickupConnection is associated with terminaladdress (i.e.
   * pickupConnection.getAddress() == terminaladdress)
   * <OL>
   * <LI>Let tc be the new TerminalConnection created and returned
   * <LI>tc.getCallControlState() == CallControlTerminalConnection.TALKING
   * <LI>pickupConnection.getCallControlState() ==
   * CallControlConnection.ESTABLISHED
   * <LI>tc.getConnection() == pickupConnection
   * <LI>TermConnCreatedEv is delivered for tc
   * <LI>CallCtlTermConnTalkingEv/TermConnActiveEv is delivered for tc
   * <LI>CallCtlConnEstablishedEv/ConnConnectedEv is delivered for
   * pickupConnection
   * </OL>
   * <B>Post-conditions Outcome 2:</B>
   * If pickupConnection is not associated with terminaladdress (i.e.
   * pickupConnection.getAddress() != terminaladdress)
   * <OL>
   * <LI>Let tc be the new TerminalConnection created and returned
   * <LI>Let connection be the new Connection created
   * <LI>Let call = pickupConnection.getCall()
   * <LI>tc.getCallControlState() == CallControlTerminalConnection.TALKING
   * <LI>connection.getCallControlState() == CallControlConnection.ESTABLISHED
   * <LI>connection.getAddress() == terminaladdress
   * <LI>connection.getCall() == call
   * <LI>tc.getConnection() == connection
   * <LI>pickupConnection.getCallControlState() ==
   * CallControlConnection.DISCONNECTED
   * <LI>TermConnCreatedEv is delivered for tc
   * <LI>CallCtlTermConnTalkingEv/TermConnActiveEv is delivered for tc
   * <LI>ConnCreatedEv is delivered for connection
   * <LI>CallCtlConnEstablishedEv/ConnConnectedEv is delivered for connection
   * <LI>CallCtlConnDisconnectedEv/ConnDisconnectedEv is delivered for
   * pickupConnection
   * </OL>
   * @see javax.telephony.events.ConnCreatedEv
   * @see javax.telephony.events.TermConnCreatedEv
   * @see javax.telephony.events.ConnConnectedEv
   * @see javax.telephony.events.ConnDisconnectedEv
   * @see javax.telephony.events.TermConnActiveEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnTalkingEv
   * @see javax.telephony.callcontrol.events.CallCtlConnEstablishedEv
   * @see javax.telephony.callcontrol.events.CallCtlConnDisconnectedEv
   * @param pickupConnection The Connection to be picked up
   * @param terminalAddress The Address associated with the Terminal
   * @return The new TerminalConnection associated with the Terminal
   * @exception InvalidArgumentException Either the Connection or Address given
   * as arguments is not valid.
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Connection is not "queued" or "alerting".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public TerminalConnection pickup(Connection pickupConnection,
				   Address terminalAddress)
    throws InvalidArgumentException, InvalidStateException,
      MethodNotSupportedException, PrivilegeViolationException,
      ResourceUnavailableException;


  /**
   * This method "picks up" a Call at this Terminal. Picking up a Call is
   * analogous to answering a Call at this Terminal
   * (i.e. <CODE>TerminalConnection.answer()</CODE>), except the Call typically
   * is not ringing at this Terminal. For example, this method is used to
   * answer a "queued" Call or a Call which is ringing at another Terminal
   * across the room.
   * <p>
   * This method is similar to the
   * <CODE>CallControlTerminal.pickup(Connection, Address)</CODE> method
   * except an explicit TerminalConnection is given. Since an explicit
   * TerminalConnection is given, this implies its Connection must be
   * "alerting" since "queued" Connections may not have any associated
   * TerminalConnections.
   * <p>
   * @param pickTermConn The TerminalConnection to be picked up
   * @param terminalAddress The Address associated with the Terminal
   * @return The new TerminalConnection associated with the Terminal
   * @exception InvalidArgumentException Either the Connection or Address given
   * as arguments is not valid.
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Connection is not "queued".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public TerminalConnection pickup(TerminalConnection pickTermConn,
				   Address terminalAddress)
    throws InvalidArgumentException, InvalidStateException,
      MethodNotSupportedException, PrivilegeViolationException,
      ResourceUnavailableException;


  /**
   * This method "picks up" a Call at this Terminal. Picking up a Call is
   * analogous to answering a Call at this Terminal
   * (i.e. <CODE>TerminalConnection.answer()</CODE>), except the Call typically
   * is not ringing at this Terminal. For example, this method is used to
   * answer a "queued" Call or a Call which is ringing at another Terminal
   * across the room.
   * <p>
   * This method is similar to the
   * <CODE>CallControlTerminal.pickup(Connection, Address)</CODE> method
   * except an Address is given instead of an explicit Connection. This method
   * permits the implementation to choose a suitable Connection associated with
   * 'pickupAddress'.
   * <p>
   * @param pickupAddress The Address which is to be picked up
   * @param terminalAddress The Address associated with the Terminal
   * @return The new TerminalConnection associated with the Terminal
   * @exception InvalidArgumentException Either the Connection or Address given
   * as arguments is not valid.
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Connection is not "queued" or "alerting".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public TerminalConnection pickup(Address pickupAddress,
				   Address terminalAddress)
    throws InvalidArgumentException, InvalidStateException,
      MethodNotSupportedException, PrivilegeViolationException,
      ResourceUnavailableException;


  /**
   * This method "picks up" a Call at this Terminal. Picking up a Call is
   * analogous to answering a Call at this Terminal
   * (i.e. <CODE>TerminalConnection.answer()</CODE>), except the Call typically
   * is not ringing at this Terminal. For example, this method is used to
   * answer a "queued" Call or a Call which is ringing at another Terminal
   * across the room.
   * <p>
   * This method takes two arguments: a string "pickup group" code and an
   * Address associated with this Terminal. The Address argument chooses the
   * Address associated with this Terminal on which to pick up the Call. A new
   * TerminalConnection is created and returned which is in the
   * <CODE>CallControlTerminalConnection.TALKING</CODE> state and associated
   * with this Terminal.
   *
   * <H5>The Pickup Group Code</H5>
   *
   * The application designates the Call to pick up via a string code rather
   * than giving a specific Connection endpoint. An administrator of the
   * underlying telephony platform can create groups of endpoints associated
   * with a particular group code. From the code, the implementations decides
   * which particular Connection is to be picked up. Once the Connection has
   * been determined, this method behavior similarly to the
   * <CODE>CallControlTerminal.pickup(Connection, Address)</CODE> method.
   * <p>
   * @param pickupGroup The pickup group
   * @param terminalAddress The Address associated with the Terminal
   * @return The new TerminalConnection associated with the Terminal
   * @exception InvalidArgumentException Either the Connection or Address given
   * as arguments is not valid.
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Connection is not "queued" or "alerting".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public TerminalConnection pickupFromGroup(String pickupGroup,
					    Address terminalAddress)
    throws InvalidArgumentException, InvalidStateException,
      MethodNotSupportedException, PrivilegeViolationException,
      ResourceUnavailableException;


  /**
   * This method "picks up" a Call at this Terminal. Picking up a Call is
   * analogous to answering a Call at this Terminal
   * (i.e. <CODE>TerminalConnection.answer()</CODE>), except the Call typically
   * is not ringing at this Terminal. For example, this method is used to
   * answer a "queued" Call or a Call which is ringing at another Terminal
   * across the room.
   * <p>
   * This method takes a single argument: an Address associated with this
   * Terminal. The Address argument chooses the Address associated with this
   * Terminal on which to pick up the Call. A new TerminalConnection is
   * created and returned which is in the
   * <CODE>CallControlTerminalConnection.TALKING</CODE> state and associated
   * with this Terminal.
   *
   * <H5>The Pickup Group Code</H5>
   *
   * This method does not take the pickup group code as an argument. Instead,
   * the implementation chooses a suitable Call to be picked up. This Call
   * should have a Connection in either the
   * <CODE>CallControlConnection.ALERTING</CODE> state or the
   * <CODE>CallControlConnection.QUEUED</CODE> state. The Address associated
   * with this Connection should belong to the same pickup group as the
   * Address given as the argument. Once the Connection has been determined,
   * this method behavior similarly to the
   * <CODE>CallControlTerminal.pickup(Connection, Address)</CODE> method.
   * <p>
   * @param terminalAddress The Address associated with the Terminal
   * @return The new TerminalConnection associated with the Terminal
   * @exception InvalidArgumentException Either the Connection or Address given
   * as arguments is not valid.
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Connection is not "queued" or "alerting".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public TerminalConnection pickupFromGroup(Address terminalAddress)
    throws InvalidArgumentException, InvalidStateException,
      MethodNotSupportedException, PrivilegeViolationException,
      ResourceUnavailableException;
}
