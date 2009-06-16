/*
#pragma ident "@(#)AddressCapabilities.java	1.9      99/02/23     SMI"

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

package javax.telephony.capabilities;

/** 
 * The <CODE>AddressCapabilities</CODE> interface represents the initial
 * capabilities interface for the Address. This interface supports basic
 * queries for the core package.
 * <p>
 * Applications obtain the static Address capabilities via the
 * <CODE>Provider.getAddressCapabilities()</CODE> method, and the dynamic
 * capabilities via the <CODE>Address.getCapabilities()</CODE> method. This
 * interface is used to represent both static and dynamic capabilities.
 * <p>
 * Any package which extends the core <CODE>Address</CODE> interface should
 * also extend this interface to provide additional capability queries for
 * that particular package.
 * <p>
 * @see javax.telephony.Provider
 * @see javax.telephony.Address
 */

public interface AddressCapabilities {

  /**
   * Returns true if this Address can be observed, false otherwise.
   * <p>
   * @return True if this Address can be observed, false otherwise.
   */
  public boolean isObservable();
}
