/*
#pragma ident "@(#)TermConnPassiveEv.java	1.10      99/03/02     SMI"

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

package javax.telephony.events;

/**
 * The <CODE>TermConnPassiveEv</CODE> interface indicates that the state of
 * the TerminalConnection object has changed to
 * <CODE>TerminalConnection&#46PASSIVE</CODE>. This interface extends the
 * <CODE>TermConnEv</CODE> interface and is reported via the
 * <CODE>CallObserver</CODE> interface.
 * <p>
 * @see javax.telephony.TerminalConnection
 * @see javax.telephony.CallObserver
 * @see javax.telephony.events.TermConnEv
 */

public interface TermConnPassiveEv extends TermConnEv {

  /**
   * Event id
   */
  public static final int ID = 118;
}

