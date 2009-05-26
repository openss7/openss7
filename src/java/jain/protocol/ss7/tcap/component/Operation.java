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
 * File Name     : Operation.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     13/11/2000  Phelim O'Doherty    Added new mandatory constructor.
 *                                         Deprecated family and specifier methods.
 *                                         Deprecated ANSI specific constants.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap.component;

import jain.*;
import jain.protocol.ss7.tcap.*;

/**
 * This class represents the Operation parameter of a TCAP Component primitive.
 * Operation identifies the action to be executed by the remote application.
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 */
public final class Operation implements java.io.Serializable {

    /**
    * @deprecated - As of JAIN TCAP API v1.1, use constructor with mandatory
    * Operation Type and Code.
    */
    public Operation() {
    }

    /**
    * Constructor for the Operation object, which accepts mandatory operation
    * type and code parameters.
    *
    * @since JAIN TCAP v1.1
    */
    public Operation(int operationType, byte[] operationCode) {
        setOperationType(operationType);
        setOperationCode(operationCode);
    }

    /**
    * Sets the Operation Type (Operation Code Identifier) of the Operation, which
    * when set will Identify the Operation Code to follow.
    *
    * @param  operationType  one of the following:-
    *    <UL>
    *    <LI> OPERATIONTYPE_GLOBAL (National for ANSI)
    *    <LI> OPERATIONTYPE_LOCAL (Private for ANSI)
    *    </UL>
    */
    public void setOperationType(int operationType) {
        m_operationType = operationType;
        m_operationTypePresent = true;
    }

    /**
    * Sets the Operation Code of the Operation. The Operation Code will be used
    * by an ANSI and ITU implementation of the JAIN TCAP API. The Operation Code within
    * the ANSI Specification is decomposed into an Operation Family and Operation
    * Specifier, but these are handled by the one field Operation Code within the
    * JAIN TCAP API.
    *
    * @param  operationCode  The new OperationCode value
    *
    * @since JAIN TCAP v1.1
    */
    public void setOperationCode(byte[] operationCode) {
        m_operationCode = operationCode;
        m_operationCodePresent = true;
    }

