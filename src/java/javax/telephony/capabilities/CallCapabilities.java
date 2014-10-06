/*
#pragma ident "@(#)CallCapabilities.java	1.9      99/02/10     SMI"

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
 * The <CODE>CallCapabilities</CODE> interface represents the initial
 * capabilities interface for the Call. This interface supports basic queries
 * for the core package.
 * <p>
 * Applications obtain the static Call capabilities via the
 * <CODE>Provider.getCallCapabilities()</CODE> method, and the dynamic capabilities
 * via the <CODE>Call.getCapabilities()</CODE> method. This interface is used
 * to represent both static and dynamic capabilities.
 * <p>
 * Any package which extends the core <CODE>Call</CODE> interface should also
 * extend this interface to provide additional capability queries for that
 * particular package.
 * <p>
 * @see javax.telephony.Provider
 * @see javax.telephony.Call
 */

public interface CallCapabilities {

  /**
   * Returns true if the application can invoke <CODE>Call.connect()</CODE>,
   * false otherwise.
   * <p>
   * @return True if the application can perform a connect, false otherwise.
   */
  public boolean canConnect();


  /**
   * Returns true if this Call can be observed, false otherwise.
   * <p>
   * @return True if this Call can be observed, false otherwise.
   */
  public boolean isObservable();
}
