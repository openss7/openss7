/*
 @(#) $RCSfile: UserTeleserviceInfoItu.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:09 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:09 $ by $Author: brian $
 */

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the itu ISUP UserTeleserviceInfo parameter.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class UserTeleserviceInfoItu implements java.io.Serializable {
    public static final byte CS_ITU_STANDARD = 0;
    public static final byte CS_ISO_IEC_STANDARD = 1;
    public static final byte CS_NATIONAL_STANDARD =2;
    public static final byte CS_STANDARD_FOR_NETWORK = 0;
    public static final byte INTERPRETATION_FIRST_HIGH_LAYER_CHARACTERISTICS_IDENTIFICATION = 0;
    public static final byte PRESENTATION_HIGH_LAYER_PROTOCOL_PROFILE = 0;
    public static final boolean EI_NEXT_OCTET_EXIST = false;
    public static final boolean EI_LAST_OCTET = true;
    public static final byte HLCI_TELEPHONY = 0;
    public static final byte HLCI_FASCIMILE_GROUP_2_3 = 0;
    public static final byte HLCI_FACSIMILE_GROUP_4 = 0;
    public static final byte HLCI_FACSIMILE_SERVICE_GROUP_4 = 0;
    public static final byte HLCI_SYNTAX_BASED_VIDEOTEX = 0;
    public static final byte HLCI_INTERNATIONLA_VIDEOTEX_INTERWORKING = 0;
    public static final byte HLCI_TELEX_SERVICE = 0;
    public static final byte HLCI_MESSAGE_HANDLING_SYSTEM = 0;
    public static final byte HLCI_OSI_APPLICATION = 0;
    public static final byte HLCI_FTAM_APPLICATION = 0;
    public static final byte HLCI_MAINTENANCE = 0;
    public static final byte HLCI_MANAGEMENT = 0;
    public static final byte HLCI_VIDEOTELEPHONY = 0;
    public static final byte HLCI_VIDEOCONFERENCING = 0;
    public static final byte HLCI_AUDIOGRAPHIC_CONFERENCING = 0;
    public static final byte HLCI_MULTIMEDIA_SERVICE = 0;
    public static final byte EHLCI_TELEPHONY = 0;
    public static final byte EHLCI_CAPABILITY_SET_OF_INITIAL_CHANEL_H221 = 0;
    public static final byte EHLCI_CAPABILITY_SET_OF_INITIAL_SUBSEQUENT_CHANEL_H221 = 0;
    public static final byte EHLCI_CAPABILITY_SET_OF_INITIAL_CHANEL_H221_with_3DOT1_KHZ_AUDIO_SPEECH = 0;
    public static final byte EHLCI_FASCIMILE_GROUP_2_3 = 0;
    public static final byte EHLCI_FACSIMILE_GROUP_4 = 0;
    public static final byte EHLCI_FACSIMILE_SERVICE_GROUP_4 = 0;
    public static final byte EHLCI_SYNTAX_BASED_VIDEOTEX = 0;
    public static final byte EHLCI_INTERNATIONLA_VIDEOTEX_INTERWORKING = 0;
    public static final byte EHLCI_TELEX_SERVICE = 0;
    public static final byte EHLCI_MESSAGE_HANDLING_SYSTEM = 0;
    public static final byte EHLCI_OSI_APPLICATION = 0;
    public static final byte EHLCI_FTAM_APPLICATION = 0;
    public static final byte EHLCI_VIDEOTELEPHONY = 0;
    public static final byte EHLCI_VIDEOCONFERENCING = 0;
    public static final byte EHLCI_AUDIOGRAPHIC_CONFERENCING = 0;
    public static final byte EHLCI_MULTIMEDIA_SERVICE = 0;
    /** Constructs a new UserTeleserviceInfoItu, parameters with default values.  */
    public UserTeleserviceInfoItu() {
    }
    /** Constructs a UserTeleserviceInfoItu class from the input parameters specified.
      * @param in_presentation  The presentation, range 0-3. <ul>
      * <li>PRESENTATION_HIGH_LAYER_PROTOCOL_PROFILE. </ul>
      * @param in_interpretation  The interpretation, range 0-7 <ul>
      * <li>INTERPRETATION_FIRST_HIGH_LAYER_CHARACTERISTICS_IDENTIFICATION. </ul>
      * @param in_codingStandard  The coding standard value, range 0-3 <ul>
      * <li>CS_ITU_STANDARD, <li>CS_ISO_IEC_STANDARD, <li>CS_NATIONAL_STANDARD and
      * <li>CS_STANDARD_FOR_NETWORK. </ul>
      * @param in_highLayerCharacteristicsIdPresent  The high layer characteristics
      * identification <ul> <li>EI_NEXT_OCTET_EXIST and <li>EI_LAST_OCTET. </ul>
      * @param in_highLayerCharacteristicsId  The high layer characteristics Id, range 0-128
      * <ul> <li>HLCI_TELEPHONY, <li>HLCI_FASCIMILE_GROUP_2_3, <li>HLCI_FACSIMILE_GROUP_4,
      * <li>HLCI_FACSIMILE_SERVICE_GROUP_4, <li>HLCI_SYNTAX_BASED_VIDEOTEX,
      * <li>HLCI_INTERNATIONLA_VIDEOTEX_INTERWORKING, <li>HLCI_TELEX_SERVICE,
      * <li>HLCI_MESSAGE_HANDLING_SYSTEM, <li>HLCI_OSI_APPLICATION,
      * <li>HLCI_FTAM_APPLICATION, <li>HLCI_MAINTENANCE, <li>HLCI_MANAGEMENT,
      * <li>HLCI_VIDEOTELEPHONY, <li>HLCI_VIDEOCONFERENCING,
      * <li>HLCI_AUDIOGRAPHIC_CONFERENCING and <li>HLCI_MULTIMEDIA_SERVICE. </ul>
      * @param in_extendedHighLayerCharacteristicsId  The extended high layer
      * characteristics Id, range 0-128 <ul> <li>EHLCI_TELEPHONY,
      * <li>EHLCI_CAPABILITY_SET_OF_INITIAL_CHANEL_H221,
      * <li>EHLCI_CAPABILITY_SET_OF_INITIAL_SUBSEQUENT_CHANEL_H221,
      * <li>EHLCI_CAPABILITY_SET_OF_INITIAL_CHANEL_H221_with_3DOT1_KHZ_AUDIO_SPEECH,
      * <li>EHLCI_FASCIMILE_GROUP_2_3, <li>EHLCI_FACSIMILE_GROUP_4,
      * <li>EHLCI_FACSIMILE_SERVICE_GROUP_4, <li>EHLCI_SYNTAX_BASED_VIDEOTEX,
      * <li>EHLCI_INTERNATIONLA_VIDEOTEX_INTERWORKING, <li>EHLCI_TELEX_SERVICE,
      * <li>EHLCI_MESSAGE_HANDLING_SYSTEM, <li>EHLCI_OSI_APPLICATION,
      * <li>EHLCI_FTAM_APPLICATION, <li>EHLCI_VIDEOTELEPHONY, <li>EHLCI_VIDEOCONFERENCING,
      * <li>EHLCI_AUDIOGRAPHIC_CONFERENCING and <li>EHLCI_MULTIMEDIA_SERVICE. </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public UserTeleserviceInfoItu(byte in_presentation,
                              byte in_interpretation,
                              byte in_codingStandard,
                              boolean in_highLayerCharacteristicsIdPresent,
                              byte in_highLayerCharacteristicsId,
                              byte in_extendedHighLayerCharacteristicsId)
        throws ParameterRangeInvalidException {
        this.setPresentation(in_presentation);
        this.setInterpretation(in_interpretation);
        this.setCodingStandard(in_codingStandard);
        this.setHighLayerCharacteristicsIdPresent(in_highLayerCharacteristicsIdPresent);
        this.setHighLayerCharacteristicsId(in_highLayerCharacteristicsId);
        this.setExtendedHighLayerCharacteristicsId(in_extendedHighLayerCharacteristicsId);
    }
    /** Gets the Presentation field of UserTeleserviceInfo parameter.
      * @return Presentation, range 0-3, see UserTeleserviceInfoItu().
      */
    public byte getPresentation() {
        return m_presentation;
    }
    /** Sets the Presentation field of UserTeleserviceInfo parameter.
      * @param in_presentation  The Presentation value, range 0-3, see
      * UserTeleserviceInfoItu().
      */
    public void setPresentation(byte in_presentation)
        throws ParameterRangeInvalidException {
        if (0 <= in_presentation && in_presentation <= 3) {
            m_presentation = in_presentation;
            return;
        }
        throw new ParameterRangeInvalidException("Presentation value " + in_presentation + " out of range.");
    }
    /** Gets the Interpretation field of UserTeleserviceInfo parameter.
      * @return Interpretation, range 0-7, see UserTeleserviceInfoItu().
      */
    public byte getInterpretation() {
        return m_interpretation;
    }
    /** Sets the Interpretation field of UserTeleserviceInfo parameter.
      * @param in_interpretation  The Interpretation value, range 0-7, see
      * UserTeleserviceInfoItu().
      */
    public void setInterpretation(byte in_interpretation)
        throws ParameterRangeInvalidException {
        if (0 <= in_interpretation && in_interpretation <= 7) {
            m_interpretation = in_interpretation;
            return;
        }
        throw new ParameterRangeInvalidException("Interpretation value " + in_interpretation + " out of range.");
    }
    /** Gets the CodingStandard field of UserTeleserviceInfo parameter.
      * @return CodingStandard, range 0-3, see UserTeleserviceInfoItu().
      */
    public byte getCodingStandard() {
        return m_codingStandard;
    }
    /** Sets the Coding Standard field of UserTeleserviceInfo parameter.
      * @param in_codingStandard  The CodingStandard value, range 0-3, see
      * UserTeleserviceInfoItu().
      */
    public void setCodingStandard(byte in_codingStandard)
        throws ParameterRangeInvalidException {
        if (0 <= in_codingStandard && in_codingStandard <= 3) {
            m_codingStandard = in_codingStandard;
            return;
        }
        throw new ParameterRangeInvalidException("CodingStandard value " + in_codingStandard + " out of range.");
    }
    /** Gets the HighLayerCharacteristicsIdPresent field of UserTeleserviceInfo parameter.
      * @return HighLayerCharacteristicsIdPresent, see UserTeleserviceInfoItu().
      */
    public boolean getHighLayerCharacteristicsIdPresent() {
        return m_hlcip;
    }
    /** Sets the HighLayerCharacteristicsIdPresent field of UserTeleserviceInfo parameter.
      * @param in_highLayerCharacteristicsIdPresent  The HighLayerCharacteristicsIdPresent
      * value, see UserTeleserviceInfoItu().
      */
    public void setHighLayerCharacteristicsIdPresent(boolean in_highLayerCharacteristicsIdPresent) {
        m_hlcip = in_highLayerCharacteristicsIdPresent;
    }
    /** Gets the HighLayerCharacteristicsId field of UserTeleserviceInfo parameter.
      * @return HighLayerCharacteristicsId, range 0-127, see UserTeleserviceInfoItu().
      */
    public byte getHighLayerCharacteristicsId() {
        return m_hlcid;
    }
    /** Sets the HighLayerCharacteristicsId field of UserTeleserviceInfo parameter.
      * @param in_highLayerCharacteristicsId  The HighLayerCharacteristicsId value, range
      * 0-127, see UserTeleserviceInfoItu().
      */
    public void setHighLayerCharacteristicsId(byte in_highLayerCharacteristicsId)
        throws ParameterRangeInvalidException {
        if (0 <= in_highLayerCharacteristicsId && in_highLayerCharacteristicsId <= 127) {
            m_hlcid = in_highLayerCharacteristicsId;
            return;
        }
        throw new ParameterRangeInvalidException("HighLayerCharacteristicsId value " + in_highLayerCharacteristicsId + " out of range.");
    }
    /** Gets the ExtendedHighLayerCharacteristicsId field of UserTeleserviceInfo
      * parameter.
      * @return ExtendedHighLayerCharacteristicsId, range 0-127, see
      * UserTeleserviceInfoItu().
      */
    public byte getExtendedHighLayerCharacteristicsId() {
        return m_ehlcid;
    }
    /** Sets the ExtendedHighLayerCharacteristicsId field of UserTeleserviceInfo
      * parameter.
      * @param in_extendedHighLayerCharacteristicsId  The
      * ExtendedHighLayerCharacteristicsId value, range 0-127, see
      * UserTeleserviceInfoItu().
      */
    public void setExtendedHighLayerCharacteristicsId(byte in_extendedHighLayerCharacteristicsId)
        throws ParameterRangeInvalidException {
        if (0 <= in_extendedHighLayerCharacteristicsId && in_extendedHighLayerCharacteristicsId <= 127) {
            m_ehlcid = in_extendedHighLayerCharacteristicsId;
            return;
        }
        throw new ParameterRangeInvalidException("ExtendedHighLayerCharacteristicsId value " + in_extendedHighLayerCharacteristicsId + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
      * UserTeleserviceInfoItu class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.UserTeleserviceInfoItu");
        b.append("\n\tm_presentation: " + m_presentation);
        b.append("\n\tm_interpretation: " + m_interpretation);
        b.append("\n\tm_codingStandard: " + m_codingStandard);
        b.append("\n\tm_hlcip: " + m_hlcip);
        b.append("\n\tm_hlcid: " + m_hlcid);
        b.append("\n\tm_ehlcid: " + m_ehlcid);
        return b.toString();
    }
    protected byte m_presentation;
    protected byte m_interpretation;
    protected byte m_codingStandard;
    protected boolean m_hlcip;
    protected byte m_hlcid;
    protected byte m_ehlcid;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
