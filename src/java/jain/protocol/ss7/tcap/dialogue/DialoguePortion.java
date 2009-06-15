/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date$ by $Author$
 */

package jain.protocol.ss7.tcap.dialogue;

import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This class represents the optional Dialogue Portion of a dialogue
  * handling primitive. An ITU 1992 or ITU 1996 implementation of the
  * JAIN TCAP API should only use the Application Context and User
  * Information parameters within this class. An ANSI 1992
  * implementation does not include a Dialogue Portion therefore should
  * not use this class and an ANSI 1996 implementation may use all the
  * parameters within this class.
  * @version 1.2.2
  * @author Monavacon
  */
public final class DialoguePortion implements java.io.Serializable {
    /** Constructs a new DialoguePortion.  */
    public DialoguePortion() {
    }
    /** Sets the Protocol Version parameter of the Dialogue Portion.
      * The protocol version is only specific to an ANSI 1996
      * implementation of the ANSI 1996 specification. A Begin or UNI
      * Request event or the first backwards Continue or End event may
      * optionally include a protocol version.
      * @param protocolVersion
      * This can only have the following value until futher releases of
      * the ANSI TCAP Specification: <ul>
      * <li>DP_PROTOCOL_VERSION_ANSI_96 </ul> */
    public void setProtocolVersion(int protocolVersion) {
        m_protocolVersion = protocolVersion;
        m_protocolVersionPresent = true;
    }
    /** Sets the Application Context Identifier parameter of the
      * Dialogue Portion. This field is specific only to an ANSI 1996
      * implementation of the JAIN TCAP API, as ITU has only one value
      * for the Application Context Tag. This field informs an
      * application how to interpret the application context information
      * within this object. This can have the following values: <ul>
      * <li>APPLICATION_CONTEXT_INTEGER
      * <li>APPLICATION_CONTEXT_OBJECT </ul>
      * <em>Note to Developers:-</em> The Application Context Identifier
      * is defaulted to "APPLICATION_CONTEXT_INTEGER".
      * @param appContextIdentifier
      * The new Application Context Identifier.
      * @since JAIN TCAP v1.1 */
    public void setAppContextIdentifier(int appContextIdentifier) {
        m_appContextIdentifier = appContextIdentifier;
    }
    /** Sets the Application Context Name parameter of the Dialogue
      * Portion. The contents of the byte[] are not inspected by TCAP.
      * A Begin, UNI or UserAbort Request event or the first backwards
      * Continue or End event may optionally include an application
      * context.
      * @param appContextName
      * The new Application Context Name value.  */
    public void setAppContextName(byte[] appContextName) {
        m_appContextName = appContextName;
        m_appContextNamePresent = true;
    }
    /** Sets the Security Context Identifier parameter of the Dialogue
      * Portion. This field informs an application how to interpret the
      * Security context information within this object. This can have
      * the following values: <ul>
      * <li>SECURITY_CONTEXT_INTEGER
      * <li>SECURITY_CONTEXT_OBJECT </ul>
      * <em>Note to Developers:-</em> The Security Context Identifier is
      * defaulted to "SECURITY_CONTEXT_INTEGER".
      * @param securityContextIdentifier
      * The new Security Context Identifier value.  */
    public void setSecurityContextIdentifier(int securityContextIdentifier) {
        m_securityContextIdentifier = securityContextIdentifier;
    }
    /** Sets the Security Context Information parameter of the Dialogue
      * Portion. The security context byte[] is not inspected by TCAP.
      * The Security Context parameter is only used in an ANSI 1996
      * implementation of Jain Tcap.
      * @param securityContextInformation
      * The new Security Context Information.  */
    public void setSecurityContextInfo(byte[] securityContextInformation) {
        m_securityContextInformation = securityContextInformation;
        m_securityContextInfoPresent = true;
    }
    /** Sets the Confidentiality Information parameter of the Dialogue
      * Portion. The Confidentiality parameter is only used in an ANSI
      * 1996 implementation of Jain Tcap, the parameters byte[] is not
      * inspected by TCAP.
      * @param confidentialityInfo
      * The new Confidentiality Information.  */
    public void setConfidentialityInformation(byte[] confidentialityInfo) {
        m_confidentialityInformation = confidentialityInfo;
        m_confidentialityInformationPresent = true;
    }
    /** Sets the User Information parameter of of this Dialogue Portion.
      * @param userInformation
      * The new User Information.  */
    public void setUserInformation(byte[] userInformation) {
        m_userInformation = userInformation;
        m_userInformationPresent = true;
    }
    /** Indicates if the Protocol Version parameter is present in this
      * Event. This method is only used by an ANSI 1996 implementation
      * of the Jain Tcap Specification.
      * @return
      * True if Protocol Version has been set, false otherwise.  */
    public boolean isProtocolVersionPresent() {
        return m_protocolVersionPresent;
    }
    /** Gets the Protocol Version parameter of the Dialogue Portion.
      * The protocol version of the dialogue portion is only specific to
      * an ANSI 1996 implementation of the Jain Tcap specification.
      * @return
      * The Protocol Version of the Dialogue Portion. This can only have
      * the following value until futher releases of the ANSI TCAP
      * Specification: <ul>
      * <li>DP_PROTOCOL_VERSION_ANSI_96 </ul>
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public int getProtocolVersion()
        throws ParameterNotSetException {
        if (m_protocolVersionPresent)
            return m_protocolVersion;
        throw new ParameterNotSetException("Protocol Version: not set.");
    }
    /** Gets the Application Context Identifier parameter of the
      * Dialogue Portion. This field informs an ANSI 1996 application
      * how to interpret the application context information within this
      * object. <p>
      * <em>Note to developers:-</em> Application Context Identifier
      * does not have an isPresent method, and can be ignored if no
      * application context is present.
      * @return
      * The Application Context Identifier of the Dialogue Portion. The
      * Application Context Identifier is initialized to
      * "APPLICATION_CONTEXT_INTEGER", therefore if not previously set
      * this method will always return this value. An application should
      * call the isAppContextNamePresent method before accessing the
      * appContextIdentifier.
      * @since JAIN TCAP v1.1 */
    public int getAppContextIdentifier() {
        return m_appContextIdentifier;
    }
    /** Indicates if the Application Context Name parameter is present
      * in this Event.
      * @return
      * True if Application Context Name has been set, false otherwise.  */
    public boolean isAppContextNamePresent() {
        return m_appContextNamePresent;
    }
    /** Gets the Application Context Name parameter of the Dialogue
      * Portion.
      * @return
      * The Application Context Name of the Dialogue Portion.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.
      */
    public byte[] getAppContextName()
        throws ParameterNotSetException {
        if (m_appContextNamePresent)
            return m_appContextName;
        throw new ParameterNotSetException("Application Context Name: not set.");
    }
    /** Gets the Security Context Identifier parameter of the Dialogue
      * Portion. This field informs an application how to interpret the
      * Security context information within this object. <p>
      * <em>Note to developers:-</em> Security Context Identifier does
      * not have an isPresent method, as it can be ignored if no
      * Security context is present.
      * @return
      * The Security Context Identifier of the Dialogue Portion.  The
      * Security Context Identifier is initialized to
      * "SECURITY_CONTEXT_INTEGER", therefore if not previously set this
      * method will always return this value. An application should call
      * the isSecurityContextInfoPresent method before accessing the
      * securityContextIdentifier.
      * @since JAIN TCAP v1.1 */
    public int getSecurityContextIdentifier() {
        return m_securityContextIdentifier;
    }
    /** Indicates if the Security Context Information parameter is
      * present in this Event.
      * @return
      * True if Security Context Information has been set, false
      * otherwise.  */
    public boolean isSecurityContextInfoPresent() {
        return m_securityContextInfoPresent;
    }
    /** Gets the Security Context Information parameter of the Dialogue
      * Portion. The Security Context parameter is only used in an ANSI
      * 1996 implementation of Jain Tcap.
      * @return
      * The Security Context Information parameter of the Dialogue
      * Portion.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.
      */
    public byte[] getSecurityContextInfo()
        throws ParameterNotSetException {
        if (m_securityContextInfoPresent)
            return m_securityContextInformation;
        throw new ParameterNotSetException("Security Context: not set.");
    }
    /** Indicates if the Confidentiality Information parameter is
      * present in this Event.
      * @return
      * True if Confidentiality Information has been set, false
      * otherwise.  */
    public boolean isConfidentialityInformationPresent() {
        return m_confidentialityInformationPresent;
    }
    /** Gets the Confidentiality Information parameter of the Dialogue
      * Portion. The Confidentiality Information parameter is only used
      * in an ANSI 1996 implementation of Jain Tcap.
      * @return
      * The Confidentiality Information of the Dialogue Portion.
      * @exception
      * ParameterNotSetException
      * This exception is thrown if this parameter has not been set.
      */
    public byte[] getConfidentialityInformation()
        throws ParameterNotSetException {
        if (m_confidentialityInformationPresent)
            return m_confidentialityInformation;
        throw new ParameterNotSetException("Confidentiality Information: not set.");
    }
    /** Indicates if the User Information parameter is present in this
      * Event.
      * @return
      * True if User Information has been set, false otherwise.  */
    public boolean isUserInformationPresent() {
        return m_userInformationPresent;
    }
    /** Gets the User Information parameter of this Dialogue Portion.
      * User Information is the information which can be exchanged
      * between TCAP applications independently from the remote
      * operation service.
      * @return
      * The User Information.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.
      */
    public byte[] getUserInformation()
        throws ParameterNotSetException {
        if (m_userInformationPresent)
            return m_userInformation;
        throw new ParameterNotSetException("User Information: not set.");
    }
    /** Clears all previously set parameters.  */
    public void clearAllParameters() {
        m_protocolVersionPresent = false;
        m_userInformationPresent = false;
        m_appContextNamePresent = false;
        m_confidentialityInformationPresent = false;
        m_securityContextInfoPresent = false;
    }
    /** String representation of class DialoguePortion.
      * @return
      * String provides description of class DialoguePortion.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\n\nDialoguePortion");
        b.append("\n\tm_protocolVersion = " + m_protocolVersion);
        b.append("\n\tm_appContextName = ");
        if (m_appContextName != null)
            b.append(m_appContextName.toString());
        b.append("\n\tm_appContextIdentifier = " + m_appContextIdentifier);
        b.append("\n\tm_userInformation = ");
        if (m_userInformation != null)
            b.append(m_userInformation.toString());
        b.append("\n\tm_securityContextInformation = ");
        if (m_securityContextInformation != null)
            b.append(m_securityContextInformation.toString());
        b.append("\n\tm_securityContextIdentifier = " + m_securityContextIdentifier);
        b.append("\n\tm_confidentialityInformation = ");
        if (m_confidentialityInformation != null)
            b.append(m_confidentialityInformation.toString());
        b.append("\n\tm_protocolVersionPresent = " + m_protocolVersionPresent);
        b.append("\n\tm_userInformationPresent = " + m_userInformationPresent);
        b.append("\n\tm_appContextNamePresent = " + m_appContextNamePresent);
        b.append("\n\tm_securityContextInfoPresent = " + m_securityContextInfoPresent);
        b.append("\n\tm_confidentialityInformationPresent = " + m_confidentialityInformationPresent);
        return b.toString();
    }
    /** The Protocol Version parameter of the Dialogue Portion.
      * @serial m_protocolVersion
      * - a default serializable field.  */
    private int m_protocolVersion = 0;
    /** The Application Context Name parameter of the Dialogue Portion.
      * @serial m_appContextName
      * - a default serializable field.  */
    private byte[] m_appContextName = null;
    /** The Application Context Identifier parameter of the Dialogue Portion.
      * @serial m_appContextIdentifier
      * - a default serializable field.  */
    private int m_appContextIdentifier = DialogueConstants.APPLICATION_CONTEXT_INTEGER;
    /** The User Information parameter of the Dialogue Portion.
      * @serial m_userInformation
      * - a default serializable field.  */
    private byte[] m_userInformation = null;
    /** The Security Context Information parameter of the Dialogue Portion.
      * @serial m_securityContextInfo
      * - a default serializable field.  */
    private byte[] m_securityContextInformation = null;
    /** The Security Context Identifier parameter of the Dialogue Portion.
      * @serial m_securityContextIdentifier
      * - a default serializable field.  */
    private int m_securityContextIdentifier = DialogueConstants.SECURITY_CONTEXT_INTEGER;
    /** The Confidentiality Information parameter of the Dialogue Portion.
      * @serial m_confidentialityInformation
      * - a default serializable field.  */
    private byte[] m_confidentialityInformation = null;
    /** Whether Protocol Version is present.
      * @serial m_protocolVersionPresent
      * - a default serializable field.  */
    private boolean m_protocolVersionPresent = false;
    /** Whether User Information is present.
      * @serial m_userInformationPresent
      * - a default serializable field.  */
    private boolean m_userInformationPresent = false;
    /** Whether Application Context Name is present.
      * @serial m_appContextNamePresent
      * - a default serializable field.  */
    private boolean m_appContextNamePresent = false;
    /** Whether Security Context Information is present.
      * @serial m_securityContextInfoPresent
      * - a default serializable field */
    private boolean m_securityContextInfoPresent = false;
    /** Whether Confidentiality Information is present.
      * @serial m_confidentialityInformationPresent
      * - a default serializable field.  */
    private boolean m_confidentialityInformationPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
