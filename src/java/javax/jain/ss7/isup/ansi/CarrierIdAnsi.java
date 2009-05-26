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

/* ***************************************************************************/


public class CarrierIdAnsi extends java.lang.Object implements java.io.Serializable {
    public static final byte TNI_NATIONAL_NETWORK = 0x00; 
    public static final byte NIP_UNKNOWN = 0x00; 
    public static final byte NIP_3_DIGIT_CARRIER_IDENTIFICATION = 0x01; 
    public static final byte NIP_4_DIGIT_CARRIER_IDENTIFICATION = 0x02; 
    /**
      * Constructs an ANSI CarrierId class from the input parameters specified.
      * @param in_nwIdPlan  The network identification plan, range 0 - 15.
      * <li>TNI_NATIONAL_NETWORK
      * @param in_typeOfNwId  The type of network identification, range 0 - 15.
      * <li>NIP_UNKNOWN
      * <li>NIP_3_DIGIT_CARRIER_IDENTIFICATION
      * <li>NIP_4_DIGIT_CARRIER_IDENTIFICATION
      * @param in_digits  The BCD coded digits as specified in ANSI.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public CarrierIdAnsi(byte in_nwIdPlan, byte in_typeOfNwId, byte[] in_digits) throws ParameterRangeInvalidException {
        this();
        this.setnetworkIdPlan(in_nwIdPlan);
        this.setTypeOfNetworkId(in_typeOfNwId);
        this.setDigits(in_digits);
    }
    /**
      * Constructs a new ANSI CarrierId class, parameters with default values.
      */
    public CarrierIdAnsi() {
    }
    /**
      * Gets the Network Identification plan field of the parameter.
      * @return The network id. plan, range 0 - 15, see CarrierIdAnsi().
      */
    public byte getNetworkIdPlan() {
        return m_nip;
    }
    /**
      * Sets the Network Identification Plan field of the parameter.
      * @param networkIdPlan  The network identification plan, range 0 - 15, see CarrierIdAnsi().
      * @exception ParameterRangeInvalidException  Thrown if the sub-field is out of the specified range.
      */
    public void setNetworkIdPlan(byte networkIdPlan) throws ParameterRangeInvalidException {
        if (0 > m_npi || m_npi > 15)
            throw ParameterRangeInvalidException("Network Plan " + networkIdPlan + " is out of range.");
        m_nip = networkIdPlan;
    }
    /**
      * Gets the Type of Network Identification field of the parameter.
      * @return The type of network id, range 0 - 15, see CarrierIdAnsi().
      */
    public byte getTypeOfNetworkId() {
        return m_ton;
    }
    /**
      * Sets the Type of Network Identification field of the parameter.  
      * @param typeOfNetworkId  The type of network id., range 0 - 15, see CarrierIdAnsi().
      * @exception ParameterRangeInvalidException  Thrown if the sub-field is out of the specified range.
      */
    public void setTypeOfNetworkId(byte typeOfNetworkId) throws ParameterRangeInvalidException {
        if (0 > m_ton || m_ton > 15)
            throw ParameterRangeInvalidException("Network Type " + typeOfNetworkId + " is out of range.");
        m_ton = typeOfNetworkId;
    }
    /**
      * Gets the Digits field of the parameter. Each digit is represented as one byte, e.g. number 9876543210 can be
      * represented as digits[0]=9, digits[1]=8, digits[2]=7, and so on.
      * @return The digits array with one digit stored in a byte.
      */
    public byte[] getDigits() {
        return m_dig;
    }
    /**
      * Sets the Digits field of the parameter. Each digit is represented as one byte, e.g. number 9876543210 can be
      * represented as digits[0]=9, digits[0]=8, digits[0]=7, and so on.
      * @param aDigits  Array of digits with one digit stored in a byte.
      */
    public void setDigits(byte[] aDigits) {
        m_dig = aDigits;
    }
    /**
      * The toString method retrieves a string containing the values of the members of the CarrierIdAnsi class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\nCarrierIdAnsi:");
        b.append("\n\tNetwork Id Plan: " + m_nip);
        b.append("\n\tType of Network Id: " + m_ton);
        b.append("\n\tDigits: " + m_dig);
        return b.toString();
    }

    private byte m_nip = null;
    private byte m_ton = null;
    private byte[] m_dig = new byte[];
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
