/*
#pragma ident "@(#)TerminalCapabilities.java	1.9      99/02/23     SMI"

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
 * The <CODE>TerminalCapabilities</CODE> interface represents the initial
 * capabilities interface for the Terminal. This interface supports basic
 * queries for the core package.
 * <p>
 * Applications obtain the static Terminal capabilities via the
 * <CODE>Provider.getTerminalCapabilities()</CODE> method, and the dynamic
 * capabilities via the <CODE>Terminal.getCapabilities()</CODE> method. This
 * interface is used to represent both static and dynamic capabilities.
 * <p>
 * Any package which extends the core <CODE>Terminal</CODE> interface should
 * also extend this interface to provide additional capability queries for
 * that particular package.
 * <p>
 * @see javax.telephony.Provider
 * @see javax.telephony.Terminal
 */

public interface TerminalCapabilities {

  /**
   * Returns true if this Terminal is observable, false otherwise.
   * <p>
   * @return True if this Terminal is observable, false otherwise.
   */
  public boolean isObservable();
}
