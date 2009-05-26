/*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyrights:
 *
 * Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 * 901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 * This product and related documentation are protected by copyright and
 * distributed under licenses restricting its use, copying, distribution, and
 * decompilation. No part of this product or related documentation may be
 * reproduced in any form by any means without prior written authorization of
 * Sun and its licensors, if any.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 * States Government is subject to the restrictions set forth in DFARS
 * 252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 * The product described in this manual may be protected by one or more U.S.
 * patents, foreign patents, or pending applications.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Author:
 *
 * AePONA Limited, Interpoint Building
 * 20-24 York Street, Belfast BT15 1AQ
 * N. Ireland.
 *
 *
 * Module Name   : JAIN TCAP API
 * File Name     : InidirectionalReqEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     14/11/2000  Phelim O'Doherty    Deprecated dialogueIdPresent method
 *                                         as Dialogue Id is mandatory.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

package jain.protocol.ss7.tcap.dialogue;

import jain.*;
import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.SccpUserAddress;

/**
 *  An event representing a TCAP Unidirectional Request dialogue primitive. This
 *  event will be passed from the Listener (the TC User) to the Provider (TCAP)
 *  to trigger the sending of components to the destination node when no reply
 *  is expected. This primitive is used when there is no need to establish a
 *  transaction with another peer TC User. <BR>
 *  The mandatory parameters of this primitive are supplied to the constructor.
 *  Optional parameters may then be set using the set methods. <p>
 *
 *  The optional parameters 'Application Context Name' and 'User Information'
 *  are centrally located in the Dialogue Portion class, therefore to manipulate
 *  them it is necessary to instantiate the Dialogue Portion Object and use the
 *  accessors method for the two parameters in that Dialogue Portion Object.
 *
 *@author     Sun Microsystems Inc.
 *@version    1.1
 *@see        DialogueReqEvent
 */


public final class UnidirectionalReqEvent extends jain.protocol.ss7.tcap.DialogueReqEvent {

	/**
	 *  Constructs a new UnidirectionalReqEvent, with only the Event Source and the
	 *  <a href="package-summary.html">JAIN TCAP Mandatory</a> parameters being
	 *  supplied to the constructor.
	 *
	 * Within this method TcapUserAddress has been changed to SccpUserAddress.
	 * The method has not been deprecated as a new method using type SccpUserAddress
	 * would have to be created with a different method name. This is less desirable
	 * than the effective removal of the old method.
	 *
	 *@param  source         the Event Source supplied to the constructor
	 *@param  originAddress  the Originating Address supplied to the constructor
	 *@param  destAddress    the Destination Address supplied to the constructor
	 */

	public UnidirectionalReqEvent(Object source, SccpUserAddress originAddress, SccpUserAddress destAddress) {
		super(source);
		setOriginatingAddress(originAddress);
		setDestinationAddress(destAddress);
	}


	/**
	 *  Sets the Destination Address parameter of the Unidirectional Request primitive.
	 *  Destination Address is an SCCP parameter that is required from the
	 *  application.
	 *
	 * Within this method TcapUserAddress has been changed to SccpUserAddress.
	 * The method has not been deprecated as a new method using type SccpUserAddress
	 * would have to be created with a different method name. This is less desirable
	 * than the effective removal of the old method.
	 *
	 *@param  destination   The new Destination Address value
	 */

	public void setDestinationAddress(SccpUserAddress destination) {

                m_destinationAddress = destination;
		m_destinationAddressPresent = true;
	}


	/**
	 *  Sets the Originating Address parameter of the Unidirectional Request primitive.
	 *  Origination Address is an SCCP parameter that is required from the
	 *  application.
	 *
	 * Within this method TcapUserAddress has been changed to SccpUserAddress.
	 * The method has not been deprecated as a new method using type SccpUserAddress
	 * would have to be created with a different method name. This is less desirable
	 * than the effective removal of the old method.
	 *
	 *@param  origin The new Originating Address value
	 */

	public void setOriginatingAddress(SccpUserAddress origin) {

                m_originatingAddress = origin;
		m_originatingAddressPresent = true;
	}


	/**
	 *@deprecated    As of JAIN TCAP v1.1. No replacement. The
         *      <var>dialogueId</var> parameter is mandatory in all Dialogue
         *      Request events.
	 */

