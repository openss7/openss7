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
 * File Name     : UniDirectionalIndEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     01/11/2000  Phelim O'Doherty    Added overriding methods for set
 *                                         and get dialogueId with no
 *                                         functionality.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

package jain.protocol.ss7.tcap.dialogue;

import jain.*;
import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.SccpUserAddress;

/**
 *  An event representing a TCAP Unidirectional indication dialogue primitive.
 *  This event will be passed from the Provider (TCAP) to the Listener(the TC
 *  User) to indicate the receipt of a TCAP message from an Unstructured
 *  dialogue (a dialogue between two TC Users with no explicit association
 *  between them). <BR>
 *  The mandatory parameters of this primitive are supplied to the constructor.
 *  Optional parameters may then be set using the set methods. <p>
 *
 *  The optional parameters 'Application Context Name' and 'User Information'
 *  are centrally located in the Dialogue Portion class, therefore to manipulate
 *  them it is necessary it instantiate the Dialogue Portion Object and use the
 *  accessors method for the two parameters in that Dialogue Portion Object. A
 *  Uni Indication event is the only Dialogue Indication Event that does not use
 *  a Dialogue Id within the supported variants. However for the JAIN TCAP API
 *  specification the Dialogue Id will be included to associate Component Events
 *  with this Dialogue Event, hence simplifying the implementation of a JAIN
 *  TCAP application.
 *
 *@author     Sun Microsystems Inc.
 *@version    1.1
 *@see        DialogueIndEvent
 */


public final class UnidirectionalIndEvent extends jain.protocol.ss7.tcap.DialogueIndEvent {

	/**
	 *  Constructs a new UnidirectionalIndEvent, with only the Event Source and the
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
	 *@param  componentsPresent  the Components Present Flag supplied to the constructor
	 */

	public UnidirectionalIndEvent(Object source, SccpUserAddress originAddress,
			SccpUserAddress destAddress, boolean componentsPresent) {
		super(source);
		setOriginatingAddress(originAddress);
		setDestinationAddress(destAddress);
		setComponentsPresent(componentsPresent);
	}


	/**
	 *  <b>Note to developers</b> :- A Dialogue Id is forbidden in a UNI Indication
	 *  message, therefore this method should not be used in a UnidirectionalInd
	 *  Event. This method overrides the setDialogueId in the Dialogue Indication
	 *  Event and contains no functionality.
	 *
	 *@param  dialogueId  The new DialogueId value
	 *@since JAIN TCAP v1.1
	 */

	public void setDialogueId(int dialogueId) {

		// Empty method overriding setDialogueId in Dialogue Indication event
		// as a UNI Indication doesn't contain a dialogue Id.
	}


	/**
	 *  Sets the Destination Address parameter of the Unidirectional indication primitive.
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
	 *  Sets the Originating Address parameter of the Unidirectional indication primitive.
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
	 *  Sets the Quality of Service parameter of the unidirectional indication
	 *  primitive.
	 *
	 *@param  qualityOfService  The new Quality Of Service value
	 */

	public void setQualityOfService(byte qualityOfService) {

		m_qualityOfService = qualityOfService;
		m_qualityOfServicePresent = true;
	}


	/**
	 *  Sets the Components present parameter of this Unidirectional indication
	 *  primitive. This flag is used to determine if their are any components
	 *  associated with this primitive. This flag will be reset to false when the
	 *  <a href="#clearAllParameters">clearAllParameters()</a> method is invoked.
	 *
	 *@param  componentsPresent  The new Components Present Flag
	 */

	public void setComponentsPresent(boolean componentsPresent) {

		m_componentsPresent = componentsPresent;
	}


	/**
	 *@deprecated    As of JAIN TCAP v1.1, there is no concept of a <var>dialogueId
	 *      </var> in a UNI Indication message.
	 */
	public boolean isDialogueIdPresent() {
		return m_dialogueIdPresent;
	}


	/**
	 *  <b>Note to developers</b> :- A Dialogue Id is forbidden in a UNI Indication
	 *  message, therefore this method should not be used in a UnidirectionalInd
	 *  Event. This method will always return the
	 *  MandatoryParameterNotSetException, with the error message "A UNI Indication
	 *  does not contain a Dialogue Identifier".
	 *
	 *@return  Will always throw a MandatoryParameterNotSetException
	 *@exception  MandatoryParameterNotSetException  this exception is thrown if
	 *      this JAIN Mandatory Parameter has not been set.
	 *@since JAIN TCAP v1.1
	 */

	public int getDialogueId() throws MandatoryParameterNotSetException {
		throw new MandatoryParameterNotSetException("A UNI Indication does not contain a Dialogue Identifier");
	}


