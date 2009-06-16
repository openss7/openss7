/*
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


package javax.telephony.media.events;

/**
 * The MediaTermConnUnavailableEv interface indicates that there is currently
 * no media available on the TerminalConnection. This event is most likely
 * cause by a change in state of the TerminalConnection which respect to the
 * call. For example, when someone goes on hold, media is no longer avaiable
 * on that TerminalConnection. This event interface extends the
 * javax.telephony.events.TermConnEv interface, through which the application may
 * obtain the TerminalConnection object associated with this event.
 *
 * @deprecated   As of JTAPI 1.3, replaced by {@link javax.telephony.media.MediaService}
 */

public interface MediaTermConnUnavailableEv extends MediaTermConnEv {

  /**
   * Event id
   */
  public static final int ID = 403;

}
