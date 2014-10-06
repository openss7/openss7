/*
 * Copyright (c) 1999 Sun Microsystems, Inc. All Rights Reserved.
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
import  javax.telephony.Terminal;

/**
 * The <CODE>TerminalEvent</CODE> interface is the base interface for all Terminal-
 * related events. All events which pertain to the Terminal object must
 * extend this interface. Events which extend this interface are reported via
 * the <CODE>TerminalListener</CODE> interface.
 * <p>
 * The only event defined in the core package for the Terminal is the
 * <CODE>TERMINAL_EVENT_TRANSMISSION_ENDED</CODE>.
 * <p>
 * The <CODE>TerminalEvent.getTerminal()</CODE> method on this interface returns the
 * Terminal associated with the Terminal event.
 * <p>
 * @see javax.telephony.Event
 * @see javax.telephony.TerminalListener
 * @see javax.telephony.Terminal
 * @version 03/23/99 1.3
 */
public interface TerminalEvent extends Event {

  /**
   * Returns the Terminal associated with this Terminal event.
   * <p>
   * @return The Terminal associated with this event.
   */
  public Terminal getTerminal();


/**
 * Event id codes returned for this interface
 */

/**
 * The <CODE>TERMINAL_EVENT_TRANSMISSION_ENDED</CODE> event indicates that the application
 * will no longer receive Terminal events on the instance of the
 * <CODE>TerminalListener</CODE>.
 * <p>
 * This constant indicates a specific event passed via a <CODE>TerminalEvent</CODE>
 * event, and is reported on the <CODE>TerminalListener</CODE> interface.
 */
public static final int TERMINAL_EVENT_TRANSMISSION_ENDED = 121;


}

