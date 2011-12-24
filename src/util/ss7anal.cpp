//  ==========================================================================
//  
//  @(#) $Id$
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
//  Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
//  Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
//  License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>, or
//  write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
//  02139, USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date$ by $Author$
//  
//  --------------------------------------------------------------------------
//  
//  $Log$
//  ==========================================================================

static char const ident[] = "$Id$";

//
// This file is for analysis of SS7 pcap files or live streams.  It uses the
// pcap library to open the capture file or live stream.  It expects
// DLT_MTP2_WITH_PHDR link types.
//
// The program determines a few things automatically.
//
// 1.	There is no protocol type in the DLT_MTP2_WITH_PDHR header.  It could be
//	ANSI or ITU or just about anything.  The pseudo header does, however,
//	intidcate whether Annex A extended sequence numbers are used or not, so
//	we do know the length of the level 2 header.  Otherwise, because there
//	are FISUs in the file, we would be able to determine the length of the
//	level2 header by the minimum sized message in the file.  We will
//	probably still used this as an extra check.
//
//	The size of the largest possible FISU or LSSU is always smaller than the
//	smallest possible MSU.  FISU and LSSU only differ in size by one octet,
//	so any FISU or LSSU in the file can identify whether extended sequence
//	numbers are used or not.
//
// 2.	The MTP level could be ANSI or ITU or anything.  There is no information
//	in the pseudo header tha provides the level 3 protocol; however, what is
//	really required is not the exact protocol but a determination of the
//	length of the MTP RL (routing label): once that is determined everything
//	else is easy.
//
//	The entire size of an ITU (14-bit PC) management message, such as
//	SLTA or SLTM, is too large to be a FISU or LSSU, but to small to be any
//	ANSI (24-bit PC) message.  The 24-bit PC routing label is 3 bytes longer
//	than an ITU routing label.  The minimum sized ANSI message is on octet
//	long (SLC).  The minimum sized ITU message is zero octets long: SLC in
//	SLS field of RL.  So ANSI minimum message size is SIF(1) + RL(7) +
//	SLC(1) = SIO(8).  The ITU minimum message size is SIF(1) + RL(4) =
//	SIO(4).  Therefore, any MTP management message with an SIO field shorter
//	than 8 is an ITU message.
//
//	For ISUP, things are a little different.  There are always two octets of
//	CIC and then one octet message type.  Of the message types for ISUP
//	there are several messages that have no parameters, or a normally empty
//	optional parameter list.  These are RLC, CCR, RSC, BLO, UBL, BLA, UBA,
//	LPA, UCIC.  Some have fixed sized mandatory parameters only: COT, SUS,
//	RES, FAR, FAA.  Some have no parameters but only exist for ITU: OLM,
//	CRG.  Some have no parameters but only exist for ANSI: CRA, CVT.
//
//	Probably one of the best ones is COT.  COT must be sent performed on any
//	active ISUP trunk and has only one byte fixed parameter.  RLC is good,
//	but only if the ITUT version does not have any optional parameters (ANSI
//	RLC does not support optional parameters).  An ITU COT message is SIF(1)
//	+ RL(4) + CIC(2) + MT(1) + COTI(1) = SIO(8).  An ANSI COT message is
//	SIF(1) + RL(7) + CIC(2) + MT(1) + COTI(1) = SIO(11).  An ITU RLC message
//	is (often) SIF(1) + RL(4) + CIC(2) + MT(1) + EOP(1) = SIO(8).  An ANSI
//	RLC message is SIF(1) + RL(7) + CIC(2) + MT(1) = SIO(10).  The minimum
//	ANSI ISUP message size is SIF(1) + RL(7) + CIC(2) + MT(1) = SIO(10), so
//	any ISUP message with an SIO field shorter than 10 is an ITU message.
//
//	Then there is the brute force approach which is to decode every message
//	both ways and see whether one fails.  A sufficient numer of consecutive
//	one-sided failures (on MSUs) should indicate that it is the other.  Or
//	simple statistics can be kept.  Note that the protocol is per-link and
//	not per file (or capture source): there is nothing stopping mixed
//	protocol links being captured to or from the same source.
//
// 3.	The rate of the signalling link can be estimated and narrowed down.  If
//	Annex A is used and the routing label is 56 bits (ANSI), it is likely
//	that the rate is 1.544.  This can be verified by checking the timestamps
//	between an MSU and the previous packet.  Our capture compresses FISUs by
//	sending the first and last FISU, so the MSU and its previous packet
//	should be separated by close to the emmission delay for the MSU.  The
//	reason we need the rate is to perform erlang calculations.
//
// 4.	Because signalling links must send periodic SLTM and STLA messages, and
//	may send SSLTM and SSLTA messages, it is possible to go beyond the link
//	code that is contained in the pseudo header to identify the local and
//	remote point codes and the signalling link code of the signalling link.
//	This is done by examining any (S)SLTM/(S)SLTA messages in the data
//	stream.
//
// 5.	Once the link OPC/DPC/SLC have been established, the direction flags in
//	the pseudo-header can be fixed up.  Two link ids that have symmetrical
//	PC/PC/SLC identities represent two directions of the same link.  If the
//	greater number of links have the same point code at one end, then that
//	is the host of interest and messges leaving that point code are Tx
//	direction and messages arriving at that point code are Rx messages.
//
// 6.	Once the signalling point codes on either end of the signalling link are
//	established from SLTM/SLTA messages, it is possible to to determine
//	whether signalling points on either end of the link have the transfer
//	function.  MSUs that are addressed to DPCs of other than the signalling
//	point at the end of the link, are expecting transfer functionality from
//	the SPC at the end of the link.  The same is true in the reverse
//	direction.  It can be determined whether the link is an F link, and A/E
//	links, or a B/C/D link.  It might be further possible to distinguish a C
//	link by its tendency to send TFP for a destination before sending
//	messages to that destination, and otherwise dead quiet.
//
// 7.	After identifying these things, it is possible to generate almost all
//	Q.752 and ANSI T1.116 operational measurements from the data stream.  It
//	would be a good idea to capture frames until the determinations above
//	are made first and then begin again with the captured frames.  When
//	working with a file source, the file can simply be rewound.
//

extern "C" {
#       include <stropts.h>
#       include <stdlib.h>
#       include <unistd.h>
#       include <stdio.h>
#	include <string.h>
#	include <errno.h>
#	include <sys/types.h>
#	include <sys/stat.h>
#	include <sys/poll.h>
#	include <sys/time.h>
#	include <sys/ioctl.h>
#	include <sys/stat.h>
#	include <fcntl.h>

#ifdef _GNU_SOURCE
#	include <getopt.h>
#endif

#	include <time.h>
#	include <signal.h>
#	include <syslog.h>
#	include <sys/utsname.h>

#	include <pcap/pcap.h>
}

#include <list>
#include <vector>
#include <algorithm>
#include <hash_map>

using std::list;
using std::vector;
using __gnu_cxx::hash_map;

int output = -1;
int debug = 0;

FILE *stdlog = NULL;

