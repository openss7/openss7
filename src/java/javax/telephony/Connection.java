/*
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

package javax.telephony;
import  javax.telephony.capabilities.ConnectionCapabilities;

/**
 * A <CODE>Connection</CODE> represents a link (i&#46e&#46 an association) between 
 * a {@link Call} object and an {@link Address} object. 
 * 
 * <H4>Introduction</H4>
 *
 * The purpose of a Connection object is to describe
 * the relationship between a Call object and an Address object. A Connection
 * object exists if the Address is a part of the telephone call. Each
 * Connection has a <EM>state</EM> which describes the particular stage of the
 * relationship between the Call and Address. These states and their meanings
 * are described below. Applications use the <CODE>Connection.getCall()</CODE>
 * and <CODE>Connection.getAddress()</CODE> methods to obtain the Call and
 * Address associated with this Connection, respectively.
 * <p>
 * From one perspective, an application may view a Call only in terms of the
 * Address/Connection objects which are part of the Call. This is termed a
 * <EM>logical</EM> view of the Call because it ignores the details provided
 * by the Terminal and TerminalConnection objects which are also associated
 * with a Call. In many instances, simple applications (such as an
 * <EM>outcall</EM> program) may only need to concern itself with the logical
 * view. In this logical view, a telephone call is views as two or more
 * endpoint addresses in communication. The Connection object describes the
 * state of each of these endpoint addresses with respect to the Call.
 *
 * <H4>Calls and Addresses</H4>
 *
 * Connection objects are immutable in terms of their Call and Address
 * references. In other words, the Call and Address object references do
 * not change throughout the lifetime of the Connection object instance. The
 * same Connection object may not be used in another telephone call. The
 * existence of a Connection implies that its Address is associated with its
 * Call in the manner described by the Connection's state.
 * <p>
 * Although a Connection's Address and Call references remain valid throughout
 * the lifetime of the Connection object, the same is not true for the Call
 * and Address object's references to this Connection. Particularly, when a 
 * Connection moves into the Connection.DISCONNECTED state, it is no longer
 * listed by the <CODE>Call.getConnections()</CODE> and
 * <CODE>Address.getConnections()</CODE> methods. Typically, when a Connection
 * moves into the <CODE>Connection.DISCONNECTED</CODE> state, the application
 * loses its references to it to facilitate its garbage collection.
 *
 * <H4>TerminalConnections</H4>
 *
 * Connections objects are containers for zero or more TerminalConnection
 * objects. Connection objects are containers for zero or more TerminalConnection
 * objects. Connection objects represent the relationship between the Call and
 * the Address, whereas TerminalConnection objects represent the relationship
 * between the Connection and the Terminal. The relationship between the Call and
 * the Address may be viewed as a logical view of the Call. The relationship
 * between a Connection and a Terminal represents the physical view of the
 * Call, i.e. at which Terminal the telephone calls terminates. The
 * TerminalConnection object specification provides further information.
 *
 * <H4>Connection States</H4>
 *
 * Below is a description of each Connection state in real-world terms. These
 * real-world descriptions have no bearing on the specifications of methods,
 * they only serve to provide a more intuitive understanding of what is going
 * on. Several methods in this specification state pre-conditions based upon
 * the state of the Connection.
 * <p>
 * <TABLE CELLPADDING=2>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>Connection.IDLE</CODE></TD>
 * <TD WIDTH="85%">
 * This state is the initial state for all new Connections. Connections which
 * are in the <CODE>Connection.IDLE</CODE> state are not actively part of a
 * telephone call, yet their references to the Call and Address objects are
 * valid. Connections typically do not stay in the <CODE>Connection.IDLE</CODE>
 * state for long, quickly transitioning to other states.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>Connection.DISCONNECTED</CODE></TD>
 * <TD WIDTH="85%">
 * This state implies it is no longer part of the telephone call, although its
 * references to Call and Address still remain valid. A Connection in this
 * state is interpreted as once previously belonging to this telephone call.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>Connection.INPROGRESS</CODE></TD>
 * <TD WIDTH="85%">
 * This state implies that the Connection, which represents the destination
 * end of a telephone call, is in the process of contacting the destination
 * side. Under certain circumstances, the Connection may not progress beyond
 * this state. Extension packages elaborate further on this state in various
 * situations.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>Connection.ALERTING</CODE></TD>
 * <TD WIDTH="85%">
 * This state implies that the Address is being notified of an incoming call.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>Connection.CONNECTED</CODE></TD>
 * <TD WIDTH="85%">
 * This state implies that a Connection and its Address is actively part of a
 * telephone call. In common terms, two people talking to one another are
 * represented by two Connections in the <CODE>Connection.CONNECTED</CODE>
 * state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>Connection.UNKNOWN</CODE></TD>
 * <TD WIDTH="85%">
 * This state implies that the implementation is unable to determine the
 * current state of the Connection. Typically, methods are invalid on
 * Connections which are in this state. Connections may move in and out of the
 * <CODE>Connection.UNKNOWN</CODE> state at any time.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="15%"><CODE>Connection.FAILED</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates that a Connection to that end of the call has failed
 * for some reason. One reason why a Connection would be in the
 * <CODE>Connection.FAILED</CODE> state is because the party was busy.
 * </TD>
 * </TR>
 * </TABLE>
 *
 * <H4>Connection State Transitions</H4>
 *
 * With these loose, real-world meanings in the back of one's mind, the
 * Connection class defines a finite-state diagram which describes the
 * allowable Connection state transitions. This finite-state diagram must be
 * guaranteed by the implementation. Each method which causes a change in
 * a Connection state must be consistent with this state diagram. This finite
 * state diagram is below:
 * <P>
 * Note there is a general left-to-right progression of the state transitions.
 * A Connection object may transition into and out of the
 * <CODE>Connection.UNKNOWN</CODE> state at any time (hence, the asterisk
 * qualifier next to its bidirectional transition arrow).
 * <p>
 * <IMG SRC="doc-files/core-connectionstates.gif" ALIGN="center">
 * </P>
 *
 * <H4>The Connection.disconnect() Method</H4>
 *
 * The primary method supported on the core package's Connection interface is
 * the <CODE>Connection.disconnect()</CODE> method. This method drops an entire
 * Connection from a telephone call. The result of this method is to move the
 * Connection object into the <CODE>Connection.DISCONNECTED</CODE> state. See
 * the specification of the <CODE>Connection.disconnect()</CODE> method on
 * this page for more detailed information.
 *
 * <H4>Listeners and Events</H4>
 *
 * All events pertaining to the Connection object are reported via the
 * <CODE>CallListener</CODE> interface on the Call object associated with this
 * Connection. In the core package, events are reported to a CallListener when
 * a new Connection is created and whenever a Connection changes state.
 * Listeners are added to Call objects via the <CODE>Call.addCallListener()</CODE>
 * method and more indirectly via the <CODE>Address.addCallListener()</CODE>
 * and <CODE>Terminal.addCallListener()</CODE> methods. See the specifications
 * for the Call, Address, and Terminal interfaces for more information.
 * <p>
 * The following Connection-related events are defined in the core package.
 * Each of these events extend the <CODE>ConnectionEvent</CODE> interface (which, in
 * turn, extends the <CODE>CallEvent</CODE> interface).
 * <p>
 * <TABLE CELLPADDING=2>
 * <TR>
 * <TD WIDTH="20%"><CODE>ConnectionCreated</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates a new Connection has been created on a Call.
 * </TD>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>ConnectionInProgress</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the Connection has moved into the
 * <CODE>Connection.INPROGRESS</CODE> state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>ConnectionAlerting</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the Connection has moved into the <CODE>Connection.ALERTING</CODE>
 * state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>ConnectionConnected</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the Connection has moved into the
 * <CODE>Connection.CONNECTED</CODE> state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>ConnectionDisconnected</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the Connection has moved into the
 * <CODE>Connection.DISCONNECTED</CODE> state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>ConnectionFailed</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the Connection has moved into the <CODE>Connection.FAILED</CODE>
 * state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>ConnectionUnknown</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the Connection has moved into the <CODE>Connection.UNKNOWN</CODE>
 * state.
 * </TD>
 * </TR>
 * </TABLE>
 * <p>
 * @see javax.telephony.ConnectionEvent
 * @see javax.telephony.ConnectionListener
 * @version 04/05/99 1.48
 */

