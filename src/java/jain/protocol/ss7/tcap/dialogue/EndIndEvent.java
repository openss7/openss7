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
 * File Name     : EndIndEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     15/11/2000  Phelim O'Doherty    Updated after public release and
 *                                         certification process comments.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

package jain.protocol.ss7.tcap.dialogue;

import jain.*;
import jain.protocol.ss7.tcap.*;

/**
 *  An event representing a TCAP End indication dialogue primitive. This event
 *  will be passed from the Provider (TCAP) to the Listener(the TC User) to
 *  indicate the termination of a structured dialogue with the originating node.
 *  <BR>
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
 *@see        DialogueIndEvent
 */

public final class EndIndEvent extends jain.protocol.ss7.tcap.DialogueIndEvent {

	/**
	 *  Constructs a new EndIndEvent, with only the Event Source and the <a
	 *  href="package-summary.html">JAIN TCAP Mandatory</a> parameters being
	 *  supplied to the constructor.
	 *
	 *@param  source      the Event Source supplied to the constructor
	 *@param  dialogueId  the Dialogue Identifier supplied to the constructor
	 *@param  componentsPresent  the Components Present Flag supplied to the constructor
	 */

	public EndIndEvent(Object source, int dialogueId, boolean componentsPresent) {
		super(source);
                setDialogueId(dialogueId);
                setComponentsPresent(componentsPresent);
	}


	/**
	 *  Sets the Quality of Service parameter of the End indication primitive.
	 *
	 *@param  qualityOfService  The new Quality Of Service value
	 */

	public void setQualityOfService(byte qualityOfService) {

		m_qualityOfService = qualityOfService;
		m_qualityOfServicePresent = true;
	}


	/**
	 *  Sets the Components present parameter of this End indication primitive.
	 *  This flag is used to determine if their are any components associated with
	 *  this primitive. This flag will be reset to false when the <a
	 *  href="#clearAllParameters">clearAllParameters()</a> method is invoked.
	 *
	 *@param  componentsPresent The new Components Present Flag
	 */

	public void setComponentsPresent(boolean componentsPresent) {

		m_componentsPresent = componentsPresent;
	}


	/**
	 *  Indicates if the Quality of Service parameter is present in this Event.
	 *
	 *@return    true if Quality of Service has been set, false otherwise.
	 */

	public boolean isQualityOfServicePresent() {
		return m_qualityOfServicePresent;
	}


	/**
	 *  Gets the Quality of Service parameter of the End indication primitive.
	 *  Quality of Service is an SCCP parameter that is required from the
	 *  application.
	 *
	 *@return the Quality of Service parameter of the EndEvent
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      parameter has not yet been set
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
	 *  This method returns the type of this primitive.
	 *
	 *@return    The Primitive Type of this Event
	 */

	public int getPrimitiveType() {
		return (jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_END);
	}


	/**
	 *  Returns the Components present flag of this End indication primitive. This
	 *  flag is used to determine if their are any components associated with this
	 *  primitive. This flag will be reset to false when the <a
	 *  href="#clearAllParameters">clearAllParameters()</a> method is invoked.
	 *
	 *@return    the Components Present of the EndEvent.
	 */

	public boolean isComponentsPresent() {
		return (m_componentsPresent);
	}


	/**
	 *  Clears all previously set parameters and resets the 'Components Present'
	 *  flag to false.
	 */

	public void clearAllParameters() {
		m_dialoguePortionPresent = false;
		m_dialogueIdPresent = false;
		m_qualityOfServicePresent = false;
		m_componentsPresent = false;
	}


	/**
	 *  String representation of class EndIndEvent
	 *
	 *@return    String provides description of class EndIndEvent
	 */
	public String toString() {
		StringBuffer buffer = new StringBuffer(500);
		buffer.append("\n\nEndIndEvent");
		buffer.append(super.toString());
		buffer.append("\n\nqualityOfService = ");
		buffer.append(m_qualityOfService);
		buffer.append("\n\nqualityOfServicePresent = ");
		buffer.append(m_qualityOfServicePresent);
		buffer.append("\n\ncomponentsPresent = ");
		buffer.append(m_componentsPresent);

		return buffer.toString();
	}


	/**
	 *  The Quality of Service parameter of the End indication dialogue primitive
	 *
	 *@serial    m_qualityOfService - a default serializable field
	 */

	private byte m_qualityOfService = 0;

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

