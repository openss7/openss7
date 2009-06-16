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
 * Phone button interface.
 *
 */
public interface PhoneButton extends Component {

  /**
   * Returns the button information.
   * <p>
   * @return The string button information.
   */
  public String getInfo();


  /**
   * Sets button information.
   * <p>
   * @param buttonInfo The button information.
   */
  public void setInfo(String buttonInfo);


  /**
   * Returns the associated lamp information.
   * <p>
   * @return The associated lamp object.
   */
  public PhoneLamp getAssociatedPhoneLamp();


  /**
   * Press the button.
   * <p>
   */
  public void buttonPress();
  
  /**
   * Set the button press duration threshold.
   * When a button is pressed and held for this duration or longer, an application
   * may wish to take special action.  By setting this threshold value (in milliseconds)
   * the application asks that an event be sent indicating that the button was pressed
   * and held "for a long time."
   * <p>
   * The default threshold is
   * <i><b>ISSUE: implementation dependent (should we pick a number, like 1000 millisec?)</b></i>;
   * if the threshold is set to 0, the PHONE_TERMINAL_BUTTON_PRESS_THRESHOLD_EXCEEDED_EVENT
   * event is sent immediately after PHONE_TERMINAL_BUTTON_PRESS_EVENT.
   * th
   * <p>
   * @since 1.4
   */
  public int setButtonPressThreshold(int threshold);
  
  
  /**
   * Return the button press duration threshold.
   * <p>
   * @since 1.4
   */
  public int getButtonPressThreshold();
}