enum MessageType {
	ISUP_MT_IAM	= 0x01,		// 0b00000001 - Initial address
	ISUP_MT_SAM	= 0x02,		// 0b00000010 - Subsequent address (not ANSI)
	ISUP_MT_INR	= 0x03,		// 0b00000011 - Information request
	ISUP_MT_INF	= 0x04,		// 0b00000100 - Information
	ISUP_MT_COT	= 0x05,		// 0b00000101 - Continuity
	ISUP_MT_ACM	= 0x06,		// 0b00000110 - Address complete
	ISUP_MT_CON	= 0x07,		// 0b00000111 - Connect (not ANSI)
	ISUP_MT_FOT	= 0x08,		// 0b00001000 - Forward transfer
	ISUP_MT_ANM	= 0x09,		// 0b00001001 - Answer
	ISUP_MT_REL	= 0x0c,		// 0b00001100 - Release
	ISUP_MT_SUS	= 0x0d,		// 0b00001101 - Suspend
	ISUP_MT_RES	= 0x0e,		// 0b00001110 - Resume
	ISUP_MT_RLC	= 0x10,		// 0b00010000 - Release complete
	ISUP_MT_CCR	= 0x11,		// 0b00010001 - Continuity check request
	ISUP_MT_RSC	= 0x12,		// 0b00010010 - Reset circuit
	ISUP_MT_BLO	= 0x13,		// 0b00010011 - Blocking
	ISUP_MT_UBL	= 0x14,		// 0b00010100 - Unblocking
	ISUP_MT_BLA	= 0x15,		// 0b00010101 - Blocking acknowledgement
	ISUP_MT_UBA	= 0x16,		// 0b00010110 - Unblocking acknowledgement
	ISUP_MT_GRS	= 0x17,		// 0b00010111 - Circuit group reset
	ISUP_MT_CGB	= 0x18,		// 0b00011000 - Circuit group blocking
	ISUP_MT_CGU	= 0x19,		// 0b00011001 - Circuit group unblocking
	ISUP_MT_CGBA	= 0x1a,		// 0b00011010 - Circuit group blocking acknowledgement
	ISUP_MT_CGUA	= 0x1b,		// 0b00011011 - Circuit group unblocking acknowledgement
	ISUP_MT_CMR	= 0x1c,		// 0b00011100 - Call Modification Request (not ANSI)
	ISUP_MT_CMC	= 0x1d,		// 0b00011101 - Call Modification Completed (not ANSI)
	ISUP_MT_CMRJ	= 0x1e,		// 0b00011110 - Call Modification Reject (not ANSI)
	ISUP_MT_FAR	= 0x1f,		// 0b00011111 - Facility request (not ANSI)
	ISUP_MT_FAA	= 0x20,		// 0b00100000 - Facility accepted (not ANSI)
	ISUP_MT_FRJ	= 0x21,		// 0b00100001 - Facility reject (not ANSI)
	ISUP_MT_FAD	= 0x22,		// 0b00100010 - Facility Deactivated (not ANSI)
	ISUP_MT_FAI	= 0x23,		// 0b00100011 - Facility Information (not ANSI)
	ISUP_MT_LPA	= 0x24,		// 0b00100100 - Loop back acknowledgement
	ISUP_MT_CSVQ	= 0x25,		// 0b00100101 - CUG Selection and Validation Request (not ANSI)
	ISUP_MT_CSVR	= 0x26,		// 0b00100110 - CUG Selection and Validation Response (not ANSI)
	ISUP_MT_DRS	= 0x27,		// 0b00100111 - Delayed release (not ANSI)
	ISUP_MT_PAM	= 0x28,		// 0b00101000 - Pass along
	ISUP_MT_GRA	= 0x29,		// 0b00101001 - Circuit group reset acknowledgement
	ISUP_MT_CQM	= 0x2a,		// 0b00101010 - Circuit group query
	ISUP_MT_CQR	= 0x2b,		// 0b00101011 - Circuit group query response
	ISUP_MT_CPG	= 0x2c,		// 0b00101100 - Call progress
	ISUP_MT_USR	= 0x2d,		// 0b00101101 - User-to-user information (not ANSI)
	ISUP_MT_UCIC	= 0x2e,		// 0b00101110 - Unequipped circuit identification code
	ISUP_MT_CFN	= 0x2f,		// 0b00101111 - Confusion
	ISUP_MT_OLM	= 0x30,		// 0b00110000 - Overload (not ANSI)
	ISUP_MT_CRG	= 0x31,		// 0b00110001 - Charge information (not ANSI)
	ISUP_MT_NRM	= 0x32,		// 0b00110010 - Network resource management (not ANSI)
	ISUP_MT_FAC	= 0x33,		// 0b00110011 - Facility
	ISUP_MT_UPT	= 0x34,		// 0b00110100 - User part test (not ANSI)
	ISUP_MT_UPA	= 0x35,		// 0b00110101 - User part available (not ANSI)
	ISUP_MT_IDR	= 0x36,		// 0b00110110 - Identification request (not ANSI)
	ISUP_MT_IRS	= 0x37,		// 0b00110111 - Identification response (not ANSI)
	ISUP_MT_SGM	= 0x38,		// 0b00111000 - Segmentation
	ISUP_MT_CRA	= 0xe9,		// 0b11101001 - Circuit Reservation Ack (ANSI 2000)
	ISUP_MT_CRM	= 0xea,		// 0b11101010 - Circuit Reservation (ANSI 2000)
	ISUP_MT_CVR	= 0xeb,		// 0b11101011 - Circuit Validation Response (ANSI 2000)
	ISUP_MT_CVT	= 0xec,		// 0b11101100 - Circuit Validation Test (ANSI 2000)
	ISUP_MT_EXM	= 0xed,		// 0b11101101 - Exit (old Bellcore/ANSI 2000)
	ISUP_MT_NON	= 0xf8,		// 0b11111000 - National Notification (Spain)
	ISUP_MT_LLM	= 0xfc,		// 0b11111100 - National Malicious Call (Spain)
	ISUP_MT_CAK	= 0xfd,		// 0b11111101 - Charge Acknowledgement (Singapore)
	ISUP_MT_TCM	= 0xfe,		// 0b11111110 - Tariff Charge (Singapore)
	ISUP_MT_MCP	= 0xff,		// 0b11111111 - Malicious Call Print (Singapore)
};					// message type
enum ParameterType {
	ISUP_PT_EOP	= 0UL,		// 0x00 - 0b00000000 - End of optional parameters
	ISUP_PT_CLRF	= 1UL,		// 0x01 - 0b00000001 - Call reference @
	ISUP_PT_TMR	= 2UL,		// 0x02 - 0b00000010 - Transmission medium requirement
	ISUP_PT_ATP	= 3UL,		// 0x03 - 0b00000011 - Access transport
	ISUP_PT_CDPN	= 4UL,		// 0x04 - 0b00000100 - Called party number
	ISUP_PT_SUBN	= 5UL,		// 0x05 - 0b00000101 - Subsequent number
	ISUP_PT_NCI	= 6UL,		// 0x06 - 0b00000110 - Nature of connection indicators
	ISUP_PT_FCI	= 7UL,		// 0x07 - 0b00000111 - Forward call indicators
	ISUP_PT_OFCI	= 8UL,		// 0x08 - 0b00001000 - Optional forward call indicators
	ISUP_PT_CPC	= 9UL,		// 0x09 - 0b00001001 - Calling party's category
	ISUP_PT_CGPN	= 10UL,		// 0x0a - 0b00001010 - Calling party number
	ISUP_PT_RDGN	= 11UL,		// 0x0b - 0b00001011 - Redirecting number
	ISUP_PT_RDNN	= 12UL,		// 0x0c - 0b00001100 - Redirection number
	ISUP_PT_CONR	= 13UL,		// 0x0d - 0b00001101 - Connection request
	ISUP_PT_INRI	= 14UL,		// 0x0e - 0b00001110 - Information request indicators
	ISUP_PT_INFI	= 15UL,		// 0x0f - 0b00001111 - Information indicators @
	ISUP_PT_COTI	= 16UL,		// 0x10 - 0b00010000 - Continuity indicators
	ISUP_PT_BCI	= 17UL,		// 0x11 - 0b00010001 - Backward call indicators
	ISUP_PT_CAUS	= 18UL,		// 0x12 - 0b00010010 - Cause indicators
	ISUP_PT_RDI	= 19UL,		// 0x13 - 0b00010011 - Redirection information
	ISUP_PT_CGI	= 21UL,		// 0x15 - 0b00010101 - Circuit group supervision msg type ind
	ISUP_PT_RS	= 22UL,		// 0x16 - 0b00010110 - Range and status
	ISUP_PT_CMI	= 23UL,		// 0x17 - 0b00010111 - Call modification indicators (Blue Book)
	ISUP_PT_FACI	= 24UL,		// 0x18 - 0b00011000 - Facility indicator
	ISUP_PT_FAII	= 25UL,		// 0x19 - 0b00011001 - Facility information indicators (Bcore)
	ISUP_PT_CUGI	= 26UL,		// 0x1a - 0b00011010 - Closed user group interlock code
	ISUP_PT_INDEX	= 27UL,		// 0x1b - 0b00011011 - Index (Bellcore)
	ISUP_PT_USI	= 29UL,		// 0x1d - 0b00011101 - User service information
	ISUP_PT_SPC	= 30UL,		// 0x1e - 0b00011110 - Signalling point code @ (Bellcore)
	ISUP_PT_UUI	= 32UL,		// 0x20 - 0b00100000 - User to user information
	ISUP_PT_CONN	= 33UL,		// 0x21 - 0b00100001 - Connected number
	ISUP_PT_SRIS	= 34UL,		// 0x22 - 0b00100010 - Suspend/resume indicators
	ISUP_PT_TNS	= 35UL,		// 0x23 - 0b00100011 - Transit network selection @
	ISUP_PT_EVNT	= 36UL,		// 0x24 - 0b00100100 - Event information
	ISUP_PT_CSI	= 38UL,		// 0x26 - 0b00100110 - Circuit state indicator @
	ISUP_PT_ACL	= 39UL,		// 0x27 - 0b00100111 - Automatic congestion level
	ISUP_PT_OCDN	= 40UL,		// 0x28 - 0b00101000 - Original called number
	ISUP_PT_OBCI	= 41UL,		// 0x29 - 0b00101001 - Optional backward call indicators
	ISUP_PT_UUIND	= 42UL,		// 0x2a - 0b00101010 - User to user indicators
	ISUP_PT_ISPC	= 43UL,		// 0x2b - 0b00101011 - Origination ISC point code
	ISUP_PT_GNOT	= 44UL,		// 0x2c - 0b00101100 - Generic notification
	ISUP_PT_CHI	= 45UL,		// 0x2d - 0b00101101 - Call history information
	ISUP_PT_ADI	= 46UL,		// 0x2e - 0b00101110 - Access delivery information
	ISUP_PT_NSF	= 47UL,		// 0x2f - 0b00101111 - Network specific facilities @
	ISUP_PT_USIP	= 48UL,		// 0x30 - 0b00110000 - User service information prime
	ISUP_PT_PROP	= 49UL,		// 0x31 - 0b00110001 - Propagation delay counter
	ISUP_PT_ROPS	= 50UL,		// 0x32 - 0b00110010 - Remote operations @
	ISUP_PT_SA	= 51UL,		// 0x33 - 0b00110011 - Service activation @
	ISUP_PT_UTI	= 52UL,		// 0x34 - 0b00110100 - User teleservice information
	ISUP_PT_TMU	= 53UL,		// 0x35 - 0b00110101 - Transmission medium used
	ISUP_PT_CDI	= 54UL,		// 0x36 - 0b00110110 - Call diversion information
	ISUP_PT_ECI	= 55UL,		// 0x37 - 0b00110111 - Echo control information
	ISUP_PT_MCI	= 56UL,		// 0x38 - 0b00111000 - Message compatibility information
	ISUP_PT_PCI	= 57UL,		// 0x39 - 0b00111001 - Parameter compatibility information
	ISUP_PT_MLPP	= 58UL,		// 0x3a - 0b00111010 - MLPP preference (Precedence)
	ISUP_PT_MCIQ	= 59UL,		// 0x3b - 0b00111011 - MCID request indicator
	ISUP_PT_MCIR	= 60UL,		// 0x3c - 0b00111100 - MCID response indicator
	ISUP_PT_HOPC	= 61UL,		// 0x3d - 0b00111101 - Hop counter (reserved)
	ISUP_PT_TMRP	= 62UL,		// 0x3e - 0b00111110 - Transmission medium requirement prime
	ISUP_PT_LN	= 63UL,		// 0x3f - 0b00111111 - Location number
	ISUP_PT_RDNR	= 64UL,		// 0x40 - 0b01000000 - Redirection number restriction
	ISUP_PT_FREEP	= 65UL,		// 0x41 - 0b01000001 - Freephone indicators (reserved)
	ISUP_PT_GREF	= 66UL,		// 0x42 - 0b01000010 - Generic reference (reserved)
	ISUP_PT_GNUM	= 192UL,	// 0xc0 - 0b11000000 - Generic number (address Bellcore)
	ISUP_PT_GDIG	= 193UL,	// 0xc1 - 0b11000001 - Generic digits @
	ISUP_PT_EGRESS	= 195UL,	// 0xc3 - 0b11000011 - Egress (ANSI)
	ISUP_PT_JUR	= 196UL,	// 0xc4 - 0b11000100 - Jurisdiction (ANSI)
	ISUP_PT_CIDC	= 197UL,	// 0xc5 - 0b11000101 - Carrier identification code (ANSI)
	ISUP_PT_BGROUP	= 198UL,	// 0xc6 - 0b11000110 - Business group (ANSI)
	ISUP_PT_NOTI	= 225UL,	// 0xe1 - 0b11100001 - Notification indicator (ANSI)
	ISUP_PT_SVACT	= 226UL,	// 0xe2 - 0b11100010 - Service activation (ANSI)
	ISUP_PT_TRNSRQ	= 227UL,	// 0xe3 - 0b11100011 - Transaction request (ANSI,Bellcore)
	ISUP_PT_SPR	= 228UL,	// 0xe4 - 0b11100100 - Special processing request (Bellcore)
	ISUP_PT_CGCI	= 229UL,	// 0xe5 - 0b11100101 - Cc't group char ind (ANSI,Bellcore)
	ISUP_PT_CVRI	= 230UL,	// 0xe6 - 0b11100110 - Cc't validation resp ind (ANSI,Bellcore)
	ISUP_PT_OTGN	= 231UL,	// 0xe7 - 0b11100111 - Outgoing trunk group numb (ANSI,Bellcore)
	ISUP_PT_CIN	= 232UL,	// 0xe8 - 0b11101000 - Circuit ident name (ANSI,Bellcore)
	ISUP_PT_CLLI	= 233UL,	// 0xe9 - 0b11101001 - Common language loc id (ANSI,Bellcore)
	ISUP_PT_OLI	= 234UL,	// 0xea - 0b11101010 - Originating line info (ANSI,Bellcore)
	ISUP_PT_CHGN	= 235UL,	// 0xeb - 0b11101011 - Charge number (ANSI,Bellcore)
	ISUP_PT_SVCD	= 236UL,	// 0xec - 0b11101100 - Service code indicator (ANSI,Bellcore)
	ISUP_PT_CSEL	= 238UL,	// 0xee - 0b11101110 - Carrier selection info (ANSI,Bellcore)
	ISUP_PT_ORI	= 243UL,	// 0xf3 - 0b11110011 - Outgoing route identification (Spain)
	ISUP_PT_IRI	= 244UL,	// 0xf4 - 0b11110100 - Incoming route identification (Spain)
	ISUP_PT_RATE	= 248UL,	// 0xf8 - 0b11111000 - Rate (Spain)
	ISUP_PT_IIC	= 249UL,	// 0xf9 - 0b11111001 - Identifier of incoming circuit (Spain)
	ISUP_PT_TOI	= 253UL,	// 0xfd - 0b11111101 - Trunk offering information (Singapore)
	ISUP_PT_TON	= 253UL,	// 0xfd - 0b11111101 - Type of notification (Spain)
	ISUP_PT_CRI	= 254UL,	// 0xfe - 0b11111110 - Charge rate information (Singapore)
	ISUP_PT_ICCI	= 255UL,	// 0xff - 0b11111111 - Call charge information (Singapore)
};
class Error;
class File;
class Message;
class Point;
class Node;
class Link;
class Path;
class Switch;
class TrunkGroup;
class Circuit;

