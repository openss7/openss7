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

package javax.telephony.phone.events;

/**
 * The SpeakerVolumeEv interface extends the PhoneTermEv interface and is
 * reported via the PhoneTerminalObserver interface. This event interface
 * indicates that the volume of a speaker component has changed.
 * <p>
 * Applications may use the <EM>getVolume()</EM> method on this interface
 * to obtain the new volume of the speaker component.
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.phone.PhoneTerminalEvent}
 */
public interface SpeakerVolumeEv extends PhoneTermEv {

  /**
   * Event id
   */
  public static final int ID = 508;

  /**
   * Returns the volume of the speaker.
   * <p>
   * @return The volume of the speaker.
   */
  public int getVolume();
}
