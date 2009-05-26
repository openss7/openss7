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
 * File Name     : DialoguePortion.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     31/10/2000  Phelim O'Doherty    Added Identifer fields for App context
 *                                         and Sec context.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

package jain.protocol.ss7.tcap.dialogue;

import jain.*;
// import jain.protocol.ss7.tcap.*;


/**
 *  This class represents the optional Dialogue Portion of a dialogue handling
 *  primitive. An ITU 1992 or ITU 1996 implementation of the JAIN TCAP API
 *  should only use the Application Context and User Information parameters
 *  within this class. An ANSI 1992 implementation does not include a Dialogue
 *  Portion therefore should not use this class and an ANSI 1996 implementation
 *  may use all the parameters within this class.
 *
 *@author     Sun Microsystems Inc.
 *@version    1.1
 */

public final class DialoguePortion implements java.io.Serializable {

	/**
	 *  Constructs a new DialoguePortion
	 */

	public DialoguePortion() {
	}


	/**
	 *  Sets the Protocol Version parameter of the Dialogue Portion. The protocol
	 *  version is only specific to an ANSI 1996 implementation of the ANSI 1996
	 *  specification. A Begin or UNI Request event or the first backwards Continue
	 *  or End event may optionally include a protocol version.
	 *
	 *@param  protocolVersion  This can only have he following value until
         *      futher releases of the ANSI TCAP Specification:
	 *      <UL>
	 *        <LI> DP_PROTOCOL_VERSION_ANSI_96
	 *      </UL>
	 */

	public void setProtocolVersion(int protocolVersion) {

		m_protocolVersion = protocolVersion;
		m_protocolVersionPresent = true;
	}


	/**
	 *  Sets the Application Context Identifier parameter of the Dialogue Portion.
	 *  This field is specific only to an ANSI 1996 implementation of the JAIN TCAP
	 *  API, as ITU has only one value for the Application Context Tag. This field
	 *  informs an application how to interpret the application context information
	 *  within this object. This can have the following values:
	 *  <UL>
	 *    <LI> APPLICATION_CONTEXT_INTEGER
	 *    <LI> APPLICATION_CONTEXT_OBJECT
	 *  </UL>
	 *  <BR>
         *  <B>Note to Developers</b>:- The Application Context Identifier is
         *  defaulted to "APPLICATION_CONTEXT_INTEGER".
	 *
	 *@param  appContextIdentifier  The new Application Context Identifier
	 *@since  JAIN TCAP v1.1
	 */

	public void setAppContextIdentifier(int appContextIdentifier) {

		m_appContextIdentifier = appContextIdentifier;
	}


	/**
	 *  Sets the Application Context Name parameter of the Dialogue Portion. The
	 *  contents of the byte[] are not inspected by TCAP. A Begin, UNI or UserAbort
	 *  Request event or the first backwards Continue or End event may optionally
	 *  include an application context.
	 *
	 *@param  appContextName  The new Application Context Name value
	 */

	public void setAppContextName(byte[] appContextName) {

		m_appContextName = appContextName;
		m_appContextNamePresent = true;

	}


	/**
	 *  Sets the Security Context Identifier parameter of the Dialogue Portion.
	 *  This field informs an application how to interpret the Security context
	 *  information within this object. This can have the following values:
	 *  <UL>
	 *    <LI> SECURITY_CONTEXT_INTEGER
	 *    <LI> SECURITY_CONTEXT_OBJECT
	 *  </UL>
	 *  <BR>
         *  <B>Note to Developers</B>- The Security Context Identifier is defaulted to
	 *  "SECURITY_CONTEXT_INTEGER".
	 *
	 *@param  securityContextIdentifier The new Security Context Identifier value
	 */

	public void setSecurityContextIdentifier(int securityContextIdentifier) {

		m_securityContextIdentifier = securityContextIdentifier;
	}


	/**
	 *  Sets the Security Context Information parameter of the Dialogue Portion.
	 *  The security context byte[] is not inspected by TCAP. The Security Context
	 *  parameter is only used in an ANSI 1996 implementation of Jain Tcap.
	 *
	 *@param  securityContextInformation The new Security Context Information
	 */

	public void setSecurityContextInfo(byte[] securityContextInformation) {

		m_securityContextInformation = securityContextInformation;
		m_securityContextInfoPresent = true;

	}


	/**
	 *  Sets the Confidentiality Information parameter of the Dialogue Portion. The
	 *  Confidentiality parameter is only used in an ANSI 1996 implementation of
	 *  Jain Tcap, the parameters byte[] is not inspected by TCAP.
	 *
	 *@param  confidentialityInfo  The new Confidentiality Information
	 */

	public void setConfidentialityInformation(byte[] confidentialityInfo) {
		m_confidentialityInformation = confidentialityInfo;
		m_confidentialityInformationPresent = true;

	}


	/**
	 *  Sets the User Information parameter of of this Dialogue Portion.
	 *
	 *@param  userInformation The new User Information
	 */

	public void setUserInformation(byte[] userInformation) {
		m_userInformation = userInformation;
		m_userInformationPresent = true;

	}


