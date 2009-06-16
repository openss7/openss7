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
 * The <CODE>CallControlTerminalConnection</CODE> interface extends the core
 * <CODE>TerminalConnection</CODE> interface with additional
 * features and greater detail about the TerminalConnection state. Applications
 * may query a TerminalConnection object using the <CODE>instanceof</CODE>
 * operator to see whether it supports this interface.
 *
 * <H4>CallControlTerminalConnection State</H4>
 *
 * This interface defines states for the TerminalConnection which provide
 * greater detail beyond the states defined in the
 * <CODE>TerminalConnection</CODE> interface. These states are related to the
 * states defined in the core package in certain, specific ways, as defined
 * below. Applications may obtain the
 * <CODE>CallControlTerminalConnection</CODE> state via the
 * <CODE>getCallControlState()</CODE> method defined on this interface. This
 * method returns one of the integer state constants defined in this interface.
 * <p>
 * Below is a description of each <CODE>CallControlTerminalConnection</CODE>
 * state in real-world terms. These real-world descriptions only serve to
 * provide a more intuitive understanding of what is going on. Several methods
 * in this specification state pre-conditions based upon the
 * <CODE>CallControlTerminalConnection</CODE> state. Some of these states are
 * identical to those defined in the core package.
 * <p>
 * <TABLE CELLPADDING=2>
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlTerminalConnection.IDLE</CODE></TD>
 * <TD WIDTH="85%">
 * This state has the same definition as in the core package. It is the initial
 * <CODE>CallControlTerminalConnection</CODE> state for all new
 * TerminalConnection objects. TerminalConnections typically do not stay in
 * this state for long, quickly transitioning to another state.
 * </TD>
 * </TR>
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlTerminalConnection.RINGING</CODE></TD>
 * <TD WIDTH="85%">
 * This state has the same definition as in the core package. It indicates
 * that the associated Terminal is ringing and has an incoming telephone call.
 * </TD>
 * </TR>
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlTerminalConnection.TALKING</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates that the Terminal is actively part of a Call, is
 * typically "off-hook", and the party is communicating on the telephone call.
 * </TD>
 * </TR>
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlTerminalConnection.HELD</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates that a Terminal is part of a Call, but is on hold.
 * Other Terminals which are on the same Call and associated with the same
 * Connection may or may not also be in this state.
 * </TD>
 * </TR>
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlTerminalConnection.BRIDGED</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates that a Terminal is currently bridged into a Call. A
 * Terminal may typically join a Call when it is bridged. A bridged Terminal
 * is part of the telephone call, but not active. Typically, some hardware
 * resource is occupied while a Terminal is bridged into a Call.
 * </TD>
 * </TR>
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlTerminalConnection.INUSE</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates that a Terminal hardware resource is
 * currently in use. The terminal is not available for the Call associated
 * with this Terminal Connection, that is the Terminal may not join the
 * call.  This state is similar to the
 * <CODE>CallControlTerminalConnection.BRIDGED</CODE> state except that the
 * Terminal may not join the Call.
 * </TD>
 * </TR>
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlTerminalConnection.DROPPED</CODE></TD>
 * <TD WIDTH="85%">
 * This state has the same definition as in the core package. It indicates
 * that a particular Terminal has permanently left the Call.
 * </TD>
 * </TR>
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlTerminalConnection.UNKNOWN</CODE></TD>
 * <TD WIDTH="85%">
 * This state has the same definition as in the core package. It indicates
 * that the implementation is unable to determine the state of the
 * TerminalConnection. TerminalConnections may transition into and out of this
 * state at any time.
 * </TD>
 * </TR>
 * </TABLE>
 *
 * <H4>State Transitions</H4>
 *
 * Similar to the <CODE>TerminalConnection</CODE> state transition diagram,
 * the <CODE>CallControlTerminalConnection</CODE> state must transition
 * according to the rules illustrated in the finite state diagram below. The
 * implementation must guarantee that the
 * <CODE>CallControlTerminalConnection</CODE> state abides by this transition
 * diagram.
 * <p>
 * The asterisk next to a state transition, as in the core package, implies
 * a transition to/from another state as designated by the direction of the
 * transition arrow.
 * <P>
 * <IMG SRC="doc-files/callctl-terminalconnectionstates.gif" ALIGN="center">
 * </P>
 *
 * <H4>Core vs. CallControl Package States</H4>
 *
 * There is a strong relationship between the <CODE>TerminalConnection</CODE>
 * states and the <CODE>CallControlTerminalConnection</CODE> states. If an
 * implementation supports the call control package, it must ensure this
 * relationship is properly maintained.
 * <p>
 * Since the states defined in the <CODE>CallControlTerminalConnection</CODE>
 * interface provide more detail to the states defined in the
 * <CODE>TerminalConnection</CODE> interface, each state in the
 * <CODE>TerminalConnection</CODE> interface corresponds to a state defined in
 * the <CODE>CallControlTerminalConnection</CODE> interface. Conversely, each
 * <CODE>CallControlTerminalConnection</CODE> state corresponds to exactly one
 * <CODE>TerminalConnection</CODE> state. This arrangement permits applications
 * to view either the core state or the call control state and still see a
 * consistent view.
 * <p>
 * The following table outlines the relationship between the core package
 * TerminalConnection states and the call control package TerminalConnection
 * states.
 * <p>
 * <TABLE CELLPADDING=2>
 * <TR>
 * <TD WIDTH="60%">
 * <STRONG><EM>If the call control package state is...</EM></STRONG>
 * </TD>
 * <TD WIDTH="40%">
 * <STRONG><EM>then the core package state must be...</EM></STRONG>
 * </TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlTerminalConnection.IDLE</CODE></TD>
 * <TD WIDTH="40%"><CODE>TerminalConnection.IDLE</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlTerminalConnection.RINGING</CODE></TD>
 * <TD WIDTH="40%"><CODE>TerminalConnection.RINGING</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlTerminalConnection.TALKING</CODE></TD>
 * <TD WIDTH="40%"><CODE>TerminalConnection.ACTIVE</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlTerminalConnection.HELD</CODE></TD>
 * <TD WIDTH="40%"><CODE>TerminalConnection.ACTIVE</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlTerminalConnection.INUSE</CODE></TD>
 * <TD WIDTH="40%"><CODE>TerminalConnection.PASSIVE</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlTerminalConnection.BRIDGED</CODE></TD>
 * <TD WIDTH="40%"><CODE>TerminalConnection.PASSIVE</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlTerminalConnection.DROPPED</CODE></TD>
 * <TD WIDTH="40%"><CODE>TerminalConnection.DROPPED</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlTerminalConnection.UNKNOWN</CODE></TD>
 * <TD WIDTH="40%"><CODE>TerminalConnection.UNKNOWN</CODE></TD>
 * </TR>
 * </TABLE>
 *
 * <H4>Observers and Events</H4>
 *
 * All events pertaining to the <CODE>CallControlTerminalConnection</CODE>
 * interface are reported via the <CODE>CallObserver.callChangedEvent()</CODE>
 * method. The application observer object must also implement the
 * <CODE>CallControlCallObserver</CODE> interface to express interest in the
 * call control package events. Applications receive TerminalConnection-related
 * events in the call control package when the call control state changes.
 * <p>
 * Observers which are registered on a Call receive events when the
 * <CODE>CallControlTerminalConnection</CODE> state changes. Note that when the
 * <CODE>CallControlTerminalConnection</CODE> state changes, it sometimes
 * results in the <CODE>TerminalConnection</CODE> state changing (according to
 * the table above). In these instances, both the proper call control and core
 * package events are delivered to the observer.
 * <p>
 * The <CODE>CallControlTerminalConnection</CODE> state events defined in this
 * package are: <CODE>CallCtlTermConnBridgedEv</CODE>,
 * <CODE>CallCtlTermConnDroppedEv</CODE>, <CODE>CallCtlTermConnHeldEv</CODE>,
 * <CODE>CallCtlTermConnInUseEv</CODE>, <CODE>CallCtlTermConnRingingEv</CODE>,
 * <CODE>CallCtlTermConnTalkingEv</CODE>, and
 * <CODE>CallCtlTermConnUnknownEv</CODE>.
 * <p>
 * @see javax.telephony.TerminalConnection
 * @see javax.telephony.CallObserver
 * @see javax.telephony.callcontrol.CallControlCallObserver
 * @see javax.telephony.callcontrol.events.CallCtlCallEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnRingingEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnTalkingEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnHeldEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnBridgedEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnInUseEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnDroppedEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnUnknownEv
 */

