/*
 * Copyright (c) 1999 Sun Microsystems, Inc. All Rights Reserved.
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
import  javax.telephony.CallEvent;

/**
 * This interface is an extension of the <CODE>ConnectionListener</CODE> interface,
 * and reports state changes of the Call, its Connections and its TerminalConnections.
 *
 * <H4>Introduction</H4>
 *
 *<p>
 * As is true for the <CODE>CallListener</CODE> interface,
 * specific TerminalConnection object state changes are reported
 * to specific corresponding <CODE>TerminalConnectionListener</CODE> methods.
 * The eventID carried by the event (a <CODE>TerminalConnectionEvent</CODE>)
 * corresponds to the method called, and indicates which specific state change occurred.
 *
 * <H4>Adding a Listener to a Call</H4>
 *
 * Applications may add a <CODE>TerminalConnectionListener</CODE> to a Call
 * via one of several mechanisms.
 * They may directly add a <CODE>TerminalConnectionListener</CODE> by
 * specifying an object which implements that interface, as an argument to
 * the <CODE>Call.addCallListener()</CODE> method.
 * Applications may also add a <CODE>TerminalConnectionListener</CODE> to 
 * Calls indirectly by passing this listener object to either the
 * <CODE>Address.addCallListener()</CODE> or
 * <CODE>Terminal.addCallListener()</CODE> methods. These methods add the given
 * listener to the Call when the Call comes to the Address or Terminal. See
 * the specifications for Call, Address, and Terminal for more information.
 *
 *
 * <H4>Call State Changes and TerminalConnection Events</H4>
 * <p>
 * In the core package, an event is delivered whenever the Call changes state.
 * An event is delivered to a method of this interface specifically when a TerminalConnection object
 * changes state. The method called, and the event ID reported, indicate
 * the new TerminalConnection state.
 * The methods in this interface are: 
 * <CODE>terminalConnectionActive</CODE>,
 * <CODE>terminalConnectionCreated</CODE>,
 * <CODE>terminalConnectionDropped</CODE>,
 * <CODE>terminalConnectionPassive</CODE>,
 * <CODE>terminalConnectionRinging</CODE>,
 * and <CODE>terminalConnectionUnknown</CODE>.
 *
 * <H4>TerminalConnection Events</H4>
 *
 * All events pertaining to the TerminalConnection object are reported on this
 * interface. TerminalConnection events implement the <CODE>TerminalConnectionEvent</CODE>
 * interface, which in turn, extends the <CODE>CallEvent</CODE> interface.
 * TerminalConnectionEvents have an event ID which identifies the new state
 * of the affected TerminalConnection object. In the
 * core package, an event is delivered to this interface whenever the
 * TerminalConnection changes state.
 *
 * @see javax.telephony.TerminalConnection
 * @see javax.telephony.CallEvent
 * @see javax.telephony.ConnectionEvent
 * @see javax.telephony.TerminalConnectionEvent
 * @see javax.telephony.MetaEvent
 * @see javax.telephony.CallListener
 * @see javax.telephony.ConnectionListener
 * @version 05/19/99 1.9
 */

public interface TerminalConnectionListener extends ConnectionListener  {

  /**
   * Interface reporting all events associated with the Call and related Connection and TerminalConnection objects.
   *
   * The methods in this interface indicate either state changes in objects
   * (either a CallEvent, ConnectionEvent or TerminalConnectionEvent indicating a state change in the
   * corresponding object), or, with the meta event methods, indicate that
   * a high-level operation is occurring.
   * <p>
   * Each method is used to convey a particular state change or meta operation;
   * hence an application may implement the methods corresponding to the events
   * they wish to have reported, and may stub out those they wish to ignore.
   */






  /*
   * TerminalConnection event methods.
   */




/**
 * The <CODE>terminalConnectionActive</CODE> method is called to report that the state of the
 * TerminalConnection object has changed to <CODE>TerminalConnection.ACTIVE</CODE>.
 *<p>
 * @param event A TerminalConnectionEvent with eventID <CODE>TerminalConnection.ACTIVE</CODE>.
 */
public void terminalConnectionActive(TerminalConnectionEvent event);

/**
 * The <CODE>terminalConnectionCreated</CODE> method is called to report that a new
 * TerminalConnection object has been created.
 *<p>
 * @param event A TerminalConnectionEvent with eventID <CODE>TerminalConnection.IDLE</CODE>.
 */
public void terminalConnectionCreated(TerminalConnectionEvent event);

/**
 * The <CODE>terminalConnectionDropped</CODE> method is called to report that the state of the
 * TerminalConnection object has changed to <CODE>TerminalConnection.DROPPED</CODE>.
 *<p>
 * @param event A TerminalConnectionEvent with eventID <CODE>TerminalConnection.DROPPED</CODE>.
 */
public void terminalConnectionDropped(TerminalConnectionEvent event);


/**
 * The <CODE>terminalConnectionPassive</CODE> method is called to report that the state of the
 * TerminalConnection object has changed to <CODE>TerminalConnection.PASSIVE</CODE>.
 *<p>
 * @param event A TerminalConnectionEvent with eventID <CODE>TerminalConnection.PASSIVE</CODE>.
 */
public void terminalConnectionPassive(TerminalConnectionEvent event);

/**
 * The <CODE>terminalConnectionRinging</CODE> method is called to report that the state of the
 * TerminalConnection object has changed to <CODE>TerminalConnection.RINGING</CODE>.
 *<p>
 * @param event A TerminalConnectionEvent with eventID <CODE>TerminalConnection.RINGING</CODE>.
 */
public void terminalConnectionRinging(TerminalConnectionEvent event);

/**
 * The <CODE>terminalConnectionUnknown</CODE> method is called to report that the state of the
 * TerminalConnection object has changed to <CODE>TerminalConnection.UNKNOWN</CODE>.
 *<p>
 * @param event A TerminalConnectionEvent with eventID <CODE>terminalConnection.UNKNOWN</CODE>.
 */
public void terminalConnectionUnknown(TerminalConnectionEvent event);



}
 
