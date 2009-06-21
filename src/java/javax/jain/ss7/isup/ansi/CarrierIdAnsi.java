/*
 @(#) $RCSfile: CarrierIdAnsi.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:06 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:06 $ by $Author: brian $
 */

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;


/** A class representing the ANSI ISUP Carrier Identification parameter.
  * This class provides the methods to access the sub-fields of this parameter.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CarrierIdAnsi implements java.io.Serializable {
    public static final byte TNI_NATIONAL_NETWORK = 0x00; 
    public static final byte NIP_UNKNOWN = 0x00; 
    public static final byte NIP_3_DIGIT_CARRIER_IDENTIFICATION = 0x01; 
    public static final byte NIP_4_DIGIT_CARRIER_IDENTIFICATION = 0x02; 
    /** Constructs an ANSI CarrierId class from the input parameters specified.
      * @param in_nwIdPlan  The network identification plan, range 0 - 15; <ul>
      * <li>TNI_NATIONAL_NETWORK.  </ul>
      * @param in_typeOfNwId  The type of network identification, range 0 - 15; <ul>
      * <li>NIP_UNKNOWN, <li>NIP_3_DIGIT_CARRIER_IDENTIFICATION and
      * <li>NIP_4_DIGIT_CARRIER_IDENTIFICATION.  </ul>
      * @param in_digits  The BCD coded digits as specified in ANSI.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public CarrierIdAnsi(byte in_nwIdPlan, byte in_typeOfNwId, byte[] in_digits) throws ParameterRangeInvalidException {
        this();
        this.setNetworkIdPlan(in_nwIdPlan);
        this.setTypeOfNetworkId(in_typeOfNwId);
        this.setDigits(in_digits);
    }
    /** Constructs a new ANSI CarrierId class, parameters with default values.  */
    public CarrierIdAnsi() {
    }
    /** Gets the Network Identification plan field of the parameter.
      * @return The network id. plan, range 0 - 15, see CarrierIdAnsi().  */
    public byte getNetworkIdPlan() {
        return m_nip;
    }
    /** Sets the Network Identification Plan field of the parameter.
      * @param networkIdPlan  The network identification plan, range 0 - 15, see
      * CarrierIdAnsi().
      * @exception ParameterRangeInvalidException  Thrown if the sub-field is out of
      * the specified range.  */
    public void setNetworkIdPlan(byte networkIdPlan) throws ParameterRangeInvalidException {
        if (0 > m_nip || m_nip > 15)
            throw new ParameterRangeInvalidException("Network Plan " + networkIdPlan + " is out of range.");
        m_nip = networkIdPlan;
    }
    /** Gets the Type of Network Identification field of the parameter.
      * @return The type of network id, range 0 - 15, see CarrierIdAnsi().  */
    public byte getTypeOfNetworkId() {
        return m_ton;
    }
    /** Sets the Type of Network Identification field of the parameter.  
      * @param typeOfNetworkId  The type of network id., range 0 - 15, see
      * CarrierIdAnsi().
      * @exception ParameterRangeInvalidException  Thrown if the sub-field is out of
      * the specified range.  */
    public void setTypeOfNetworkId(byte typeOfNetworkId) throws ParameterRangeInvalidException {
        if (0 > m_ton || m_ton > 15)
            throw new ParameterRangeInvalidException("Network Type " + typeOfNetworkId + " is out of range.");
        m_ton = typeOfNetworkId;
    }
    /** Gets the Digits field of the parameter. Each digit is represented as one byte,
      * e.g. number 9876543210 can be represented as digits[0]=9, digits[1]=8,
      * digits[2]=7, and so on.
      * @return The digits array with one digit stored in a byte.  */
    public byte[] getDigits() {
        return m_dig;
    }
    /** Sets the Digits field of the parameter. Each digit is represented as one byte,
      * e.g. number 9876543210 can be represented as digits[0]=9, digits[0]=8,
      * digits[0]=7, and so on.
      * @param aDigits  Array of digits with one digit stored in a byte.  */
    public void setDigits(byte[] aDigits) {
        m_dig = aDigits;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the CarrierIdAnsi class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\nCarrierIdAnsi:");
        b.append("\n\tNetwork Id Plan: " + m_nip);
        b.append("\n\tType of Network Id: " + m_ton);
        b.append("\n\tDigits: " + m_dig);
        return b.toString();
    }

    private byte m_nip;
    private byte m_ton;
    private byte[] m_dig;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
