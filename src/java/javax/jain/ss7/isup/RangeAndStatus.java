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

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ISUP Range And Status parameter.
  * This class is common to the ITU and ANSI variants and provides the methods to access
  * the sub-fields of this parameter.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class RangeAndStatus extends java.lang.Object implements java.io.Serializable {
    /** Constructs a new RangeAndStatus class with default values.  */
    public RangeAndStatus() {
    }
    /** Constructs a RangeAndStatus class from the input parameters specified.
      * @param in_range  The range subfield, range 0 to 31.
      * @param in_status  The status, for ANSI, the STATUS subfield will not have more
      * than 24 bits of information (or 3 bytes) and for ITU the STATUS subfield will not
      * carry status information that is more than 32 bits (or 4 bytes).
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public RangeAndStatus(byte in_range, byte[] in_status)
        throws ParameterRangeInvalidException {
        this.setRange(in_range);
        this.setStatus(in_status);
    }
    /** Gets the Range field of the parameter.
      * For ANSI the range field cannot be more than 23 and for ITU the range field cannot
      * be more than 31.
      * @return Range subfield, range 0-31 Range 0 indicates that no status bytes are
      * present. Range+1 gives the number of circuits affected by the message.
      */
    public byte getRange() {
        return m_range;
    }
    /** Sets the Range field of the parameter.For ANSI the range field cannot be more than
      * 23 and for ITU the range field cannot be more than 31.
      * @param in_range  The Range subfield, range 0-31 Range 0 indicates that there are
      * no status bytes. Range+1 gives the number of circuits affected by the message.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setRange(byte in_range)
        throws ParameterRangeInvalidException {
        if (0 <= in_range && in_range <= 31) {
            m_range = in_range;
            return;
        }
        throw new ParameterRangeInvalidException("Range value " + in_range + " out of range.");
    }
    /** Gets the status field of the parameter.For ANSI, the STATUS subfield will not have
      * more than 24 bits of information (or 3 bytes) and for ITU the STATUS subfield will
      * not carry status information that is more than 32 bits (or 4 bytes).
      * @return byte[], the status value, where each bit contains the status of a voice
      * circuit.
      */
    public byte[] getStatus() {
        return m_status;
    }
    /** Sets the status field of the parameter.
      * For ANSI, the STATUS subfield will not have more than 24 bits of information (or 3
      * bytes) and for ITU the STATUS subfield will not carry status information that is
      * more than 32 bits (or 4 bytes).  
      * @param in_status  The status value, where each bit value indicates the status of a
      * voice circuit.
      */
    public void setStatus(byte[] in_status) {
        m_status = in_status;
    }
    /** Tests if the status field of the parameter is PRESENT or NOT.  BEFORE ACCESSING
      * THE STATUS SUBFIELD THE APPLICATION SHOULD CHECK WHETHER THE STATUS SUBFIELD IS
      * INCLUDED OR NOT. THIS IS BECAUSE THERE MAY BE CASES WHERE EVEN THOUGH THE RANGE
      * FIELD IS NON-ZERO, THE STATUS FIELD IS NOT PRESENT (eg. in the
      * EventCircuitGrpQueryResp, THE RangeAndStatus class WOULD HAVE NON-ZERO RANGE BUT
      * NO STATUS BYTES ARE PRESENT.
      * @return boolean, if the Status subfield is present then it is true else false. the
      * status of one circuit.
      */
    public boolean isStatusPresent() {
        return (m_status != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
      * RangeAndStatus class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.RangeAndStatus");
        b.append("\n\tm_range: " + m_range);
        b.append("\n\tm_status: ");
        if (m_status != null)
            b.append(JainSS7Utility.bytesToHex(m_status, 0, m_status.length));
        return b.toString();
    }
    protected byte m_range;
    protected byte[] m_status = null;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
