/*
 @(#) $RCSfile: GenericDigits.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:04 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:04 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing a Generic Digits parameter.
  * This class is common to ITU and ANSI variants and provides methods to access all
  * the sub-fields of the GenericDigits parameter.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class GenericDigits implements java.io.Serializable {
    public static final byte TOD_ACCOUNT_CODE = 0;
    public static final byte TOD_AUTHORIZATION_CODE = 1;
    public static final byte TOD_PRIVATE_NETWORK_TRAVELLING_CLASS_MARK = 2;
    public static final byte TOD_BUSINESS_COMMUNICATION_GROUP_IDENTITY = 3;
    public static final byte TOD_BILL_TO_NUMBER = 15;
    public static final byte ES_BCD_EVEN = 0;
    public static final byte ES_BCD_ODD = 1;
    public static final byte ES_IA5_CHARACTER = 2;
    public static final byte ES_BINARY_CODED = 3;
    /** Constructs a new GenericDigits taking no parameters.
      * This constructor sets all the sub-field values to their spare value (which is
      * zero).  */
    public GenericDigits() {
    }
    /** Constructor for GenericDigits.
      * @param typeOfDigits  The type of Digits, range 0 to 31; <ul>
      * <li>TOD_ACCOUNT_CODE, <li>TOD_AUTHORIZATION_CODE,
      * <li>TOD_PRIVATE_NETWORK_TRAVELLING_CLASS_MARK,
      * <li>TOD_BUSINESS_COMMUNICATION_GROUP_IDENTITY (ITU only) and
      * <li>TOD_BILL_TO_NUMBER (ANSI only). </ul>
      * @param encodingScheme  The encoding scheme, range 0 to 7; <ul>
      * <li>ES_BCD_EVEN, <li>ES_BCD_ODD, <li>ES_IA5_CHARACTER and <li>ES_BINARY_CODED.
      * </ul>
      * @param digits  The value of the digits.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public GenericDigits(byte typeOfDigits, byte encodingScheme, byte[] digits)
        throws ParameterRangeInvalidException {
        this();
        this.setTypeOfDigits(typeOfDigits);
        this.setEncodingScheme(encodingScheme);
        this.setDigits(digits);
    }
    /** Gets the Digits field of the parameter.
      * The digits are stored in one byte for each digit. So when the digits are
      * received from the network in BCD odd or BCD even format, it is converted into
      * binary format. In case of IA5 character encoding scheme, the digits are
      * returned to the application as received from the network.
      * @return The Digits value.  */
    public byte[] getDigits() {
        return m_digits;
    }
    /** Sets the Digit field of the parameter.
      * Refer to getDigits method for more details.  */
    public void setDigits(byte[] digits) {
        m_digits = digits;
    }
    /** Gets the Encoding Scheme field of the parameter.
      * When the encoding scheme for the digits is BCD digits then it is converted to
      * binary format, so the encoding scheme will not return the values BCD even and
      * BCD odd given in the ITU and ANSI specifications. This is to keep in line with
      * the JAIN ISUP API standard where in all the digits parameter are sent
      * byte-by-byte.
      * @return byte The Encoding Scheme, range 0 to 7, see GenericDigits().  */
    public byte getEncodingScheme() {
        return m_encodingScheme;
    }
    /** Sets the Encoding Scheme field of the parameter.
      * Refer to getEncodingScheme method for more details.
      * @param encodingScheme  The Encoding Scheme value, range 0 to 7, see
      * GenericDigits().
      * @exception ParameterRangeInvalidException  Thrown when the value is out of
      * range.  */
    public void setEncodingScheme(byte encodingScheme)
        throws ParameterRangeInvalidException {
        switch (encodingScheme) {
            case ES_BCD_EVEN:
            case ES_BCD_ODD:
            case ES_IA5_CHARACTER:
            case ES_BINARY_CODED:
                m_encodingScheme = encodingScheme;
                return;
        }
        throw new ParameterRangeInvalidException("Encoding Scheme value " + encodingScheme + " out of range.");
    }
    /** Gets the Type of Digits field of the parameter.
      * @return byte The TypeOfDigits value, range 0 to 31, see GenericDigits().  */
    public byte getTypeOfDigits() {
        return m_typeOfDigits;
    }
    /** Sets the Type of Digits field of the parameter.
      * @param typeOfDigits  The TypeOfDigits value, range 0 to 31, see
      * GenericDigits().
      * @exception ParameterRangeInvalidException  Thrown when the value is out of
      * range.  */
    public void setTypeOfDigits(byte typeOfDigits)
        throws ParameterRangeInvalidException {
        switch (typeOfDigits) {
            case TOD_ACCOUNT_CODE:
            case TOD_AUTHORIZATION_CODE:
            case TOD_PRIVATE_NETWORK_TRAVELLING_CLASS_MARK:
            case TOD_BUSINESS_COMMUNICATION_GROUP_IDENTITY:
            case TOD_BILL_TO_NUMBER:
                m_typeOfDigits = typeOfDigits;
                return;
        }
        throw new ParameterRangeInvalidException("TypeOfDigits value " + typeOfDigits + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of
      * the GenericDigits class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.GenericDigits");
        b.append("\n\tm_typeOfDigits: " + m_typeOfDigits);
        b.append("\n\tm_encodingScheme: " + m_encodingScheme);
        b.append("\n\tm_digits: ");
        b.append(JainSS7Utility.bytesToHex(m_digits, 0, m_digits.length));
        return b.toString();
    }
    protected byte m_typeOfDigits;
    protected byte m_encodingScheme;
    protected byte[] m_digits;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
