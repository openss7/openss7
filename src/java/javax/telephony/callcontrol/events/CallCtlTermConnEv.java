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
import	javax.telephony.events.TermConnEv;

/**
 * The <CODE>CallCtlTermConnEv</CODE> interface is the base interface for all
 * call control package TerminalConnection-related events. All events which
 * pertain to the <CODE>CallControlTerminalConnection</CODE> interface must
 * extend this interface. Events which extend this interface are reported via
 * the <CODE>CallObserver.callChangedEvent()</CODE> method. The observer
 * object must also implement the <CODE>CallControlCallObserver</CODE>
 * interface to signal it is interested in call control package events. This
 * interface extends both the <CODE>CallCtlCallEv</CODE> and core
 * <CODE>TermConnEv</CODE> interfaces.
 * <p>
 * A number of event interfaces defined in this package extend this interface.
 * Each of these events conveys a change in the call control package state of
 * the TerminalConnection. The event interfaces which extend this interface
 * are: <CODE>CallCtlTermConnBridgedEv</CODE>,
 * <CODE>CallCtlTermConnDroppedEv</CODE>, <CODE>CallCtlTermConnHeldEv</CODE>,
 * <CODE>CallCtlTermConnInUseEv</CODE>, <CODE>CallCtlTermConnRingingEv</CODE>,
 * <CODE>CallCtlTermConnTalkingEv</CODE>, and
 * <CODE>CallCtlTermConnUnknownEv</CODE>
 * <p>
 * This interface supports no additional methods.
 * <p>
 * @see javax.telephony.TerminalConnection
 * @see javax.telephony.CallObserver
 * @see javax.telephony.events.TermConnEv
 * @see javax.telephony.callcontrol.CallControlTerminalConnection
 * @see javax.telephony.callcontrol.CallControlCallObserver
 * @see javax.telephony.callcontrol.events.CallCtlCallEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnBridgedEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnDroppedEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnHeldEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnInUseEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnRingingEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnTalkingEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnUnknownEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcontrol.CallControlTerminalConnectionEvent}
 */

public interface CallCtlTermConnEv extends CallCtlCallEv, TermConnEv {
}
//  LocalWords:  TermConnEv TerminalConnection CallCtlTermConnBridgedEv
//  LocalWords:  CallCtlTermConnDroppedEv CallCtlTermConnHeldEv
//  LocalWords:  CallCtlTermConnInUseEv CallCtlTermConnRingingEv
//  LocalWords:  CallCtlTermConnTalkingEv CallCtlTermConnUnknownEv
