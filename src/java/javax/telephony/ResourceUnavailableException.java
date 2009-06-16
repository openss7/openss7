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
 * The <CODE>ResourceUnavailableException</CODE> indicates that a resource inside the
 * system in not available to complete an operation. The type embodied in this
 * exception further clarifies what is not available and is obtained via the
 * <CODE>ResourceUnavailableException.getType()</CODE> method.
 * @version 03/23/99 1.11
 */ 

public class ResourceUnavailableException extends Exception {

  /**
   * This private variable stores the type of resource.
   * @serial
   */
  private int _type;


  /**
   * Indicates the specific reason is unspecified.
   */
  public static final int UNKNOWN = 0;


  /**
   * The originating device was not available for this action.
   */
  public static final int ORIGINATOR_UNAVAILABLE = 1;


  /**
   * The number of observers existing already reached the limit.
   */
  public static final int OBSERVER_LIMIT_EXCEEDED = 2;


  /**
   * The number of trunks which are currently in use has been
   * exceeded.
   */
  public static final int TRUNK_LIMIT_EXCEEDED = 3;


  /**
   * The internal resources to handle another method have been exceeded.
   */
  public static final int OUTSTANDING_METHOD_EXCEEDED = 4;


  /**
   * An internal resource, unspecified by the implementation, has been
   * exceeded.
   */
  public static final int UNSPECIFIED_LIMIT_EXCEEDED = 5;


  /**
   * No dialtone detected.
   */
  public static final int NO_DIALTONE = 6;


  /**
   * A user has not responded in the time allowed by an implementation.
   */
  public static final int USER_RESPONSE = 7;


  /**
   * Constructor, takes a type but no string.
   */
  public ResourceUnavailableException(int type) {
    super();
    _type = type;
  }


  /**
   * Constructor, takes a type and a string.
   */
  public ResourceUnavailableException(int type, String s) {
    super(s);
    _type = type;
  }


  /**
   * Returns the type of resource which was unavailable.
   * <p>
   * @return The type of resource unavailable.
   */
  public int getType() {
    return _type;
  }
}