public interface CallControlTerminalConnection extends TerminalConnection {

  /**
   * The <CODE>CallControlTerminalConnection.IDLE</CODE> state has the same
   * definition as in the core package. It is the initial
   * <CODE>CallControlTerminalConnection</CODE> state for all new
   * TerminalConnections. TerminalConnections typically do not stay in this
   * state for long, but quickly transition to another state.
   */
  public static final int IDLE = 0x60;


  /**
   * The <CODE>CallControlTerminalConnection.RINGING</CODE> state has the same
   * definition as in the core package. It indicates that the associated 
   * Terminal is ringing and has an incoming telephone call.
   */
  public static final int RINGING = 0x61;


  /**
   * The <CODE>CallControlTerminalConnection.TALKING</CODE> state indicates
   * that a Terminal is actively part of a Call, is typically "off-hook", and the
   * party is communicating on the telephone call.
   */
  public static final int TALKING = 0x62;


  /**
   * The <CODE>CallControlTerminalConnection.HELD</CODE> state indicates that
   * a Terminal is part of a Call, but is on hold. Other Terminals which are
   * on the same Call and associated with the same Connection may or may not
   * also be in this state.
   */
  public static final int HELD = 0x63;


  /**
   * The <CODE>CallControlTerminalConnection.BRIDGED</CODE> state indicates
   * that a Terminal is currently bridged into a Call. A Terminal may
   * typically join a telephone call when it is bridged. A bridged Terminal is
   * part of the telephone call, but not active. Typically, some hardware
   * resource is occupied while a Terminal is bridged into a Call.
   */
  public static final int BRIDGED = 0x64;


