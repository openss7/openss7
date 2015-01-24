/*
 @(#) src/java/javax/jain/ss7/isup/UserServiceInfo.java <p>
 
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

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ISUP User Service Information Prime parameter. This class
  * provides methods common to ITU and ANSI variants.  The ITU variant has no
  * additional sub-fields while the ANSI variant class inherits from the core class
  * and adds its own specific sub-fields.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class UserServiceInfo extends UserServiceInfoCommon {
    /** Constructs a new UserServiceInfo class, parameters with default values.  */
    public UserServiceInfo() {
    }
    /** Constructs a UserServiceInfoCommon class from the input parameters specified.
      * @param in_infoTransferCap  The information transfer capability, range 0 to 31;
      * <ul> <li>ITC_SPEECH, <li>ITC_UNRESTRICTED_DIGITAL_INFORMATION,
      * <li>ITC_RESTRICTED_DIGITAL_INFORMATION, <li>ITC_3DOT1_KHZ_AUDIO = 16,
      * <li>ITC_UNRESTRICTED_DIGITAL_INFORMATION_WITH_TONES_ANNOUNCE MENTS and
      * <li>ITC_VIDEO (ITU only). </ul>
      * @param in_codingStd  The coding standard, range 0 to 3; <ul>
      * <li>CS_ITU_STANDARD, <li>CS_ISO_IEC_STANDARD (ITU only),
      * <li>CS_NATIONAL_STANDARD and <li>CS_STANDARD_FOR_NETWORK (ITU only). </ul>
      * @param in_ext1  The extension 1; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_infoTransferRate  The information transfer rate, range 0 to 31; <ul>
      * <li>ITR_64_KBPS, <li>ITR_2_by_64_KBPS (ITU only), <li>ITR_384_KBPS,
      * <li>ITR_1472_KBPS, <li>ITR_1536_KBPS, <li>ITR_1920_KBPS and
      * <li>ITR_MULTI_RATE. </ul>
      * @param in_transferMode  The transfer mode value, range 0 to 3; <ul>
      * <li>TM_CIRCUIT_MODE and <li>TM_PACKET_MODE. </ul>
      * @param in_ext2  The extensition 2; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_establishment  The establishment, range 0 to 3; <ul>
      * <li>ESTABLISHMENT_DEMAND. </ul>
      * @param in_config  The configuration, range 0 to 3; <ul>
      * <li>CONFIGURATION_POINT_TO_POINT. </ul>
      * @param in_struct  The structure, range 0 to 7; <ul> <li>STRUCTURE_DEFAULT,
      * <li>STRUCTURE_8_KHZ_INTEGRITY, <li>STRUCTURE_SERVICE_DATA_UNIT_INTEGRITY and
      * <li>STRUCTURE_UNSTRUCTURED. </ul>
      * @param in_ext2a  The extension 2a; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_infoTransferRateDestToOrig  The information transfer rate from
      * Destination to Origination, range 0 to 31.
      * @param in_symmetry  The symmetry, range 0 to 3; <ul>
      * <li>SYMMETRY_BIDIRECTIONAL_SYMMETRIC. </ul>
      * @param in_ext2b  The extension 2b; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_userInfoLayer1  The user information layer 1 protocol, range 0 to
      * 31; <ul> <li>UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_V110_X30,
      * <li>UIL1P_RECOMMENDATION_G711_U_LA, <li>UIL1P_RECOMMENDATION_G711_A_LAW (ITU
      * only), <li>UIL1P_RECOMMENDATION_G721_I460 (ITU only),
      * <li>UIL1P_RECOMMENDATION_H221_H242 (ITU only),
      * <li>UIL1P_RECOMMENDATION_G722_G725_7KHZ_AUDIO (ANSI only),
      * <li>UIL1P_RECOMMENDATION_H223_H245 (ITU only),
      * <li>UIL1P_RECOMMENDATION_H223_H245,
      * <li>UIL1P_NON_ITU_STANDARDIZED_RATE_ADAPTION,
      * <li>UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_V120 and
      * <li>UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_X31_HDLC_FLAG_STUFFING. </ul>
      * @param in_layer1Id  The layer 1 identification, range 0 to 3.
      * @param in_ext3  The extension 3; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_rateMultiplier  The Rate Multiplier, range 0 to 127.
      * @param in_ext21  The extension 2_1; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_userRate  The User rate, range 0 to 31; <ul> <li>UR_RATE_I460,
      * <li>UR_0DOT6_KBPS, <li>UR_1DOT2_KBPS, <li>UR_2DOT4_KBPS, <li>UR_3DOT6_KBPS,
      * <li>UR_4DOT8_KBPS, <li>UR_7DOT2_KBPS, <li>UR_8DOT0_KBPS, <li>UR_9DOT6_KBPS,
      * <li>UR_14DOT4_KBPS, <li>UR_16DOT0_KBPS, <li>UR_19DOT2_KBPS,
      * <li>UR_32DOT0_KBPS, <li>UR_38DOT4_KBPS (ITU only), <li>UR_48DOT0_KBPS,
      * <li>UR_56DOT0_KBPS, <li>UR_64DOT0_KBPS (ANSI only), <li>UR_57DOT6_KBPS (ITU
      * only), <li>UR_28DOT8_KBPS (ITU only), <li>UR_24DOT0_KBPS (ITU only),
      * <li>UR_0DOT1345_KBPS, <li>UR_0DOT100_KBPS, <li>UR_FWD_0DOT075_KBPS,
      * <li>UR_BWD_1DOT2_KBPS, <li>UR_BWD_0DOT075_KBPS, <li>UR_FWD_1DOT2_KBPS,
      * <li>UR_0DOT050_KBPS, <li>UR_0DOT075_KBPS, <li>UR_0DOT110_KBPS,
      * <li>UR_0DOT150_KBPS, <li>UR_0DOT200, <li>UR_0DOT300_KBPS and
      * <li>UR_12DOT0_KBPS. </ul> 
      * @param in_negotiation  The negotiation; <ul>
      * <li>NEGOT_INBAND_NEGOTIATION_IMPOSSIBLE and
      * <li>NEGOT_INBAND_NEGOTIATION_POSSIBLE. </ul>
      * @param in_syncFlag  The synchronous asynchronous flag; <ul>
      * <li>SYN_SYNCHRONOUS and <li>SYN_ASYNCHRONOUS. </ul>
      * @param in_ext3a  The Extension 3a; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_flowCntrlRx  The Flow Control on Rx; <ul> <li>FCRX_NOT_ACCEPT and
      * <li>FCRX_ACCEPT. </ul>
      * @param in_flowCntrlTx  The flow control on Tx; <ul> <li>FCTX_NOT_REQUIRED and
      * <li>FCTX_REQUIRED. </ul>
      * @param in_nicRx  The network independent clock (NIC) on Rx; <ul>
      * <li>NICRX_NOT_ACCEPT and <li>NICRX_ACCEPT. </ul>
      * @param in_nicTx  the network independent clock (NIC) on Tx; <ul>
      * <li>NICTX_NOT_REQUIRED and <li>NICTX_REQUIRED. </ul>
      * @param in_intermediateRate  The Intermediate rate, range 0 to 3; <ul>
      * <li>IR_NOT_USED, <li>IR_8_KBPS, <li>IR_16_KBPS and <li>IR_32_KBPS. </ul>
      * @param in_ext3b1  The extension 3b1; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_bandNegotiationInd  The in-band/out-of-band negotiation indicator;
      * <ul> <li>ION_UNACCEPTABLE and
      * <li>ION_NEGOTIATION_DONE_INBAND_USING_LOGICAL_LINK_ZERO. </ul>
      * @param in_msgOrigInd  The assignor assignee indicator; <ul>
      * <li>AA_DEFAULT_ASSIGNEE and <li>AA_ASSIGNOR_ONLY. </ul>
      * @param in_lliNegotiationInd  The logical link identifier negotiation
      * indicator; <ul> <li>LLIN_DEFAULT and <li>LLIN_NEGOTIATION. </ul>
      * @param in_modeInd  The mode of operation; <ul> <li>MO_BIT_TRANSPARENT and
      * <li>MO_PROTOCOL_SENSITIVE. </ul>
      * @param in_multiFrameInd  The multi-frame establishment support; <ul>
      * <li>MFES_NOT_SUPPORTED and <li>MFES_SUPPORTED. </ul>
      * @param in_rateAdaptationInd  The rate adaption; <ul> <li>RAH_NOT_INCLUDED and
      * <li>RAH_INCLUDED. </ul>
      * @param in_ext3b2  The extension 3b2; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_parity  the Parity, range 0 to 7; <ul> <li>PI_ODD, <li>PI_EVEN,
      * <li>PI_NONE, <li>PI_FORCED_TO_0 and <li>PI_FORCED_TO_1. </ul>
      * @param in_numDataBits  The number of data bits excluding parity, range 0 and
      * 3; <ul> <li>NDBEP_NOT_USED, <li>NDBEP_5_BIT, <li>NDBEP_7_BIT and
      * <li>NDBEP_8_BIT. </ul>
      * @param in_numStopBits  The number of stop bits, range 0 and 3; <ul>
      * <li>NSB_NOT_USED, <li>NSB_1_bit, <li>NSB_1DOT5_bit and <li>NSB_2_bit. </ul>
      * @param in_ext3c  The extension 3c; <li> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_modemType  The modemType, range 0 and 63.
      * @param in_duplexModeInd  The duplex mode indicator; <ul> <li>DM_HALF_DUPLEX
      * and <li>DM_FULL_DUPLEX. </ul>
      * @param in_ext3d  The extension 3d; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_userInfoLayer2  The user information layer 2 protocol, range 0 and
      * 31; <ul> <li>UIL2P_Q921_I441 (ITU only), <li>UIL2P_T1DOT602 (ANSI only),
      * <li>UIL2P_X25 and <li>UIL2P_LAN_LOGIC_CONTROL (ITU only). </ul>
      * @param in_layer2Id  The layer 2 identification, range 0 and 3.
      * @param in_ext4  The extension 4; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_userInfoLayer3  The user information layer 3 protocol, range 0 and
      * 31; <ul> <li>UIL3P_Q931 (ITU only), <li>UIL3P_T1DOT607 (ANSI only),
      * <li>UIL3P_X25 and <li>UIL3P_ISO_IEC_TR_9577 (ITU only). </ul>
      * @param in_layer3Id  The layer 3 identification, range 0 and 3.
      * @param in_ext5  The extension 5; <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public UserServiceInfo(byte in_infoTransferCap, byte in_codingStd, boolean in_ext1,
            byte in_infoTransferRate, byte in_transferMode, boolean in_ext2,
            byte in_establishment, byte in_config, byte in_struct, boolean in_ext2a,
            byte in_infoTransferRateDestToOrig, byte in_symmetry, boolean in_ext2b,
            byte in_userInfoLayer1, byte in_layer1Id, boolean in_ext3,
            byte in_userInfoLayer2, byte in_layer2Id, boolean in_ext4,
            byte in_userInfoLayer3, byte in_layer3Id, boolean in_ext5,
            byte in_rateMultiplier, boolean in_ext21, byte in_userRate,
            boolean in_negotiation, boolean in_syncFlag, boolean in_ext3a,
            boolean in_flowCntrlRx, boolean in_flowCntrlTx, boolean in_nicRx,
            boolean in_nicTx, byte in_intermediateRate, boolean in_ext3b1,
            boolean in_bandNegotiationInd, boolean in_msgOrigInd, boolean
            in_lliNegotiationInd, boolean in_modeInd, boolean in_multiFrameInd,
            boolean in_rateAdaptationInd, boolean in_ext3b2, byte in_parity,
            byte in_numDataBits, byte in_numStopBits, boolean in_ext3c, byte in_modemType,
            boolean in_duplexModeInd, boolean in_ext3d)
        throws ParameterRangeInvalidException {
    super(in_infoTransferCap, in_codingStd, in_ext1, in_infoTransferRate,
            in_transferMode, in_ext2, in_establishment, in_config, in_struct,
            in_ext2a, in_infoTransferRateDestToOrig, in_symmetry, in_ext2b,
            in_userInfoLayer1, in_layer1Id, in_ext3, in_userInfoLayer2, in_layer2Id,
            in_ext4, in_userInfoLayer3, in_layer3Id, in_ext5, in_rateMultiplier,
            in_ext21, in_userRate, in_negotiation, in_syncFlag, in_ext3a,
            in_flowCntrlRx, in_flowCntrlTx, in_nicRx, in_nicTx, in_intermediateRate,
            in_ext3b1, in_bandNegotiationInd, in_msgOrigInd, in_lliNegotiationInd,
            in_modeInd, in_multiFrameInd, in_rateAdaptationInd, in_ext3b2, in_parity,
            in_numDataBits, in_numStopBits, in_ext3c, in_modemType, in_duplexModeInd,
            in_ext3d);
    }
    /** The toString method retrieves a string containing the values of the members of
      * the UserServiceInfo class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.UserServiceInfo");
        return b.toString();
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
