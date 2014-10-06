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

package javax.telephony.phone.capabilities;
/**
 * Component capabilities
 *
 */
public interface ComponentCapabilities {

  /**
   * Returns true if the component can be observed. For example, 
   * this method on a PhoneMicrophone component would return true, if 
   * events for changes in gain setting can be received through
   * the TerminalObserver interface and also if the "get" methods on
   * each of the component interfaces is expected to be successful.
   * <p>
   * @return True if the component can be observed, false otherwise.
   */
  public boolean canObserve();


  /**
   * Returns true if the component can be controlled. For example, 
   * this method on a PhoneMicrophone component would return true, if 
   * the gain setting can be adjusted programmatically.
   * <p>
   * @return True if the component can be controlled, false otherwise.
   */
  public boolean canControl();


  /**
   * Returns true if a button-press threshold time can be set.
   * @return True if a button press threshold may be set, false otherwise.
   * @since 1.4
   */
  public boolean canSetButtonPressThreshold();

}