  /**
   * The <CODE>CallControlTerminalConnection.INUSE</CODE> state indicates that
   * a Terminal is part of a Call, but is not active. It may not Call, however
   * the resource on the Terminal is currently in use. This state is similar
   * to the <CODE>CallControlTerminalConnection.BRIDGED</CODE> state however,
   * the Terminal may not join the Call.
   */
  public static final int INUSE = 0x65;


  /**
   * The <CODE>CallControlTerminalConnection.DROPPED</CODE> state has the same
   * definition as in the core package. It indicates that a particular
   * Terminal has permanently left the Call.
   */
  public static final int DROPPED = 0x66;


  /**
   * The <CODE>CallControlTerminalConnection.UNKNOWN</CODE> state has the same
   * definition as in the core package. It indicates that the implementation
   * is unable to determine the state of the TerminalConnection.
   * TerminalConnections may transition into and out of this state at any time.
   */
  public static final int UNKNOWN = 0x67;


  /**
   * Returns the call control state of the TerminalConnection. The return
   * values will be one of the integer state constants defined above.
   * <p>
   * @return The current call control state of the TerminalConnection.
   */
  public int getCallControlState();


  /**
   * Places a TerminalConnection on hold with respect to the Call of which it
   * is a part. Many Terminals may be on the same Call and associated with the
   * same Connection. Any one of them may go "on hold" at any time, provided
   * they are active in the Call. The TerminalConnection must be in the
   * <CODE>CallControlTerminalConnection.TALKING</CODE> state. This method
   * returns when the TerminalConnection has moved to the
   * <CODE>CallControlTerminalConnection.HELD</CODE> state, or until an error
   * occurs and an exception is thrown.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>(this.getTerminal()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlTerminalConnection.TALKING
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>(this.getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlTerminalConnection.HELD
   * <LI>CallCtlTermConnHeldEv is delivered for this TerminalConnection
   * </OL>
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the TerminalConnection is not "talking".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   * @see javax.telephony.callcontrol.events.CallCtlTermConnHeldEv
   */
  public void hold()
    throws InvalidStateException, MethodNotSupportedException,
      PrivilegeViolationException, ResourceUnavailableException;


  /**
   * Takes a TerminalConnection off hold with respect to the Call of which it
   * is a part. Many Terminals may be on the same Call and associated with the
   * same Connection. Any one of them may go "on hold" at any time, provided
   * they are active in the Call. The TerminalConnection must be in the
   * <CODE>CallControlTerminalConnection.HELD</CODE> state. This method returns
   * successfully when the TerminalConnection moves into the
   * <CODE>CallControlTerminalConnection.TALKING</CODE> state or until an error
   * occurs and an exception is thrown.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>((this.getTerminal()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlTerminalConnection.HELD
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>((this.getTerminal()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlTerminalConnection.TALKING
   * <LI>CallCtlTermConnTalkingEv is delivered for this TerminalConnection
   * </OL>
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the TerminalConnection is not "held".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   * @see javax.telephony.callcontrol.events.CallCtlTermConnTalkingEv
   */
  public void unhold()
    throws InvalidStateException, MethodNotSupportedException,
      PrivilegeViolationException, ResourceUnavailableException;


