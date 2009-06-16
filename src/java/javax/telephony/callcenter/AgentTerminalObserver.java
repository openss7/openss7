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

package javax.telephony.callcenter;
import  javax.telephony.TerminalObserver;

/**
 * The <CODE>AgentTerminalObserver</CODE> interface reports all state changes
 * in the <CODE>Agent</CODE> that is associated with the
 * <CODE>AgentTerminal</CODE> as events. Applications instantiate an object
 * which implements this interface and use the
 * <CODE>Terminal.addObserver()</CODE> to request delivery of events to this
 * observer object. Events will be delivered to this interface only if the
 * Provider is in the <CODE>Provider.IN_SERVICE</CODE> state. All events which
 * are reported via this interface must extend the <CODE>AgentTermEv</CODE>
 * interface.
 * <p>
 * Events are reported via the
 * <CODE>TerminalObserver.terminalChangedEvent()</CODE> method. This interface
 * defines no additional methods and therefore serves as a way applications
 * signal to the implementation that is desires call center package events.
 * <p>
 * Note that the state changes in the <CODE>Agent</CODE> are also reported via
 * the <CODE>ACDAddressObserver</CODE> for the <CODE>ACDAddress</CODE> on which
 * the <CODE>Agent</CODE> is logged in to.
 * <p>
 * @see javax.telephony.Terminal
 * @see javax.telephony.TerminalObserver
 * @see javax.telephony.callcenter.AgentTerminal
 * @see javax.telephony.callcenter.ACDAddressObserver
 * @see javax.telephony.callcenter.events.AgentTermEv
 * @see javax.telephony.callcenter.events.AgentTermBusyEv
 * @see javax.telephony.callcenter.events.AgentTermLoggedOffEv
 * @see javax.telephony.callcenter.events.AgentTermLoggedOnEv
 * @see javax.telephony.callcenter.events.AgentTermNotReadyEv
 * @see javax.telephony.callcenter.events.AgentTermReadyEv
 * @see javax.telephony.callcenter.events.AgentTermUnknownEv
 * @see javax.telephony.callcenter.events.AgentTermWorkNotReadyEv
 * @see javax.telephony.callcenter.events.AgentTermWorkReadyEv
 * @deprecated As of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.AgentTerminalListener}
 */

public interface AgentTerminalObserver extends TerminalObserver {
}
