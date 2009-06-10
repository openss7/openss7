/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date$ by $Author$
 */

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the itu ISUP CUGInterlockCode parameter.

    @author Monavacon Limited
    @version 1.2.2
  */
public class CUGInterlockCodeItu implements java.io.Serializable {
    /** Constructs a CUGInterlockCodeItu class, parameters with default values.  */
    public CUGInterlockCodeItu() {
    }
    /** Constructs a CUGInterlockCodeItu class from the input parameters specified.
        @param in_binaryCode  The binary code, range 0-0xFFFF.
        @param in_networkIdentity  The network identity value, range 0-4 digits.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public CUGInterlockCodeItu(int in_binaryCode, java.lang.String in_networkIdentity)
        throws ParameterRangeInvalidException {
        this.setBinaryCode(in_binaryCode);
        this.setNetworkIdentity(in_networkIdentity);
    }
    /** Gets the BinaryCode field of CUGInterlockCode parameter.
        @return BinaryCode, range 0-0xFFFF.
      */
    public int getBinaryCode() {
        return m_binaryCode;
    }
    /** Sets the BinaryCode field of CUGInterlockCode parameter.
        @param in_binaryCode  The BinaryCode value, range 0-0xFFFF.
      */
    public void setBinaryCode(int in_binaryCode)
        throws ParameterRangeInvalidException {
        if (0 <= in_binaryCode  && in_binaryCode <= 0xffff) {
            m_binaryCode = in_binaryCode;
            return;
        }
        throw new ParameterRangeInvalidException("BinaryCode value " + in_binaryCode + " out of range.");
    }
    /** Gets the NetworkIdentity field of CUGInterlockCode parameter.
        @return NetworkIdentity, range 0-4 digits.
      */
    public java.lang.String getNetworkIdentity() {
        return m_networkIdentity;
    }
    /** Sets the NetworkIdentity field of CUGInterlockCode parameter.
        @param in_networkIdentity  The NetworkIdentity value, range 0-4 digits. */
    public void setNetworkIdentity(java.lang.String in_networkIdentity)
        throws ParameterRangeInvalidException {
        if (0 <= in_networkIdentity.length() && in_networkIdentity.length() <= 4) {
            m_networkIdentity = in_networkIdentity;
            return;
        }
        throw new ParameterRangeInvalidException("NetworkIdentity value " + in_networkIdentity + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
        CUGInterlockCodeItu class.
        @return A string representation of the member variables. */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.CUGInterlockCodeItu");
        b.append("\n\tm_binaryCode: " + m_binaryCode);
        b.append("\n\tm_networkIdentity: " + m_networkIdentity);
        return b.toString();
    }
    protected int m_binaryCode;
    protected String m_networkIdentity;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