public interface Connection {

  /**
   * The <CODE>Connection&#46IDLE</CODE> state is the initial state for all new
   * Connections. Connections which are in the Connection.IDLE state are not
   * actively part of a telephone call, yet their references to the Call and
   * Address objects are valid. Connections typically do not stay in the
   * <CODE>Connection.IDLE</CODE> state for long, quickly transitioning to
   * other states.
   */
  public static final int IDLE = 0x30;


  /**
   * The <CODE>Connection&#46INPROGRESS</CODE> state implies that the Connection,
   * which represents the destination end of a telephone call, is in the
   * process of contacting the destination side. Under certain circumstances,
   * the Connection may not progress beyond this state. Extension packages
   * elaborate further on this state in various situations.
   */
  public static final int INPROGRESS = 0x31;


  /**
   * The <CODE>Connection&#46ALERTING</CODE> state implies that the Address is
   * being notified of an incoming call.
   */
  public static final int ALERTING = 0x32;


  /**
   * The <CODE>Connection&#46CONNECTED</CODE> state implies that a Connection and
   * its Address is actively part of a telephone call. In common terms, two
   * people talking to one another are represented by two Connections in the
   * <CODE>Connection.CONNECTED</CODE> state.
   */
  public static final int CONNECTED = 0x33;


  /**
   * The <CODE>Connection&#46DISCONNECTED</CODE> state implies it is no longer
   * part of the telephone call, although its references to Call and Address
   * still remain valid. A Connection in the
   * <CODE>Connection.DISCONNECTED</CODE> state is interpreted as once
   * previously belonging to this telephone call.
   */
  public static final int DISCONNECTED = 0x34;


