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
 * File Name     : ProviderAbortIndEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     14/11/2000  Phelim O'Doherty    Updated after public release and
 *                                         certification process comments.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

package jain.protocol.ss7.tcap.dialogue;

import jain.*;
import jain.protocol.ss7.tcap.*;

/**
 *  An event representing a TCAP ProviderAbort indication dialogue primitive.
 *  This event will be passed from the Provider (TCAP) to the Listener(the TC
 *  User) to inform the Tc User that the dialogue has been terminated by the
 *  service provider (the transaction sublayer) in reaction to a transaction
 *  abort by the transaction sublayer. Any pending components are not
 *  transmitted. <BR>
 *  The mandatory parameters of this primitive are supplied to the constructor.
 *  Optional parameters may then be set using the set methods.
 *
 *@author     Sun Microsystems Inc.
 *@version    1.1
 *@see        DialogueIndEvent
 */

public final class ProviderAbortIndEvent extends DialogueIndEvent {

	/**
	 *  Constructs a new ProviderAbortIndEvent, with only the Event Source and the
	 *  <a href="package-summary.html">JAIN TCAP Mandatory</a> parameters being
	 *  supplied to the constructor.
	 *
	 *@param  source      the Event Source supplied to the constructor
	 *@param  dialogueId  the Dialogue Identifier supplied to the constructor
	 *@param  pAbort      the PAbort Cause supplied to the constructor
	 */

	public ProviderAbortIndEvent(Object source, int dialogueId, int pAbort) {
		super(source);
		setDialogueId(dialogueId);
		setPAbort(pAbort);
	}


	/**
	 *  Sets the Quality of Service parameter of the ProviderAbort indication
	 *  primitive.
	 *
	 *@param  qualityOfService  The new Quality Of Service value
	 */

	public void setQualityOfService(byte qualityOfService) {

		m_qualityOfService = qualityOfService;
		m_qualityOfServicePresent = true;
	}


	/**
	 *  Sets the Provider Abort (pAbort) cause of the ProviderAbort indication
	 *  primitive. The PAbort parameter contains information indicating the reason
	 *  that the TCAP layer aborted a dialogue.
	 *
	 *@param  pAbort  one of the following:
         *     <UL>
         *        <LI>P_ABORT_UNRCGNZ_MSG_TYPE
         *        <LI>P_ABORT_UNRECOGNIZED_TRANSACTION_ID
         *        <LI>P_ABORT_BADLY_FORMATTED_TRANSACTION_PORTION
         *        <LI>P_ABORT_INCORRECT_TRANSACTION_PORTION
         *        <LI>P_ABORT_RESOURCE_LIMIT
         *        <LI>P_ABORT_ABNORMAL_DIALOGUE
         *        <LI>P_ABORT_UNRECOG_DIALOGUE_PORTION_ID(ANSI ONLY)
         *        <LI>P_ABORT_BADLY_STRUCTURED_DIALOGUE_PORTION(ANSI ONLY)
         *        <LI>P_ABORT_MISSING_DIALOGUE_PORTION(ANSI ONLY)
         *        <LI>P_ABORT_INCONSISTENT_DIALOGUE_PORTION(ANSI ONLY)
         *        <LI>P_ABORT_PERMISSION_TO_RELEASE_PROBLEM(ANSI ONLY)
         *      </UL>
	 */

	public void setPAbort(int pAbort) {
		m_pAbort = pAbort;
		m_pAbortPresent = true;
	}


	/**
	 *  Indicates if the Quality of Service parameter is present in this Event.
	 *
	 *@return  true if Quality of Service has been set, false otherwise.
	 */

	public boolean isQualityOfServicePresent() {
		return m_qualityOfServicePresent;
	}


	/**
	 *  Gets the Quality of Service parameter of the ProviderAbort indication
	 *  primitive. Quality of Service is an SCCP parameter that is required from
	 *  the application.
	 *
	 *@return the Quality of Service parameter of the ProviderAbortEvent
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
	 *  Gets the Provider Abort (pAbort) parameter of the ProviderAbort indication
	 *  primitive. The PAbort parameter contains information indicating the reason
	 *  that the TCAP layer aborted a dialogue.
	 *
	 *@return one of the following:
         *     <UL>
         *        <LI>P_ABORT_UNRCGNZ_MSG_TYPE
         *        <LI>P_ABORT_UNRECOGNIZED_TRANSACTION_ID
         *        <LI>P_ABORT_BADLY_FORMATTED_TRANSACTION_PORTION
         *        <LI>P_ABORT_INCORRECT_TRANSACTION_PORTION
         *        <LI>P_ABORT_RESOURCE_LIMIT
         *        <LI>P_ABORT_ABNORMAL_DIALOGUE
         *        <LI>P_ABORT_UNRECOG_DIALOGUE_PORTION_ID(ANSI ONLY)
         *        <LI>P_ABORT_BADLY_STRUCTURED_DIALOGUE_PORTION(ANSI ONLY)
         *        <LI>P_ABORT_MISSING_DIALOGUE_PORTION(ANSI ONLY)
         *        <LI>P_ABORT_INCONSISTENT_DIALOGUE_PORTION(ANSI ONLY)
         *        <LI>P_ABORT_PERMISSION_TO_RELEASE_PROBLEM(ANSI ONLY)
         *      </UL>
         *
	 *@exception  MandatoryParameterNotSetException  this exception is thrown if
	 *      this JAIN Mandatory parameter has not been set
	 */

	public int getPAbort() throws MandatoryParameterNotSetException {
		if (m_pAbortPresent == true) {
			return m_pAbort;
		}
		else {
			throw new MandatoryParameterNotSetException();
		}
	}


	/**
	 *  This method returns the type of this primitive.
	 *
	 *@return    The Primitive Type of the Event
	 */

	public int getPrimitiveType() {
		return (jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_PROVIDER_ABORT);
	}


	/**
	 *  Clears all previously set parameters .
	 */

	public void clearAllParameters() {
		m_dialoguePortionPresent = false;
		m_dialogueIdPresent = false;
		m_qualityOfServicePresent = false;
		m_pAbortPresent = false;
	}


	/**
	 *  String representation of class ProviderAbortIndEvent
	 *
	 *@return    String provides description of class ProviderAbortIndEvent
	 */
	public String toString() {
		StringBuffer buffer = new StringBuffer(500);
		buffer.append("\n\nProviderAbortIndEvent");
		buffer.append(super.toString());
		buffer.append("\n\nqualityOfService = ");
		buffer.append(m_qualityOfService);
		buffer.append("\n\npAbort = ");
		buffer.append(m_pAbort);
		buffer.append("\n\nqualityOfServicePresent = ");
		buffer.append(m_qualityOfServicePresent);
		buffer.append("\n\npAbortPresent = ");
		buffer.append(m_pAbortPresent);

		return buffer.toString();
	}


	/**
	 *  The Quality of Service parameter of the ProviderAbort indication dialogue
	 *  primitive
	 *
	 *@serial    m_qualityOfService - a default serializable field
	 */

	private byte m_qualityOfService = 0;

	/**
	 *  The PAbort parameter of the ProviderAbort indication dialogue primitive
	 *
	 *@serial    m_pAbort - a default serializable field
	 */

	private int m_pAbort = 0;

	/**
	 *@serial    m_qualityOfServicePresent - a default serializable field
	 */

	private boolean m_qualityOfServicePresent = false;

	/**
	 *@serial    m_pAbortPresent - a default serializable field
	 */

	private boolean m_pAbortPresent = false;
}
