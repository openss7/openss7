/*
#pragma ident "%W%      %E%     SMI"

 * Copyright (c) 1996,1997 Sun Microsystems, Inc. All Rights Reserved.
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
import  javax.telephony.CallObserver;

/**
 * The <CODE>CallCenterCallObserver</CODE> interface extends the event
 * reporting of of the core <CODE>CallObserver</CODE> to include call center
 * related events. Applications instantiate an object which implements this
 * interface and use the <CODE>Call.addObserver()</CODE> to request delivery
 * of events to this observer object. Events will be delivered to this
 * interface only if the Provider is in the <CODE>Provider.IN_SERVICE</CODE>
 * state. All events which are reported via this interface must extend the
 * <CODE>CallCentCallEv</CODE> interface.
 * <p>
 * Events are reported via the <CODE>CallObserver.callChangedEvent()</CODE>
 * method. This interface defines no additional methods and therefore serves
 * as a way applications signal to the implementation that is desires call
 * center package events.
 * <p>
 * @see javax.telephony.Call
 * @see javax.telephony.CallObserver
 * @see javax.telephony.callcenter.events.CallCentCallEv
 * @see javax.telephony.callcenter.events.CallCentTrunkValidEv
 * @see javax.telephony.callcenter.events.CallCentTrunkInvalidEv
 * @see javax.telephony.callcenter.events.CallCentCallAppDataEv
 * @see javax.telephony.callcenter.events.CallCentConnInProgressEv
 * @deprecated As of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.CallCenterCallListener}
 */
public interface CallCenterCallObserver extends CallObserver {
}
