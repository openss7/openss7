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
 * The MediaTermConnAvailableEv interface indicates that media is currently
 * available on the TerminalConnection. Media becomes available on the
 * TerminalConnection when the state of the TerminalConnection changes with
 * respect to the telephone call. For example, when a TerminalConnection
 * becomes active on the telephone call, media is made available to the
 * application. This event interface extends the javax.telephony.events.TermConnEv
 * interface, through which the application may obtain the TerminalConnection
 * object associated with this event.
 *
 * @deprecated   As of JTAPI 1.3, replaced by {@link javax.telephony.media.MediaService}
 */

public interface MediaTermConnAvailableEv extends MediaTermConnEv {

  /**
   * Event id
   */
  public static final int ID = 400;
}
