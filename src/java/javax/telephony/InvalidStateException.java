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
 * An <CODE>InvalidStateException</CODE> indicates the current state of an object involved
 * in the method invocation does not meet the acceptable pre-conditions for
 * the method. Each method which changes the call model typically has a set of
 * states in which the object must be as a pre-condition for the method. Each
 * method documents the pre-condition states for objects. Typically, this
 * method will succeed in the future once the object in question has reached
 * the proper state. 
 * <p>
 * This exception provides the application with the object in question and
 * the state it is currently in.
 * @version 03/23/99 1.11
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
   * The invalid object in question is the Terminal
   */
  public static final int TERMINAL_OBJECT = 3;

  /**
   * The invalid object in question is the Address
   */
  public static final int ADDRESS_OBJECT = 4;


  /**
   * The invalid object in question is the Terminal Connection
   */
  public static final int TERMINAL_CONNECTION_OBJECT = 5;


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
