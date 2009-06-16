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

package javax.telephony.callcenter.events;

/**
 * The <CODE>ACDAddrLoggedOnEv</CODE> interface indicates that an Agent has
 * moved into the <CODE>Agent.LOG_ON</CODE> state. This interface extends the
 * <CODE>ACDAddrEv</CODE> interface and is reported via the
 * <CODE>ACDAddressObserver</CODE> interface for the <CODE>ACDAddress</CODE>
 * associated with the Agent.
 * <p>
 * This interface defines no additional methods.
 * <p>
 * @see javax.telephony.Terminal
 * @see javax.telephony.TerminalObserver
 * @see javax.telephony.callcenter.Agent
 * @see javax.telephony.callcenter.ACDAddressObserver
 * @see javax.telephony.callcenter.events.ACDAddrEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.ACDAddressEvent}
 */
public interface ACDAddrLoggedOnEv extends ACDAddrEv {

  /**
   * Event id
   */
  public static final int ID = 302;
}
