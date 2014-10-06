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
import  javax.telephony.Connection;

/**
 * The <CODE>ConnEv</CODE> interface is the base event interface for all
 * Connection-related events. All events which pertain to the Connection object
 * must extend this interface. This interface extends the <CODE>CallEv</CODE>
 * interface and therefore is reported via the <CODE>CallObserver</CODE>
 * interface.
 * <p>
 * The core package defines events which are reported when the Connection
 * changes state. These events are: <CODE>ConnInProgressEv</CODE>,
 * <CODE>ConnAlertingEv</CODE>, <CODE>ConnConnectedEv</CODE>,
 * <CODE>ConnDisconnectedEv</CODE>, <CODE>ConnFailedEv</CODE>, and
 * <CODE>ConnUnknownEv</CODE>. Also, the <CODE>ConnCreatedEv</CODE> is sent
 * when a new Connection is created.
 * <p>
 * The <CODE>ConnEv.getConnection()</CODE> method on this interface returns the
 * Connection associated with this Connection event.
 * <p>
 * @see javax.telephony.Connection
 * @see javax.telephony.CallObserver
 * @see javax.telephony.events.CallEv
 * @see javax.telephony.events.ConnCreatedEv
 * @see javax.telephony.events.ConnInProgressEv
 * @see javax.telephony.events.ConnAlertingEv
 * @see javax.telephony.events.ConnConnectedEv
 * @see javax.telephony.events.ConnDisconnectedEv
 * @see javax.telephony.events.ConnFailedEv
 * @see javax.telephony.events.ConnUnknownEv
 */

public interface ConnEv extends CallEv {

  /**
   * Returns the Connection associated with this Connection event.
   * <p>
   * @return The Connection associated with this event.
   */
  public Connection getConnection();
}
