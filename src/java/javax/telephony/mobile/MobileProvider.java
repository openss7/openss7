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
import  java.util.Date;
import  java.util.TimeZone;

/**
 *
 * A <CODE>MobileProvider</CODE> represents the telephony software-entity that interfaces
 * with a wireless subsystem. It is an extension of the core provider
 * with various additions needed for wireless communication. 
 *
 * <H4>MobileProvider States</H4>
 *
 * This interface defines states for the Provider which provide greater
 * detail beyond the states defined in the <CODE>Provider</CODE> interface.
 * The states defined by this interface are related to the states defined in
 * the core package in certain, specific ways, as defined below. Applications
 * may obtain the <CODE>MobileProvider</CODE> state via the
 * <CODE>Provider.getState()</CODE> method defined on the core. This
 * method returns one of the integer state constants defined in this interface.
 * <p>
 * Below is a description of each <CODE>MobileProvider</CODE> state in
 * real-world terms. These real-world descriptions only serve to provide a
 * more intuitive understanding of what is going on. Several methods in this
 * specification state pre-conditions based upon the
 * <CODE>MobileProvider</CODE> state. Most of the states are identical
 * to those defined in the core package.
 * <p>
 * <TABLE>

 * <TR>
 * <TD WIDTH="15%"><CODE>MobileProvider.RESTRICTED_SERVICE</CODE></TD>
 * <TD WIDTH="85%">
 * This state indicates that the subscription is invalid (invalid 
 * subscription, forbidden network...): from the network point of view, only emergency calls are 
 * allowed. Local operations (such as Personal Information Management) are 
 * available.
 * </TD>
 * </TR>
 * </TABLE>
 * <p>
 * <H4>Core vs. Mobile Package States</H4>
 *
 * There is a strong relationship between the
 * <CODE>MobileProvider</CODE> states and the <CODE>Provider</CODE>
 * states. If an implementation supports the mobile package, it must
 * ensure this relationship is properly maintained.
 * <p>
 * Since the states defined in the <CODE>MobileProvider</CODE> interface
 * provide more detail to the states defined in the <CODE>Provider</CODE>
 * interface, each state in the <CODE>Provider</CODE> interface corresponds
 * to a state defined in the <CODE>MobileProvider</CODE> interface.
 * Conversely, each <CODE>MobileProvider</CODE> state corresponds to
 * exactly one <CODE>Provider</CODE> state. This arrangement permits
 * applications to view either the core state or the mobile state and
 * still see a consistent view.
* <p>
 * The following table outlines the relationship between the core package
 * Provider states and the mobile package Provider states.
 * <p>
 * <TABLE CELLPADDING=2>
 * <TR>
 * <TD WIDTH="60%">
 * <STRONG><EM>If the mobile package state is...</EM></STRONG>
 * </TD>
 * <TD WIDTH="40%">
 * <STRONG><EM>then the core package state must be...</EM></STRONG>
 * </TD>
 * </TR>
 *

 * <TR>
 * <TD WIDTH="60%"><CODE>MobileProvider.RESTRICTED_SERVICE</CODE></TD>
 * <TD WIDTH="40%"><CODE>Provider.IN_SERVICE</CODE></TD>
 * </TR>
 * </TABLE>
 *
 * <p> 
 * <H4>Listeners and Events</H4>
 *
 * Each time a state changes occurs on a MobileProvider, the application is notified
 * via an <EM>event</EM>. This event is reported via the
 * <CODE>MobileProviderListener</CODE> interface. Applications instantiate objects
 * which implement this interface and use the
 * <CODE>Provider.addProviderListener(MobileProviderListener)</CODE> method to begin 
 * the delivery of events.
 *
 * @see javax.telephony.mobile.MobileProviderEvent
 * @see javax.telephony.mobile.MobileProviderListener
 * @version 05/24/99 1.12
 */

public interface MobileProvider extends javax.telephony.Provider {

	/**
	 * The <CODE>MobileProvider.RESTRICTED_SERVICE</CODE> state indicates a 
	 * MobileProvider services are restricted because of missing user
	 * subscription or invalid/missing password such as four-digit PIN-code.
	 * In this state, only emergency calls are allowed. 
	 */
	public static final int RESTRICTED_SERVICE = 0x13;

