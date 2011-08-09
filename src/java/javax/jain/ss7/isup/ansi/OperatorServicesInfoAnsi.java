/*
 @(#) $RCSfile: OperatorServicesInfoAnsi.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:07 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:07 $ by $Author: brian $
 */

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ansi ISUP OperatorServicesInfo parameter.

    @author Monavacon Limited
    @version 1.2.2
  */
public class OperatorServicesInfoAnsi implements java.io.Serializable {
    public static final byte IT_ORIGINAL_ACCESS_PREFIX = 0;
    public static final byte IT_BILL_TO_INFORMATION_ENTRY_HANDELING = 2;
    public static final byte IT_BILL_TO_TYPE = 3;
    public static final byte IT_BILL_TO_SPECIFIC_INFORMATION = 4;
    public static final byte IT_SPECIAL_HANDELING = 5;
    public static final byte IT_ACCESS_SIGNALING = 7;
    public static final byte IV_IT1_UNKNOWN = 0;
    public static final byte IV_IT1_1_001 = 1;
    public static final byte IV_IT1_0_01 = 2;
    public static final byte IV_IT1_0 = 3;
    public static final byte IV_IT2_ENTRY_UNKNOWN_HANDELING_UNKNOWN = 0;
    public static final byte IV_IT2_ENTRY_MANUAL_BY_OPERATOR_STATION_HANDELING = 1;
    public static final byte IV_IT2_ENTRY_MANUAL_BY_OPERATOR_PERSON_HANDELING = 2;
    public static final byte IV_IT2_ENTRY_AUTOMATED_BY_TONE_INPUT_STATION_HANDELING = 3;
    public static final byte IV_IT2_ENTRY_UNKNOWN_STATION_HANDELING = 4;
    public static final byte IV_IT2_ENTRY_UNKNOWN_PERSON_HANDELING = 5;
    public static final byte IV_IT2_ENTRY_MANUAL_BY_OPERATOR_UNKNOWN_HANDELING = 6;
    public static final byte IV_IT2_ENTRY_AUTOMATED_BY_TONE_INPUT_UNKNOWN_HANDELING = 7;
    public static final byte IV_IT2_ENTRY_AUTOMATED_BY_TONE_INPUT_PERSON_HANDELING = 8;
    public static final byte IV_IT2_ENTRY_AUTOMATED_BY_SPOKEN_INPUT_UNKNOWN_HANDELING = 9;
    public static final byte IV_IT2_ENTRY_AUTOMATED_BY_SPOKEN_INPUT_STATION_HANDELING = 10;
    public static final byte IV_IT2_ENTRY_AUTOMATED_BY_SPOKEN_INPUT_PERSON_HANDELING = 11;
    public static final byte IV_IT3_UNKNOWN = 0;
    public static final byte IV_IT3_CARD_14_DIGIT_FORMAT = 1;
    public static final byte IV_IT3_CARD_89C_FORMAT = 2;
    public static final byte IV_IT3_CARD_OTHER_FORMAT = 3;
    public static final byte IV_IT3_COLLECT = 4;
    public static final byte IV_IT3_THIRD_NUMBER = 5;
    public static final byte IV_IT3_SENT_PAID = 6;
    public static final byte IV_IT4_NIDB_AUTHORIZES = 0;
    public static final byte IV_IT4_NIDB_REPORTS_VERIFY_BY_AUTOMATED_MEANS = 2;
    public static final byte IV_IT4_NIDB_REPORTS_VERIFY_BY_OPERATOR = 3;
    public static final byte IV_IT4_NO_NIDB_QUERY = 4;
    public static final byte IV_IT4_NO_NIDB_RESPONSE = 5;
    public static final byte IV_IT4_NIDB_REPORTS_UNAVAILABLE = 6;
    public static final byte IV_IT4_NO_NIDB_RESPONSE_TIMEOUT = 7;
    public static final byte IV_IT4_NO_NIDB_RESPONSE_REJECT_COMPONENT = 8;
    public static final byte IV_IT4_NO_NIDB_RESPONSE_ACG_IN_EFECT = 9;
    public static final byte IV_IT4_NO_NIDB_RESPONSE_SCCP_FAILURE = 10;
    public static final byte IV_IT5_UNKNOWN = 0;
    public static final byte IV_IT5_CALL_COMPLETION = 1;
    public static final byte IV_IT5_RATE_INFORMATION = 2;
    public static final byte IV_IT5_TROUBLE_REPORTING = 3;
    public static final byte IV_IT5_TIME_AND_CHANGES = 4;
    public static final byte IV_IT5_CREDIT_REPORTING = 5;
    public static final byte IV_IT5_GENERAL_ASSISTANCE = 6;
    public static final byte IV_IT7_UNKNOWN = 0;
    public static final byte IV_IT7_DIAL_PULSE = 1;
    public static final byte IV_IT7_DUAL_TONE_MULTIFREQUENCY =2;
    /** Constructs a new ANSI OperatorServicesInfo class, parameters with default values.  */
    public OperatorServicesInfoAnsi() {
    }
    /** Constructs an ANSI OperatorServicesInfo class from the input parameters specified.
        @param in_infoType  The information type, range 0-7.  <ul>
        <li>IT_ORIGINAL_ACCESS_PREFIX, <li>IT_BILL_TO_INFORMATION_ENTRY_HANDELING,
        <li>IT_BILL_TO_TYPE, <li>IT_BILL_TO_SPECIFIC_INFORMATION, <li>IT_SPECIAL_HANDELING
        and <li>IT_ACCESS_SIGNALING. </ul>
        @param in_infoValu  The information value, range 0-15.  <ul> <li>IV_IT1_UNKNOWN,
        <li>IV_IT1_1_001, <li>IV_IT1_0_01, <li>IV_IT1_0,
        <li>IV_IT2_ENTRY_UNKNOWN_HANDELING_UNKNOWN,
        <li>IV_IT2_ENTRY_MANUAL_BY_OPERATOR_STATION_HANDELING,
        <li>IV_IT2_ENTRY_MANUAL_BY_OPERATOR_PERSON_HANDELING,
        <li>IV_IT2_ENTRY_AUTOMATED_BY_TONE_INPUT_STATION_HANDELING,
        <li>IV_IT2_ENTRY_UNKNOWN_STATION_HANDELING,
        <li>IV_IT2_ENTRY_UNKNOWN_PERSON_HANDELING,
        <li>IV_IT2_ENTRY_MANUAL_BY_OPERATOR_UNKNOWN_HANDELING,
        <li>IV_IT2_ENTRY_AUTOMATED_BY_TONE_INPUT_UNKNOWN_HANDELING,
        <li>IV_IT2_ENTRY_AUTOMATED_BY_TONE_INPUT_PERSON_HANDELING,
        <li>IV_IT2_ENTRY_AUTOMATED_BY_SPOKEN_INPUT_UNKNOWN_HANDELING,
        <li>IV_IT2_ENTRY_AUTOMATED_BY_SPOKEN_INPUT_STATION_HANDELING,
        <li>IV_IT2_ENTRY_AUTOMATED_BY_SPOKEN_INPUT_PERSON_HANDELING, <li>IV_IT3_UNKNOWN,
        <li>IV_IT3_CARD_14_DIGIT_FORMAT, <li>IV_IT3_CARD_89C_FORMAT,
        <li>IV_IT3_CARD_OTHER_FORMAT, <li>IV_IT3_COLLECT, <li>IV_IT3_THIRD_NUMBER,
        <li>IV_IT3_SENT_PAID, <li>IV_IT4_NIDB_AUTHORIZES,
        <li>IV_IT4_NIDB_REPORTS_VERIFY_BY_AUTOMATED_MEANS,
        <li>IV_IT4_NIDB_REPORTS_VERIFY_BY_OPERATOR, <li>IV_IT4_NO_NIDB_QUERY,
        <li>IV_IT4_NO_NIDB_RESPONSE, <li>IV_IT4_NIDB_REPORTS_UNAVAILABLE,
        <li>IV_IT4_NO_NIDB_RESPONSE_TIMEOUT, <li>IV_IT4_NO_NIDB_RESPONSE_REJECT_COMPONENT,
        <li>IV_IT4_NO_NIDB_RESPONSE_ACG_IN_EFECT,
        <li>IV_IT4_NO_NIDB_RESPONSE_SCCP_FAILURE, <li>IV_IT5_UNKNOWN,
        <li>IV_IT5_CALL_COMPLETION, <li>IV_IT5_RATE_INFORMATION,
        <li>IV_IT5_TROUBLE_REPORTING, <li>IV_IT5_TIME_AND_CHANGES,
        <li>IV_IT5_CREDIT_REPORTING, <li>IV_IT5_GENERAL_ASSISTANCE, <li>IV_IT7_UNKNOWN,
        <li>IV_IT7_DIAL_PULSE and <li>IV_IT7_DUAL_TONE_MULTIFREQUENCY. </ul>
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public OperatorServicesInfoAnsi(byte in_infoType, byte in_infoValu)
        throws ParameterRangeInvalidException {
        this.setInfoType(in_infoType);
        this.setInfoValue(in_infoValu);
    }
    /** Gets the InfoType field of OperatorServicesInfo parameter.
        @return InfoType, range 0-7, see OperatorServicesInfoAnsi().
      */
    public byte getInfoType() {
        return m_infoType;
    }
    /** Sets the InfoType field of OperatorServicesInfo parameter.
        @param in_infoType  The InfoType value, range 0-7, see OperatorServicesInfoAnsi().
        @exception ParameterRangeInvalidException  Thrown when the sub-field is out of the
        range specified.
      */
    public void setInfoType(byte in_infoType)
        throws ParameterRangeInvalidException {
        if (0 <= in_infoType && in_infoType <= 7) {
            m_infoType = in_infoType;
            return;
        }
        throw new ParameterRangeInvalidException("InfoType value " + in_infoType + " out of range.");
    }
    /** Gets the InfoValue field of OperatorServicesInfo parameter.
        @return InfoValue, range 0-15, see OperatorServicesInfoAnsi()
      */
    public byte getInfoValue() {
        return m_infoValue;
    }
    /** Sets the InfoValue field of OperatorServicesInfo parameter.
        @param in_infoValue  The InfoValue value, range 0-15, see
        OperatorServicesInfoAnsi().
        @exception ParameterRangeInvalidException  Thrown when the sub-field is out of the
        range specified.
      */
    public void setInfoValue(byte in_infoValue)
        throws ParameterRangeInvalidException {
        if (0 <= in_infoValue && in_infoValue <= 15) {
            m_infoValue = in_infoValue;
            return;
        }
        throw new ParameterRangeInvalidException("InfoValue value " + in_infoValue + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
        OperatorServicesInfoAnsi class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.ansi.OperatorServicesInfoAnsi");
        b.append("\n\tm_infoType: " + m_infoType);
        b.append("\n\tm_infoValue: " + m_infoValue);
        return b.toString();
    }
    protected byte m_infoType;
    protected byte m_infoValue;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
