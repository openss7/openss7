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
import  javax.telephony.capabilities.TerminalConnectionCapabilities;

/**
 * The <CODE>TerminalConnection</CODE> represents the relationship between a Connection and
 * a Terminal. 
 *
 * <H4>Introduction</H4>
 *
 * A TerminalConnection object must always be associated with a
 * Connection object and a Terminal object. The Connection and the Terminal
 * objects associated with the TerminalConnection do not change throughout the
 * lifetime of the TerminalConnection. Applications obtain the Connection and
 * Terminal associated with the TerminalConnection via the
 * <CODE>TerminalConnection.getConnection()</CODE> and
 * <CODE>TerminalConnection.getTerminal()</CODE> methods, respectively.
 * <p>
 * Because a TerminalConnection is associated with a Connection, it there is
 * also associated with some Call. The TerminalConnection describes the
 * specific relationship between a physical Terminal endpoint with respect to
 * an Address on a Call. TerminalConnections provide a <EM>physical</EM> view
 * of a Call. For a particular Address endpoint on a Call, there may be zero
 * or more Terminals at which the Call terminates. The TerminalConnection
 * describes each specific Terminal on the Call associated with a particular
 * Address endpoint on the Call. Many simple applications may not care about
 * which specific Terminals are on the Call at a particular Address endpoint.
 * In these cases, the logical view provided by Connections are sufficient.
 *
 * <H4>Requirements for TerminalConnections</H4>
 *
 * In order for a Terminal to be on a Call and associated with a Connection,
 * the Terminal must be associated with the Address object endpoint of the
 * Connection. That is, for each TerminalConnection on a Connection, the
 * Connection's Address must be associated with the TerminalConnection's
 * Terminal. The following predicates illustrates this necessary relationship:
 * <p>
 * <OL>
 * <LI>Let address = connection.getAddress();
 * <LI>Let tc[] = connection.getTerminalConnections();
 * <LI>For all i in tc[], let terminal[i] = tc[i].getTerminal();
 * <LI>Assert for all i: address is an element of terminal[i].getAddresses();
 * <LI>Assert for all i: terminal[i] is an element of address.getTerminals();
 * </OL>
 *
 * <H4>TerminalConnection States</H4>
 *
 * The TerminalConnection has a <EM>state</EM> which describes the current
 * relationship between a Terminal and a Connection. TerminalConnection states
 * are distinct from Connection states. Connection states describe the
 * relationship between an entire Address endpoint and a Call, whereas the
 * TerminalConnection state describes the relationship between one of the
 * Terminals at the endpoint Address on the Call with respect to its
 * Connection. Different Terminals on a Call which are associated with the
 * same Connection may be in different states. Furthermore, the state of the
 * TerminalConnection has a dependency and specific relationship to the state
 * of its Connection, as described later on.
 * <p>
 * The TerminalConnection interface in the core package has six states defined
 * in real-world terms below:
 *
 * <TABLE CELLPADING=2>
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnection.IDLE</CODE></TD>
 * <TD WIDTH="80%">
 * This state is the initial state for all TerminalConnections.
 * TerminalConnection objects do not stay in this state for long. They
 * typically transition into another state quickly.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnection.RINGING</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates the a Terminal is ringing, indicating that the
 * Terminal has an incoming Call.
 * </TD>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnection.PASSIVE</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that a Terminal is part of a telephone call but not in
 * an active fashion. This may imply that a resource of the Terminal is being
 * used and may limit actions on the Terminal.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnection.ACTIVE</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that a Terminal is actively part of a telephone call.
 * This usually implies that the party speaking on that Terminal is part of
 * the telephone call.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnection.DROPPED</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that a particular Terminal has permanently left the
 * telephone call.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnection.UNKNOWN</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that the implementation is unable to determine the
 * state of the TerminalConnection. TerminalConnections may transition into
 * and out of this state at any time.
 * </TD>
 * </TR>
 * </TABLE>
 * <p>
 * When a TerminalConnection moves into the
 * <CODE>TerminalConnection.DROPPED</CODE> state, it is no longer associated
 * with its Connection and Terminal. That is, both of these objects lose their
 * references to the TerminalConnection. However, the TerminalConnection still
 * maintains its references to the Connection and Terminal object for
 * application reference. That is, when a TerminalConnection moves into the
 * <CODE>TerminalConnection.DROPPED</CODE> state, the methods
 * <CODE>TerminalConnection.getConnection()</CODE> and
 * <CODE>TerminalConnection.getTerminal()</CODE> still return valid objects.
 *
 * <H4>TerminalConnection state transitions</H4>
 *
 * Similar to the Connection, there is a finite-state diagram which describes
 * the allowable state transitions of a TerminalConnection. The implementation
 * must guarantee these state transitions. The specifications of methods which
 * affect the state of the TerminalConnections also obey these state
 * transitions. This state diagram is below:
 * <p>
 * <IMG SRC="doc-files/core-terminalconnectionstates.gif" ALIGN="center">
 * <p>
 * Note the TerminalConnection may transition into the
 * <CODE>TerminalConnection.DROPPED</CODE> state from any state, and into and
 * out of the <CODE>TerminalConnection.UNKNOWN</CODE> state from any state.
 * 
 * <H4>Relationship between Connections and TerminalConnections</H4>
 *
 * As mentioned previously, the state of the Connection determines the
 * following about TerminalConnections:
 * <p>
 * <UL>
 * <LI>Whether TerminalConnections may exist on a Connection.
 * <LI>The allowable states of the TerminalConnections if they exist.
 * </UL>
 * <p>
 * These properties about Connections and TerminalConnections are guaranteed
 * by the implementation. This relationship is further illustrated in the
 * description of such methods as <CODE>Call.connect()</CODE>,
 * <CODE>Connection.disconnected()</CODE>, and
 * <CODE>TerminalConnection.answer()</CODE>. The following chart defines the
 * specific relationship between Connection states and TerminalConnections.
 * <p>
 * <TABLE CELLPADDING=2>
 * <TH WIDTH="20%"><EM>If the Connection is in state...</EM></TH>
 * <TH WIDTH="80%"><EM>... then the TerminalConnection is</EM></TD>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>Connection.IDLE</CODE></TD>
 * <TD WIDTH="80%">
 * No TerminalConnections may exist on this Connection, that is, the
 * <CODE>Connection.getTerminalConnections()</CODE> method returns null.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>Connection.INPROGRESS</CODE></TD>
 * <TD WIDTH="80%">
 * No TerminalConnections may exist on this Connection, that is, the
 * <CODE>Connection.getTerminalConnections()</CODE> method returns null.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>Connection.ALERTING</CODE></TD>
 * <TD WIDTH="80%">
 * Zero or more TerminalConnections may exist on this Connection, and each
 * must be in the <CODE>TerminalConnection.RINGING</CODE> state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>Connection.CONNECTED</CODE></TD>
 * <TD WIDTH="80%">
 * Zero or more TerminalConnections may exist on this Connection, and each
 * must be in the <CODE>TerminalConnection.PASSIVE</CODE> or the
 * <CODE>TerminalConnection.ACTIVE</CODE> state. Note that when
 * TerminalConnections must into the <CODE>TerminalConnection.DROPPED</CODE>
 * state they are no longer associated with the Connection.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>Connection.DISCONNECTED</CODE></TD>
 * <TD WIDTH="80%">
 * No TerminalConnections may exist on this Connection, that is, the
 * <CODE>Connection.getTerminalConnections()</CODE> method returns null. Note
 * that all TerminalConnections previously associated with this Connection will
 * move into the <CODE>TerminalConnection.DROPPED</CODE> state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>Connection.FAILED</CODE></TD>
 * <TD WIDTH="80%">
 * No TerminalConnections may exist on this Connection, that is, the
 * <CODE>Connection.getTerminalConnections()</CODE> method returns null. Note
 * that all TerminalConnections previously associated with this Connection will
 * move into the <CODE>TerminalConnection.DROPPED</CODE> state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>Connection.UNKNOWN</CODE></TD>
 * <TD WIDTH="80%">
 * Zero or more TerminalConnections may exist on this Connection, and each
 * must be in the <CODE>TerminalConnection.UNKNOWN</CODE> state.
 * </TD>
 * </TR>
 * </TABLE>
 *
 * <H4>The TerminalConnection.answer() Method</H4>
 *
 * The primary method supported on the core package's TerminalConnection
 * interface is the <CODE>TerminalConnection.answer()</CODE> method. This
 * method answers a telephone call at a Terminal. This method moves the
 * TerminalConnection into the <CODE>TerminalConnection.ACTIVE</CODE> state
 * upon success. The TerminalConnection must be in the
 * <CODE>TerminalConnection.RINGING</CODE> state when this method is invoked.
 *
 * <H4>Listeners and Events</H4>
 *
 * All events pertaining to the TerminalConnection object are reported via the
 * <CODE>CallListener</CODE> interface on the Call object associated with this
 * TerminalConnection. In the core package, events are reported to a
 * CallListener when a new TerminalConnection is created and whenever a
 * TerminalConnection changes state. Listeners are added to Call objects via
 * the <CODE>Call.addCallListener()</CODE> method and more indirectly via the
 * <CODE>Address.addCallListener()</CODE> and
 * <CODE>Terminal.addCallListener()</CODE> methods. See the specifications for
 * the Call, Address, and Terminal interfaces for more information.
 * <p>
 * The following TerminalConnection-related events are defined in the core
 * package. Each of these events extend the <CODE>TerminalConnectionEvent</CODE> interface
 * (which, in turn, extends the <CODE>CallEvent</CODE> interface).
 * <p>
 * <TABLE CELLPADDING=2>
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnectionCreated</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates a new TerminalConnection has been created on a Connection.
 * </TD>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnectionRinging</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the TerminalConnection has moved into the
 * <CODE>TerminalConnection.RINGING</CODE> state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnectionActive</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the TerminalConnection has moved into the
 * <CODE>TerminalConnection.ACTIVE</CODE> state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnectionPassive</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the TerminalConnection has moved into the
 * <CODE>TerminalConnection.PASSIVE</CODE> state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnectionDropped</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the TerminalConnection has moved into the
 * <CODE>TerminalConnection.DROPPED</CODE> state.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>TerminalConnectionUnknown</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the TerminalConnection has moved into the
 * <CODE>TerminalConnection.UNKNOWN</CODE> state.
 * </TD>
 * </TR>
 * </TABLE>
 * <p>
 * @see javax.telephony.CallListener
 * @see javax.telephony.TerminalConnectionListener
 * @see javax.telephony.TerminalListener
 * @see javax.telephony.TerminalConnectionEvent
 * @see javax.telephony.CallEvent
 * @version 04/05/99 1.40
 */