  /**
   * The <CODE>Connection&#46FAILED</CODE> state indicates that a Connection to
   * that end of the call has failed for some reason. One reason why a
   * Connection would be in the <CODE>Connection.FAILED</CODE> state is
   * because the party was busy.
   */
  public static final int FAILED = 0x35;


  /**
   * The <CODE>Connection&#46UNKNOWN</CODE> state implies that the implementation
   * is unable to determine the current state of the Connection. Typically,
   * method are invalid on Connections which are in the
   * <CODE>Connection.UNKNOWN</CODE> state. Connections may move in and out of
   * this state at any time.
   */
  public static final int UNKNOWN = 0x36;


  /**
   * Returns the current state of the Connection. The return value will
   * be one of states defined above.
   * <p>
   * @return The current state of the Connection.
   */
  public int getState();


  /**
   * Returns the Call object associated with this Connection. This Call
   * reference remains valid throughout the lifetime of the Connection object,
   * despite the state of the Connection object. This Call reference does not
   * change once the Connection object has been created.
   * <p>
   * @return The call object associated with this Connection.
   */
  public Call getCall();


  /**
   * Returns the Address object associated with this Connection. This Address
   * object reference remains valid throughout the lifetime of the Connection
   * object despite the state of the Connection object. This Address reference
   * does not change once the Connection object has been created.
   * <p>
   * @return The Address associated with this Connection.
   */
  public Address getAddress();


  /**
   * Returns an array of TerminalConnection objects associated with this
   * Connection. TerminalConnection objects represent the relationship between
   * a Connection and a specific Terminal endpoint. There may be zero
   * TerminalConnections associated with this Connection. In that case, this
   * method returns null. Connection objects lose their reference to a
   * TerminalConnection once the TerminalConnection moves into the
   * <CODE>TerminalConnection.DROPPED</CODE> state.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let TerminalConnection tc[] = this.getTerminalConnections()
   * <LI>tc == null or tc.length >= 1
   * <LI>For all i, tc[i].getState() != TerminalConnection.DROPPED
   * </OL>
   * @return An array of TerminalConnection objects associated with this
   * Connection, null if there are no TerminalConnections.
   */
  public TerminalConnection[] getTerminalConnections();


