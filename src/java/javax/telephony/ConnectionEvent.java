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
import  javax.telephony.Connection;

/**
 * The <CODE>ConnectionEvent</CODE> interface is the base event interface for all
 * Connection-related events. All events which pertain to the Connection object
 * must extend this interface. This interface extends the <CODE>CallEvent</CODE>
 * interface and therefore is reported via the <CODE>CallListener</CODE>
 * interface.
 * <p>
 * The core package defines events which are reported when the Connection
 * changes state. These events are: <CODE>CONNECTION_IN_PROGRESS</CODE>,
 * <CODE>CONNECTION_ALERTING</CODE>, <CODE>CONNECTION_CONNECTED</CODE>,
 * <CODE>CONNECTION_DISCONNECTED</CODE>, <CODE>CONNECTION_FAILED</CODE>, and
 * <CODE>CONNECTION_UNKNOWN</CODE>. Also, the <CODE>CONNECTION_CREATED</CODE> is sent
 * when a new Connection is created.
 * <p>
 * Each of these events is reported to the corresponding method for an application
 * registered as a <CODE>ConnectionListener</CODE>
 * or <CODE>TerminalConnectionListener</CODE>.
 * <p>
 * The <CODE>ConnectionEvent.getConnection</CODE> method on this interface returns the
 * Connection associated with this Connection event.
 * <p>
 * @see javax.telephony.Connection
 * @see javax.telephony.CallListener
 * @see javax.telephony.CallEvent
 * @version 04/05/99 1.6
 */

public interface ConnectionEvent extends CallEvent {

  /**
   * Returns the Connection associated with this Connection event.
   * <p>
   * @return The Connection associated with this event.
   */
  public Connection getConnection();

/**
 * Event id codes returned for this interface
 */

/**
 * The <CODE>CONNECTION_ALERTING</CODE> event indicates that the state of the
 * Connection object has changed to <CODE>Connection.ALERTING</CODE>.
 *<p>
 * This constant indicates a specific event passed via a <CODE>ConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int CONNECTION_ALERTING = 104;

/**
 * The <CODE>CONNECTION_CONNECTED</CODE> event indicates that the state of the
 * Connection object has changed to <CODE>Connection.CONNECTED</CODE>.
 *<p>
 * This constant indicates a specific event passed via a <CODE>ConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int CONNECTION_CONNECTED = 105;

/**
 * The <CODE>CONNECTION_CREATED</CODE> event indicates that a new Connection
 * object has been created.
 *<p>
 * This constant indicates a specific event passed via a <CODE>ConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int CONNECTION_CREATED = 106;

/**
 * The <CODE>CONNECTION_DISCONNECTED</CODE> event indicates that the state of
 * the Connection object has changed to <CODE>Connection.DISCONNECTED</CODE>.
 *<p>
 * This constant indicates a specific event passed via a <CODE>ConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int CONNECTION_DISCONNECTED = 107;

/**
 * The <CODE>CONNECTION_FAILED</CODE> interface indicates that the state of the
 * Connection object has changed to <CODE>Connection.FAILED</CODE>.
 *<p>
 * This constant indicates a specific event passed via a <CODE>ConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int CONNECTION_FAILED = 108;

/**
 * The <CODE>CONNECTION_IN_PROGRESS</CODE> event indicates that the state of the
 * Connection object has changed to <CODE>Connection.IN_PROGRESS</CODE>.
 *<p>
 * This constant indicates a specific event passed via a <CODE>ConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int CONNECTION_IN_PROGRESS = 109;

/**
 * The <CODE>CONNECTION_UNKNOWN</CODE> event indicates that the state of the
 * Connection object has changed to <CODE>Connection.UNKNOWN</CODE>.
 *<p>
 * This constant indicates a specific event passed via a <CODE>ConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int CONNECTION_UNKNOWN = 110;

}



