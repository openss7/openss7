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
import  javax.telephony.capabilities.TerminalConnectionCapabilities;

/**
 * The <CODE>CallControlTerminalConnectionCapabilities</CODE> interface
 * extends the core <CODE>TerminalConnectionCapabilities</CODE> interface.
 * This interface provides methods to reflect the capabilities of the methods
 * on the <CODECallControlTerminalConnection</CODE> interface.
 * <p>
 * The <CODE>Provider.getTerminalConnectionCapabilities()</CODE> method returns
 * the static TerminalConnection capabilities, and the
 * <CODE>TerminalConnection.getCapabilities()</CODE> method returns the dynamic
 * TerminalConnection capabilities. The object returned from each of these
 * methods can be queried with the <CODE>instanceof</CODE> operator to check
 * if it supports this interface. This same interface is used to reflect both
 * static and dynamic TerminalConnection capabilities.
 * <p>
 * @see javax.telephony.Provider
 * @see javax.telephony.TerminalConnection
 * @see javax.telephony.capabilities.TerminalConnectionCapabilities
 */

public interface CallControlTerminalConnectionCapabilities extends TerminalConnectionCapabilities {

  /**
   * Returns true if the application can invoke the hold feature, false
   * otherwise.
   * <p>
   * @return True if the application can invoke the hold feature, false
   * otherwise.
   */
  public boolean canHold();


  /**
   * Returns true if the application can invoke the unhold feature, false
   * otherwise.
   * <p>
   * @return True if the application can invoke the unhold feature, false
   * otherwise.
   */
  public boolean canUnhold();


  /**
   * Returns true if the application can invoke the join feature, false
   * otherwise.
   * <p>
   * @return True if the application can invoke the join feature, false
   * otherwise.
   */
  public boolean canJoin();


  /**
   * Returns true if the application can invoke the leave feature, false
   * otherwise.
   * <p>
   * @return True if the application can invoke the leave feature, false
   * otherwise.
   */
  public boolean canLeave();
}
