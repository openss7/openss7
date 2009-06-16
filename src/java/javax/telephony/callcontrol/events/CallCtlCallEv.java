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

package javax.telephony.callcontrol.events;
import	javax.telephony.events.CallEv;
import  javax.telephony.*;

/**
 * The <CODE>CallCtlCallEv</CODE> interface is the base interface for all call
 * control package Call-related events. All events which pertain to the
 * <CODE>CallControlCall</CODE> interface must extend this interface. Events
 * which extend this interface are reported via the
 * <CODE>CallObserver.callChangedEvent()</CODE> method. The observer object
 * must also implement the <CODE>CallControlCallObserver</CODE> interface to
 * signal it is interested in call control package events. This interface
 * extend both the <CODE>CallCtlEv</CODE> and core <CODE>CallEv</CODE>
 * interfaces.
 * <p>
 * The <CODE>CallCtlConnEv</CODE> and <CODE>CallCtlTermConnEv</CODE> events
 * extend this interface. This reflects the fact that all events pertaining to
 * the <CODE>CallControlConnection</CODE> interface and the
 * <CODE>CallControlTerminalConnection</CODE> interface are reported via the
 * <CODE>CallControlCallObserver</CODE> interface.
 *
 * <H4>Additional Call Information</H4>
 *
 * This interface supports methods which return additional information
 * regarding the telephone call. Specifically, it returns the <EM>calling
 * address</EM>, <EM>calling terminal</EM>, <EM>called address</EM>, and
 * <EM>last redirected address</EM> information. This information is returned
 * by the <CODE>getCallingAddress()</CODE>, <CODE>getCallingTerminal()</CODE>,
 * <CODE>getCalledAddress()</CODE>, and <CODE>getLastRedirectedAddress()</CODE>
 * methods on this interface, respectively.
 * <p>
 * @see javax.telephony.Call
 * @see javax.telephony.Address
 * @see javax.telephony.Terminal
 * @see javax.telephony.CallObserver
 * @see javax.telephony.events.CallEv
 * @see javax.telephony.callcontrol.CallControlCall
 * @see javax.telephony.callcontrol.CallControlCallObserver
 * @see javax.telephony.callcontrol.events.CallCtlEv
 * @see javax.telephony.callcontrol.events.CallCtlConnEv
 * @see javax.telephony.callcontrol.events.CallCtlTermConnEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcontrol.CallControlCallEvent}
 */

public interface CallCtlCallEv extends CallCtlEv, CallEv {

  /**
   * Returns the calling Address associated with this call. The calling
   * Address is defined as the Address which placed the telephone call.
   * <p>
   * If the calling address is unknown or not yet known, this method returns
   * null.
   * <p>
   * @return The calling Address.
   */
  public Address getCallingAddress();


  /**
   * Returns the calling Terminal associated with this Call. The calling
   * Terminal is defined as the Terminal which placed the telephone call.
   * <p>
   * If the calling Terminal is unknown or not yet know, this method returns
   * null.
   * <p>
   * @return The calling Terminal.
   */
  public Terminal getCallingTerminal();


  /**
   * Returns the called Address associated with this Call. The called
   * Address is defined as the Address to which the call has been originally
   * placed.
   * <p>
   * If the called address is unknown or not yet known, this method returns
   * null.
   * <p>
   * @return The called Address.
   */
  public Address getCalledAddress();


  /**
   * Returns the last redirected Address associated with this Call.
   * The last redirected Address is the Address at which the current telephone
   * call was placed immediately before the current Address. This is common
   * if a Call is forwarded to several Addresses before being answered.
   * <p>
   * If the the last redirected address is unknown or not yet known, this
   * method returns null.
   * <p>
   * @return The last redirected Address for this telephone Call.
   */
  public Address getLastRedirectedAddress();
}
