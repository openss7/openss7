package javax.csapi.cc.jcc;

/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 2001, 2002 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Telcordia Technologies, Inc.
 *  Farooq Anjum, John-Luc Bakker, Ravi Jain
 *  445 South Street
 *  Morristown, NJ 07960
 *
 *  Version       : 1.1
 *  Notes         :
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/**
    This exception indicates that a party given as an argument to the method call was invalid. 
    This may either be the originating party of a telephone call or the destination party of a
    telephone call. 
    @since 1.0b
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
