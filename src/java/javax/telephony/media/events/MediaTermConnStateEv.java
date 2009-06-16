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
 * Old JTAPI-1.2 event.<p>
 * The MediaTermConnStateEv interface indicates that the playing/recording
 * state has changed on the TerminalConnection object. This event interface
 * extends the javax.telephony.events.TermConnEv interface, through which the
 * application may obtain the TerminalConnection object associated with
 * this event.
 * <p>
 * Applications may obtain the new state via the getMediaState() method on this
 * interface, or via the MediaTerminalConnection.getMediaState() method.
 *
 * @deprecated   As of JTAPI 1.3, replaced by {@link javax.telephony.media.MediaEvent}
 */

public interface MediaTermConnStateEv extends MediaTermConnEv {

  /**
   * Event id
   */
  public static final int ID = 402;


  /**
   * Returns the current state of playing/recording on the TerminalConnection
   * in the form of a bit mask.
   * <p>
   * @return The current playing/recording state.
   */
  public int getMediaState();
}
