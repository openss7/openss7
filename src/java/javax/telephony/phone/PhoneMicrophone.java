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
 * Phone microphone interface.
 *
 */
public interface PhoneMicrophone extends Component {

  /**
   * The microphone gain is MUTE.
   */
  public static final int MUTE = 0;


  /**
   * The microphone gain is MID.
   */
  public static final int MID = 50;


  /**
   * The full microhphone gain.
   */
  public static final int FULL = 100;


  /**
   * Returns the current microphone gain.
   * <p>
   * @return The current microphone gain.
   */
  public int getGain();


  /**
   * Sets the microphone gain to a value between MUTE and FULL, inclusive.
   * <p>
   * @param gain A microphone gain between MUTE and FULL, inclusive.
   * @exception InvalidArgumentException The microphone gain is not valid.
   */
  public void setGain(int gain) throws InvalidArgumentException;
}
