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
import  javax.telephony.AddressObserver;

/**
 * The <CODE>ACDAddressObserver</CODE> interface reports all state changes in
 * the <CODE>Agent</CODE> that are associated with the <CODE>ACDAddress</CODE>
 * as events. Applications instantiate an object which implements this
 * interface and use the <CODE>Address.addObserver()</CODE> to request delivery
 * of events to this observer object. Events will be delivered to this
 * interface only if the Provider is in the <CODE>Provider.IN_SERVICE</CODE>
 * state. All events which are reported via this interface must extend the
 * <CODE>ACDAddrEv</CODE> interface.
 * <p>
 * Events are reported via the
 * <CODE>AddressObserver.addressChangedEvent()</CODE> method. This interface
 * defines no additional methods and therefore serves as a way applications
 * signal to the implementation that is desires call center package events.
 * <p>
 * Note that the state changes in the <CODE>Agent</CODE> are also reported via
 * the <CODE>AgentTermianlObserver</CODE> for the <CODE>AgentTerminal</CODE> on
 * which the <CODE>Agent</CODE> is logged in to.
 * <p>
 * @see javax.telephony.Address
 * @see javax.telephony.AddressObserver
 * @see javax.telephony.callcenter.ACDAddress
 * @see javax.telephony.callcenter.AgentTerminalObserver
 * @see javax.telephony.callcenter.events.ACDAddrEv
 * @see javax.telephony.callcenter.events.ACDAddrBusyEv
 * @see javax.telephony.callcenter.events.ACDAddrLoggedOffEv
 * @see javax.telephony.callcenter.events.ACDAddrLoggedOnEv
 * @see javax.telephony.callcenter.events.ACDAddrNotReadyEv
 * @see javax.telephony.callcenter.events.ACDAddrReadyEv
 * @see javax.telephony.callcenter.events.ACDAddrUnknownEv
 * @see javax.telephony.callcenter.events.ACDAddrWorkNotReadyEv
 * @see javax.telephony.callcenter.events.ACDAddrWorkReadyEv
 * @deprecated As of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.ACDAddressListener}
 */

public interface ACDAddressObserver extends AddressObserver {
}
