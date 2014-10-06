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
 * The <CODE>CallCenterProvider</CODE> interface extends the core
 * <CODE>Provider</CODE> interface. This interface defines additional methods
 * to query the Provider's local domain. This interface defines method to
 * return the routeable addresses, the ACD addresses, and the ACD manager
 * addresses in the Provider's domain.
 * <p>
 * @see javax.telephony.Provider
 * @see javax.telephony.callcenter.RouteAddress
 * @see javax.telephony.callcenter.ACDAddress
 * @see javax.telephony.callcenter.ACDManagerAddress
 */

public interface CallCenterProvider extends Provider {

  /**
   * Returns an array of routeable Addresses associated with the Provider and
   * within the Provider's domain. This list is static (i.e. is does not
   * change) after the Provider is first created. If no routeable Addresses are
   * associated with this Provider, then this method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let RouteAddress[] addresses = this.getRouteableAddresses()
   * <LI>addresses == null or addresses.length >= 1
   * </OL>
   * <p>
   * @return An array of RouteAddresses in the Provider's domain
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public RouteAddress[] getRouteableAddresses()
    throws MethodNotSupportedException;


  /**
   * Returns an array of ACD Addresses associated with the Provider and
   * within the Provider's domain. This list is static (i.e. is does not
   * change) after the Provider is first created. If no ACD Addresses are
   * associated with this Provider, then this method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let ACDAddress[] addresses = this.getACDAddresses()
   * <LI>addresses == null or addresses.length >= 1
   * </OL>
   * <p>
   * @return An array of ACDAddresses in the Provider's domain
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public ACDAddress[] getACDAddresses()
    throws MethodNotSupportedException;


  /**
   * Returns an array of ACD manager Addresses associated with the Provider and
   * within the Provider's domain. This list is static (i.e. is does not
   * change) after the Provider is first created. If no ACD manager Addresses
   * are associated with this Provider, then this method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let ACDManagerAddress[] addresses = this.getACDManagerAddresses()
   * <LI>addresses == null or addresses.length >= 1
   * </OL>
   * <p>
   * @return An array of ACDManagerAddresses in the Provider's domain
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public ACDManagerAddress[] getACDManagerAddresses()
    throws MethodNotSupportedException;
}
