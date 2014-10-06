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
import  javax.telephony.callcenter.RouteSession;

/**
 * The <CODE>RouteSessionEvent</CODE> interface is the base event interface
 * for all events pertaining to the <CODE>RouteSession</CODE> interface. Events
 * which extend this interface are not part of the JTAPI observer mechanism.
 * All events which extend this interface are reported via the
 * <CODE>RouteCallback</CODE> interface.
 * <p>
 * The call center package defines event interface which extend this interface
 * to report changes in the state of the <CODE>RouteSession</CODE> interface.
 * These interfaces are: <CODE>RouteUsedEvent</CODE>, <CODE>RouteEvent</CODE>,
 * <CODE>RouteEndEvent</CODE>, <CODE>RouteCallbackEndedEvent</CODE>, and
 * <CODE>ReRouteEvent</CODE>
 * <p>
 * @see javax.telephony.callcenter.RouteSession
 * @see javax.telephony.callcenter.RouteCallback
 * @see javax.telephony.callcenter.events.RouteUsedEvent
 * @see javax.telephony.callcenter.events.RouteEvent
 * @see javax.telephony.callcenter.events.ReRouteEvent
 * @see javax.telephony.callcenter.events.RouteEndEvent
 * @see javax.telephony.callcenter.events.RouteCallbackEndedEvent
 */

public interface RouteSessionEvent {

  /**
   * Returns the <CODE>RouteSession</CODE> associated with this event.
   * <p>
   * @return The RouteSession associated with this event.
   */
  public RouteSession getRouteSession();
}
