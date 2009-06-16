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

package javax.telephony.mobile;
import  javax.telephony.Event;

/**
 * The <CODE>MobileRadioEvent</CODE> class is the listener event 
 * class for all radio state change events. Each event is specified by unique 
 * identification number.
 *
 * @version 02/26/99 1.2
 */

public interface MobileRadioEvent extends javax.telephony.Event {

	/**
	 * The event id for signal level change. 
	 */
	public final static int MOBILERADIO_SIGNAL_LEVEL_CHANGED = 0x2;

	/**
	 * The event id for radio start state change to on. 
	 */
	public final static int MOBILERADIO_STARTSTATE_ON = 0x4;

	/**
	 * The event id for radio start state change to off. 
	 */
	public final static int MOBILERADIO_STARTSTATE_OFF = 0x6;

	/**
	 * The event id for radio turned on event. 
	 */
	public final static int MOBILERADIO_RADIO_ON = 0x8;

	/**
	 * The event id for radio turned off event. 
	 */
	public final static int MOBILERADIO_RADIO_OFF = 0x9;

	/**
	 * Returns the MobileRadio associated with this event.
	 * <p>
	 * @return The MobileRadio associated with this event.
	 */
	public MobileRadio getMobileRadio();


} // end of class MobileRadioEvent
