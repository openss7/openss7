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

package javax.telephony.phone;
import  javax.telephony.*;
/**
 * Phone speaker interface
 *
 */
public interface PhoneSpeaker extends Component {

  /**
   * Speaker volume definition for muting.
   */
  public static final int MUTE = 0;


  /**
   * Speaker volume definition for the middle volume.
   */
  public static final int MID = 50;


  /**
   * Speaker volume definition for highest volume.
   */
  public static final int FULL = 100;
  

  /**
   * Returns the volume of the speaker.
   * <p>
   * @return The volume of the speaker.
   */
  public int getVolume();
  

  /**
   * Sets the speaker or handset volume. The volume value may be anything
   * between MUTE or FULL, inclusive.
   * <p>
   * @param volume The volume, between MUTE and FULL.
   */
  public void setVolume(int volume);
}