  /**
   * Makes a currently bridged TerminalConnection active on a Call. Other
   * Terminals, which share the same Address as this Terminal, may be active
   * on the same Call. This situation is known as <EM>bridging</EM>. The
   * TerminalConnection must be in the
   * <CODE>CallControlTerminalConnection.BRIDGED</CODE> state. This method
   * returns which the TerminalConnection has moved to the
   * <CODE>CallControlTerminalConnection.TALKING</CODE> state or until an error
   * occurs and an exception is thrown.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>((this.getTerminal()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlTerminalConnection.BRIDGED
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>((this.getTerminal()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlTerminalConnection.TALKING
   * <LI>CallCtlTermConnTalkingEv/TermConnActiveEv is delivered for this
   * TerminalConnection
   * </OL>
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the TerminalConnection is not "bridged".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   * @see javax.telephony.events.TermConnActiveEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnTalkingEv
   */
  public void join()
    throws InvalidStateException, MethodNotSupportedException,
      PrivilegeViolationException, ResourceUnavailableException;


  /**
   * Places a currently active TerminalConnection in a <EM>bridged</EM> state on a 
   * Call. Other Terminals, which share the same Address as this Terminal, may remain
   * active on the same Call. This situation where Terminals share an Address
   * on a call is known as <EM>bridging</EM>. The TerminalConnection on which this
   * method is invoked must be in the 
   * <CODE>CallControlTerminalConnection.TALKING</CODE> state.
   * <p>
   * There are two possible outcomes of this method depending upon the number
   * of remaining, active TerminalConnections on the Call. If there are other
   * active TerminalConnections, then this TerminalConnection moves into the
   * <CODE>CallControlTerminalConnection.BRIDGED</CODE> state and this method
   * returns. If there are no other active TerminalConnections, then this
   * TerminalConnection moves into the
   * <CODE>CallControlTerminalConnection.DROPPED</CODE> state. Its associated
   * Connection moves into the <CODE>CallControlConnection.DISCONNECTED</CODE>
   * state, i.e. the entire endpoint leaves the telephone call. This method
   * waits until one of these two outcomes occurs or until an error occurs and
   * an exception is thrown.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>((this.getTerminal()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlTerminalConnection.TALKING
   * </OL>
   * <B>Post-conditions Outcome 1:</B>
   * There are no other active TerminalConnections on this Call (no others
   * which are either "held" or "talking"
   * <OL>
   * <LI>((this.getTerminal()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>Let connection = this.getConnection()
   * <LI>Let tc[] = connection.getTerminalConnections() before this method is
   * invoked
   * <LI>tc[i].getCallControlState() == CallControlTerminalConnection.DROPPED,
   * for all i
   * <LI>connection.getCallControlState() == CallControlConnection.DISCONNECTED
   * <LI>CallCtlTermConnDroppedEv/TermConnDroppedEv is delivered for all tc[i]
   * <LI>CallCtlConnDisconnectedEv/ConnDisconnectedEv is delivered for
   * connection
   * </OL>
   * <B>Post-conditions Outcome 2:</B>
   * There are other active TerminalConnections on this Call (others which are
   * either "held" or "talking"
   * <OL>
   * <LI>((this.getTerminal()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlTerminalConnection.BRIDGED
   * <LI>CallCtlTermConnBridgedEv/TermConnPassiveEv is delivered for this
   * TerminalConnection
   * </OL>
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the TerminalConnection is not "talking".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   * @see javax.telephony.events.TermConnPassiveEv
   * @see javax.telephony.events.TermConnDroppedEv
   * @see javax.telephony.events.ConnDisconnectedEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnBridgedEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnDroppedEv
   * @see javax.telephony.callcontrol.events.CallCtlConnDisconnectedEv
   */
  public void leave()
    throws InvalidStateException, MethodNotSupportedException,
      PrivilegeViolationException, ResourceUnavailableException;
}
