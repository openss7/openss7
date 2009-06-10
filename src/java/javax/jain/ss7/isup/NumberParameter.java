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

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** This is the base class for all the NumberParameter/ID subclasses.
  * These subclasses extend this NumberParameter Class and add their own specific methods
  * and parameters.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class NumberParameter implements java.io.Serializable {
    public static final byte NAI_SUBSCRIBER_NUMBER = 1;
    public static final byte NAI_UNKOWN = 2;
    public static final byte NAI_NATIONAL_NUMBER = 3;
    public static final byte NAI_INTERNATIONAL_NUMBER = 4;
    public static final byte NAI_CALLED_PARTY_SUBSCRIBER_NUMBER = 5;
    public static final byte NAI_CALLED_PARTY_NO_NUMBER = 6;
    public static final byte NAI_CALLED_PARTY_NATIONAL_NUMBER = 7;
    public static final byte NAI_ABBREVIATED_NUMBER = 8;
    public static final byte NAI_NON_UNIQUE_SUBSCRIBER_NUMBER = 113;
    public static final byte NAI_NON_UNIQUE_NATIONAL_NUMBER = 115;
    public static final byte NAI_NON_UNIQUE_INTERNATIONAL_NUMBER = 116;
    public static final byte NAI_TEST_LINE_TEST_CODE = 119;
    public static final byte NPI_ISDN_NUMBERING_PLAN = 1;
    public static final byte NPI_DATA_NUMBERING_PLAN = 3;
    public static final byte NPI_TELEX_NUMBERING_PLAN = 4;
    public static final byte NPI_PRIVATE_NUMBERING_PLAN = 5;
    /** Constructs a new NumberParameter taking no parameters.
      * This constructor sets all the sub-field values to their spare value (which is
      * zero).
      */
    public NumberParameter() {
    }
    /** Constructs a new NumberParameter.
      * @param natureOfAddressIndicator  The nature of address indicator, range 0-127 <ul>
      * <li>NAI_SUBSCRIBER_NUMBER, <li>NAI_UNKOWN, <li>NAI_NATIONAL_NUMBER,
      * <li>NAI_INTERNATIONAL_NUMBER, <li>NAI_CALLED_PARTY_SUBSCRIBER_NUMBER (ANSI only),
      * <li>NAI_CALLED_PARTY_NO_NUMBER (ANSI only), <li>NAI_CALLED_PARTY_NATIONAL_NUMBER
      * (ANSI only), <li>NAI_ABBREVIATED_NUMBER (ANSI only),
      * <li>NAI_NON_UNIQUE_SUBSCRIBER_NUMBER (ANSI only),
      * <li>NAI_NON_UNIQUE_NATIONAL_NUMBER (ANSI only),
      * <li>NAI_NON_UNIQUE_INTERNATIONAL_NUMBER (ANSI only) and
      * <li>NAI_TEST_LINE_TEST_CODE (ANSI only). </ul>
      * @param numberingPlanIndicator  The numbering plan indicator, range 0-7 <ul>
      * <li>NPI_ISDN_NUMBERING_PLAN, <li>NPI_DATA_NUMBERING_PLAN,
      * <li>NPI_TELEX_NUMBERING_PLAN and <li>NPI_PRIVATE_NUMBERING_PLAN. </ul>
      * @param addressSignal  The address signal.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      *
      */
    public NumberParameter(byte natureOfAddressIndicator, byte numberingPlanIndicator,
            byte[] addressSignal)
        throws ParameterRangeInvalidException {
        this.setNatureOfAddressInd(natureOfAddressIndicator);
        this.setNumberingPlanInd(numberingPlanIndicator);
        this.setAddressSignal(addressSignal);
    }
    /** Gets the Address Signal field of the parameter.
      * One address signal is stored in a byte. The number of address signals can be
      * between 1 to 32.
      * @return The Address Signal array with each digit per byte.
      */
    public byte[] getAddressSignal() {
        return m_addressSignal;
    }
    /** Sets the Address Signal field of the parameter.
      * Refer to getAddressSignal method for more details.
      * @param addressSignal  Address signal array.
      * @exception ParameterRangeInvalidException  Is thrown when the sub-field is out of
      * the parameter range specified.
      */
    public void setAddressSignal(byte[] addressSignal)
        throws ParameterRangeInvalidException {
        if (1 <= addressSignal.length && addressSignal.length <= 31) {
            m_addressSignal = addressSignal;
            return;
        }
        throw new ParameterRangeInvalidException("AddressSignal value " + addressSignal + " out of range.");
    }
    /** Gets the Nature of Address Indicator field of the parameter, range 0 to 127, see
      * NumberParameter().
      * @return Byte the NatureOfAddressIndicator value.
      */
    public byte getNatureOfAddressInd() {
        return m_natureOfAddressInd;
    }
    /** Sets the Nature of Address Indicator field of the parameter.
      * @param natureOfAddressIndicator  The nature of address indicator value, range 0 to
      * 127, see NumberParameter().
      */
    public void setNatureOfAddressInd(byte natureOfAddressIndicator)
        throws ParameterRangeInvalidException {
        if (0 <= natureOfAddressIndicator && natureOfAddressIndicator <= 127) {
            m_natureOfAddressInd = natureOfAddressIndicator;
            return;
        }
        throw new ParameterRangeInvalidException("NatureOfAddressInd value " + natureOfAddressIndicator + " out of range.");
    }
    /** Gets the Numbering plan Indicator field of the parameter.
      * @return Byte the NumberingPlanIndicator value, range 0 to 7, see
      * NumberParameter().
      */
    public byte getNumberingPlanInd() {
        return m_numberingPlan;
    }
    /** Sets the Numbering plan Indicator field of the parameter.
      * @param numberingPlanIndicator  Numbering plan indicator value, range 0 to 7, see
      * NumberParameter().
      * @exception ParameterRangeInvalidException  Thrown when the value is out of range.
      */
    public void setNumberingPlanInd(byte numberingPlanIndicator)
        throws ParameterRangeInvalidException {
        if (0 <= numberingPlanIndicator && numberingPlanIndicator <= 7) {
            m_numberingPlan = numberingPlanIndicator;
            return;
        }
        throw new ParameterRangeInvalidException("NumberingPlanIndicator value " + numberingPlanIndicator + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
      * NumberParameter class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.NumberParameter");
        b.append("\n\tm_addressSignal: " + JainSS7Utility.bytesToHex(m_addressSignal, 0, m_addressSignal.length));
        b.append("\n\tm_natureOfAddressInd: " + m_natureOfAddressInd);
        b.append("\n\tm_numberingPlan: " + m_numberingPlan);
        return b.toString();
    }
    protected byte[] m_addressSignal;
    protected byte m_natureOfAddressInd;
    protected byte m_numberingPlan;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
