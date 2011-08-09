/*
 @(#) $RCSfile: InvokeReqEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:30 $ <p>
 
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
  * An event representing a TCAP Invoke request component primitive. The
  * mandatory parameters of this primitive are supplied to the
  * constructor. Optional parameters may then be set using the set
  * methods.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public final class InvokeReqEvent extends ComponentReqEvent {
    /** Constructs a new Invoke Request Event, with only the Event
      * Source and the JAIN TCAP Mandatory parameters being supplied to
      * the constructor.
      * @param source
      * The Event Source supplied to the constructor.
      * @param dialogueId
      * The Dialogue Identifier supplied to the constructor.
      * @param operation
      * The Operation supplied to the constructor.  */
    public InvokeReqEvent(java.lang.Object source, int dialogueId,
            Operation operation) {
        super(source);
        setDialogueId(dialogueId);
        setOperation(operation);
    }
    /** Sets the lastInvoke flag of this Invoke Event. The
      * InvokeLastEvent parameter determines whether an INVOKE_L or an
      * INVOKE_NL component is being sent through the API. It is
      * defaulted to false i.e. an INVOKE_NL component, in order to send
      * an INVOKE_L component the InvokeLastEvent parameter must be set
      * to true. NOTE: This field is not used by ITU implementation,
      * however it is mandatory for ANSI.
      * @param lastInvokeEvent
      * True if this is an INVOKE_L Event, false if this is an INVOKE_NL
      * Event.  */
    public void setLastInvokeEvent(boolean lastInvokeEvent) {
        m_lastInvokeEvent = lastInvokeEvent;
    }
    /** @deprecated
      * As of JAIN TCAP v1.1. Use the setLinkedId method instead.  */
    public void setLinkId(int linkId) {
        m_linkedId = linkId;
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
    /** Sets the Timeout parameter of the INVOKE request component.
      * @param timeOut
      * The new Time Out value.  */
    public void setTimeOut(long timeOut) {
        m_timeOut = timeOut;
        m_timeOutPresent = true;
    }
    /** Sets the Operation parameter of the Invoke request Component.
      * @param operation
      * The new Operation value.  */
    public void setOperation(Operation operation) {
        m_operation = operation;
        m_operationPresent = true;
    }
    /** Sets the Parameters' parameter of the Invoke request Component.
      * @param parameters
      * The new Parameters value.  */
    public void setParameters(Parameters parameters) {
        m_parameters = parameters;
        m_parametersPresent = true;
    }
    /** Sets the Class Type parameter of the INVOKE request component.
      * @param classType
      * One - of the following: <ul>
      * <li>CLASS_1 - Reporting success or failure;
      * <li>CLASS_2 - Reporting failure;
      * <li>CLASS_3 - Reporting success;
      * <li>CLASS_4 - Outcome not reported. </ul> */
    public void setClassType(int classType) {
        m_classType = classType;
        m_classTypePresent = true;
    }
    /** Indicates if the Invoke Id is present in this Event.
      * @return
      * True if Invoke Identifier has been set, false otherwise.  */
    public boolean isInvokeIdPresent() {
        return m_invokeIdPresent;
    }
    /** Indicates if this is the last Invoke Event. The InvokeLastEvent
      * parameter determines whether an INVOKE_L or an INVOKE_NL
      * component is being sent through the API. It is defaulted to
      * false i.e. an INVOKE_NL component, in order to send an INVOKE_L
      * component the InvokeLastEvent parameter must be set to true.
      * NOTE: This field is not used by ITU implementation, however it
      * is mandatory for ANSI.
      * @return
      * True if this is an INVOKE_L Event, false if this is an INVOKE_NL
      * Event.  */
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
        throw new ParameterNotSetException("Link Id: not set.");
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
    /** Indicates if the Timeout field is present in this Event.
      * @return
      * True if Timeout has been set, false otherwise.  */
    public boolean isTimeOutPresent() {
        return m_timeOutPresent;
    }
    /** Gets the Timeout parameter of the INVOKE request component.
      * Timeout indicates the amount of time to wait for a response to
      * an invocation.
      * @return
      * The timeout value in milliseconds.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public long getTimeOut()
        throws ParameterNotSetException {
        if (m_timeOutPresent)
            return m_timeOut;
        throw new ParameterNotSetException("Time Out: not set.");
    }
    /** Gets the Operation parameter of the INVOKE request Component.
      * Operation identifies the action to be executed by the remote
      * application.
      * @return
      * The Operation of the INVOKE request Component.
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
    /** Gets the Parameters' parameter of the INVOKE request Component.
      * Parameters contains any parameters that accompany an operation
      * or that are provided in reply to an operation.
      * @return
      * The Parameters of the INVOKE request Component.
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public Parameters getParameters()
        throws ParameterNotSetException {
        if (m_parametersPresent)
            return m_parameters;
        throw new ParameterNotSetException("Parameters: not set.");
    }
    /** Indicates if the Class type field is present in this Event.
      * @return
      * True if the Class type has been set, false otherwise.  */
    public boolean isClassTypePresent() {
        return m_classTypePresent;
    }
    /** Returns the Component Class type.
      * @return
      * One of the following: <ul>
      * <li>CLASS_1 - Reporting success or failure;
      * <li>CLASS_2 - Reporting failure;
      * <li>CLASS_3 - Reporting success;
      * <li>CLASS_4 - Outcome not reported. </ul>
      * @exception ParameterNotSetException
      * This exception is thrown if this parameter has not yet been set.  */
    public int getClassType()
        throws ParameterNotSetException {
        if (m_classTypePresent)
            return m_classType;
        throw new ParameterNotSetException("Class Type: not set.");
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
        m_timeOutPresent = false;
        m_classTypePresent = false;
        m_operationPresent = false;
        m_lastInvokeEvent = false;
    }
    /** java.lang.String representation of class InvokeReqEvent.
      * @return
      * java.lang.String provides description of class InvokeReqEvent.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\n\nInvokeReqEvent");
        b.append(super.toString());
        b.append("\n\tm_operation = ");
        if (m_operation != null)
            b.append(m_operation.toString());
        b.append("\n\tm_parameters = ");
        if (m_parameters != null)
            b.append(m_parameters.toString());
        b.append("\n\tm_linkedId = " + m_linkedId);
        b.append("\n\tm_timeOut = " + m_timeOut);
        b.append("\n\tm_classType = " + m_classType);
        b.append("\n\tm_linkedIdPresent = " + m_linkedIdPresent);
        b.append("\n\tm_parametersPresent = " + m_parametersPresent);
        b.append("\n\tm_operationPresent = " + m_operationPresent);
        b.append("\n\tm_timeOutPresent = " + m_timeOutPresent);
        b.append("\n\tm_classTypePresent = " + m_classTypePresent);
        b.append("\n\tm_lastInvokeEvent = " + m_lastInvokeEvent);
        return b.toString();
    }
    /** The Operation parameter of the INVOKE request Component.
      * @serial m_operation
      * - a default serializable field.  */
    private Operation m_operation = null;
    /** The Parameters' parameter of the INVOKE request Component.
      * @serial m_parameters
      * - a default serializable field.  */
    private Parameters m_parameters = null;
    /** The Linked Id parameter of the INVOKE request component.
      * @serial m_linkedId
      * - a default serializable field.  */
    private int m_linkedId = 0;
    /** The Time-Out parameter of the INVOKE request component.
      * @serial m_timeOut
      * - a default serializable field.  */
    private long m_timeOut = 0;
    /** The Class Type parameter of the INVOKE request Component.
      * @serial m_classType
      * - a default serializable field.  */
    private int m_classType = 0;
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
    /** Whether the Time Out is present.
      * @serial m_timeOutPresent
      * - a default serializable field.  */
    private boolean m_timeOutPresent = false;
    /** Whether the Class Type is present.
      * @serial m_classTypePresent
      * - a default serializable field.  */
    private boolean m_classTypePresent = false;
    /** Whether the Last Invoke Event is present.
      * @serial m_lastInvokeEvent
      * - a default serializable field.  */
    private boolean m_lastInvokeEvent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
