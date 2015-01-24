/*
 @(#) src/java/jain/protocol/ss7/tcap/component/ResultIndEvent.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package jain.protocol.ss7.tcap.component;

import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * An event representing a TCAP Result indication component primitive.
  * The mandatory parameters of this primitive are supplied to the
  * constructor. Optional parameters may then be set using the set
  * methods.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public final class ResultIndEvent extends ComponentIndEvent {
    /** Constructs a new Result indication Event, with only the Event
      * Source and the JAIN TCAP Mandatory parameters being supplied to
      * the constructor.
      * @param source 
      * The Event source supplied to the constructor.
      * @param dialogueId 
      * The Dialogue Identifier supplied to the constructor.
      * @param lastComponent 
      * The Last Component Flag supplied to the constructor.
      * @param lastResultEvent 
      * The Last Result Event Flag supplied to the constructor.  */
    public ResultIndEvent(java.lang.Object source, int dialogueId,
            boolean lastComponent, boolean lastResultEvent) {
        super(source);
        setDialogueId(dialogueId);
        setLastComponent(lastComponent);
        setLastResultEvent(lastResultEvent);
    }
    /** The LastResultEvent parameter determines whether a RESULT_L or a
      * RESULT_NL component is being sent through the API.  Note to
      * developers :- This field is mandatory to both an ITU and ANSI
      * implementation of the JAIN TCAP API (unlike the lastInvokeEvent
      * parameter in the InvokeIndEvent). It is defaulted to false that
      * is a RESULT_NL component, in order to send an RESULT_L component
      * the lastResultEvent parameter must be set to true.
      * @param lastResultEvent
      * The new LastResultEvent value.  */
    public void setLastResultEvent(boolean lastResultEvent) {
        m_lastResultEvent = lastResultEvent;
    }
    /** Sets the Operation parameter of the Result Indication Component.
      * @param operation
      * The new Operation value.  */
    public void setOperation(Operation operation) {
        m_operation = operation;
        m_operationPresent = true;
    }
    /** Sets the Parameters' parameter of the Result Indication
      * Component.
      * @param parameters
      * The new Parameters value.  */
    public void setParameters(Parameters parameters) {
        m_parameters = parameters;
        m_parametersPresent = true;
    }
    /** @deprecated
      * As of JAIN TCAP v1.1. No replacement - no function for parameter.  */
    public void setLinkId(int linkId) {
        m_linkId = linkId;
        m_linkIdPresent = true;
    }
    /** Gets the Dialogue Id parameter of this Result Indication, which
      * is a reference identifier which identifies a specific dialogue
      * or transaction and all associated components within that
      * dialogue or transaction.
      * @return
      * The Dialogue Id of this Result indication.
      * @exception MandatoryParameterNotSetException
      * Thrown when this JAIN Mandatory parameter has not been set.  */
    public int getDialogueId()
        throws MandatoryParameterNotSetException {
        if (m_dialogueIdPresent)
            return m_dialogueId;
        throw new MandatoryParameterNotSetException("Dialogue Id: not set.");
    }
    /** Gets the Invoke Id parameter of this result indication. Invoke
      * Id identifies the an operation invocation and its result. The
      * mapping rules for the Invoke Identifier in an ERROR Component
      * are:- The Invoke Identifier is the Invoke Identifier as
      * specified in the ITU recommendation and the Correlation
      * Identifier as specified in the ANSI recommendation.
      * @return
      * The Invoke Id of the result indication event.
      * @exception ParameterNotSetException
      * This exception is thrown if this is a JAIN Optional parameter
      * has not been set.  */
    public int getInvokeId()
        throws ParameterNotSetException {
        if (m_invokeIdPresent)
            return m_invokeId;
        throw new ParameterNotSetException("Invoke Id: not set.");
    }
    /** The LastResultEvent parameter determines whether a RESULT_L or a
      * RESULT_NL component is being sent through the API.  Note to
      * developers :- This field is mandatory to both an ITU and ANSI
      * implementation of the JAIN TCAP API (unlike the lastInvokeEvent
      * parameter in the InvokeIndEvent). It is defaulted to false that
      * is a RESULT_NL component, in order to send an RESULT_L component
      * the lastResultEvent parameter must be set to true.
      * @return
      * One of the following:- <ul>
      * <li>true - if this is a RESULT_L Component.
      * <li>false - if this is a RESULT_NL Component. </ul> */
    public boolean isLastResultEvent() {
        return m_lastResultEvent;
    }
    /** Indicates if the optional Operation field is present in this
      * Event.
      * @return
      * True if Operation has been set, false otherwise.  */
    public boolean isOperationPresent() {
        return m_operationPresent;
    }
    /** Gets the Operation parameter of the Result indication Component.
      * Operation identifies the action to be executed by the remote
      * application.
      * @return
      * The Operation of the Result indication Component.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public Operation getOperation()
        throws ParameterNotSetException {
        if (m_operationPresent)
            return m_operation;
        throw new ParameterNotSetException("Operation: not set.");
    }
    /** Indicates if the 'Parameters' field is present in this Event.
      * @return
      * True if 'Parameters' has been set, false otherwise.  */
    public boolean isParametersPresent() {
        return m_parametersPresent;
    }
    /** Gets the Parameters' parameter of the Result indication
      * Component. Parameters contains any parameters that accompany an
      * operation or that are provided in reply to an operation.
      * @return
      * The Parameters of the Result indication Component.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public Parameters getParameters()
        throws ParameterNotSetException {
        if (m_parametersPresent)
            return m_parameters;
        throw new ParameterNotSetException("Parameters: not set.");
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
      * The Primitive Type of this event.  */
    public int getPrimitiveType() {
        return jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_RESULT;
    }
    /** Clears all previously set parameters.  */
    public void clearAllParameters() {
        m_dialogueIdPresent = false;
        m_invokeIdPresent = false;
        m_linkIdPresent = false;
        m_operationPresent = false;
        m_parametersPresent = false;
        m_lastResultEvent = false;
        m_lastComponentPresent = false;
    }
    /** java.lang.String representation of class ResultIndEvent.
      * @return
      * java.lang.String provides description of class ResultIndEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\n\nResultIndEvent");
        b.append(super.toString());
        b.append("\n\tm_operation = ");
        if (m_operation != null)
            b.append(m_operation.toString());
        b.append("\n\tm_parameters = ");
        if (m_parameters != null)
            b.append(m_parameters.toString());
        b.append("\n\tm_parametersPresent = " + m_parametersPresent);
        b.append("\n\tm_operationPresent = " + m_operationPresent);
        b.append("\n\tm_lastResultEvent = " + m_lastResultEvent);
        return b.toString();
    }
    /** The Operation parameter of the RESULT indication Component.
      * @serial m_operation
      * - a default serializable field.  */
    private Operation m_operation = null;
    /** The Parameters' parameter of the RESULT indication Component.
      * @serial m_parameters
      * - a default serializable field.  */
    private Parameters m_parameters = null;
    /** The Link Id parameter of the RESULT indication component.
      * @serial m_linkId
      * - a default serializable field.  */
    private int m_linkId = 0;
    /** Whether the Link Id is present.
      * @serial m_linkIdPresent
      * - a default serializable field.  */
    private boolean m_linkIdPresent = false;
    /** Whether Parameters are present.
      * @serial m_parametersPresent
      * - a default serializable field.  */
    private boolean m_parametersPresent = false;
    /** Whether the Operation is present.
      * @serial m_operationPresent
      * - a default serializable field.  */
    private boolean m_operationPresent = false;
    /** Whether the Last Result is present.
      * @serial m_lastResultEvent
      * - a default serializable field.  */
    private boolean m_lastResultEvent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
