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
 * A <CODE>PlatformException</CODE> indicates an implementation-specific exception. The
 * specific exceptions which implementations throw is documented in their
 * release notes.
 * <p>
 * JTAPI v1.1.1 NOTE: PlatformException extends Java's RuntimeException. This
 * permits it to be thrown from a JTAPI method without being declared in its
 * signature. Note that no JTAPI methods declare PlatformException to be
 * thrown. This is a change from v1.1, but does not affect applications.
 * <p>
 * Since PlatformException typically denotes some form of unrecoverable
 * platform-dependent error, invoking the method again typically does not
 * yield success. These types of exceptions are often best dealt with at a
 * higher level, in a top-level "try-catch" block where the entire application
 * could be restarted.
 * @version %G% %I%
 */

public class PlatformException extends RuntimeException {

  /**
   * Constructor with no string.
   */
  public PlatformException() {
    super();
  }


  /**
   * Constructor which takes a string description.
   *
   * @param s description of the fault.
   */
  public PlatformException(String s) {
    super(s);
  }
}