	/**
	 *  Indicates if the Protocol Version parameter is present in this Event. This
	 *  method is only used by an ANSI 1996 implementation of the Jain Tcap
	 *  Specification.
	 *
	 *@return true if Protocol Version has been set, false otherwise.
	 */

	public boolean isProtocolVersionPresent() {
		return m_protocolVersionPresent;
	}


	/**
	 *  Gets the Protocol Version parameter of the Dialogue Portion. The protocol
	 *  version of the dialogue portion is only specific to an ANSI 1996
	 *  implementation of the Jain Tcap specification
	 *
	 *@return the Protocol Version of the Dialogue Portion. This can only have
         *the following value until futher releases of the ANSI TCAP Specification:
	 *      <UL>
	 *        <LI> DP_PROTOCOL_VERSION_ANSI_96
	 *      </UL>
	 *
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      parameter has not yet been set
	 */

	public int getProtocolVersion() throws ParameterNotSetException {
		if (m_protocolVersionPresent == true) {
			return m_protocolVersion;
		}
		else {
			throw new ParameterNotSetException();
		}
	}


	/**
	 *  Gets the Application Context Identifier parameter of the Dialogue Portion.
	 *  This field informs an ANSI 1996 application how to interpret the
	 *  application context information within this object. <p>
	 *
	 *  <b>Note to developers</b> :- Application Context Identifier does not have
	 *  an isPresent method, and can be ignored if no application context is
	 *  present.
	 *
	 *@return    the Application Context Identifier of the Dialogue Portion. The
	 *      Application Context Identifier is initialized to
	 *      "APPLICATION_CONTEXT_INTEGER", therefore if not previously set this
	 *      method will always return this value. An application should call the
	 *      <code>isAppContextNamePresent</code> method before accessing the <code>
	 *      appContextIdentifier</code> .
	 *@since     JAIN TCAP v1.1
	 */

	public int getAppContextIdentifier() {
		return m_appContextIdentifier;
	}



	/**
	 *  Indicates if the Application Context Name parameter is present in this
	 *  Event.
	 *
	 *@return true if Application Context Name has been set, false otherwise.
	 */

	public boolean isAppContextNamePresent() {
		return m_appContextNamePresent;
	}


	/**
	 *  Gets the Application Context Name parameter of the Dialogue Portion.
	 *
	 *@return  the Application Context Name of the Dialogue Portion.
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      parameter has not yet been set
	 */

	public byte[] getAppContextName() throws ParameterNotSetException {
		if (m_appContextNamePresent == true) {
			return m_appContextName;
		}
		else {
			throw new ParameterNotSetException();
		}
	}



	/**
	 *  Gets the Security Context Identifier parameter of the Dialogue Portion.
	 *  This field informs an application how to interpret the Security context
	 *  information within this object. <p>
	 *
	 *  <b>Note to developers</b> :- Security Context Identifier does not have an
	 *  isPresent method, as it can be ignored if no Security context is present.
	 *
	 *@return    the Security Context Identifier of the Dialogue Portion. The
	 *      Security Context Identifier is initialized to "SECURITY_CONTEXT_INTEGER",
         *      therefore if not previously set this method will always return this
         *      value. An application should call the <code>
	 *      isSecurityContextInfoPresent</code> method before accessing the <code>
	 *      securityContextIdentifier</code> .
	 *@since     JAIN TCAP v1.1
	 */

	public int getSecurityContextIdentifier() {
		return m_securityContextIdentifier;
	}



	/**
	 *  Indicates if the Security Context Information parameter is present in this
	 *  Event.
	 *
	 *@return    true if Security Context Information has been set, false otherwise.
	 */
	public boolean isSecurityContextInfoPresent() {
		return m_securityContextInfoPresent;
	}


	/**
	 *  Gets the Security Context Information parameter of the Dialogue Portion.
	 *  The Security Context parameter is only used in an ANSI 1996 implementation
	 *  of Jain Tcap.
	 *
	 *@return the Security Context Information parameter of the Dialogue Portion
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      parameter has not yet been set
	 */

	public byte[] getSecurityContextInfo() throws ParameterNotSetException {
		if (m_securityContextInfoPresent == true) {
			return m_securityContextInformation;
		}
		else {
			throw new ParameterNotSetException();
		}
	}


	/**
	 *  Indicates if the Confidentiality Information parameter is present in this
	 *  Event.
	 *
	 *@return    true if Confidentiality Information has been set, false otherwise.
	 */

	public boolean isConfidentialityInformationPresent() {
		return m_confidentialityInformationPresent;
	}


	/**
	 *  Gets the Confidentiality Information parameter of the Dialogue Portion. The
	 *  Confidentiality Information parameter is only used in an ANSI 1996
	 *  implementation of Jain Tcap.
	 *
	 *@return the Confidentiality Information of the Dialogue Portion.
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      parameter has not been set
	 */

	public byte[] getConfidentialityInformation() throws ParameterNotSetException {
		if (m_confidentialityInformationPresent == true) {
			return m_confidentialityInformation;
		}
		else {
			throw new ParameterNotSetException();
		}
	}


