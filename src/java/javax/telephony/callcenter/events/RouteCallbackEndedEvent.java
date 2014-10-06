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

package javax.telephony.callcenter.events;
import  javax.telephony.callcenter.RouteAddress;

/**
 * The <CODE>RouteCallbackEndedEvent</CODE> event interface indicates the
 * <CODE>RouteSession</CODE> interface has moved into the
 * <CODE>RouteSession.ROUTE_CALLBACK_ENDED</CODE> state and the registration of
 * a routing callback has ended. This event is reported via the
 * <CODE>RouteCallback</CODE> interface.
 * <p>
 * @see javax.telephony.callcenter.RouteSession
 * @see javax.telephony.callcenter.RouteCallback
 */

public interface RouteCallbackEndedEvent {

  /**
   * Returns the <CODE>RouteAddress</CODE> that is associated with the
   * <CODE>RouteSession</CODE> associated with this event.
   * <p>
   * @return The RouteAddress associated with the RouteSession.
   */
  public RouteAddress getRouteAddress();
}
