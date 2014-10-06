/*
#pragma ident "@(#)CallCentEv.java	1.13      99/02/09     SMI"

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
import  javax.telephony.events.Ev;

/**
 * The <CODE>CallCentEv</CODE> interface is the base event interface for all
 * call center package events. This interface extends the core <CODE>Ev</CODE>
 * interface. All call center package events must extend this interface.
 * <p>
 * This interface contains a single method <CODE>getCallCenterCause()</CODE>
 * which returns the call center package-specific cause value for the event.
 * <p>
 * The call center package defines the following interfaces which directly
 * extend this interface: <CODE>ACDAddrEv</CODE>, <CODE>AgentTermEv</CODE>,
 * and <CODE>CallCenterCallEv</CODE>.
 * <p>
 * @see javax.telephony.events.Ev
 * @see javax.telephony.callcenter.events.ACDAddrEv
 * @see javax.telephony.callcenter.events.AgentTermEv
 * @see javax.telephony.callcenter.events.CallCentCallEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.CallCenterEvent}
 */
public interface CallCentEv extends Ev {

  /**
   * This cause indicates no agents were available to handle the call.
   */
   public static final int CAUSE_NO_AVAILABLE_AGENTS = 302;


  /**
   * Returns the call center package cause value for this event. This method
   * may also return the <CODE>Ev.CAUSE_NORMAL</CODE> constant or the
   * <CODE>Ev.CAUSE_UNKNOWN</CODE> constant.
   * <p>
   * @return The call center package cause for the event.
   */
  public int getCallCenterCause();
}

