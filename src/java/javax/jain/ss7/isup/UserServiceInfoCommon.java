/*
 @(#) $RCSfile: UserServiceInfoCommon.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:47 $ <p>
 
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

/** A class representing the ISUP User Service Information parameter.
  * This class is the super class of UserServiceInfo class and UserServiceInfoPrime class.
  * It provides access methods for all the common sub-fields in ITU and ANSI variant.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class UserServiceInfoCommon implements java.io.Serializable {
    public static final boolean EI_NEXT_OCTET_EXIST = false;
    public static final boolean EI_LAST_OCTET = true;
    public static final byte CS_ITU_STANDARD = 0;
    public static final byte CS_ISO_IEC_STANDARD = 1;
    public static final byte CS_NATIONAL_STANDARD = 2;
    public static final byte CS_STANDARD_FOR_NETWORK = 0;
    public static final byte ITC_SPEECH = 0;
    public static final byte ITC_UNRESTRICTED_DIGITAL_INFORMATION = 8;
    public static final byte ITC_RESTRICTED_DIGITAL_INFORMATION = 9;
    public static final byte ITC_3DOT1_KHZ_AUDIO = 16;
    public static final byte ITC_UNRESTRICTED_DIGITAL_INFORMATION_WITH_TONES_ANNOUNCEMENTS = 17;
    public static final byte ITC_VIDEO = 24;
    public static final byte TM_CIRCUIT_MODE = 9;
    public static final byte TM_PACKET_MODE = 2;
    public static final byte ITR_64_KBPS = 16;
    public static final byte ITR_2_by_64_KBPS = 7;
    public static final byte ITR_384_KBPS = 19;
    public static final byte ITR_1472_KBPS = 20;
    public static final byte ITR_1536_KBPS = 21;
    public static final byte ITR_1920_KBPS = 23;
    public static final byte ITR_MULTI_RATE = 24;
    public static final byte STRUCTURE_DEFAULT = 0;
    public static final byte STRUCTURE_8_KHZ_INTEGRITY = 1;
    public static final byte STRUCTURE_SERVICE_DATA_UNIT_INTEGRITY = 4;
    public static final byte STRUCTURE_UNSTRUCTURED = 7;
    public static final byte CONFIGURATION_POINT_TO_POINT = 0;
    public static final byte ESTABLISHMENT_DEMAND = 0;
    public static final byte SYMMETRY_BIDIRECTIONAL_SYMMETRIC = 0;
    public static final byte UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_V110_X30 = 1;
    public static final byte UIL1P_RECOMMENDATION_G711_U_LA = 2;
    public static final byte UIL1P_RECOMMENDATION_G711_A_LAW = 0;
    public static final byte UIL1P_RECOMMENDATION_G721_I460 = 0;
    public static final byte UIL1P_RECOMMENDATION_H221_H242 = 0;
    public static final byte UIL1P_RECOMMENDATION_G722_G725_7KHZ_AUDIO = 5;
    public static final byte UIL1P_RECOMMENDATION_H223_H245 = 0;
    public static final byte UIL1P_NON_ITU_STANDARDIZED_RATE_ADAPTION = 7;
    public static final byte UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_V120 = 8;
    public static final byte UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_X31_HDLC_FLAG_STUFFING = 9;
    public static final boolean SYN_SYNCHRONOUS = false;
    public static final boolean SYN_ASYNCHRONOUS = true;
    public static final boolean NEGOT_INBAND_NEGOTIATION_IMPOSSIBLE = false;
    public static final boolean NEGOT_INBAND_NEGOTIATION_POSSIBLE = true;
    public static final byte UR_RATE_I460 = 0;
    public static final byte UR_0DOT6_KBPS = 1;
    public static final byte UR_1DOT2_KBPS = 2;
    public static final byte UR_2DOT4_KBPS = 3;
    public static final byte UR_3DOT6_KBPS = 4;
    public static final byte UR_4DOT8_KBPS = 5;
    public static final byte UR_7DOT2_KBPS = 6;
    public static final byte UR_8DOT0_KBPS = 7;
    public static final byte UR_9DOT6_KBPS = 8;
    public static final byte UR_14DOT4_KBPS = 9;
    public static final byte UR_16DOT0_KBPS = 10;
    public static final byte UR_19DOT2_KBPS = 11;
    public static final byte UR_32DOT0_KBPS = 12;
    public static final byte UR_38DOT4_KBPS = 13;
    public static final byte UR_48DOT0_KBPS = 14;
    public static final byte UR_56DOT0_KBPS = 15;
    public static final byte UR_64DOT0_KBPS = 16;
    public static final byte UR_57DOT6_KBPS = 17;
    public static final byte UR_28DOT8_KBPS = 18;
    public static final byte UR_24DOT0_KBPS = 19;
    public static final byte UR_0DOT1345_KBPS = 21;
    public static final byte UR_0DOT100_KBPS = 22;
    public static final byte UR_FWD_0DOT075_KBPS = 23;
    public static final byte UR_BWD_1DOT2_KBPS = 24;
    public static final byte UR_BWD_0DOT075_KBPS = 25;
    public static final byte UR_FWD_1DOT2_KBPS = 26;
    public static final byte UR_0DOT050_KBPS = 25;
    public static final byte UR_0DOT075_KBPS = 32;
    public static final byte UR_0DOT110_KBPS = 27;
    public static final byte UR_0DOT150_KBPS = 28;
    public static final byte UR_0DOT200_KBPS = 29;
    public static final byte UR_0DOT300_KBPS = 30;
    public static final byte UR_12DOT0_KBPS = 31;
    public static final byte IR_NOT_USED = 0;
    public static final byte IR_8_KBPS = 32;
    public static final byte IR_16_KBPS = 64;
    public static final byte IR_32_KBPS = 96;
    public static final boolean NICTX_NOT_REQUIRED = false;
    public static final boolean NICTX_REQUIRED = true;
    public static final boolean NICRX_NOT_ACCEPT = false;
    public static final boolean NICRX_ACCEPT = true;
    public static final boolean FCTX_NOT_REQUIRED = false;
    public static final boolean FCTX_REQUIRED = true;
    public static final boolean FCRX_NOT_ACCEPT = false;
    public static final boolean FCRX_ACCEPT = true;
    public static final boolean RAH_NOT_INCLUDED = false;
    public static final boolean RAH_INCLUDED = true;
    public static final boolean MFES_NOT_SUPPORTED = false;
    public static final boolean MFES_SUPPORTED = true;
    public static final boolean MO_BIT_TRANSPARENT = false;
    public static final boolean MO_PROTOCOL_SENSITIVE = true;
    public static final boolean LLIN_DEFAULT = false;
    public static final boolean LLIN_NEGOTIATION = true;
    public static final boolean AA_DEFAULT_ASSIGNEE = false;
    public static final boolean AA_ASSIGNOR_ONLY = true;
    public static final boolean ION_UNACCEPTABLE = false;
    public static final boolean ION_NEGOTIATION_DONE_INBAND_USING_LOGICAL_LINK_ZERO = true;
    public static final byte NSB_NOT_USED = 0;
    public static final byte NSB_1_bit = 1;
    public static final byte NSB_1DOT5_bit = 2;
    public static final byte NSB_2_bit = 3;
    public static final byte NDBEP_NOT_USED = 0;
    public static final byte NDBEP_5_BIT = 1;
    public static final byte NDBEP_7_BIT = 2;
    public static final byte NDBEP_8_BIT = 3;
    public static final byte PI_ODD = 0;
    public static final byte PI_EVEN = 2;
    public static final byte PI_NONE = 3;
    public static final byte PI_FORCED_TO_0 = 4;
    public static final byte PI_FORCED_TO_1 = 5;
    public static final boolean DM_HALF_DUPLEX = false;
    public static final boolean DM_FULL_DUPLEX = true;
    public static final byte UIL2P_Q921_I441 = 1;
    public static final byte UIL2P_T1DOT602 = 0;
    public static final byte UIL2P_X25 = 6;
    public static final byte UIL2P_LAN_LOGIC_CONTROL = 0;
    public static final byte UIL3P_Q931 = 2;
    public static final byte UIL3P_T1DOT607 = 2;
    public static final byte UIL3P_X25 = 6;
    public static final byte UIL3P_ISO_IEC_TR_9577 = 0;
    /** Constructs a new UserServiceInfoCommon class, parameters with default values.  */
    public UserServiceInfoCommon() {
    }
    /** Constructs a UserServiceInfoCommon class from the input parameters specified.
      * @param in_infoTransferCap  The information transfer capability, range 0 to 31 <ul>
      * <li>ITC_SPEECH, <li>ITC_UNRESTRICTED_DIGITAL_INFORMATION,
      * <li>ITC_RESTRICTED_DIGITAL_INFORMATION, <li>ITC_3DOT1_KHZ_AUDIO = 16,
      * <li>ITC_UNRESTRICTED_DIGITAL_INFORMATION_WITH_TONES_ANNOUNCEMENTS and
      * <li>ITC_VIDEO (ITU only). </ul>
      * @param in_codingStd  The coding standard, range 0 to 3 <ul> <li>CS_ITU_STANDARD,
      * <li>CS_ISO_IEC_STANDARD (ITU only), <li>CS_NATIONAL_STANDARD and
      * <li>CS_STANDARD_FOR_NETWORK (ITU only). </ul>
      * @param in_ext1  The extension 1 <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_infoTransferRate  The information transfer rate, range 0 to 31 <ul>
      * <li>ITR_64_KBPS, <li>ITR_2_by_64_KBPS (ITU only), <li>ITR_384_KBPS,
      * <li>ITR_1472_KBPS, <li>ITR_1536_KBPS, <li>ITR_1920_KBPS and <li>ITR_MULTI_RATE.
      * </ul>
      * @param in_transferMode  The transfer mode value, range 0 to 3 <ul>
      * <li>TM_CIRCUIT_MODE and <li>TM_PACKET_MODE. </ul>
      * @param in_ext2  The extensition 2 <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_establishment  The establishment, range 0 to 3 <ul>
      * <li>ESTABLISHMENT_DEMAND. </ul>
      * @param in_config  The configuration, range 0 to 3 <ul>
      * <li>CONFIGURATION_POINT_TO_POINT. </ul>
      * @param in_struct  The structure, range 0 to 7 <ul> <li>STRUCTURE_DEFAULT,
      * <li>STRUCTURE_8_KHZ_INTEGRITY, <li>STRUCTURE_SERVICE_DATA_UNIT_INTEGRITY and
      * <li>STRUCTURE_UNSTRUCTURED. </ul>
      * @param in_ext2a  The extension 2a <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_infoTransferRateDestToOrig  The information transfer rate from
      * Destination to Origination, range 0 to 31.
      * @param in_symmetry  The symmetry, range 0 to 3 <ul>
      * <li>SYMMETRY_BIDIRECTIONAL_SYMMETRIC. </ul>
      * @param in_ext2b  The extension 2b <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_userInfoLayer1  The user information layer 1 protocol, range 0 to 31
      * <ul>  <li>UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_V110_X30,
      * <li>UIL1P_RECOMMENDATION_G711_U_LA, <li>UIL1P_RECOMMENDATION_G711_A_LAW (ITU
      * only), <li>UIL1P_RECOMMENDATION_G721_I460 (ITU only),
      * <li>UIL1P_RECOMMENDATION_H221_H242 (ITU only),
      * <li>UIL1P_RECOMMENDATION_G722_G725_7KHZ_AUDIO (ANSI only),
      * <li>UIL1P_RECOMMENDATION_H223_H245 (ITU only), <li>UIL1P_RECOMMENDATION_H223_H245,
      * <li>UIL1P_NON_ITU_STANDARDIZED_RATE_ADAPTION,
      * <li>UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_V120 and
      * <li>UIL1P_ITU_STANDARDIZED_RATE_ADAPTION_X31_HDLC_FLAG_STUFFING. </ul>
      * @param in_layer1Id  The layer 1 identification, range 0 to 3.
      * @param in_ext3  The extension 3 <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_rateMultiplier  the Rate Multiplier, range 0 to 127.
      * @param in_ext21  The extension 2_1 <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_userRate  The User rate, range 0 to 31 <ul> <li>UR_RATE_I460,
      * <li>UR_0DOT6_KBPS, <li>UR_1DOT2_KBPS, <li>UR_2DOT4_KBPS, <li>UR_3DOT6_KBPS,
      * <li>UR_4DOT8_KBPS, <li>UR_7DOT2_KBPS, <li>UR_8DOT0_KBPS, <li>UR_9DOT6_KBPS,
      * <li>UR_14DOT4_KBPS, <li>UR_16DOT0_KBPS, <li>UR_19DOT2_KBPS, <li>UR_32DOT0_KBPS,
      * <li>UR_38DOT4_KBPS (ITU only), <li>UR_48DOT0_KBPS, <li>UR_56DOT0_KBPS,
      * <li>UR_64DOT0_KBPS (ANSI only), <li>UR_57DOT6_KBPS (ITU only),
      * <li>UR_28DOT8_KBPS (ITU only), <li>UR_24DOT0_KBPS (ITU only),
      * <li>UR_0DOT1345_KBPS, <li>UR_0DOT100_KBPS, <li>UR_FWD_0DOT075_KBPS,
      * <li>UR_BWD_1DOT2_KBPS, <li>UR_BWD_0DOT075_KBPS, <li>UR_FWD_1DOT2_KBPS,
      * <li>UR_0DOT050_KBPS, <li>UR_0DOT075_KBPS, <li>UR_0DOT110_KBPS,
      * <li>UR_0DOT150_KBPS, <li>UR_0DOT200, <li>UR_0DOT300_KBPS and <li>UR_12DOT0_KBPS.
      * </ul>
      * @param in_negotiation  The negotiation <ul>
      * <li>NEGOT_INBAND_NEGOTIATION_IMPOSSIBLE and <li>NEGOT_INBAND_NEGOTIATION_POSSIBLE.
      * </ul>
      * @param in_syncFlag  The synchronous asynchronous flag <ul> <li>SYN_SYNCHRONOUS and
      * <li>SYN_ASYNCHRONOUS. </ul>
      * @param in_ext3a  The Extension 3a <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_flowCntrlRx  the Flow Control on Rx <ul> <li>FCRX_NOT_ACCEPT and
      * <li>FCRX_ACCEPT. </ul>
      * @param in_flowCntrlTx  The flow control on Tx <ul> <li>FCTX_NOT_REQUIRED and
      * <li>FCTX_REQUIRED. </ul>
      * @param in_nicRx  The network independent clock (NIC) on Rx <ul>
      * <li>NICRX_NOT_ACCEPT and <li>NICRX_ACCEPT. </ul>
      * @param in_nicTx  the network independent clock (NIC) on Tx <ul>
      * <li>NICTX_NOT_REQUIRED and <li>NICTX_REQUIRED. </ul>
      * @param in_intermediateRate  the Intermediate rate, range 0 to 3 <ul>
      * <li>IR_NOT_USED, <li>IR_8_KBPS, <li>IR_16_KBPS and <li>IR_32_KBPS. </ul>
      * @param in_ext3b1  the extension 3b1 <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_bandNegotiationInd  The in-band/out-of-band negotiation indicator <ul>
      * <li>ION_UNACCEPTABLE and <li>ION_NEGOTIATION_DONE_INBAND_USING_LOGICAL_LINK_ZERO.
      * </ul>
      * @param in_msgOrigInd  The assignor assignee indicator <ul> <li>AA_DEFAULT_ASSIGNEE
      * and <li>AA_ASSIGNOR_ONLY. </ul>
      * @param in_lliNegotiationInd  The logical link identifier negotiation indicator
      * <ul> <li>LLIN_DEFAULT and <li>LLIN_NEGOTIATION. </ul>
      * @param in_modeInd  The mode of operation <ul> <li>MO_BIT_TRANSPARENT and
      * <li>MO_PROTOCOL_SENSITIVE. </ul>
      * @param in_multiFrameInd  The multi-frame establishment support <ul>
      * <li>MFES_NOT_SUPPORTED and <li>MFES_SUPPORTED. </ul>
      * @param in_rateAdaptationInd  The rate adaption <ul> <li>RAH_NOT_INCLUDED and
      * <li>RAH_INCLUDED. </ul>
      * @param in_ext3b2  The extension 3b2 <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_parity  the Parity, range 0 to 7 <ul> <li>PI_ODD, <li>PI_EVEN,
      * <li>PI_NONE, <li>PI_FORCED_TO_0 and <li>PI_FORCED_TO_1. </ul>
      * @param in_numDataBits  The number of data bits excluding parity, range 0 to 3
      * <ul> <li>NDBEP_NOT_USED, <li>NDBEP_5_BIT, <li>NDBEP_7_BIT and <li>NDBEP_8_BIT.
      * </ul>
      * @param in_numStopBits  The number of stop bits, range 0 to 3 <ul>
      * <li>NSB_NOT_USED, <li>NSB_1_bit, <li>NSB_1DOT5_bit and <li>NSB_2_bit. </ul>
      * @param in_ext3c  The extension 3c <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_modemType  The modemType, range 0 to 63.
      * @param in_duplexModeInd  The duplex mode indicator <ul> <li>DM_HALF_DUPLEX and
      * <li>DM_FULL_DUPLEX. </ul>
      * @param in_ext3d  The extension 3d <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_userInfoLayer2  The user information layer 2 protocol, range 0 to 31
      * <ul> <li>UIL2P_Q921_I441 (ITU only), <li>UIL2P_T1DOT602 (ANSI only), <li>UIL2P_X25
      * and <li>UIL2P_LAN_LOGIC_CONTROL (ITU only). </ul>
      * @param in_layer2Id  The layer 2 identification, range 0 to 3.
      * @param in_ext4  The extension 4 <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @param in_userInfoLayer3  The user information layer 3 protocol, range 0 to 31
      * <ul> <li>UIL3P_Q931 (ITU only), <li>UIL3P_T1DOT607 (ANSI only), <li>UIL3P_X25 and
      * <li>UIL3P_ISO_IEC_TR_9577 (ITU only). </ul>
      * @param in_layer3Id  The layer 3 identification, range 0 to 3.
      * @param in_ext5  The extension 5 <ul> <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public UserServiceInfoCommon(byte in_infoTransferCap, byte in_codingStd, boolean
            in_ext1, byte in_infoTransferRate, byte in_transferMode, boolean in_ext2, byte
            in_establishment, byte in_config, byte in_struct, boolean in_ext2a, byte
            in_infoTransferRateDestToOrig, byte in_symmetry, boolean in_ext2b, byte
            in_userInfoLayer1, byte in_layer1Id, boolean in_ext3, byte in_userInfoLayer2,
            byte in_layer2Id, boolean in_ext4, byte in_userInfoLayer3, byte in_layer3Id,
            boolean in_ext5, byte in_rateMultiplier, boolean in_ext21, byte in_userRate,
            boolean in_negotiation, boolean in_syncFlag, boolean in_ext3a, boolean
            in_flowCntrlRx, boolean in_flowCntrlTx, boolean in_nicRx, boolean in_nicTx,
            byte in_intermediateRate, boolean in_ext3b1, boolean in_bandNegotiationInd,
            boolean in_msgOrigInd, boolean in_lliNegotiationInd, boolean in_modeInd,
            boolean in_multiFrameInd, boolean in_rateAdaptationInd, boolean in_ext3b2,
            byte in_parity, byte in_numDataBits, byte in_numStopBits, boolean in_ext3c,
            byte in_modemType, boolean in_duplexModeInd, boolean in_ext3d)
        throws ParameterRangeInvalidException {
        this.setInfoTransferCap(in_infoTransferCap);
        this.setCodingStd(in_codingStd);
        this.setExtension1(in_ext1);
        this.setInfoTransferRate(in_infoTransferRate);
        this.setTransferMode(in_transferMode);
        this.setExtension2(in_ext2);
        this.setEstablishment(in_establishment);
        this.setConfiguration(in_config);
        this.setStructure(in_struct);
        this.setExtension2a(in_ext2a);
        this.setInfoTransferRateDestToOrig(in_infoTransferRateDestToOrig);
        this.setSymmetry(in_symmetry);
        this.setExtension2b(in_ext2b);
        this.setUserInfoLayer1Protocol(in_userInfoLayer1);
        this.setLayer1Id(in_layer1Id);
        this.setExtension3(in_ext3);
        this.setUserInfoLayer2Protocol(in_userInfoLayer2);
        this.setLayer2Id(in_layer2Id);
        this.setExtension4(in_ext4);
        this.setUserInfoLayer3Protocol(in_userInfoLayer3);
        this.setLayer3Id(in_layer3Id);
        this.setExtension5(in_ext5);
        this.setRateMultiplier(in_rateMultiplier);
        this.setExtension2_1(in_ext21);
        this.setUserRate(in_userRate);
        this.setNegotiation(in_negotiation);
        this.setSyncAsyncInd(in_syncFlag);
        this.setExtension3a(in_ext3a);
        this.setFlowControlRx(in_flowCntrlRx);
        this.setFlowControlTx(in_flowCntrlTx);
        this.setNICRx(in_nicRx);
        this.setNICTx(in_nicTx);
        this.setIntermediateRate(in_intermediateRate);
        this.setExtension3b1(in_ext3b1);
        this.setBandNegotiationInd(in_bandNegotiationInd);
        this.setMessageOriginatorInd(in_msgOrigInd);
        this.setLogLinkIdNegotiationInd(in_lliNegotiationInd);
        this.setModeOfOperation(in_modeInd);
        this.setMultiFrameEstablishmentInd(in_multiFrameInd);
        this.setRateAdaptationHdr(in_rateAdaptationInd);
        this.setExtension3b2(in_ext3b2);
        this.setParity(in_parity);
        this.setNumberOfDataBits(in_numDataBits);
        this.setNumberOfStopBits(in_numStopBits);
        this.setExtension3c(in_ext3c);
        this.setModemType(in_modemType);
        this.setDuplexModeInd(in_duplexModeInd);
        this.setExtension3d(in_ext3d);
        // this.setExt3b2();
    }
    /** Gets the Information Transfer Capability.
      * @return The Information Transfer Capability value, range 0 to 31, see
      * UserServiceInfoCommon().
      */
    public byte getInfoTransferCap() {
        return m_infoTransferCap;
    }
    /** Sets the Information Transfer Capability.
      * @param aInfoTransferCap  The InfoTransferCap value, range 0 to 31, see
      * UserServiceInfoCommon().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setInfoTransferCap(byte aInfoTransferCap)
        throws ParameterRangeInvalidException {
        if (0 <= aInfoTransferCap && aInfoTransferCap <= 31) {
            m_infoTransferCap = aInfoTransferCap;
            return;
        }
        throw new ParameterRangeInvalidException("InformationTransferCap value " + aInfoTransferCap + " out of range.");
    }
    /** Gets the Coding Standard.
      * @return byte containing the CodingStd value, range 0 to 3, see
      * UserServiceInfoCommon().
      */
    public byte getCodingStd() {
        return m_codingStd;
    }
    /** Sets the Coding Standard.
      * @param aCodingStd  The CodingStd value, range 0 to 3, see UserServiceInfoCommon().
      */
    public void setCodingStd(byte aCodingStd)
        throws ParameterRangeInvalidException {
        if (0 <= aCodingStd && aCodingStd <= 3) {
            m_codingStd = aCodingStd;
            return;
        }
        throw new ParameterRangeInvalidException("CodingStd value " + aCodingStd + " out of range.");
    }
    /** Gets the extension bit flag for byte 1.
      * @return Boolean flag containing the extension 1, see UserServiceInfoCommon().
      */
    public boolean getExtension1() {
        return m_ext1;
    }
    /** Sets the extension 1 bit flag for byte 1.
      * @param aExt1  The extension 1, see UserServiceInfoCommon().
      */
    public void setExtension1(boolean aExt1) {
        m_ext1 = aExt1;
    }
    /** Gets the Information Transfer Rate.
      * @return The InfoTransferRate value, range 0 to 31, see UserServiceInfoCommon().
      */
    public byte getInfoTransferRate() {
        return m_infoTransferRate ;
    }
    /** Sets the Information Transfer Rate.
      * @param aInfoTransferRate  The InfoTransferRate value, range 0 to 31, see
      * UserServiceInfoCommon().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setInfoTransferRate(byte aInfoTransferRate)
        throws ParameterRangeInvalidException {
        if (0 <= aInfoTransferRate && aInfoTransferRate <= 31) {
            m_infoTransferRate = aInfoTransferRate;
            return;
        }
        throw new ParameterRangeInvalidException("InfoTransferRate value " + aInfoTransferRate + " out of range.");
    }
    /** Gets the Transfer Mode.
      * @return Byte containing the TransferMode value, range 0 to 3, see
      * UserServiceInfoCommon().
      */
    public byte getTransferMode() {
        return m_transferMode;
    }
    /** Sets the Transfer Mode.
      * @param aTransferMode  The TransferMode value, see UserServiceInfoCommon().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setTransferMode(byte aTransferMode)
        throws ParameterRangeInvalidException {
        if (0 <= aTransferMode && aTransferMode <= 3) {
            m_transferMode = aTransferMode;
            return;
        }
        throw new ParameterRangeInvalidException("TransferMode value " + aTransferMode + " out of range.");
    }
    /** Gets the extension 2 bit flag for byte 2.
      * @return Boolean containing the extension 2, see UserServiceInfoCommon().
      */
    public boolean getExtension2() {
        return m_ext2;
    }
    /** Sets the extension 2 bit flag for byte 2.
      * @param aExt2  The extension 2, see UserServiceInfoCommon().
      */
    public void setExtension2(boolean aExt2) {
        m_ext2 = aExt2;
    }
    /** Gets the Establishment field value.
      * @return byte the Establishment, range 0 to 3, see UserServiceInfoCommon().
      */
    public byte getEstablishment() {
        return m_establishment ;
    }
    /** Sets the Establishment field value.
      * @param aEstablishment  The Establishment, range 0 to 3, see UserServiceInfoCommon().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setEstablishment(byte aEstablishment)
        throws ParameterRangeInvalidException {
        if (0 <= aEstablishment && aEstablishment <= 3) {
            m_establishment = aEstablishment;
            return;
        }
        throw new ParameterRangeInvalidException("Establishment value " + aEstablishment + " out of range.");
    }
    /** Gets the Configuration field value.
      * @return byte the Configuration, range 0 to 3, see UserServiceInfoCommon().
      */
    public byte getConfiguration() {
        return m_config ;
    }
    /** Sets the Configuration field value.
      * @param aConfiguration  The Configuration, range 0 to 3, see
      * UserServiceInfoCommon().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setConfiguration(byte aConfiguration)
        throws ParameterRangeInvalidException {
        if (0 <= aConfiguration && aConfiguration <= 3) {
            m_config = aConfiguration;
            return;
        }
        throw new ParameterRangeInvalidException("Configuration value " + aConfiguration + " out of range.");
    }
    /** Gets the Structure field value.
      * @return The Structure, range 0 to 7, see UserServiceInfoCommon().
      */
    public byte getStructure() {
        return m_struct;
    }
    /** Sets the Structure field value.
      * @param aStructure  The Structure, range 0 to 7, see UserServiceInfoCommon().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setStructure(byte aStructure)
        throws ParameterRangeInvalidException {
        if (0 <= aStructure && aStructure <= 7) {
            m_struct = aStructure;
            return;
        }
        throw new ParameterRangeInvalidException("Structure value " + aStructure + " out of range.");
    }
    /** Gets the extension 2a bit flag for byte 2 indicating 'more' bytes to follow.
      * @return The extension 2a, see UserServiceInfoCommon().
      */
    public boolean getExtension2a() {
        return m_ext2a;
    }
    /** Sets the extension 2a bit flag for byte 2 indicating 'more' bytes to follow.
      * @param aExt2a  The extension 2a, see UserServiceInfoCommon().
      */
    public void setExtension2a(boolean aExt2a) {
        m_ext2a = aExt2a;
    }
    /** Gets the Information Transfer Rate from Destination to Origination.
      * @return byte the InfoTransferRate from Destination to Origination value, range 0
      * to 31, see UserServiceInfoCommon().
      */
    public byte getInfoTransferRateDestToOrig() {
        return m_infoTransferRateDestToOrig;
    }
    /** Sets the Information Transfer Rate from Destination to Origination.
      * @param aInfoTransferRateDestToOrig  The InfoTransferRate from Destination to
      * Origination value, see UserServiceInfoCommon().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setInfoTransferRateDestToOrig(byte aInfoTransferRateDestToOrig)
        throws ParameterRangeInvalidException {
        if (0 <= aInfoTransferRateDestToOrig && aInfoTransferRateDestToOrig <= 31) {
            m_infoTransferRateDestToOrig = aInfoTransferRateDestToOrig;
            return;
        }
        throw new ParameterRangeInvalidException("InfoTransferRateDestToOrig value " + aInfoTransferRateDestToOrig + " out of reange.");
    }
    /** Gets the Symmetry field value.
      * @return byte the Symmetry, range 0 to 3, see UserServiceInfoCommon().
      */
    public byte getSymmetry() {
        return m_symmetry;
    }
    /** Sets the Symmetry field value.
      * @param aSymmetry  The Symmetry, range 0 to 3, see UserServiceInfoCommon().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setSymmetry(byte aSymmetry)
        throws ParameterRangeInvalidException {
        if (0 <= aSymmetry && aSymmetry <= 3) {
            m_symmetry = aSymmetry;
            return;
        }
        throw new ParameterRangeInvalidException("Symmetry value " + aSymmetry + " out of range.");
    }
    /** Gets the extension 2b for byte 2b indicating 'more' bytes to follow.
      * @return The extension 2b, see UserServiceInfoCommon().
      */
    public boolean getExtension2b() {
        return m_ext2b ;
    }
    /** Sets the extension 2b for byte 2b indicating 'more' bytes to follow.
      * @param aExt2b  The extension 2b, see UserServiceInfoCommon().
      */
    public void setExtension2b(boolean aExt2b) {
        m_ext2b = aExt2b;
    }
    /** Gets the User Information Layer 1 Protocol.
      * @return byte the User Information Layer 1 Protocol, range 0 to 31, see
      * UserServiceInfoCommon().
      */
    public byte getUserInfoLayer1Protocol() {
        return m_userInfoLayer1;
    }
    /** Sets the User Information Layer 1 Protocol.
      * @param aUserInfoLayer1Protocol  The User Information Layer 1 Protocol, range 0 to
      * 31, see UserServiceInfoCommon().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setUserInfoLayer1Protocol(byte aUserInfoLayer1Protocol)
        throws ParameterRangeInvalidException {
        if (0 <= aUserInfoLayer1Protocol && aUserInfoLayer1Protocol <= 31) {
            m_userInfoLayer1 = aUserInfoLayer1Protocol;
            return;
        }
        throw new ParameterRangeInvalidException("InfoLayer1Protocol value " + aUserInfoLayer1Protocol + " out of range.");

    }
    /** Gets the layer 1 identification.
      * @return byte the layer 1 identification, range 0 to 3, see
      * userserviceinfocommon().
      */
    public byte getLayer1Id() {
        return m_layer1Id;
    }
    /** Sets the layer 1 identification.
      * @param aLayer1Id  the layer 1 identification, range 0 to 3, see
      * userserviceinfocommon().
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setLayer1Id(byte aLayer1Id)
        throws ParameterRangeInvalidException {
        if (0 <= aLayer1Id && aLayer1Id <= 3) {
            m_layer1Id = aLayer1Id;
            return;
        }
        throw new ParameterRangeInvalidException("layer1id value " + aLayer1Id + " out of range.");
    }
    /** Gets the extension 3 bit flag for indicating if more bytes will follow byte 3 or not.
      * @return the extension 3, see userserviceinfocommon().
      */
    public boolean getExtension3() {
        return m_ext3;
    }
    /** Sets the extension 3 bit flag for indicating if more bytes will follow byte 3 or
      * not.
      * @param aExt3  the extension 3, see userserviceinfocommon().
      */
    public void setExtension3(boolean aExt3) {
        m_ext3 = aExt3;
    }
    /** Gets the rate multiplier.
      * @return byte the rate multiplier, range 0 to 127.
      */
    public byte getRateMultiplier() {
        return m_rateMultiplier;
    }
    /** Sets the rate multiplier.
      * @param aRateMultiplier  the rate multiplier, range 0 to 127.
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setRateMultiplier(byte aRateMultiplier)
        throws ParameterRangeInvalidException {
        if (0 <= aRateMultiplier && aRateMultiplier <= 127) {
            m_rateMultiplier = aRateMultiplier;
            return;
        }
        throw new ParameterRangeInvalidException("ratemultiplier value " + aRateMultiplier + " out of range.");
    }
    /** Gets the extension 2.1 bit flag indicating whether more bytes follow the 2.1
      * byte.
      * @return the extension 2_1, see userserviceinfocommon().
      */
    public boolean getExtension2_1() {
        return m_ext21;
    }
    /** Sets the extension 2.1 bit flag indicating whether more bytes follow the 2.1 byte.
      * @param aExt2_1  the extension 2_1, see userserviceinfocommon().
      */
    public void setExtension2_1(boolean aExt2_1) {
        m_ext21 = aExt2_1;
    }
    /** Gets the user rate.
      * @return byte the user rate, range 0 to 31, see userserviceinfocommon().
      */
    public byte getUserRate() {
        return m_userRate;
    }
    /** Sets the user rate.
      * @param aUserRate  the user rate, range 0 to 31, see userserviceinfocommon().
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setUserRate(byte aUserRate)
        throws ParameterRangeInvalidException {
        if (0 <= aUserRate && aUserRate <= 31) {
            m_userRate = aUserRate;
            return;
        }
        throw new ParameterRangeInvalidException("userrate value " + aUserRate + " out of range.");
    }
    /** Gets the negotiation.
      * @return the negotiation, see userserviceinfocommon().
      */
    public boolean getNegotiation() {
        return m_negotiation;
    }
    /** Sets the negotiation.
      * @param aNegotiation  the negotiation, see userserviceinfocommon().
      */
    public void setNegotiation(boolean aNegotiation) {
        m_negotiation = aNegotiation;
    }
    /** Gets the synchronous asynchronous indicator.
      * @return the synchronous asynchronous indicator, see userserviceinfocommon().
      */
    public boolean getSyncAsyncInd() {
        return m_syncFlag;
    }
    /** Sets the synchronous asynchronous indicator.
      * @param aSyncAsync  the synchronous asynchronous, see userserviceinfocommon().
      */
    public void setSyncAsyncInd(boolean aSyncAsync) {
        m_syncFlag = aSyncAsync;
    }
    /** Gets the extension bit flag for byte 3a.
      * @return the extension 3a, see userserviceinfocommon().
      */
    public boolean getExtension3a() {
        return m_ext3a;
    }
    /** Sets the extension bit flag for byte 3a.
      * @param aExt3a  the extension 3a, see userserviceinfocommon().
      */
    public void setExtension3a(boolean aExt3a) {
        m_ext3a = aExt3a;
    }
    /** Gets the flow control on reception.
      * @return the flow control on rx, see userserviceinfocommon().
      */
    public boolean getFlowControlRx() {
        return m_flowCntrlRx;
    }
    /** Sets the flow control on reception.
      * @param aFlowControlRx  the flow control on rx, see userserviceinfocommon().
      */
    public void setFlowControlRx(boolean aFlowControlRx) {
        m_flowCntrlRx = aFlowControlRx;
    }
    /** Gets the flow control on transmission.
      * @return the flow control on tx, see userserviceinfocommon().
      */
    public boolean getFlowControlTx() {
        return m_flowCntrlTx;
    }
    /** Sets the flow control on tx.
      * @param aFlowControlTx  the flow control on tx, see userserviceinfocommon().
      */
    public void setFlowControlTx(boolean aFlowControlTx) {
        m_flowCntrlTx = aFlowControlTx;
    }
    /** Gets the network independent clock (nic) on rx.
      * @return the network independent clock (nic) on rx, see userserviceinfocommon().
      */
    public boolean getNICRx() {
        return m_nicRx;
    }
    /** Sets the network independent clock (nic) on rx.
      * @param aNICRx  the network independent clock (nic) on rx, see
      * userserviceinfocommon().
      */
    public void setNICRx(boolean aNICRx) {
        m_nicRx = aNICRx;
    }
    /** Gets the network independent clock (nic) on tx.
      * @return the network independent clock (nic) on tx, see userserviceinfocommon().
      */
    public boolean getNICTx() {
        return m_nicTx;
    }
    /** Sets the network independent clock (nic) on tx.
      * @param aNICTx  the network independent clock (nic) on tx, see
      * userserviceinfocommon().
      */
    public void setNICTx(boolean aNICTx) {
        m_nicTx = aNICTx;
    }
    /** Gets the intermediate rate.
      * @return the intermediate rate, range 0 - 3, see userserviceinfocommon().
      */
    public byte getIntermediateRate() {
        return m_intermediateRate;
    }
    /** Sets the intermediate rate.
      * @param aIntermediateRate  the intermediate rate, range 0 to 3, see
      * userserviceinfocommon().
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setIntermediateRate(byte aIntermediateRate)
        throws ParameterRangeInvalidException {
        if (0 <= aIntermediateRate && aIntermediateRate <= 3) {
            m_intermediateRate = aIntermediateRate;
            return;
        }
        throw new ParameterRangeInvalidException("intermediaterate value " + aIntermediateRate + " out of range.");
    }
    /** Gets the extension bit flag for byte 3b1.
      * @return the extension 3b1, see userserviceinfocommon().
      */
    public boolean getExtension3b1() {
        return m_ext3b1;
    }
    /** Sets the extension bit flag for byte 3b1.
      * @param aExt3b1  the extension 3b1, see userserviceinfocommon().
      */
    public void setExtension3b1(boolean aExt3b1) {
        m_ext3b1 = aExt3b1;
    }
    /** Gets the extension bit flag for byte 3b2.
      * @return the extension 3b2, see userserviceinfocommon().
      */
    public boolean getExtension3b2() {
        return m_ext3b2;
    }
    /** Sets the extension bit flag for byte 3b2
      * @param aExt3b2  the extension 3b2, see userserviceinfocommon().
      */
    public void setExtension3b2(boolean aExt3b2) {
        m_ext3b2 = aExt3b2;
    }
    /** Gets the inband negotiation indicator.
      * @return the inband negotiation indicator, see userserviceinfocommon().
      */
    public boolean getBandNegotiationInd() {
        return m_bandNegotiationInd;
    }
    /** Sets the inband negotiation indicator.
      * @param aInBandOutbandNegot  the inband negotiation indicator, see
      * userserviceinfocommon().
      */
    public void setBandNegotiationInd(boolean aInBandOutbandNegot) {
        m_bandNegotiationInd = aInBandOutbandNegot;
    }
    /** Gets the message originator indicator  assignor/assignee.
      * @return the assignor/assignee indicator, see userserviceinfocommon().
      */
    public boolean getMessageOriginatorInd() {
        return m_msgOrigInd;
    }
    /** Sets the message originator indicator assignor assignee.
      * @param aAssignorAssignee  the assignor assignee indicator, see
      * userserviceinfocommon().
      */
    public void setMessageOriginatorInd(boolean aAssignorAssignee) {
        m_msgOrigInd = aAssignorAssignee;
    }
    /** Gets the logical link identifier negotiation indicator.
      * @return the logical link identifier negotiation indicator, see
      * userserviceinfocommon().
      */
    public boolean getLogLinkIdNegotiationInd() {
        return m_lliNegotiationInd;
    }
    /** Sets the logical link identifier negotiation indicator.
      * @param aLLINegotiationInd  the logical link identifier negotiation indicator, see
      * userserviceinfocommon().
      */
    public void setLogLinkIdNegotiationInd(boolean aLLINegotiationInd) {
        m_lliNegotiationInd = aLLINegotiationInd;
    }
    /** Gets the mode of operation.
      * @return the mode of operation, see userserviceinfocommon().
      */
    public boolean getModeOfOperation() {
        return m_modeInd;
    }
    /** Sets the mode of operation.
      * @param aModeOfOperation  the mode of operation, see userserviceinfocommon().
      */
    public void setModeOfOperation(boolean aModeOfOperation) {
        m_modeInd = aModeOfOperation;
    }
    /** Gets the multi-frame establishment indicator.
      * @return the multi-frame establishment indicator, see userserviceinfocommon().
      */
    public boolean getMultiFrameEstablishmentInd() {
        return m_multiFrameInd;
    }
    /** Sets the multi-frame establishment indicator.
      * @param aMultiFrameInd  the multi-frame establishment indicator, see
      * userserviceinfocommon().
      */
    public void setMultiFrameEstablishmentInd(boolean aMultiFrameInd) {
        m_multiFrameInd = aMultiFrameInd;
    }
    /** Gets the rate adaption indicator - header/no header.
      * @return the rate adaption indicator, see userserviceinfocommon().
      */
    public boolean getRateAdaptationHdr() {
        return m_rateAdaptationInd;
    }
    /** Sets the rate adaption indicator - header/no header.
      * @param aRateAdaptationHdr  the rate adaption, see userserviceinfocommon().
      */
    public void setRateAdaptationHdr(boolean aRateAdaptationHdr) {
        m_rateAdaptationInd = aRateAdaptationHdr;
    }
    /** Gets the extension bit flag for byte 3b2.
      * @return the extension 3b2, see userserviceinfocommon().
      */
    public boolean getExt3b2() {
        return m_ext3b2;
    }
    /** Sets the extension bit flag for byte 3b2.
      * @param aExt3b2  the extension 3b2, see userserviceinfocommon().
      */
    public void setExt3b2(boolean aExt3b2) {
        m_ext3b2 = aExt3b2;
    }
    /** Gets the parity information.
      * @return the parity, range 0 to 7, see userserviceinfocommon().
      */
    public byte getParity() {
        return m_parity;
    }
    /** Sets the parity information.
      * @param aParity  the parity, range 0 to 7, see userserviceinfocommon()
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setParity(byte aParity)
        throws ParameterRangeInvalidException {
        if (0 <= aParity && aParity <= 7) {
            m_parity = aParity;
            return;
        }
        throw new ParameterRangeInvalidException("parity value " + aParity + " out of range.");
    }
    /** Gets the number of data bits excluding parity.
      * @return the number of data bits excluding parity, range 0 to 3, see
      * userserviceinfocommon().
      */
    public byte getNumberOfDataBits() {
        return m_numDataBits;
    }
    /** Sets the number of data bits excluding parity.
      * @param aNumOfDataBits  the number of data bits excluding parity, range 0 to 3, see
      * userserviceinfocommon().
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setNumberOfDataBits(byte aNumOfDataBits)
        throws ParameterRangeInvalidException {
        if (0 <= aNumOfDataBits && aNumOfDataBits <= 3) {
            m_numDataBits = aNumOfDataBits;
            return;
        }
        throw new ParameterRangeInvalidException("numberofdatabits value " + aNumOfDataBits + " out of range.");
    }
    /** Gets the number of stop bits.
      * @return the number of stop bits, range 0 to 3, see userserviceinfocommon().
      */
    public byte getNumberOfStopBits() {
        return m_numStopBits;
    }
    /** Sets the number of stop bits.
      * @param aNumOfStopBits  the number of stop bits, range 0 to 3, see
      * userserviceinfocommon().
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setNumberOfStopBits(byte aNumOfStopBits)
        throws ParameterRangeInvalidException {
        if (0 <= aNumOfStopBits && aNumOfStopBits <= 3) {
            m_numStopBits = aNumOfStopBits;
            return;
        }
        throw new ParameterRangeInvalidException("numberofstopbits value " + aNumOfStopBits + " out of range.");
    }
    /** Gets the extension bit flag for byte 3c.
      * @return the extension 3c, see userserviceinfocommon().
      */
    public boolean getExtension3c() {
        return m_ext3c;
    }
    /** Sets the extension bit flag for byte 3c.
      * @param aExt3c  the extension 3c, see userserviceinfocommon().
      */
    public void setExtension3c(boolean aExt3c) {
        m_ext3c = aExt3c;
    }
    /** Gets the modem type.
      * @return the modemtype, range 0 to 63, see userserviceinfocommon().
      */
    public byte getModemType() {
        return m_modemType;
    }
    /** Sets the modem type.
      * @param aModemType  the modemtype, range 0 to 63, see userserviceinfocommon().
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setModemType(byte aModemType)
        throws ParameterRangeInvalidException {
        if (0 <= aModemType && aModemType <= 63) {
            m_modemType = aModemType;
            return;
        }
        throw new ParameterRangeInvalidException("modemtype value " + aModemType + " out of range.");
    }
    /** Gets the duplex mode indicator.
      * @return the duplexmode indicator, see userserviceinfocommon().
      */
    public boolean getDuplexModeInd() {
        return m_duplexModeInd;
    }
    /** Sets the duplex mode indicator.
      * @param aDuplexMode  the duplexmode indicator, see userserviceinfocommon().
      */
    public void setDuplexModeInd(boolean aDuplexMode) {
        m_duplexModeInd = aDuplexMode;
    }
    /** Gets the extension bit flag for byte 3d.
      * @return the extension 3d, see userserviceinfocommon().
      */
    public boolean getExtension3d() {
        return m_ext3d;
    }
    /** Sets the extension bit flag for byte 3d.
      * @param aExt3d  the extension 3d, see userserviceinfocommon().
      */
    public void setExtension3d(boolean aExt3d) {
        m_ext3d = aExt3d;
    }
    /** Gets the user information layer 2 protocol.
      * @return the user information layer 2 protocol, range 0 to 31, see
      * userserviceinfocommon().
      */
    public byte getUserInfoLayer2Protocol() {
        return m_userInfoLayer2;
    }
    /** Sets the user information layer 2 protocol.
      * @param aUserInfoLayer2Protocol  the user information layer 2 protocol, range 0 to
      * 31, see userserviceinfocommon().
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setUserInfoLayer2Protocol(byte aUserInfoLayer2Protocol)
        throws ParameterRangeInvalidException {
        if (0 <= aUserInfoLayer2Protocol && aUserInfoLayer2Protocol <= 31) {
            m_userInfoLayer2 = aUserInfoLayer2Protocol;
            return;
        }
        throw new ParameterRangeInvalidException("userinfolayer2protocol value " + aUserInfoLayer2Protocol + " out of range.");
    }
    /** Gets the layer 2 identification.
      * @return the layer 2 identification, range 0 to 3, see userserviceinfocommon().
      */
    public byte getLayer2Id() {
        return m_layer2Id;
    }
    /** Sets the layer 2 identification.
      * @param aLayer2Id  the layer 2 identification, range 0 to 3, see
      * userserviceinfocommon().
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setLayer2Id(byte aLayer2Id)
        throws ParameterRangeInvalidException {
        if (0 <= aLayer2Id && aLayer2Id <= 3) {
            m_layer2Id = aLayer2Id;
            return;
        }
        throw new ParameterRangeInvalidException("layer2id value " + aLayer2Id + " out of range.");
    }
    /** Gets the extension 4 bit flag for indicating if more bytes will follow byte 4 or
      * not.
      * @return the extension 4, see userserviceinfocommon().
      */
    public boolean getExtension4() {
        return m_ext4;
    }
    /** Sets the extension 4 bit flag for indicating if more bytes will follow byte 4 or
      * not.
      * @param aExt4  the extension 4, see userserviceinfocommon().
      */
    public void setExtension4(boolean aExt4) {
        m_ext4 = aExt4;
    }
    /** Gets the user information layer 3 protocol.
      * @return the user information layer 3 protocol, range 0 to 31, see
      * userserviceinfocommon().
      */
    public byte getUserInfoLayer3Protocol() {
        return m_userInfoLayer3;
    }
    /** Sets the user information layer 3 protocol.
      * @param aUserInfoLayer3Protocol  the user information layer 3 protocol, range 0 to
      * 31, see userserviceinfocommon().
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setUserInfoLayer3Protocol(byte aUserInfoLayer3Protocol)
        throws ParameterRangeInvalidException {
        if (0 <= aUserInfoLayer3Protocol && aUserInfoLayer3Protocol <= 31) {
            m_userInfoLayer3 = aUserInfoLayer3Protocol;
            return;
        }
        throw new ParameterRangeInvalidException("userinfolayer3protocol value " + aUserInfoLayer3Protocol + " out of range.");
    }
    /** Gets the layer 3 identification.
      * @return the layer 3 identification, range 0 to 3, see userserviceinfocommon().
      */
    public byte getLayer3Id() {
        return m_layer3Id;
    }
    /** Sets the layer 3 identification.
      * @param aLayer3Id  the layer 3 identification, range 0 to 3, see
      * userserviceinfocommon().
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public void setLayer3Id(byte aLayer3Id)
        throws ParameterRangeInvalidException {
        if (0 <= aLayer3Id && aLayer3Id <= 3) {
            m_layer3Id = aLayer3Id;
            return;
        }
        throw new ParameterRangeInvalidException("layer3id value " + aLayer3Id + " out of range.");
    }
    /** Gets the extension 5 bit flag for indicating if more bytes will follow byte 5 or
      * not.
      * @return the extension 5, see userserviceinfocommon().
      */
    public boolean getExtension5() {
        return m_ext5;
    }
    /** Sets the extension 5 bit flag for indicating if more bytes will follow byte 5 or
      * not.
      * @param aExt5  the extension 5, see userserviceinfocommon().
      */
    public void setExtension5(boolean aExt5) {
        m_ext5 = aExt5;
    }
    /** The tostring method retrieves a string containing the values of the members of the
      * userserviceinfocommon class.
      * @return a string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(1024);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.userserviceinfocommon");
        b.append("\n\tm_infoTransferCap: " + m_infoTransferCap);
        b.append("\n\tm_codingStd: " + m_codingStd);
        b.append("\n\tm_ext1: " + m_ext1);
        b.append("\n\tm_infoTransferRate: " + m_infoTransferRate);
        b.append("\n\tm_transferMode: " + m_transferMode);
        b.append("\n\tm_ext2: " + m_ext2);
        b.append("\n\tm_establishment: " + m_establishment);
        b.append("\n\tm_config: " + m_config);
        b.append("\n\tm_struct: " + m_struct);
        b.append("\n\tm_ext2a: " + m_ext2a);
        b.append("\n\tm_infoTransferRateDestToOrig: " + m_infoTransferRateDestToOrig);
        b.append("\n\tm_symmetry: " + m_symmetry);
        b.append("\n\tm_ext2b: " + m_ext2b);
        b.append("\n\tm_userInfoLayer1: " + m_userInfoLayer1);
        b.append("\n\tm_layer1Id: " + m_layer1Id);
        b.append("\n\tm_ext3: " + m_ext3);
        b.append("\n\tm_userInfoLayer2: " + m_userInfoLayer2);
        b.append("\n\tm_layer2Id: " + m_layer2Id);
        b.append("\n\tm_ext4: " + m_ext4);
        b.append("\n\tm_userInfoLayer3: " + m_userInfoLayer3);
        b.append("\n\tm_layer3Id: " + m_layer3Id);
        b.append("\n\tm_ext5: " + m_ext5);
        b.append("\n\tm_rateMultiplier: " + m_rateMultiplier);
        b.append("\n\tm_ext21: " + m_ext21);
        b.append("\n\tm_userRate: " + m_userRate);
        b.append("\n\tm_negotiation: " + m_negotiation);
        b.append("\n\tm_syncFlag: " + m_syncFlag);
        b.append("\n\tm_ext3a: " + m_ext3a);
        b.append("\n\tm_flowCntrlRx: " + m_flowCntrlRx);
        b.append("\n\tm_flowCntrlTx: " + m_flowCntrlTx);
        b.append("\n\tm_nicRx: " + m_nicRx);
        b.append("\n\tm_nicTx: " + m_nicTx);
        b.append("\n\tm_intermediateRate: " + m_intermediateRate);
        b.append("\n\tm_ext3b1: " + m_ext3b1);
        b.append("\n\tm_bandNegotiationInd: " + m_bandNegotiationInd);
        b.append("\n\tm_msgOrigInd: " + m_msgOrigInd);
        b.append("\n\tm_lliNegotiationInd: " + m_lliNegotiationInd);
        b.append("\n\tm_modeInd: " + m_modeInd);
        b.append("\n\tm_multiFrameInd: " + m_multiFrameInd);
        b.append("\n\tm_rateAdaptationInd: " + m_rateAdaptationInd);
        b.append("\n\tm_ext3b2: " + m_ext3b2);
        b.append("\n\tm_parity: " + m_parity);
        b.append("\n\tm_numDataBits: " + m_numDataBits);
        b.append("\n\tm_numStopBits: " + m_numStopBits);
        b.append("\n\tm_ext3c: " + m_ext3c);
        b.append("\n\tm_modemType: " + m_modemType);
        b.append("\n\tm_duplexModeInd: " + m_duplexModeInd);
        b.append("\n\tm_ext3d: " + m_ext3d);
        return  b.toString();
    }
    protected byte m_infoTransferCap;
    protected byte m_codingStd;
    protected boolean m_ext1;
    protected byte m_infoTransferRate;
    protected byte m_transferMode;
    protected boolean m_ext2;
    protected byte m_establishment;
    protected byte m_config;
    protected byte m_struct;
    protected boolean m_ext2a;
    protected byte m_infoTransferRateDestToOrig;
    protected byte m_symmetry;
    protected boolean m_ext2b;
    protected byte m_userInfoLayer1;
    protected byte m_layer1Id;
    protected boolean m_ext3;
    protected byte m_userInfoLayer2;
    protected byte m_layer2Id;
    protected boolean m_ext4;
    protected byte m_userInfoLayer3;
    protected byte m_layer3Id;
    protected boolean m_ext5;
    protected byte m_rateMultiplier;
    protected boolean m_ext21;
    protected byte m_userRate;
    protected boolean m_negotiation;
    protected boolean m_syncFlag;
    protected boolean m_ext3a;
    protected boolean m_flowCntrlRx;
    protected boolean m_flowCntrlTx;
    protected boolean m_nicRx;
    protected boolean m_nicTx;
    protected byte m_intermediateRate;
    protected boolean m_ext3b1;
    protected boolean m_bandNegotiationInd;
    protected boolean m_msgOrigInd;
    protected boolean m_lliNegotiationInd;
    protected boolean m_modeInd;
    protected boolean m_multiFrameInd;
    protected boolean m_rateAdaptationInd;
    protected boolean m_ext3b2;
    protected byte m_parity;
    protected byte m_numDataBits;
    protected byte m_numStopBits;
    protected boolean m_ext3c;
    protected byte m_modemType;
    protected boolean m_duplexModeInd;
    protected boolean m_ext3d;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
