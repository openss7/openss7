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

package javax.telephony.callcontrol.events;
import	javax.telephony.events.TermEv;

/**
 * The <CODE>CallCtlTermEv</CODE> interface is the base interface for all
 * call control package Terminal-related events. All events which pertain to
 * the <CODE>CallControlTerminal</CODE> interface must extend this interface.
 * Events which extend this interface are reported via the
 * <CODE>TerminalObserver.terminalChangedEvent()</CODE> method. The observer
 * object must also implement the <CODE>CallControlTerminalObserver</CODE>
 * interface to signal it is interested in call control package events. This
 * interface extends both the <CODE>CallCtlEv</CODE> and <CODE>TermEv</CODE>
 * interfaces.
 * <p>
 * The only event defined in the call control package for the Terminal is the
 * <CODE>CallCtlTermDoNotDisturbEv</CODE>.
 * <p>
 * This interface supports no additional methods.
 * <p>
 * @see javax.telephony.Terminal
 * @see javax.telephony.TerminalObserver
 * @see javax.telephony.events.TermEv
 * @see javax.telephony.callcontrol.CallControlTerminal
 * @see javax.telephony.callcontrol.CallControlTerminalObserver
 * @see javax.telephony.callcontrol.events.CallCtlTermDoNotDisturbEv
 * @see javax.telephony.callcontrol.events.CallCtlEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcontrol.CallControlTerminalEvent}
 */

public interface CallCtlTermEv extends CallCtlEv, TermEv {
}

