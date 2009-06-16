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
 * The <CODE>ProviderUnavailableException</CODE> indicates that the Provider is currently
 * not available to the application. This exception extends Java's
 * RuntimeException, and therefore can be thrown on any JTAPI method. It is
 * typically thrown in two situations: when
 * <CODE>JtapiPeer.getProvider()</CODE> is called or on any method when the
 * Provider is in a <CODE>Provider.SHUTDOWN</CODE> state. Because this method
 * extends <CODE>RuntimeException</CODE>, it can be thrown from any method
 * without being declared.
 * <p>
 * This exception is thrown on <CODE>JtapiPeer.getProvider()</CODE> when the
 * requested Provider is not available to the application for a number of
 * reasons, including when an invalid service string or optional argument was
 * given. If this exception is thrown on a random JTAPI method, it indicates
 * that the method call is invalid because the Provider is not in the "in
 * service" state.
 * <p>
 * This exception stores the reason for the failure which may be obtained via
 * the <CODE>ProviderUnavailableException.getReason()</CODE> method.
 * <p>
 * <h4>
 * <b>Note:</b> the Jtapi-1.4 ProviderUnavailableException is
 * <u>incompatible</u> with previous releases!
 * </h4>
 * In all releases before the Jtapi-1.4 Final-Final release,
 * <tt>getReason()</tt> was named <tt>getCause()</tt>.
 * <p>
 * In J2SE-1.4 the Throwable class from
 * (which ProviderUnavailableException extends)
 * defines <u><tt><b>Throwable</b> getCause()</tt></u>. Therefore,
 * it is not possible to define <u><tt><b>int</b> getCause()</tt></u>
 * in this class.
 * <p>
 * JTAPI applications that use <u><tt>int getCause()</tt></u>
 * can be compiled with JDK-1.3, and continue to run on JDK-1.4;
 * but to compile on JDK-1.4, the application must be re-coded to 
 * use JTAPI-1.4.
 * 
 * @version 03/23/99 1.11
 */

public class ProviderUnavailableException extends RuntimeException {

  /**
   * Constant definition for an unknown cause.
   */
  public static final int CAUSE_UNKNOWN = 0xA0;


  /**
   * Constant definition for the Provider not in the "in service" state.
   */
  public static final int CAUSE_NOT_IN_SERVICE = 0xA1;


  /**
   * Constant definition for an invalid service string given to
   * <CODE>JtapiPeer.getProvider()</CODE>.
   */
  public static final int CAUSE_INVALID_SERVICE = 0xA2;


  /**
   * Constant definition for an invalid optional argument given to
   * <CODE>JtapiPeer.getProvider()</CODE>.
   */
  public static final int CAUSE_INVALID_ARGUMENT = 0xA3;


  /**
   * This private variable holds the cause for this exception.
   * @serial
   */
  private int _cause;


  /**
   * Constructor with no cause and string.
   */
  public ProviderUnavailableException() {
    super();

    _cause = ProviderUnavailableException.CAUSE_UNKNOWN;
  }


  /**
   * Constructor which takes a cause string.
   *
   * @param cause reason code for this fault
   */
  public ProviderUnavailableException(int cause) {
    _cause = cause;
  }


  /**
   * Constructor which takes a string description.
   * 
   * @param s description of the fault
   */
  public ProviderUnavailableException(String s) {
    super(s);

    _cause = ProviderUnavailableException.CAUSE_UNKNOWN;
  }


  /**
   * Constructor which takes both a string and a cause.
   *
   * @param cause reason code for the fault
   * @param s description of the fault
   */
  public ProviderUnavailableException(int cause, String s) {
    super(s);

    _cause = cause;
  }


  /**
   * Returns the cause for this exception.
   * <p>
   * <b>Note:</b> this method was named <tt>getCause()</tt> in previous releases.
   * It has been renamed to avoid interference with the JDK-1.4
   * <tt>Throwable.getCause()</tt>
   * <p>
   * @return The cause of this exception.
   */
  public int getReason() {
    return _cause;
  }
}
