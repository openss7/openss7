/*
 @(#) src/java/jain/protocol/ss7/tcap/component/Operation.java <p>
 
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
  * This class represents the Operation parameter of a TCAP Component
  * primitive. Operation identifies the action to be executed by the
  * remote application.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public final class Operation implements java.io.Serializable {
    /** Update v1.1: Constants all deprecated as they are application
      * specific.  */
    private final static int OPERATION_TYPE = 0;
    /** Indicates a Local(ITU) or Private(ANSI) operation type, this has
      * an integer value of 1.  */
    public static final int OPERATIONTYPE_LOCAL = OPERATION_TYPE | 1;
    /** Operation Type constants: The following constants represent the
      * allowable values for the operation type parameter depending on
      * the protocol variant.  Indicates a Global(ITU) or National(ANSI)
      * operation type, this has an integer value of 2.  */
    public static final int OPERATIONTYPE_GLOBAL= OPERATION_TYPE | 2;
    /** Update v1.1: Constants all deprecated as they are application
      * specific.  */
    private final static int OPERATION_FAMILY = 0;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONFAMILY_CALLERINTERACTION = OPERATION_FAMILY | 5;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONFAMILY_CHARGING = OPERATION_FAMILY | 2;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONFAMILY_PROVIDEINSTRUCTION = OPERATION_FAMILY | 3;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONFAMILY_CONNECTIONCONTROL = OPERATION_FAMILY | 4;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONFAMILY_PARAMETER = OPERATION_FAMILY | 1;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONFAMILY_SENDNOTIFICATION = OPERATION_FAMILY | 6;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONFAMILY_NETWORKMANAGEMENT = OPERATION_FAMILY | 7;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONFAMILY_PROCEDURAL = OPERATION_FAMILY | 8;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONFAMILY_OPERATIONCONTROL = OPERATION_FAMILY | 9;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONFAMILY_REPORTEVENT = OPERATION_FAMILY | 10;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONFAMILY_MISCELLANEOUS = OPERATION_FAMILY | 127;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int PARAMETERSPECIFIER_PROVIDEVALUE = 1;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int PARAMETERSPECIFIER_SETVALUE = 2;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int CHARGINGSPECIFIER_BILLCALL = 3;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int PROVIDEINSTRUCTIONSPECIFIER_START = 4;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int PROVIDEINSTRUCTIONSPECIFIER_ASSIST = 5;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int CONNECTIONCONTROLSPECIFIER_CONNECT = 6;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int CONNECTIONCONTROLSPECIFIER_TEMPORARYCONNECT = 7;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int CONNECTIONCONTROLSPECIFIER_DISCONNECT = 8;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int CONNECTIONCONTROLSPECIFIER_TEMPORARYDISCONNECT = 9;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int CALLERINTERACTIONSPECIFIER_INFORMATIONPROVIDED = 10;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int CALLERINTERACTIONSPECIFIER_PLAYANNOUNCEMENTCOLLECTDIGIT = 11;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int CALLERINTERACTIONSPECIFIER_INFORMATIONWAITING = 12;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int CALLERINTERACTIONSPECIFIER_PLAYANNOUNCEMENT = 13;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int SENDNOTIFICATIONSPECIFIER_PARTYFREE = 14;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int NETWORKMANAGEMENTSPECIFIER_AUTOMATICCALLGAP = 15;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int PROCEDURALSPECIFIER_TEMPORARYHANDOVER = 16;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific..  */
    public static final int PROCEDURALSPECIFIER_REPORTASSISTTERMINATION = 17;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int OPERATIONCONTROLSPECIFIER_CANCEL = 18;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int REPORTEVENTSPECIFIER_VOICEMESSAGEAVAILABLE = 19;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int REPORTEVENTSPECIFIER_VOICEMESSAGERETRIEVED = 20;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int MISCELLANEOUSSPECIFIER_QUEUECALL = 21;
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement, application specific.  */
    public static final int MISCELLANEOUSSPECIFIER_DEQUEUECALL = 22;
    /** @deprecated
      * As of JAIN TCAP API v1.1, use constructor with mandatory
      * Operation Type and Code.  */
    public Operation() {
    }
    /** Constructor for the Operation object, which accepts mandatory
      * operation type and code parameters.
      * @since JAIN TCAP v1.1
      */
    public Operation(int operationType, byte[] operationCode) {
        setOperationType(operationType);
        setOperationCode(operationCode);
    }
    /** Sets the Operation Type (Operation Code Identifier) of the
      * Operation, which when set will Identify the Operation Code to
      * follow.
      * @param operationType
      * One of the following:- <ul>
      * <li>OPERATIONTYPE_GLOBAL (National for ANSI);
      * <li>OPERATIONTYPE_LOCAL (Private for ANSI). </ul> */
    public void setOperationType(int operationType) {
        m_operationType = operationType;
        m_operationTypePresent = true;
    }
    /** Sets the Operation Code of the Operation. The Operation Code
      * will be used by an ANSI and ITU implementation of the JAIN TCAP
      * API. The Operation Code within the ANSI Specification is
      * decomposed into an Operation Family and Operation Specifier, but
      * these are handled by the one field Operation Code within the
      * JAIN TCAP API.
      * @param operationCode
      * The new OperationCode value.
      * @since JAIN TCAP v1.1 */
    public void setOperationCode(byte[] operationCode) {
        m_operationCode = operationCode;
        m_operationCodePresent = true;
    }
    /** Returns the Operation Type (Operation Code Identifier) of the
      * Operation, which Identifies the Operation Code which follows it.
      * @return
      * One of the following: <ul>
      * <li>OPERATIONTYPE_GLOBAL (National for ANSI);
      * <li>OPERATIONTYPE_LOCAL (Private for ANSI). </ul>
      * @exception MandatoryParameterNotSetException
      * Thrown if this parameter has not been set.
      * @since JAIN TCAP v1.1 */
    public int getOperationType()
        throws MandatoryParameterNotSetException {
        if (m_operationTypePresent)
            return m_operationType;
        throw new MandatoryParameterNotSetException("Operation Type: not set.");
    }
    /** Returns the Operation Code of the Operation. The Operation Code
      * will be used by an ITU implementation of the JAIN TCAP API.
      * @return
      * The operation code value.
      * @exception MandatoryParameterNotSetException
      * Thrown if this parameter has not been set.
      * @since JAIN TCAP v1.1 */
    public byte[] getOperationCode()
        throws MandatoryParameterNotSetException {
        if (m_operationCodePresent)
            return m_operationCode;
        throw new MandatoryParameterNotSetException("Operation Code: not set.");
    }
    /** @deprecated
      * As of Jain Tcap v1.1. No replacement. Operation Type is
      * mandatory */
    public boolean isOperationTypePresent() {
        return m_operationTypePresent;
    }
    /** @deprecated
      * As of Jain Tcap v1.1. No replacement. Operation Code is
      * mandatory.  */
    public boolean isOperationCodePresent() {
        return m_operationCodePresent;
    }
    /** @deprecated
      * As of Jain Tcap v1.1. No replacement. Breaking the Operation
      * Code into family and specifier for an ANSI brings no value to
      * the API.  */
    public boolean isOperationFamilyPresent() {
        return m_operationFamilyPresent;
    }
    /** @deprecated
      * As of Jain Tcap v1.1. Use getOperationCode method.  Breaking the
      * Operation Code into family and specifier for an ANSI brings no
      * value to the API.  */
    public int getOperationFamily()
        throws ParameterNotSetException {
        if (m_operationFamilyPresent)
            return m_operationFamily;
        throw new ParameterNotSetException("Operation Family: not set.");
    }
    /** @deprecated
      * As of Jain Tcap v1.1. No replacement. Breaking the Operation
      * Code into family and specifier for an ANSI brings no value to
      * the API.  */
    public boolean isOperationSpecifierPresent() {
        return m_operationSpecifierPresent;
    }
    /** @deprecated
      * As of Jain Tcap v1.1. Use getOperationCode method.  Breaking the
      * Operation Code into family and specifier for an ANSI brings no
      * value to the API.  */
    public int getOperationSpecifier()
        throws ParameterNotSetException {
        if (m_operationSpecifierPresent)
            return m_operationSpecifier;
        throw new ParameterNotSetException("Operation Specifier: not set.");
    }
    /** @deprecated
      * As of Jain Tcap v1.1. No replacement. Breaking the Operation
      * Code into family and specifier for an ANSI brings no value to
      * the API.  */
    public boolean isPrivateOperationDataPresent() {
        return m_privateOperationDataPresent;
    }
    /** @deprecated
      * As of Jain Tcap v1.1. Use getOperationCode method.  Breaking the
      * Operation Code into family and specifier for an ANSI brings no
      * value to the API.  */
    public byte[] getPrivateOperationData()
        throws ParameterNotSetException {
        if (m_privateOperationDataPresent)
            return m_privateOperationData;
        throw new ParameterNotSetException("Operation Data: not set.");
    }
    /** @deprecated
      * As of Jain Tcap v1.1. Use setOperationCode method.  Breaking the
      * Operation Code into family and specifier for an ANSI brings no
      * value to the API.  */
    public void setOperationFamily(int operationFamily) {
        m_operationFamily = operationFamily;
        m_operationFamilyPresent = true;
    }
    /** @deprecated
      * As of Jain Tcap v1.1. Use setOperationCode method.  Breaking the
      * Operation Code into family and specifier for an ANSI brings no
      * value to the API.  */
    public void setOperationSpecifier(int operationSpecifier) {
        m_operationSpecifier = operationSpecifier;
        m_operationSpecifierPresent = true;
    }
    /** @deprecated
      * As of Jain Tcap v1.1. Use setOperationCode method.  Breaking the
      * Operation Code into family and specifier for an ANSI brings no
      * value to the API.  */
    public void setPrivateOperationData(byte[] privateOperationData) {
        m_privateOperationData = privateOperationData;
        m_privateOperationDataPresent = true;
    }
    /** Clears all previously set parameters.  */
    public void clearAllParameters() {
        m_operationTypePresent = false;
        m_operationFamilyPresent = false;
        m_operationCodePresent = false;
        m_operationSpecifierPresent = false;
        m_privateOperationDataPresent = false;
    }
    /** java.lang.String representation of class
      * jain.protocol.ss7.tcap.component.Operation.
      * @return
      * java.lang.String provides description of class
      * jain.protocol.ss7.tcap.component.Operation.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\n\nOperation");
        b.append("\n\noperationType = " + m_operationType);
        b.append("\n\noperationCode = ");
        if (m_operationCode!= null )
            b.append(m_operationCode.toString());
        return b.toString();
    }
    /** The Operation Type parameter of this Operation.
      * @serial m_operationType
      * - a default serializable field.  */
    private int m_operationType = 0;
    /** The Operation Code parameter of the Operation.
      * @serial m_operationCode
      * - a default serializable field.  */
    private byte[] m_operationCode = null;
    /** The Operation Family parameter of the Operation.
      * @deprecated Update v1.1:- Deprecated field
      * @serial m_operationFamily
      * - a default serializable field.  */
    private int m_operationFamily = 0;
    private int m_operationSpecifier = 0;
    /** The Operation Data parameter of the Operation.
      * @deprecated Update v1.1:- Deprecated field
      * @serial m_privateOperationData
      * - a default serializable field.  */
    private byte[] m_privateOperationData = null;
    /** Whether Operation Family is present.
      * @deprecated Update v1.1:- Deprecated field
      * @serial m_operationFamilyPresent
      * - a default serializable field.  */
    private boolean m_operationFamilyPresent = false;
    /** Whether Operation Specifier is present.
      * @deprecated Update v1.1:- Deprecated field
      * @serial m_operationSpecifierPresent
      * - a default serializable field.  */
    private boolean m_operationSpecifierPresent = false;
    /** Whether Operation Code is present.
      * @deprecated Update v1.1:- Deprecated field
      * @serial m_operationCodePresent
      * - a default serializable field.  */
    private boolean m_operationCodePresent = false;
    /** Whether Operation Type is present.
      * @deprecated Update v1.1:- Deprecated field
      * @serial m_operationTypePresent
      * - a default serializable field.  */
    private boolean m_operationTypePresent = false;
    /** Whether Operation Data is present.
      * @deprecated Update v1.1:- Deprecated field
      * @serial m_privatOperationDataPresent
      * - a default serializable field.  */
    private boolean m_privateOperationDataPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
