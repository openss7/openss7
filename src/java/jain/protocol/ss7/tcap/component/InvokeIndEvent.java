/*
 @(#) $RCSfile: InvokeIndEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:30 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:30 $ by $Author: brian $
 */

package jain.protocol.ss7.tcap.component;

import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * An event representing a TCAP Invoke indication component primitive.
  * The mandatory parameters of this primitive are supplied to the
  * constructor. Optional parameters may then be set using the set
  * methods.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public final class InvokeIndEvent extends ComponentIndEvent {
    /** Constructs a new Invoke indication Event, with only the Event
      * Source and the JAIN TCAP Mandatory parameters being supplied to
      * the constructor.
      * @param source
      * The Event Source supplied to the constructor.
      * @param operation
      * The Operation supplied to the constructor.
      * @param lastComponent
      * The Last Component supplied to the constructor.  */
    public InvokeIndEvent(java.lang.Object source, Operation operation,
            boolean lastComponent) {
        super(source);
        setOperation(operation);
        setLastComponent(lastComponent);
    }
    /** The InvokeLastEvent parameter determines whether an INVOKE_L or
      * an INVOKE_NL component is being sent through the API. It is
      * defaulted to false that is an INVOKE_NL component, in order to
      * send an INVOKE_L component the InvokeLastEvent parameter must be
      * set to true. <p>
      *
      * <em>Note to developers:</em> This field is not used by an ITU
      * implementation, however it is mandatory for ANSI implementation.
      * @param lastInvokeEvent
      * Determines if this is an INVOKE_L or INVOKE_NL for an ANSI
      * implementation of the JAIN TCAP API.  */
    public void setLastInvokeEvent(boolean lastInvokeEvent) {
        m_lastInvokeEvent = lastInvokeEvent;
    }
    /** @deprecated
      * As of JAIN TCAP v1.1. Use the setLinkedId method instead.  */
    public void setLinkId(int linkedId) {
        m_linkedId = linkedId;
        m_linkedIdPresent = true;
    }
    /** Sets the linked Id parameter of the INVOKE indication component.
      * The linked Id parameter links an operation invocation to a
      * previous operation invocation by the remote TCAP application.
      * Note to developers :- The mapping of the Linked Identifier is as
      * follows: <ul>
      * <li>The Linked Identifier is the Linked Identifier as specified
      * in the ITU recommendation for the INVOKE component.
      * <li>The Linked Identifier is the Correlation Identifier as
      * specified in the ANSI recommendation for the INVOKE_L and
      * INVOKE_NL components. </ul>
      * @param linkedId
      * The new Linked Identifier value.  */
    public void setLinkedId(int linkedId) {
        setLinkId(linkedId);
    }
    /** Sets the Operation parameter of the Invoke indication Component.
      * @param operation
      * The new Operation value.  */
    public void setOperation(Operation operation) {
        m_operation = operation;
        m_operationPresent = true;
    }
    /** Sets the Parameters' parameter of the Invoke indication
      * Component.
      * @param parameters
      * The new Parameters value.  */
    public void setParameters(Parameters parameters) {
        m_parameters = parameters;
        m_parametersPresent = true;
    }
    /** Gets the Dialogue Id parameter of this Invoke Indication, which
      * is a reference identifier which identifies a specific dialogue
      * or transaction and all associated components within that
      * dialogue or transaction.
      * @return
      * The Dialogue Id of this Invoke indication.
      * @exception ParameterNotSetException
      * Thrown when this JAIN Optional parameter has not been set.  */
    public int getDialogueId()
        throws ParameterNotSetException {
        if (m_dialogueIdPresent)
            return m_dialogueId;
        throw new ParameterNotSetException("Dialogue Id: not set.");
    }
    /** Gets the Invoke Id parameter of this invoke indication. Invoke
      * Id identifies the operation invocation and its result. The
      * mapping rules for the Invoke Identifier in an INVOKE Component
      * are:- The Invoke Identifier is the Invoke Identifier as
      * specified in both the ITU and ANSI recommendations for the
      * Invoke component.
      * @return
      * The Invoke Id of the invoke indication event.
      * @exception ParameterNotSetException
      * This exception is thrown if this is a JAIN Optional parameter
      * has not been set.  */
    public int getInvokeId()
        throws ParameterNotSetException {
        if (m_invokeIdPresent)
            return m_invokeId;
        throw new ParameterNotSetException("Invoke Id: not set.");
    }
    /** Indicates if the Dialogue Id is present in this Event.
      * @return
      * True if Dialogue Id has been set, false otherwise.  */
    public boolean isDialogueIdPresent() {
        return m_dialogueIdPresent;
    }
    /** The InvokeLastEvent parameter determines whether an INVOKE_L or
      * an INVOKE_NL component is being sent through the API for an ANSI
      * implementation of the JAIN TCAP API. <p>
      * <em>Note to developers:</em>
      * This field is not used by an ITU implementation, however it is
      * mandatory for ANSI implementation.
      * @return
      * One of the following:- <ul>
      * <li>true - if this is an INVOKE_L component;
      * <li>false - if this is an INVOKE_NL component. </ul> */
    public boolean isLastInvokeEvent() {
        return m_lastInvokeEvent;
    }
    /** @deprecated
      * As of JAIN TCAP v1.1. Use the isLinkedPresent method instead.  */
    public boolean isLinkIdPresent() {
        return m_linkedIdPresent;
    }
    /** @deprecated
      * As of JAIN TCAP v1.1. Use the getLinkedId method instead.  */
    public int getLinkId()
        throws ParameterNotSetException {
        if (m_linkedIdPresent)
	    return m_linkedId;
        throw new ParameterNotSetException("Linked Id: not set.");
    }
    /** Indicates if this optional linked Id (correlation Id) parameter
      * is present in this Event. Update v1.1: Added
      * get/setLinkedId/isPresent methods to incur with standards
      * variable naming and clarified mapping between ITU and ANSI
      * standards.
      * @return
      * True if linked Id (correlation Id) has been set, false
      * otherwise.  */
    public boolean isLinkedIdPresent() {
        return isLinkIdPresent();
    }
    /** Gets the linked Id parameter of the INVOKE indication component.
      * The linked Id parameter links an operation invocation to a
      * previous operation invocation by the remote TCAP application.
      * <p>
      * <em>Note to developers:-</em>
      * The mapping of the Linked Identifier is as follows: <ul>
      * <li>The Linked Identifier is the Linked Identifier as specified
      * in the ITU recommendation for the INVOKE component.
      * <li>The Linked Identifier is the Correlation Identifier as
      * specified in the ANSI recommendation for the INVOKE_L and
      * INVOKE_NL components. </ul>
      * @return
      * The linked Id (correlationId) of the INVOKE indication
      * component.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public int getLinkedId()
        throws ParameterNotSetException {
        try {
            return getLinkId();
        } catch (ParameterNotSetException e) {
            throw e;
        }
    }
    /** Gets the Operation parameter of the INVOKE indication Component.
      * Operation identifies the action to be executed by the remote
      * application.
      * @return
      * The Operation of the INVOKE indication Component.
      * @exception MandatoryParameterNotSetException
      * This exception is thrown if this JAIN Mandatory parameter has
      * not been set.  */
    public Operation getOperation()
        throws MandatoryParameterNotSetException {
        if (m_operationPresent)
            return m_operation;
        throw new MandatoryParameterNotSetException("Operation: not set.");
    }
    /** Indicates if the 'Parameters' field is present in this Event.
      * @return
      * True if 'Parameters' has been set, false otherwise.  */
    public boolean isParametersPresent() {
        return m_parametersPresent;
    }
    /** Gets the Parameters' parameter of the INVOKE indication
      * Component. Parameters contains any parameters that accompany an
      * operation or that are provided in reply to an operation.
      * @return
      * The Parameters of the INVOKE indication Component.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public Parameters getParameters()
        throws ParameterNotSetException {
        if (m_parametersPresent)
            return m_parameters;
        throw new ParameterNotSetException("Parameters: not set.");
    }
    /** This method returns the type of this primitive.
      * @return
      * The Primitive Type of this event.  */
    public int getPrimitiveType() {
        return jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_INVOKE;
    }
    /** Clears all previously set parameters.  */
    public void clearAllParameters() {
        m_dialogueIdPresent = false;
        m_invokeIdPresent = false;
        m_linkedIdPresent = false;
        m_parametersPresent = false;
        m_lastInvokeEvent = false;
        m_lastComponentPresent = false;
        m_operationPresent = false;
    }
    /** java.lang.String representation of class
      * jain.protocol.ss7.tcap.component.InvokeIndEvent.
      * @return
      * java.lang.String provides description of class
      * jain.protocol.ss7.tcap.component.InvokeIndEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\n\nInvokeIndEvent");
        b.append(super.toString());
        b.append("\n\tm_operation = ");
        if (m_operation != null)
            b.append(m_operation.toString());
        b.append("\n\tm_parameters = ");
        if (m_parameters != null)
            b.append(m_parameters.toString());
        b.append("\n\tm_linkedId = " + m_linkedId);
        b.append("\n\tm_linkedIdPresent = " + m_linkedIdPresent);
        b.append("\n\tm_parametersPresent = " + m_parametersPresent);
        b.append("\n\tm_operationPresent = " + m_operationPresent);
        b.append("\n\tm_lastInvokeEvent = " + m_lastInvokeEvent);
        return b.toString();
    }
    /** The Operation parameter of the Invoke indication Component.
      * @serial m_operation
      * - a default serializable field.  */
    private Operation m_operation = null;
    /** The Parameters' parameter of the Invoke indication Component.
      * @serial m_parameters
      * - a default serializable field.  */
    private Parameters m_parameters = null;
    /** The Linked Id parameter of the Invoke indication component.
      * @serial m_linkedId
      * - a default serializable field.  */
    private int m_linkedId = 0;
    /** Whether the Linked Id is present.
      * @serial m_linkedIdPresent
      * - a default serializable field.  */
    private boolean m_linkedIdPresent = false;
    /** Whether the Parameters are present.
      * @serial m_parametersPresent
      * - a default serializable field.  */
    private boolean m_parametersPresent = false;
    /** Whether the Operation is present.
      * @serial m_operationPresent
      * - a default serializable field.  */
    private boolean m_operationPresent = false;
    /** Whether the Last Invoke Event is present.
      * @serial m_lastInvokeEvent
      * - a default serializable field.  */
    private boolean m_lastInvokeEvent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
