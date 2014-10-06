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
import  javax.telephony.callcontrol.CallControlForwarding;

/**
 * The <CODE>CallCtlAddrForwardEv</CODE> interface indicates the state of the
 * forward feature has changed for the Address. This interface extends the
 * <CODE>CallCtlAddrEv</CODE> interface and is reported via the
 * <CODE>AddressObserver.addressChangedEvent()</CODE> method. The observer
 * object must also implement the <CODE>CallControlAddressObserver</CODE>
 * interface to signal it is interested in call control package events.
 * <p>
 * This interface supports a single method which returns the current state of
 * the forwarding feature.
 * <p>
 * @see javax.telephony.Address
 * @see javax.telephony.AddressObserver
 * @see javax.telephony.callcontrol.CallControlAddress
 * @see javax.telephony.callcontrol.CallControlAddressObserver
 * @see javax.telephony.callcontrol.events.CallCtlAddrEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcontrol.CallControlAddressEvent}
 */

public interface CallCtlAddrForwardEv extends CallCtlAddrEv {

  /**
   * Event id
   */
  public static final int ID = 201;


  /**
   * Returns the current forwarding on the Address.
   * <p>
   * @return An array of CallControlForwarding objects.
   */
  public CallControlForwarding[] getForwarding();
}

