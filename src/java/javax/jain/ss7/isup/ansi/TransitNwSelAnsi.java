/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ANSI ISUP Transit Network Selection.
    This class inherits from the core class and adds methods to access the sub-fields
    specific to the ANSI variant.
    @author Monavacon Limited
    @version 1.2.2
  */
public class TransitNwSelAnsi extends TransitNwSel {
    public static final byte CC_UNSPECIFIED = 0;
    public static final byte CC_INTERNATIONAL_CALL_NO_OPERATOR_REQUESTED = 1;
    public static final byte CC_INTERNATIONAL_CALL_OPERATOR_REQUESTED = 2;
    /** Constructs a new ANSI TransitNwSel class, parameters with default values.  */
    public TransitNwSelAnsi() {
    }
    /** Constructs an ANSI TransitNwSel class from the input parameters specified.
        @param in_nwIdPlan  The network identification plan, range 0 to 15, see
        TransitNwSel().
        @param in_typeOfNwId  The type of network identification, range 0 to 7, see
        TransitNwSel().
        @param in_nwId  The network identification.
        @param in_cktCode  The circuit code value, range 0 to 15. <ul> <li>CC_UNSPECIFIED,
        <li>CC_INTERNATIONAL_CALL_NO_OPERATOR_REQUESTED and
        <li>CC_INTERNATIONAL_CALL_OPERATOR_REQUESTED. </ul>
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public TransitNwSelAnsi(byte in_nwIdPlan, byte in_typeOfNwId, byte[] in_nwId,
            byte in_cktCode)
        throws ParameterRangeInvalidException {
        super(in_nwIdPlan, in_typeOfNwId, in_nwId);
        this.setCircuitCode(in_cktCode);
    }
    /** Gets the Circuit Code field of the parameter.
        This indicates whether an operator is requested for an internation call and such
        related information.
        @return Byte containing the circuit code, range 0 to 15, see TransitNwSelAnsi().
      */
    public byte getCircuitCode() {
        return m_circuitCode;
    }
    /** Sets the Circuit Code field of the parameter.
        @param circuitCode  The CircuitCode value, range 0 to 15, see TransitNwSelAnsi().
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setCircuitCode(byte circuitCode)
        throws ParameterRangeInvalidException {
        if (0 <= circuitCode && circuitCode <= 15) {
            m_circuitCode = circuitCode;
            return;
        }
        throw new ParameterRangeInvalidException("CircuitCode value " + circuitCode + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
        TransitNwSelAnsi class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ansi.TransitNwSelAnsi");
        b.append("\n\tm_circuitCode: " + m_circuitCode);
        return b.toString();
    }
    protected byte m_circuitCode;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
