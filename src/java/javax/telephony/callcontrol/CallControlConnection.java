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
 * The <CODE>CallControlConnection</CODE> interface extends the core
 * <CODE>Connection</CODE> interface and provides additional features and
 * greater detail about the Connection state. Applications may query a
 * Connection object using the <CODE>instanceof</CODE> operator to see whether
 * it supports this interface.
 *
 * <H4>CallControlConnection State</H4>
 *
 * This interface defines states for the Connection which provide greater
 * detail beyond the states defined in the <CODE>Connection</CODE> interface.
 * The states defined by this interface are related to the states defined in
 * the core package in certain, specific ways, as defined below. Applications
 * may obtain the <CODE>CallControlConnection</CODE> state via the
 * <CODE>getCallControlState()</CODE> method defined on this interface. This
 * method returns one of the integer state constants defined in this interface.
 * <p>
 * Below is a description of each <CODE>CallControlConnection</CODE> state in
 * real-world terms. These real-world descriptions only serve to provide a
 * more intuitive understanding of what is going on. Several methods in this
 * specification state pre-conditions based upon the
 * <CODE>CallControlConnection</CODE> state. Some of these states are identical
 * to those defined in the core package.
 * <p>
 * <TABLE CELLPADDING=2>
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.IDLE</CODE></TD>
 * <TD WIDTH="85%">
 * This state has the same definition as in the core package. It is the
 * initial <CODE>CallControlConnection</CODE> state for all new Connections.
 * Connections typically do not stay in this state for long, but quickly
 * transition to another state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.OFFERED</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates than an incoming call is being offered to the Address
 * associated with the Connection. Typically, applications must either accept
 * or reject this offered call before the called party is alerted to the
 * incoming telephone call.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.QUEUED</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates that a Connection is queued at the particular Address
 * associated with the Connection. For example, some telephony platforms permit
 * the "queueing" of incoming telephone call to an Address when the Address is
 * busy.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.NETWORK_REACHED</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates that an outgoing telephone call has reached the
 * network. Applications may not receive further events about this leg of the
 * telephone call, depending upon the ability of the telephone network to
 * provide additional progress information. Applications must decide whether
 * to treat this as a connected telephone call.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.NETWORK_ALERTING</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates that an outgoing telephone call is alerting at the
 * destination end, which was previously only known to have reached the
 * network. Typically, Connections transition into this state from the
 * <CODE>CallControlConnection.NETWORK_REACHED</CODE> state. This state results
 * from additional progress information being sent from the telephone network. 
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.ALERTING</CODE></TD>
 * <TD WIDTH="85%">
 * This state has the same definition as in the core package. It implies that
 * the Address is being notified of an incoming call.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.INITIATED</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates the originating end of a telephone call has begun
 * the process of placing a telephone call, but the dialing of the destination
 * telephone address has not yet begun. Typically, a telephone associated with
 * the Address has gone "off-hook".
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.DIALING</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates the originating end of a telephone call has begun the
 * process of dialing a destination telephone address, but has not yet
 * completed dialing.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.ESTABLISHED</CODE></TD>
 * <TD WIDTH="85%">
 * This state is similar to that of <CODE>Connection.CONNECTED</CODE>. It
 * indicates that the endpoint has reached its final, active state in the
 * telephone call.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.DISCONNECTED</CODE></TD>
 * <TD WIDTH="85%">
 * This state has the same definition as in the core package. It implies the
 * Connection object is no longer part of the telephone call.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.FAILED</CODE></TD>
 * <TD WIDTH="85%">
 * This state has the same definition as in the core package. It indicates
 * that a particular leg of a telephone call has failed for some reason,
 * perhaps because the party was busy.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>CallControlConnection.UNKNOWN</CODE></TD>
 * <TD WIDTH="85%">
 * This state has the same definition as in the core package. It indicates
 * the implementation is unable to determine the current call control package
 * state of the Connection object. Typically, methods are invalid on this
 * object when it is in this state.
 * </TD>
 * </TR>
 * </TABLE>
 *
 * <H4>State Transitions</H4>
 *
 * Similar to the <CODE>Connection</CODE> state transition diagram, the
 * <CODE>CallControlConnection</CODE> state must transition according to rules
 * illustrated in the finite state diagram below. An implementation must
 * guarantee that <CODE>CallControlConnection</CODE> state must abide by this
 * transition diagram.
 * <p>
 * Note there is a general left-to-right progression of the state transitions.
 * The asterisk next to a state transition, as in the core package, implies
 * a transition to/from another state, except where noted.
 * <p>
 * <IMG SRC="doc-files/callctl-connectionstates.gif" ALIGN="center">
 * </P>
 *
 * <H4>Core vs. CallControl Package States</H4>
 *
 * There is a strong relationship between the
 * <CODE>CallControlConnection</CODE> states and the <CODE>Connection</CODE>
 * states. If an implementation supports the call control package, it must
 * ensure this relationship is properly maintained.
 * <p>
 * Since the states defined in the <CODE>CallControlConnection</CODE> interface
 * provide more detail to the states defined in the <CODE>Connection</CODE>
 * interface, each state in the <CODE>Connection</CODE> interface corresponds
 * to a state defined in the <CODE>CallControlConnection</CODE> interface.
 * Conversely, each <CODE>CallControlConnection</CODE> state corresponds to
 * exactly one <CODE>Connection</CODE> state. This arrangement permits
 * applications to view either the core state or the call control state and
 * still see a consistent view.
 * <p>
 * The following table outlines the relationship between the core package
 * Connection states and the call control package Connection states.
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
 *
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.IDLE</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.IDLE</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.QUEUED</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.INPROGRESS</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.OFFERED</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.INPROGRESS</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.ALERTING</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.ALERTING</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.INITIATED</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.CONNECTED</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.DIALING</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.CONNECTED</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.NETWORK_REACHED</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.CONNECTED</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.NETWORK_ALERTING</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.CONNECTED</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.ESTABLISHED</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.CONNECTED</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.DISCONNECTED</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.DISCONNECTED</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.FAILED</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.FAILED</CODE></TD>
 * </TR>
 * <TR>
 * <TD WIDTH="60%"><CODE>CallControlConnection.UNKNOWN</CODE></TD>
 * <TD WIDTH="40%"><CODE>Connection.UNKNOWN</CODE></TD>
 * </TR>
 * </TABLE>
 *
 * <H4>Observers and Events</H4>
 *
 * All events pertaining to the <CODE>CallControlConnection</CODE> interface
 * are reported via the <CODE>CallObserver.callChangedEvent()</CODE> method.
 * The application observer object must also implement the
 * <CODE>CallControlCallObserver</CODE> interface to express interest in the
 * call control package events.
 * <p>
 * Observers which are registered on a Call receive events when the
 * <CODE>CallControlConnection</CODE> state changes. Note that when the
 * <CODE>CallControlConnection</CODE> state changes, it sometimes results in
 * the <CODE>Connection</CODE> state changing (according to the table above).
 * In these instances, both the proper call control and core package events are
 * delivered to the observer.
 * <p>
 * The <CODE>CallControlConnection</CODE> state events defined in this package
 * are: <CODE>CallCtlConnOfferedEv</CODE>, <CODE>CallCtlConnQueuedEv</CODE>,
 * <CODE>CallCtlConnAlertingEv</CODE>, <CODE>CallCtlConnInitiatedEv</CODE>,
 * <CODE>CallCtlConnDialingEv</CODE>, <CODE>CallCtlConnNetworkReachedEv</CODE>,
 * <CODE>CallCtlConnNetworkAlertingEv</CODE>, <CODE>CallCtlConnFailedEv</CODE>,
 * <CODE>CallCtlConnEstablishedEv</CODE>, <CODE>CallCtlConnUnknownEv</CODE>, 
 * and <CODE>CallCtlConnDisconnectedEv</CODE>.
 * <p>
 * @see javax.telephony.Connection
 * @see javax.telephony.CallObserver
 * @see javax.telephony.callcontrol.CallControlCallObserver
 * @see javax.telephony.callcontrol.events.CallCtlCallEv
 * @see javax.telephony.callcontrol.events.CallCtlConnEv
 * @see javax.telephony.callcontrol.events.CallCtlConnAlertingEv
 * @see javax.telephony.callcontrol.events.CallCtlConnDialingEv
 * @see javax.telephony.callcontrol.events.CallCtlConnDisconnectedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnEstablishedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnFailedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnInitiatedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnNetworkAlertingEv
 * @see javax.telephony.callcontrol.events.CallCtlConnNetworkReachedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnOfferedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnQueuedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnUnknownEv
 */

