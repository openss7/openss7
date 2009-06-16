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
 * Phone hook switch interface.
 *
 */
public interface PhoneHookswitch extends Component {

  /**
   * The Hookswitch is ON_HOOK.
   */
  public static final int ON_HOOK = 0;
  

  /**
   * The Hookswitch is OFF_HOOK.
   */
  public static final int OFF_HOOK = 1;

  
  /**
   * Sets the state of the hookswitch to either ON_HOOK or OFF_HOOK.
   * <p>
   * @param hookSwitchState The desired state of the hook switch.
   * @exception InvalidArgumentException The provided hookswitch state is not
   * valid.
   */
  public void setHookSwitch(int hookSwitchState)
    throws InvalidArgumentException;


  /**
   * Returns the current state of the hookswitch.
   * <p>
   * @return The current state of the hookswitch.
   */
  public int getHookSwitchState();
}

