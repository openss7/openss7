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
import  javax.telephony.events.CallEv;

/**
 * The <CODE>CallObserver</CODE> interface reports all changes which happen to
 * the Call object and all of the Connection and TerminalConnection objects
 * which are part of the Call.
 * 
 * <H4>Introduction</H4>
 *
 * These changes are reported as events to the
 * <CODE>CallObserver.callChangedEvent()</CODE> method. Applications must
 * instantiate an object which implements this interface and then add the
 * observer to the call using one of several mechanisms described below to
 * receive all future events associated with the Call and its Connections and
 * TerminalConnections.
 * <p>
 * The <CODE>CallObserver.callChangedEvent()</CODE> method receives an array
 * of events which all must extend the <CODE>CallEv</CODE> interface. Since
 * several changes may happen to a single JTAPI object at once, a list of
 * events is needed to convey those changes which happen at the same time.
 * Applications iterate through the array of events provided.
 *
 * <H4>Adding an Observer to a Call</H4>
 *
 * Applications may add an observer to a Call via one of several mechanisms.
 * They may directly add an observer via the <CODE>Call.addObserver()</CODE>
 * method. Applications may also add observers to Calls indirectly via the
 * <CODE>Address.addCallObserver()</CODE> and
 * <CODE>Terminal.addCallObserver()</CODE> methods. These methods add the given
 * observer to the Call when the Call comes to the Address or Terminal. See
 * the specifications for Call, Address, and Terminal for more information.
 *
 * <H4>Call State Changes</H4>
 *
 * In the core package, an event is delivered whenever the Call changes state.
 * The event interfaces which correspond to these state changes for the core
 * package are: <CODE>CallActiveEv</CODE> and <CODE>CallInvalidEv</CODE>.
 *
 * <H4>Connection Events</H4>
 *
 * All events pertaining to the Connection object are reported on this
 * interface. Connection events extend the <CODE>ConnEv</CODE> event, which
 * in turn, extends the <CODE>CallEv</CODE> event. In the core package, an
 * event is delivered to this interface whenever the Connection changes state.
 *
 * <H4>TerminalConnection Events</H4>
 *
 * All events pertaining to the TerminalConnection object are reported on this
 * interface. TerminalConnection events extend the <CODE>TermConnEv</CODE>
 * interface, which in turn, extends the <CODE>CallEv</CODE> interface. In the
 * core package, an event is delivered to this interface whenever the
 * TerminalConnection changes state.
 *
 * <H4>Call Observation Ending</H4>
 *
 * At various times, the underlying implementation may not be able to observe
 * the Call. In these instances, the CallObserver is sent an
 * CallObservationEndedEv event. This indicates that the application will not
 * receive further events associated with the Call object. This observer is
 * no longer reported via the <CODE>Call.getObservers()</CODE> method.
 *
 * @see javax.telephony.events.CallEv
 * @see javax.telephony.events.ConnEv
 * @see javax.telephony.events.TermConnEv
 * @see javax.telephony.events.CallObservationEndedEv
 * @see javax.telephony.events.CallActiveEv
 * @see javax.telephony.events.CallInvalidEv
 * @see javax.telephony.events.ConnAlertingEv
 * @see javax.telephony.events.ConnConnectedEv
 * @see javax.telephony.events.ConnCreatedEv
 * @see javax.telephony.events.ConnDisconnectedEv
 * @see javax.telephony.events.ConnFailedEv
 * @see javax.telephony.events.ConnInProgressEv
 * @see javax.telephony.events.ConnUnknownEv
 * @see javax.telephony.events.TermConnActiveEv
 * @see javax.telephony.events.TermConnCreatedEv
 * @see javax.telephony.events.TermConnDroppedEv
 * @see javax.telephony.events.TermConnPassiveEv
 * @see javax.telephony.events.TermConnRingingEv
 * @see javax.telephony.events.TermConnUnknownEv
 * @version %G% %I%
 * @deprecated As of JTAPI 1.4, replaced by {@link javax.telephony.CallListener}
 */

public interface CallObserver {

  /**
   * Reports all events associated with the Call object. This method passes
   * an array of CallEv objects as its arguments which correspond to the list
   * of events representing the changes to the Call object as well as changes
   * to all of the Connection and TerminalConnection objects associated with
   * this Call.
   * <p>
   * @param eventList The list of Call events.
   */
  public void callChangedEvent(CallEv[] eventList);
}
