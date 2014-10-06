/*
#pragma ident "@(#)ConnectionCapabilities.java	1.8      99/02/23     SMI"

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
 * The <CODE>ConnectionCapabilities</CODE> interface represents the initial
 * capabilities interface for the Connection. This interface supports basic
 * queries for the core package.
 * <p>
 * Applications obtain the static Connection capabilities via the
 * <CODE>Provider.getConnectionCapabilities()</CODE> method, and the dynamic
 * capabilities via the <CODE>Connection.getCapabilities()</CODE> method. This
 * interface is used to represent both static and dynamic capabilities.
 * <p>
 * Any package which extends the core <CODE>Connection</CODE> interface should
 * also extend this interface to provide additional capability queries for
 * that particular package.
 * <p>
 * @see javax.telephony.Provider
 * @see javax.telephony.Connection
 */

public interface ConnectionCapabilities {

  /**
   * Returns true if the application can invoke
   * <CODE>Connection.disconnect()</CODE>perform a disconnect(), false
   * otherwise.
   * <p>
   * @return True if the application can disconnect, false otherwise.
   */
  public boolean canDisconnect();
}


