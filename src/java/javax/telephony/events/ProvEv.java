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
import  javax.telephony.Provider;

/**
 * The <CODE>ProvEv</CODE> interface is the base interface for all Provider-
 * related events. All events which pertain to the Provider object must
 * extend this interface. Events which extend this interface are reported via
 * the <CODE>ProviderObserver</CODE> interface.
 * <p>
 * The core package defines events which are reported when the Provider changes
 * state. These events are: <CODE>ProvInServiceEv</CODE>,
 * <CODE>ProvOutOfServiceEv</CODE>, and <CODE>ProvShutdownEv</CODE>. Also, the
 * core package defines the <CODE>ProvObservationEndedEv</CODE> event which is
 * sent when the Provider becomes unobservable.
 * <p>
 * The <CODE>ProvEv.getProvider()</CODE> method on this interface returns the
 * Provider associated with the Provider event.
 * <p>
 * @see javax.telephony.events.ProvInServiceEv
 * @see javax.telephony.events.ProvOutOfServiceEv
 * @see javax.telephony.events.ProvShutdownEv
 * @see javax.telephony.events.ProvObservationEndedEv
 * @see javax.telephony.events.Ev
 * @see javax.telephony.ProviderObserver
 * @see javax.telephony.Provider
 */

public interface ProvEv extends Ev {

  /**
   * Returns the Provider associated with this Provider event.
   * <p>
   * @return The Provider associated with this event.
   */
  public Provider getProvider();
}
