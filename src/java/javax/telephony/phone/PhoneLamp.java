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
 * Phone lamp interface.
 *
 */
public interface PhoneLamp extends Component {

  /**
   * The lamp mode is OFF.
   */
  public static final int LAMPMODE_OFF           = 0x0;

  
  /**
   * The lamp mode is FLASH, which means slow on and off.
   */
  public static final int LAMPMODE_FLASH         = 0x1;

  
  /**
   * The lamp is STEADY, which means continuously lit.
   */
  public static final int LAMPMODE_STEADY        = 0x2;

  
  /**
   * The lamp mode is FLUUTER, which means fast on and off.
   */
  public static final int LAMPMODE_FLUTTER       = 0x3;

  
  /**
   * The lamp mode is BROKENFLUTTER, which is the superposition of flash and
   * flutter.
   */
  public static final int LAMPMODE_BROKENFLUTTER = 0x4;


  /**
   * The lamp mode is WINK.
   */
  public static final int LAMPMODE_WINK          = 0x5;

  
  /**
   * Returns an array of supported lamp modes.
   * <p>
   * @return An array of supported lamp modes.
   */
  public int[] getSupportedModes();

  
  /**
   * Sets the current lamp mode to a mode supported by the lamp and
   * returns by getSupportedModes().
   * <p>
   * @param mode The desired lamp mode.
   * @exception InvalidArgumentException The provided lamp mode is not valid.
   */
  public void setMode(int mode) throws InvalidArgumentException;

  
  /**
   * Returns the current lamp mode.
   * <p>
   * @return The current lamp mode.
   */
  public int getMode();
  

  /**
   * Returns the button associated with the lamp.
   * <p>
   * @return The button associated with the lamp.
   */
  public PhoneButton getAssociatedPhoneButton();
}
