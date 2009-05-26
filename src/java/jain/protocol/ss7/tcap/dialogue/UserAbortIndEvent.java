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
 * File Name     : UserAbortIndEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     14/11/2000  Phelim O'Doherty    Updated after public release and
 * certification process comments.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

package jain.protocol.ss7.tcap.dialogue;

import jain.*;
import jain.protocol.ss7.tcap.*;

/**
 *  An event representing a TCAP UserAbort indication dialogue primitive. This
 *  event will be passed from the Provider (TCAP) to the Listener(the TC User)
 *  to indicate the abrupt termination of a dialogue. <BR>
 *  The mandatory parameters of this primitive are supplied to the constructor.
 *  Optional parameters may then be set using the set methods.
 *
 *@author     Sun Microsystems Inc.
 *@version    1.1
 *@see        DialogueIndEvent
 */

public final class UserAbortIndEvent extends DialogueIndEvent {

	/**
	 *  Constructs a new UserAbortIndEvent, with only the Event Source and the <a
	 *  href="package-summary.html">JAIN TCAP Mandatory</a> parameters being
	 *  supplied to the constructor.
	 *
	 *@param  source      the Event Source supplied to the constructor
	 *@param  dialogueId  the Dialogue Identifier supplied to the constructor
	 */

	public UserAbortIndEvent(Object source, int dialogueId) {
		super(source);
		setDialogueId(dialogueId);
	}


	/**
	 *  Sets the Quality of Service parameter of the UserAbort indication
	 *  primitive.
	 *
	 *@param  qualityOfService  The new Quality Of Service value
	 */

	public void setQualityOfService(byte qualityOfService) {

		m_qualityOfService = qualityOfService;
		m_qualityOfServicePresent = true;
	}


	/**
	 *  Sets the User Abort Information parameter of the UserAbort indication
	 *  primitive.
	 *
	 *@param  userAbortInformation  The new User Abort Information
	 */

	public void setUserAbortInformation(byte[] userAbortInformation) {
		m_userAbortInformation = userAbortInformation;
		m_userAbortInformationPresent = true;

	}


	/**
	 *  Sets the Abort Reason parameter of the UserAbort indication primitive.
	 *  Abort Reason indicates whether a dialogue is aborted because the received
	 *  application context name is not supported and no alternative one can be
	 *  proposed (abort reason = application context not supported) or because of
	 *  any other user problem (abort reason = user specific).
	 *
	 *@param  abortReason  one of the following:
         *	<UL>
	 *          <LI>ABORT_REASON_ACN_NOT_SUPPORTED
         *          <LI>ABORT_REASON_USER_SPECIFIC
         *      </UL>
	 */

	public void setAbortReason(int abortReason) {

		m_abortReason = abortReason;
		m_abortReasonPresent = true;
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
	 *  Gets the Quality of Service parameter of the UserAbort indication
	 *  primitive. Quality of Service is an SCCP parameter that is required from
	 *  the application.
	 *
	 *@return  the Quality of Service parameter of the UserAbortEvent
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
	 *  Indicates if the User Abort Information parameter is present in this Event.
	 *
	 *@return    true if User Abort Information has been set, false otherwise.
	 */

	public boolean isUserAbortInformationPresent() {
		return m_userAbortInformationPresent;
	}


	/**
	 *  Gets the User Abort Information parameter of the UserAbort indication
	 *  primitive. User Information is the information, which can be exchanged
	 *  between TCAP applications independently from the remote operation service.
	 *
	 *@return the User Abort Information of the UserAbortEvent.
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      parameter has not been set
	 */

	public byte[] getUserAbortInformation() throws ParameterNotSetException {
		if (m_userAbortInformationPresent == true) {
			return m_userAbortInformation;
		}
		else {
			throw new ParameterNotSetException();
		}
	}


	/**
	 *  Indicates if the abort reason parameter is present in this Event.
	 *
	 *@return true if abort reason has been set, false otherwise.
	 */

	public boolean isAbortReasonPresent() {
		return m_abortReasonPresent;
	}


	/**
	 *  Gets the abort reason parameter of the UserAbort indication primitive.
	 *  Abort Reason indicates whether a dialogue is aborted because the received
	 *  application context name is not supported and no alternative one can be
	 *  proposed (abort reason = application context not supported) or because of
	 *  any other user problem (abort reason = user specific).
	 *
	 *@return one of the following:
	 *      <UL>
	 *        <LI> ABORT_REASON_ACN_NOT_SUPPORTED
	 *        <LI> ABORT_REASON_USER_SPECIFIC
	 *      </UL>
	 *
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      parameter has not yet been set
	 */

	public int getAbortReason() throws ParameterNotSetException {
		if (m_abortReasonPresent == true) {
			return m_abortReason;
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
		return (jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_USER_ABORT);
	}


	/**
	 *  Clears all previously set parameters .
	 */

	public void clearAllParameters() {
		m_dialoguePortionPresent = false;
		m_dialogueIdPresent = false;
		m_qualityOfServicePresent = false;
		m_userAbortInformationPresent = false;
		m_abortReasonPresent = false;
	}


	/**
	 *  String representation of class UserAbortIndEvent
	 *
	 *@return    String provides description of class UserAbortIndEvent
	 */
	public String toString() {

		StringBuffer buffer = new StringBuffer(500);
		buffer.append("\n\nUserAbortIndEvent");
		buffer.append(super.toString());
		buffer.append("\n\nabortReason = ");
		buffer.append(m_abortReason);
		buffer.append("\n\nqualityOfService = ");
		buffer.append(m_qualityOfService);
		buffer.append("\n\nuserAbortInformation = ");
		if (m_userAbortInformation != null) {
                        for(int ptr=0; ptr < m_userAbortInformation.length; ptr++) {
			    buffer.append(m_userAbortInformation[ptr]);
                            buffer.append(" ");
                        }
		} else {
		    buffer.append("value is null");
		}
		buffer.append("\n\nqualityOfServicePresent = ");
		buffer.append(m_qualityOfServicePresent);
		buffer.append("\n\nuserAbortInformationPresent = ");
		buffer.append(m_userAbortInformationPresent);
		buffer.append("\n\nabortReasonPresent = ");
		buffer.append(m_abortReasonPresent);

		return buffer.toString();
	}


	/**
	 *  The Abort Reason parameter of the UserAbort indication dialogue primitive
	 *
	 *@serial    m_abortReason - a default serializable field
	 */

	private int m_abortReason = 0;

	/**
	 *  The Quality of Service parameter of the UserAbort indication dialogue
	 *  primitive
	 *
	 *@serial    m_qualityOfService - a default serializable field
	 */

	private byte m_qualityOfService = 0;

	/**
	 *  The User Abort Information parameter of the UserAbort indication dialogue
	 *  primitive
	 *
	 *@serial    m_userAbortInformation - a default serializable field
	 */

	private byte[] m_userAbortInformation = null;

	/**
	 *@serial    m_qualityOfServicePresent - a default serializable field
	 */

	private boolean m_qualityOfServicePresent = false;

	/**
	 *@serial    m_userAbortInformationPresent - a default serializable field
	 */

	private boolean m_userAbortInformationPresent = false;

	/**
	 *@serial    m_abortReasonPresent - a default serializable field
	 */

	private boolean m_abortReasonPresent = false;
}
