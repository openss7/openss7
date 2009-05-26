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
    An InvalidStateException indicates that that current state of an object involved in the 
    method invocation does not meet the acceptable pre-conditions for the method. Each method 
    which changes the call model typically has a set of states in which the object must be as 
    a pre-condition for the method. Each method documents the pre-condition states for objects.
    Typically, this method might succeed later when the object in question reaches the proper state.
    <p>
    This exception provides the application with the object in question and the state it is 
    currently in.
    @since 1.0b
*/
public class InvalidStateException extends Exception {

  /**
   * The type of the party.
   * @serial
   */
  private int    _type;
  /**
   * The current state.
   * @serial
   */
  private int    _state;
  /**
   * The current object.
   * @serial
   */
  private Object _object;

  /**
   * The invalid object in question is the Provider
   */
  public static final int PROVIDER_OBJECT = 0;

  /**
   * The invalid object in question is the Call
   */
  public static final int CALL_OBJECT = 1;

  /**
   * The invalid object in question is the Connection
   */
  public static final int CONNECTION_OBJECT = 2;

  /**
   * The invalid object in question is the Address
   */
  public static final int ADDRESS_OBJECT = 4;

  /**
   * Constructor with no string.
   * 
   * @param object instance associated with the invalid sate.
   * @param type type of failure
   * @param state current state at time of fault
   */
  public InvalidStateException(Object object, int type, int state) {
    super();
    _type   = type;
    _object = object;
    _state  = state;
  }


  /**
   * Constructor which takes a string description.
   *
   * @param object instance associated with the invalid sate.
   * @param type type of failure
   * @param state current state at time of fault
   * @param s desciption of the fault
   */
  public InvalidStateException(Object object, int type, int state, String s) {
    super(s);
    _type   = type;
    _object = object;
    _state = state;
  }

  /**
   * Returns the type of object in question.
   * 
   * @return The type of object in question.
   */
  public int getObjectType() {
    return _type;
  }


  /**
   * Returns the object which has the incorrect state.
   * 
   * @return The object which is in the wrong state.
   */
  public Object getObject() {
    return _object;
  }


  /**
   * Returns the state of the object.
   * <p>
   * @return The state of the object.
   */
  public int getState() {
    return _state;
  }
}
