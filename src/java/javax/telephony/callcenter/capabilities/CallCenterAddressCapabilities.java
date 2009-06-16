/*
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

package javax.telephony.callcenter.capabilities;
import  javax.telephony.capabilities.AddressCapabilities;

/**
 * The CallCenterAddressCapabilities interface extends the AddressCapabilities
 * interface to add capabilities methods for the CallCenterAddress interface.
 * Applications query these methods to find out what actions are possible on
 * the CallCenterAddress interface.
 */

public interface CallCenterAddressCapabilities extends AddressCapabilities {

  /**
   * This method returns true if the method addCallObserver with the remain
   * flag on the CallCenterAddress interface is supported.
   *
   * @param remain addres supports the callcenter remain capability
   * @return True if the method addCallObserver on the CallCenterAddress
   * interface is supported.
   */
  public boolean canAddCallObserver(boolean remain);
}
