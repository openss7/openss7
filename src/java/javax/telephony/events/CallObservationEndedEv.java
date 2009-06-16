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

package javax.telephony.events;

/**
 * The <CODE>CallObservationEndedEv</CODE> event indicates that the application
 * will no longer receive Call events on the instance of the
 * <CODE>CallObserver</CODE> This interface extends the <CODE>CallEv</CODE>
 * interface and is reported on the <CODE>CallObserver</CODE> interface.
 * <p>
 * @see javax.telephony.events.CallEv
 * @see javax.telephony.CallObserver
 */

public interface CallObservationEndedEv extends CallEv {

  /**
   * Event id
   */
  public static final int ID = 103;


  /**
   * This method returns the object which is responsible for the observation of
   * the call ending. If this method returns a Call, the observation ended
   * because either the Call could no longer be observed or the observer was
   * removed via the <CODE>Call.removeObserver()</CODE> method. If this method
   * returns either an Address or Terminal, then additional obsevers will not
   * be added if the Call arrives at the returned Address or Terminal.
   * <p>
   * @since JTAPI v1.2
   * @return The object responsible for the observation ending for the Call.
   */
  public Object getEndedObject();
}
