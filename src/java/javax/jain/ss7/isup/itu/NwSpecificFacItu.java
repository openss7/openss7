/*
 @(#) src/java/javax/jain/ss7/isup/itu/NwSpecificFacItu.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ITU ISUP Network Specific Facility parameter.
  * This has methods for accessing the sub-fields of this parameter.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class NwSpecificFacItu implements java.io.Serializable {
    public static final byte TNI_NATIONAL_NETWORK_IDENTIFICATION = 2;
    public static final byte TNI_INTERNATIONAL_NETWORK_IDENTIFICATION = 3;
    public static final boolean EI_NEXT_OCTET_EXIST = false;
    public static final boolean EI_LAST_OCTET = true;
    /** Constructs a NwSpecificFacItu.  */
    public NwSpecificFacItu() {
    }
    /** Constructs a NwSpecificFacItu class from the input parameters specified.
      * @param in_nwIdLen  The length Of network identification value, range 0 to 255.
      * @param in_nwIdPlan  The network identification plan value, ranges from 0 to 15.
      * @param in_typeOfNwId  The type of network identification, range 0 to 7. <ul>
      * <li>TNI_NATIONAL_NETWORK_IDENTIFICATION and
      * <li>TNI_INTERNATIONAL_NETWORK_IDENTIFICATION. </ul>
      * @param in_extNwId  The network id extension flag. <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_nwId  The network identification.
      * @param in_nwSpecificFac  The network specific facility.
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public NwSpecificFacItu(short in_nwIdLen, byte in_nwIdPlan, byte in_typeOfNwId,
            boolean in_extNwId, byte[] in_nwId, byte[] in_nwSpecificFac)
        throws ParameterRangeInvalidException {
        this.setNetworkIdLength(in_nwIdLen);
        this.setNetworkIdPlan(in_nwIdPlan);
        this.setTypeOfNetworkId(in_typeOfNwId);
        this.setNetworkIdExtensionInd(in_extNwId);
        this.setNetworkId(in_nwId);
        this.setNetworkSpecificFac(in_nwSpecificFac);
    }
    /** Gets the Length Of Network Indentification field of the parameter.
      * @return short. The LenghtOfNetwork Indentification value range is 0 to 255.
      */
    public short getNetworkIdLength() {
        return m_networkIdLength;
    }
    /** Sets the Length Of Network Indentification field of the parameter.
      * @param networkIdLength  The Length Of Network Identification value, range 0 to
      * 255.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setNetworkIdLength(short networkIdLength)
        throws ParameterRangeInvalidException {
        if (0 <= networkIdLength && networkIdLength <= 255) {
            m_networkIdLength = networkIdLength;
            return;
        }
        throw new ParameterRangeInvalidException("NetworkIdLength value " + networkIdLength + " out of range.");
    }
    /** Gets the Network Identification Plan field of the parameter.
      * @return byte. The NetworkIdentificationPlan value ranges from 0 to 15.
      */
    public byte getNetworkIdPlan() {
        return m_networkIdentificationPlan;
    }
    /** Sets the Network Identification Plan field of the parameter.
      * @param aNetworkIdentificationPlan  The network id plan value, range 0 to 15.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setNetworkIdPlan(byte aNetworkIdentificationPlan)
        throws ParameterRangeInvalidException {
        if (0 <= aNetworkIdentificationPlan && aNetworkIdentificationPlan <= 15) {
            m_networkIdentificationPlan = aNetworkIdentificationPlan;
            return;
        }
        throw new ParameterRangeInvalidException("NetworkIdPlan value " + aNetworkIdentificationPlan + " out of range.");
    }
    /** Gets the Type Of Network Identification field of the parameter.
      * @return The TypeOfNetworkIdentification value, range 0 to 7, see
      * NwSpecificFacItu().
      */
    public byte getTypeOfNetworkId() {
        return m_typeOfNetworkIdentification;
    }
    /** Sets the Type Of Network Identification field of the parameter.
      * @param aTypeOfNetworkIdentification  The TypeOfNetworkIdentification value, range
      * 0 to 7, see NwSpecificFacItu().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setTypeOfNetworkId(byte aTypeOfNetworkIdentification)
        throws ParameterRangeInvalidException {
        if (0 <= aTypeOfNetworkIdentification && aTypeOfNetworkIdentification <= 7) {
            m_typeOfNetworkIdentification = aTypeOfNetworkIdentification;
            return;
        }
        throw new ParameterRangeInvalidException("TypeOfNetworkId value " + aTypeOfNetworkIdentification + " out of range.");
    }
    /** Gets the network id extension flag.
      * @return The Extension flag value, see NwSpecificFacItu().
      */
    public boolean getNetworkIdExtensionInd() {
        return m_nwIdExtension;
    }
    /** Sets the network id extension flag.
      * @param nwIdExtension  Network id extension flag value, see NwSpecificFacItu().
      */
    public void setNetworkIdExtensionInd(boolean nwIdExtension) {
        m_nwIdExtension = nwIdExtension;
    }
    /** Gets the Network Identification field of the parameter.
      * @return byte[] the Network Identification value.
      */
    public byte[] getNetworkId() {
        return m_networkIdentification;
    }
    /** Sets the Network Identification field of the parameter.
      * @param aNetworkIdentification  The Network Identification value.
      */
    public void setNetworkId(byte[] aNetworkIdentification)
        throws ParameterRangeInvalidException {
        m_networkIdentification = aNetworkIdentification;
    }
    /** Gets the Network Specific Facility field of the parameter.
      * @return byte[] the NwSpecificFac value.
      */
    public byte[] getNetworkSpecificFac() {
        return m_wSpecificFac;
    }
    /** Sets the Network Specific Facility field of the parameter.
      * @param aNwSpecificFac  The Network Specific Fac value.
      */
    public void setNetworkSpecificFac(byte[] aNwSpecificFac) {
        m_wSpecificFac = aNwSpecificFac;
    }
    /** The toString method retrieves a string containing the values of the members of the
      * NwSpecificFacItu class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.NwSpecificFacItu");
        b.append("\n\tm_networkIdLength: " + m_networkIdLength);
        b.append("\n\tm_networkIdentificationPlan: " + m_networkIdentificationPlan);
        b.append("\n\tm_typeOfNetworkIdentification: " + m_typeOfNetworkIdentification);
        b.append("\n\tm_nwIdExtension: " + m_nwIdExtension);
        b.append("\n\tm_networkIdentification: " + JainSS7Utility.bytesToHex(m_networkIdentification, 0, m_networkIdentification.length));
        b.append("\n\tm_wSpecificFac: " + JainSS7Utility.bytesToHex(m_wSpecificFac, 0, m_wSpecificFac.length));
        return b.toString();
    }
    protected short m_networkIdLength;
    protected byte m_networkIdentificationPlan;
    protected byte m_typeOfNetworkIdentification;
    protected boolean m_nwIdExtension;
    protected byte[] m_networkIdentification;
    protected byte[] m_wSpecificFac;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
