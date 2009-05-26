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
 * File Name     : ResultReqEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     14/11/2000  Phelim O'Doherty    Deprecated Link Id accessor methods.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap.component;

import jain.*;
import jain.protocol.ss7.tcap.*;

/**
 * An event representing a TCAP Result request component primitive. The
 * mandatory parameters of this primitive are supplied to the constructor.
 * Optional parameters may then be set using the set methods.
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 * @see        ComponentReqEvent
 */
public final class ResultReqEvent extends ComponentReqEvent {

    /**
    * Constructs a new Result request Event, with only the Event Source and the
    * <a href="package-summary.html">JAIN TCAP Mandatory</a> parameters being
    * supplied to the constructor.
    *
    * @param  source           the Event Source supplied to the constructor
    * @param  dialogueId       the Dialogue Id supplied to the constructor
    * @param  lastResultEvent  the Last Result Event flag supplied to the constructor
    */
    public ResultReqEvent(Object source, int dialogueId, boolean lastResultEvent) {

        super(source);
        setDialogueId(dialogueId);
        setLastResultEvent(lastResultEvent);
    }

    /**
    * Sets the lastResult flag of the Result request component. The
    * LastResultEvent parameter determines whether an RESULT_L or an RESULT_NL
    * component is being sent through the API. It is defaulted to false i.e. an
    * RESULT_NL component, in order to send an RESULT_L component the
    * LastResultEvent parameter must be set to true.
    *
    * @param  lastResultEvent  The LastResultEvent value
    */
    public void setLastResultEvent(boolean lastResultEvent) {

        m_lastResultEvent = lastResultEvent;
    }

    /**
    * Sets the Operation parameter of the Result Request Component
    *
    * @param  operation The new Operation value
    */
    public void setOperation(Operation operation) {

        m_operation = operation;
        m_operationPresent = true;
    }

    /**
    * Sets the Parameters' parameter of the Result Request Component.
    *
    * @param  params  The new Parameters value
    */
    public void setParameters(Parameters params) {

        m_parameters = params;
        m_parametersPresent = true;
    }

    /**
    * @deprecated    As of JAIN TCAP v1.1. No replacement - no function for
    * parameter.
    */
    public void setLinkId(int value) {

        m_linkId = value;
        m_linkIdPresent = true;
      }

    /**
    * Indicates if the Invoke Id is present in this Event.
    *
    * @return    true if Invoke Id has been set.
    */
    public boolean isInvokeIdPresent() {
        return m_invokeIdPresent;
    }

    /**
    * Indicates if the Last Result Component Event. The LastResultEvent parameter
    * determines whether an RESULT_L or an RESULT_NL component is being sent
    * through the API. It is defaulted to false i.e. an RESULT_NL component,
    * inorder to send an RESULT_L component the LastResultEvent parameter must be
    * set to true.
    *
    * @return    true if this is the Last Result Component.
    */
    public boolean isLastResultEvent() {
        return (m_lastResultEvent);
    }

    /**
    * Indicates if the Operation field is present in this Event.
    *
    * @return true if Operation has been set, false otherwise.
    */
    public boolean isOperationPresent() {
        return m_operationPresent;
    }

    /**
    * Gets the Operation parameter of the Result request Component. Operation
    * identifies the action to be executed by the remote application.
    *
    * @return  the Operation of the RESULT request Component
    * @exception  ParameterNotSetException  this exception is thrown if this
    *      parameter has not been set
    */
    public Operation getOperation() throws ParameterNotSetException {
        if (m_operationPresent == true) {
            return (m_operation);
        } else {
            throw new ParameterNotSetException();
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
    * Gets the Parameters' parameter of the Result request Component. <code>
    * Parameters</code> contains any parameters that accompany an operation or
    * that are provided in reply to an operation.
    *
    * @return the Parameters of the RESULT request Component
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
    * @deprecated    As of JAIN TCAP v1.1. No replacement - no function for parameter.
    */
    public boolean isLinkIdPresent() {
        return m_linkIdPresent;
    }

    /**
    * @deprecated As of JAIN TCAP v1.1. No replacement - no function for parameter.
    */
    public int getLinkId() throws ParameterNotSetException {
        if (m_linkIdPresent == true) {
            return (m_linkId);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * This method returns the type of this primitive.
    *
    * @return The Primitive Type of this event
    */
    public int getPrimitiveType() {
        return (jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_RESULT);
    }

    /**
    * Clears all previously set parameters .
    */
    public void clearAllParameters() {
        m_dialogueIdPresent = false;
        m_invokeIdPresent = false;
        m_linkIdPresent = false;
        m_operationPresent = false;
        m_parametersPresent = false;
        m_lastResultEvent = false;
    }

    /**
    * String representation of class ResultReqEvent
    *
    * @return    String provides description of class ResultReqEvent
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nResultReqEvent");
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
        buffer.append("\n\nparametersPresent = ");
        buffer.append(m_parametersPresent);
        buffer.append("\n\noperationPresent = ");
        buffer.append(m_operationPresent);
        buffer.append("\n\nlastResultEvent = ");
        buffer.append(m_lastResultEvent);

        return buffer.toString();
    }

    /**
    * The Operation parameter of the Result request Component
    *
    * @serial    m_operation - a default serializable field
    */
    private Operation m_operation = null;

    /**
    * The Parameters' parameter of the Result request Component
    *
    * @serial    m_parameters - a default serializable field
    */
    private Parameters m_parameters = null;

    /**
    * The Link Id parameter of the Result request component
    *
    * @serial    m_linkId - a default serializable field
    */
    private int m_linkId = 0;

    /**
    * @serial    linkIdPresent - a default serializable field
    */
    private boolean m_linkIdPresent = false;

    /**
    * @serial    m_parametersPresent - a default serializable field
    */
    private boolean m_parametersPresent = false;

    /**
    * @serial    m_operationPresent - a default serializable field
    */
    private boolean m_operationPresent = false;

    /**
    * @serial    m_lastResultEvent - a default serializable field
    */
    private boolean m_lastResultEvent = false;
}

