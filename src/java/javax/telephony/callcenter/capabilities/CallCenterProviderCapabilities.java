/*
 * Copyright (c) 1997 Sun Microsystems, Inc. All Rights Reserved.
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

package javax.telephony.callcenter.capabilities;
import  javax.telephony.capabilities.ProviderCapabilities;

/**
 * The CallCenterProviderCapabilities interface extends the
 * ProviderCapabilities interface to add capabilities methods for the
 * CallCenterProvider interface. Applications query these methods to find out
 * what actions are possible on the CallCenterProvider interface.
 */
public interface CallCenterProviderCapabilities extends ProviderCapabilities {

  /**
   * This method returns true if the method getRouteableAddresses on the
   * CallCenterProvider interface is supported.
   * <p>
   * @return True if the method addCallObserver on the CallCenterProvider
   * interface is supported.
   */
  public boolean canGetRouteableAddresses();


  /**
   * This method returns true if the method getACDAddresses on the
   * CallCenterProvider interface is supported.
   * <p>
   * @return True if the method getACDAddresses on the CallCenterProvider
   * interface is supported.
   */
  public boolean canGetACDAddresses();


  /**
   * This method returns true if the method getACDManagerAddresses on the
   * CallCenterProvider interface is supported.
   * <p>
   * @return True if the method CallCenterProvodier.getACDManagerAddresses()
   * is supported.
   */
  public boolean canGetACDManagerAddresses();
}