class Error {
      private:
	int error;
      public:
	 Error(int val):error(val) {
	};

	~Error(void) {
	};
};

//  -------------------------------------------------------------------------
//
//  File class
//
//  -------------------------------------------------------------------------
class File {
      private:
	File() {
	};

      protected:
	FILE *fp;

      public:
	File(char *path) {
		if ((fp = fopen(path, "r")) == NULL)
			throw Error(1);
	};
	virtual ~File(void) {
		fclose(fp);
	};
	virtual Message *getmessage(void) = 0;
};

//  -------------------------------------------------------------------------
//
//  FileText class
//
//  -------------------------------------------------------------------------
//  Represents a text file.  When text files are used, each file can potentially
//  represent a different data link path.  However, we have no idea which data
//  link path that might be (no identifier) until
//
class FileText:public File {
      private:
	FileText(void) { };

      public:
	FileText(char *path) : File(path) {
		if (fgetc(fp) != '#')
			throw Error(1);
		ungetc('#', fp);
	};
	virtual ~FileText(void) {
	};
	virtual Message *getmessage(void) {
		return (NULL);
	};
};

//  -------------------------------------------------------------------------
//
//  FilePcap class
//
//  -------------------------------------------------------------------------
//  This class represents a pcap formatted file.
//
class FilePcap : public File {
      private:
	pcap_t *pp;
	char errbuf[PCAP_ERRBUF_SIZE];
	FilePcap(void) { };

      public:
	FilePcap(char *path) : File(path), pp(NULL) {
		if ((pp = pcap_fopen_offline(fp, errbuf)) == NULL)
			throw Error(1);
	};
	virtual ~FilePcap(void) {
		pcap_close(pp);
	};
	virtual Message *getmessage(void) {
		return (NULL);
	};
	int getDLT(void) const {
		return pcap_datalink(pp);
	};
	const char *getDLTName(void) const {
		return pcap_datalink_val_to_name(pcap_datalink(pp));
	};
	const char *getDLTDesc(void) const {
		return pcap_datalink_val_to_description(pcap_datalink(pp));
	};
};

File *
filetype(char *path)
{
	File *f;

	try {
		f = new FileText(path);
	}
	catch Error e1 {
		try {
			f = new FilePcap(path);
		}
                catch Error e2 {
			throw e2;
                }
        }
        return (f);
}

struct pseudohdr {
	unsigned char dir;
	unsigned char xsn;
	union {
		unsigned char lkid[2];
		unsigned short code;
	};
};

class Message {
      protected:
	virtual void read(void) = 0;
      public:
	struct pcap_pkthdr hdr;
	struct pseudohdr phdr;
	unsigned char *buf, *beg, *mid, *end;
	unsigned int bsn, bib, fsn, fib, li, ni, mp, si, dpc, opc, sls, cic, mt;
	Message(FILE *f) {
		this->read();
	};
        virtual ~Message() {
		if (beg != NULL)
			free(beg);
	};
	bool decode(Path& path);
};

class Mtp2Message : public Message {
};

class Mtp3Message : public Message {
};

class MtpmMessage : public Message {
};

class IsupMessage : public Message {
};

class SccpMessage : public Message {
};

//  -------------------------------------------------------------------------
//
//  Point class
//
//  -------------------------------------------------------------------------
// A Point class represents a signalling point somewhere in the network.
//
enum pointType {
	PT_UNKNOWN,	// generic signalling point
	PT_SSP,		// Service Switching Point
	PT_SCP,		// Service Control Point
	PT_STP		// Signal Transfer Point
};
class Point {
      protected:
	static hash_map<int,Point *> points;
	int pcode;			// point code for this point
	enum pointType signallingPoint;	// point code type
	Point(void) { };

      public:
	Point(int pc):pcode(pc) {
		points[pcode] = this;
	};
	virtual ~Point(void) {
		points.erase(pcode);
	};
	int pointCode(void) {
		return pcode;
	};
	int makeSSP(void) { signallingPoint = PT_SSP; }
	int makeSCP(void) { signallingPoint = PT_SCP; }
	int makeSTP(void) { signallingPoint = PT_STP; }
};

//  -------------------------------------------------------------------------
//
//  Pair class
//
//  -------------------------------------------------------------------------
//  A Pair class represents a mated pair of signalling points in the network.
//  This can be a mated pair of STPs or SCPs.  Pairs are identified by the point
//  codes in the pair.  A Pair can have an optional third point code, which is
//  an ANSI capability code that is used to refer to the mated pair.

struct prid {
	int pca;    // a node point code
	int pcb;    // b node point code
};
struct prid_hastr {
	size_t operator() (struct prid p) const {
		return (p.pca + p.pcb);
	};
};
struct prid_eqstr {
	bool operator() (const struct prid p1, const struct prid p2) const {
		return ((p1.pca == p2.pca && p1.pcb == p2.pcb) ||
			(p1.pcb == p2.pca && p1.pca == p2.pcb));
	};
};
class Pair {
      private:
	static hash_map<struct prid,Pair *,prid_hastr,prid_eqstr> pairs;
	struct prid prid;
	Pair(void) { };
	Node& aNode;	// 1st signalling point in pair
	Node& bNode;	// 2nd signalling point in pair
	Point *cPoint;	// optional common point code (capability code)

      public:
	Pair(Node& a, Node& b, Point *c = NULL) : aNode(a), bNode(b), cPoint(c),
		prid(a.pointCode(),b.pointCode())
	{
		if (pairs.find(prid) != pairs.end())
			throw Error(1);
		pairs[prid] = this;
	};
	~Pair(void) {
		pairs.erase(pid);
	};
};

//  -------------------------------------------------------------------------
//
//  Mate class
//
//  -------------------------------------------------------------------------
//  Provides a base class for mated things like SCP and STP.
//
class Mate {
      protected;
	Mate *mate;

      public:
	Mate(void) : mate(NULL) { };
	Mate(Mate *m = NULL) : mate(m) { };
	virtual Mate *getMate(void) const { return mate; };
	void setMate(Mate *m) {
		if (mate != m) {
			if (mate)
				mate->setMate(NULL);
			if ((mate = m))
				mate->setMate(this);
		}
	};
};


//  -------------------------------------------------------------------------
//
//  Switch class
//
//  -------------------------------------------------------------------------
//  A Switch class represents a signalling point with ISUP trunks to or from
//  which ISUP messages are exchanged.  Switch instances are indexed by Switches
//  have trunk groups to other switches.  Trunk groups are made up of individual
//  Circuits.
//
class Switch {
      private:
	static hash_map<int,Switch*> switches;
	Node& node;
	list<TrunkGroup *> trunkgroups;		// list of trunk groups
      public:
	Switch(int pc) : node(Node::get(pc)) {
		switches[node.pointCode()] = this;
		node.makeSSP;
	};
	~Switch(void) {
		switches.erase(node.pointCode());
		list<TrunkGroup *>::iterator it;
		while ((it = trunkgroups.begin()) != trunkgroups.end())
			delete (*it); // will remove self from list
	};
	void add(TrunkGroup& tg) {
		trunkgroups.push_back(tg);
	};
	static Switch& get(int pc) {
		Switch *s;
		hash_map<int,Switch*>::iterator it;

		if ((it = switches.find(pc)) == switches.end) {
			if ((s = new Switch(pc)) == NULL)
				throw Error(1);
		} else
			s = switches[pc];
		return s;
	};
};

//  -------------------------------------------------------------------------
//
//  Database class
//
//  -------------------------------------------------------------------------
//  I don't know how must we will do with it to start, but this is a class for
//  databases (SCP).
//
class Database : public Mate {
      private:
	static hash_map<int,Database *> databases;
	Node& node;
	list<SubSystem *> subsystems;	// list of subsystems
      public:
	Database(int pc, Mate *m = NULL) : Mate(m), node(Node::get(pc)) {
		node.makeSCP();
		databases[node.pointCode()] = this;
	};
	~Database(void) {
		databases.erase(node.pointCode());
	};
	virtual Database *getMate(void) const {
		return dynamic_downcast<Database *>(mate);
	};
};



//  -------------------------------------------------------------------------
//
//  Relay class
//
//  -------------------------------------------------------------------------
//  I don't know how much we will do with it to start, but this is a class for
//  relays (STP).
class Relay : public Mate {
      private:
	static hash_map<int,Relay *> relays;
	Node& node;

      public:
	Relay(int pc, Mate *m = NULL) : Mate(m), node(Node::get(pc)) {
		node.makeSTP();
		relays[node.pointCode()] = this;
	};
	~Relay(void) {
		relays.erase(node.pointCode());
	};
	virtual Relay *getMate(void) const {
		return dynamic_downcast<Relay *>(mate);
	};
};



//  -------------------------------------------------------------------------
//
//  Cluster class
//
//  -------------------------------------------------------------------------
//  This is for a cluster of databases that do not fit the simple model of the
//  mated pair (which is handled by the Pair class).
//
class Cluster {
      private:
	static list<Cluster *> clusters;
	list<Database *> databases;             // list of databases
      public:
	Cluster(void) {
		clusters.push_bash(this);
	};
	~Cluster(void) {
		for (list<Cluster *>::iterator it = clusters.begin();
				it != clustsers.end(); ++it) {
			if ((*it) == this) {
				clusters.erase(it);
				break;
			}
		}
		list<Database *>::iterator it;
		while ((it = databases.front()) != databases.end())
			delete (*it); // will remove self from list
	};
};

//  -------------------------------------------------------------------------
//
//  Node class
//
//  -------------------------------------------------------------------------
//  Node class represents a signalling point at one end of a link.  Nodes are
//  considered interesting when they are local.  Nodes are important when they
//  are handling user traffic (whether local or remote).
//
enum locType {
	LT_UNKNOWN,
	LT_LOCAL,
	LT_REMOTE
};
class Node : public Point {
      private:
	list<Link *> links;	// direct links attached to this node
	enum locType location;	// location
	bool interesting;	// interesting (monitored) node
	bool important;		// important (monitored) node
	Node(void) { };