public interface TerminalConnection {

  /**
  * This state is the initial state for
   * all TerminalConnection objects.
   */
  public static final int IDLE = 0x40;


  /**
   * This state indicates the a Terminal
   * is ringing, indicating that the Terminal has an incoming Call.
   */
  public static final int RINGING = 0x41;


  /**
   * This state indicates that a
   * Terminal is part of a telephone call but not in an active fashion. This
   * may imply that a resource of the Terminal is being used and may limit
   * actions on the Terminal.
   */
  public static final int PASSIVE = 0x42;


  /**
   * This state indicates that a Terminal
   * is actively part of a telephone call. This usually implies that the party
   * speaking on that Terminal is party of the telephone call.
   */
  public static final int ACTIVE = 0x43;


  /**
   * This state indicates that a
   * particular Terminal has permanently left the telephone call.
   */
  public static final int DROPPED = 0x44;


  /**
   * This state indicates that the
   * implementation is unable to determine the state of the TerminalConnection.
   */
  public static final int UNKNOWN = 0x45;


  /**
   * Returns the state of the TerminalConnection object.
   * <p>
   * @return The current state of the TerminalConnection object.
   */
  public int getState();


  /**
   * Returns the Terminal associated with this TerminalConnection object.
   * A TerminalConnection's reference to its Terminal remains valid for
   * the lifetime of the object, even if the Terminal loses its references
   * to this TerminalConnection object. Also, this reference does not
   * change once the TerminalConnection object has been created.
   * <p>
   * @return The Terminal object associated with this TerminalConnection.
   */
  public Terminal getTerminal();


