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
    This exception indicates that the JccProvider is currently not available to the application.
    This exception is typically thrown in two cases: when {@link JccPeer#getProvider(String)} is called or 
    on any method when the JccProvider is in a {@link JccProvider#SHUTDOWN} state. 
    <p>
    The exception stores the reason for the failure which may be obtained via the {@link #getReason()}
    method on this interface. 
    @since 1.0b
*/
public class ProviderUnavailableException extends RuntimeException {

  /**
   * Constant definition for an unknown cause.
   */
  public static final int CAUSE_UNKNOWN = 0xA0;


  /**
   * Constant definition for the JccProvider not in the {@link JccProvider#IN_SERVICE} state.
   */
  public static final int CAUSE_NOT_IN_SERVICE = 0xA1;


  /**
   * Constant definition for an invalid service string given to
   * {@link JccPeer#getProvider(String)}.
   */
  public static final int CAUSE_INVALID_SERVICE = 0xA2;


  /**
   * Constant definition for an invalid optional argument given to
   * {@link JccPeer#getProvider(String)}.
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
    this(ProviderUnavailableException.CAUSE_UNKNOWN);
  }


  /**
   * Constructor which takes a cause string.
   *
   * @param cause reason code for this fault
   */
  public ProviderUnavailableException(int cause) {
  	super();
    _cause = cause;
  }


  /**
   * Constructor which takes a string description.
   * 
   * @param s description of the fault
   */
  public ProviderUnavailableException(String s) {
    this(ProviderUnavailableException.CAUSE_UNKNOWN,s);
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
   * Returns the cause for this exception.  This methods was originally called getCause().  However, this signature conflicted with J2SDKv1.4.0 Beta 3.
   * <p>
   * @return The cause of this exception.
   */
  public int getReason() {
    return _cause;
  }
}
