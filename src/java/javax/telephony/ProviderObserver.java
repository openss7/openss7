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
import  javax.telephony.events.ProvEv;

/**
 * The <CODE>ProviderObserver</CODE> interface reports all changes which happen
 * to the Provider object.
 *
 * <H4>Introduction</H4>
 *
 * These changes are reported as events to the
 * <CODE>ProviderObserver.providerChangedEvent()</CODE> method. Applications
 * must instantiate an object which implements this interface and then use
 * the <CODE>Provider.addObserver()</CODE>) method to register the object to
 * receive all future events associated with the Provider object.
 * <p>
 * The <CODE>ProviderObserver.providerChangedEvent()</CODE> method receives
 * an array of events which all must extend the <CODE>ProvEv</CODE> interface.
 * Since several changes may happen to a single JTAPI object at once, a list
 * of events is needed to convey those changes which happen at the same time.
 * Applications iterate through the array of events provided.
 *
 * <H4>Provider State Changes</H4>
 *
 * In the core package, an event is delivered whenever the Provider changes
 * state. The event interfaces which correspond to these state changes for
 * the core package are: <CODE>ProvInServiceEv</CODE>,
 * <CODE>ProvOutOfServiceEv</CODE>, and <CODE>ProvShutdownEv</CODE>.
 *
 * <H4>Provider Observation Ending</H4>
 *
 * At various times, the underlying implementation may not be able to observe
 * the Provider. In these instances, the ProviderObserver is sent an
 * ProvObservationEndedEv event. This indicates that the application will not
 * receive further events associated with the Provider object. This observer
 * will no longer be reported via the <CODE>Provider.getObservers()</CODE>
 * method.
 * <p>
 * @see javax.telephony.events.ProvEv
 * @see javax.telephony.events.ProvInServiceEv
 * @see javax.telephony.events.ProvOutOfServiceEv
 * @see javax.telephony.events.ProvShutdownEv
 * @see javax.telephony.events.ProvObservationEndedEv
 * @version %G% %I%
 * @deprecated As of JTAPI 1.4, replaced by {@link javax.telephony.ProviderListener}
 */

public interface ProviderObserver {

  /**
   * Reports all events associated with the Provider object. This method
   * passes an array of ProvEv objects as its arguments which correspond to
   * the list of events representing the changes to the Provider object.
   * <p>
   * @param eventList The list of Provider events.
   */
  public void providerChangedEvent(ProvEv[] eventList);
}

