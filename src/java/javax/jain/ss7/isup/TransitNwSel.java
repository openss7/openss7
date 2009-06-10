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

/** A class representing the ISUP Transit Network Selection parameter.
  * This class defines the methods to access the common sub-fields to ITU and ANSI
  * variants for this parameter. The ITU variant has no other sub-fields, but the ANSI
  * variant inherits from the core package to access ANSI specific sub-fields.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class TransitNwSel implements java.io.Serializable {
    public static final byte TNI_CCITT_IDENTIFICATION = 0;
    public static final byte TNI_CCITT_NATIONAL_NETWORK_IDENTIFICATION = 2;
    public static final byte NIP_CCITT_UNKNOWN = 0;
    public static final byte NIP_CCITT_PUBLIC_DATA_NETWORK_IDENTIFICATION_CODE = 3;
    public static final byte NIP_CCITT_PUBLIC_LAND_MOBILE_NETWORK_IDENTIFICATION_CODE = 6;
    public static final byte NIP_NATIONAL_UNKNOWN = 0;
    public static final byte NIP_NATIONAL_3_DIGIT_CARRIER_IDENTIFICATION_WITH_CIRCUIT_CODE = 1;
    public static final byte NIP_NATIONAL_4_DIGIT_CARRIER_IDENTIFICATION_WITH_CIRCUIT_CODE = 2;
    /** Constructs a new TransitNwSel class, parameters with default values.  */
    public TransitNwSel() {
    }
    /** Constructs a TransitNwSel class from the input parameters specified.
      * @param in_nwIdPlan  The network identification plan, range 0 to 15 <ul>
      * <li>NIP_CCITT_UNKNOWN, <li>NIP_CCITT_PUBLIC_DATA_NETWORK_IDENTIFICATION_CODE,
      * <li>NIP_CCITT_PUBLIC_LAND_MOBILE_NETWORK_IDENTIFICATION_CODE,
      * <li>NIP_NATIONAL_UNKNOWN (ANSI only),
      * <li>NIP_NATIONAL_3_DIGIT_CARRIER_IDENTIFICATION_WITH_CIRCUIT_CODE (ANSI only) and
      * <li>NIP_NATIONAL_4_DIGIT_CARRIER_IDENTIFICATION_WITH_CIRCUIT_CODE (ANSI only).
      * </ul>
      * @param in_typeOfNwId - The type of network identification, range 0 to 7 <ul>
      * <li>TNI_CCITT_IDENTIFICATION and <li>TNI_CCITT_NATIONAL_NETWORK_IDENTIFICATION.
      * </ul>
      * @param in_nwId - The network identification.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public TransitNwSel(byte in_nwIdPlan, byte in_typeOfNwId, byte[] in_nwId)
        throws ParameterRangeInvalidException {
        this.setNetworkIdPlan(in_nwIdPlan);
        this.setTypeOfNetworkId(in_typeOfNwId);
        this.setNwId(in_nwId);
    }
    /** Gets the Network Identification Plan field of the parameter.
      * @return byte. The NetworkIdentificationPlan value, range 0 to 15, see
      * TransitNwSel().
      */
    public byte getNetworkIdPlan() {
        return m_nip;
    }
    /** Sets the Network Identification Plan field of the parameter.
      * @param aNetworkIdentificationPlan  The network id plan value, range 0 to 15, see
      * TransitNwSel().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setNetworkIdPlan(byte aNetworkIdentificationPlan)
        throws ParameterRangeInvalidException {
        if (0 <= aNetworkIdentificationPlan && aNetworkIdentificationPlan <= 15) {
            m_nip = aNetworkIdentificationPlan;
            return;
        }
        throw new ParameterRangeInvalidException("NetworkIdPlan value " + aNetworkIdentificationPlan + " out of range.");
    }
    /** Gets the Type Of Network Identification field of the parameter.
      * @return Byte containing the TypeOfNetworkIdentification value, range 0 to 7, see
      * TransitNwSel().
      */
    public byte getTypeOfNetworkId() {
        return m_ton;
    }
    /** Sets the Type Of Network Identification field of the parameter.
      * @param aTypeOfNetworkIdentification  The TypeOfNetworkIdentification value, range
      * 0 to 7, see TransitNwSel().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setTypeOfNetworkId(byte aTypeOfNetworkIdentification)
        throws ParameterRangeInvalidException {
        if (0 <= aTypeOfNetworkIdentification && aTypeOfNetworkIdentification <= 7) {
            m_ton = aTypeOfNetworkIdentification;
            return;
        }
        throw new ParameterRangeInvalidException("TypeOfNetworkId value " + aTypeOfNetworkIdentification + " out of range.");
    }
    /** Gets the Network Identification field of the parameter.
      * @return byte[] the Network Identification value, where each byte has a network id
      * digit coded in binary format.
      */
    public byte[] getNwId() {
        return m_nid;
    }
    /** Sets the Network Identification field of the parameter.
      * @param aNetworkIdentification  The Network Identification value, where each byte
      * has a network id digit coded in binary format.
      */
    public void setNwId(byte[] aNetworkIdentification) {
        m_nid = aNetworkIdentification;
    }
    /** The toString method retrieves a string containing the values of the members of the
      * TransitNwSel class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.TransitNwSel");
        return b.toString();
    }
    protected byte m_nip;
    protected byte m_ton;
    protected byte[] m_nid;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
