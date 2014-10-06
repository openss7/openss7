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

package javax.telephony.callcontrol.events;

/**
 * The <CODE>CallCtlTermDoNotDisturbEv</CODE> interface indicates the state of
 * the do not disturb feature has changed for the Terminal. This interface
 * extends the <CODE>CallCtlTermEv</CODE> interface and is reported via the
 * <CODE>TerminalObserver.terminalChangedEvent()</CODE> method. The observer
 * object must also implement the <CODE>CallControlTerminalObserver</CODE>
 * interface to signal it is interested in call control package events.
 * <p>
 * This interface supports a single method which returns the current state of
 * the do not disturb feature.
 * <p>
 * @see javax.telephony.Terminal
 * @see javax.telephony.TerminalObserver
 * @see javax.telephony.callcontrol.CallControlTerminal
 * @see javax.telephony.callcontrol.CallControlTerminalObserver
 * @see javax.telephony.callcontrol.events.CallCtlTermEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcontrol.CallControlTerminalEvent}
 */

public interface CallCtlTermDoNotDisturbEv extends CallCtlTermEv {

  /**
   * Event id
   */
  public static final int ID = 221;


  /**
   * Returns true if the do not disturb feature is activated, false otherwise.
   * <p>
   * @return True if the do not disturb feature is activated, false otherwise.
   */
  public boolean getDoNotDisturbState();
}

