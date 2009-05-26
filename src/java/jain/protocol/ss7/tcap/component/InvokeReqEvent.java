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
 * File Name     : InvokeReqEvent.java
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
 * An event representing a TCAP Invoke request component primitive. The
 * mandatory parameters of this primitive are supplied to the constructor.
 * Optional parameters may then be set using the set methods.
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 * @see        ComponentReqEvent
 */
public final class InvokeReqEvent extends ComponentReqEvent {

    /**
    * Constructs a new Invoke Request Event, with only the Event Source and the
    * <a href="package-summary.html">JAIN TCAP Mandatory</a> parameters being
    * supplied to the constructor.
    *
    * @param  source      the Event Source supplied to the constructor
    * @param  dialogueId  the Dialogue Identifier supplied to the constructor
    * @param  operation   the Operation supplied to the constructor
    */
    public InvokeReqEvent(Object source, int dialogueId, Operation operation) {

        super(source);
        setDialogueId(dialogueId);
        setOperation(operation);
    }

    /**
    * Sets the lastInvoke flag of this Invoke Event. The InvokeLastEvent
    * parameter determines whether an INVOKE_L or an INVOKE_NL component is being
    * sent through the API. It is defaulted to false i.e. an INVOKE_NL component,
    * in order to send an INVOKE_L component the InvokeLastEvent parameter must
    * be set to true. NOTE: This field is not used by ITU implementation, however
    * it is mandatory for ANSI.
    *
    * @param lastInvoke <B>true</b> if this is an INVOKE_L Event,
    * <B>false</B> if this is an INVOKE_NL Event.
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
    * Sets the Timeout parameter of the INVOKE request component.
    *
    * @param  timeOut The new Time Out value
    */
    public void setTimeOut(long timeOut) {

        m_timeOut = timeOut;
        m_timeOutPresent = true;
    }

    /**
    * Sets the Operation parameter of the Invoke request Component
    *
    * @param  operation The new Operation value
    */
    public void setOperation(Operation operation){

        m_operation = operation;
        m_operationPresent = true;
    }

    /**
    * Sets the Parameters' parameter of the Invoke request Component.
    *
    * @param  params  The new Parameters value
    */
    public void setParameters(Parameters params) {

        m_parameters = params;
        m_parametersPresent = true;
    }

    /**
    * Sets the Class Type parameter of the INVOKE request component.
    *
    * @param  one of the following:
    *    <UL>
    *    <LI>CLASS_1 - Reporting success or failure
    *    <LI>CLASS_2 - Reporting failure
    *    <LI>CLASS_3 - Reporting success
    *    <LI>CLASS_4 - Outcome not reported
    *    </UL>
    */
    public void setClassType(int classType) {

        m_classType = classType;
        m_classTypePresent = true;
    }

    /**
    * Indicates if the Invoke Id is present in this Event.
    *
    * @return    true if Invoke Identifier has been set, false otherwise.
    */
    public boolean isInvokeIdPresent() {
        return m_invokeIdPresent;
    }

    /**
    * Indicates if this is the last Invoke Event. The InvokeLastEvent parameter
    * determines whether an INVOKE_L or an INVOKE_NL component is being sent
    * through the API. It is defaulted to false i.e. an INVOKE_NL component, in
    * order to send an INVOKE_L component the InvokeLastEvent parameter must be
    * set to true. NOTE: This field is not used by ITU implementation, however it
    * is mandatory for ANSI.
    *
    * @return  <B>true</b> if this is an INVOKE_L Event, <B>false</B>
    * if this is an INVOKE_NL Event.
    */
    public boolean isLastInvokeEvent() {
        return (m_lastInvokeEvent);
    }

    /**
    * @deprecated As of JAIN TCAP v1.1. Use the {@link #isLinkedIdPresent()
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
    * invocation by the remote TCAP application. <p><b>Note to developers</b> :- The
    * mapping of the Linked Identifier is as follows:
    * <UL>
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
    * Indicates if the Timeout field is present in this Event.
    *
    * @return    true if Timeout has been set, false otherwise.
    */
    public boolean isTimeOutPresent() {
        return m_timeOutPresent;
    }

