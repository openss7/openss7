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

package javax.telephony.callcenter;
import  javax.telephony.MethodNotSupportedException;

/**
 * <H4>Introduction</H4>

 * A <CODE>RouteSession</CODE> represents an outstanding route request of a
 * Call. Each session is associated with a particular <CODE>RouteAddress</CODE>
 * which represents the Address to which the Call was originally placed. This
 * <CODE>RouteAddress</CODE> is obtained via the <CODE>getRouteAddress()</CODE>
 * method defined on this interface. Applications must have previously asked to
 * route calls to this Address.
 *
 * <H4>Routing Callbacks</H4>
 *
 * Each <CODE>RouteSession</CODE> may be associated with zero or more routing
 * callbacks, as represented by the <CODE>RouteCallback</CODE> interface. Using
 * the <CODE>RouteCallback</CODE> interface, applications may routing decisions
 * for Calls. Applications register a callback via the 
 * <CODE>RouteAddress.registerRouteCallback()</CODE> method. Current callbacks
 * registered on a <CODE>RouteAddress</CODE> are associated with all new
 * <CODE>RouteSessions</CODE> created at that <CODE>RouteAddress</CODE>. A
 * <CODE>RouteSession</CODE>, therefore, may have more than one callback
 * associated with it. The first <CODE>RouteCallback</CODE> to respond with a
 * routing request for a particular <CODE>RouteSession</CODE> wins, in the case
 * multiple <CODE>RouteCallbacks</CODE> exist.
 *
 * <H4>The Routing State</H4>
 *
 * A <CODE>RouteSession</CODE> has a <EM>state</EM> which represents the
 * current status of the Call with respect to the routing requests submitted
 * by the application. These states are defined as static integer constants on
 * this interface. Applications obtain the current state via the
 * <CODE>RouteSession.getState()</CODE> method. The various states of a route
 * session are summarized below.
 * <p>
 * <TABLE>
 * <TR>
 * <TD WIDTH="20%"><CODE>RouteSession.ROUTE</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that an application has been requested to route a
 * Call.
 * </TD>
 * </TR>
 * <TR>
 * <TD WIDTH="20%"><CODE>RouteSession.ROUTE_USED</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that a destination has been selected for a Call. This
 * destination is one which the application had selected during its routing of
 * the Call.
 * </TD>
 * </TR>
 * <TR>
 * <TD WIDTH="20%"><CODE>RouteSession.ROUTE_END</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that the routing of a Call has terminated.
 * </TD>
 * </TR>
 * <TR>
 * <TR>
 * <TD WIDTH="20%"><CODE>RouteSession.RE_ROUTE</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that an application has been requested to select
 * another destination for the Call.
 * </TD>
 * </TR>
 * <TR>
 * <TD WIDTH="20%"><CODE>RouteSession.ROUTE_CALLBACK_ENDED</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that all <CODE>RouteCallback</CODE> objects have been
 * removed from this routing session. There are no more routing callbacks
 * associated with this session. This is the final state for the
 * <CODE>RouteSession</CODE> interface.
 * </TD>
 * </TR>
 * </TABLE>
 *
 * <H4>RouteSession State Transition Diagram</H4>
 *
 * The states of the <CODE>RouteSession</CODE> must transition according to
 * the finite state diagram below. The implementation must guarantee the state
 * of a <CODE>RouteSession</CODE> adheres to these valid transitions.
 * <p>
 * <IMG SRC="doc-files/callcent-routestate.gif" ALIGN="center">
 * <p>
 *
 * <H4>Selecting the Routing for a Call</H4>
 *
 * Applications use the <CODE>RouteSession.selectRoute()</CODE> method to
 * select possible destinations for the Call associated with this routing
 * session. The state of the <CODE>RouteSession</CODE> must either be
 * <CODE>RouteSession.ROUTE</CODE> or <CODE>RouteSession.RE_ROUTE</CODE> in
 * order for this method to be valid.
 * <p>
 * An array of destination address strings are given to this method. This list
 * represents a priority-order list of possible destinations for the Call. The
 * Call is routed to the first destination given (at index 0). If it fails, the
 * second destination is attempted. This process is repeated until there are no
 * more remaining destinations, or until a successful destination has been
 * chosen. If a successful destination has been chosen, the state of the
 * <CODE>RouteSession</CODE> moves into <CODE>RouteSession.ROUTE_USED</CODE>.
 * <p>
 * @see javax.telephony.callcenter.RouteCallback
 * @see javax.telephony.callcenter.RouteAddress
 * @see javax.telephony.callcenter.events.RouteSessionEvent
 * @see javax.telephony.callcenter.events.RouteEvent
 * @see javax.telephony.callcenter.events.ReRouteEvent
 * @see javax.telephony.callcenter.events.RouteUsedEvent
 * @see javax.telephony.callcenter.events.RouteEndEvent
 * @see javax.telephony.callcenter.events.RouteCallbackEndedEvent
 */

public interface RouteSession {

  /**
   * The <CODE>RouteSession.ROUTE</CODE> state indicates that an application
   * has been requested to route a Call.
   */
  public static final int ROUTE = 1;


  /**
   * The <CODE>RouteSession.ROUTE_USED</CODE> state indicates that a
   * destination has been selected for a Call. This destination is one which
   * the application had selected during its routing of the Call.
   */
  public static final int ROUTE_USED = 2;


  /**
   * The <CODE>RouteSession.ROUTE_END</CODE> state indicates that the routing
   * of a Call has terminated.
   */
  public static final int ROUTE_END = 3;


