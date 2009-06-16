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
import  javax.telephony.callcenter.CallCenterTrunk;

/**
 * The <CODE>CallCentTrunkEv</CODE> interface is the base event interface for
 * all <CODE>CallCenterTrunk</CODE>-related events in the call center package.
 * Every <CODE>CallCenterTrunk</CODE>-related event must extend this interface.
 * This event extends the <CODE>CallCentCallEv</CODE> interface and all events
 * which extend this interface are reported via the
 * <CODE>CallCenterCallObserver</CODE> interface.
 * <p>
 * This interface contains a single method, <CODE>getTrunk()</CODE>, which
 * returns the <CODE>CallCenterTrunk</CODE> associated with this event.
 * <p>
 * The call center package defines two event interfaces which extends this
 * interface. They are the <CODE>CallCentTrunkValidEv</CODE> interface and
 * the <CODE>CallCentTrunkInvalidEv</CODE> interface. These interfaces report
 * a state change in the <CODE>CallCenterTrunk</CODE>.
 * <p>
 * @see javax.telephony.Call
 * @see javax.telephony.CallObserver
 * @see javax.telephony.callcenter.CallCenterCallObserver
 * @see javax.telephony.callcenter.CallCenterTrunk
 * @see javax.telephony.callcenter.events.CallCentCallEv
 * @see javax.telephony.callcenter.events.CallCentTrunkValidEv
 * @see javax.telephony.callcenter.events.CallCentTrunkInvalidEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.CallCenterTrunkEvent}
 */
public interface CallCentTrunkEv extends CallCentCallEv {

  /**
   * Returns the <CODE>CallCenterTrunk</CODE> associated with this event.
   * <p>
   * @return The associated CallCenterTrunk.
   */
  public CallCenterTrunk getTrunk();
}