  /**
   * Returns the Connection object associated with this TerminalConnection.
   * A TerminalConnection's reference to the Connection remains valid
   * throughout the lifetime of the TerminalConnection. Also, this reference
   * does not change once the TerminalConnection object has been created.
   * <p>
   * @return The Connections associated with this TerminalConnection.
   */
  public Connection getConnection();


  /**
   * Answers an incoming telephone call on this TerminalConnection. This
   * method waits (i.e. the invoking thread blocks) until the telephone call
   * has been answered at the endpoint before returning. When this method
   * returns successfully, the state of this TerminalConnection object is
   * <CODE>TerminalConnection.ACTIVE</CODE>.
   *
   * <H4>Allowable TerminalConnection States</H4>
   *
   * The TerminalConnection must be in the
   * <CODE>TerminalConnection.RINGING</CODE> state when this method is invoked.
   * According to the specification of the TerminalConnection object, this
   * state implies the associated Connection object is also in the
   * <CODE>Connection.ALERTING</CODE> state. There may be more than one
   * TerminalConnection on the Connection which are in the
   * <CODE>TerminalConnection.RINGING</CODE> state. In fact, if the Connection
   * is in the <CODE>Connection.ALERTING</CODE> state, all of these
   * TerminalConnections must be in the <CODE>TerminalConnection.RINGING</CODE>
   * state. Any of these TerminalConnections may invoke this method to answer
   * the telephone call.
   *
   * <H4>Multiple TerminalConnections</H4>
   *
   * The underlying telephone hardware determines the resulting state of other
   * TerminalConnection objects after the telephone call has been answered by
   * one of the TerminalConnections. The other TerminalConnection object may
   * either move into the <CODE>TerminalConnection.PASSIVE</CODE> state or the
   * <CODE>TerminalConnection.DROPPED</CODE> state. If a TerminalConnection
   * moves into the <CODE>TerminalConnection.PASSIVE</CODE> state, it remains
   * part of the telephone call, but not actively so. It may have the ability
   * to join the call in the future. If a TerminalConnection moves into the
   * <CODE>TerminalConnection.DROPPED</CODE> state, it is removed from the
   * telephone call and will never have the ability to join the call in the
   * future. The appropriate events are delivered to the application indicates
   * into which of these two states the other TerminalConnection objects have
   * moved.
   *
   * <H4>Events</H4>
   *
   * The following events are reported to applications via the CallListener
   * interface as a result of the successful outcome of this method:
   * <p>
   * <OL>
   * <LI>TerminalConnectionActive for the TerminalConnection which invoked this method.
   * <LI>ConnectionConnected for the Connection associated with the
   * TerminalConnection.
   * <LI>TerminalConnectionPassive or TerminalConnectionActive for other TerminalConnections
   * associated with the Connection.
   * </OL>
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>((this.getTerminal()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == TerminalConnection.RINGING
   * <LI>(this.getConnection()).getState() == Connection.ALERTING
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>((this.getTerminal()).getProvider()).getState() == Provider.IN_SERVICE
   * <LI>this.getState() == TerminalConnection.ACTIVE
   * <LI>(this.getConnection()).getState() == Connection.CONNECTED
   * <LI>TerminalConnectionActive for the TerminalConnection which invoked this method.
   * <LI>ConnectionConnected for the Connection associated with the
   * TerminalConnection.
   * <LI>TerminalConnectionPassive or TerminalConnectionActive for other TerminalConnections
   * associated with the Connection.
   * </OL>
   * @see javax.telephony.TerminalConnectionListener
   * @see javax.telephony.ConnectionListener
   * @see javax.telephony.TerminalConnectionEvent
   * @see javax.telephony.ConnectionEvent
   * @exception PrivilegeViolationException The application did not have
   * proper authority to answer the telephone call. For example, the
   * Terminal associated with the TerminalConnection may not be in the
   * Provider's local domain.
   * @exception ResourceUnavailableException The necessary resources to
   * answer the telephone call were not available when the method was invoked.
   * @exception MethodNotSupportedException This method is currently not
   * supported by this implementation.
   * @exception InvalidStateException An object was not in the proper state,
   * violating the pre-conditions of this method. For example, the Provider
   * was not in the Provider.IN_SERVICE state or the TerminalConnection was
   * not in the TerminalConnection.RINGING state.
   */
  public void answer()
    throws PrivilegeViolationException, ResourceUnavailableException,
      MethodNotSupportedException, InvalidStateException;


