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

package javax.telephony.events;
import  javax.telephony.Call;

/**
 * The <CODE>CallEv</CODE> interface is the base interface for all Call-related
 * events. All events which pertain to the Call object must extend this
 * interface. Events which extend this interface are reported via the
 * <CODE>CallObserver</CODE> interface.
 * <p>
 * The core package defines events which are reported when the Call changes
 * state. These events are: <CODE>CallActiveEv</CODE> and
 * <CODE>CallInvalidEv</CODE>. Also, the core package defines the
 * <CODE>CallObservationEndedEv</CODE> event which is sent when the Call
 * becomes unobservable.
 * <p>
 * The <CODE>ConnEv</CODE> and <CODE>TermConnEv</CODE> events extend this
 * interface. This reflects the fact that all Connection and TerminalConnection
 * events are reported via the <CODE>CallObserver</CODE> interface.
 * <p>
 * The <CODE>CallEv.getCall()</CODE> method on this interface returns the Call
 * associated with the Call event.
 * <p>
 * @see javax.telephony.events.CallActiveEv
 * @see javax.telephony.events.CallInvalidEv
 * @see javax.telephony.events.CallObservationEndedEv
 * @see javax.telephony.events.Ev
 * @see javax.telephony.events.ConnEv
 * @see javax.telephony.events.TermConnEv
 * @see javax.telephony.CallObserver
 * @see javax.telephony.Call
 */

public interface CallEv extends Ev {

  /**
   * Returns the Call object associated with this Call event.
   * <p>
   * @return The Call associated with this event.
   */
  public Call getCall();
}
