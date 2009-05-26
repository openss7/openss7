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
 * File Name     : NoticeIndEvent.java
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
import jain.protocol.ss7.SccpUserAddress;

/**
 *  An event representing a TCAP Notice indication dialogue primitive. This
 *  event will be passed from the Provider (TCAP) to the Listener(the TC User)
 *  to indicate that the Network Service Provider has been unable to provide the
 *  requested service. <BR>
 *  The mandatory parameters of this primitive are supplied to the constructor.
 *  Optional parameters may then be set using the set methods.
 *
 *@author     Sun Microsystems Inc.
 *@version    1.1
 *@see        DialogueIndEvent
 */

public final class NoticeIndEvent extends DialogueIndEvent {

	/**
	 *  Constructs a new NoticeIndEvent, with only the Event Source and the <a
	 *  href="package-summary.html">JAIN TCAP Mandatory</a> parameters being
	 *  supplied to the constructor.
	 *
	 *@param  source       the Event Source supplied to the constructor
	 *@param  dialogueId   the Dialogue Identifier supplied to the constructor
	 *@param  reportCause  the Report Cause supplied to the constructor
	 */

	public NoticeIndEvent(Object source, int dialogueId, byte[] reportCause) {
		super(source);
		setDialogueId(dialogueId);
		setReportCause(reportCause);
	}


	/**
	 *  Sets the Destination Address parameter of the Notice indication primitive.
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
	 *  Sets the Originating Address parameter of the Notice indication primitive.
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
	 *  Sets the Report Cause parameter of this Notice indication primitive.
	 *
	 *@param  reportCause The new Report Cause value
	 */

	public void setReportCause(byte[] reportCause) {

		m_reportCause = reportCause;
		m_reportCausePresent = true;
	}


	/**
	 *  Indicates if the Destination Address parameter is present in this Event.
	 *
	 *@return    true if Destination Address has been set, false otherwise.
	 */

	public boolean isDestinationAddressPresent() {
		return m_destinationAddressPresent;
	}


	/**
	 *  Gets the Destination Address parameter of the Notice indication primtive.
	 *  Destination Address is an SCCP parameter that is required from the
	 *  application.
	 *
	 * The return type of this get method has been changed from TcapUserAddress.
	 * The TcapUserAddress class has been deprecated in this release (V1.1). This
	 * method has not been deprecated as it's replacement would then have to have
	 * a different name.
	 *
	 *@return the SccpUserAddress representing the Destination Address of
         *      the Notice indication primtive.
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      parameter has not yet been set
	 */

	public SccpUserAddress getDestinationAddress() throws ParameterNotSetException {
		if (m_destinationAddressPresent == true) {
			return m_destinationAddress;
		}
		else {
			throw new ParameterNotSetException();
		}
	}


	/**
	 *  Indicates if the Originating Address parameter is present in this Event.
	 *
	 *@return true if Originating Address has been set, false otherwise.
	 */

	public boolean isOriginatingAddressPresent() {
		return m_originatingAddressPresent;
	}


	/**
	 *  Gets the Originating Address parameter of the Notice indication primitive.
	 *  Origination Address is an SCCP parameter that is required from the
	 *  application.
	 *
	 * The return type of this get method has been changed from TcapUserAddress.
	 * The TcapUserAddress class has been deprecated in this release (V1.1). This
	 * method has not been deprecated as it's replacement would then have to have
	 * a different name.
	 *
	 *@return the SccpUserAddress represnting the Originating Address of the
         *      Notice Event.
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      parameter has not been set
	 */

	public SccpUserAddress getOriginatingAddress() throws ParameterNotSetException {
		if (m_originatingAddressPresent == true) {
			return m_originatingAddress;
		}
		else {
			throw new ParameterNotSetException();
		}
	}


	/**
	 *  Indicates if the Report Cause parameter is present in this Event.
	 *
	 *@return    true if Report Cause has been set, false otherwise.
	 */

	public boolean isReportCausePresent() {
		return m_reportCausePresent;
	}


	/**
	 *  Gets the Report Cause parameter of this Notice indication primitive. Report
	 *  Cause contains information indicating the reason that the message was
	 *  returned by the SCCP
	 *
	 *@return the report cause of this Notice indication primitive.
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      Mandatory parameter has not yet been set
	 */

	public byte[] getReportCause() throws ParameterNotSetException {
		if (m_reportCausePresent == true) {
			return m_reportCause;
		}
		else {
			throw new ParameterNotSetException();
		}
	}


	/**
	 *  This method returns the type of this primitive.
	 *
	 *@return    The Primitive Type value
	 */

	public int getPrimitiveType() {
		return (jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_NOTICE);
	}


	/**
	 *  Clears all previously set parameters .
	 */

	public void clearAllParameters() {
		m_dialoguePortionPresent = false;
		m_dialogueIdPresent = false;
		m_destinationAddressPresent = false;
		m_originatingAddressPresent = false;
		m_reportCausePresent = false;
	}



	/**
	 *  String representation of class NoticeIndEvent
	 *
	 *@return    String provides description of class NoticeIndEvent
	 */
	public String toString() {
		StringBuffer buffer = new StringBuffer(500);
		buffer.append("\n\nNoticeIndEvent");
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
		buffer.append("\n\nreportCause = ");
		if (m_reportCause != null) {
			buffer.append(m_reportCause.toString());
		}
		else {
			buffer.append("value is null");
		}
		buffer.append("\n\ndestinationAddressPresent = ");
		buffer.append(m_destinationAddressPresent);
		buffer.append("\n\noriginatingAddressPresent = ");
		buffer.append(m_originatingAddressPresent);
		buffer.append("\n\nreportCausePresent = ");
		buffer.append(m_reportCausePresent);

		return buffer.toString();
	}


	/**
	 *  The Destination User Address parameter of the Notice indication dialogue
	 *  primitive
	 *
	 *@serial    m_destinationAddress - a default serializable field
	 */

	private SccpUserAddress m_destinationAddress = null;

	/**
	 *  The Originating User Address parameter of the Notice indication dialogue
	 *  primitive
	 *
	 *@serial    m_originatingAddress - a default serializable field
	 */

	private SccpUserAddress m_originatingAddress = null;

	/**
	 *  The Report Cause parameter of the Notice indication dialogue primitive
	 *
	 *@serial    m_reportCause - a default serializable field
	 */

	private byte[] m_reportCause = null;

	/**
	 *@serial    m_destinationAddressPresent - a default serializable field
	 */

	private boolean m_destinationAddressPresent = false;

	/**
	 *@serial    m_originatingAddressPresent - a default serializable field
	 */

	private boolean m_originatingAddressPresent = false;

	/**
	 *@serial    m_reportCausePresent - a default serializable field
	 */

	private boolean m_reportCausePresent = false;

}
