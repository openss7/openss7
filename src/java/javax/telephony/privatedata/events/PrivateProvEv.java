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

package javax.telephony.privatedata.events;
import  javax.telephony.events.ProvEv;

/**
 * The <CODE>PrivateProvEv</CODE> interface sends platform-specific event
 * information to a <CODE>ProviderObserver</CODE>. This interface extends the
 * core <CODE>ProvEv</CODE> interface. This event could be a stand-alone event
 * for private data that is not associated with any other event. This interface
 * could also be used to extend any other event for private data.
 * <p>
 * When used as a stand-alone event, the ID returned by <CODE>Ev.getID()</CODE>
 * should be the ID defined in this interface. When used to extend another
 * event to add private data to that event, the ID returned by
 * <CODE>Ev.getID()</CODE> should be the ID defined in the other event
 * interface.
 * <p>
 * @see javax.telephony.events.ProvEv
 * @see javax.telephony.ProviderObserver
 * @see javax.telephony.privatedata.PrivateData
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.privatedata.PrivateDataEvent}
 */

public interface PrivateProvEv extends ProvEv {

  /**
   * The Event ID.
   * <p>
   * @since JTAPI v1.2
   */
  public static final int ID = 602;


  /**
   * Returns platform-specific information to the application. The format of
   * the data and the action that should be taken upon receipt of the data is
   * platform-dependent.
   * <p>
   * @return The platform-specific data.
   */
  public Object getPrivateData();
}