public interface CallControlConnection extends Connection {

  /**
   * The <CODE>CallControlConnection.IDLE</CODE> state has the same definition
   * as in the core package. It is the initial
   * <CODE>CallControlConnection</CODE> state for all new Connections. 
   * Connections typically do not stay in this state for long, quickly
   * transitioning to another state.
   */
  public static final int IDLE = 0x50;


  /**
   * The <CODE>CallControlConnection.OFFERED</CODE> state indicates than an
   * incoming call is being offered to the Address associated with the
   * Connection. Typically, applications must either accept or reject this
   * offered call before the called party is alerted to the incoming telephone
   * call.
   * <p>
   * @since JTAPI v1.2
   */
  public static final int OFFERED = 0x51;


  /**
   * The <CODE>CallControlConnection.QUEUED</CODE> state indicates that a
   * Connection is queued at the particular Address associated with the
   * Connection. A queued call is not active on a call.  For example, some 
   * telephony platforms permit the "queueing"
   * of incoming telephone call to an Address when the Address is busy.
   */
  public static final int QUEUED = 0x52;


  /**
   * The <CODE>CallControlConnection.ALERTING</CODE> state has the same
   * definition as in the core package. It means that the Address is being
   * notified of an incoming call.
   */
  public static final int ALERTING = 0x53;


