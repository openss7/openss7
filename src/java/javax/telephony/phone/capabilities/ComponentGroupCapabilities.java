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
import  javax.telephony.Address;
/**
 * Component Group Capabilities interface
 *
 */
public interface ComponentGroupCapabilities {

  /**
   * Returns true if the ComponentGroup can be "activated" on the Terminal
   * that the ComponentGroup is associated with. For example, activation 
   * of a headset on a certain Terminal allows media to flow between the 
   * headset and the telephone line associated with the terminal for all 
   * calls on the line . This method allows the application to determine 
   * if activation of the ComponentGroup on its Terminal is supported.
   * <p>
   * @return True if the component group can be activated on its Terminal,
   * false otherwise.
   */
  public boolean canActivate();


  /**
   * Returns true if the ComponentGroup can be "activated" on the specified 
   * Address at the Terminal that the ComponentGroup is associated with. 
   * For example, activation of a headset on a certain Address at a Terminal 
   * allows media to flow between the headset and the telephone line 
   * associated with the Terminal for all calls on the specified Address. 
   * This method allows the application to determine if activation of the 
   * ComponentGroup on a specific Address at a Terminal is supported.
   * <p>
   * @param address test if feature available for this address
   * @return True if the component group can be activated on its Terminal
   * at the specified Address, false otherwise.
   */
  public boolean canActivate(Address address); 
}
