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
import javax.telephony.callcontrol.*;

/**
 * 
 * A <CODE>MobileAddress</CODE> interface represents what we commonly think of as a "telephone
 * number." In the mobile world, the telephone number depends of the 
 * subscription and so often of the SIM card.  
 * <p>
 *
 * <H4>Address and Call Objects</H4>
 *
 * Address objects represent the <EM>logical</EM> endpoints of a telephone
 * call. A logical view of a telephone call views the call as originating
 * from one Address endpoint and terminates at another Address endpoint.
 * <p>
 *
 * <H4>Address and Events</H4>
 *
 * In the core package, the Address is described as static. In the mobile world, a
 * subscription (and so the phone number) may appear or disappear with the insertion or 
 * extraction of the support (subscription included in the SIM card), but the MobileAddress
 * object life is the same as the MobileProvider object life (as in the core). If the 
 * subscription changes, it will generate an event (through the {@link MobileProviderListener}).
 * 
 * @see javax.telephony.Address
 * @see javax.telephony.mobile.MobileProvider
 * @version 03/03/99 1.6
 */

public interface MobileAddress extends Address {
	// NYI: CallControlAddress support may be added here
	// when supplementary services are added to the mobile package.

	/**
	 * Returns the state of call 'knocking' feature. 
	 *
	 * <P>In the mobile world the user can turn call waiting indication on and
	 * off, as a do not disturb.  getCallWaiting does an inquiry to the
	 * network to get the setting on or off.</P>
	 *
	 * @return True, if call waiting is enabled.
	 * @exception MethodNotSupportedException Indicates this
	 * method is not supported by the implementation.  */
	public boolean getCallWaiting() 
		throws MethodNotSupportedException;

	/** 
	 * Sets the state of call 'knocking' feature.
	 *
	 * @param state true, if call waiting is enabled.
	 * @exception MethodNotSupportedException Indicates this
	 * method is not supported by the implementation.
	 */
	public void setCallWaiting(boolean state) 
		throws MethodNotSupportedException;

	/**
	 * Returns the Subscription Identification String associated with
	 * this address.
	 *
	 * This methods returns the subscription ID string such as IMSI.
	 * (International Mobile Subscriber Identification)
	 * 
	 * @return the subscriber ID String, NULL if no such ID is implemented.
	 */
	public String getSubscriptionId();

}


