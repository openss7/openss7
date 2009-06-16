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
 * The <CODE>MobileProviderEvent</CODE> interface is the interface for
 * all mobileprovider events. Each event is specified by unique 
 * identification number, specified in the MobileProvider 
 * interface.
 * <H4>State and State Cause Mapping</H4>
 * 
 * In each state, the state cause must be set as described below:
 * <p>
 * <TABLE CELLPADDING=2>
 * <TR>
 * <TD WIDTH="40%">
 * <STRONG><EM>MobileProvider State</EM></STRONG>
 * </TD>
 * <TD WIDTH="60%">
 * <STRONG><EM>MobileProviderEvent Cause</EM></STRONG>
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="40%"><CODE>IN_SERVICE</CODE></TD>
 * <TD WIDTH="60%"><CODE>CAUSE_NORMAL</CODE></TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="40%"><CODE>RESTRICTED_SERVICE</CODE></TD>
 * <TD WIDTH="60%"><CODE>CAUSE_FORBIDDEN, CAUSE_FORBIDDEN_ZONE, 
 *   CAUSE_NETWORK_NOT_SELECTED, CAUSE_SUBSCRIPTION_ERROR
 *   or CAUSE_SEARCHING</CODE></TD>
 * </TR> 
 *
 * <TR>
 * <TD WIDTH="40%"><CODE>SHUTDOWN</CODE></TD>
 * <TD WIDTH="60%"><CODE>CAUSE_NORMAL</CODE></TD>
 * </TR> 
 *
 * <TR>
 * <TD WIDTH="40%"><CODE>OUT_OF_SERVICE</CODE></TD>
 * <TD WIDTH="60%"><CODE>CAUSE_RADIO_OFF, CAUSE_NO_NETWORK 
 *     or CAUSE_UNKNOWN</CODE></TD>
 * </TR> 
 * </TABLE>
 *
 * @see javax.telephony.mobile.MobileProvider
 * @see javax.telephony.mobile.MobileProviderEvent
 * @see javax.telephony.mobile.MobileProviderListener
 * @version 05/17/99 1.9
 */

public interface MobileProviderEvent extends ProviderEvent {

	
	/**
	 * The <CODE>MobileProvider&#46CAUSE_NORMAL</CODE> cause indicates
	 * there is no cause.  
	 */
	public static final int CAUSE_NORMAL = 0x0;
 
	/**
	 * The <CODE>MobileProvider&#46CAUSE_FORBIDDEN</CODE> cause indicates 
	 * the provider is in the restricted state because the current 
	 * network is forbidden i.e., emergency calls only. 
	 */
	public static final int CAUSE_FORBIDDEN = 0x1;

	/**
	 * The <CODE>MobileProvider&#46CAUSE_FORBIDDEN_ZONE</CODE> cause indicates 
	 * the provider is in the restricted state because the mobile phone
	 * is currently in a forbidden regional zone i.e., only emergency
	 * calls are allowed.
	 */
	public static final int CAUSE_FORBIDDEN_ZONE = 0x2;


	/**
	 * The <CODE>MobileProvider&#46CAUSE_SUSBSCRIPTION_ERROR</CODE> 
	 * cause indicates the provider is in the restricted or in out_of_service
	 * state because of a subscription related error. The SIM is either missing or 
	 * there is some other problem such as invalid entry code.
	 */
	public static final int CAUSE_SUSBCRIPTION_ERROR = 0x3;

    /**
     * The <code>MobileProvider&#46CAUSE_RADIO_OFF</code> cause
     * indicates that the radio is turned off. In other words,
     * the MobileRadio is off. 
     */
	public static final int CAUSE_RADIO_OFF = 0x4;

    /**
     * The <code>MobileProvider&#46CAUSE_NO_NETWORK</code> value
     * indicates there is no cellular networks detected in the area.
     */
	public static final int CAUSE_NO_NETWORK = 0x5;

    /**
     * The <code>MobileProvider&#46CAUSE_NETWORK_NOT_SELECTED</code> value
     * indicates that currently no network is selected but networks
     * are available in the area. 
     */
	public static final int CAUSE_NETWORK_NOT_SELECTED = 0x6;

	/**
	 * The <CODE>MobileProvider&#46CAUSE_SEARHING</CODE> indicates
	 * the implementation is searching for cellular networks.
	 * 
	 */
	public static final int CAUSE_SEARCHING = 0x7;

	/**
	 * The <CODE>MobileProvider&#46CAUSE_ILLEGAL_MOBILE</CODE> indicates
	 * there maybe a problem with mobile equipment ID information.
	 *
	 * This cause is because sometimes the subscription ID is not available and the
	 * mobile equipment ID is used to authenticate the mobile. It is through this
	 * that invalid or stolen equipment can be identified and the network will
	 * refuse normal service, but will provide restricted (emergency) service.
	 * Some systems keep a list of stolen mobiles - called the black list.
	 * Gray list is mobiles that have some software or radio problem and need to be
	 * recalled.
	 */
	public static final int CAUSE_ILLEGAL_MOBILE = 0x8 ;


    /**
     * The <code>MobileProvider&#46CAUSE_UNKNOWN</code> value
     * indicates the reason for state is not available or
     * is none of the above causes.
     */
	public static final int CAUSE_UNKNOWN = 0x99;

	/**
	 * The event id for <code>MobileProvider</code>
	 *  state change for restricted network service.
	 */
	public final static int PROVIDER_RESTRICTED_SERVICE = 0x101;

	/**
	 * The event id for <code>MobileProvider</code> network change. 
	 */
	public final static int MOBILEPROVIDER_NETWORK_CHANGE = 0x3;

	/**
	 * The event id for <code>MobileProvider</code> no network situation. 
	 */
	public final static int MOBILEPROVIDER_NO_NETWORK = 0x8;

	/**
	 * Returns the <code>MobileProvider</code> associated with this event.
	 * <p>
	 * @return The MobileProvider associated with this event.
	 */
	public MobileProvider getMobileProvider();

	/**
     * Returns the cause for a service state.
     * <p>
     * @return a cause value.
     */
    public int getMobileStateCause();

}
