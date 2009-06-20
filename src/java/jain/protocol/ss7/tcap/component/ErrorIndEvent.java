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

package jain.protocol.ss7.tcap.component;

import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * An event representing a TCAP Error indication component primitive.
  * The mandatory parameters of this primitive are supplied to the
  * constructor. Optional parameters may then be set using the set
  * methods.
  * @version 1.2.2
  * @author Monavacon Limited
  * @see ComponentIndEvent
  */
public final class ErrorIndEvent extends ComponentIndEvent {
    /** Constructs a new Error indication Event, with only the Event
      * Source and the JAIN TCAP Mandatory parameters being supplied to
      * the constructor.
      * @param source
      * The Event Source supplied to the constructor.
      * @param dialogueId
      * The Dialogue Identifier supplied to the constructor.
      * @param errorType
      * The Error Type supplied to the constructor.
      * @param errorCode
      * The Error Code supplied to the constructor.
      * @param lastComponent
      * The Last Component Flag supplied to the constructor.  */
    public ErrorIndEvent(java.lang.Object source, int dialogueId,
            int errorType, byte[] errorCode, boolean lastComponent) {
        super(source);
        setDialogueId(dialogueId);
        setErrorType(errorType);
        setErrorCode(errorCode);
        setLastComponent(lastComponent);
    }
    /** Sets the Parameters' parameter of the ERROR indication
      * Component.
      * @param params
      * The new Parameters value.  */
    public void setParameters(Parameters parameters) {
        m_parameters = parameters;
        m_parametersPresent = true;
    }
    /** Sets the error type of this ERROR indication Component.
      * @param errorType
      * One of the following: <ul>
      * <li>ERROR_LOCAL - indicates a local or private error;
      * <li>ERROR_GLOBAL - indicates a global or national error. </ul>
      * @see ComponentConstants */
    public void setErrorType(int errorType) {
        m_errorType = errorType;
        m_errorTypePresent = true;
    }
    /** Sets the error code of this ERROR indication Component.
      * @param errorCode
      * The new Error Code value.
      * @see ComponentConstants */
    public void setErrorCode(byte[] errorCode) {
        m_errorCode = errorCode;
        m_errorCodePresent = true;
    }
    /** @deprecated
      * As of JAIN TCAP v1.1. No replacement - no function for parameter.
      */
    public void setLinkId(int linkId) {
        m_linkId = linkId;
        m_linkIdPresent = true;
    }
    /** Gets the Dialogue Id parameter of this Error Indication, which
      * is a reference identifier which identifies a specific dialogue
      * or transaction and all associated components within that
      * dialogue or transaction.
      * @return
      * The Dialogue Id of this Error indication.
      * @exception MandatoryParameterNotSetException
      * Thrown when this JAIN Mandatory parameter has not been set.  */
    public int getDialogueId()
        throws MandatoryParameterNotSetException {
        if (m_dialogueIdPresent)
            return m_dialogueId;
        throw new MandatoryParameterNotSetException("Dialogue Id: not set.");
    }
    /** Gets the Invoke Id parameter of this error indication. Invoke Id
      * identifies an operation invocation and its result. The mapping
      * rules for the Invoke Identifier in an ERROR Component are:- The
      * Invoke Identifier is the Invoke Identifier as specified in the
      * ITU recommendation and the Correlation Identifier as specified
      * in the ANSI recommendation.
      * @return
      * The Invoke Id of the indication event.
      * @exception ParameterNotSetException
      * This exception is thrown if this JAIN Optional parameter has not
      * been set.  */
    public int getInvokeId()
        throws ParameterNotSetException {
        if (m_invokeIdPresent)
            return m_invokeId;
        throw new ParameterNotSetException("Invoke Id: not set.");
    }
    /** Indicates if the Optional 'Parameters' field is present in this
      * Event.
      * @return
      * True if 'Parameters' has been set, false otherwise.  */
    public boolean isParametersPresent() {
        return m_parametersPresent;
    }
    /** Gets the 'Parameters' parameter of the ERROR indication
      * Component. Parameters contains any parameters that accompany an
      * operation or that are provided in reply to an operation.
      * @return
      * The Parameters of the Error indication Component.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public Parameters getParameters()
        throws ParameterNotSetException {
        if (m_parametersPresent)
            return m_parameters;
        throw new ParameterNotSetException("Parameters: not set.");
    }
    /** Returns the error type of this Error indication Component.
      * @return
      * One of the following: <ul>
      * <li>ERROR_LOCAL - indicates a local or private error;
      * <li>ERROR_GLOBAL - indicates a global or national error. </ul>
      * @exception MandatoryParameterNotSetException
      * This exception is thrown if this JAIN Mandatory parameter has
      * not been set.  */
    public int getErrorType()
        throws MandatoryParameterNotSetException {
        if (m_errorTypePresent)
            return m_errorType;
        throw new MandatoryParameterNotSetException("Error Type: not set.");
    }
    /** Returns the error code of this ERROR indication Component.
      * @return
      * The error code.
      * @exception MandatoryParameterNotSetException
      * This exception is thrown if this JAIN Mandatory parameter has
      * not been set.
      * @see ComponentConstants */
    public byte[] getErrorCode()
        throws MandatoryParameterNotSetException {
        if (m_errorCodePresent)
            return m_errorCode;
        throw new MandatoryParameterNotSetException("Error Code: not set.");
    }
    /** @deprecated As of JAIN TCAP v1.1. No replacement - no function
      * for parameter.
      */
    public boolean isLinkIdPresent() {
        return m_linkIdPresent;
    }
    /** @deprecated As of JAIN TCAP v1.1. No replacement - no function
      * for parameter.
      */
    public int getLinkId()
        throws ParameterNotSetException {
        if (m_linkIdPresent)
            return m_linkId;
        throw new ParameterNotSetException("Link Id: not set.");
    }
    /** This method returns the type of this primitive.
      * @return
      * The Primitive Type of this event.  */
    public int getPrimitiveType() {
        return jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_ERROR;
    }
    /** Clears all previously set parameters.  */
    public void clearAllParameters() {
        m_dialogueIdPresent = false;
        m_invokeIdPresent = false;
        m_linkIdPresent = false;
        m_parametersPresent = false;
        m_lastComponentPresent = false;
        m_errorTypePresent = false;
        m_errorCodePresent = false;
    }
    /** java.lang.String representation of class ErrorIndEvent.
      * @return
      * java.lang.String provides description of class ErrorIndEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(500);
        b.append("\n\nErrorIndEvent");
        b.append(super.toString());
        b.append("\n\nm_parameters = ");
        if (m_parameters != null)
            b.append(m_parameters.toString());
        b.append("\n\nm_errorType = ");
        b.append(m_errorType);
        b.append("\n\nm_errorCode = ");
        if (m_errorCode != null)
            for(int i = 0; i < m_errorCode.length; i++){
                b.append(m_errorCode[i]);
                b.append(" ");
            }
        b.append("\n\nm_parametersPresent = ");
        b.append(m_parametersPresent);
        b.append("\n\nm_errorCodePresent = ");
        b.append(m_errorCodePresent);
        b.append("\n\nm_errorTypePresent = ");
        b.append(m_errorTypePresent);
        return b.toString();
    }
    /** The Parameters' parameter of the Error indication Component.
      * @serial m_parameters
      * a default serializable field.  */
    private Parameters m_parameters = null;

    /** The link ID parameter of the Error indication Component.
      * @serial m_linkID
      * a default serializable field.  */
    private int m_linkId = 0;

    /** The Error Type parameter of the Error indication Component.
      * Update v1.1: Deprecated field.
      * @deprecated
      * @serial m_errorType
      * a default serializable field.  */
    private int m_errorType = 0;

    /**
      * The Error Code parameter of the Error indication Component.
      * @serial m_errorCode
      * a default serializable field.  */
    private byte[] m_errorCode = null;

    /** Update v1.1: Deprecated field
      * @deprecated
      * @serial m_linkIdPresent
      * a default serializable field.  */
    private boolean m_linkIdPresent = false;

    /**
      * @serial m_parametersPresent
      * a default serializable field.  */
    private boolean m_parametersPresent = false;

    /**
      * @serial m_errorCodePresent
      * a default serializable field.  */
    private boolean m_errorCodePresent = false;

    /**
      * @serial m_errorTypePresent
      * a default serializable field.  */
    private boolean m_errorTypePresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