  /**
   * Returns the dynamic capabilities for the instance of the
   * TerminalConnection object. Dynamic capabilities tell the application which
   * actions are possible at the time this method is invoked based upon the
   * implementations knowledge of its ability to successfully perform the
   * action. This determination may be based upon argument passed to this
   * method, the current state of the call model, or some implementation-
   * specific knowledge. These indications do not guarantee that a particular
   * method will be successful when invoked, however.
   * <p>
   * The dynamic TerminalConnection capabilities require no additional
   * arguments.
   * <p>
   * @return The dynamic TerminalConnection capabilities.
   */
  public TerminalConnectionCapabilities getCapabilities();


  /**
   * Gets the TerminalConnectionCapabilities object with respect to a Terminal
   * and an Address. If null is passed as a Terminal parameter, the general/
   * provider-wide Terminal Connection capabilities are returned.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2. The
   * <CODE>TerminalConnection.getCapabilities()</CODE> method returns the
   * dynamic TerminalConnection capabilities. This method now should simply
   * invoke the <CODE>TerminalConnection.getCapabilities()</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. This method has been replaced by the
   * TerminalConnection.getCapabilities() method.
   * @param address This argument is ignored in JTAPI v1.2 and later.
   * @param terminal This argument is ignored in JTAPI v1.2 and later.
   * @exception InvalidArgumentException This exception is never thrown in
   * JTAPI v1.2 and later.
   * @exception PlatformException A platform-specific exception occurred.
   * @return The static TerminalConnectionCapabilities capabilities.
   */
  public TerminalConnectionCapabilities
    getTerminalConnectionCapabilities(Terminal terminal, Address address)
      throws InvalidArgumentException, PlatformException;
}
