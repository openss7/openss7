/*
 * Copyright (c) 1997 Sun Microsystems, Inc. All Rights Reserved.
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
 * The <CODE>JtapiPeer</CODE> interface represents a vendor's particular
 * implementation of the Java Telephony API.
 *
 * <H4> Introduction </H4>
 *
 * Each JTAPI implementation vendor 
 * must implement this interface.  The JtapiPeer object returned by the
 * <CODE>JtapiPeerFactory.getJtapiPeer()</CODE> method determines which
 * Providers are made available to the application.
 *
 * <H4>Obtaining a Provider</H4>
 *
 * Applications use the <CODE>JtapiPeer.getProvider()</CODE> method on this
 * interface to obtain new Provider objects. Each implementation may support
 * one or more different "services" (e.g. for different types of underlying
 * network substrate). A list of available services can be obtained via the
 * <CODE>JtapiPeer.getServices()</CODE> method.
 * <p>
 * Applications may also supply optional arguments to the Provider through the
 * <CODE>JtapiPeer.getProvider()</CODE> method. These arguments are appended
 * to the <CODE>providerString</CODE> argument passed to the
 * <CODE>JtapiPeer.getProvider()</CODE> method. The <CODE>providerString</CODE>
 * argument has the following format:
 * <p>
 * &lt service name &gt ; arg1 = val1; arg2 = val2; ...
 * <p>
 * Where &lt service name &gt is not optional, and each optional argument pair
 * which follows is separated by a semi-colon. The keys for these arguments
 * is implementation specific, except for two standard-defined keys:
 * <OL>
 * <LI>login: provides the login user name to the Provider.
 * <LI>passwd: provides a password to the Provider.
 * </OL>
 * @see JtapiPeerFactory
 * @version %G% %I%
 */

public interface JtapiPeer {

  /**
   * Returns the name of this JtapiPeer object instance. This name is the same
   * name used as an argument to <EM>JtapiPeerFactory.getJtapiPeer()</EM>
   * method.
   * <p>
   * @return The name of this JtapiPeer object instance.
   */
  public String getName();


  /**
   * Returns the services that this implementation supports. This method
   * returns <CODE>null</CODE> if no services as supported.
   * <p>
   * @return The services that this implementation supports.
   */
  public String[] getServices();


  /**
   * Returns an instance of a <CODE>Provider</CODE> object given a string 
   * argument which
   * contains the desired service name. Optional arguments may also be
   * provided in this string, with the following format:
   * <p>
   * &lt service name &gt ; arg1 = val1; arg2 = val2; ...
   * <p>
   * Where &lt service name &gt is not optional, and each optional argument
   * pair which follows is separated by a semi-colon. The keys for these
   * arguments is implementation specific, except for two standard-defined
   * keys:
   * <OL>
   * <LI>login: provides the login user name to the Provider.
   * <LI>passwd: provides a password to the Provider.
   * </OL>
   * <p>
   * If the argument is null, this method returns some default Provider as
   * determined by the JtapiPeer object. The returned Provider is in the
   * <CODE>Provider.OUT_OF_SERVICE</CODE> state.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>this.getProvider().getState() == Provider.OUT_OF_SERVICE
   * </OL>
   * @param providerString The name of the desired service plus an optional
   * arguments.
   * @return An instance of the Provider object.
   * @exception ProviderUnavailableException Indicates a Provider corresponding
   * to the given string is unavailable.
   */
  public Provider getProvider(String providerString)
    throws ProviderUnavailableException;
}