    /**
    * Gets the Timeout parameter of the INVOKE request component. Timeout
    * indicates the amount of time to wait for a response to an invocation.
    *
    * @return  the timeout value in milliseconds
    * @exception  ParameterNotSetException  this exception is thrown if this
    * parameter has not been set
    */
    public long getTimeOut() throws ParameterNotSetException {
        if (m_timeOutPresent == true) {
            return (m_timeOut);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * Gets the Operation parameter of the INVOKE request Component. .Operation
    * identifies the action to be executed by the remote application.
    *
    * @return the Operation of the INVOKE request Component
    * @exception  MandatoryParameterNotSetException  this exception is thrown if
    *      this JAIN Mandatory parameter has not been set
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
    * Gets the Parameters' parameter of the INVOKE request Component. <code>
    * Parameters</code> contains any parameters that accompany an operation or
    * that are provided in reply to an operation.
    *
    * @return the Parameters of the INVOKE request Component
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
    * Indicates if the Class type field is present in this Event.
    *
    * @return true if the Class type has been set, false otherwise.
    */
    public boolean isClassTypePresent() {
        return m_classTypePresent;
    }

    /**
    * Returns the Component Class type.
    *
    * @return one of the following:
    *    <UL>
    *    <LI>CLASS_1 - Reporting success or failure
    *    <LI>CLASS_2 - Reporting failure
    *    <LI>CLASS_3 - Reporting success
    *    <LI>CLASS_4 - Outcome not reported
    *    </UL>
    * @exception  ParameterNotSetException  this exception is thrown if this
    * parameter has not yet been set
    */
    public int getClassType() throws ParameterNotSetException {
        if (m_classTypePresent == true) {
            return (m_classType);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * This method returns the type of this primitive.
    *
    * @return    The Primitive Type of this event
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
        m_timeOutPresent = false;
        m_classTypePresent = false;
        m_operationPresent = false;
        m_lastInvokeEvent = false;
    }

    /**
    * String representation of class InvokeReqEvent
    *
    * @return    String provides description of class InvokeReqEvent
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nInvokeReqEvent");
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
        buffer.append("\n\ntimeOut = ");
        buffer.append(m_timeOut);
        buffer.append("\n\nclassType = ");
        buffer.append(m_classType);
        buffer.append("\n\nlinkedIdPresent = ");
        buffer.append(m_linkedIdPresent);
        buffer.append("\n\nparametersPresent = ");
        buffer.append(m_parametersPresent);
        buffer.append("\n\noperationPresent = ");
        buffer.append(m_operationPresent);
        buffer.append("\n\ntimeOutPresent = ");
        buffer.append(m_timeOutPresent);
        buffer.append("\n\nclassTypePresent = ");
        buffer.append(m_classTypePresent);
        buffer.append("\n\nlastInvokeEvent = ");
        buffer.append(m_lastInvokeEvent);

        return buffer.toString();
    }


    /**
    * The Operation parameter of the INVOKE request Component
    *
    * @serial    m_operation - a default serializable field
    */
    private Operation m_operation = null;

    /**
    * The Parameters' parameter of the INVOKE request Component
    *
    * @serial    m_parameters - a default serializable field
    */
    private Parameters m_parameters = null;

    /**
    * The Linked Id parameter of the INVOKE request component
    *
    * @serial    m_linkedId - a default serializable field
    */
    private int m_linkedId = 0;

    /**
    * The Time-Out parameter of the INVOKE request component
    *
    *@serial    m_timeOut - a default serializable field
    */
    private long m_timeOut = 0;

    /**
    * The Class Type parameter of the INVOKE request Component
    *
    * @serial    m_classType - a default serializable field
    */
    private int m_classType = 0;

    /**
    * @serial    m_linkedIdPresent - a default serializable field
    */
    private boolean m_linkedIdPresent = false;

    /**
    * @serial    m_parametersPresent - a default serializable field
    */
    private boolean m_parametersPresent = false;

    /**
    *@serial    m_operationPresent - a default serializable field
    */
    private boolean m_operationPresent = false;

    /**
    * @serial    m_timeOutPresent - a default serializable field
    */
    private boolean m_timeOutPresent = false;

    /**
    * @serial    m_classTypePresent - a default serializable field
    */
    private boolean m_classTypePresent = false;

    /**
    * @serial    m_lastInvokeEvent - a default serializable field
    */
    private boolean m_lastInvokeEvent = false;
}

