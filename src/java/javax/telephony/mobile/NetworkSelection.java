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
 * The <CODE>NetworkSelection</CODE> interface provides the specific list based services.
 * <p>
 * Users may choose between manual and automatic network selection and
 * may interact with lists of preferred and forbidden network lists.
 * (See GSM 03.22 Public Land Mobile Network)
 * <p>
 * @see javax.telephony.mobile.MobileProvider
 */

public interface NetworkSelection extends MobileProvider {

 
    /**
     * Returns the current network selection mode. The mode must be 
     * null if the current mode is manual i.e., the network was 
     * selected by using the setNetwork-method.
     * <p>
     * @return The current selection mode String object.
     * @exception MethodNotSupportedException Indicates network selection
     * is not supported.
     */
    public String getCurrentSelectionMode()
        throws MethodNotSupportedException;


    /**
     * Returns the (automatic) network selection algorithm modes available.
     * If no other selection modes than the manual selection is available,
     * than this must return null. The modes are cellular network 
     * system and implementation specific. For example, in GSM-system 
     * this might return only one string: "AUTOMATIC" and in CDMA-systems, 
     * the modes could be "PREFERRED", "ALTERNATE" and "HOME" or similar. 
     * In other words, this is totally up to the implementation.
     * <p>
     * @return The selection modes available names as text strings.
     * @exception MethodNotSupportedException Indicates network selection
     * is not supported.
     */
    public String[] getSelectionModes()
        throws MethodNotSupportedException;

	/**
	 * Sets the network selection mode. These modes are cellular system and 
	 * implementation specific and are usually automatic selection algorithms. 
	 * If the mode is null, then the action must be same as for setNetwork 
	 * i.e., manual.
	 * 
	 * @param mode Mode to be selected (or null for manual mode)
	 * @throws MethodNotSupportedException Indicates the setting 
	 *         of the selection mode capability is not supported.
	 * @throws InvalidArgumentException Indicates that the input 
	 *         parameter is invalid.
	 * @throws InvalidStateException Indicates the implementation 
	 *         is in a state in which setting the network selection mode is not possible 
	 *         (call ongoing, for example).
	 */
    public void setSelectionMode(String mode)
        throws InvalidArgumentException,MethodNotSupportedException,
		   InvalidStateException;

    /**
     * Returns the network list specified. The information in network
     * lists might change at any time.
     * <p>
	* Preferred Networks are network service carriers the user has found to
	* offer service, and is preferred over other networks.
	* <p>
     * @return A list of networks.
     * @exception MethodNotSupportedException Indicates the capability 
     * to get network lists is not supported.
     * @exception ResourceUnavailableException Indicates that 
     * a resource inside the system in not available to 
     * complete an operation. 
     */
    public MobileNetwork[] getPreferredNetworks()
        throws MethodNotSupportedException, ResourceUnavailableException;


    /**
     * Returns the network list specified. The information in network
     * lists might change at any time.
     * <p>
	* Forbidden Networks are network service carriers to which the user's home
	* network carrier has no agreement of cross service or to which the user has
	* reason (cost) not to use this network. 
     * <p>
     * @return A list of networks.
     * @exception MethodNotSupportedException Indicates the capability 
     * to get network lists is not supported.
     * @exception ResourceUnavailableException Indicates that 
     * a resource inside the system in not available to 
     * complete an operation. 
     */
    public MobileNetwork[] getForbiddenNetworks()
        throws MethodNotSupportedException, ResourceUnavailableException;

}

