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

package javax.telephony.callcontrol.capabilities;
import  javax.telephony.capabilities.AddressCapabilities;

/**
 * The <CODE>CallControlAddressCapabilities</CODE> interface extends the core
 * <CODE>AddressCapabilities</CODE> interface. This interface provides
 * methods to reflect the capabilities of the methods on the
 * <CODE>CallControlAddress</CODE> interface.
 * <p>
 * The <CODE>Provider.getAddressCapabilities()</CODE> method returns the
 * static Address capabilities, and the
 * <CODE>Address.getCapabilities()</CODE> method returns the dynamic Address
 * capabilities. The object returned from each of these methods can be queried
 * with the <CODE>instanceof</CODE> operator to check if it supports this
 * interface. This same interface is used to reflect both static and dynamic
 * Address capabilities.
 * <p>
 * @see javax.telephony.Provider
 * @see javax.telephony.Address
 * @see javax.telephony.capabilities.AddressCapabilities
 */

public interface CallControlAddressCapabilities extends AddressCapabilities {

  /**
   * Returns true if the application can set the forwarding on this Address,
   * false otherwise.
   * <p>
   * @return True if the application can set the forwarding on this Address,
   * false otherwise.
   */
  public boolean canSetForwarding();


  /**
   * Returns true if the application can obtain the current forwarding status
   * on this Address, false otherwise.
   * <p>
   * @return True if the application can obtain the current forwarding status
   * on this Address, false otherwise.
   */
  public boolean canGetForwarding();


  /**
   * Returns true if the application can cancel the forwarding on this Address,
   * false otherwise.
   * <p>
   * @return True if the application can cancel the forwarding on this Address,
   * false otherwise.
   */
  public boolean canCancelForwarding();


  /**
   * Returns true if the application can obtain the do not disturb state, false
   * otherwise.
   * <p>
   * @return True if the application can obtain the do not disturb state, false
   * otherwise.
   */
  public boolean canGetDoNotDisturb();


  /**
   * Returns true if the application can set the do not disturb state, false
   * otherwise.
   * <p>
   * @return True if the application can set the do not disturb state, false
   * otherwise.
   */
  public boolean canSetDoNotDisturb();


  /**
   * Returns true if the application can obtain the message waiting state,
   * false otherwise.
   * <p>
   * @return True if the application can obtain the message waiting state,
   * false otherwise.
   */
  public boolean canGetMessageWaiting();


  /**
   * Returns true if the application can set the message waiting state, false
   * otherwise.
   * <p>
   * @return True if the application can set the message waiting state, false
   * otherwise.
   */
  public boolean canSetMessageWaiting();
}
