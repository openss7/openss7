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
import  javax.telephony.Event;

/**
 * The <CODE>MetaEvent</CODE> interface is the base interface for
 * all JTAPI Meta events.
 * All Meta events must extend this interface.
 * <p>
 * An individual <CODE>MetaEvent</CODE>
 * conveys, directly and with necessary details,
 * what an application needs to know to respond to a higher-level JTAPI event.
 * <p>
 * Currently, meta events are defined in pairs, and they convey three
 * types of information:
 * first, the type of higher-level operation which has occurred;
 * second, the beginning and end of the sequence of "normal" JTAPI events
 * which were generated to convey the consequential JTAPI model state changes
 * that occurred because of this higher-level operation;
 * third, which JTAPI entities were involved in the higher-level operation.
 * <p>
 * A JTAPI implementation is alerted to changes in the state of the associated telephony platform,
 * and reflects that state by sending a stream of JTAPI objects, delimited by MetaEvents.
 * An application learns of the details of that state change by processing all the events
 * between the starting and ending MetaEvents.
 * <p>
 * Generally the application may draw incorrect conclusions about the current state of
 * the associated telephony platform if it decides to act before processing
 * all the events delimited by the MetaEvents.
 * Specifically, an application which wishes to submit queries to the JTAPI implementation
 * about the current state of JTAPI entities should not submit the query until it has
 * processed the matching "ending" MetaEvent.
 * <p>
 * The specific Meta event is indicated by
 * the <CODE>Event.getID()</CODE>
 * value returned by the event.
 * The specific Meta event provides context information about the higher-level event.
 * <p>
 * The <CODE>Event.getMetaEvent</CODE> method returns the Meta event
 * associated with a "normal" event (or returns null).
 * <p>
 * @version 04/06/99 1.5
 */
public interface MetaEvent extends Event {

}

