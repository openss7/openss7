/*
 @(#) $RCSfile: CauseInd.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:47 $ <p>
 
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
 
 Last Modified $Date: 2011-09-02 08:46:47 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing a Cause Indicator parameter.
    This class is common to ITU and ANSI variants.

    @author Monavacon Limited
    @version 1.2.2
  */
public class CauseInd implements java.io.Serializable {
    public static final boolean EI_OCTET_CONTINUE_THROUGH_NEXT = false;
    public static final boolean EI_LAST_OCTET = true;
    public static final byte CS_ITU_STANDARD = 0;
    public static final byte CS_ISO_IEC_STANDARD = 1;
    public static final byte CS_ANSI_STANDARD = 2;
    public static final byte CS_NATIONAL_STANDARD = 2;
    public static final byte CS_STANDARD_SPECIFIC_TO_IDENTIFIED_LOCATION = 3;
    public static final byte LOCATION_USER = 0;
    public static final byte LOCATION_LOCAL_PRIVATE_NETWORK = 1;
    public static final byte LOCATION_LOCAL_LOCAL_NETWORK = 2;
    public static final byte LOCATION_TRANSIT_NETWORK = 3;
    public static final byte LOCATION_REMOTE_LOCAL_NETWORK = 4;
    public static final byte LOCATION_REMOTE_PRIVATE_NETWORK = 5;
    public static final byte LOCATION_LOCAL_INTERFACE = 6;
    public static final byte LOCATION_INTERNATIONAL_NETWORK = 7;
    public static final byte LOCATION_NETWORK_BEYOND_INTERWORKING_POINT = 10;
    /** Constructs a new CauseInd taking no parameters.
        This constructor sets all the sub-field values to their spare value (which is
        zero).
      */
    public CauseInd() {
    }
    /** Constructs a new Cause Indicator.
        @param location  The location, range 0-15.
        @param codingStandard  The coding standard, range 0-3.
        @param causeValue  The cause value, range 0-127.
        @param diagnostic  The diagnostic.
        @param ext1  The extension one indicator.
        @param ext2  The extension two indicator.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public CauseInd(byte location, byte codingStandard, byte causeValue,
            byte[] diagnostic, boolean ext1, boolean ext2)
        throws ParameterRangeInvalidException {
        this.setLocation(location);
        this.setCodingStandard(codingStandard);
        this.setCauseValue(causeValue);
        this.setDiagnostic(diagnostic);
        this.setExt1(ext1);
        this.setExt2(ext2);
    }
    /** Gets the cause value field of the parameter.
        @return Byte containing the Cause Value, range 0 -127, see T1.113.3 for ANSI and
        Q.850 for ITU.
      */
    public byte getCauseValue() {
        return m_causeValue;
    }
    /** Sets the cause value field of the parameter.
        @param causeValue  Cause value, range 0 -127, see T1.113.3 for ANSI and Q.850
        for ITU.
      */
    public void setCauseValue(byte causeValue)
        throws ParameterRangeInvalidException {
        if (0 <= causeValue && causeValue <= 127) {
            m_causeValue = causeValue;
            return;
        }
        throw new ParameterRangeInvalidException("CauseValue value " + causeValue + " out of range.");
    }
    /** Gets the Coding Standard field of the parameter.
        @return Byte containing the CodingStandard value, range 0 to 3, see CauseInd().
      */
    public byte getCodingStandard() {
        return m_codingStandard;
    }
    /** Sets the Coding Standard field of the parameter.
        @param codingStandard  The coding standard value, range 0 to 3, see
        CauseInd().
        @exception ParameterRangeInvalidException  Thrown if the sub-field is out of
        range.
      */
    public void setCodingStandard(byte codingStandard)
        throws ParameterRangeInvalidException {
        if (0 <= codingStandard && codingStandard <= 3) {
            m_codingStandard = codingStandard;
            return;
        }
        throw new ParameterRangeInvalidException("CodingStandard value " + codingStandard + " out of range.");
    }
    /** Gets the diagnostic field of the parameter.
        @return Byte array containing the Diagnostic value.
      */
    public byte[] getDiagnostic() {
        return m_diagnostic;
    }
    /** Sets the diagnostic field of the parameter.
        @param diagnostic  Diagnostics subfield.
      */
    public void setDiagnostic(byte[] diagnostic) {
        m_diagnostic = diagnostic;
    }
    /** Gets the location field of the parameter.
        @return Byte containing the Location value, range 0 to 15, see CauseInd().
      */
    public byte getLocation() {
        return m_location;
    }
    /** Sets the location field of the parameter.
        @param location  The location, range 0 to 15, see CauseInd().
        @exception ParameterRangeInvalidException  Thrown if the sub-field is out of
        range.
      */
    public void setLocation(byte location)
        throws ParameterRangeInvalidException {
        if (0 <= location && location <= 15) {
            m_location = location;
            return;
        }
        throw new ParameterRangeInvalidException("Location value " + location + " out of range.");
    }
    /** Gets the ext1 field of the parameter.
        @return Boolean the Ext1 value, see CauseInd().
      */
    public boolean getExt1() {
        return m_ext1;
    }
    /** Sets the ext1 field of the parameter,
        @param ext1  The Ext1 value, see CauseInd().
      */
    public void setExt1(boolean ext1) {
        m_ext1 = ext1;
    }
    /** Gets the ext2 field of the parameter.
        @return Boolean the Ext2 value, see CauseInd().
      */
    public boolean getExt2() {
        return m_ext2;
    }
    /** Sets the ext2 field of the parameter.
        @param ext2  The Ext1 value, see CauseInd().
      */
    public void setExt2(boolean ext2) {
        m_ext2 = ext2;
    }
    /** The toString method retrieves a string containing the values of the members of the
        CauseInd class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.CauseInd");
        b.append("\n\tm_causeValue: " + m_causeValue);
        b.append("\n\tm_codingStandard: " + m_codingStandard);
        b.append("\n\tm_diagnostic: " + JainSS7Utility.bytesToHex(m_diagnostic, 0, m_diagnostic.length));
        b.append("\n\tm_location: " + m_location);
        b.append("\n\tm_ext1: " + m_ext1);
        b.append("\n\tm_ext2: " + m_ext2);
        return b.toString();
    }
    protected byte m_causeValue;
    protected byte m_codingStandard;
    protected byte[] m_diagnostic;
    protected byte m_location;
    protected boolean m_ext1;
    protected boolean m_ext2;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
