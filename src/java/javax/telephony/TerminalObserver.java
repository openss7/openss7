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
import  javax.telephony.events.TermEv;

/**
 * The <CODE>TerminalObserver</CODE> interface reports all changes which happen
 * to the Terminal object. 
 *
 * <H4>Introduction</H4>
 *
 * These changes are reported as events to the
 * <CODE>TerminalObserver.terminalChangedEvent()</CODE> method. Applications
 * must instantiate an object which implements this interface and then use
 * the <CODE>Terminal.addObserver()</CODE>) method to register the object to
 * receive all future events associated with the Terminal object.
 * <p>
 * The <CODE>TerminalObserver.terminalChangedEvent()</CODE> method receives
 * an array of events which all must extend the <CODE>TermEv</CODE> interface.
 * Since several changes may happen to a single JTAPI object at once, a list
 * of events is needed to convey those changes which happen at the same time.
 * Applications iterate through the array of events provided.
 *
 * <H4>Terminal Observation Ending</H4>
 *
 * At various times, the underlying implementation may not be able to observe
 * the Terminal. In these instances, the TerminalObserver is sent an
 * TermObservationEndedEv event. This indicates that the application will not 
 * receive further events associated with the Terminal object. This observer
 * is no longer reported via the <CODE>Terminal.getObservers()</CODE> method.
 * <p>
 * @see javax.telephony.events.TermEv
 * @see javax.telephony.events.TermObservationEndedEv
 * @version %G% %I%
 * @deprecated As of JTAPI 1.4, replaced by {@link javax.telephony.TerminalListener}
 */

public interface TerminalObserver {

  /**
   * Reports all events associated with the Terminal object. This method
   * passes an array of TermEv objects as its arguments which correspond to
   * the list of events representing the changes to the Terminal object.
   * <p>
   * @param eventList The list of Terminal events.
   */
  public void terminalChangedEvent(TermEv[] eventList);
}
