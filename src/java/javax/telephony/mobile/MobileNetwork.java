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
import  javax.telephony.*;

/**
 * A <CODE>MobileNetwork</CODE> interface represents an actual cellular network.
 * 
 * The MobileNetwork object contains all available information
 * about the wireless network.
 *
 * @see javax.telephony.mobile.MobileProvider
 * @see javax.telephony.mobile.MobileProviderEvent
 * @see javax.telephony.mobile.MobileProviderListener
 * @version 03/03/99 1.6
 */

public interface MobileNetwork {
	/**
	 * Returns true if the network is currently available.
	 *
	 * @return true if mobile network is available
	 */
	public boolean isAvailable() ;

	/**
	 * Returns true if the network is current network in use.
	 * (convenience method equivalent to {@link NetworkSelection#getCurrentNetwork} )
	 *
	 * @return true if mobile network is current
	 */
	public boolean isCurrent() ;


	/**
	 * Returns true if the network is currently restricted.
	 *
	 * @return true if mobile network is restricted
	 */
	public boolean isRestricted() ;


    /**
     * Returns the all available network name synonyms. This usually
     * means one short and long name (per network). The shortest name 
     * or the only name available must be the FIRST (=[0]) in the array. 
     * The other names must be sorted by character length, also.
     * For example, [0] = "tl", [1] = "tele", [2] = "telecom"  
     * <p>
     * @return All available names of a network, 
     * null, if none available.
     */
    public String[] getNames();
	
	
    /**
     * Returns the network code in a String format.
	* <br> 
     * Example: MCC-MNC, SID-NID
	* <br> 
	* where : MCC = Mobile Country Code, MNC = Mobile Network Code
	* <br> 
	* SID = Subscriber ID, NID = Network ID
     * <p>
     * @return The network code.
     */
    public String getCode();

	


}