    /**
    * Returns the Operation Type (Operation Code Identifier) of the Operation,
    * which Identifies the Operation Code which follows it.
    *
    * @return  one of the following:
    *      <UL>
    *        <LI> OPERATIONTYPE_GLOBAL (National for ANSI)
    *        <LI> OPERATIONTYPE_LOCAL (Private for ANSI)
    *      </UL>
    *
    * @exception MandatoryParameterNotSetException thrown if this parameter has
    * not been set.
    * @since JAIN TCAP v1.1
    */
    public int getOperationType()throws MandatoryParameterNotSetException {
        if (m_operationTypePresent == true) {
            return (m_operationType);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * Returns the Operation Code of the Operation. The Operation Code will be
    * used by an ITU implementation of the JAIN TCAP API.
    *
    * @return The operation code value
    * @exception MandatoryParameterNotSetException thrown if this parameter has
    * not been set.
    * @since JAIN TCAP v1.1
    */
    public byte[] getOperationCode()  throws MandatoryParameterNotSetException {
        if (m_operationCodePresent == true) {
            return (m_operationCode);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * @deprecated As of Jain Tcap v1.1. No replacement. Operation Type is
    * mandatory
    */
    public boolean isOperationTypePresent() {
        return m_operationTypePresent;
    }

    /**
    * @deprecated As of Jain Tcap v1.1. No replacement. Operation Code is
    * mandatory
    */
    public boolean isOperationCodePresent() {
        return m_operationCodePresent;
    }

    /**
    * @deprecated As of Jain Tcap v1.1. No replacement. Breaking the
    * Operation Code into family and specifier for an ANSI brings no value
    * to the API.
    */
    public boolean isOperationFamilyPresent() {
        return m_operationFamilyPresent;
    }

    /**
    * @deprecated As of Jain Tcap v1.1. Use {@link #getOperationCode getOperationCode}
    * method. Breaking the Operation Code into family and specifier for an ANSI
    * brings no value to the API.
    */
    public int getOperationFamily() throws ParameterNotSetException {
        if (m_operationFamilyPresent == true) {
            return (m_operationFamily);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * @deprecated As of Jain Tcap v1.1. No replacement. Breaking the Operation
    * Code into family and specifier for an ANSI brings no value to the API.
    */
    public boolean isOperationSpecifierPresent() {
        return m_operationSpecifierPresent;
    }

    /**
    * @deprecated As of Jain Tcap v1.1. Use {@link #getOperationCode getOperationCode}
    * method. Breaking the Operation Code into family and specifier for an ANSI
    * brings no value to the API.
    */
    public int getOperationSpecifier() throws ParameterNotSetException {
        if (m_operationSpecifierPresent == true) {
            return (m_operationSpecifier);
        } else {
            throw new ParameterNotSetException();
        }
      }

    /**
    * @deprecated As of Jain Tcap v1.1. No replacement. Breaking the Operation
    * Code into family and specifier for an ANSI brings no value to the API.
    */
    public boolean isPrivateOperationDataPresent() {
        return m_privateOperationDataPresent;
    }

    /**
    * @deprecated As of Jain Tcap v1.1. Use {@link #getOperationCode getOperationCode}
    * method. Breaking the Operation Code into family and specifier for an ANSI
    * brings no value to the API.
    */
    public byte[] getPrivateOperationData() throws ParameterNotSetException {
        if (m_privateOperationDataPresent == true) {
            return (m_privateOperationData);
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * @deprecated As of Jain Tcap v1.1. Use {@link #setOperationCode setOperationCode}
    * method. Breaking the Operation Code into family and specifier for an ANSI
    * brings no value to the API.
    */
    public void setOperationFamily(int operationFamily) {
        m_operationFamily = operationFamily;
        m_operationFamilyPresent = true;
    }

    /**
    * @deprecated As of Jain Tcap v1.1. Use {@link #setOperationCode setOperationCode}
    * method. Breaking the Operation Code into family and specifier for an ANSI
    * brings no value to the API.
    */
    public void setOperationSpecifier(int operationSpecifier) {
        m_operationSpecifier = operationSpecifier;
        m_operationSpecifierPresent = true;
    }

    /**
    *  @deprecated As of Jain Tcap v1.1. Use {@link #setOperationCode setOperationCode}
    * method. Breaking the Operation Code into family and specifier for an ANSI
    * brings no value to the API.
    */
    public void setPrivateOperationData(byte[] privateOperationData) {
        m_privateOperationData = privateOperationData;
        m_privateOperationDataPresent = true;
    }

    /**
    * Clears all previously set parameters
    */
    public void clearAllParameters() {
        m_operationTypePresent = false;
        m_operationFamilyPresent = false;
        m_operationCodePresent = false;
        m_operationSpecifierPresent = false;
        m_privateOperationDataPresent = false;
    }

    /**
    * String representation of class jain.protocol.ss7.tcap.component.Operation
    *
    * @return    String provides description of class jain.protocol.ss7.tcap.component.Operation
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nOperation");
        buffer.append("\n\noperationType = ");
        buffer.append(m_operationType);
        buffer.append("\n\noperationCode = ");
        if (m_operationCode!= null ) {
            buffer.append(m_operationCode.toString());
        } else {
            buffer.append("value is null");
        }
        return buffer.toString();
    }

    /**
    * The Operation Type parameter of this Operation
    *
    * @serial    m_operationType - a default serializable field
    */
    private int m_operationType = 0;

    /**
    * The Operation Code parameter of the Operation
    *
    * @serial    m_operationCode - a default serializable field
    */
    private byte[] m_operationCode = null;

    private final static int OPERATION_TYPE = 0;

    /**
    * Indicates a Local(ITU) or Private(ANSI) operation type, this has an integer
    * value of 1.
    */
    public final static int OPERATIONTYPE_LOCAL = OPERATION_TYPE | 1;

    /**
    * Operation Type constants: <BR>
    * The following constants represent the allowable values for the operation
    * type parameter depending on the protocol variant. <BR>
    * <BR>
    * Indicates a Global(ITU) or National(ANSI) operation type, this has an
    * integer value of 2.
    */
    public final static int OPERATIONTYPE_GLOBAL = OPERATION_TYPE | 2;

    /**
    * The Operation Family parameter of the Operation
    *
    * Update v1.1:- Deprecated field
    *
    * @serial    m_operationFamily - a default serializable field
    */
    private int m_operationFamily = 0;
    private int m_operationSpecifier = 0;

    /**
    * @serial    m_privateOperationData - a default serializable field
    * Update v1.1:- Deprecated field
    */
    private byte[] m_privateOperationData = null;

    /**
    *  Initialisation of the parameter present flags.
    */

    /**
    * @serial    m_operationFamilyPresent - a default serializable field
    *Update v1.1:- Deprecated field
    */
    private boolean m_operationFamilyPresent = false;

    /**
    * @serial    m_operationSpecifierPresent - a default serializable field
    * Update v1.1:- Deprecated field
    */
    private boolean m_operationSpecifierPresent = false;

    /**
    * @serial    m_operationCodePresent - a default serializable field
    * Update v1.1:- Deprecated field
    */
    private boolean m_operationCodePresent = false;

    /**
    * @serial    m_operationTypePresent - a default serializable field
    * Update v1.1:- Deprecated field
    */
    private boolean m_operationTypePresent = false;

    /**
    *@serial    m_privatOperationDataPresent - a default serializable field
    *Update v1.1:- Deprecated field
    */
    private boolean m_privateOperationDataPresent = false;

    /**
    * Update v1.1: Constants all deprecated as they are application specific.
    */
    private final static int OPERATION_FAMILY = 0;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONFAMILY_CALLERINTERACTION = OPERATION_FAMILY | 5;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONFAMILY_CHARGING = OPERATION_FAMILY | 2;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONFAMILY_PROVIDEINSTRUCTION = OPERATION_FAMILY | 3;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONFAMILY_CONNECTIONCONTROL = OPERATION_FAMILY | 4;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONFAMILY_PARAMETER = OPERATION_FAMILY | 1;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONFAMILY_SENDNOTIFICATION = OPERATION_FAMILY | 6;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONFAMILY_NETWORKMANAGEMENT = OPERATION_FAMILY | 7;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONFAMILY_PROCEDURAL = OPERATION_FAMILY | 8;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONFAMILY_OPERATIONCONTROL = OPERATION_FAMILY | 9;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONFAMILY_REPORTEVENT = OPERATION_FAMILY | 10;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONFAMILY_MISCELLANEOUS = OPERATION_FAMILY | 127;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int PARAMETERSPECIFIER_PROVIDEVALUE = 1;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int PARAMETERSPECIFIER_SETVALUE = 2;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int CHARGINGSPECIFIER_BILLCALL = 3;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int PROVIDEINSTRUCTIONSPECIFIER_START = 4;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int PROVIDEINSTRUCTIONSPECIFIER_ASSIST = 5;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int CONNECTIONCONTROLSPECIFIER_CONNECT = 6;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int CONNECTIONCONTROLSPECIFIER_TEMPORARYCONNECT = 7;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int CONNECTIONCONTROLSPECIFIER_DISCONNECT = 8;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int CONNECTIONCONTROLSPECIFIER_TEMPORARYDISCONNECT = 9;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int CALLERINTERACTIONSPECIFIER_INFORMATIONPROVIDED = 10;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int CALLERINTERACTIONSPECIFIER_PLAYANNOUNCEMENTCOLLECTDIGIT = 11;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int CALLERINTERACTIONSPECIFIER_INFORMATIONWAITING = 12;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int CALLERINTERACTIONSPECIFIER_PLAYANNOUNCEMENT = 13;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int SENDNOTIFICATIONSPECIFIER_PARTYFREE = 14;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int NETWORKMANAGEMENTSPECIFIER_AUTOMATICCALLGAP = 15;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int PROCEDURALSPECIFIER_TEMPORARYHANDOVER = 16;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific..
    */
    public final static int PROCEDURALSPECIFIER_REPORTASSISTTERMINATION = 17;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int OPERATIONCONTROLSPECIFIER_CANCEL = 18;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int REPORTEVENTSPECIFIER_VOICEMESSAGEAVAILABLE = 19;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int REPORTEVENTSPECIFIER_VOICEMESSAGERETRIEVED = 20;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int MISCELLANEOUSSPECIFIER_QUEUECALL = 21;

    /**
    * @deprecated As of JAIN TCAP API v1.1. No replacement, application specific.
    */
    public final static int MISCELLANEOUSSPECIFIER_DEQUEUECALL = 22;
}

