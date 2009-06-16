/*
#pragma ident "%W%      %E%     SMI"

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

package javax.telephony.callcontrol.capabilities;
import  javax.telephony.*;
import  javax.telephony.capabilities.CallCapabilities;

/**
 * The <CODE>CallControlCallCapabilities</CODE> interface extends the core
 * <CODE>CallCapabilities</CODE> interface. This interface provides methods to
 * reflect the capabilities of the methods on the <CODE>CallControlCall</CODE>
 * interface.
 * <p>
 * The <CODE>Provider.getCallCapabilities()</CODE> method returns the static
 * Call capabilities, and the <CODE>Call.getCapabilities()</CODE> method
 * returns the dynamic Call capabilities. The object returned from each of
 * these methods can be queried with the <CODE>instanceof</CODE> operator to
 * check if it supports this interface. This same interface is used to reflect
 * both static and dynamic Call capabilities.
 * <p>
 * @see javax.telephony.Provider
 * @see javax.telephony.Call
 * @see javax.telephony.capabilities.CallCapabilities
 */

public interface CallControlCallCapabilities extends CallCapabilities {

  /**
   * Returns true if the application can invoke the drop feature, false
   * otherwise.
   * <p>
   * @return True if the application can invoke the drop feature, false
   * otherwise.
   */
  public boolean canDrop();


 /**
   * Returns true if the application can invoke the off hook feature, false
   * otherwise.
   * <p>
   * @return true if the application can invoke the off hook feature, false
   * otherwise.
   */
  public boolean canOffHook();


 /**
   * Returns true if the application can set the conference controller, false
   * otherwise.
   * <p>
   * @return true if the application can set the conference controller, false
   * otherwise.
   */
  public boolean canSetConferenceController();

 
 /**
   * Returns true if the application can set the transfer controller, false
   * otherwise.
   * <p>
   * @return true if the application can set the transfer controller, false
   * otherwise.
   */
  public boolean canSetTransferController();


  /**
   * Returns true if the application can invoke the set transferring enabling
   * feature, false otherwise. The value returned by this method is independent
   * of the ability of the application to invoke the transfer feature.
   * <p>
   * Applications are not required to inform the implementation of the purpose
   * of the consultation call and may rely upon the default value returned by
   * the <CODE>CallControlCall.getTransferEnable()</CODE> method.
   * <p>
   * @return True if the application can invoke the set transferring enabling
   * feature, false otherwise.
   */
  public boolean canSetTransferEnable();


  /**
   * Returns true if the application can invoke the set conferencing enabling
   * feature, false otherwise. The value returned by this method is independent
   * of the ability of the application to invoke the conference feature.
   * <p>
   * Applications are not required to inform the implementation of the purpose
   * of the consultation call and may rely upon the default value returned by
   * the <CODE>CallControlCall.getConferenceEnable()</CODE> method.
   * <p>
   * @return True if the application can invoke the set conferencing enabling
   * feature, false otherwise.
   */
  public boolean canSetConferenceEnable();


  /**
   * Returns true if the application can invoke the transfer feature, false
   * otherwise.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI 1.2 with
   * overloaded versions. These versions permit applications to give typed
   * argument to obtain the capabilities for a particular overloaded version
   * of the <CODE>CallControlCall.transfer()</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. The default behavior of this method in
   * JTAPI v1.2 and later should invoke the canTransfer(Call) method.
   * @return True if the application can invoke the transfer feature, false
   * otherwise.
   */
  public boolean canTransfer();


  /**
   * Returns true if the application can invoke the overloaded transfer feature
   * which takes a Call as an argument, false otherwise.
   * <p>
   * The argument provided is for typing purposes only. The particular
   * instance of the object given is ignored and not used to determine the
   * capability outcome is any way.
   * <p>
   * @since JTAPI v1.2
   * @param call This argument is used for typing information to determine
   * the overloaded version of the transfer() method.
   * @return True if the application can invoke the transfer feature which
   * takes a Call as an argument, false otherwise.
   */
  public boolean canTransfer(Call call);


  /**
   * Returns true if the application can invoke the overloaded transfer feature
   * which takes a destination string as an argument, false otherwise.
   * <p>
   * The argument provided is for typing purposes only. The particular
   * instance of the object given is ignored and not used to determine the
   * capability outcome is any way.
   * <p>
   * @since JTAPI v1.2
   * @param destination This argument is used for typing information to
   * determine the overloaded version of the transfer() method.
   * @return True if the application can invoke the transfer feature which
   * takes a destination string as an argument, false otherwise.
   */
  public boolean canTransfer(String destination);


  /**
   * Returns true if the application can invoke the conference feature, false
   * otherwise.
   * <p>
   * @return True if the application can invoke the conference feature, false
   * otherwise.
   */
  public boolean canConference();


  /**
   * Returns true if the application can invoke the add party feature, false
   * otherwise.
   * <p>
   * @return True if the application can invoke the add party feature, false
   * otherwise.
   */
  public boolean canAddParty();


  /**
   * Returns true if the application can invoke the consult feature, false
   * otherwise.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI 1.2 with
   * overloaded versions. These versions permit applications to give typed
   * argument to obtain the capabilities for a particular overloaded version
   * of the <CODE>CallControlCall.consult()</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. The default behavior of this method in
   * JTAPI v1.2 and later should invoke the canConsult(TerminalConnection,
   * String) method.
   * @return True if the application can invoke the consult feature, false
   * otherwise.
   */
  public boolean canConsult();


  /**
   * Returns true if the application can invoke the overloaded consult feature
   * which takes a TerminalConnection and string as arguments, false otherwise.
   * <p>
   * The arguments provided are for typing purposes only. The particular
   * instances of the objects given are ignored and not used to determine the
   * capability outcome is any way.
   * <p>
   * @since JTAPI v1.2
   * @param tc This argument is used for typing information to determine
   * the overloaded version of the consult() method.
   * @param destination This argument is used for typing information to
   * destination the overloaded version of the consult() method.
   * @return True if the application can invoke the consult feature which
   * takes a TerminalConnection and a string as arguments.
   */
  public boolean canConsult(TerminalConnection tc, String destination);


  /**
   * Returns true if the application can invoke the overloaded consult feature
   * which takes a TerminalConnection as an argument, false otherwise.
   * <p>
   * The arguments provided are for typing purposes only. The particular
   * instances of the objects given are ignored and not used to determine the
   * capability outcome is any way.
   * <p>
   * @since JTAPI v1.2
   * @param tc This argument is used for typing information to determine
   * the overloaded version of the consult() method.
   * @return True if the application can invoke the consult feature which
   * takes a TerminalConnection as an argument.
   */
  public boolean canConsult(TerminalConnection tc);
}
