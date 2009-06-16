/*
#pragma ident "@(#)RouteCallback.java	1.12      97/10/28     SMI"

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

package javax.telephony.callcenter;
import  javax.telephony.callcenter.events.*;

/**
 * The <CODE>RouteCallback</CODE> interface is used by applications to receive
 * routing requests for a particular <CODE>RouteSession</CODE>. Applications
 * instantiates an object which implements this interface and registers the
 * callback for a particular routeable address via the
 * <CODE>RouteAddress.registerRouteCallback()</CODE> method.
 * <p>
 * Applications override the individual methods defined by this interface, each
 * of which corresponds to a different kind of routing request.
 * <p>
 * @see javax.telephony.callcenter.RouteAddress
 * @see javax.telephony.callcenter.RouteSession
 */
public interface RouteCallback {

  /**
   * This method indicates the application is being asked to route a Call. The
   * <CODE>RouteSession</CODE> associated with this <CODE>RouteCallback</CODE>
   * has transitioned into the <CODE>RouteSession.ROUTE</CODE> state.
   * <p>
   * @param event The RouteEvent object describing the routing request.
   */
  public void routeEvent(RouteEvent event);


  /**
   * This method indicates the application is being asked to select another
   * route for a Call. The <CODE>RouteSession</CODE> associated with this
   * <CODE>RouteCallback</CODE> has transitioned into the
   * <CODE>RouteSession.RE_ROUTE</CODE> state.
   * <p>
   * @param event The ReRouteEvent object describing the routing request.
   */
  public void reRouteEvent(ReRouteEvent event);

  
  /**
   * This method indicates that a Call has successfully reached a final
   * destination which the application has selected. The
   * <CODE>RouteSession</CODE> associated with this <CODE>RouteCallback</CODE>
   * has transitioned into the <CODE>RouteSession.ROUTE_USED</CODE> state.
   * <p>
   * @param event The RouteUsedEvent object describing the route used.
   */
  public void routeUsedEvent(RouteUsedEvent event);


  /**
   * This method indicates that a routing session has ended. The
   * <CODE>RouteSession</CODE> associated with this <CODE>RouteCallback</CODE>
   * has transitioned into the <CODE>RouteSession.ROUTE_END</CODE> state.
   * <p>
   * @param event The RouteEndEvent object describing the ending of the routing
   * session.
   */
  public void routeEndEvent(RouteEndEvent event);


  /**
   * This method indicates that this callback will no longer receive routing
   * requests or routing information and the callback has been terminated.
   * The <CODE>RouteSession</CODE> associated with this
   * <CODE>RouteCallback</CODE> has transitioned into the
   * <CODE>RouteSession.ROUTE_CALLBACK_ENDED</CODE> state.
   * <p>
   * @param event The RouteCallbackEndedEvent object describing the ending of
   * the routing callback.
   */
  public void routeCallbackEndedEvent(RouteCallbackEndedEvent event);
}
