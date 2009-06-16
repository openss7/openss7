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
import  javax.telephony.*;

/**
 * The <CODE>RouteAddress</CODE> interface extends the core
 * <CODE>Address</CODE> interface and add methods to allow applications the
 * ability to select destinations for calls coming into this Address.
 * <p>
 * Applications may register to route calls for this Address via the
 * <CODE>registerRouteCallback()</CODE> method defined on this interface. This
 * method takes an instance of an object which implements the
 * <CODE>RouteCallback</CODE> interface. For each Call which comes into this
 * Address, a new <CODE>RouteSession</CODE> is created for each callback which
 * is currently registered. The callbacks will receive routing requests via the
 * callbacks.
 * <p>
 * Applications may register to route calls for all RouteAddresses via a special
 * Address created by the Provider. This Address has the name
 * <CODE>RouteAddress.ALL_ROUTE_ADDRESS</CODE> and may be obtained via the
 * <CODE>Provider.getAddresses()</CODE> or <CODE>Provider.getAddress()</CODE>
 * method. When applications invoke the <CODE>registerRouteCallback()</CODE>
 * method on this special address, the callback will receive routing requests
 * for all routeable Addresses in the Provider's domain.
 */

public interface RouteAddress extends Address {

  /**
   * The string name of the special Address created by the Provider used by
   * applications to register a callback for all routeable Addresses in the
   * Provider's domain.
   */
  public static final String ALL_ROUTE_ADDRESS = "AllRouteAddress";


  /**
   * Registers a callback to route calls for this Address. The callback given
   * as an argument will be notified of all routing requests for Calls which
   * come into this Address. Applications may register a callback for all
   * routeable Addresses in the Provider's domain by invoke this method on a
   * special Address with the name <CODE>RouteAddress.ALL_ROUTE_ADDRESS</CODE>.
   * <p>
   * Multiple callbacks may be registered on a single Address. This method
   * throws <CODE>ResourceUnavailableException</CODE> if the maximum number of
   * registered callbacks on the Address has been exceeded.
   * <p>
   * <B>Pre-Conditions</B>
   * <OL>
   * <LI>this.getProvider().getState() == Provider.IN_SERVICE
   * </OL>
   * <B>Post-Conditions</B>
   * <OL>
   * <LI>this.getProvider().getState() == Provider.IN_SERVICE
   * <LI>routeCallback is an element of this.getRouteCallback()
   * </OL>
   * @param routeCallback The callback to be registered.
   * @exception ResourceUnavailableException Indicates the maximum number of
   * registered callbacks for this Address has been exceeded.
   * @exception MethodNotSupportedException The implementation does not support
   * this method.
   */
  public void registerRouteCallback(RouteCallback routeCallback)
    throws ResourceUnavailableException, MethodNotSupportedException;

  
  /**
   * Cancels a previously registered routing callback for this Address. If the
   * given callback is currently no registered on this Address, this method
   * fails silently, i.e. no callback is removed and no exception is thrown.
   * <p>
   * <B>Pre-Conditions</B>
   * <OL>
   * <LI>this.getProvider().getState() == Provider.IN_SERVICE
   * </OL>
   * <B>Post-Conditions</B>
   * <OL>
   * <LI>this.getProvider().getState() == Provider.IN_SERVICE
   * <LI>routeCallback is not an element of this.getRouteCallback()
   * </OL>
   * @param routeCallback The callback to be cancelled.
   * @exception MethodNotSupportedException will be thrown if provider does
   * not support this method.
   */
  public void cancelRouteCallback(RouteCallback routeCallback)
    throws MethodNotSupportedException;


  /**
   * Returns an array of all callbacks which are registered to route Calls for
   * this Address. This method returns <CODE>null</CODE> if there exists no
   * registered callbacks.
   * <p>
   * @return An array of register callbacks.
   * @exception MethodNotSupportedException The implementation does not support
   * this method.
   */
  public RouteCallback[] getRouteCallback()
    throws MethodNotSupportedException;


  /**
   * Returns an array of all active route sessions associated with this
   * Address. This method returns <CODE>null</CODE> if there exists no active
   * route sessions.
   * <p>
   * @return An array of active route sessions associated with this Address.
   * @exception MethodNotSupportedException The implementation does not support
   * this method.
   */
  public RouteSession[] getActiveRouteSessions()
    throws MethodNotSupportedException;
}
