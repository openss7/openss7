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

package javax.telephony.callcenter.events;
import  javax.telephony.events.ConnEv;

/**
 * The <CODE>CallCentConnEv</CODE> is the base event interface for all call
 * center Connection events. Each Connection-related event in this package must
 * extend this interface. This interface extends both the <CODE>ConnEv</CODE>
 * interface and the <CODE>CallCentCallEv</CODE> interface. Events which extend
 * this interface are reported via the <CODE>CallCenterCallObserver</CODE>
 * interface.
 * <p>
 * Currently, this package defines the <CODE>CallCentConnInProgressEv</CODE>
 * event interface which extends the core event to provide additional call
 * center-related information when the Connection moves into this state.
 * <p>
 * @see javax.telephony.callcenter.CallCenterCallObserver
 * @see javax.telephony.callcenter.events.CallCentConnInProgressEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.CallCenterConnectionEvent}
 */
public interface CallCentConnEv extends CallCentCallEv, ConnEv {
}
