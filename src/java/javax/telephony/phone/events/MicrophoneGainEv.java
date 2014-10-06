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
 * The MicrophoneGainEv interface extends the PhoneTermEv interface and is
 * reported via the PhoneTerminalObserver interface. This event interface
 * indicates that the gain of a microphone component has changed.
 * <p>
 * Applications may use the <EM>getGain()</EM> method on this interface
 * to obtain the new gain of the microphone component.
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.phone.PhoneTerminalEvent}
 */
public interface MicrophoneGainEv extends PhoneTermEv {

  /**
   * Event id
   */
  public static final int ID = 505;

  /**
   * Returns the gain of the microphone.
   * <p>
   * @return The gain of the microphone.
   */
  public int getGain();
}
