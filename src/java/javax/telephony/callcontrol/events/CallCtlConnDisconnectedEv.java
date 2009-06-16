/*
#pragma ident "%W%      %E%     SMI"

 * Copyright (c) 1996 Sun Microsystems, Inc. All Rights Reserved.
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
 * The <CODE>CallCtlConnDisconnectedEv</CODE> interface indicates that the call
 * control package state of the Connection is now
 * <CODE>CallControlConnection.DISCONNECTED</CODE>. This method
 * <CODE>CallControlConnection.getCallControlState()</CODE> returns the
 * current state. This interface extends the <CODE>CallCtlConnEv</CODE>
 * interface and is reported via the
 * <CODE>CallObserver.callChangedEvent()</CODE> method. The observer object
 * must also implement the <CODE>CallControlCallObserver</CODE> interface to
 * signal it is interested in call control package events.
 * <p>
 * This interface supports no additional methods.
 * <p>
 * @see javax.telephony.CallObserver
 * @see javax.telephony.Connection
 * @see javax.telephony.callcontrol.CallControlConnection
 * @see javax.telephony.callcontrol.CallControlCallObserver
 * @see javax.telephony.callcontrol.events.CallCtlConnEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcontrol.CallControlConnectionEvent}
 */

public interface CallCtlConnDisconnectedEv extends CallCtlConnEv {

  /**
   * Event id
   */
  public static final int ID = 205;
}