      public:
	Node(int pc) : Point(pc), location(LT_UNKNOWN),
		interesting(false), important(false)
	{
	};
	virtual ~Node(void) {
	}
	bool isInteresting(void) const { return interesting; }
	bool isImportant(void) const { return important; }
	enum locType getLocaltion(void) const { return location; }
	void makeInteresting(void) { interesting = true; }
	void makeImportant(void) { important = true; }
	void setLocal(void) { location = LT_LOCAL; }
	void setRemote(void) { location = LT_REMOTE; }
};

//  -------------------------------------------------------------------------
//
//  LinkSet class
//
//  -------------------------------------------------------------------------
//  A LinkSet class is a group of Links that span the same nodes.  They are a
//  linkset in the true sense.
//
struct lsid {
	int pca;
	int pcb;
};
class LinkSet {
      private:
	static hash_map<struct lsid,LinkSet *,lsid_hastr,lsid_eqstr> linksets;
	map<int,Link *> links;
	struct lsid lsid;
	Node& aNode;
	Node& bNode;
	map<int,Node *> aRoutes;
	map<int,Node *> bRoutes;
	LinkSet(void) { };

      public:
	LinkSet(struct lsid id) :
		lsid(id),
		aNode(Node::get(id.pca)),
		bNode(Node::get(id.pcb))
	{
		linksets[lsid] = this;
	};
	~LinkSet(void) {
	};
};

//  -------------------------------------------------------------------------
//
//  Link class
//
//  -------------------------------------------------------------------------
//  A Link class is a pairing of paths that span between points.  An instance of
//  this class is not constructed until a Path instance determines its
//  membership to links (by intercepting SLTA/SLTM messages).
//
struct lkid {
	int opc;
	int dpc;
	int slc;
};
struct lkid_hastr {
	size_t operator() (struct lkid l) const {
		return (l.opc + l.dpc + l.slc);
        };
};
struct lkid_eqstr {
	bool operator() (const struct lkid l1, const struct lkid l2) const {
		return (l1.slc == l2.slc &&
			((l1.opc == l2.opc && l1.dpc == l2.dpc) ||
			 (l1.opc == l2.dpc && l1.dpc == l2.opc)));
	};
};
class Link {
      private:
	static hash_map < struct lkid, Link *, lkid_hastr, lkid_eqstr > links;
	Node& aNode;	// a signalling point
	Node& bNode;	// b signalling point
	struct lkid lkid;
	list<Path *> paths;		// list of paths (usually two)
	static Link& get(struct lkid id) {
		Link *l;
		hash_map < struct lkid, Link *, lkid_hastr, lkid_eqstr >::iterator it;

		if ((it = links.find(id)) == links.end()) {
			Node& a = Node::get(id.opc);
			Node& b = Node::get(id.dpc);

			if ((l = new Link(a, b, id.slc)) == NULL)
				throw Error(1);
		} else
			l = links[id];
		return (*l);
	};
      public:
	 Link(Node& a, Node& b, int slc)
	    : aNode(a), bNode(b), lkid(a.pointCode(), b.pointCode(), slc) {
		links[lkid] = this;
		aNode.add(*this);
		bNode.add(*this);
	};
	~Link(void) {
		bNode.del(this);
		aNode.del(this);
		links.erase(lkid);
	};
	int pointCodeA(void) const {
		return aNode.pointCode();
	};
	int pointCodeB(void) const {
		return bNode.pointCode();
	};
	int slc(void) const {
		return lkid.lsc;
	};
	void add(Path& p) {
		paths.push_back(&p);
	};
	void del(Path& p) {
		for (list<Path *>::iterator it = paths.begin(); it != paths.end(); ++it) {
			if ((*it) == &p) {
				paths.erase(it);
				break;
			}
		}
	};
};

//  -------------------------------------------------------------------------
//
//  Path class
//
//  -------------------------------------------------------------------------
// A Path class is a unique path within the source that is identified by its
// signalling link id in the DLT_MTP2_WITH_PHDR pseudo-header.  A new Path is
// created whenever a new id is discovered in a pseudo-header.  Path classes
// consume messages with an appropriate pseudo-header.
//
struct ptid {
	unsigned char sent;
	unsigned char annexa;
	unsigned char linkno[2];
};
enum rlType {
	RL_UNKNOWN,
	RL_14BIT_PC,
	RL_24BIT_PC
};
enum lhType {
        LH_UNKNOWN,
        LH_BASIC,
        LH_ANNEXA
};
enum mpType {
	MP_UNKNOWN,
	MP_JAPAN,
	MP_NATIONAL,
	MP_INTERNATIONAL
};
class Path {
      private:
        //
        // Path instances are indexed by their link code that appears in the
        // DLT_MTP_WITH_PDHR pseudo-header (see struct ptid).
        //
	static hash_map<int,Path *> paths;
	struct ptid ptid;
	int linkno;
	Link *link;			// pointer to link membership (null till discovered)
	struct lkid lkid;
	double rate;			// signalling link rate (0 till determined)
	enum rlType routingLabel;       // routing label type
        enum lhType linkHeader;         // link header type
	enum mpType messagePriority;	// message priority type
        list<Message *> msgs;           // message buffer
	Path(void) { };
	void process(Message & msg);
	static Path & get(struct ptid id) {
		int lkno = (id.linkno[0] << 8) | (id.linkno[1] << 0);
		Path *p;
		hash_map < int, Path * >::iterator it;

		if ((it = paths.find(lkno)) == paths.end()) {
			if ((p = new Path(id)) == NULL)
				throw Error(1);
		} else
			p = paths[lkno];
		return (*p);
	};
      public:
	Path(struct ptid id) :
            ptid(id), linkno((int) (id.linkno[0] << 8) | (int) (id.linkno[1] << 0)),
            link(NULL), rate(0) {
		paths[linkno] = this;
	};
	~Path(void) {
		if (link != NULL)
			link->del(this);
		paths.erase(linkno);
	};
	const struct lkid &getLkid(void) {
		return lkid;
	};
	const struct ptid &getPtid(void) {
		return ptid;
	};
	static void add(Message & msg) {
		get(*msg.getPtid()).process(msg);
	};
	enum lhType getLinkHeader(void) const {
		return linkHeader;
	};
	void setLinkHeader(enum lhType type) {
		if (debug && linkHeader == LH_BASIC && type == LH_ANNEXA)
			fprintf(stderr, "link header type changed from basic to annex a\n");
		if (debug && linkHeader == LH_ANNEXA && type == LH_BASIC)
			fprintf(stderr, "link header type changed from annex a to basic\n");
		if (debug && linkHeader == LH_UNKNOWN && type == LH_ANNEXA)
			fprintf(stderr, "link header discovered as annex a\n");
		if (debug && linkHeader == LH_UNKNOWN && type == LH_BASIC)
			fprintf(stderr, "link header discovered as basic\n");
		linkHeader = type;
	};
	void setLink(Message& msg);
};

//  Message has decoded as an (S)SLTM/(S)SLTA message.  The message decoder
//  passes the message to this routine to set the link parameters if they have
//  not already been set.
void
Path::setLink(Message & msg)
{
	int loc_pc = msg.opc;
	int rem_pc = msg.dpc;
	int slc = msg.slc;

	if (link != NULL) {
		// already assigned a link, just check it
		if (lkid.slc == msg.scl && lkid.opc == msg.opc && lkid.dpc == msg.dpc)
			return;
		if (debug)
			fprintf(stderr, "link id changed %06x:%06x:%02x -> %06x:%06x:%02x\n",
				lkid.opc, lkid.dpc, lkid.slc, msg.opc, msg.dpc, msg.slc);
		link->del(*this);
		link == NULL;
	}
	lkid.opc = msg.opc;
	lkid.dpc = msg.dpc;
	lkid.slc = msg.slc;

	link = Link::get(lkid);
	link->add(*this);
}

//  Most of the magic is done here in processing the message.  Each message
//  received alters characteristics of the involved objects and can lead to
//  discovery of new objects.  If insufficient messages have been received to
//  determine the path type and link membership, the messages are buffered in
//  the @msgs@ list until sufficient messages have been received to do so.
//
void
Path::process(Message & msg)
{
	// Process the message and determine link membership as soon as possible.
	if (msg.phdr.xsn) {
		// pseudo-header says we are annex a
		if (debug && linkHeader == LH_BASIC)
			fprintf(stderr, "W: link header specified annex a when basic\n");
		if (debug && linkHeader == LH_UNKNOWN)
			fprintf(stderr, "I: link header specified basic\n");
	}
	if (msg.hdr.len < 5) {
		// only basic sequence numbers can be this small
		if (debug && linkHeader == LH_ANNEXA)
			fprintf(stderr, "W: link header changed from Annex A to basic\n");
		if (debug && linkHeader == LH_UNKNOWN)
			fprintf(stderr, "I: link header discovered to be basic\n");
		linkHeader = LH_BASIC;
	}
	if (5 < msg.hdr.len && msg.hdr.len < 8) {
		// only annex A sequence numbers can be this size
		if (debug && linkHeader == LH_BASIC)
			fprintf(stderr, "W: link header changed from basic to Annex A\n");
		if (debug && linkHeader == LH_UNKNOWN)
			fprintf(stderr, "I: link header discovered to be annex A\n");
		linkHeader = LH_ANNEXA;
	}
	if (linkHeader == LH_UNKNOWN) {
		// cannot decode message further
		msgs.push_back(msg);
		return;
	}
	// decode header and sif
}

struct tgid {
	int opc;
	int dpc;
};
struct tgid_hastr {
	size_t operator() (struct tgid t) const {
		return (t.opc + t.dpc);
	};
};
struct tgid_eqstr {
	bool operator() (const struct tgid t1, const struct tgid t2) const {
		return ((t1.opc == t2.opc && t1.dpc == t2.dpc) ||
			(t1.dpc == t2.opc && t1.opc == t2.dpc));
	};
};

//  -------------------------------------------------------------------------
//
//  TrunkGroup class
//
//  -------------------------------------------------------------------------
//  A Trunk group is a group of circuits that terminate on the same pair of
//  switches.
//
class TrunkGroup {
      private:
	static hash_map <struct tgid,TrunkgGroup *,tgid_hastr,tgid_eqstr> groups;
	Switch& aSwitch;
	Switch& bSwitch;
	struct tgid tgid;
	list<Circuit*> circuits;	// list of circuits
	static TrunkGroup& get(struct tgid &tgid) {
		struct tgid tgid = { opc, dpc };
		return get(tgid);
	};
      public:
	TrunkGroup(Switch& a, Switch& b)
	    : aSwitch(a), bSwitch(b), tgid(a.pointCode(), b.pointCode()) {
		groups[tgid] = this;
		aSwitch.add(*this);
		bSwitch.add(*this);
	};
	~TrunkGroup(void) {
		bSwitch.del(*this);
		aSwitch.del(*this);
		groups.erase(tgid);
	};
	int pointCodeA(void) {
		return aSwitch.pointCode();
	};
	int pointCodeB(void) {
		return bSwitch.pointCode();
	};
	static TrunkGroup& get(int a, int b) {
		TrunkGroup *t;
		hash_map<struct tgid,TrunkGroup *,tgid_hastr,tgid_eqstr>::iterator it;
		if ((it = groups.find(tgid)) == groups.end) {
			Switch& a = Switch::get(tgid.opc);
			Switch& b = Switch::get(tgid.dpc);
			if ((t = new TrunkGroup(a, b)) == NULL)
				throw Error(1);
		} else
			t = groups[tgid];
		return t;
	};
};

