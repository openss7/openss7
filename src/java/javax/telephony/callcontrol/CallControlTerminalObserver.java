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

package javax.telephony.callcontrol;
import  javax.telephony.TerminalObserver;

/**
 * The <CODE>CallControlTerminalObserver</CODE> interface reports all events
 * for the <CODE>CallControlTerminal</CODE> interface. Applications implement
 * this interface to receive <CODE>CallControlTerminal</CODE>-related events.
 * All events are reported via the
 * <CODE>TerminalObserver.terminalChangedEvent()</CODE> method. This interface,
 * therefore, allows applications to signal to the implementation that they
 * are interested in call control package events. This interface defines no
 * additional methods.
 * <p>
 * All events must extend the <CODE>CallCtlTermEv</CODE> event interface,
 * which in turn, extends the core <CODE>TermEv</CODE> interface.
 * <p>
 * The following are those events which are associated with this interface:
 * <p>
 * <TABLE CELLPADDING=2>
 * <TR>
 * <TD WIDTH="20%"><CODE>CallCtlTermDoNotDisturbEv</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the Do Not Disturb characteristics of this Terminal has changed.
 * </TD>
 * </TR>
 * </TABLE>
 * <p>
 * @see javax.telephony.TerminalObserver
 * @see javax.telephony.events.TermEv
 * @see javax.telephony.callcontrol.CallControlTerminal
 * @see javax.telephony.callcontrol.events.CallCtlTermEv
 * @see javax.telephony.callcontrol.events.CallCtlTermDoNotDisturbEv
 * @deprecated As of JTAPI 1.4, replaced by {@link javax.telephony.callcontrol.CallControlTerminalListener}
 */

public interface CallControlTerminalObserver extends TerminalObserver {
}
