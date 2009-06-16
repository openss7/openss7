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
import  javax.telephony.Provider;
import  javax.telephony.events.*;


/**
 * The <CODE>ProviderEvent</CODE> interface is the base interface for all Provider
 * related events. All events which pertain to the Provider object must
 * extend this interface. Events which extend this interface are reported via
 * the <CODE>ProviderListener</CODE> interface.
 * <p>
 * The core package defines events which are reported when the Provider changes
 * state. These events are: <CODE>PROVIDER_IN_SERVICE</CODE>,
 * <CODE>PROVIDER_OUT_OF_SERVICE</CODE>, and <CODE>PROVIDER_SHUTDOWN</CODE>. Also, the
 * core package defines the <CODE>PROVIDER_EVENT_TRANSMISSION_ENDED</CODE> event which is
 * sent when the Provider becomes unobservable.
 * <p>
 * The <CODE>ProviderEvent.getProvider</CODE> method on this interface returns the
 * Provider associated with the Provider event.
 * <p>
 * @see javax.telephony.Event
 * @see javax.telephony.ProviderListener
 * @see javax.telephony.Provider
 * @version 03/23/99 1.8
 */
public interface ProviderEvent extends Event {

  /**
   * Returns the Provider associated with this Provider event.
   * <p>
   * @return The Provider associated with this event.
   */
  public Provider getProvider();

/**
 * Event id codes returned for this interface
 */

/**
 * The <CODE>PROVIDER_IN_SERVICE</CODE> interface indicates that the state of the
 * Provider object has changed to <CODE>Provider.IN_SERVICE</CODE>.
 * <p>
 * This constant indicates a specific event passed via a <CODE>ProviderEvent</CODE>
 * event, and is reported on the <CODE>ProviderListener</CODE> interface.
 */
public static final int PROVIDER_IN_SERVICE = 111;

/**
 * The <CODE>PROVIDER_EVENT_TRANSMISSION_ENDED</CODE> event indicates that the application
 * will no longer receive Provider events on the instance of the
 * <CODE>ProviderListener</CODE>.
 * <p>
 * This constant indicates a specific event passed via a <CODE>ProviderEvent</CODE>
 * event, and is reported on the <CODE>ProviderListener</CODE> interface.
 */
public static final int PROVIDER_EVENT_TRANSMISSION_ENDED = 112;

/**
 * The <CODE>PROVIDER_OUT_OF_SERVICE</CODE> interface indicates that the state of
 * the Provider object has changed to <CODE>Provider.OUT_OF_SERVICE</CODE>.
 * <p>
 * This constant indicates a specific event passed via a <CODE>ProviderEvent</CODE>
 * event, and is reported on the <CODE>ProviderListener</CODE> interface.
 */
public static final int PROVIDER_OUT_OF_SERVICE = 113;

/**
 * The <CODE>PROVIDER_SHUTDOWN</CODE> interface indicates that the state of the
 * Provider object has changed to <CODE>Provider.SHUTDOWN</CODE>.
 * <p>
 * This constant indicates a specific event passed via a <CODE>ProviderEvent</CODE>
 * event, and is reported on the <CODE>ProviderListener</CODE> interface.
 */
public static final int PROVIDER_SHUTDOWN = 114;

}


