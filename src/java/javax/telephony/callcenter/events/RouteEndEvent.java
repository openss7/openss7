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

/**
 * The <CODE>RouteEndEvent</CODE> event interface indicates the
 * <CODE>RouteSession</CODE> interface has moved into the
 * <CODE>RouteSession.ROUTE_END</CODE> state and the routing of the Call has
 * terminated. This interface extends the <CODE>RouteSessionEvent</CODE>
 * interface and is reported via the <CODE>RouteCallback</CODE> interface.
 * <p>
 * The reason for the termination of routing may be determined via the
 * <CODE>RouteSessionEvent.getRouteSession().getCause()</CODE> method.
 * <p>
 * @see javax.telephony.callcenter.RouteSession
 * @see javax.telephony.callcenter.RouteCallback
 * @see javax.telephony.callcenter.events.RouteSessionEvent
 */

public interface RouteEndEvent extends RouteSessionEvent {
}
