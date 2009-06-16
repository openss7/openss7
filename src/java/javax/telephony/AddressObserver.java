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
import  javax.telephony.events.AddrEv;

/**
 *
 * The <CODE>AddressObserver</CODE> interface reports all changes which happen
 * to the Address object.
 *
 * <H4>Introduction</H4>
 * These changes are reported as events to the
 * <CODE>AddressObserver.addressChangedEvent()</CODE> method. Applications
 * must instantiate an object which implements this interface and then use
 * the <CODE>Address.addObserver()</CODE>) method to register the object to
 * receive all future events associated with the Address object.
 * <p>
 * The <CODE>AddressObserver.addressChangedEvent()</CODE> method receives
 * an array of events which all must extend the <CODE>AddrEv</CODE> interface.
 * Since several changes may happen to a single JTAPI object at once, a list
 * of events is needed to convey those changes which happen at the same time.
 * Applications iterate through the array of events provided.
 *
 * <H4>Address Observation Ending</H4>
 *
 * At various times, the underlying implementation may not be able to observe
 * the Address. In these instances, the AddressObserver is sent an
 * AddrObservationEndedEv event. This indicates that the application will not
 * receive further events associated with the Address object. The observer is
 * no longer reported via the <CODE>Address.getObservers()</CODE> method.
 * <p>
 * @see javax.telephony.events.AddrEv
 * @see javax.telephony.events.AddrObservationEndedEv
 * @version %G% %I%
 * @deprecated As of JTAPI 1.4, replaced by {@link javax.telephony.AddressListener}
 */

public interface AddressObserver {

  /**
   * Reports all events associated with the Address object. This method
   * passes an array of AddrEv objects as its arguments which correspond to
   * the list of events representing the changes to the Address object.
   * <p>
   * @param eventList The list of Address events.
   */
  public void addressChangedEvent(AddrEv[] eventList);
}

