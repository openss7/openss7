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
 * File Name     : InvokeIndEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     14/11/2000  Phelim O'Doherty    Deprecated the get/setLinkId methods
 *                                         and replaced with get/setLinkedId
 *                                         methods.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap.component;

import jain.*;
import jain.protocol.ss7.tcap.*;

/**
 * An event representing a TCAP Invoke indication component primitive. The
 * mandatory parameters of this primitive are supplied to the constructor.
 * Optional parameters may then be set using the set methods.
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 * @see        ComponentIndEvent
 */
public final class InvokeIndEvent extends ComponentIndEvent {

    /**
    * Constructs a new Invoke indication Event, with only the Event Source and
    * the <a href="package-summary.html">JAIN TCAP Mandatory</a> parameters being
    * supplied to the constructor.
    *
    * @param  source         the Event Source supplied to the constructor
    * @param  operation      the Operation supplied to the constructor
    * @param  lastComponent  the Last Component supplied to the constructor
    */
    public InvokeIndEvent(Object source, Operation operation, boolean lastComponent) {

        super(source);
        setOperation(operation);
        setLastComponent(lastComponent);
    }

    /**
    * The InvokeLastEvent parameter determines whether an INVOKE_L or an
    * INVOKE_NL component is being sent through the API. It is defaulted to false
    * that is an INVOKE_NL component, in order to send an INVOKE_L component the
    * InvokeLastEvent parameter must be set to true. <P>
    *
    * <B>Note to developers:</b> This field is not used by an ITU implementation,
    * however it is mandatory for ANSI implementation.
    *
    * @param  lastInvokeEvent  determines if this is an INVOKE_L or INVOKE_NL for an
    *        ANSI implementation of the JAIN TCAP API.
    * @parameter lastInvoke - one of the following:
    *      <UL>
    *        <LI> true if this is an ANSI INVOKE_L component
    *        <LI> false if this is an ANSI INVOKE_NL component
    *      </UL>
    *
    */
    public void setLastInvokeEvent(boolean lastInvokeEvent) {

        m_lastInvokeEvent = lastInvokeEvent;
    }

    /**
    * @deprecated    As of JAIN TCAP v1.1. Use the {@link #setLinkedId(int)
    * setLinkedId} method instead.
    */
    public void setLinkId(int value) {

        m_linkedId = value;
        m_linkedIdPresent = true;
    }

    /**
    * Sets the linked Id parameter of the INVOKE indication component. The linked
    * Id parameter links an operation invocation to a previous operation
    * invocation by the remote TCAP application. <b>Note to developers</b> :- The
    * mapping of the Linked Identifier is as follows:
    *  <UL>
    *    <LI> The Linked Identifier is the Linked Identifier as specified in the
    *    ITU recommendation for the INVOKE component.
    *    <LI> The Linked Identifier is the Correlation Identifier as specified in
    *    the ANSI recommendation for the INVOKE_L and INVOKE_NL components.
    *  </UL>
    *
    *
    * @param  linkedId The new Linked Identifier value
    */
    public void setLinkedId(int linkedId) {

        setLinkId(linkedId);
    }

    /**
    * Sets the Operation parameter of the Invoke indication Component
    *
    * @param  operation The new Operation value
    */
    public void setOperation(Operation operation) {

        m_operation = operation;
        m_operationPresent = true;
    }

    /**
    * Sets the Parameters' parameter of the Invoke indication Component.
    *
    * @param  params  The new Parameters value
    */
    public void setParameters(Parameters params) {

        m_parameters = params;
        m_parametersPresent = true;
    }

