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

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

/** A class representing the ansi ISUP CircuitIdName parameter.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CircuitIdNameAnsi implements java.io.Serializable {
    /** Constructs a new ANSI CircuitIdName class, parameters with default values.  */
    public CircuitIdNameAnsi() {
        super();
    }
    /** Constructs an ANSI CircuitIdName class from the input parameters specified.
      * @param in_trunkNumber  The trunkNumber, range 0-4 octets.
      * @param in_clliCodeA  The CLLICodeA field.
      * @param in_clliCodeB  The CLLICodeB field.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public CircuitIdNameAnsi(long in_trunkNumber, CommonLangLocationIdAnsi in_clliCodeA,
            CommonLangLocationIdAnsi in_clliCodeB)
        throws ParameterRangeInvalidException {
        this();
        this.setTrunkNumber(in_trunkNumber);
        m_clliCodeA = in_clliCodeA;
        m_clliCodeB = in_clliCodeB;
    }
    /** Gets the TrunkNumber field of CircuitIdName parameter
      * @return TrunkNumber, range 0-32 bits.  */
    public long getTrunkNumber() {
        return m_trunkNumber;
    }
    /** Sets the TrunkNumber field of CircuitIdName parameter.
      * @param in_trunkNumber  The TrunkNumber value, range 0-32 bits.
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the range specified.  */
    public void setTrunkNumber(long in_trunkNumber) throws ParameterRangeInvalidException {
        if (0 > in_trunkNumber | in_trunkNumber > 0xffffffff)
            throw new ParameterRangeInvalidException();
        m_trunkNumber = in_trunkNumber;
    }
    /** Gets the CLLICodeA field of CircuitIdName parameter.
      * @return CLLICodeA, range s.  */
    public CommonLangLocationIdAnsi getCLLICodeA() {
        return m_clliCodeA;
    }
    /** Sets the CLLICodeA field of CircuitIdName parameter.
      * @param in_clliCodeA  The CLLICodeA value, range s.  */
    public void setCLLICodeA(CommonLangLocationIdAnsi in_clliCodeA) {
        m_clliCodeA = in_clliCodeA;
    }
    /** Gets the CLLICodeB field of CircuitIdName parameter.
      * @return CLLICodeB, range.  */
    public CommonLangLocationIdAnsi getCLLICodeB() {
        return m_clliCodeB;
    }
    /** Sets the CLLICodeB field of CircuitIdName parameter.
      * @param in_clliCodeB,  The CLLICodeB value, range.  */
    public void setCLLICodeB(CommonLangLocationIdAnsi in_clliCodeB) {
        m_clliCodeB = in_clliCodeB;
    }
    /** The toString method retrieves a string containing the values of the members of the CircuitIdNameAnsi class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\nCircuitIdNameAnsi:");
        b.append("\n\tCLLI Code A: " + m_clliCodeA);
        b.append("\n\tCLLI Code B: " + m_clliCodeB);
        b.append("\n\tTrunk Number: " + m_trunkNumber);
        return b.toString();
    }
    private CommonLangLocationIdAnsi m_clliCodeA;
    private CommonLangLocationIdAnsi m_clliCodeB;
    private long m_trunkNumber;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
