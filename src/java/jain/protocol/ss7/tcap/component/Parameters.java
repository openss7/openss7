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
 * File Name     : Parameters.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     14/11/2000  Phelim O'Doherty    Updated after public release and
 *                                         certification process comments.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap.component;

import jain.*;
import jain.protocol.ss7.tcap.*;

/**
 * Parameters is a parameter of a component. It is used to hold any parameters
 * that accompany an operation or are provided in reply to an operation.
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 */
public final class Parameters implements java.io.Serializable {

    /**
    * Constructs a new Parameters.
    *
    * @param  parameterIdentifier the Parameter Identifier supplied to the constructor
    * @param  parameter the paramater supplied to the constructor
    */
    public Parameters(int parameterIdentifier, byte[] parameter) {
        setParameterIdentifier(parameterIdentifier);
        setParameter(parameter);
    }

    /**
    * Sets the 'Parameter Identifier' parameter of the Component
    *
    * @param  parameterIdentifier one of the following:
    *    <UL>
    *    <LI> PARMETERTYPE_SINGLE
    *    <LI> PARMETERTYPE_SEQUENCE
    *    <LI> PARMETERTYPE_SET
    *    </UL>
    */
    public void setParameterIdentifier(int parameterIdentifier) {
        m_parameterIdentifier = parameterIdentifier;
        m_parameterIdentifierPresent = true;
    }

    /**
    * Sets the 'Parameter values' field of the Component
    *
    * @param  parameter The new Parameter value
    */
    public void setParameter(byte[] parameter) {
        m_parameter = parameter;
        m_parameterPresent = true;
    }

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement. The Parameter
    * field is mandatory.
    */
    public boolean isParameterIdentifierPresent() {
        return m_parameterIdentifierPresent;
    }

    /**
    * Returns the parameter Identifier of this Parameter.
    *
    * Update v1.1: Changed exception type of method from ParameterNotSetException
    * to MandatoryParameterNotSetException.
    *
    * @return one of the following:
    *    <UL>
    *    <LI> PARMETERTYPE_SINGLE
    *    <LI> PARMETERTYPE_SEQUENCE
    *    <LI> PARMETERTYPE_SET
    *    </UL>
    * @exception  MandatoryParameterNotSetException  this exception is thrown if this
    *      parameter has not been set
    */
    public int getParameterIdentifier() throws MandatoryParameterNotSetException {
        if (m_parameterIdentifierPresent == true) {
            return (m_parameterIdentifier);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement. The Parameter
    * field is mandatory.
    */
    public boolean isParameterPresent() {
        return m_parameterPresent;
    }

    /**
    * Returns the supplied parameter values.
    *
    * Update v1.1: Changed exception type of method from ParameterNotSetException
    * to MandatoryParameterNotSetException.
    *
    * @return  the value of Parameters.
    * @exception  MandatoryParameterNotSetException  this exception is
    * thrown if this parameter has not been set
    */
    public byte[] getParameter() throws MandatoryParameterNotSetException {
        if (m_parameterPresent == true) {
            return (m_parameter);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * Clears all previously set parameters.
    */
    public void clearAllParameters() {
        m_parameterIdentifierPresent = false;
        m_parameterPresent = false;
    }

    /**
    * String representation of class Parameters
    *
    * @return    String provides description of class Parameters
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nParameter");
        buffer.append("\n\nparameterIdentifier = ");
        buffer.append(m_parameterIdentifier);
        buffer.append("\n\nparameter = ");
        if (m_parameter != null) {
            for(int i = 0; i < m_parameter.length; i++){
                buffer.append(m_parameter[i]);
                buffer.append(" ");
            }
        } else {
            buffer.append("value is null");
        }
        buffer.append("\n\nparameterIdentifierPresent = ");
        buffer.append(m_parameterIdentifierPresent);
        buffer.append("\n\nparameterPresent = ");
        buffer.append(m_parameterPresent);

        return buffer.toString();
    }


    /**
    * The 'Parameter Identifier' parameter of the PARAMETERS Component
    *
    * @serial    m_parameterIdentifier - a default serializable field
    */
    private int m_parameterIdentifier = 0;

    /**
    * The Parameters' parameter of the PARAMETERS Component
    *
    * @serial    m_parameter - a default serializable field
    */
    private byte[] m_parameter = null;

    /**
    * Indicates if parameter is present
    *
    * @serial    m_parameterIdentifierPresent - a default serializable field
    */
    private boolean m_parameterIdentifierPresent = false;

    /**
    *
    * @serial    m_parameterPresent - a default serializable field
    */
    private boolean m_parameterPresent = false;

    private final static int PARAMETER_IDENTIFIER = 0;

    /**
    * Indicates a parameter single, which holds only one User Information
    * element. It has an integer value of 1.
    */
    public final static int PARAMETERTYPE_SINGLE = PARAMETER_IDENTIFIER | 1;

    /**
    * Indicates a parameter sequence, which holds a sequence of User Information
    * elements. It has an integer value of 2.
    */
    public final static int PARAMETERTYPE_SEQUENCE = PARAMETER_IDENTIFIER | 2;

    /**
    * Indicates a parameter set, which holds a set of User Information elements.
    * It has an integer value of 3.
    */
    public final static int PARAMETERTYPE_SET = PARAMETER_IDENTIFIER | 3;

    /**
    * Parameter identifier constants. The following constants represent the
    * allowable values for the parameter identifier.
    */
}

