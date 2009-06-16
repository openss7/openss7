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

package javax.telephony.events;
import  javax.telephony.TerminalConnection;

/**
 * The <CODE>TermConnEv</CODE> interface is the base event interface for all
 * TerminalConnection-related events. All events which pertain to the
 * TerminalConnection object must extend this interface. This interface extends
 * the <CODE>CallEv</CODE> interface and therefore is reported via the
 * <CODE>CallObserver</CODE> interface.
 * <p>
 * The core package defines events which are reported when the
 * TerminalConnection changes state. These events are:
 * <CODE>TermConnRingingEv</CODE>, <CODE>TermConnActiveEv</CODE>,
 * <CODE>TermConnPassiveEv</CODE>, <CODE>TermConnDroppedEv</CODE>, and
 * <CODE>TermConnUnknownEv</CODE>. Also, a <CODE>TermConnCreatedEv</CODE> is
 * sent when a new TerminalConnection is created.
 * <p>
 * The <CODE>TermConnEv.getTerminalConnection()</CODE> method on this
 * interface returns the TerminalConnection associated with this
 * TerminalConnection event.
 * <p>
 * @see javax.telephony.TerminalConnection
 * @see javax.telephony.CallObserver
 * @see javax.telephony.events.CallEv
 * @see javax.telephony.events.TermConnEv
 * @see javax.telephony.events.TermConnRingingEv
 * @see javax.telephony.events.TermConnActiveEv
 * @see javax.telephony.events.TermConnPassiveEv
 * @see javax.telephony.events.TermConnDroppedEv
 * @see javax.telephony.events.TermConnUnknownEv
 */

public interface TermConnEv extends CallEv {

  /**
   * Returns the TerminalConnection associated with this event.
   * <p>
   * @return The TerminalConnection associated with this event.
   */
  public TerminalConnection getTerminalConnection();
}

