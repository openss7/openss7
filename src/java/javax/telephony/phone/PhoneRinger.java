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
 * Phone ringer interface.
 *
 */
public interface PhoneRinger extends Component {

  /**
   * Ringer volume definition for the ringer off.
   */
  public static final int OFF  = 0;


  /**
   * Ringer volume definition for the middle volume.
   */
  public static final int MIDDLE  = 50;


  /**
   * Ringer volume definition for the ringer at maximum volume.
   */
  public static final int FULL = 100; 


  /**
   * Returns true if the ringer is on, false otherwise.
   * <p>
   * @return True if the ringer is on, false otherwise
   */
  public int isRingerOn();


  /**
   * Returns the current ringer volume.
   * <p>
   * @return The current ringer volume.
   */
  public int getRingerVolume();


  /**
   * Sets the ringer volume between ZERO or FULL, inclusive.
   * <p>
   * @param volume The ringer volume, between ZERO and FULL, inclusive.
   * @exception InvalidArgumentException The volume provided was not valid.
   */
  public void setRingerVolume(int volume) throws InvalidArgumentException;


  /**
   * Returns the current ringer pattern.
   * <p>
   * @return The current ringer pattern.
   */
  public int getRingerPattern();


  /**
   * Returns the number of available ringing patterns. An index between zero
   * and the returns value minus one may be used for the setRingerPattern()
   * method.
   * <p>
   * @return The number of available ringer patterns.
   */
  public int getNumberOfRingPatterns();


  /**
   * Set the ringer pattern given an valid index number returned by
   * getNumberOfRingPatterns().
   * <p>
   * @param ringerPattern The desired ringer pattern.
   * @exception InvalidArgumentException The ring pattern provided was not
   * valid.
   */
  public void setRingerPattern(int ringerPattern)
    throws InvalidArgumentException;

  
  /**
   * Returns the number of complete ring cycles that the ringer has been
   * ringing. A value of 0 indicates that the ringer is not being rung.
   * <p>
   * @return The current ringer count.
   */
  public int getNumberOfRings();
}