    /**
    * Gets the Dialogue Id parameter of this Invoke Indication, which is a
    * reference identifier which identifies a specific dialogue or transaction
    * and all associated components within that dialogue or transaction.
    *
    * @return the Dialogue Id of this Invoke indication
    * @exception  ParameterNotSetException if this JAIN Optional parameter has not
    * been set.
    */
    public int getDialogueId() throws ParameterNotSetException {
        if (m_dialogueIdPresent == true) {
            return (m_dialogueId);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * Gets the Invoke Id parameter of this invoke indication. Invoke Id
    * identifies the operation invocation and its result. The mapping rules for
    * the Invoke Identifier in an INVOKE Component are:- <BR>
    * The Invoke Identifier is the Invoke Identifier as specified in both the ITU
    * and ANSI recommendations for the Invoke component.
    *
    * @return the Invoke Id of the invoke indication event
    * @exception  ParameterNotSetException  this exception is thrown if this is a
    * JAIN Optional parameter has not been set
    */
    public int getInvokeId() throws ParameterNotSetException {
        if (m_invokeIdPresent == true) {
            return (m_invokeId);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * Indicates if the Dialogue Id is present in this Event.
    *
    * @return true if Dialogue Id has been set, false otherwise.
    */
    public boolean isDialogueIdPresent() {
        return m_dialogueIdPresent;
    }

    /**
    * The InvokeLastEvent parameter determines whether an INVOKE_L or an
    * INVOKE_NL component is being sent through the API for an ANSI
    * implementation of the JAIN TCAP API. <P>
    *
    * <B>Note to developers</B> : This field is not used by an ITU
    * implementation, however it is mandatory for ANSI implementation.
    *
    * @return    one of the following:-
    *      <UL>
    *        <LI> true - if this is an INVOKE_L component
    *        <LI> false - if this is an INVOKE_NL component
    *      </UL>
    *
    */
    public boolean isLastInvokeEvent() {
        return (m_lastInvokeEvent);
    }

    /**
    * @deprecated    As of JAIN TCAP v1.1. Use the {@link #isLinkedIdPresent()
    * isLinkedPresent} method instead.
    */
    public boolean isLinkIdPresent() {
        return m_linkedIdPresent;
    }

    /**
    * @deprecated As of JAIN TCAP v1.1. Use the {@link #getLinkedId() getLinkedId}
    * method instead.
    */
    public int getLinkId() throws ParameterNotSetException {
        if (m_linkedIdPresent == true) {
	    return (m_linkedId);
	} else {
	    throw new ParameterNotSetException();
	}
    }

    /**
    * Indicates if this optional linked Id (correlation Id) parameter is present
    * in this Event.
    *
    * Update v1.1: Added get/setLinkedId/isPresent methods to incur with
    * standards variable naming and clarified mapping between ITU and ANSI standards.
    *
    * @return true if linked Id (correlation Id) has been set, false otherwise.
    */
    public boolean isLinkedIdPresent() {
        return (isLinkIdPresent());
    }

    /**
    * Gets the linked Id parameter of the INVOKE indication component. The linked
    * Id parameter links an operation invocation to a previous operation
    * invocation by the remote TCAP application. <P><b>Note to developers</b> :- The
    * mapping of the Linked Identifier is as follows:
    *  <UL>
    *    <LI> The Linked Identifier is the Linked Identifier as specified in the
    *    ITU recommendation for the INVOKE component.
    *    <LI> The Linked Identifier is the Correlation Identifier as specified in
    *    the ANSI recommendation for the INVOKE_L and INVOKE_NL components.
    *  </UL>
    *
    *
    * @return the linked Id (correlationId) of the INVOKE indication component
    * @exception  ParameterNotSetException  this exception is thrown if this
    * parameter has not been set
    */
    public int getLinkedId() throws ParameterNotSetException {
        try {
            return (getLinkId());
        } catch (ParameterNotSetException e) {
            throw e;
        }
    }

    /**
    * Gets the Operation parameter of the INVOKE indication Component. Operation
    * identifies the action to be executed by the remote application.
    *
    * @return the Operation of the INVOKE indication Component
    * @exception  MandatoryParameterNotSetException  this exception is thrown if
    * this JAIN Mandatory parameter has not been set
    */
    public Operation getOperation() throws MandatoryParameterNotSetException {
        if (m_operationPresent == true) {
            return (m_operation);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * Indicates if the 'Parameters' field is present in this Event.
    *
    * @return true if 'Parameters' has been set, false otherwise.
    */
    public boolean isParametersPresent() {
        return m_parametersPresent;
    }

    /**
    * Gets the Parameters' parameter of the INVOKE indication Component. <code>
    * Parameters</code> contains any parameters that accompany an operation or
    * that are provided in reply to an operation.
    *
    * @return the Parameters of the INVOKE indication Component
    * @exception  ParameterNotSetException  this exception is thrown if this
    * parameter has not been set
    */
    public Parameters getParameters() throws ParameterNotSetException {
        if (m_parametersPresent == true) {
            return (m_parameters);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * This method returns the type of this primitive.
    *
    * @return  The Primitive Type of this event
    */
    public int getPrimitiveType() {
        return (jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_INVOKE);
    }

    /**
    * Clears all previously set parameters .
    */
    public void clearAllParameters() {
        m_dialogueIdPresent = false;
        m_invokeIdPresent = false;
        m_linkedIdPresent = false;
        m_parametersPresent = false;
        m_lastInvokeEvent = false;
        m_lastComponentPresent = false;
        m_operationPresent = false;
    }

    /**
    * String representation of class jain.protocol.ss7.tcap.component.InvokeIndEvent
    *
    * @return    String provides description of class
    * jain.protocol.ss7.tcap.component.InvokeIndEvent
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nInvokeIndEvent");
        buffer.append(super.toString());
        buffer.append("\n\noperation = ");
        if (m_operation != null) {
            buffer.append(m_operation.toString());
        } else {
            buffer.append("value is null");
        }
        buffer.append("\n\nparameters = ");
        if (m_parameters != null) {
            buffer.append(m_parameters.toString());
        } else {
            buffer.append("value is null");
        }
        buffer.append("\n\nlinkedId = ");
        buffer.append(m_linkedId);
        buffer.append("\n\nlinkedIdPresent = ");
        buffer.append(m_linkedIdPresent);
        buffer.append("\n\nparametersPresent = ");
        buffer.append(m_parametersPresent);
        buffer.append("\n\noperationPresent = ");
        buffer.append(m_operationPresent);
        buffer.append("\n\nlastInvokeEvent = ");
        buffer.append(m_lastInvokeEvent);

        return buffer.toString();
    }

    /**
    * The Operation parameter of the Invoke indication Component
    *
    * @serial    m_operation - a default serializable field
    */
    private Operation m_operation = null;

    /**
    * The Parameters' parameter of the Invoke indication Component
    *
    * @serial    m_parameters - a default serializable field
    */
    private Parameters m_parameters = null;

    /**
    * The Linked Id parameter of the Invoke indication component
    *
    * @serial    m_linkedId - a default serializable field
    */
    private int m_linkedId = 0;

    /**
    * @serial    m_linkedIdPresent - a default serializable field
    */
    private boolean m_linkedIdPresent = false;

    /**
    * @serial    m_parametersPresent - a default serializable field
    */
    private boolean m_parametersPresent = false;

    /**
    * @serial    m_operationPresent - a default serializable field
    */
    private boolean m_operationPresent = false;

    /**
    * @serial    m_lastInvokeEvent - a default serializable field
    */
    private boolean m_lastInvokeEvent = false;
}

