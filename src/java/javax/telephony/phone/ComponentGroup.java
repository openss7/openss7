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
import  javax.telephony.phone.capabilities.*;

/**
 * A ComponentGroup is a grouping of Component objects. Terminals may be
 * composed of zero or more ComponentGroups. Applications query the
 * PhoneTerminal interface for the available ComponentGroups. Then they query
 * this interface for the components which make up this component group.
 */

public interface ComponentGroup {

  /**
   * The component group is of type HEAD_SET.
   */
  public final static int HEAD_SET = 1;


  /**
   * The component group is of type HAND_SET.
   */
  public final static int HAND_SET = 2;


  /**
   * The component group is of type SPEAKER_PHONE.
   */
  public final static int SPEAKER_PHONE = 3;


  /**
   * The componet group is of type PHONE_SET.
   */
  public final static int PHONE_SET = 4;


  /**
   * The component group is of type OTHER.
   */
  public final static int OTHER = 5;


  /**
   * Returns the type of group, either HEAD_SET, HAND_SET, SPEAKER_PHONE,
   * PHONE_SET or OTHER.
   * <p>
   * @return The type of group.
   */
  public int getType();


  /**
   * Returns a string describing the component group.
   * <p>
   * @return A string description of the component group.
   */
  public String getDescription();


  /**
   * Returns the groups components, null if the group contains zero components.
   * <p>
   * @return An array of Component objects.
   */
  public Component[] getComponents();


  /**
   * Enables all routing of events or media stream between all Components
   * of this group and calls on any of the Addresses asociated with the parent
   * Terminal.
   * <p>
   * @return true if successful and false if unsuccessful.
   */
  public boolean activate();


  /**
   * Disables all routing of events or media stream between all Components
   * of this group and calls on any of the Addresses associated with the parent
   * Terminal.
   * <p>
   * @return true if successful and false if unsuccessful.
   */
  public boolean deactivate();


  /**
   * Enables all routing of events or media stream between all Components
   * of this group and calls to the specified Address.
   * <p>
   * @param address The Address that the group is to be activated on.
   * @return true if successful and false if unsuccessful.
   * @exception InvalidArgumentException The provided Address is not valid
   * for the Terminal.
   */
  public boolean activate(Address address)
    throws InvalidArgumentException;


  /**
   * Disables all routing of events or media stream between all Components
   * of this group and the specified Address.
   * <p>
   * @param address The Address that the group is to be deactivated on.
   * @return true if successful and false if unsuccessful.
   * @exception InvalidArgumentException The provided Address is not valid
   * for the Terminal.
   */
  public boolean deactivate(Address address)
    throws InvalidArgumentException;


  /**
   * Returns the dynamic capabilities for this <CODE>ComponentGroup</CODE>
   * instance. Static capabilities are not available for component groups.
   * <p>
   * @return The dynamic component group capabilities.
   */
  public ComponentGroupCapabilities getCapabilities();
}