//  -------------------------------------------------------------------------
//
//  Circuit class
//
//  -------------------------------------------------------------------------
//  A Circuit class represent a circuit between two switches.  Several things
//  can be determined about switches.  ISUP management messages can be used to
//  determine the characteristics and state of the circuits.  Directionality can
//  be determined over the long term and so can selection method.  We can
//  collect all manner of statistics associated with a circuit.
//
class Circuit {
      private:
	static hash_map < struct cid, Circuit *, cic_hastr, cic_eqstr > circuits;
	TrunkGroup& trunkGroup;		// trunk group to which we belong
	struct cid cid;			// circuit identifier
	list<Call *> calls;		// list of calls
	enum CircuitState state;	// circuit state
	enum CircuitDirection direction;
	enum CircuitSelection selection;
	static Circuit & get(struct cid &id) {
		Circuit *c;
		hash_map < struct cid, Circuit *, cic_hastr, cic_eqstr >::iterator cic;
		if ((cic = circuits.find(cid)) == circuits.end) {
			TrunkGroup & tg = TrunkGroup::get(cid.opc, cid.dpc);
			if ((c = new Circuit(tg)) == NULL)
				throw Error(1);
		} else
			c = circuits[cid];
		return *c;
	};
	void process(Message & msg);

      public:
	Circuit(TrunkGroup & tg, int code)
		: trunkGroup(tg), cid(tg.pointCodeA(), tg.pointCodeB(), code)
	{
		circuits[cid] = this;
		tg.add(*this);
	};
	~Circuit(void) {
		tg.del(*this);
		circuits.erase(cid);
	};
	static void add(Message & msg) {
		struct cid cid = { msg->opc, msg->dpc, cic };
		get(cid).process(msg);
	};
};

/** process circuit-related message
  * @param msg a reference to the received message
  *
  * This method is the primary method for correlating circuit-related ISUP
  * messaging.  It uses the methods of the former isupcre program.
  */
void
Circuit::process(Message& msg)
{
}


//  =========================================================================
//
//  DETAILED METHODS
//
//  =========================================================================
//  These are collected here simply for readability.  These functions are
//  volumous.

// Check whether a message that is too large to definitely be an ITUT message
// (i.e. >= 10 SIF octets) is possibly an ITUT message.
bool
Message::reasonableItutIsupMsg(void)
{
	// mid must point to the beginning of the routing label
	size_t size = end - mid; // length of SIF field (excluding SIO)

	// need 7 bytes just to have a RL+CIC+MT
	if (size < 7)
		return (false);

	switch (mid[6]) {
	case ISUP_MT_CCR:	// ITUT: (none)
	case ISUP_MT_RSC:	// ITUT: (none)
	case ISUP_MT_BLO:	// ITUT: (none)
	case ISUP_MT_UBL:	// ITUT: (none)
	case ISUP_MT_BLA:	// ITUT: (none)
	case ISUP_MT_UBA:	// ITUT: (none)
	case ISUP_MT_LPA:	// ITUT: (none)
	case ISUP_MT_UCIC:	// ITUT: (none)
	case ISUP_MT_OLM:	// ITUT: (none)
	case ISUP_MT_CRG:	// ITUT: (none)
		// These all have no parameters whatsoever so cannot be longer than the
		// message type. RL(4)+CIC(2)+MT(1) == 7.
		if (size != 7)
			return (false);
		return (true);
	case ISUP_MT_COT:	// 0b00000101 - Continuity
		// These all have a single octet fixed parameter and no optional
		// parameters, so cannot be longer than one plus the message type.
		// RL(4)+CIC(2)+MT(1)+MFP(1) == 8.
		if (size != 8)
			return (false);
		return (true);
	case ISUP_MT_RLC:	// ITUT: O() >= 8
	case ISUP_MT_DRS:	// ITUT: O() >= 8
	case ISUP_MT_FOT:	// ITUT: O() >= 8
	case ISUP_MT_ANM:	// ITUT: O() >= 8
	case ISUP_MT_UPT:	// ITUT: O() >= 8
	case ISUP_MT_FAC:	// ITUT: O() >= 8
	case ISUP_MT_UPA:	// ITUT: O() >= 8
	case ISUP_MT_IDR:	// ITUT: O() >= 8
	case ISUP_MT_IRS:	// ITUT: O() >= 8
	case ISUP_MT_SGM:	// ITUT: O() >= 8
	case ISUP_MT_NRM:	// ITUT: O() >= 8
		// These all have no mandatory parameters, but optional parameters, but
		// can be sent with no optional parameters.  When the optional
		// parameter list is empty (pointer is zero), these must be of a fixed
		// size: RL(4)+CIC(2)+MT(1)+EOP(0) == 8.
		if (size < 8)
			return (false);
		if (size > 8 && mid[7] == 0)
			return (false);
		return (true);
	case ISUP_MT_INR:	// 0b00000011 - Information request
	case ISUP_MT_INF:	// 0b00000100 - Information
	case ISUP_MT_ACM:	// 0b00000110 - Address complete
	case ISUP_MT_SUS:	// 0b00001101 - Suspend
	case ISUP_MT_RES:	// 0b00001110 - Resume
	case ISUP_MT_CMR:	// 0b00011100 - Call Modification Request (not ANSI)
	case ISUP_MT_CMC:	// 0b00011101 - Call Modification Completed (not ANSI)
	case ISUP_MT_CMRJ:	// 0b00011110 - Call Modification Reject (not ANSI)
	case ISUP_MT_FAR:	// 0b00011111 - Facility request (not ANSI)
	case ISUP_MT_FAA:	// 0b00100000 - Facility accepted (not ANSI)
	case ISUP_MT_CPG:	// 0b00101100 - Call progress
		// These have one-octet fixed parameter and optional parameter list.
		// These all have a one-octet fixed parameter and optional parameter list
		// and are often sent with no optional parameters.  When the optional
		// parameter list is empty (pointer is zero), these must be of a fixed
		// size: RL(4)+CIC(2)+MT(1)+MFP(1)+EOP(0) == 9.
		if (size < 9)
			return (false);
		if (size > 9 && mid[7] == 0)
			return (false);
		return (true);
	case ISUP_MT_IAM:	// ITUT: F(NCI(1) FCI(2) CPC(1) TMR(1)) V(CDPN(2+N(2min)) O() >= 17
		if (size < 17)
			return (false);
		return (true);
	case ISUP_MT_FRJ:	// ITUT: F(FACI(1)) V(CAUS(2+N(2min))) O() >= 13
	case ISUP_MT_CQR:	// ITUT: V(RS(2+N(1min)) CSI(2+N(1min))) >= 13
		if (size < 13)
			return (false);
		return (true);
	case ISUP_MT_SAM:	// ITUT: V(SUBN(2+N(2min))) O() >= 12
	case ISUP_MT_REL:	// ITUT: V(CAUS(2+N(2min))) O() >= 12
	case ISUP_MT_CFN:	// ITUT: V(CAUS(2+N(2min))) O() >= 12
		if (size < 12)
			return (false);
		return (true);
	case ISUP_MT_GRS:	// ITUT: V(RS(2+N(min1))) O() >= 11
	case ISUP_MT_USR:	// ITUT: V(UUI(2+N(1min))) O() >= 11
	case ISUP_MT_CGB:	// ITUT: F(CGI(1)) V(RS(2+N(1min))) >= 11
	case ISUP_MT_CGBA:	// ITUT: F(CGI(1)) V(RS(2+N(1min))) >= 11
	case ISUP_MT_CGU:	// ITUT: F(CGI(1)) V(RS(2+N(1min))) >= 11
	case ISUP_MT_CGUA:	// ITUT: F(CGI(1)) V(RS(2+N(1min))) >= 11
		if (size < 11)
			return (false);
		return (true);
	case ISUP_MT_CON:	// ITUT: F(BCI(2)) O() >= 10
		if (size > 10 && mid[9] == 0)
			return (false);
		/* fall through */
	case ISUP_MT_GRA:	// ITUT: V(RS(2+N(min1))) >= 10
	case ISUP_MT_CQM:	// ITUT: V(RS(2+N(min1))) >= 10
		if (size < 10)
			return (false);
		return (true);
	case ISUP_MT_PAM:	// 0b00101000 - Pass along
		// not really expected
		return (true);
	default:
	case ISUP_MT_FAD:	// (Bellcore only)
	case ISUP_MT_FAI:	// (Bellcore only)
	case ISUP_MT_CRA:	// (Bellcore only)
	case ISUP_MT_CRM:	// (Bellcore only)
	case ISUP_MT_CVR:	// (Bellcore only)
	case ISUP_MT_CVT:	// (Bellcore only)
	case ISUP_MT_EXM:	// (Bellcore only)
	case ISUP_MT_NON:	// (Spain)
	case ISUP_MT_LLM:	// (Spain)
	case ISUP_MT_CAK:	// (Singapore)
	case ISUP_MT_TCM:	// (Singapore)
	case ISUP_MT_MCP:	// (Singapore)
	case ISUP_MT_CSVQ:	// (Bellocore only)
	case ISUP_MT_CSVR:	// (Bellocore only)
		return (false);
	}
}