	/**
	 * Returns the current state of the MobileProvider. 
	 * <p>
	 * @return The current state of the mobileprovider.
	 */
	public int getMobileState();





    /**
     * Scans for available networks in the area or returns the previous
     * search results, if available. The state of particular 
     * network may or may not be known. 
     * <p>
     * @param fullsearch True, if full search is initiated. This may take
     * several tens of seconds to complete. False, if previous results
     * without a full scan are asked for. 
     * @return A list of networks in the area or previous search results.
     * @exception MethodNotSupportedException Indicates that the scanning 
     * for available networks is not supported.
     * @exception InvalidStateException Indicates the implementation is
     * in a such state which network search is not possible (call ongoing,
     * for example.)
     * @exception ResourceUnavailableException indicates that a resource
     * inside the system is not available to complete an operation. This
     * may often be the case when the application wants the 
     * previous results. 
     * @see javax.telephony.mobile.MobileNetwork
     */
    public MobileNetwork[] getAvailableNetworks(boolean fullsearch)
        throws MethodNotSupportedException, InvalidStateException,
			   ResourceUnavailableException;
	
   /**
     * Cancels an outstanding getAvailableNetworks-request, if possible.
     * <p>
     * @exception MethodNotSupportedException Indicates the cancelling of 
     * the radio scan capability is not supported.
     * @exception InvalidStateException Indicates the implementation is
     * in a such state in which the cancelling of an operation is not 
     * possible.
     */
    public void cancelAvailableNetworkRequest() throws MethodNotSupportedException, 
 								 InvalidStateException;

    /**
     * Returns the current network object. If no connection to
     * a network is available this must return null.
     * The current network is the one the mobile has registered to.
     * <p>
     * @return A network object of the current network.
     */
    public MobileNetwork getCurrentNetwork();

    /**
     * Get the home network object. The information capability is in some
     * cases dynamic i.e., it may or may not be available. (Information
     * may be stored in a changeable SIM-card, for example.)
     * <p>
     * @return The home network.
     * @exception MethodNotSupportedException Indicates the home network
     * information capability is not supported.
     * @exception ResourceUnavailableException Indicates that 
     * a resource inside the system is not available to 
     * complete an operation. 
     */
    public MobileNetwork getHomeNetwork()
        throws MethodNotSupportedException, ResourceUnavailableException;

	/**
	 * Selects a network as the current servicing network. If the selected network 
	 * is available, it will become the current network. <STRONG>Note: setting 
	 * the network with this method forces the network selection mode to 
	 * manual.</STRONG>
	 *
	 * @param network Network to be selected.
	 * @throws MethodNotSupportedException Indicates network 
	 *          selection is not supported.
	 * @throws InvalidStateException Indicates the implementation 
	 *         is in a state in which setting the network is not possible 
	 *         (call ongoing, for example.)
	 * @throws ResourceUnavailableException Indicates that a 
	 *         resource inside the system is not available to complete the operation.
	 */
    public void setNetwork(MobileNetwork network)
		throws MethodNotSupportedException, 
			   InvalidStateException,
			   ResourceUnavailableException;
	
    /**
     * Return the current roaming status. True, if roaming.
     * 
     * @return The current roaming status.
     * @exception MethodNotSupportedException Indicates this method is
     * not supported by the implementation.
     * @exception InvalidStateException Indicates the <CODE>MobileProvider</CODE>
     * or <CODE>MobileRadio</CODE> is in a state in which the roaming
     * state is not available.
     */ 
	public boolean isRoaming()         
		throws MethodNotSupportedException,InvalidStateException;
	
    /**
     * Returns the type of network/wireless service. Totally up 
     * the implementation.
     * Example: "GSM", "PCS1900", "CDMA" ...
     * <p>
     * @return A string representing the air interface type.
     */
    public String getType();

    /**
     * Get the network time. 
     * <p>
     * @return The current network time.
	 * @exception MethodNotSupportedException Indicates the home network
     * information capability is not supported.    */
    public Date getNetworkTime() throws MethodNotSupportedException;

    /**
     * Get the network time zone. 
     * <p>
     * @return The current network time zone.
	 * @exception MethodNotSupportedException Indicates the home network
     * information capability is not supported.   */
    public TimeZone getNetworkTimeZone() throws MethodNotSupportedException;
	
 
}

