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
import  javax.telephony.ProviderEvent;

/**
 * The <CODE>ProviderListener</CODE> interface reports all changes which happen
 * to the Provider object.
 *<p>
 * These state changes are reported as events
 * to the <CODE>ProviderListener</CODE> method corresponding
 * to the type of event.
 *
 *<p>
 * Applications
 * must instantiate an object which implements this interface and then use
 * the <CODE>Provider.addProviderListener()</CODE>) method to register the object to
 * receive all future events associated with the Provider object.
 *<p>
 * The <CODE>ProviderListener</CODE> methods each receive one event at a time,
 * unlike the <CODE>ProviderObserver</CODE>.
 *
 * <H4>Provider State Changes</H4>
 *
 * In the core package, an event is delivered whenever the Provider changes
 * state. The event interfaces which correspond to these state changes for
 * the core package are: <CODE>providerInService</CODE>,
 * <CODE>providerOutOfService</CODE>, and <CODE>providerShutdown</CODE>.
 *
 * <H4>Provider Observation Ending</H4>
 *
 * At various times, the underlying implementation may not be able to observe
 * the Provider. In these instances, the ProviderListener is sent an
 * JProvObservationEndedEv event, via the
 * <CODE>ProviderListener.providerListenerEndedEvent()</CODE> method. This indicates
 * that the application will not
 * receive further events associated with the Provider object. This observer
 * will no longer be reported via the <CODE>Provider.getProviderListeners</CODE>
 * method.
 * <p>
 * @see javax.telephony.Provider
 * @see javax.telephony.ProviderEvent
 */
public interface ProviderListener extends java.util.EventListener {



/**
 * The <CODE>providerInService</CODE> method is called to indicate 
 * that the state of the Provider object has changed
 * to <CODE>Provider.IN_SERVICE</CODE>.
 *<p>
 * @param a ProviderEvent with event ID ProviderEvent.PROVIDER_IN_SERVICE.
 */
public void providerInService(ProviderEvent event);

/**
 * The <CODE>providerEventTransmissionEnded</CODE> method is called to indicate
 * that the application will no longer receive Provider events on the instance of the
 * <CODE>ProviderListener</CODE>.
 *<p>
 * @param a ProviderEvent with event ID ProviderEvent.PROVIDER_EVENT_TRANSMISSION_ENDED.
 */
public void providerEventTransmissionEnded(ProviderEvent event);

/**
 * The <CODE>providerOutOfService</CODE> method is called to indicate
 * that the state of
 * the Provider object has changed to <CODE>Provider.OUT_OF_SERVICE</CODE>.
 *<p>
 * @param a ProviderEvent with event ID ProviderEvent.PROVIDER_OUT_OF_SERVICE.
 */
public void providerOutOfService(ProviderEvent event);

/**
 * The <CODE>providerShutdown</CODE> method is called to indicate that the state of the
 * Provider object has changed to <CODE>Provider.SHUTDOWN</CODE>.
 *<p>
 * @param a ProviderEvent with event ID ProviderEvent.PROVIDER_SHUTDOWN.
 */
public void providerShutdown(ProviderEvent event);


}
