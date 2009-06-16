/*
#pragma ident "%W%      %E%     SMI"

 * Copyright (c) 1996 Sun Microsystems, Inc. All Rights Reserved.
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

package javax.telephony.events;
import  javax.telephony.Terminal;

/**
 * The <CODE>TermEv</CODE> interface is the base interface for all 
 * Terminal-related events. All events which pertain to the Terminal object must
 * extend this interface. Events which extend this interface are reported via
 * the <CODE>TerminalObserver</CODE> interface.
 * <p>
 * The only event defined in the core package for the Terminal is the
 * <CODE>TermObservationEndedEv</CODE>.
 * <p>
 * The <CODE>TermEv.getTerminal()</CODE> method on this interface returns the
 * Terminal associated with the Terminal event.
 * <p>
 * @see javax.telephony.events.TermObservationEndedEv
 * @see javax.telephony.events.Ev
 * @see javax.telephony.TerminalObserver
 * @see javax.telephony.Terminal
 */

public interface TermEv extends Ev {

  /**
   * Returns the Terminal associated with this Terminal event.
   * <p>
   * @return The Terminal associated with this event.
   */
  public Terminal getTerminal();
}

