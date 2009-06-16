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
 * The ButtonPressEv interface extends the PhoneTermEv interface and is
 * reported via the PhoneTermObserver interface. This event interface
 * indicates that a button component has been pressed.
 * <p>
 * Applications may obtain the identifying information associated with
 * this button via the <EM>getInfo()</EM> method.
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.phone.PhoneTerminalEvent}
 */
public interface ButtonPressEv extends PhoneTermEv {

  /**
   * Event id
   */
  public static final int ID = 501;

  /**
   * Returns the button information.
   * <p>
   * @return The string button information.
   */
  public String getInfo();
}
