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
 * Phone display interface.
 *
 */

public interface PhoneDisplay extends Component {

  /**
   * Returns the number of display rows.
   * <p>
   * @return The number of display rows.
   */
  public int getDisplayRows();

  
  /**
   * Returns the number of display columns.
   * <p>
   * @return The number of display columns.
   */
  public int getDisplayColumns();

  
  /**
   * Returns the displayed string starting at coordinates (x, y).
   * <p>
   * @param x The x-coordinate.
   * @param y The y-coordinate.
   * @return The string displayed starting at coordinates (x, y).
   * @exception InvalidArgumentException Either the coordinates provided
   * were invalid.
   */
  public String getDisplay(int x, int y)
    throws InvalidArgumentException;


  /**
   * Displays the given string starting at coordinates (x, y).
   * <p>
   * @param string The string to display.
   * @param x The x-coordinate.
   * @param y The y-coordinate.
   * @exception InvalidArgumentException Either the coordinates provided
   * were invalid.
   */
  public void setDisplay(String string, int x, int y)
    throws InvalidArgumentException;
}
