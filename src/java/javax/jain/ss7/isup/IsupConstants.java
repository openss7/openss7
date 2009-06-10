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

/** The IsupConstants class defines the ISUP constants of primitive type.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class IsupConstants {
    public IsupConstants(){
    }
    /** Access Delivery Indicator, no set-up message generated (ITU). */
    public static final byte ADI_NO_SETUP_MESSAGE_GENERATED = 0x01; 
    /** Access Delivery Indicator, set-up message generated (ITU). */
    public static final byte ADI_SETUP_MESSAGE_GENERATED = 0x00; 

    /** Automatic congestion level, level 1 exceeded. */
    public static final byte ACL_LEVEL1_EXCEEDED = 1; 
    /** Automatic congestion level, level 2 exceeded. */
    public static final byte ACL_LEVEL2_EXCEEDED = 2; 
    /** Automatic congestion level, level 3 exceeded. */
    public static final byte ACL_LEVEL3_EXCEEDED = 3; 

    /** Calling party category, unknown or unspecified. */
    public static final int CPC_UNKNOWN = 0; 
    /** Calling party category, french operator. */
    public static final int CPC_OPERATOR_FRENCH = 0x01; 
    /** Calling party category, english operator. */
    public static final int CPC_OPERATOR_ENGLISH = 0x02; 
    /** Calling party category, german operator. */
    public static final int CPC_OPERATOR_GERMAN = 0x03; 
    /** Calling party category, russian operator. */
    public static final int CPC_OPERATOR_RUSSIAN = 0x04; 
    /** Calling party category, spanish operator. */
    public static final int CPC_OPERATOR_SPANISH = 0x05; 

    public static final int CPC_OPERATOR_LANGUAGE6 = 0x06;
    public static final int CPC_OPERATOR_LANGUAGE7 = 0x07;
    public static final int CPC_OPERATOR_LANGUAGE8 = 0x08;
    public static final int CPC_OPERATOR_CODE9 = 0x09;

    /** Calling party category, normal subscriber. */
    public static final int CPC_ORDINARY_CALLING_SUBSCRIBER = 0x0A; 
    /** Calling party category, subscriber with priority. */
    public static final int CPC_CALLING_SUBSCRIBER_WITH_PRIORITY = 0x0B; 
    /** Calling party category, data call. */
    public static final int CPC_DATA_CALL = 0x0C; 
    /** Calling party category, test call. */
    public static final int CPC_TEST_CALL = 0x0D; 

    /** Calling party category, pay station. */
    public static final int CPC_PAY_PHONE = 0x0E; 

    public static final int CPC_SPARE = 0x0E;
    public static final int CPC_PAYPHONE = 0x0F;

    /** Calling party category, emergency service call. */
    public static final int CPC_EMERGENCY_SERVICE_CALL = 0xE0; 
    /** Calling party category, high priority emergency service call. */
    public static final int CPC_HIGH_PRIORITY_EMERGENCY_SERVICE_CALL = 0xE1; 
    /** Calling party category, nationa security emergency preparedness call. */
    public static final int CPC_NATIONAL_SECURITY_EMERGENCY_PREPAREDNESS_CALL = 0xE2; 

    /** Carrier selection information, no indication (ANSI). */
    public static final byte CSI_NO_INDICATION = 0x00; 
    /** Carrier selection information, presubscribed no override (ANSI). */
    public static final byte CSI_IDENTIFICATION_PRESUBSCRIBED_NOT_INPUT_BY_CALLING_PARTY = 0x01; 
    /** Carrier selection information, presubscribed, override (ANSI). */
    public static final byte CSI_IDENTIFICATION_PRESUBSCRIBED_INPUT_BY_CALLING_PARTY = 0x02; 
    /** Carrier selection information, presubscribed, unknown (ANSI). */
    public static final byte CSI_IDENTIFICATION_PRESUBSCRIBED_INPUT_BY_CALLING_PARTY_UNDERMINED = 0x03; 
    /** Carrier selection information, no presubscription (ANSI). */
    public static final byte CSI_IDENTIFICATION_NOT_PRESUBSCRIBED_INPUT_BY_CALLING_PARTY = 0x04; 

    /** Circuit group supervision message type indicator, maintenance oriented (ITU). */
    public static final byte CGSMTI_MAINTENANCE_ORIENTED = 0x00; 
    /** Circuit group supervision message type indicator, hardware failure oriented (ITU). */
    public static final byte CGSMTI_HARDWARE_FAILURE_ORIENTED = 0x01; 

    /** Continuity indicators, continuity check failed (ITU). */
    public static final byte CI_CONTINUITY_CHECK_FAILED = 0x00; 
    /** Continuity indicators, continuity check successful (ITU). */
    public static final byte CI_CONTINUITY_CHECK_SUCCESSFUL = 0x01; 

    /** Circuit validation response indiator, success (ANSI). */
    public static final byte CVRI_SUCCESSFUL = 0x00; 
    /** Circuit validation response indiator, failure (ANSI). */
    public static final byte CVRI_FAILURE = 0x01; 

    /** Facility indicator, user-to-user service (ITU). */
    public static final byte FI_USER_TO_USER_SERVICE = 0x02; 

    /** Redirecting number restriction indicator, presentation allowed (ITU). */
    public static final byte RRI_PRESENTATION_ALLOWED = 0x00; 
    /** Redirecting number restriction indicator, presentation restricted (ITU). */
    public static final byte RRI_PRESENTATION_RESTRICTED = 0x01; 

    /** Special processing request, service processig requested (ANSI). */
    public static final byte SPR_SERVICE_PROCESSING_REQUESTED = 0x7F; 

    /** Suspend resume indicator, ISDN subscriber initiated. */
    public static final byte SRI_ISDN_SUBSCRIBER_INITIATED = 0x00; 
    /** Suspend resume indicator, network initiated. */
    public static final byte SRI_NETWORK_INITIATED = 0x01; 

    /** Transmission medium requirement, speech (ITU). */
    public static final byte TMR_SPEECH = 0x00; 
    /** Transmission medium requirement, 64 kbps unrestricted (ITU). */
    public static final byte TMR_64_KBPS_UNRESTRICTED = 0x02; 
    /** Transmission medium requirement, 3.1 kHz audio (ITU). */
    public static final byte TMR_3DOT1_KHZ_AUDIO = 0x03; 
    /** Transmission medium requirement, 64 kbps preferred (ITU). */
    public static final byte TMR_64_KBPS_PREFERRED = 0x06; 
    /** Transmission medium requirement, 2x64 kbps unrestricted (ITU). */
    public static final byte TMR_2_by_64_KBPS_UNRESTRICTED = 0x07; 
    /** Transmission medium requirement, 384 kbps unrestricted (ITU). */
    public static final byte TMR_384_KBPS_UNRESTRICTED = 0x08; 
    /** Transmission medium requirement, 1536 kbps unrestricted (ITU). */
    public static final byte TMR_1536_KBPS_UNRESTRICTED = 0x09; 
    /** Transmission medium requirement, 1920 kbps unrestricted (ITU). */
    public static final byte TMR_1920_KBPS_UNRESTRICTED = 0x0A; 

    /** Transmission medium requirement prime, speech (ITU). */
    public static final byte TMRP_SPEECH = 0x00; 
    /** Transmission medium requirement prime, 64 kbps unrestricted (ITU). */
    public static final byte TMRP_64_KBPS_UNRESTRICTED = 0x02; 
    /** Transmission medium requirement prime, 3.1 kHz audio (ITU). */
    public static final byte TMRP_3DOT1_KHZ_AUDIO = 0x03; 

    // note that all TMR are applicable to TMR prime (when we want request two
    // services combined, we want all the choices for both)

    /** Transmission medium used, speech (ITU). */
    public static final byte TMU_SPEECH = 0x00; 
    /** Transmission medium used, 64 kbps unrestricted (ITU). */
    public static final byte TMU_64_KBPS_UNRESTRICTED = 0x02; 
    /** Transmission medium used, 3.1 kHz speech (ITU). */
    public static final byte TMU_3DOT1_KHZ_AUDIO = 0x03; 
    /** Transmission medium used, 64 kbit preferred (ITU). */
    public static final byte TMU_64_KBPS_PREFERRED = 0x06; 

    // note that all TMR are applicable to TMU (yes we really expect the
    // required transmission medium to be used)

    /** ISUP primitive, unset. */
    public static final int ISUP_PRIMITIVE_UNSET = 0;
    /** ISUP primitive, call progress alerting. */
    public static final int ISUP_PRIMITIVE_ALERT = 1;
    /** ISUP primitive, answer. */
    public static final int ISUP_PRIMITIVE_ANSWER = 2; 
    /** ISUP primitive, initial address. */
    public static final int ISUP_PRIMITIVE_SETUP = 3;
    /** ISUP primitive, release. */
    public static final int ISUP_PRIMITIVE_RELEASE = 4;
    /** ISUP primitive, release complete. */
    public static final int ISUP_PRIMITIVE_RELEASE_RESPONSE = 5;
    /** ISUP primitive, blocking request. */
    public static final int ISUP_PRIMITIVE_BLOCKING = 6;
    /** ISUP primitive, unblocking request. */
    public static final int ISUP_PRIMITIVE_UNBLOCKING = 7;
    /** ISUP primitive, blocking acknolwedgement. */
    public static final int ISUP_PRIMITIVE_BLOCKING_ACK = 8;
    /** ISUP primitive, unblocking acknowledgement. */
    public static final int ISUP_PRIMITIVE_UNBLOCKING_ACK = 9; 
    /** ISUP primitive, reset circuit request. */
    public static final int ISUP_PRIMITIVE_RESET_CIRCUIT = 10;
    /** ISUP primitive, maintenance timeout indication. */
    public static final int ISUP_PRIMITIVE_MAINT_TMO_IND = 11; 
    /** ISUP primitive, call failure indication. */
    public static final int ISUP_PRIMITIVE_CALL_FAILURE_IND = 12; 
    /** ISUP primitive, ISUP error indication. */
    public static final int ISUP_PRIMITIVE_ISUP_ERROR_IND = 13; 
    /** ISUP primitive, automatic reattempt indication. */
    public static final int ISUP_PRIMITIVE_REATTEMPT_IND = 14; 
    /** ISUP primitive, resteart indication. */
    public static final int ISUP_PRIMITIVE_START_RESET_IND = 15; 
    /** ISUP primitive, stop request. */
    public static final int ISUP_PRIMITIVE_STOP_MAINT_REQ = 16; 
    /** ISUP primitive, MTP pause indication. */
    public static final int ISUP_PRIMITIVE_MTP_PAUSE = 17; 
    /** ISUP primitive, MTP resume indication. */
    public static final int ISUP_PRIMITIVE_MTP_RESUME = 18; 
    /** ISUP primitive, MTP status indication (congestion status). */
    public static final int ISUP_PRIMITIVE_MTP_STATUS_CONGESTION = 19; 
    /** ISUP primitive, MTP status indication (user status). */
    public static final int ISUP_PRIMITIVE_MTP_STATUS_USER_PART = 20; 
    /** ISUP primitive, call progress. */
    public static final int ISUP_PRIMITIVE_CALL_PROGRESS = 21; 
    /** ISUP primitive, circuit group query. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY = 22; 
    /** ISUP primitive, cirguit group query acknolwedgemet. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY_RESP = 23; 
    /** ISUP primitive, circuit reservation. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_RESERVATION = 24; 
    /** ISUP primitive, circuit validation response. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_VALIDATION_RESP = 25; 
    /** ISUP primitive, confusion. */
    public static final int ISUP_PRIMITIVE_CONFUSION = 26; 
    /** ISUP primitive, connect. */
    public static final int ISUP_PRIMITIVE_CONNECT = 27; 
    /** ISUP primitive, exit. */
    public static final int ISUP_PRIMITIVE_EXIT = 28; 
    /** ISUP primitive, facility. */
    public static final int ISUP_PRIMITIVE_FACILITY = 29; 
    /** ISUP primitive, facility reqject. */
    public static final int ISUP_PRIMITIVE_FACILITY_REJECT = 30; 
    /** ISUP primitive, facility request. */
    public static final int ISUP_PRIMITIVE_FACILITY_REQUEST = 31; 
    /** ISUP primitive, facility accept. */
    public static final int ISUP_PRIMITIVE_FACILITY_ACCEPT = 32; 
    /** ISUP primitive, forward transfer. */
    public static final int ISUP_PRIMITIVE_FORWARD_TRANSFER = 33; 
    /** ISUP primitive, suspend. */
    public static final int ISUP_PRIMITIVE_SUSPEND = 34; 
    /** ISUP primitive, resume. */
    public static final int ISUP_PRIMITIVE_RESUME = 35;
    /** ISUP primitive, information. */
    public static final int ISUP_PRIMITIVE_INFORMATION = 36; 
    /** ISUP primitive, informatoin request. */
    public static final int ISUP_PRIMITIVE_INFORMATION_REQ = 37; 
    /** ISUP primitive, continuity check indication. */
    public static final int ISUP_PRIMITIVE_CONTINUITY_CHECK_REQ = 38; 
    /** ISUP primitive, overload (ACL). */
    public static final int ISUP_PRIMITIVE_OVERLOAD = 39; 
    /** ISUP primitive, loopback acknowlegement. */
    public static final int ISUP_PRIMITIVE_LOOPBACK_ACK = 40; 
    /** ISUP primitive, unequipped CIC. */
    public static final int ISUP_PRIMITIVE_UNEQUIPPED_CIC = 41; 
    /** ISUP primitive, circuit reservation ack. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_RESERVATION_ACK = 42; 
    /** ISUP primitive, circuit validation test request. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_VALIDATION_TEST = 43;
    /** ISUP primitive, user part test. */
    public static final int ISUP_PRIMITIVE_USER_PART_TEST = 44; 
    /** ISUP primitive, MTP status (user part available). */
    public static final int ISUP_PRIMITIVE_USER_PART_AVAILABLE = 45; 
    /** ISUP primitive, network resource management. */
    public static final int ISUP_PRIMITIVE_NETWORK_RESOURCE_MGMT = 46; 
    /** ISUP primitive, user to user information. */
    public static final int ISUP_PRIMITIVE_USER_TO_USER_INFO = 47; 
    /** ISUP primitive, identification request. */
    public static final int ISUP_PRIMITIVE_IDENTIFICATION_REQ = 48; 
    /** ISUP primitive, identification response. */
    public static final int ISUP_PRIMITIVE_IDENTIFICATION_RESP = 49; 
    /** ISUP primitive, segmentation. */
    public static final int ISUP_PRIMITIVE_SEGMENTATION = 50; 
    /** ISUP primitive, circuit group reset. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_GRP_RESET = 51; 
    /** ISUP primitive, circuit group reset acknowledgement. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_GRP_RESET_ACK = 52; 
    /** ISUP primitive, circuit group blocking request. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING = 53; 
    /** ISUP primitive, circuit group unblocking request. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING = 54; 
    /** ISUP primitive, circuit group blocking acknolwedgement. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING_ACK = 55; 
    /** ISUP primitive, circuit group unblocking acknonwledgement. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING_ACK = 56; 
    /** ISUP primitive, pass along. */
    public static final int ISUP_PRIMITIVE_PASS_ALONG = 57; 
    /** ISUP primitive, subsequent address message. */
    public static final int ISUP_PRIMITIVE_SUBSEQUENT_ADDRESS = 58; 
    /** ISUP primitive, raw message. */
    public static final int ISUP_PRIMITIVE_RAW_MESSAGE = 59; 
    /** ISUP primitive, circuit group local query. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY = 60; 
    /** ISUP primitive, circuit group local query response. */
    public static final int ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY_RESP = 61; 
    /** ISUP primitive, continutity check request. */
    public static final int ISUP_PRIMITIVE_CONTINUITY = 62; 

    /** Indicates ISUP protocol corresponding to ANSI 1992 specification. */
    public static final int ISUP_PV_ANSI_1992 = 100; 
    /** Indicates ISUP protocol corresponding to ANSI 1995 specification. */
    public static final int ISUP_PV_ANSI_1995 = 104; 
    /** Indicates ISUP protocol corresponding to ITU 1993 specification. */
    public static final int ISUP_PV_ITU_1993 = 1; 

    /** Not set. */
    public static final int NOT_SET = -1; 
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