  /**
   * The <CODE>CallControlConnection.INITIATED</CODE> state indicates the
   * originating end of a telephone call has begun the process of placing a
   * telephone call, but the dialing of the destination telephone address has
   * not yet begun. Typically, a telephone associated with the Address has
   * gone "off-hook".
   */
  public static final int INITIATED = 0x54;


  /**
   * The <CODE>CallControlConnection.DIALING</CODE> state indicates the
   * originating end of a telephone call has begun the process of dialing a
   * destination telephone address, but has not yet completed.
   */
  public static final int DIALING = 0x55;


  /**
   * The <CODE>CallControlConnection.NETWORK_REACHED</CODE> state indicates
   * that an outgoing telephone call has reached the network. Applications may
   * not receive further events about this leg of the telephone call, depending
   * upon the ability of the telephone network to provide additional progress
   * information. Applications must decide whether to treat this as a
   * connected telephone call.
   */
  public static final int NETWORK_REACHED = 0x56;


  /**
   * The <CODE>CallControlConnection.NETWORK_ALERTING</CODE> state indicates
   * that an outgoing telephone call is alerting at the destination end, which
   * was previously only known to have reached the network. Typically,
   * Connections transition into this state from the
   * <CODE>CallControlConnection.NETWORK_REACHED</CODE> state. This state
   * results from additional progress information being sent from a
   * telephone network that was capable of transmitting that information.
   */
  public static final int NETWORK_ALERTING = 0x57;


  /**
   * The <CODE>CallControlConnection.ESTABLISHED</CODE> state is similar to
   * that of <CODE>Connection.CONNECTED</CODE>. It indicates that the endpoint
   * has reached its final, active state in the telephone call.
   */
  public static final int ESTABLISHED = 0x58;


  /**
   * The <CODE>CallControlConnection.DISCONNECTED</CODE> state has the same
   * definition as in the core package. It indicates that the Connection object 
   * is no longer part of the telephone call.
   */
  public static final int DISCONNECTED = 0x59;


  /**
   * The <CODE>CallControlConnection.FAILED</CODE> state has the same
   * definition as in the core package. It indicates that a Connection
   * is no longer able to participate in a call.  It is a final state in
   * the life of a Connection.  It indicates that a particular leg of
   * a telephone call has failed for some reason, perhaps because the party
   * was busy.
   */
  public static final int FAILED = 0x5A;


  /**
   * The <CODE>CallControlConnection.UNKNOWN</CODE> state has the same
   * definition as in the core package. It indicates that the state of
   * the Connection is not known to its Provider. Typically, methods are 
   * invalid on this object when it is in this state.
   */
  public static final int UNKNOWN = 0x5B;


  /**
   * The <CODE>CallControlConnection.OFFERING</CODE> state has been deprecated
   * in JTAPI v1.2. It has the same meaning as the new
   * <CODE>CallControlConnection.OFFERED</CODE> state. This constant has been
   * replaced to be more tense-consistent with the event name.
   * <p>
   * @deprecated Since JTAPI v1.2.
   */
  public static final int OFFERING = 0x51;


  /**
   * Returns the current call control state of the Connection. The return value
   * will be one of integer state constants defined above.
   * <p>
   * @return The current call control state of the Connection.
   */
  public int getCallControlState();


