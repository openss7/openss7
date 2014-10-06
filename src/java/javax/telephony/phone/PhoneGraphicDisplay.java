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
import  javax.telephony.phone.Component;
import  javax.telephony.PlatformException;
import  java.awt.Graphics;
import  java.awt.Dimension;

/**
 * A PhoneGraphicsDisplay represents a display device that is
 * pixel-addressable, and which can be drawn into using AWT primitives.
 */

public interface PhoneGraphicDisplay extends Component {

  /**
  * Returns a Graphics object for drawing into the display.
  * <p>
  * @return A Graphic object, as defined in the AWT.
  */
  public Graphics getGraphics();


  /**
   * Returns the size of the display.
   * <p>
   * @return The size of the display, packaged in an AWT Dimension object.
   */
  public Dimension size();
}
