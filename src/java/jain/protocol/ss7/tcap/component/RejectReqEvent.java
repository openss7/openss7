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
  * An event representing a TCAP Reject request component primitive. The
  * mandatory parameters of this primitive are supplied to the
  * constructor. Optional parameters may then be set using the set
  * methods.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public final class RejectReqEvent extends ComponentReqEvent {
    /** Constructs a new Reject request Event, with only the Event
      * Source and the JAIN TCAP Mandatory parameters being supplied to
      * the constructor.
      * @param source
      * The Event Source supplied to the construcutor.
      * @param dialogueId
      * The Dialogue Identifier supplied to the construcutor.
      * @param problemType
      * The Problem Type supplied to the construcutor.
      * @param problem
      * The Problem supplied to the construcutor.  */
    public RejectReqEvent(java.lang.Object source, int dialogueId,
            int problemType, int problem) {
        super(source);
        setDialogueId(dialogueId);
        setProblemType(problemType);
        setProblem(problem);
    }
    /** @deprecated
      * As of JAIN TCAP v1.1, only one type of Reject Request for each variant.  */
    public void setRejectType(int rejectType) {
        m_rejectType = rejectType;
        m_rejectTypePresent = true;
    }
    /** Sets the Parameters' parameter of the Reject Request Component.
      * @param parameters
      * The new Parameters value.  */
    public void setParameters(Parameters parameters) {
        m_parameters = parameters;
        m_parametersPresent = true;
    }
    /** Sets the problem type of this Reject request Component.
      * @param problemType
      * The new Problem Type value, one of the following:- <ul>
      * <li>PROBLEM_TYPE_GENERAL a problem that does not relate to any
      * specific component type.
      * <li>PROBLEM_TYPE_INVOKE a problem that relates only to the
      * invoke component type.
      * <li>PROBLEM_TYPE_RETURN_RESULT a problem that relates to the
      * return result component type.
      * <li>PROBLEM_TYPE_RETURN_ERROR a problem that relates only to the
      * return error component.
      * <li>PROBLEM_TYPE_DIALOGUE a problem specific to a Dialogue
      * primitive. </ul> */
    public void setProblemType(int problemType) {
        m_problemType = problemType;
        m_problemTypePresent = true;
    }
    /** Sets the problem details of this Reject request Component.
      * @param problem
      * One of the following problem codes, grouped by Problem type:
      * <ul>
      * <li>PROBLEM_TYPE_GENERAL <ol>
      *     <li>PROBLEM_CODE_BADLY_STRUCTURED_COMPONENT
      *     <li>PROBLEM_CODE_MISTYPED_COMPONENT
      *     <li>PROBLEM_CODE_UNRECOGNISED_COMPONENT </ol>
      * <li>PROBLEM_TYPE_INVOKE <ol>
      *     <li>PROBLEM_CODE_DUPLICATE_INVOKE_ID
      *     <li>PROBLEM_CODE_INITIATING_RELEASE
      *     <li>PROBLEM_CODE_LINKED_RESPONSE_UNEXPECTED
      *     <li>PROBLEM_CODE_MISTYPED_PARAMETER
      *     <li>PROBLEM_CODE_RESOURCE_LIMITATION
      *     <li>PROBLEM_CODE_UNEXPECTED_LINKED_OPERATION
      *     <li>PROBLEM_CODE_UNRECOGNIZED_INVOKE_ID
      *     <li>PROBLEM_CODE_UNRECOGNIZED_LINKED_ID
      *     <li>PROBLEM_CODE_UNRECOGNIZED_OPERATION </ol>
      * <li>PROBLEM_TYPE_RETURN_RESULT <ol>
      *     <li>PROBLEM_CODE_MISTYPED_PARAMETER 
      *     <li>PROBLEM_CODE_RETURN_RESULT_UNEXPECTED 
      *     <li>PROBLEM_CODE_UNRECOGNIZED_INVOKE_ID  </ol>
      * <li>PROBLEM_TYPE_RETURN_ERROR <ol>
      *     <li>PROBLEM_CODE_MISTYPED_PARAMETER
      *     <li>PROBLEM_CODE_RETURN_ERROR_UNEXPECTED
      *     <li>PROBLEM_CODE_UNRECOGNIZED_ERROR
      *     <li>PROBLEM_CODE_UNRECOGNIZED_INVOKE_ID </ol>
      * <li>PROBLEM_TYPE_DIALOGUE <ol>
      *     <li>PROBLEM_CODE_BADLY_STRUCTURED_DIALOGUE
      *     <li>PROBLEM_CODE_INCORRECT_DIALOGUE
      *     <li>PROBLEM_CODE_PERMISSION_TO_RELEASE
      *     <li>PROBLEM_CODE_RESOURCE_UNAVAILABLE
      *     <li>PROBLEM_CODE_UNASSIGNED_RESPONDING_ID
      *     <li>PROBLEM_CODE_UNRECOGNIZED_PACKAGE_TYPE </ol></ul>
      * @see ComponentConstants */
    public void setProblem(int problem) {
        m_problem = problem;
        m_problemPresent = true;
    }
    /** @deprecated
      * As of JAIN TCAP v1.1. No replacement - no function for parameter.  */
    public void setLinkId(int linkId) {
        m_linkId = linkId;
        m_linkIdPresent = true;
    }
    /** Indicates if the Invoke Id is present in this Event.
      * @return
      * True if Invoke Id has been set, false otherwise.  */
    public boolean isInvokeIdPresent() {
        return m_invokeIdPresent;
    }
    /** @deprecated
      * As of JAIN TCAP v1.1, only one type of Reject Request for each variant.  */
    public boolean isRejectTypePresent() {
        return m_rejectTypePresent;
    }
    /** @deprecated
      * As of JAIN TCAP v1.1, only one type of Reject Request for each variant.  */
    public int getRejectType()
        throws ParameterNotSetException {
        if (m_rejectTypePresent)
            return m_rejectType;
        throw new ParameterNotSetException("Reject Type: not set.");
    }
    /** Indicates if the 'Parameters' field is present in this Event.
      * @return
      * True if 'Parameters' has been set, false otherwise.  */
    public boolean isParametersPresent() {
        return m_parametersPresent;
    }
    /** Gets the Parameters' parameter of the Reject request Component.
      * Parameters contains any parameters that accompany an operation
      * or that are provided in reply to an operation.
      * @return
      * The Parameters of the Reject request Component.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public Parameters getParameters()
        throws ParameterNotSetException {
        if (m_parametersPresent)
            return m_parameters;
        throw new ParameterNotSetException("Parameters: not set.");
    }
    /** Returns the problem type of this Reject request Component.
      * @return
      * One of the following: <ul>
      * <li>PROBLEM_TYPE_GENERAL a problem that does not relate to any
      * specific component type.
      * <li>PROBLEM_TYPE_INVOKE a problem that relates only to the
      * invoke component type.
      * <li>PROBLEM_TYPE_RETURN_RESULT a problem that relates to the
      * return result component type.
      * <li>PROBLEM_TYPE_RETURN_ERROR a problem that relates only to the
      * return error component.
      * <li>PROBLEM_TYPE_DIALOGUE a problem specific to a Dialogue
      * primitive. </ul>
      * @exception MandatoryParameterNotSetException
      * This exception is thrown if this JAIN Mandatory parameter has
      * not yet been set.  */
    public int getProblemType()
        throws MandatoryParameterNotSetException {
        if (m_problemTypePresent)
            return m_problemType;
        throw new MandatoryParameterNotSetException("Problem Type: not set.");
    }
    /** Returns the problem details of this Reject request Component.
      * @return
      * One of the following problem codes, grouped by Problem type:
      * <ul>
      * <li>PROBLEM_TYPE_GENERAL <ol>
      *     <li>PROBLEM_CODE_BADLY_STRUCTURED_COMPONENT
      *     <li>PROBLEM_CODE_MISTYPED_COMPONENT
      *     <li>PROBLEM_CODE_UNRECOGNISED_COMPONENT </ol>
      * <li>PROBLEM_TYPE_INVOKE <ol>
      *     <li>PROBLEM_CODE_DUPLICATE_INVOKE_ID
      *     <li>PROBLEM_CODE_INITIATING_RELEASE
      *     <li>PROBLEM_CODE_LINKED_RESPONSE_UNEXPECTED
      *     <li>PROBLEM_CODE_MISTYPED_PARAMETER
      *     <li>PROBLEM_CODE_RESOURCE_LIMITATION
      *     <li>PROBLEM_CODE_UNEXPECTED_LINKED_OPERATION
      *     <li>PROBLEM_CODE_UNRECOGNIZED_INVOKE_ID
      *     <li>PROBLEM_CODE_UNRECOGNIZED_LINKED_ID
      *     <li>PROBLEM_CODE_UNRECOGNIZED_OPERATION </ol>
      * <li>PROBLEM_TYPE_RETURN_RESULT <ol>
      *     <li>PROBLEM_CODE_MISTYPED_PARAMETER 
      *     <li>PROBLEM_CODE_RETURN_RESULT_UNEXPECTED 
      *     <li>PROBLEM_CODE_UNRECOGNIZED_INVOKE_ID  </ol>
      * <li>PROBLEM_TYPE_RETURN_ERROR <ol>
      *     <li>PROBLEM_CODE_MISTYPED_PARAMETER
      *     <li>PROBLEM_CODE_RETURN_ERROR_UNEXPECTED
      *     <li>PROBLEM_CODE_UNRECOGNIZED_ERROR
      *     <li>PROBLEM_CODE_UNRECOGNIZED_INVOKE_ID </ol>
      * <li>PROBLEM_TYPE_DIALOGUE <ol>
      *     <li>PROBLEM_CODE_BADLY_STRUCTURED_DIALOGUE
      *     <li>PROBLEM_CODE_INCORRECT_DIALOGUE
      *     <li>PROBLEM_CODE_PERMISSION_TO_RELEASE
      *     <li>PROBLEM_CODE_RESOURCE_UNAVAILABLE
      *     <li>PROBLEM_CODE_UNASSIGNED_RESPONDING_ID
      *     <li>PROBLEM_CODE_UNRECOGNIZED_PACKAGE_TYPE </ol></ul>
      * @exception ParameterNotSetException
      * This exception is thrown if this Mandatory parameter has not
      * been set.  */
    public int getProblem()
        throws ParameterNotSetException {
        if (m_problemPresent)
            return m_problem;
        throw new ParameterNotSetException("Problem: not set.");
    }
    /** @deprecated
      * As of JAIN TCAP v1.1. No replacement - no function for parameter.  */
    public boolean isLinkIdPresent() {
        return m_linkIdPresent;
    }
    /** @deprecated
      * As of JAIN TCAP v1.1. No replacement - no function for parameter.  */
    public int getLinkId()
        throws ParameterNotSetException {
        if (m_linkIdPresent)
            return m_linkId;
        throw new ParameterNotSetException("Link Id: not set.");
    }
    /** This method returns the type of this primitive.
      * @return
      * The Primitive Type of this Event.  */
    public int getPrimitiveType() {
        return jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_REJECT;
    }
    /** Clears all previously set parameters.  */
    public void clearAllParameters() {
        m_dialogueIdPresent = false;
        m_invokeIdPresent = false;
        m_linkIdPresent = false;
        m_parametersPresent = false;
        m_problemPresent = false;
        m_problemTypePresent = false;
        m_rejectTypePresent = false;
    }
    /** String representation of class RejectReqEvent.
      * @return
      * String provides description of class RejectReqEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\n\nRejectreqEvent");
        b.append(super.toString());
        b.append("\n\tm_parameters = ");
        if (m_parameters != null)
            b.append(m_parameters.toString());
        b.append("\n\tm_problemType = " + m_problemType);
        b.append("\n\tm_problem = " + m_problem);
        b.append("\n\tm_rejectType = " + m_rejectType);
        b.append("\n\tm_parametersPresent = " + m_parametersPresent);
        b.append("\n\tm_problemTypePresent = " + m_problemTypePresent);
        b.append("\n\tm_problemPresent = " + m_problemPresent);
        b.append("\n\tm_rejectTypePresent = " + m_rejectTypePresent);
        return b.toString();
    }
    /** The Parameters' parameter of the Reject request Component.
      * @serial m_parameters
      * - a default serializable field.  */
    private Parameters m_parameters = null;
    /** The Link Id parameter of the Reject request component.
      * @serial m_linkId
      * - a default serializable field.  */
    private int m_linkId = 0;
    /** The Problem Type parameter of the Reject request Component.
      * @serial m_problemType
      * - a default serializable field.  */
    private int m_problemType = 0;
    /** The Problem parameter of the Reject request Component.
      * @serial m_problem
      * - a default serializable field.  */
    private int m_problem = 0;
    /** The Reject Type of the Reject request Component, which is defaulted to REJECT_TYPE_LOCAL.
      * @serial m_rejectType
      * - a default serializable field.  */
    private int m_rejectType = 1;
    /** Whether Link Id is present.
      * @serial m_linkIdPresent
      * - a default serializable field.  */
    private boolean m_linkIdPresent = false;
    /** Whether Parameters are present.
      * @serial m_parametersPresent
      * - a default serializable field.  */
    private boolean m_parametersPresent = false;
    /** Whether Problem Type is present.
      * @serial m_problemTypePresent
      * - a default serializable field.  */
    private boolean m_problemTypePresent = false;
    /** Whether Problem is present.
      * @serial m_problemPresent
      * - a default serializable field.  */
    private boolean m_problemPresent = false;
    /** Whether Reject Type is present.
      * @serial m_rejectTypePresent
      * - a default serializable field.  */
    private boolean m_rejectTypePresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
