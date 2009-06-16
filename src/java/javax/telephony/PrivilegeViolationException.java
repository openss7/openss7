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

package javax.telephony;

/**
 * A <CODE>PrivilegeViolationException</CODE> indicates that an action pertaining to a
 * certain object failed because the application did not have the proper
 * security permissions to execute that command.
 * <p>
 * This class stores the type of privilege not available which is obtained via
 * the <CODE>PrivilegeViolationException.getType()</CODE> method.
 * @version 03/23/99 1.11
 */ 

public class PrivilegeViolationException extends Exception {

  /**
   * This private variable stores the type of privilege not available.
   * @serial
   */
  private int _type;


  /**
   * A privilege violation occurred on the originator.
   */
  public static final int ORIGINATOR_VIOLATION = 0;


  /**
   * A privilege violation occurred on the destination.
   */
  public static final int DESTINATION_VIOLATION = 1;


  /**
   * A privilege violation occurred at an unknown place.
   */
  public static final int UNKNOWN_VIOLATION = 2;


  /**
   * Constructor, takes a type but no string.
   *
   * @param type kind of violation
   */
  public PrivilegeViolationException(int type) {
    super();
    _type = type;
  }


  /**
   * Constructor, takes a type and a string.
   * @param type kind of violation
   * @param s description of the violation
   */
  public PrivilegeViolationException(int type, String s) {
    super(s);
    _type = type;
  }


  /**
   * Returns the type of privilege which is not available.
   * <p>
   * @return The type of privilege.
   */
  public int getType() {
    return _type;
  }
}
