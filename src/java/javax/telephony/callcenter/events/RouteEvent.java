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
import  javax.telephony.*;
import  javax.telephony.callcenter.RouteAddress;

/**
 * The <CODE>RouteEvent</CODE> event interface indicates the
 * <CODE>RouteSession</CODE> interface has moved into the
 * <CODE>RouteSession.ROUTE</CODE> state and the Provider is requesting the
 * application route a Call. This interface extends the
 * <CODE>RouteSessionEvent</CODE> interface and is reported via the
 * <CODE>RouteCallback</CODE> interface.
 * <p>
 * @see javax.telephony.callcenter.RouteSession
 * @see javax.telephony.callcenter.RouteCallback
 * @see javax.telephony.callcenter.events.RouteSessionEvent
 */

public interface RouteEvent extends RouteSessionEvent {

  /**
   * Route Selection Algorithm: Select a normal route.
   */
  public final static int SELECT_NORMAL = 0;


  /**
   * Route Selection Algorithm: Select a least cost route.
   */
  public final static int SELECT_LEAST_COST = 1;


  /**
   * Route Selection Algorithm: Select an emergency route.
   */
  public final static int SELECT_EMERGENCY = 2;


  /**
   * Route Selection Algorithm: Select a route to an ACDAddress.
   */
  public final static int SELECT_ACD = 3;


  /**
   * Route Selection Algorithm: Select a user defined route.
   */
  public final static int SELECT_USER_DEFINED = 4;


  /**
   * Returns the originally requested destination for the call.
   * <p>
   * @return The originally request destination for the call.
   */
  public RouteAddress getCurrentRouteAddress();


  /**
   * Returns the calling Address.
   * <p>
   * @return The calling Address.
   */
  public Address getCallingAddress();


  /**
   * Returns the calling Terminal.
   * <p>
   * @return The calling Terminal.
   */
  public Terminal getCallingTerminal();


  /**
   * Returns the route select algorithm being used.
   * <p>
   * @return The route selection algorithm being used.
   */
  public int getRouteSelectAlgorithm();


  /**
   * Returns the ISDN call setup message when available.
   * <p>
   * @return The ISDN call setup message.
   */
  public String getSetupInformation();  
}