	public boolean isDialogueIdPresent() {
		return m_dialogueIdPresent;
	}


	/**
	 *  Gets the Destination Address parameter of the Unidirectional request
	 *  primtive. Destination Address is an SCCP parameter that is required from
	 *  the application.
	 *
	 * The return type of this get method has been changed from TcapUserAddress.
	 * The TcapUserAddress class has been deprecated in this release (V1.1). This
	 * method has not been deprecated as it's replacement would then have to have
	 * a different name.
	 *
	 *@return the SccpUserAddress representing the Destination Address of
         *      the Unidirectional request primtive.
	 *@exception  MandatoryParameterNotSetException  this exception is thrown if
	 *      this JAIN Mandatory parameter has not been set
	 */

	public SccpUserAddress getDestinationAddress() throws MandatoryParameterNotSetException {
		if (m_destinationAddressPresent == true) {
			return m_destinationAddress;
		}
		else {
			throw new MandatoryParameterNotSetException();
		}
	}


	/**
	 *  Gets the Originating Address parameter of the Unidirectional request
	 *  primitive. Origination Address is an SCCP parameter that is required from
	 *  the application.
	 *
	 * The return type of this get method has been changed from TcapUserAddress.
	 * The TcapUserAddress class has been deprecated in this release (V1.1). This
	 * method has not been deprecated as it's replacement would then have to have
	 * a different name.
	 *
	 *@return the SccpUserAddress represnting the Originating Address of
         *      the UnidirectionalEvent.
	 *@exception  MandatoryParameterNotSetException  this exception is thrown if
	 *      this JAIN Mandatory parameter has not yet been set
	 */

	public SccpUserAddress getOriginatingAddress() throws MandatoryParameterNotSetException {
		if (m_originatingAddressPresent == true) {
			return m_originatingAddress;
		}
		else {
			throw new MandatoryParameterNotSetException();
		}
	}


	/**
	 *  This method returns the type of this primitive.
	 *
	 *@return    The Primitive Type of this Event
	 */

	public int getPrimitiveType() {
		return (jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_UNIDIRECTIONAL);
	}


	/**
	 *  Clears all previously set parameters .
	 */

	public void clearAllParameters() {
		m_dialoguePortionPresent = false;
		//POD - Update v1.1: Deprecated field
                m_dialogueIdPresent = false;
		m_destinationAddressPresent = false;
		m_originatingAddressPresent = false;
		m_qualityOfServicePresent = false;
	}


	/**
	 *  String representation of class UnidirectionalReqEvent
	 *
	 *@return    String provides description of class UnidirectionalReqEvent
	 */
	public String toString() {
		StringBuffer buffer = new StringBuffer(500);
		buffer.append("\n\nUnidirectionalReqEvent");
		buffer.append(super.toString());
		buffer.append("\n\ndestinationAddress = ");
		if (m_destinationAddress != null) {
			buffer.append(m_destinationAddress.toString());
		}
		else {
			buffer.append("value is null");
		}
		buffer.append("\n\noriginatingAddress = ");
		if (m_originatingAddress != null) {
			buffer.append(m_originatingAddress.toString());
		}
		else {
			buffer.append("value is null");
		}
		buffer.append("\n\ndestinationAddressPresent = ");
		buffer.append(m_destinationAddressPresent);
		buffer.append("\n\noriginatingAddressPresent = ");
		buffer.append(m_originatingAddressPresent);

		return buffer.toString();
	}


	/**
	 *  The Destination User Address parameter of the Unidirectional request
	 *  dialogue primitive
	 *
	 *@serial    m_destinationAddress - a default serializable field
	 */

	private SccpUserAddress m_destinationAddress = null;

	/**
	 *  The Originating User Address parameter of the Unidirectional request
	 *  dialogue primitive
	 *
	 *@serial    m_originatingAddress - a default serializable field
	 */

	private SccpUserAddress m_originatingAddress = null;

	/**
	 *
	 *
	 *@serial    m_destinationAddressPresent - a default serializable field
	 */

	private boolean m_destinationAddressPresent = false;

	/**
	 *
	 *
	 *@serial    m_originatingAddressPresent - a default serializable field
	 */

	private boolean m_originatingAddressPresent = false;

}

