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

package javax.telephony.callcenter.capabilities;
import  javax.telephony.capabilities.AddressCapabilities;

/**
 * The ACDAddressCapabilities interface extends the AddressCapabilities
 * interface to add capabilities methods for the ACDAddress interface.
 * Applications query these methods to find out what actions are possible
 * on the ACDAddress interface.
 */

public interface ACDAddressCapabilities extends AddressCapabilities {

  /**
   * This method returns true if the method getLoggedOnAgents on the
   * ACDAddress interface is supported.
   * <p>
   * @return True if the method getLoggedOnAgents on the ACDAddress
   * interface is supported.
   */
  public boolean canGetLoggedOnAgents();


  /**
   * This method returns true if the method getNumberQueued on the
   * ACDAddress interface is supported.
   * <p>
   * @return True if the method getNumberQueued on the ACDAddress
   * interface is supported.
   */
  public boolean canGetNumberQueued();


  /**
   * This method returns true if the method getOldestCallQueued on the
   * ACDAddress interface is supported.
   * <p>
   * @return True if the method getOldestCallQueued on the ACDAddress
   * interface is supported.
   */
  public boolean canGetOldestCallQueued();


  /**
   * This method returns true if the method getRelativeQueueLoad on the
   * ACDAddress interface is supported.
   * <p>
   * @return True if the method getRelativeQueueLoad on the ACDAddress
   * interface is supported.
   */
  public boolean canGetRelativeQueueLoad();


  /**
   * This method returns true if the method getQueueWaitTime on the
   * ACDAddress interface is supported.
   * <p>
   * @return True if the method getQueueWaitTime on the ACDAddress
   * interface is supported.
   */
  public boolean canGetQueueWaitTime();


  /**
   * This method returns true if the method getACDManagerAddress on the
   * ACDAddress interface is supported.
   * <p>
   * @return True if the method getACDManagerAddress on the ACDAddress
   * interface is supported.
   */
  public boolean canGetACDManagerAddress();
}
