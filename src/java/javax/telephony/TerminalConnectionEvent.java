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
import  javax.telephony.TerminalConnection;

/**
 * The <CODE>TerminalConnectionEvent</CODE> interface is the base event interface for all
 * TerminalConnection-related events. All events which pertain to the
 * TerminalConnection object must extend this interface. This interface extends
 * the <CODE>CallEvent</CODE> interface and therefore is reported via the
 * <CODE>CallListener</CODE> interface.
 * <p>
 * The core package defines events which are reported when the
 * TerminalConnection changes state. These events are:
 * <CODE>TERMINAL_CONNECTION_RINGING</CODE>, <CODE>TERMINAL_CONNECTION_ACTIVE</CODE>,
 * <CODE>TERMINAL_CONNECTION_PASSIVE</CODE>, <CODE>TERMINAL_CONNECTION_DROPPED</CODE>, and
 * <CODE>TERMINAL_CONNECTION_UNKNOWN</CODE>. Also, a <CODE>TERMINAL_CONNECTION_CREATED</CODE> is
 * sent when a new TerminalConnection is created.
 * <p>
 * The <CODE>TerminalConnectionEvent.getTerminalConnection()</CODE> method on this
 * interface returns the TerminalConnection associated with this
 * TerminalConnection event.
 * <p>
 * @see javax.telephony.TerminalConnection
 * @see javax.telephony.CallListener
 * @see javax.telephony.CallEvent
 * @see javax.telephony.TerminalConnectionListener
 * @version 04/05/99 1.5
 */
public interface TerminalConnectionEvent extends CallEvent {

  /**
   * Returns the TerminalConnection associated with this event.
   * <p>
   * @return The TerminalConnection associated with this event.
   */
  public TerminalConnection getTerminalConnection();

/**
 * Event id codes returned for this interface
 */
  
/**
 * The <CODE>TERMINAL_CONNECTION_ACTIVE</CODE> event indicates that the state of
 * the TerminalConnection object has changed to
 * <CODE>TerminalConnection.ACTIVE</CODE>.
 * <p>
 * This constant indicates a specific event passed via
 * a <CODE>TerminalConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int TERMINAL_CONNECTION_ACTIVE = 115;

/**
 * The <CODE>TERMINAL_CONNECTION_DROPPED</CODE> event indicates that a new
 * TerminalConnection object has been created.
 * <p>
 * This constant indicates a specific event passed via
 * a <CODE>TerminalConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int TERMINAL_CONNECTION_CREATED = 116;

/**
 * The <CODE>TERMINAL_CONNECTION_DROPPED</CODE> event indicates that the state of
 * the TerminalConnection object has changed to
 * <CODE>TerminalConnection.DROPPED</CODE>.
 * <p>
 * This constant indicates a specific event passed via
 * a <CODE>TerminalConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int TERMINAL_CONNECTION_DROPPED = 117;


/**
 * The <CODE>TERMINAL_CONNECTION_PASSIVE</CODE> event indicates that the state of
 * the TerminalConnection object has changed to
 * <CODE>TerminalConnection.PASSIVE</CODE>.
 * <p>
 * This constant indicates a specific event passed via
 * a <CODE>TerminalConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int TERMINAL_CONNECTION_PASSIVE = 118;

/**
 * The <CODE>TERMINAL_CONNECTION_RINGING</CODE> event indicates that the state of
 * the TerminalConnection object has changed to
 * <CODE>TerminalConnection.RINGING</CODE>.
 * This interface extends the
 * <CODE>TerminalConnectionEvent</CODE> interface and is reported via the
 * <CODE>CallListener</CODE> interface.
 */
public static final int TERMINAL_CONNECTION_RINGING = 119;

/**
 * The <CODE>TERMINAL_CONNECTION_UNKNOWN</CODE> event indicates that the state of
 * the TerminalConnection object has changed to
 * <CODE>TerminalConnection.UNKNOWN</CODE>.
 * <p>
 * This constant indicates a specific event passed via
 * a <CODE>TerminalConnectionEvent</CODE>
 * event, and is reported on the <CODE>CallListener</CODE> interface.
 */
public static final int TERMINAL_CONNECTION_UNKNOWN = 120;
}