	/**
	 *  Gets the Destination Address parameter of the Unidirectional indication
	 *  primtive. Destination Address is an SCCP parameter that is required from
	 *  the application.
	 *
	 * The return type of this get method has been changed from TcapUserAddress.
	 * The TcapUserAddress class has been deprecated in this release (V1.1). This
	 * method has not been deprecated as it's replacement would then have to have
	 * a different name.
	 *
	 *@return the SCCP User Address representing the Destination
         *      Address of the Unidirectional indication primtive.
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
	 *  Gets the Originating Address parameter of the Unidirectional indication
	 *  primitive. Origination Address is an SCCP parameter that is required from
	 *  the application.
	 *
	 * The return type of this get method has been changed from TcapUserAddress.
	 * The TcapUserAddress class has been deprecated in this release (V1.1). This
	 * method has not been deprecated as it's replacement would then have to have
	 * a different name.
	 *
	 *@return the SCCP User Address represnting the Originating Address of the
         *      UnidirectionalEvent.
	 *@exception  MandatoryParameterNotSetException  this exception is thrown if
	 *      this JAIN Mandatory parameter has not been set
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
	 *  Indicates if the Quality of Service parameter is present in this Event.
	 *
	 *@return true if Quality of Service has been set, false otherwise.
	 */

	public boolean isQualityOfServicePresent() {
		return m_qualityOfServicePresent;
	}


	/**
	 *  Gets the Quality of Service parameter of the unidirectional indication
	 *  primitive. Quality of Service is an SCCP parameter that is required from
	 *  the application.
	 *
	 *@return the Quality of Service parameter of the UnidirectionalEvent
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      parameter has not been set
	 */

	public byte getQualityOfService() throws ParameterNotSetException {
		if (m_qualityOfServicePresent == true) {
			return m_qualityOfService;
		}
		else {
			throw new ParameterNotSetException();
		}
	}


	/**
	 *  Returns the Components present flag of this Unidirectional indication
	 *  primitive. This flag is used to determine if their are any components
	 *  associated with this primitive. This flag will be reset to false when the
	 *  <a href="#clearAllParameters">clearAllParameters()</a> method is invoked.
	 *
	 *@return true if Components are present in this UnidirectionalEvent,
         *        false is no Components are present in this UnidirectionalEvent.
	 */

	public boolean isComponentsPresent() {
		return (m_componentsPresent);
	}


	/**
	 *  This method returns the type of this primitive.
	 *
	 *@return    The Primitive Type of this event
	 */

	public int getPrimitiveType() {
		return (jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_UNIDIRECTIONAL);
	}


	/**
	 *  Clears all previously set parameters and resets the 'Components Present'
	 *  flag to false.
	 */

	public void clearAllParameters() {
		m_dialoguePortionPresent = false;
		m_destinationAddressPresent = false;
		m_originatingAddressPresent = false;
		m_qualityOfServicePresent = false;
		m_componentsPresent = false;
	}


	/**
	 *  String representation of class UnidirectionalIndEvent
	 *
	 *@return    String provides description of class UnidirectionalIndEvent
	 */
	public String toString() {
		StringBuffer buffer = new StringBuffer(500);
		buffer.append("\n\nUnidirectionalIndEvent");
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
		buffer.append("\n\nqualityOfService = ");
		buffer.append(m_qualityOfService);
		buffer.append("\n\ndestinationAddressPresent = ");
		buffer.append(m_destinationAddressPresent);
		buffer.append("\n\noriginatingAddressPresent = ");
		buffer.append(m_originatingAddressPresent);
		buffer.append("\n\nqualityOfServicePresent = ");
		buffer.append(m_qualityOfServicePresent);
		buffer.append("\n\ncomponentsPresent = ");
		buffer.append(m_componentsPresent);

		return buffer.toString();
	}


	/**
	 *@deprecated    As of JAIN TCAP v1.1, a UNI Indication does not contain a
	 *      <var>dialogueId</var>.
         *
	 *@serial        m_dialogueIdPresent - a default serializable field
	 */

	protected boolean m_dialogueIdPresent = false;

	/**
	 *  The Destination User Address parameter of the Unidirectional indication
	 *  dialogue primitive
	 *
	 *@serial    m_destinationAddress - a default serializable field
	 */

	private SccpUserAddress m_destinationAddress = null;

	/**
	 *  The Originating User Address parameter of the Unidirectional indication
	 *  dialogue primitive
	 *
	 *@serial    m_originatingAddress - a default serializable field
	 */

	private SccpUserAddress m_originatingAddress = null;

	/**
	 *  The Quality of Service parameter of the Unidirectional indication dialogue
	 *  primitive
	 *
	 *@serial    m_qualityOfService - a default serializable field
	 */

	private byte m_qualityOfService = 0;

	/**
	 *@serial    m_destinationAddressPresent - a default serializable field
	 */

	private boolean m_destinationAddressPresent = false;

	/**
	 *@serial    m_originatingAddressPresent - a default serializable field
	 */

	private boolean m_originatingAddressPresent = false;

	/**
	 *@serial    m_qualityOfServicePresent - a default serializable field
	 */

	private boolean m_qualityOfServicePresent = false;

	/**
	 *  The Components Present parameter of the Dialogue Indication primitive.
	 *
	 *@serial    m_componentsPresent - a default serializable field
	 */

	private boolean m_componentsPresent = false;

}

