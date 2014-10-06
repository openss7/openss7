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

import  javax.telephony.Address;



/**

 * The <CODE>AddressEvent</CODE> interface is the base interface for

 * all Address-related events.

 * All Address-related events must extend this interface.

 * Events which extend this interface are reported via the

 * <CODE>AddressListener</CODE> interface.

 * <p>

 * An individual <CODE>AddressEvent</CODE> conveys one of a series of different

 * possible Address state changes;

 * the specific Address state change is indicated by

 * the <CODE>Event.getID()</CODE>

 * value returned by the event.

 * <p>

 * The only event ID defined in the core package for the Address is the

 * <CODE>ADDRESS_EVENT_TRANSMISSION_ENDED</CODE> event.

 * <p>

 * This event ID is reported 

 * via a method in the <CODE>AddressListener</CODE>

 * interface (see <CODE>CallListener.CallInvalidEvent()</CODE>)

 * <p>

 * The <CODE>AddressEvent.getAddress()</CODE> method on this interface returns the

 * Address associated with the Address event.

 * <p>

 * @see javax.telephony.Event

 * @see javax.telephony.AddressListener

 * @see javax.telephony.Address

 * @version 03/23/99 1.3

 */

public interface AddressEvent extends javax.telephony.Event {



  /**

   * Returns the Address associated with this Address event.

   * <p>

   * @return The Address associated with this event.

   */

  public Address getAddress();

  



/**

 * Event id codes returned for this interface

 */



/**

 * The <CODE>ADDRESS_EVENT_TRANSMISSION_ENDED</CODE> event indicates that the application

 * will no longer receive Address events on the instance of the

 * <CODE>AddressListener</CODE>.

 * <p>

 * This constant indicates a specific event passed via a <CODE>AddressEvent</CODE>

 * event, and is reported on the <CODE>AddressListenerEnded</CODE> method 

 * of the <CODE>AddressListener</CODE> interface.

 */

public static final int ADDRESS_EVENT_TRANSMISSION_ENDED = 100;



}