	/**
	 *  Indicates if the User Information parameter is present in this Event.
	 *
	 *@return true if User Information has been set, false otherwise.
	 */

	public boolean isUserInformationPresent() {
		return m_userInformationPresent;
	}


	/**
	 *  Gets the User Information parameter of this Dialogue Portion. User
	 *  Information is the information which can be exchanged between TCAP
	 *  applications independently from the remote operation service.
	 *
	 *@return the User Information.
	 *@exception  ParameterNotSetException  this exception is thrown if this
	 *      parameter has not yet been set
	 */

	public byte[] getUserInformation() throws ParameterNotSetException {
		if (m_userInformationPresent == true) {
			return m_userInformation;
		}
		else {
			throw new ParameterNotSetException();
		}
	}


	/**
	 *  Clears all previously set parameters .
	 */

	public void clearAllParameters() {
		m_protocolVersionPresent = false;
		m_userInformationPresent = false;
		m_appContextNamePresent = false;
		m_confidentialityInformationPresent = false;
		m_securityContextInfoPresent = false;
	}



	/**
	 *  String representation of class DialoguePortion
	 *
	 *@return    String provides description of class DialoguePortion
	 */
	public String toString() {
		StringBuffer buffer = new StringBuffer(500);
		buffer.append("\n\nDialoguePortion");
		buffer.append("\n\nprotocolVersion = ");
		buffer.append(m_protocolVersion);
		buffer.append("\n\nappContextName = ");
		if (m_appContextName != null) {
			buffer.append(m_appContextName.toString());
		}
		else {
			buffer.append("value is null");
		}
		buffer.append("\n\nappContextIdentifier = ");
		buffer.append(m_appContextIdentifier);
		buffer.append("\n\nuserInformation = ");
		if (m_userInformation != null) {
			buffer.append(m_userInformation.toString());
		}
		else {
			buffer.append("value is null");
		}
		buffer.append("\n\nsecurityContextInformation = ");
		if (m_securityContextInformation != null) {
			buffer.append(m_securityContextInformation.toString());
		}
		else {
			buffer.append("value is null");
		}
		buffer.append("\n\nsecurityContextIdentifier = ");
		buffer.append(m_securityContextIdentifier);
		buffer.append("\n\nconfidentialityInformation = ");
		if (m_confidentialityInformation != null) {
			buffer.append(m_confidentialityInformation.toString());
		}
		else {
			buffer.append("value is null");
		}
		buffer.append("\n\nprotocolVersionPresent = ");
		buffer.append(m_protocolVersionPresent);
		buffer.append("\n\nuserInformationPresent = ");
		buffer.append(m_userInformationPresent);
		buffer.append("\n\nappContextNamePresent = ");
		buffer.append(m_appContextNamePresent);
		buffer.append("\n\nsecurityContextInfoPresent = ");
		buffer.append(m_securityContextInfoPresent);
		buffer.append("\n\nconfidentialityInformationPresent = ");
		buffer.append(m_confidentialityInformationPresent);

		return buffer.toString();
	}


	/**
	 *  The Protocol Version parameter of the Dialogue Portion
         *
         *@serial    m_protocolVersion - a default serializable field
	 */

	private int m_protocolVersion = 0;

	/**
	 *  The Application Context Name parameter of the Dialogue Portion
         *
         *@serial    m_appContextName - a default serializable field
	 */

	private byte[] m_appContextName = null;

	/**
	 *  The Application Context Identifier parameter of the Dialogue Portion
         *
         *@serial    m_appContextIdentifier - a default serializable field
	 */

	private int m_appContextIdentifier = DialogueConstants.APPLICATION_CONTEXT_INTEGER;

	/**
	 *  The User Information parameter of the Dialogue Portion
         *
         *@serial    m_userInformation - a default serializable field
	 */

	private byte[] m_userInformation = null;

	/**
	 *  The Security Context Information parameter of the Dialogue Portion
         *
         *@serial    m_securityContextInfo - a default serializable field
	 */

	private byte[] m_securityContextInformation = null;

	/**
	 *  The Security Context Identifier parameter of the Dialogue Portion
         *
         *@serial    m_securityContextIdentifier - a default serializable field
	 */

	private int m_securityContextIdentifier = DialogueConstants.SECURITY_CONTEXT_INTEGER;

	/**
	 *  The Confidentiality Information parameter of the Dialogue Portion
         *
         *@serial    m_confidentialityInformation - a default serializable field
	 */

	private byte[] m_confidentialityInformation = null;

	/**
	 *@serial    m_protocolVersionPresent - a default serializable field
	 */

	private boolean m_protocolVersionPresent = false;

	/**
	 *@serial    m_userInformationPresent - a default serializable field
	 */

	private boolean m_userInformationPresent = false;

	/**
	 *@serial    m_appContextNamePresent - a default serializable field
	 */

	private boolean m_appContextNamePresent = false;

	/**
	 *@serial    m_securityContextInfoPresent - a default serializable field
	 */

	private boolean m_securityContextInfoPresent = false;

	/**
	 *@serial    m_confidentialityInformationPresent - a default serializable field
	 */

	private boolean m_confidentialityInformationPresent = false;



}

