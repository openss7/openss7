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
 * This interface is an extension of the <CODE>CallListener</CODE> interface,
 * and reports state changes both of the Call and its Connections.
 *
 * <H4>Introduction</H4>
 * As is true for the <CODE>CallListener</CODE> interface,
 * specific Connection object state changes are reported
 * to specific corresponding <CODE>ConnectionListener</CODE> methods.
 * The eventID carried by the event (a <CODE>ConnectionEvent</CODE>)
 * corresponds to the method called, and indicates which specific state change occurred.
 *
 * <H4>Adding a Listener to a Call</H4>
 *
 * Applications may add a <CODE>ConnectionListener</CODE> to a Call
 * via one of several mechanisms.
 * They may directly add a <CODE>ConnectionListener</CODE> by implementing
 * the ConnectionListener interface, and then invoking
 * the <CODE>Call.addCallListener</CODE> method.
 * Applications may also add <CODE>ConnectionListener</CODE> to Calls indirectly by implementing
 * the ConnectionListener interface, and then invoking the
 * <CODE>Address.addCallListener</CODE> and
 * <CODE>Terminal.addCallListener</CODE> methods. These methods add the given
 * listener to the Call when the Call comes to the Address or Terminal. See
 * the specifications for Call, Address, and Terminal for more information.
 *
 * <H4>Call State Changes and Connection Events</H4>
 *
 * In the core package, an event is delivered whenever the Call changes state.
 * An event is delivered to a method of this interface specifically when a Connection object
 * changes state. The method called, and the event ID reported, indicate
 * the new Call state.
 * The methods in this interface are: 
 * <CODE>connectionAlerting</CODE>,
 * <CODE>connectionConnected</CODE>,
 * <CODE>connectionDisconnected</CODE>,
 * <CODE>connectionFailed</CODE>,
 * <CODE>connectionInProgress</CODE>,
 * and <CODE>connectionUnknown</CODE>.
 *
 * <H4>Connection Events</H4>
 *
 * All events pertaining to the Connection object are reported on this
 * interface. Connection events are instances of the <CODE>ConnectionEvent</CODE> interface,
 * which in turn, extends the <CODE>CallEvent</CODE> event interface. In the core package, an
 * event is delivered to this interface whenever a Connection changes state.
 *
 * @see javax.telephony.CallEvent
 * @see javax.telephony.Connection
 * @see javax.telephony.ConnectionEvent
 * @see javax.telephony.TerminalConnectionEvent
 * @see javax.telephony.MetaEvent
 * @see javax.telephony.CallListener
 * @see javax.telephony.TerminalConnectionListener
 * @version 05/19/99 1.9
 */

public interface ConnectionListener extends CallListener  {

  /**
   * Interface reporting all events associated with the Call and related Connection objects.
   * <p>
   * The methods in this interface indicate either state changes in objects
   * (either a CallEvent, ConnectionEvent or TerminalConnectionEvent indicating a state change in the
   * corresponding object), or, with the meta event methods, indicate that
   * a high-level operation is occurring.
   * <p>
   * Each method is used to convey a particular state change or meta operation;
   * hence an application may implement the methods corresponding to the events
   * they wish to have reported, and may stub out those they wish to ignore.
   * <p>
   */

  /*
   * Connection event methods.
   */

/**
 * The <CODE>connectionAlerting</CODE> method is called to report that the state of the
 * Connection object has changed to <CODE>Connection.ALERTING</CODE>.
 *<p>
 * @param event A ConnectionEvent with eventID <CODE>Connection.ALERTING</CODE>.
 */
public void connectionAlerting(ConnectionEvent event);

/**
 * The <CODE>connectionConnected</CODE> method is called to report that the state of the
 * Connection object has changed to <CODE>Connection.CONNECTED</CODE>.
 *<p>
 * @param event A ConnectionEvent with eventID <CODE>Connection.CONNECTED</CODE>.
 */
public void connectionConnected(ConnectionEvent event);

/**
 * The <CODE>connectionCreated</CODE> method is called to report that a new Connection
 * object has been created.
 *<p>
 * @param event A ConnectionEvent with eventID <CODE>ConnectionEvent.IDLE</CODE>.
 */
public void connectionCreated(ConnectionEvent event);

/**
 * The <CODE>connectionDisconnected</CODE> method is called to report that the state of
 * the Connection object has changed to <CODE>Connection.DISCONNECTED</CODE>.
 *<p>
 * @param event A ConnectionEvent with eventID <CODE>Connection.DISCONNECTED</CODE>.
 *<p>
 * @see javax.telephony.Connection
 * @see javax.telephony.ConnectionEvent
 */
public void connectionDisconnected(ConnectionEvent event);

/**
 * The <CODE>connectionFailed</CODE> method is called to report that the state of
 * the Connection object has changed to <CODE>Connection.FAILED</CODE>.
 *<p>
 * @param event A ConnectionEvent with eventID <CODE>Connection.FAILED</CODE>.
 *<p>
 * @see javax.telephony.Connection
 * @see javax.telephony.ConnectionEvent
 */
public void connectionFailed(ConnectionEvent event);

/**
 * The <CODE>connectionInProgress</CODE> method is called to report that the state of
 * the Connection object has changed to <CODE>Connection.IN_PROGRESS</CODE>.
 *<p>
 * @param event A ConnectionEvent with eventID <CODE>Connection.IN_PROGRESS</CODE>.
 *<p>
 * @see javax.telephony.Connection
 * @see javax.telephony.Connection
 */
public void connectionInProgress(ConnectionEvent event);

/**
 * The <CODE>connectionUnknown</CODE> method is called to report that the state of
 * the Connection object has changed to <CODE>Connection.UNKNOWN</CODE>.
 *<p>
 * @param event A ConnectionEvent with eventID <CODE>Connection.UNKNOWN</CODE>.
 *<p>
 * @see javax.telephony.Connection
 * @see javax.telephony.ConnectionEvent
 */
public void connectionUnknown(ConnectionEvent event);

}
