/*
 * EventObject.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
 * ECTF S.410-R2 Source code distribution.
 *
 * Copyright (c) 2002, Enterprise Computer Telephony Forum (ECTF),
 * All Rights Reserved.
 *
 * Use and redistribution of this file is subject to a License.
 * For terms and conditions see: javax/telephony/media/LICENSE.HTML
 * 
 * In short, you can use this source code if you keep and display
 * the ECTF Copyright and the License conditions. The code is supplied
 * "AS IS" and ECTF disclaims all warranties and liability.
 */

package javax.telephony.media.provider;

import java.util.*;			// for diagnostics
import java.lang.ref.WeakReference; 	// for diagnostics

/** 
 * Specialize java.util.EventObject to allow interposition
 * on all EventObjects.  Normally this class does nothing,
 * but by replacing this class, one can insert interesting 
 * debug behavior or information gathering (without modifying
 * the more complex Base.java).
 */
class EventObject extends java.util.EventObject {
    public EventObject(Object source) {
	super(source); 
	/*
	    synchronized (allEvents) { 		// 
		wr = new WeakReference(this); 	// 
		allEvents.add(wr); 		// 
	    } 					// 
    } 						// 
    public Date timestamp = new Date();		//
    public WeakReference wr; 			// 
    public static HashSet allEvents = new HashSet(); // 
    public void finalize() throws Throwable { 	// 
	try {					// 
	    source = null;			// 
	    synchronized (allEvents) { 		// 
		allEvents.remove(wr); 		// 
		wr = null;			// 
	    } 					// 
	    super.finalize();			//
	} catch (Throwable throwable) {		// 
	    System.out.println("EventObject.finalize(): "+throwable);
	}
	*/
    }	
}