// Check whether a message that is large enough to be an ANSI message
// (i.e. >= 10 SIF octets) is possibly an ANSI message.
bool
Message::reasonableAnsiIsupMsg(void)
{
	// mid must point to the beginning of the routing label
	size_t size = end - mid;

	// need 10 bytes just to have a RL+CIC+MT
	if (size < 10)
		return (false);

	switch (mid[9]) {
	case ISUP_MT_RLC: // ANSI: (none) == 10
	case ISUP_MT_CCR: // ANSI: (none) == 10
	case ISUP_MT_RSC: // ANSI: (none) == 10
	case ISUP_MT_BLO: // ANSI: (none) == 10
	case ISUP_MT_UBL: // ANSI: (none) == 10
	case ISUP_MT_BLA: // ANSI: (none) == 10
	case ISUP_MT_UBA: // ANSI: (none) == 10
	case ISUP_MT_LPA: // ANSI: (none) == 10
	case ISUP_MT_UCIC: // ANSI: (none) == 10
	case ISUP_MT_CRA: // ANSI: (none) == 10
	case ISUP_MT_CVT: // ANSI: (none) == 10
		if (size != 10)
			return (false);
		return (true);
	case ISUP_MT_EXM: // ANSI: O() >= 11
		if (size < 11)
			return (false);
		if (size > 11 && mid[10] == 0)
			return (false);
		return (true);
	case ISUP_MT_COT: // ANSI: F(COTI(1)) >= 11
	case ISUP_MT_CRM: // ANSI: F(NCI(1)) >= 11
		if (size != 11)
			return (false);
		return (true;

	case ISUP_MT_IAM:
	case ISUP_MT_SAM:
	case ISUP_MT_INR:
	case ISUP_MT_INF:
	case ISUP_MT_ACM:
	case ISUP_MT_CON:
	case ISUP_MT_FOT:
	case ISUP_MT_ANM:
	case ISUP_MT_REL:
	case ISUP_MT_SUS:
	case ISUP_MT_RES:
	case ISUP_MT_GRS:
	case ISUP_MT_CGB:
	case ISUP_MT_CGU:
	case ISUP_MT_CGBA:
	case ISUP_MT_CGUA:
	case ISUP_MT_CMR:
	case ISUP_MT_CMC:
	case ISUP_MT_CMRJ:
	case ISUP_MT_FAR:
	case ISUP_MT_FAA:
	case ISUP_MT_FRJ:
	case ISUP_MT_FAD:
	case ISUP_MT_FAI:
	case ISUP_MT_CSVQ:
	case ISUP_MT_CSVR:
	case ISUP_MT_DRS:
	case ISUP_MT_PAM:
	case ISUP_MT_GRA:
	case ISUP_MT_CQM:
	case ISUP_MT_CQR:
	case ISUP_MT_CPG:
	case ISUP_MT_USR:
	case ISUP_MT_CFN:
	case ISUP_MT_OLM:
	case ISUP_MT_CRG:
	case ISUP_MT_NRM:
	case ISUP_MT_FAC:
	case ISUP_MT_UPT:
	case ISUP_MT_UPA:
	case ISUP_MT_IDR:
	case ISUP_MT_IRS:
	case ISUP_MT_SGM:
	case ISUP_MT_CVR:
	case ISUP_MT_NON:
	case ISUP_MT_LLM:
	case ISUP_MT_CAK:
	case ISUP_MT_TCM:
	case ISUP_MT_MCP:
	default:
		return (false);
	}
}

int
Message::decode(Path & path)
{
	if (phdr.xsn)
		// pseudo-header says we are annex a
		path.setLinkHeader(LH_ANNEXA);
	// basic FISU/LSSU is 3,4
	// basic ITUT is 3 + SIF(1) + RL(4) + H1H0(1) = 9 min
	// basic ANSI is 3 + SIF(1) + RL(7) + H1H0(1) + SLC(1) = 13 min
	// annex FISU/LSSU is 6,7
	// annex ITUT is 6 + SIF(1) + RL(4) + H1H0(1) = 12 min
	// annex ANSI is 6 + SIF(1) + RL(7) + H1H0(1) + SLC(1) = 16 min
	switch (msg.hdr.len) {
	case 0:
	case 1:
	case 2:
		// a bad message
		return (-1);
	case 3:		// basic FISU
	case 4:		// basic LSSU
	case 5:		// basic LSSU2
		path.setLinkHeader(LH_BASIC);
		break;
	case 6:		// annex FISU
	case 7:		// annex LSSU
	case 8:		// annex LSSU2
		path.setLinkHeader(LH_ANNEXA);
		break;
	case 9:		// basic ITUT min MSU
	case 10:		// basic ITUT min MSU + 1
	case 11:		// basic ITUT min MSU + 2
		path.setLinkHeader(LH_BASIC);
		path.setRoutingLabel(RL_14BIT_PC);
		break;
	case 12:		// basic ITUT min MSU + 3 or annex ITUT min MSU
		path.setRoutingLabel(RL_14BIT_PC);
		break;
	default:
		// don't know really
		break;
	}
	if (msg.hdr.len < 5)
		// only basic sequence numbers can be this small
		path.setLinkHeader(LH_BASIC);
	if (5 < msg.hdr.len && msg.hdr.len < 8)
		// only annex A sequence numbers can be this size
		path.setLinkHeader(LH_ANNEXA);
	end = beg + hdr.len;
	mid = beg;
	switch (path.getLinkHeader()) {
	case LH_UNKNOWN:
		// cannot decode message further
		return (0);
	case LH_BASIC:
		// basic link header
		bsn = *mid & 0x7f;
		bib = (*mid++ & 0x80) >> 7;
		fsn = *mid & 0x7f;
		bib = (*mid++ & 0x80) >> 7;
		li = *mid & 0x3f;
		if ((li != hdr.len - 6) && (hdr.len <= 63 + 3 || li == 63))
			fprintf(stderr, "bad length indicator %d != %d\n", li, hdr.len - 3);
		mp = (*mid++ & 0xc0) >> 6;
		if (mp != 0) {
			path.setRoutingLabel(RL_24BIT_PC);
			path.setMessagePriority(MP_JAPAN);
		}
		break;
	case LH_ANNEXA:
		// annex a link header
		bsn = (*mid++ & 0xff) | ((*mid & 0x0f) << 8);
		bib = (*mid++ & 0x80) >> 7;
		fsn = (*mid++ & 0xff) | ((*mid & 0x0f) << 8);
		fib = (*mid++ & 0x80) >> 7;
		li = (*mid++ & 0xff) | ((*mid & 0x01) << 8);
		if ((li != hdr.len - 6) && (hdr.len <= 63 + 6 || li == 63))
			fprintf(stderr, "bad length indicator %d != %d\n", li, hdr.len - 6);
		mp = (*mid++ & 0xc0) >> 6;
		if (mp != 0) {
			path.setRoutingLabel(RL_24BIT_PC);
			path.setMessagePriority(MP_JAPAN);
		}
		break;
	}
	if (li < 3)
		return (1);	// done this message
	si = *mid & 0x0f;
	ni = (*mid & 0xc0) >> 6;
	if (ni == 0) {
		path.setRoutingLabel(RL_14BIT_PC);
		path.setMessagePriority(MP_INTERNATIONAL);
	}
	switch (path.getMessagePriority()) {
	case MP_UNKNOWN:
		mp = (*mid & 0x30) >> 4;
		break;
	case MP_JAPAN:
		break;
	case MP_NATIONAL:
		mp = (*mid & 0x30) >> 4;
		break;
	case MP_INTERNATIONAL:
		mp = 0;
		break;
	}
	mid++;
	if (li < 6) {
		fprintf(stderr, "too short for RL, li = %d", li);
		return (-1);	// bad message length
	}
	// do routing label detection
	// ANSI min ISUP size is SIF(1) + RL(7) + CIC(2) + MT(1) == 11
	// ANSI min SNMM size is SIF(1) + RL(7) + H0H1(1) + SLC(1) == 10
	// ITUT min ISUP size is SIF(1) + RL(4) + CIC(2) + MT(1) == 8
	// ITUT min SNMM size is SIF(1) + RL(4) + H0H1(1) == 6
	if (li < 10)
		path.setRoutingLabel(RL_14BIT_PC);
	else if (si == 0x05 && li < 11)
		path.setRoutingLable(RL_14BIT_PC);
	if (path.getRoutingLabel() == RL_UNKNOWN) {
		size_t size = mid - beg;

		// need to detect based on si
		// there are four types of messages that we can detect with,
		// snmm, sntm, ssntm and isup.  The most prevalent of these
		// that are usable should be ISUP COT and SLTM/SLTA.
		switch (si) {
		case 0:	// snmm (all lengths after the H0H1 octet)
			// com messages 4+1+1=6 byte ITUT 7+1+2=10 byte ANSI
			// cbm messages 4+1+1=6 byte ITUT 7+1+2=10 byte ANSI
			// trx messages 4+1+0=5 byte ITUT 7+1+0= 8 byte ANSI
			// tfc messages 4+1+2=7 byte ITUT 7+1+4=12 byte ANSI
			// tfm messages 4+1+2=7 byte ITUT 7+1+3=11 byte ANSI
			// dlc messages 4+1+2=7 byte ITUT 7+1+3=11 byte ANSI
			// slm messages 4+1+0=5 byte ITUT 7+1+1= 9 byte ANSI
			// upm messages 4+1+3=8 byte ITUT 7+1+4=12 byte ANSI
			// 5-7 is ITUT, 9-12 is ANSI, 8 is either (upm ITU, trx ANSI)
			switch (size) {
			case 5:	// trx or slm message
				switch (mid[4]) {	// ITU H0H1 byte
				case 0x17:	// tra
				case 0x27:	// trw
				case 0x13:	// rct
				case 0x28:	// css
				case 0x38:	// cns
				case 0x48:	// cnp
				case 0x12:	// eco
				case 0x22:	// eca
				case 0x16:	// lin
				case 0x26:	// lun
				case 0x36:	// lia
				case 0x46:	// lua
				case 0x56:	// lid
				case 0x66:	// lfu
				case 0x76:	// llt
				case 0x86:	// lrt
					path.setRoutingLabel(RL_14BIT_PC);
					break;
				default:
					return (-1);	// decoding error
				}
				break;
			case 6:	// com or cbm message
				switch (mid[4]) {	// ITU H0H1 byte
				case 0x11:	// coo
				case 0x21:	// coa
				case 0x51:	// cbd
				case 0x61:	// cba
					path.setRoutingLabel(RL_14BIT_PC);
					break;
				default:
					return (-1);	// decoding error
				}
				break;
			case 7:	// tfc or tfm or dlc message
				switch (mid[4]) {
				case 0x23:	// tfc
				case 0x14:	// tfp
				case 0x24:	// tcp
				case 0x34:	// tfr
				case 0x44:	// tcr
				case 0x54:	// tfa
				case 0x64:	// tca
				case 0x15:	// rst
				case 0x25:	// rsr
				case 0x35:	// rcp
				case 0x45:	// rcr
					path.setRoutingLabel(RL_14BIT_PC);
					break;
				default:
					return (-1);	// decoding error
				}
				break;
			case 8:
				switch (mid[4]) {	// ITU H0H1 byte
				case 0x1a:	// upu
				case 0x2a:	// upa
				case 0x3a:	// upt
					switch (mid[7]) {
					case 0x17:	// tra
					case 0x27:	// trw
					case 0x13:	// rct
						return (0);	// can't decide
					default:
						path.setRoutingLabel(RL_14BIT_PC);
						break;
					}
					break;
				default:
					switch (mid[7]) {
					case 0x17:	// tra
					case 0x27:	// trw
					case 0x13:	// rct
						path.setRoutingLabel(RL_24BIT_PC);
						break;
					default:
						return (0);	// can't decide
					}
					break;
				}
				break;
			case 9:	// slm message
				switch (mid[7]) {
				case 0x28:	// css
				case 0x38:	// cns
				case 0x48:	// cnp
				case 0x12:	// eco
				case 0x22:	// eca
				case 0x16:	// lin
				case 0x26:	// lun
				case 0x36:	// lia
				case 0x46:	// lua
				case 0x56:	// lid
				case 0x66:	// lfu
				case 0x76:	// llt
				case 0x86:	// lrt
					path.setRoutingLabel(RL_24BIT_PC);
					break;
				default:
					return (-1);	// decoding error
				}
				break;
			case 10:	// com or cbm message
				switch (mid[7]) {
				case 0x11:	// coo
				case 0x21:	// coa
				case 0x51:	// cbd
				case 0x61:	// cba
					path.setRoutingLabel(RL_24BIT_PC);
					break;
				default:
					return (-1);	// decoding error
				}
				break;
			case 11:	// tfm or dlc message
				switch (mid[7]) {
				case 0x14:	// tfp
				case 0x24:	// tcp
				case 0x34:	// tfr
				case 0x44:	// tcr
				case 0x54:	// tfa
				case 0x64:	// tca
				case 0x15:	// rst
				case 0x25:	// rsr
				case 0x35:	// rcp
				case 0x45:	// rcr
				case 0x18:	// dlc
					path.setRoutingLabel(RL_24BIT_PC);
					break;
				default:
					return (-1);	// decoding error
				}
				break;
			case 12:	// tfc or upm message
				switch (mid[7]) {
				case 0x23:	// tfc
				case 0x1a:	// upu
				case 0x2a:	// upa
				case 0x3a:	// upt
					path.setRoutingLabel(RL_24BIT_PC);
					break;
				default:
					return (-1);	// decoding error
				}
				break;
			}
			break;
		case 1:	// sntm
		case 2:	// snsm
			// sltm message 4+1+1+n=6+n byte ITUT 7+1+1+n=9+n byte
			// ITUT 6-21 ANSI 9-24
			// 6-8 is ITUT, 22-24 is ANSI, 9-21 is either
			switch (size) {
			case 6:
			case 7:
			case 8:
				if ((mid[4] == 0x11 || mid[4] == 0x21) && ((mid[5] >> 4) == size)) {
					path.setRoutingLabel(RL_14BIT_PC);
					break;
				}
				return (-1);	// decoding error
			case 22:
			case 23:
			case 24:
				if ((mid[7] == 0x11 || mid[7] == 0x21) && ((mid[8] >> 4) == size - 9)) {
					path.setRoutingLabel(RL_24BIT_PC);
					break;
				}
				return (-1);	// decoding error
			default:
				if (size < 9 || size > 21)
					return (-1);	// decoding error
				if ((mid[4] == 0x11 || mid[4] == 0x21) && ((mid[5] >> 4) == size)) {
					if ((mid[7] == 0x11 || mid[7] == 0x21) && ((mid[8] >> 4) == size - 9)) {
						return (0);	// can't decide
					}
					path.setRoutingLabel(RL_14BIT_PC);
					break;
				} else {
					if ((mid[7] == 0x11 || mid[7] == 0x21) && ((mid[8] >> 4) == size - 9)) {
						path.setRoutingLabel(RL_24BIT_PC);
						break;
					}
				}
				return (-1);	// decoding error
			}
			break;
		case 5:	// isup
			// cot ITUT RL(4)+CIC(2)+MT(1)+COTI(1) == 8
			// cot ANSI RL(7)+CIC(2)+MT(1)+COTI(1) == 11
			// inr! ITUT RL(4)+CIC(2)+MT(1)+INFI(1)+EOP(1) == 9
			// inr! ANSI RL(7)+CIC(2)+MT(1)+INFI(1)+EOP(1) == 12
			// inf! ITUT RL(4)+CIC(2)+MT(1)+INFI(1)+EOP(1) == 9
			// inf! ANSI RL(7)+CIC(2)+MT(1)+INFI(1)+EOP(1) == 12
			// acm! ITUT RL(4)+CIC(2)+MT(1)+BCI(1)+EOP(1) == 9
			// acm! ANSI RL(7)+CIC(2)+MT(1)+BCI(1)+EOP(1) == 12
			// sus! ITUT RL(4)+CIC(2)+MT(1)+SRIS(1)+EOP(1) == 9
			// sus! ANSI RL(7)+CIC(2)+MT(1)+SRIS(1)+EOP(1) == 12
			// res! ITUT RL(4)+CIC(2)+MT(1)+SRIS(1)+EOP(1) == 9
			// res! ANSI RL(7)+CIC(2)+MT(1)+SRIS(1)+EOP(1) == 12
			// rlc! ITUT RL(4)+CIC(2)+MT(1)+EOP(1) == 8
			// rlc ANSI RL(7)+CIC(2)+MT(1) == 10
			// ccr ITUT RL(4)+CIC(2)+MT(1) == 7
			// ccr ANSI RL(7)+CIC(2)+MT(1) == 10
			// rsc ITUT RL(4)+CIC(2)+MT(1) == 7
			// rsc ANSI RL(7)+CIC(2)+MT(1) == 10
			// blo ITUT RL(4)+CIC(2)+MT(1) == 7
			// blo ANSI RL(7)+CIC(2)+MT(1) == 10
			// ubl ITUT RL(4)+CIC(2)+MT(1) == 7
			// ubl ANSI RL(7)+CIC(2)+MT(1) == 10
			// bla ITUT RL(4)+CIC(2)+MT(1) == 7
			// bla ANSI RL(7)+CIC(2)+MT(1) == 10
			// uba ITUT RL(4)+CIC(2)+MT(1) == 7
			// uba ANSI RL(7)+CIC(2)+MT(1) == 10
			// cmr! ITUT RL(4)+CIC(2)+MT(1)+CMI(1)+EOP(0) == 9
			// cmc! ITUT RL(4)+CIC(2)+MT(1)+CMI(1)+EOP(0) == 9
			// cmrj!ITUT RL(4)+CIC(2)+MT(1)+CMI(1)+EOP(0) == 9
			// far! ITUT RL(4)+CIC(2)+MT(1)+FACI(1)+EOP(1) == 9
			// far! ANSI RL(7)+CIC(2)+MT(1)+FACI(1)+EOP(1) == 12
			// faa! ITUT RL(4)+CIC(2)+MT(1)+FACI(1)+EOP(1) == 9
			// faa! ANSI RL(7)+CIC(2)+MT(1)+FACI(1)+EOP(1) == 12
			// lpa ITUT RL(4)+CIC(2)+MT(1) == 7
			// lpa ANSI RL(7)+CIC(2)+MT(1) == 10
			// drs! ITUT RL(4)+CIC(2)+MT(1)+EOP(1) == 8
			// cpg! ITUT RL(4)+CIC(2)+MT(1)+EVNT(1)+EOP(1) == 9
			// cpg! ANSI RL(7)+CIC(2)+MT(1)+EVNT(1)+EOP(1) == 12
			// ucic ITUT RL(4)+CIC(2)+MT(1) == 7
			// ucic ANSI RL(7)+CIC(2)+MT(1) == 10
			// olm ITUT RL(4)+CIC(2)+MT(1) == 7
			// crg ITUT RL(4)+CIC(2)+MT(1) == 7
			// cra ANSI RL(7)+CIC(2)+MT(1) == 10
			// crm ANSI RL(7)+CIC(2)+MT(1)+NCI(1) == 11
			// cvt ANSI RL(7)+CIC(2)+MT(1) == 10
			// exm! ANSI RL(7)+CIC(2)+MT(1)+EOP(1) == 11
			switch (size) {
			case 7:	// ccr, rsc, blo, ubl, bla, uba, lpa, ucic, olm, crg
				switch (mid[6]) {
				case ISUP_MT_CCR:
				case ISUP_MT_RSC:
				case ISUP_MT_BLO:
				case ISUP_MT_UBL:
				case ISUP_MT_BLA:
				case ISUP_MT_UBA:
				case ISUP_MT_LPA:
				case ISUP_MT_UCIC:
				case ISUP_MT_OLM:
				case ISUP_MT_CRG:
					break;
				default:
					// FIXME complain
					break;
				}
				// These are obviously ITU based on size alone:
				// ANSI messages simply cannot be this short.
				path.setRoutingLabel(RL_14BIT_PC);
				break;
			case 8:	// cot, rlc!, drs!
				switch (mid[6]) {
				case ISUP_MT_COT:
					break;
				case ISUP_MT_RLC:
				case ISUP_MT_DRS:
					if (end[-1] != 0)
						return (-1);	// decoding error
					break;
				default:
					// FIXME complain
					break;
				}
				// These are obviously ITU based on size alone:
				// ANSI messages simply cannot be this short.
				path.setRoutingLabel(RL_14BIT_PC);
				break;
			case 9:	// inr!, inf!, acm!, sus!, res!, cmr!, cmrj!, far!, faa!,
					// cpg!
				if (end[-1] != 0)
					return (-1);	// deconding error
				switch (mid[6]) {
				case ISUP_MT_INR:
				case ISUP_MT_INF:
				case ISUP_MT_ACM:
				case ISUP_MT_SUS:
				case ISUP_MT_RES:
				case ISUP_MT_CMR:
				case ISUP_MT_CMC:
				case ISUP_MT_CMRJ:
				case ISUP_MT_FAR:
				case ISUP_MT_FAA:
				case ISUP_MT_CPG:
					break;
				default:
					// FIXME complain
					break;
				}
				// These are obviously ITU based on size alone:
				// ANSI messages simply cannot be this short.
				path.setRoutingLabel(RL_14BIT_PC);
				break;

			case 10:	// rlc, ccr, rsc, blo, ubl, bla, uba, lpa, ucic, cra, cvt
				switch (mid[9]) {
				case ISUP_MT_RLC:
				case ISUP_MT_CCR:
				case ISUP_MT_RSC:
				case ISUP_MT_BLO:
				case ISUP_MT_UBL:
				case ISUP_MT_BLA:
				case ISUP_MT_LPA:
				case ISUP_MT_UCIC:
				case ISUP_MT_CRA:
				case ISUP_MT_CVT:
					// Only really certain when the 14-bit
					// PC message type is unreasonable.
					if (!(mid[6] & 0xc0)) {
						switch (mid[6]) {
						case ISUP_MT_CCR:
						case ISUP_MT_RSC:
						case ISUP_MT_BLO:
						case ISUP_MT_UBL:
						case ISUP_MT_BLA:
						case ISUP_MT_UBA:
						case ISUP_MT_LPA:
						case ISUP_MT_UCIC:
						case ISUP_MT_OLM:
						case ISUP_MT_CRG:
						case ISUP_MT_COT:
							path.setRoutingLabel(RL_24BIT_PC);
							break;
						default:
							break;
						}
					} else
						path.setRoutingLabel(RL_24BIT_PC);
					break;
				default:
					break;	// can't decide
				}
				break;
			case 11:	// cot, crm, exm!
				switch (mid[9]) {
				case ISUP_MT_COT:
				case ISUP_MT_CRM:
					path.setRoutingLabel(RL_24BIT_PC);
					break;
				case ISUP_MT_EXM:
					if (end[-1] != 0)
						break;	// can't decide
					path.setRoutingLabel(RL_24BIT_PC);
					break;
				default:
					break;	// can't decide
				}
				path.setRoutingLabel(RL_24BIT_PC);
				break;
			case 12:	// inr!, inf!, acm!, sus!, res!, far!, faa!, cpg!
				if (end[-1] != 0)
					return (0);	// can't decide
				switch (mid[9]) {
				case ISUP_MT_INR:
				case ISUP_MT_INF:
				case ISUP_MT_ACM:
				case ISUP_MT_SUS:
				case ISUP_MT_RES:
				case ISUP_MT_FAR:
				case ISUP_MT_FAA:
				case ISUP_MT_CPG:
					path.setRoutingLabel(RL_24BIT_PC);
					break;
				default:
					break;	// can't decide
				}
				break;
			default:
				if (size < 7)
					return (-1);	// decode error
				break;	// can't decide
			}
			break;
		default:
			break;
		}
	}
	switch (path.getRoutingLabel()) {
	case RL_UNKNOWN:
		return (0);	// cannot decode further, yet
	case RL_14BIT_PC:
		// SSSSOOOO OOOOOOOO OODDDDDD DDDDDDDD
		if (li < 6)
			fprintf(stderr, "too short for RL, li = %d\n", li);
		dpc = (*mid++ & 0xff) | ((*mid & 0x3f) << 8);
		opc = ((*mid++ & 0xc0) >> 6) | ((*mid++ & 0xff) << 2) | ((*mid & 0x0f) << 10);
		sls = (*mid++ * 0xf0) >> 4;
		break;
	case RL_24BIT_PC:
		if (li < 9)
			fprintf(stderr, "too short for RL, li = %d\n", li);
		dpc = (*mid++ & 0x0ff) | ((*mid++ & 0xff) << 8) | ((*mid++ & 0xff) << 16);
		opc = (*mid++ & 0x0ff) | ((*mid++ & 0xff) << 8) | ((*mid++ & 0xff) << 16);
		sls = *mid++ & 0xff;
		break;
	}
	// discover links and point codes
	switch (si) {
	case 0:		// snmm
		h0 = (*mid & 0x0f);
		h1 = (*mid++ & 0xf0) >> 4;
		switch ((h0 << 4) | h1) {
		case 0x11:	// coo (link)
		case 0x12:	// coa (link)
			return (-1);
		case 0x15:	// cbd
			// link specific
			if (path.getRoutingLabel() == RL_14BIT_PC)
				slc = sls;
			else
				slc = (*mid++ & 0xf);
			mid++;
			path.setLink(*this);
			break;
		case 0x16:	// cba
		case 0x21:	// eco (link)
		case 0x22:	// eca (link)
		case 0x31:	// rct (route-set)
		case 0x32:	// tfc (route-set)
		case 0x41:	// tfp (route)
		case 0x42:	// tcp (route)
		case 0x43:	// tfr (route)
		case 0x44:	// tcr (route)
		case 0x45:	// tfa (route)
		case 0x46:	// tca (route)
		case 0x51:	// rst (route-test)
		case 0x52:	// rsr (route-test)
		case 0x53:	// rcp (route-test)
		case 0x54:	// rcr (route-test)
			// side getting the message has the transfer function
			// opc is adjacent
			// dpc is remote
			break;
		case 0x61:	// lin (link)
		case 0x62:	// lun (link)
		case 0x63:	// lia (link)
		case 0x64:	// lua (link)
		case 0x65:	// lid (link)
		case 0x66:	// lfu (link)
		case 0x67:	// llt (link)
		case 0x68:	// lrt (link)
		case 0x71:	// tra (adjacent)
		case 0x72:	// trw (adjacent)
		case 0x81:	// dlc
		case 0x82:	// css
		case 0x83:	// cns
		case 0x84:	// cnp
		case 0xa1:	// upu (route-set)
		case 0xa2:	// upa (route-set)
		case 0xa3:	// upt (route-set)
		default:
			return (-1);
		}
		break;
	case 1:		// sntm
	case 2:		// snsm
		h0 = (*mid & 0x0f);
		h1 = (*mid++ & 0xf0) >> 4;
		// check for SLTM/SLTA SSLTM/SSLTA message
		switch ((h0 << 4) | h1) {
		case 0x12:	// (s)slta
			/* well.... */
		case 0x11:	// (s)sltm
			if (path.getRoutingLabel() == RL_14BIT_PC)
				slc = sls;
			else
				slc = (*mid & 0x0f);
			dlen = (*mid++ & 0xf0) >> 4;
			path.setLink(*this);
			break;
		}
		break;
	case 3:		// sccp
		mt = (*mid++ & 0xff);
		break;
	case 4:		// tup
		return (-1);	// unexpected (obsolete)
	case 5:		// isup
		cic = (*mid++ & 0xff) | ((*mid++ & 0xff) << 8);
		mt = (*mid++ & 0xff);
		break;
	case 6:		// dup1
	case 7:		// dup2
		return (-1);	// unexpected (obsolete)
	case 8:		// mtup
		break;
	case 9:		// bisup
	case 10:		// siup
	case 11:		// spneup
	case 12:		// stc
	case 13:		// unused
	case 14:		// unused
	case 15:		// unused
		return (-1);	// unexpected
	}
	return (1);
}

//  =========================================================================
//
//  MAIN and OPTIONS HANDLING
//
//  =========================================================================

static void
copying(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
--------------------------------------------------------------------------------\n\
%1$s\n\
--------------------------------------------------------------------------------\n\
Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
--------------------------------------------------------------------------------\n\
This program is free software: you can  redistribute it  and/or modify  it under\n\
the terms of the  GNU Affero  General  Public  License  as published by the Free\n\
Software Foundation, version 3 of the license.\n\
\n\
This program is distributed in the hope that it will  be useful, but WITHOUT ANY\n\
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.\n\
\n\
You should have received a copy of the  GNU Affero General Public License  along\n\
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the\n\
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
--------------------------------------------------------------------------------\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the U.S. Government (\"Government\"), the following provisions apply to you. If\n\
the Software is supplied by the Department of Defense (\"DoD\"), it is classified\n\
as \"Commercial  Computer  Software\"  under  paragraph  252.227-7014  of the  DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the  license rights granted\n\
herein (the license rights customarily provided to non-Government users). If the\n\
Software is supplied to any unit or agency of the Government  other than DoD, it\n\
is  classified as  \"Restricted Computer Software\" and the Government's rights in\n\
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition\n\
Regulations (\"FAR\")  (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor\n\
regulations).\n\
--------------------------------------------------------------------------------\n\
Commercial  licensing  and  support of this  software is  available from OpenSS7\n\
Corporation at a fee.  See http://www.openss7.com/\n\
--------------------------------------------------------------------------------\n\
", ident);
}

static void
version(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stderr, "\
%1$s (OpenSS7 %2$s) %3$s (%4$s)\n\
Written by Brian Bidulock.\n\
\n\
Copyright (c) 2008, 2009, 2010, 2011  Monavacon Limited.\n\
Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008  OpenSS7 Corporation.\n\
Copyright (c) 1997, 1998, 1999, 2000, 2001  Brian F. G. Bidulock.\n\
This is free software; see the source for copying conditions.  There is NO\n\
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\
\n\
Distributed by OpenSS7 under GNU Affero General Public License Version 3,\n\
with conditions, incorporated herein by reference.\n\
\n\
See `%1$s --copying' for copying permissions.\n\
", NAME, PACKAGE, VERSION, "$Revision$ $Date$");
}

