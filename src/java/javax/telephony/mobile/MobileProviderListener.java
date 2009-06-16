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
 * The <CODE>MobileProviderListener</CODE> interface reports all changes
 * which happen to the MobileProvider object. These changes are reported
 * as one method call with separate event ids for each event type. The
 * applications must instantiate an object which implements this
 * interface and then use the <CODE>Provider.addProviderListener</CODE>
 * method to register the object to receive all future events associated
 * with the MobileProvider object.
 * <p>
 * @see javax.telephony.mobile.MobileProvider
 * @see javax.telephony.mobile.MobileProviderEvent
 */

public interface MobileProviderListener  extends javax.telephony.ProviderListener {

   /**
	* The state of MobileProvider has changed to restricted use mode. 
	*
	* The core ProviderEvent supplies inService, outOfService and 
	* serviceShutdown transitions. The MobileProvider adds the 
	* notification of serviceRestricted transitions.
	*
	* @param event service restricted event.
	*/
	public void serviceRestricted(MobileProviderEvent event);

   /**
	* The network of MobileProvider has changed. 
	*
	* @param event the network has changed due to roaming operation
	*/

	public void networkChanged(MobileProviderEvent event);
}

