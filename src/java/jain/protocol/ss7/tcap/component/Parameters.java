/*
 @(#) src/java/jain/protocol/ss7/tcap/component/Parameters.java <p>
 
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
  * Parameters is a parameter of a component. It is used to hold any
  * parameters that accompany an operation or are provided in reply to
  * an operation.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public final class Parameters implements java.io.Serializable {
    private final static int PARAMETER_IDENTIFIER = 0;
    /** Indicates a parameter single, which holds only one User
      * Information element. It has an integer value of 1.  */
    public static final int PARAMETERTYPE_SINGLE = PARAMETER_IDENTIFIER | 1;
    /** Indicates a parameter sequence, which holds a sequence of User
      * Information elements. It has an integer value of 2.  */
    public static final int PARAMETERTYPE_SEQUENCE = PARAMETER_IDENTIFIER | 2;
    /** Indicates a parameter set, which holds a set of User Information
      * elements. It has an integer value of 3.  */
    public static final int PARAMETERTYPE_SET = PARAMETER_IDENTIFIER | 3;
    /** Constructs a new Parameters.
      * @param parameterIdentifier
      * The Parameter Identifier supplied to the constructor.
      * @param parameter
      * The paramater supplied to the constructor.  */
    public Parameters(int parameterIdentifier, byte[] parameter) {
        setParameterIdentifier(parameterIdentifier);
        setParameter(parameter);
    }
    /** Sets the 'Parameter Identifier' parameter of the Component Parameters.
      * @param parameterIdentifier
      * One of the following: <ul>
      * <li>PARMETERTYPE_SINGLE;
      * <li>PARMETERTYPE_SEQUENCE;
      * <li>PARMETERTYPE_SET. </ul> */
    public void setParameterIdentifier(int parameterIdentifier) {
        m_parameterIdentifier = parameterIdentifier;
        m_parameterIdentifierPresent = true;
    }
    /** Sets the 'Parameter values' field of the Component.
      * @param parameter
      * The new Parameter value.  */
    public void setParameter(byte[] parameter) {
        m_parameter = parameter;
        m_parameterPresent = true;
    }
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement. The Parameter field is
      * mandatory.  */
    public boolean isParameterIdentifierPresent() {
        return m_parameterIdentifierPresent;
    }
    /** Returns the parameter Identifier of this Parameter. <br>
      * Update v1.1: Changed exception type of method from
      * ParameterNotSetException to MandatoryParameterNotSetException.
      * @return
      * One of the following: <ul>
      * <li>PARMETERTYPE_SINGLE;
      * <li>PARMETERTYPE_SEQUENCE;
      * <li>PARMETERTYPE_SET. </ul>
      * @exception MandatoryParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public int getParameterIdentifier()
        throws MandatoryParameterNotSetException {
        if (m_parameterIdentifierPresent)
            return m_parameterIdentifier;
        throw new MandatoryParameterNotSetException("Parameter Identifier: not set.");
    }
    /** @deprecated
      * As of JAIN TCAP API v1.1. No replacement. The Parameter field is
      * mandatory.  */
    public boolean isParameterPresent() {
        return m_parameterPresent;
    }
    /** Returns the supplied parameter values. Update v1.1: Changed
      * exception type of method from ParameterNotSetException to
      * MandatoryParameterNotSetException.
      * @return
      * The value of Parameters.
      * @exception MandatoryParameterNotSetException
      * This exception is thrown if this parameter has not been set.  */
    public byte[] getParameter()
        throws MandatoryParameterNotSetException {
        if (m_parameterPresent)
            return m_parameter;
        throw new MandatoryParameterNotSetException("Parameter: not set.");
    }
    /** Clears all previously set parameters.  */
    public void clearAllParameters() {
        m_parameterIdentifierPresent = false;
        m_parameterPresent = false;
    }
    /** java.lang.String representation of class Parameters.
      * @return
      * java.lang.String provides description of class Parameters.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(500);
        b.append("\n\nParameter");
        b.append("\n\nparameterIdentifier = " + m_parameterIdentifier);
        b.append("\n\nparameter = ");
        if (m_parameter != null)
            for(int i = 0; i < m_parameter.length; i++){
                b.append(m_parameter[i]);
                b.append(" ");
            }
        b.append("\n\nparameterIdentifierPresent = " + m_parameterIdentifierPresent);
        b.append("\n\nparameterPresent = " + m_parameterPresent);
        return b.toString();
    }
    /** The 'Parameter Identifier' parameter of the PARAMETERS
      * Component.
      * @serial m_parameterIdentifier
      * - a default serializable field.  */
    private int m_parameterIdentifier = 0;
    /** The Parameters' parameter of the PARAMETERS Component.
      * @serial m_parameter
      * - a default serializable field.  */
    private byte[] m_parameter = null;
    /** Indicates if parameter is present.
      * @serial m_parameterIdentifierPresent
      * - a default serializable field.  */
    private boolean m_parameterIdentifierPresent = false;
    /** Whether the Parameter is present.
      * @serial m_parameterPresent
      * - a default serializable field.  */
    private boolean m_parameterPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