  /**
   * The <CODE>RouteSession.RE_ROUTE</CODE> state indicates that an application
   * has been requested to select another destination for the Call.
   */
  public static final int RE_ROUTE = 4;


  /**
   * The <CODE>RouteSession.ROUTE_CALLBACK_ENDED</CODE> state indicates that
   * all <CODE>RouteCallback</CODE> objects have been removed from this routing
   * session. There are no more routing callbacks associated with this session.
   * This is the final state for the <CODE>RouteSession</CODE> interface.
   */
  public static final int ROUTE_CALLBACK_ENDED = 5;


  /**
   * Cause code indicating no error.
   */
  public static final int CAUSE_NO_ERROR = 100;

  
  /**
   * Cause code indicating a routing timer has expired.
   */
  public static final int CAUSE_ROUTING_TIMER_EXPIRED = 101;

  
  /**
   * Cause code indicating that the routing session is being terminated because
   * the application supplied an unsupported parameter in the
   * <CODE>RouteSession.routeSelect()</CODE> method.
   */
  public static final int CAUSE_PARAMETER_NOT_SUPPORTED = 102;

  
  /**
   * Cause code indicating that the routing session is being terminated because
   * because the application supplied an invalid destination in the
   * <CODE>RouteSession.routeSelect()</CODE> method.
   */
  public static final int CAUSE_INVALID_DESTINATION = 103;


  /**
   * Cause code indicating that the routing session is being terminated because
   * the <CODE>Connection</CODE> state is incompatable with the
   * <CODE>RouteSession</CODE>.
   */
  public static final int CAUSE_STATE_INCOMPATIBLE = 104;


  /**
   * Cause code indicating that the routing session is being terminated for
   * unspecified reasons.
   */
  public static final int CAUSE_UNSPECIFIED_ERROR = 105;


  /**
   * Error code indicating the application is not giving a reason for ending
   * the routing session. This value may be passed as an argument to the
   * <CODE>RouteSession.endRoute()</CODE> method.
   */
  public static final int ERROR_UNKNOWN = 1;


  /**
   * Error code indicating the application is too busy to handle the routing
   * request. This value may be passed as an argument to the
   * <CODE>RouteSession.endRoute()</CODE> method.
   */
  public static final int ERROR_RESOURCE_BUSY = 2;


  /**
   * Error code indicating the application or a database upon which it relies
   * for routing is temporary out of service and cannot handle the routing
   * request. This value may be passed as an argument to the
   * <CODE>RouteSession.endRoute()</CODE> method.
   */
  public static final int ERROR_RESOURCE_OUT_OF_SERVICE = 3;


  /**
   * Returns the <CODE>RouteAddress</CODE> associated with this routing session
   * and the one for which the application has registered to route Calls for.
   * <p>
   * @return The RouteAddress associated with this session.
   */
  public RouteAddress getRouteAddress();


  /**
   * Selects one or more possible destinations for the routing of the Call.
   * This method takes an array of string destination telephone address names,
   * in priority order. The highest priority destination is the first element
   * in the given array, and routing is attempted with this destination first.
   * Successive given destination addresses are attempted until one is found
   * which does not fail.
   * <p>
   * A <CODE>RouteUsedEvent</CODE> event is delivered to the application when
   * a successful routing destination has been selected and the Call has been
   * routed to that destination.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>this.getRouteAddress().getProvider().getState() == Provider.IN_SERVICE
   * <LI>this.getState() == RouteSession.ROUTE or RouteSession.RE_ROUTE
   * </OL>
   * <B>Post-Conditions</B>
   * <OL>
   * <LI>this.getRouteAddress().getProvider().getState() == Provider.IN_SERVICE
   * <LI>this.getState() == RouteSession.ROUTE_USED if Call was successfully
   * routed.
   * <LI>RouteUsedEvent is delivered for this RouteSession if a successful
   * destination was selected.
   * </OL>
   * @param routeSelected A list of possible destinations for the call.
   * @exception MethodNotSupportedException Routing is not supported by the
   * implementation.
   */
  public void selectRoute(String[] routeSelected)
    throws MethodNotSupportedException;


  /**
   * Ends a routing session. The application provides an integer error value
   * argument giving the reason why it is terminating this routing session.
   * These error values are defined by this interface.
   * <p>
   * If this method is successful, the state of this <CODE>RouteSession</CODE>
   * moves into the <CODE>RouteSession.ROUTE_END</CODE> state and a
   * <CODE>RouteEndEvent</CODE> is delivered.
   * <p>
   * <B>Pre-Conditions</B>
   * <OL>
   * <LI>this.getRouteAddress().getProvider().getState() == Provider.IN_SERVICE
   * </OL>
   * <p>
   * <B>Post-Conditions</B>
   * <OL>
   * <LI>this.getRouteAddress().getProvider().getState() == Provider.IN_SERVICE
   * <LI>this.getState() == RouteSession.ROUTE_END
   * <LI>RouteEndEvent is delivered to the application for this RouteSession
   * </OL>
   * @param errorValue Indicates the reason why the application is terminating
   * this routing session, as defined by the constants in this interface.
   * @exception MethodNotSupportedException The implementation does not
   * support this method.
   */
  public void endRoute(int errorValue)
    throws MethodNotSupportedException;

  
  /**
   * Returns the current state of the route session.
   * <p>
   * @return The current state of the route session.
   */
  public int getState();


  /**
   * Returns the cause indicating why this route session is in its current
   * state. These cause values are constant integer definitions defined by this
   * interface.
   * <p>
   * @return The cause of the current route session state.
   */
  public int getCause();
}
