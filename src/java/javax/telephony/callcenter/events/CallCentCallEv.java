/*
#pragma ident "@(#)CallCentCallEv.java	1.11      99/02/09     SMI"

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

package javax.telephony.callcenter.events;
import  javax.telephony.*;
import  javax.telephony.events.CallEv;
import  javax.telephony.callcenter.CallCenterTrunk;

/**
 * The <CODE>CallCentCallEv</CODE> interface is the base event interface for
 * all call center package Call-related events. Each Call-related event defined
 * in this package must extend this interface. This interface extends both the
 * core <CODE>CallEv</CODE> and the <CODE>CallCentEv</CODE> interfaces. All
 * events which extend this interface are reported via the
 * <CODE>CallCenterCallObserver</CODE> interface.
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
 * The call center package defines the following interfaces which extend this
 * interface: <CODE>CallCentConnEv</CODE>, <CODE>CallCentAppDataEv</CODE>, and
 * <CODE>CallCentTrunkEv</CODE>
 * <p>
 * @see javax.telephony.Call
 * @see javax.telephony.Address
 * @see javax.telephony.Terminal
 * @see javax.telephony.events.CallEv
 * @see javax.telephony.callcenter.CallCenterCallObserver
 * @see javax.telephony.callcenter.CallCenterCall
 * @see javax.telephony.callcenter.CallCenterTrunk
 * @see javax.telephony.callcenter.events.CallCentEv
 * @see javax.telephony.callcenter.events.CallCentConnEv
 * @see javax.telephony.callcenter.events.CallCentTrunkEv
 * @see javax.telephony.callcenter.events.CallCentCallAppDataEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.CallCenterCallEvent}
 */
public interface CallCentCallEv extends CallCentEv, CallEv {

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


  /**
   * Returns an array of all Trunks currently being used for this Call.
   * If there are no Trunks being used, this method returns null.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2 and
   * later with the <CODE>CallCenterCall.getTrunks()</CODE> method.
   * <p>
   * @deprecated JTAPI v1.2
   * @return An array of Trunks, null if there are none.
   */
  public CallCenterTrunk[] getTrunks();
}