  /**
   * Drops a Connection from an active telephone call. The Connection's Address
   * is no longer associated with the telephone call. This method does not
   * necessarily drop the entire telephone call, only the particular
   * Connection on the telephone call. This method provides the ability to
   * disconnect a specific party from a telephone call, which is especially
   * useful in telephone calls consisting of three or more parties. Invoking
   * this method may result in the entire telephone call being dropped, which
   * is a permitted outcome of this method. In that case, the appropriate
   * events are delivered to the application, indicating that more than just
   * a single Connection has been dropped from the telephone call.
   *
   * <H5>Allowable Connection States</H5>
   *
   * The Connection object must be in one of several states in order for
   * this method to be successfully invoked. These allowable states are:
   * <CODE>Connection.CONNECTED</CODE>, <CODE>Connection.ALERTING</CODE>,
   * <CODE>Connection.INPROGRESS</CODE>, or <CODE>Connection.FAILED</CODE>. If
   * the Connection is not in one of these allowable states when this method is
   * invoked, this method throws InvalidStateException. Having the Connection
   * in one of the allowable states does not guarantee a successful invocation
   * of this method.
   *
   * <H5>Method Return Conditions</H5>
   *
   * This method returns successfully only after the Connection has been
   * disconnected from the telephone call and has transitioned into the
   * <CODE>Connection.DISCONNECTED</CODE>. This method may return
   * unsuccessfully by throwing one of the exceptions listed below. Note that
   * this method waits (i.e. the invocating thread blocks) until either the
   * Connection is actually disconnected from the telephone call or an error
   * is detected and an exception thrown. Also, all of the TerminalConnections
   * associated with this Connection are moved into the
   * <CODE>TerminalConnection.DROPPED</CODE> state. As a result, they are no
   * longer reported via the Connection by the
   * <CODE>Connection.getTerminalConnections()</CODE> method.
   * <p>
   * As a result of this method returning successfully, one or more events
   * are delivered to the application. These events are listed below:
   * <p>
   * <OL>
   * <LI>A ConnectionDisconnected event for this Connection.
   * <LI>A TerminalConnectionDropped event for all TerminalConnections associated with
   * this Connection.
   * </OL>
   *
   * <H5>Dropping Additional Connections</H5>
   *
   * Additional Connections may be dropped indirectly as a result of this
   * method. For example, dropping the destination Connection of a two-party
   * Call may result in the entire telephone call being dropped. It is up to
   * the implementation to determine which Connections are dropped as a result
   * of this method. Implementations should not, however, drop additional
   * Connections if it does not reflect the natural response of the underlying
   * telephone hardware.
   * <p>
   * Dropping additional Connections implies that their TerminalConnections are
   * dropped as well. Also, if all of the Connections on a telephone call are
   * dropped as a result of this method, the Call will move into the
   * <CODE>Call.INVALID</CODE> state. The following lists additional events
   * which may be delivered to the application.
   * <p>
   * <OL>
   * <LI>ConnectionDisconnected/TerminalConnectionDropped are delivered for all other
   * Connections and TerminalConnections dropped indirectly as a result of
   * this method.
   * <LI>CallInvalid if all of the Connections are dropped indirectly as a
   * result of this method.
   * </OL>
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Connection.CONNECTED or Connection.ALERTING
   * or Connection.INPROGRESS or Connection.FAILED
   * <LI>Let TerminalConnection tc[] = this.getTerminalConnections (see post-
   * conditions)
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>((this.getCall()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == Connection.DISCONNECTED
   * <LI>For all i, tc[i].getState() == TerminalConnection.DROPPED
   * <LI>this.getTerminalConnections() == null.
   * <LI>this is not an element of (this.getCall()).getConnections()
   * <LI>ConnectionDisconnected is delivered for this Connection.
   * <LI>TerminalConnectionDropped is delivered for all TerminalConnections associated
   * with this Connection.
   * <LI>ConnectionDisconnected/TerminalConnectionDropped are delivered for all other
   * Connections and TerminalConnections dropped indirectly as a result of
   * this method.
   * <LI>CallInvalid if all of the Connections are dropped indirectly as a
   * result of this method.
   * </OL>
   * <p>
   * @see javax.telephony.ConnectionListener
   * @see javax.telephony.ConnectionEvent
   * @exception PrivilegeViolationException The application does not have
   * the authority or permission to disconnected the Connection. For example,
   * the Address associated with this Connection may not be controllable in
   * the Provider's domain.
   * @exception ResourceUnavailableException An internal resource required
   * to drop a connection is not available.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   * @exception InvalidStateException Some object required for the successful
   * invocation of this method is not in the proper state as given by this
   * method's pre-conditions. For example, the Provider may not be in the
   * Provider.IN_SERVICE state or the Connection may not be in one of the
   * allowable states.
   */
  public void disconnect()
    throws PrivilegeViolationException, ResourceUnavailableException,
      MethodNotSupportedException, InvalidStateException;

  /**
   * Returns the dynamic capabilities for the instance of the Connection
   * object. Dynamic capabilities tell the application which actions are
   * possible at the time this method is invoked based upon the implementations
   * knowledge of its ability to successfully perform the action. This
   * determination may be based upon the current state of the call model
   * or some implementation-specific knowledge. These indications do not 
   * guarantee that a particular method can be successfully invoked, however.
   * <p>
   * The dynamic Connection capabilities require no additional arguments.
   * <p>
   * @return The dynamic Connection capabilities.
   */
  public ConnectionCapabilities getCapabilities();


  /**
   * Gets the ConnectionCapabilities object with respect to a Terminal and
   * an Address. If null is passed as a Terminal parameter, the general/
   * provider-wide Connection capabilities are returned.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2. The
   * <CODE>Connection.getCapabilities()</CODE> method returns the dynamic
   * Connection capabilities. This method now should simply invoke the
   * <CODE>Connection.getCapabilities()</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. This method has been replaced by the
   * Connection.getCapabilities() method.
   * @param terminal This argument is ignored in JTAPI v1.2 and later.
   * @param address This argument is ignored in JTAPI v1.2 and later.
   * @exception InvalidArgumentException This exception is never thrown in
   * JTAPI v1.2 and later.
   * @exception PlatformException A platform-specific exception occurred.
   * @return The dynamic ConnectionCapabilities capabilities.
   */
  public ConnectionCapabilities getConnectionCapabilities(Terminal terminal,
							  Address address)
    throws InvalidArgumentException, PlatformException;
}
