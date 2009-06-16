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
 * An <CODE>InvalidPartyException</CODE> indicates that a party given as an argument to the
 * method call was invalid. This may either be the originating party of a
 * telephone call or the destination party of a telephone call.
 * @version 03/26/99 1.12
 */

public class InvalidPartyException extends Exception {

 /**
   * This private variable stores the type of party.
   * @serial
   */
  private int _type;


  /**
   * Indicates that the originating party was invalid.
   */
  public static final int ORIGINATING_PARTY = 0;


  /**
   * Indicates that the destination party was invalid.
   */
  public static final int DESTINATION_PARTY = 1;


  /**
   * Indicates that the party was unknown.
   */
  public static final int UNKNOWN_PARTY = 2;


  /**
   * Constructor with no string.
   * @param type the type of party expected.
   */
  public InvalidPartyException(int type) {
    super();
    _type = type;
  }


  /**
   * Constructor which takes a string description.
   *
   * @param type type of exception
   * @param s description of the fault
   */
  public InvalidPartyException(int type, String s) {
    super(s);
    _type = type;
  }


  /**
   * Returns the type of party.
   * <p>
   * @serial
   * @return The type of party.
   */
  public int getType() {
    return _type;
  }
}
