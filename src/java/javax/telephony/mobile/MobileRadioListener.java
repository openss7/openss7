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
import javax.telephony.*;

/**
 * The <CODE>MobileRadioListener</CODE> interface reports all changes which happen to
 * the MobileRadio object. These changes are reported as separate method calls. 
 * Applications must instantiate an object which implements this 
 * interface and then add the Listener to the network to receive all future events 
 * associated with the MobileRadio.
 * <p>
 * @version 02/26/99 1.5
 */

public interface MobileRadioListener  {
	
	/**
	 * The radio signal level has been changed or
	 * the network connection has been established. The signal
	 * level must be always notified after the network contact
	 * is successfully set up. After that, only the level 
	 * changes should be notified to the listeners.
	 * @param event the event object associated with this event.
	 */
	public void signalLevelChanged(MobileRadioEvent event);
	
	/**
	 * The radio startup state was changed to on.
	 * @param event the event object associated with this event.
	 */
	public void radioStartStateOn(MobileRadioEvent event);

	/**
	 * The radio startup state was changed to off.
	 * @param event the event object associated with this event.
	 */
	public void radioStartStateOff(MobileRadioEvent event);
	
	/**
	 * The radio was turned on.
	 * @param event the event object associated with this event.
	 */
	public void radioOn(MobileRadioEvent event);

	/**
	 * The radio was turned off.
	 * @param event the event object associated with this event.
	 */
	public void radioOff(MobileRadioEvent event);

}

