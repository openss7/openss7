/*
 * Copyright (c) 1999 Sun Microsystems, Inc. All Rights Reserved.
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
import  javax.telephony.AddressEvent;

/**
 *
 * The <CODE>AddressListener</CODE> interface reports all changes which happen
 * to the Address object.
 *
 * <H4>Introduction</H4>
 * Such a state change is reported by a call to the method of this interface
 * which corresponds to the type of state change that occurred. An event
 * (an <CODE>AddressEvent</CODE>) which has an event ID which corresponds to the state change
 * is passed as a parameter.
 * <p>
 * To receive all future events associated with an Address object, applications
 * instantiate an object which implements this interface and then register their object as an
 * Address Listener using the <CODE>Address.addAddressListener()</CODE> method.
 *<p>
 * The <CODE>AddressListener</CODE> methods each receive one event at a time.
 *
 * <H4>Address Observation Ending</H4>
 *
 * At various times, the underlying implementation may not be able to observe
 * the Address. In these instances, the AddressListener is sent an event
 * with event ID <CODE>AddressEvent.ADDRESS_EVENT_TRANSMISSION_ENDED</CODE>.
 * This indicates that the application will not
 * receive further events associated with the Address object. The listener is
 * no longer reported via the <CODE>Address.getAddressListeners()</CODE> method.
 * <p>
 * @see javax.telephony.AddressEvent
 * @see javax.telephony.Address
 */

public interface AddressListener extends java.util.EventListener {

/**
 * The <CODE>ADDRESS_EVENT_TRANSMISSION_ENDED</CODE> event indicates that the application
 * will no longer receive Address events on the instance of the
 * <CODE>AddressListener</CODE>.
 * <p>
 * This constant indicates a specific event passed via a <CODE>AddressEvent</CODE>
 * event, and is reported on the <CODE>AddressListener</CODE> interface.
 * <p>
 * @see javax.telephony.AddressEvent
 * @see javax.telephony.Address
 */

public void addressListenerEnded(AddressEvent event);

}

