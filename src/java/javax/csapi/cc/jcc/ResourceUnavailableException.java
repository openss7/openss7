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
    This exception indicates that a resource inside the system is not available to complete
    an operation. The type embodied in this exception clarifies what is not available and is
    obtained via the {@link #getType()} method in this class.
    @since 1.0b
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
   * Return the type of the exception.
   * @return the type of the exception
   */
  public int getType() {
    return _type;
  }
}

