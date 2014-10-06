/*
#pragma ident "@(#)CallControlConnectionCapabilities.java	1.5      97/09/25     SMI"

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
import  javax.telephony.capabilities.ConnectionCapabilities;

/**
 * The <CODE>CallControlConnectionCapabilities</CODE> interface extends the
 * core <CODE>ConnectionCapabilities</CODE> interface. This interface provides
 * methods to reflect the capabilities of the methods on the
 * <CODECallControlConnection</CODE> interface.
 * <p>
 * The <CODE>Provider.getConnectionCapabilities()</CODE> method returns the
 * static Connection capabilities, and the
 * <CODE>Connection.getCapabilities()</CODE> method returns the dynamic
 * Connection capabilities. The object returned from each of these methods can
 * be queried with the <CODE>instanceof</CODE> operator to check if it supports
 * this interface. This same interface is used to reflect both static and
 * dynamic Connection capabilities.
 * <p>
 * @see javax.telephony.Provider
 * @see javax.telephony.Connection
 * @see javax.telephony.capabilities.ConnectionCapabilities
 */

public interface CallControlConnectionCapabilities extends ConnectionCapabilities {

  /**
   * Returns true if the application can invoke the redirect feature, false
   * otherwise.
   * <p>
   * @return True if the application can invoke the redirect feature, false
   * otherwise.
   */
  public boolean canRedirect();


  /**
   * Returns true if the application can invoke the add to address feature,
   * false otherwise.
   * <p>
   * @return True if the application can invoke the add to address feature,
   * false otherwise.
   */
  public boolean canAddToAddress();


  /**
   * Returns true if the application can invoke the accept feature, false
   * otherwise.
   * <p>
   * @return True if the application can invoke the accept feature, false
   * otherwise.
   */
  public boolean canAccept();


  /**
   * Returns true if the application can invoke the reject feature, false
   * otherwise.
   * <p>
   * @return True if the application can invoke the reject feature, false
   * otherwise.
   */
  public boolean canReject();


  /**
   * Returns true if the application can invoke the park feature, false
   * otherwise.
   * <p>
   * @return True if the application can invoke the park feature, false
   * otherwise.
   */
  public boolean canPark();
}