static void
usage(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stderr, "\
Usage:\n\
    %1$s [options] [{-o|--outfile} outfile] [{-p|--parfile} parfile] files [...]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
", argv[0]);
}

static void
help(int argc, char *argv[])
{
	if (!output && !debug)
		return;
	(void) fprintf(stdout, "\
Usage:\n\
    %1$s [options] [{-o|--outfile} outfile] [{-p|--parfile} parfile] files [...]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    files [...]\n\
        one or more files to use for input\n\
Options:\n\
  File Options:\n\
    -o, --outfile outfile             (default: %2$s)\n\
        output file for correlation result\n\
    -p, --parfile parfile             (default: %3$s)\n\
        output file for partial records\n\
  General Options:\n\
    -q, --quiet                       (default: off)\n\
        suppress normal output\n\
    -D, --debug [LEVEL]               (default: %4$d)\n\
        increment or set debug LEVEL\n\
    -v, --verbose [LEVEL]             (default: %5$d)\n\
        increase or set verbosity LEVEL\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        prints version and exits\n\
    -C, --copying\n\
        print copying permission and exit\n\
", argv[0], outfile, parfile, debug, output);
}

int
main(int argc, char **argv)
{
	int
	    c,
	    val;

	while (1) {
#if defined _GNU_SOURCE
		int
		    option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
		    {"outfile", required_argument,  NULL,   'o'},
		    {"parfile", required_argument,  NULL,   'p'},
		    {"quiet",   no_argument,        NULL,   'q'},
		    {"debug",   optional_argument,  NULL,   'D'},
		    {"verbose", optional_argument,  NULL,   'v'},
		    {"help",    no_argument,        NULL,   'h'},
		    {"version", no_argument,        NULL,   'V'},
		    {"copying", no_argument,        NULL,   'C'},
		    {"?",       no_argument,        NULL,   'H'},
		    {NULL,      0,                  NULL,    0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "o:p:qD::v::hVC?W:", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "o:p:qDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'o':	// -o, --outfile OUTFILE
			if (debug)
				fprintf(stderr, "%s: setting outfile to %s\n", argv[0], optarg);
			strncpy(outfile, optarg, sizeof(outfile) - 1);
			break;
		case 'p':	// -p, --parfile PARFILE
			if (debug)
				fprintf(stderr, "%s: setting parfile to %s\n", argv[0], optarg);
			strncpy(parfile, optarg, sizeof(parfile) - 1);
			break;
		case 'q':	// -q, --quiet
			if (debug)
				fprintf(stderr, "%s: suppressing normal output\n", argv[0]);
			output = 0;
			debug = 0;
			break;
		case 'D':	// -D, --debug [LEVEL]
			if (debug)
				fprintf(stderr, "%s: increasing debug verbosity\n", argv[0]);
			if (optarg == NULL) {
				debug += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'v':	// -v, --verbose [LEVEL]
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				output += 1;
				break;
			}
			if ((val = strol(optarg, NULL, 0)) < 0)
				goto bad_option;
			output = val;
			break;
		case 'h':	// -h, --help
		case 'H':	// -H, --?
			if (debug)
				fprintf(stderr, "%s: printing help message\n", argv[0]);
			help(argc, argv);
			exit(0);
		case 'V':	// -V, --version
			if (debug)
				fprintf(stderr, "%s: printing version message\n", argv[0]);
			version(argc, argv);
			exit(0);
		case 'C':	// -C, --copying
			if (debug)
				fprintf(stderr, "%s: printing copying message\n", argv[0]);
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
			goto bad_nonopt;
		      bad_nonopt:
			if (output || debug) {
				if (optind < argc) {
					fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
					while (optind < argc)
						fprintf(stderr, "%s ", argv[optind++]);
				} else {
					fprintf(stderr, "%s: missing option or argument", argv[0]);
				}
				fprintf(stderr, "\n");
				fflush(stderr);
				goto bad_usage;
			      bad_usage:
				usage(argc, argv);
			}
			exit(2);
		}
	}
	ss7anal(argc - optind, &argv[optind]);
	exit(4);
}
