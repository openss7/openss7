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

package javax.telephony.phone.events;
import  javax.telephony.events.Ev;

/**
 * The PhoneEv is the base event for all events in the Phone
 * package. Each event in this package must extend this interface. This
 * interface is not meant to be a public interface, it is just a building
 * block for other event interfaces.
 * <p>
 * The PhoneEv interface contains getPhoneCause(), which returns
 * the reason for the event.
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.phone.PhoneTerminalEvent}
 */
public interface PhoneEv extends Ev {

  /**
   * Cause code indicating normal operation
   */
  public static final int CAUSE_NORMAL = 500;

  /**
   * Cause code indicating the cause was unknown
   */
  public static final int CAUSE_UNKNOWN = 501;

  /**
   * Returns the phone and core causes associated with this event. 
   * Every event has a cause. The various cause values are defined as public
   * static final variablies in this interface, with the exception of
   * CAUSE_NORMAL and CAUSE_UNKNOWN, which are defined in the core.
   * <p>
   * @return The cause of the event.
   */
  public int getPhoneCause();

}
