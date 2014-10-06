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

package javax.telephony.events;
import  javax.telephony.Address;

/**
 * The <CODE>AddrEv</CODE> interface is the base interface for all Address-
 * related events. All events which pertain to the Address object must extend
 * this interface. Events which extend this interface are reported via the
 * <CODE>AddressObserver</CODE> interface.
 * <p>
 * The only event defined in the core package for the Address is the
 * <CODE>AddrObservationEndedEv</CODE>.
 * <p>
 * The <CODE>AddrEv.getAddress()</CODE> method on this interface returns the
 * Address associated with the Address event.
 * <p>
 * @see javax.telephony.events.AddrObservationEndedEv
 * @see javax.telephony.events.Ev
 * @see javax.telephony.AddressObserver
 * @see javax.telephony.Address
 */

public interface AddrEv extends Ev {

  /**
   * Returns the Address associated with this Address event.
   * <p>
   * @return The Address associated with this event.
   */
  public Address getAddress();
}
