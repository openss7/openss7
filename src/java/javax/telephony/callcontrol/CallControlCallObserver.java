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
import  javax.telephony.CallObserver;

/**
 * The <CODE>CallControlCallObserver</CODE> interface reports all events for
 * the <CODE>CallControlCall</CODE> interface. It also reports events for the
 * <CODE>CallControlConnection</CODE> and the
 * <CODE>CallControlTerminalConnection</CODE> interfaces. Applications
 * implement this interface to receive these events. All events are reported
 * via the <CODE>CallObserver.callChangedEvent()</CODE> method. This interface,
 * therefore, allows applications to signal to the implementation that they
 * are interested in call control package events. This interface defines no
 * additional methods.
 * <p>
 * All events must extend the <CODE>CallCtlCallEv</CODE> event interface, which
 * in turn, extends the core <CODE>CallEv</CODE> interface.
 * <p>
 * The <CODE>CallControlConnection</CODE> state events defined in this package
 * are: <CODE>CallCtlConnOfferedEv</CODE>, <CODE>CallCtlConnQueuedEv</CODE>,
 * <CODE>CallCtlConnAlertingEv</CODE>, <CODE>CallCtlConnInitiatedEv</CODE>,
 * <CODE>CallCtlConnDialingEv</CODE>, <CODE>CallCtlConnNetworkReachedEv</CODE>,
 * <CODE>CallCtlConnNetworkAlertingEv</CODE>, <CODE>CallCtlConnFailedEv</CODE>,
 * <CODE>CallCtlConnEstablishedEv</CODE>, <CODE>CallCtlConnUnknownEv</CODE>, 
 * and <CODE>CallCtlConnDisconnectedEv</CODE>.
 * <p>
 * The <CODE>CallControlTerminalConnection</CODE> state events defined in this
 * package are: <CODE>CallCtlTermConnBridgedEv</CODE>,
 * <CODE>CallCtlTermConnDroppedEv</CODE>, <CODE>CallCtlTermConnHeldEv</CODE>,
 * <CODE>CallCtlTermConnInUseEv</CODE>, <CODE>CallCtlTermConnRingingEv</CODE>,
 * <CODE>CallCtlTermConnTalkingEv</CODE>, and
 * <CODE>CallCtlTermConnUnknownEv</CODE>.
 * <p>
 * @see javax.telephony.CallObserver
 * @see javax.telephony.events.CallEv
 * @see javax.telephony.Connection
 * @see javax.telephony.TerminalConnection
 * @see javax.telephony.callcontrol.events.CallCtlCallEv
 * @see javax.telephony.callcontrol.events.CallCtlConnEv
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
 * @see javax.telephony.callcontrol.events.CallCtlTermConnEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnRingingEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnTalkingEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnHeldEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnBridgedEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnInUseEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnDroppedEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnUnknownEv
 * @deprecated As of JTAPI 1.4, replaced by {@link javax.telephony.callcontrol.CallControlCallListener}
 */

public interface CallControlCallObserver extends CallObserver {
}