  /**
   * Accepts a telephone call incoming to an Address. Telephone calls into an
   * Address may first be <EM>offered</EM> to that address for acceptance
   * before the standard notion of "alerting" takes place. This method is valid
   * on a Connection in the <CODE>CallControlConnection.OFFERED</CODE> state.
   * If successful, this method moves the Connection into the
   * <CODE>CallControlConnection.ALERTING</CODE> state. This method waits
   * until the telephone call has been accepted or an error occurs and an
   * exception is thrown.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlConnection.OFFERED
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlConnection.ALERTING
   * <LI>CallCtlConnAlertingEv/ConnAlertingEv is delivered for this Connection
   * </OL>
   * @see javax.telephony.events.ConnAlertingEv
   * @see javax.telephony.callcontrol.events.CallCtlConnAlertingEv
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Connection is not "offered".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public void accept()
    throws InvalidStateException, MethodNotSupportedException,
      PrivilegeViolationException, ResourceUnavailableException;


  /**
   * Rejects a telephone call incoming to an Address. Telephone calls into an
   * Address may first be <EM>offered</EM> to that address for acceptance
   * before the standard notion of "alerting" takes place. This method is valid
   * on a Connection in the <CODE>CallControlConnection.OFFERED</CODE> state.
   * If successful, this method moves the Connection into the
   * <CODE>CallControlConnection.DISCONNECTED</CODE> state. This method
   * waits until the telephone call has been rejected or an error occurs and
   * an exception is thrown.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlConnection.OFFERED
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlConnection.DISCONNECTED
   * <LI>CallCtlConnDisconnectedEv/ConnDisconnectedEv is delivered for this
   * Connection
   * </OL>
   * @see javax.telephony.events.ConnDisconnectedEv
   * @see javax.telephony.callcontrol.events.CallCtlConnDisconnectedEv
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Connection is not "offered".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public void reject()
    throws InvalidStateException, MethodNotSupportedException,
      PrivilegeViolationException, ResourceUnavailableException;

 
  /**
   * Redirects an incoming telephone call at an Address to another telephone
   * address. This method is very similar to the transfer feature, however,
   * applications may invoke this method before first answering the telephone
   * call. This method redirects the telephone call to another telephone
   * address string provided as the argument to this method. This telephone
   * address string must be valid and complete.
   * <p>
   * This Connection must either be in the
   * <CODE>CallControlConnection.OFFERED</CODE> state or the
   * <CODE>CallControlConnection.ALERTING</CODE> state. If successful, this
   * method moves the Connection to the
   * <CODE>CallControlConnection.DISCONNECTED</CODE> state. Additionally, any
   * TerminalConnections associated with this Connection will move to the
   * <CODE>CallControlTerminalConnection.DROPPED</CODE> state.
   * <p>
   * A new Connection is created and returned corresponding to the new
   * destination leg of the telephone call. Note that this Connection may be
   * <CODE>null</CODE> in the case the Call has been redirected outside of
   * the Provider's domain and can no longer be tracked. The new Connection (if
   * not null) must at least be in the <CODE>CallControlConnection.IDLE</CODE>
   * state. The Connection may progress beyond this state before this method
   * returns, which should be reflected by the proper events. This Connection
   * behaves similarly to the destination Connection as described in
   * <CODE>Call.connect()</CODE> and undergoes similar possible state change
   * scenarios.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlConnection.OFFERED or
   * CallControlConnection.ALERTING
   * <LI>destinationAddress must be a valid and complete telephone address
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let newconnection be the new Connection created and returned
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlConnection.DISCONNECTED
   * <LI>newconnection element of (this.getCall()).getConnections()
   * <LI>newconnection.getCallControlState() at least
   * CallControlConnection.IDLE
   * <LI>Let tc[] = this.getTerminalConnections() before this method is invoked
   * <LI>tc[i].getCallControlState() == CallControlTerminalConnection.DROPPED,
   * for all i.
   * <LI>CallCtlConnDisconnected/ConnDisconnectedEv is delivered for this
   * Connection
   * <LI>CallCtlTermConnDroppedEv/TermConnDroppedEv is delivered for all tc[i]
   * <LI>ConnCreatedEv is delivered for newconnection
   * </OL>
   * @see javax.telephony.events.ConnCreatedEv
   * @see javax.telephony.events.ConnDisconnectedEv
   * @see javax.telephony.events.TermConnDroppedEv
   * @see javax.telephony.callcontrol.events.CallCtlConnDisconnectedEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnDroppedEv
   * @param destinationAddress The Connection is redirected to this telephone 
   * address
   * @return The Connection associated with the new leg of the Call.
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Connection is not "offered" or "alerting".
   * @exception InvalidPartyException The destination address to which this
   * call is redirected is not valid and/or complete.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public Connection redirect(String destinationAddress)
    throws InvalidStateException, InvalidPartyException,
      MethodNotSupportedException, PrivilegeViolationException,
      ResourceUnavailableException;


  /**
   * Appends additional address information onto an existing Connection. This
   * method is used when part of a telephone address string has been dialed
   * and additional addressing information is needed in order to complete the
   * dialing process and place the telephone call. The additional addressing
   * information is provided as the argument to this method.
   * <p>
   * This Connection must either be in the
   * <CODE>CallControlConnection.DIALING</CODE> state or the
   * <CODE>CallControlConnection.INITIATED</CODE> state. If successful, this
   * moves the Connection into one of two states. If the information provided
   * completes the addressing information, as determined by the telephony
   * platform, the Connection moves into the
   * <CODE>CallControlConnection.ESTABLISHED</CODE> state and the telephone
   * call is placed. If additional addressing information is still required,
   * the Connection moves into the <CODE>CallControlConnection.DIALING</CODE>
   * state if not already there.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlConnection.DIALING or
   * CallControlConnection.INITIATED
   * </OL>
   * <B>Post-conditions Outcome 1:</B>
   * The addressing information is complete.
   * <OL>
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlConnection.ESTABLISHED
   * <LI>CallCtlConnEstablishedEv is delivered for this Connection
   * </OL>
   * <B>Post-conditions Outcome 2:</B>
   * The addressing information is not complete.
   * <OL>
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlConnection.DIALING 
   * <LI>CallCtlConnDialingEv is delivered for this Connection
   * </OL>
   * @see javax.telephony.callcontrol.events.CallCtlConnDialingEv
   * @see javax.telephony.callcontrol.events.CallCtlConnEstablishedEv
   * @param additionalAddress The additional addressing information.
   * @exception InvalidStateException Either the Provider is not "in service"
   * or the Connection is not "initiated" or "dialing".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public void addToAddress(String additionalAddress)
    throws InvalidStateException, MethodNotSupportedException,
      PrivilegeViolationException, ResourceUnavailableException;


  /**
   * Parks a Connection at a destination telephone address. This method
   * is similar to the transfer feature, except the Connection at the new
   * destination Address is in a special <EM>queued</EM> state. Parking a
   * Connection at a destination Address drops the Connection from the Call and
   * creates and returns a new Connection at the specified destination address
   * in the <CODE>CallControlConnection.QUEUED</CODE> state.
   * <p>
   * The new destination telephone address string is given as an argument to
   * this method and must be a valid and complete telephone address. The
   * <CODE>CallControlTerminal.pickup()</CODE> method permits applications to
   * "unpark" the new Connection.
   * <p>
   * The Connection must be in the
   * <CODE>CallControlConnection.ESTABLISHED</CODE> state. If this method
   * is successful, this Connection moves to the
   * <CODE>CallControlConnection.DISCONNECTED</CODE> state. All of its
   * associated TerminalConnections move to the
   * <CODE>CallControlTerminalConnection.DROPPED</CODE> state.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getCallControlState() == CallControlConnection.ESTABLISHED
   * <LI>destinationAddress must be a valid and complete telephone address.
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let newconnection be the new Connection created and returned
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>newconnection element of (this.getCall()).getConnections()
   * <LI>newconnection.getCallControlState() == CallControlConnection.QUEUED
   * <LI>this.getCallControlState() == CallControlConnection.DISCONNECTED
   * <LI>Let tc[] = this.getTerminalConnections() before this method is invoked
   * <LI>tc[i].getCallControlState() == CallControlTerminalConnection.DROPPED,
   * for all i
   * <LI>ConnCreatedEv is delivered for newconnection
   * <LI>CallCtlConnQueuedEv/ConnInProgressEv is delivered for newconnection
   * <LI>CallCtlConnDisconnected/ConnDisconnectedEv is delivered for this
   * Connection
   * <LI>CallCtlTermConnDroppedEv/TermConnDroppedEv is delivered for all tc[i]
   * </OL>
   * @see javax.telephony.events.ConnCreatedEv
   * @see javax.telephony.events.ConnInProgressEv
   * @see javax.telephony.events.ConnDisconnectedEv
   * @see javax.telephony.events.TermConnDroppedEv
   * @see javax.telephony.callcontrol.events.CallCtlConnQueuedEv
   * @see javax.telephony.callcontrol.events.CallCtlConnDisconnectedEv
   * @see javax.telephony.callcontrol.events.CallCtlTermConnDroppedEv
   * @param destinationAddress The telephone address string at which this
   * connection is to be parked.
   * @return The new Connection which is parked at the new destination Address.
   * @exception InvalidStateException Either the Provider was not "in service"
   * or the Connection was not "established".
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception InvalidPartyException The party to which to party the
   * Connection is invalid.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to invoke this method.
   * @exception ResourceUnavailableException An internal resource necessary
   * for the successful invocation of this method is not available.
   */
  public Connection park(String destinationAddress)
    throws InvalidStateException, MethodNotSupportedException,
      PrivilegeViolationException, InvalidPartyException,
      ResourceUnavailableException;
}
