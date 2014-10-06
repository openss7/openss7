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
import	javax.telephony.events.ConnEv;

/**
 * The <CODE>CallCtlConnEv</CODE> interface is the base interface for all call
 * control package Connection-related events. All events which pertain to the
 * <CODE>CallControlConnection</CODE> interface must extend this interface.
 * Events which extend this interface are reported via the
 * <CODE>CallObserver.callChangedEvent()</CODE> method. The observer object
 * must also implement the <CODE>CallControlCallObserver</CODE> interface to
 * signal it is interested in call control package events. This interface
 * extends both the <CODE>CallCtlCallEv</CODE> and core <CODE>ConnEv</CODE>
 * interfaces.
 * <p>
 * A number of event interfaces defined in this package extend this interface.
 * Each of these events conveys a change in the call control package state of
 * the Connection. The event interfaces which extend this interface are:
 * <CODE>CallCtlConnAlertingEv</CODE>, <CODE>CallCtlConnDialingEv</CODE>,
 * <CODE>CallCtlConnDisconnectedEv</CODE>,
 * <CODE>CallCtlConnEstablishedEv</CODE>, <CODE>CallCtlConnFailedEv</CODE>,
 * <CODE>CallCtlConnInitiatedEv</CODE>,
 * <CODE>CallCtlConnNetworkAlertingEv</CODE>,
 * <CODE>CallCtlConnNetworkReachedEv</CODE>, CODE>CallCtlConnOfferedEv</CODE>,
 * <CODE>CallCtlConnQueuedEv</CODE>, and <CODE>CallCtlConnUnknownEv</CODE>
 * <p>
 * This interface supports no additional methods.
 * <p>
 * @see javax.telephony.Connection
 * @see javax.telephony.CallObserver
 * @see javax.telephony.events.ConnEv
 * @see javax.telephony.callcontrol.CallControlConnection
 * @see javax.telephony.callcontrol.CallControlCallObserver
 * @see javax.telephony.callcontrol.events.CallCtlCallEv
 * @see javax.telephony.callcontrol.events.CallCtlConnAlertingEv
 * @see javax.telephony.callcontrol.events.CallCtlConnDialingEv
 * @see javax.telephony.callcontrol.events.CallCtlConnDisconnectedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnEstablishedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnFailedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnInitiatedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnNetworkAlertingEv
 * @see javax.telephony.callcontrol.events.CallCtlConnNetworkReachedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnOfferedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnQueuedEv
 * @see javax.telephony.callcontrol.events.CallCtlConnUnknownEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcontrol.CallControlConnectionEvent}
 */

public interface CallCtlConnEv extends CallCtlCallEv, ConnEv {
}
