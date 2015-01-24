// ==========================================================================
// 
// @(#) src/util/isupcre.cpp
// 
// --------------------------------------------------------------------------
// 
// Copyright (C) 2001-2003 OpenSS7 Corporation <http://www.openss7.com>
// 
// All Rights Reserved.
// 
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 of the License, or (at your option)
// any later version.
// 
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
// 
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// --------------------------------------------------------------------------
// 
// U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
// behalf of the U.S. Government ("Government"), the following provisions
// apply to you.  If the Software is supplied by the Department of Defense
// ("DoD"), it is classified as "Commercial Computer Software" under
// paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
// Regulations ("DFARS") (or any successor regulations) and the Government is
// acquiring only the license rights granted herein (the license rights
// customarily provided to non-Government users).  If the Software is
// supplied to any unit or agency of the Government other than DoD, it is
// classified as "Restricted Computer Software" and the Government's rights
// in the Software are defined in paragraph 52.227-19 of the Federal
// Acquisition Regulations ("FAR") (or any success regulations) or, in the
// cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
// (or any successor regulations).
// 
// --------------------------------------------------------------------------
// 
// Commercial licensing and support of this software is available from
// OpenSS7 Corporation at a fee.  See http://www.openss7.com/
// 
// ==========================================================================

static char const ident[] = "src/util/isupcre.cpp (" PACKAGE_ENVR ") " PACKAGE_DATE;

extern "C" {
#	include <stropts.h>
#	include <stdlib.h>
#	include <unistd.h>
#	include <stdio.h>
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

int output = 1;
int debug = 0;

char outfile[256] = "/dev/stdout";
char parfile[256] = "/dev/null";

int nfiles = 0;

FILE *files[FOPEN_MAX] = { NULL, };

FILE *stdlog = NULL;

class Error {
      public:
	int error;
	 Error(int val) {
		error = val;
	};
	~Error(void) {
	};
};

unsigned char *ss7readline(FILE * f, struct timeval *tv, unsigned char *buf, size_t max);

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
	ISUP_MT_UBL	= 0x14,		// 0b00010100 - Unblcoking
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

void dump_oparms(unsigned char *p, unsigned char *e)
{
	unsigned char pt;
	unsigned int val;
	size_t i, len, odd;
	for (;;) {
		if (p >= e)
			return;
		switch ((pt = *p++)) {
		case ISUP_PT_EOP: return;
		case ISUP_PT_CLRF:	fprintf(stdout, ", CLRF: 0x");	break;
		case ISUP_PT_TMR:	fprintf(stdout, ", TMR: 0x");	break;
		case ISUP_PT_ATP:	fprintf(stdout, ", ATP: 0x");	break;
		case ISUP_PT_CDPN:	fprintf(stdout, ", CDPN: 0x");
			len = *p++ & 0xff;
			odd = (*p & 0x80) >> 7;
			for (i = 0; i < 2; i++)
				fprintf(stdout, "%02x", *p++);
			fprintf(stdout, " ");
			for (i = 0; i < (len - 2) * 2 - odd; i++) {
				if (i & 0x1) {
					val = *p++;
					val >>= 4;
					val &= 0x0f;
				} else {
					val = *p;
					val &= 0x0f;
				}
				if (val < 10)
					fprintf(stdout, "%c", val + '0');
				else
					fprintf(stdout, "%c", val - 10 + 'a');
			}
			if (odd)
				p++;
			break;
		case ISUP_PT_SUBN:	fprintf(stdout, ", SUBN: 0x");	break;
		case ISUP_PT_NCI:	fprintf(stdout, ", NCI: 0x");	break;
		case ISUP_PT_FCI:	fprintf(stdout, ", FCI: 0x");	break;
		case ISUP_PT_OFCI:	fprintf(stdout, ", OFCI: 0x");	break;
		case ISUP_PT_CPC:	fprintf(stdout, ", CPC: 0x");	break;
		case ISUP_PT_CGPN:	fprintf(stdout, ", CGPN: 0x");
			len = *p++ & 0xff;
			odd = (*p & 0x80) >> 7;
			for (i = 0; i < 2; i++)
				fprintf(stdout, "%02x", *p++);
			fprintf(stdout, " ");
			for (i = 0; i < (len - 2) * 2 - odd; i++) {
				if (i & 0x1) {
					val = *p++;
					val >>= 4;
					val &= 0x0f;
				} else {
					val = *p;
					val &= 0x0f;
				}
				if (val < 10)
					fprintf(stdout, "%c", val + '0');
				else
					fprintf(stdout, "%c", val - 10 + 'a');
			}
			if (odd)
				p++;
			continue;
		case ISUP_PT_RDGN:	fprintf(stdout, ", RDGN: 0x");	break;
		case ISUP_PT_RDNN:	fprintf(stdout, ", RDNN: 0x");	break;
		case ISUP_PT_CONR:	fprintf(stdout, ", CONR: 0x");	break;
		case ISUP_PT_INRI:	fprintf(stdout, ", INRI: 0x");	break;
		case ISUP_PT_INFI:	fprintf(stdout, ", INFI: 0x");	break;
		case ISUP_PT_COTI:	fprintf(stdout, ", COTI: 0x");	break;
		case ISUP_PT_BCI:	fprintf(stdout, ", BCI: 0x");	break;
		case ISUP_PT_CAUS:	fprintf(stdout, ", CAUS: 0x");	break;
		case ISUP_PT_RDI:	fprintf(stdout, ", RDI: 0x");	break;
		case ISUP_PT_CGI:	fprintf(stdout, ", CGI: 0x");	break;
		case ISUP_PT_RS:	fprintf(stdout, ", RS: 0x");	break;
		case ISUP_PT_CMI:	fprintf(stdout, ", CMI: 0x");	break;
		case ISUP_PT_FACI:	fprintf(stdout, ", FACI: 0x");	break;
		case ISUP_PT_FAII:	fprintf(stdout, ", FAII: 0x");	break;
		case ISUP_PT_CUGI:	fprintf(stdout, ", CUGI: 0x");	break;
		case ISUP_PT_INDEX:	fprintf(stdout, ", INDEX: 0x");	break;
		case ISUP_PT_USI:	fprintf(stdout, ", USI: 0x");	break;
		case ISUP_PT_SPC:	fprintf(stdout, ", SPC: 0x");	break;
		case ISUP_PT_UUI:	fprintf(stdout, ", UUI: 0x");	break;
		case ISUP_PT_CONN:	fprintf(stdout, ", CONN: 0x");	break;
		case ISUP_PT_SRIS:	fprintf(stdout, ", SRIS: 0x");	break;
		case ISUP_PT_TNS:	fprintf(stdout, ", TNS: 0x");	break;
		case ISUP_PT_EVNT:	fprintf(stdout, ", EVNT: 0x");	break;
		case ISUP_PT_CSI:	fprintf(stdout, ", CSI: 0x");	break;
		case ISUP_PT_ACL:	fprintf(stdout, ", ACL: 0x");	break;
		case ISUP_PT_OCDN:	fprintf(stdout, ", OCDN: 0x");	break;
		case ISUP_PT_OBCI:	fprintf(stdout, ", OBCI: 0x");	break;
		case ISUP_PT_UUIND:	fprintf(stdout, ", UUIND: 0x");	break;
		case ISUP_PT_ISPC:	fprintf(stdout, ", ISPC: 0x");	break;
		case ISUP_PT_GNOT:	fprintf(stdout, ", GNOT: 0x");	break;
		case ISUP_PT_CHI:	fprintf(stdout, ", CMI: 0x");	break;
		case ISUP_PT_ADI:	fprintf(stdout, ", ADI: 0x");	break;
		case ISUP_PT_NSF:	fprintf(stdout, ", NSF: 0x");	break;
		case ISUP_PT_USIP:	fprintf(stdout, ", USIP: 0x");	break;
		case ISUP_PT_PROP:	fprintf(stdout, ", PROP: 0x");	break;
		case ISUP_PT_ROPS:	fprintf(stdout, ", ROPS: 0x");	break;
		case ISUP_PT_SA:	fprintf(stdout, ", SA: 0x");	break;
		case ISUP_PT_UTI:	fprintf(stdout, ", UTI: 0x");	break;
		case ISUP_PT_TMU:	fprintf(stdout, ", TMU: 0x");	break;
		case ISUP_PT_CDI:	fprintf(stdout, ", CDI: 0x");	break;
		case ISUP_PT_ECI:	fprintf(stdout, ", ECI: 0x");	break;
		case ISUP_PT_MCI:	fprintf(stdout, ", MCI: 0x");	break;
		case ISUP_PT_PCI:	fprintf(stdout, ", PCI: 0x");	break;
		case ISUP_PT_MLPP:	fprintf(stdout, ", MLPP: 0x");	break;
		case ISUP_PT_MCIQ:	fprintf(stdout, ", MCIQ: 0x");	break;
		case ISUP_PT_MCIR:	fprintf(stdout, ", MCIR: 0x");	break;
		case ISUP_PT_HOPC:	fprintf(stdout, ", HOPC: 0x");	break;
		case ISUP_PT_TMRP:	fprintf(stdout, ", TMRP: 0x");	break;
		case ISUP_PT_LN:	fprintf(stdout, ", LN: 0x");	break;
		case ISUP_PT_RDNR:	fprintf(stdout, ", RDNR: 0x");	break;
		case ISUP_PT_FREEP:	fprintf(stdout, ", FREEP: 0x");	break;
		case ISUP_PT_GREF:	fprintf(stdout, ", GREF: 0x");	break;
		case ISUP_PT_GNUM:	fprintf(stdout, ", GNUM: 0x");	break;
		case ISUP_PT_GDIG:	fprintf(stdout, ", GDIG: 0x");	break;
		case ISUP_PT_EGRESS:	fprintf(stdout, ", EGRESS: 0x"); break;
		case ISUP_PT_JUR:	fprintf(stdout, ", JUR: 0x");	break;
		case ISUP_PT_CIDC:	fprintf(stdout, ", CIDC: 0x");	break;
		case ISUP_PT_BGROUP:	fprintf(stdout, ", BGROUP: 0x"); break;
		case ISUP_PT_NOTI:	fprintf(stdout, ", NOTI: 0x");	break;
		case ISUP_PT_SVACT:	fprintf(stdout, ", SVACT: 0x");	break;
		case ISUP_PT_TRNSRQ:	fprintf(stdout, ", TRNSRQ: 0x"); break;
		case ISUP_PT_SPR:	fprintf(stdout, ", SPR: 0x");	break;
		case ISUP_PT_CGCI:	fprintf(stdout, ", CGCI: 0x");	break;
		case ISUP_PT_CVRI:	fprintf(stdout, ", CVRI: 0x");	break;
		case ISUP_PT_OTGN:	fprintf(stdout, ", OTGN: 0x");	break;
		case ISUP_PT_CIN:	fprintf(stdout, ", CIN: 0x");	break;
		case ISUP_PT_CLLI:	fprintf(stdout, ", CLLI: 0x");	break;
		case ISUP_PT_OLI:	fprintf(stdout, ", OLI: 0x");	break;
		case ISUP_PT_CHGN:	fprintf(stdout, ", CHGN: 0x");	break;
		case ISUP_PT_SVCD:	fprintf(stdout, ", SVCD: 0x");	break;
		case ISUP_PT_CSEL:	fprintf(stdout, ", CSEL: 0x");	break;
		default:		fprintf(stdout, ", [0x%02x]: 0x", pt); break;
		}
		len = *p++ & 0xff;
		for (i = 0; i < len; i++)
			fprintf(stdout, "%02x", *p++);
	}
}

class Message;

class File {
      private:
	File() { }
      protected:
	FILE *fp;
	virtual void checktype(void) = 0;
      public:
	File(char *path) {
		if ((fp = fopen(path, "r")) == NULL)
			throw Error(1);
		this->checktype();
	}
	virtual ~File(void) { if (fp != NULL) fclose(fp); }
	virtual Message *getmessage(void);
}

class FileText : public File {
      private:
	FileText(void) { }
      public:
	FileText(char *path) : File(path) { }
	virtual ~FileText(void) { if (fp != NULL) fclose(fp); fp = NULL; }
	virtual Message *getmessage(void);
      private:
	virtual void checktype(void) {
		if (fgetc(fp) != '#')
			throw Error(1);
	}
}

class FilePcap : public File {
      private:
	pcap_t *pp;
	FilePcap(void) { }
      public:
	FilePcap(char *path) : File(path), pp(NULL) { }
	virtual ~FilePcap(void) {
		if (pp != NULL) { pcap_close(pp); fp = NULL; }
		if (fp != NULL) {     fclose(fp); fp = NULL; }
	}
	virtual Message *getmessage(void);
      private:
	virtual void checktype(void) {
		char errbuf[PCAP_ERRBUF_SIZE];
		if ((pp = pcap_fopen_offline(fp, errbuf)) == NULL)
			throw Error(1);
	}
}

File *filetype(char *path) {
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

class Message {
      public:
	struct timeval tv;
	unsigned char *beg, *mid, *end;
	unsigned int bsn, bib, fsn, fib, li, ni, mp, si, dpc, opc, sls, cic, mt;
	Message(FILE *f) { this->read(); }
	virtual ~Message() { if (beg != NULL) free(beg); }
      protected:
	virtual void read(void) = 0;
}

class Message {
      public:
	struct timeval tv;
	unsigned char *beg, *mid, *end;
	unsigned int bsn, bib, fsn, fib, li, ni, mp, si, dpc, opc, sls, cic, mt;
	Message(FILE * f) {
		size_t size;
		unsigned int tmp;
		beg = (unsigned char *) malloc(4096);
		if (beg == NULL)
			throw Error(1);
		end = ss7readline(f, &tv, beg, 4096);
		if (!end || beg == end)
			throw Error(1);
		size = end - beg;
		if (output > 3 || debug)
			 fprintf(stderr, "Got good message %zu bytes long\n", size);
		if (size < 11)
			throw Error(0);
		beg = (unsigned char *) realloc(beg, size);
		if (beg == NULL)
			throw Error(1);
		end = beg + size;
		mid = beg;
		bsn =  *mid & 0x7f;
		bib = (*mid++ & 0x10) >> 7;
		fsn =  *mid & 0x7f;
		fib = (*mid++ & 0x10) >> 7;
		li  =  *mid++ & 0x3f;
		si  =  *mid & 0x0f;
		if (si != 0x05)
			throw Error(0);
		mp   = (*mid >> 4) & 0x03;
		ni   = (*mid++ >> 6) & 0x03;
		dpc  =  *mid++ & 0xff;
		tmp  =  *mid++ & 0xff;
		dpc |= tmp << 8;
		tmp  =  *mid++ & 0xff;
		dpc |= tmp << 16;
		opc  =  *mid++ & 0xff;
		tmp  =  *mid++ & 0xff;
		opc |= tmp << 8;
		tmp  =  *mid++ & 0xff;
		opc |= tmp << 16;
		sls  =  *mid++ & 0xff;
		cic  =  *mid++ & 0xff;
		tmp  =  *mid++ & 0xff;
		cic |= tmp << 8;
		mt = *mid++;
		if (mid > end)
			throw Error(0);
	};
	~Message(void) {
		if (beg != NULL)
			free(beg);
	};
	void part(void) {
		fprintf(stdlog, "%012d.%06d ", (int)tv.tv_sec, (int)tv.tv_usec);
		for (mid = beg; mid < end; mid++)
			fprintf(stdlog, "%02x", *mid);
		fprintf(stdlog, "\n");
	};
	void dump(void) {
		size_t len, i, odd;
		unsigned int val;
		unsigned char *p;
		struct tm *gmt;
		gmt = gmtime(&tv.tv_sec);
		fprintf(stdout, "%04d/%02d/%02d %02d:%02d:%02d.%06d", gmt->tm_year + 1900,
			gmt->tm_mon+1, gmt->tm_mday, gmt->tm_hour, gmt->tm_min, gmt->tm_sec,
			(int)tv.tv_usec);
		fprintf(stdout, ", OPC: %03u.%03u.%03u, DPC: %03u.%03u.%03u, SLS: %03u, CIC: %05u",
			(opc >> 16) & 0xff, (opc >> 8) & 0xff, (opc >> 0) & 0xff,
			(dpc >> 16) & 0xff, (dpc >> 8) & 0xff, (dpc >> 0) & 0xff, sls, cic);
		switch (mt) {
		case ISUP_MT_IAM:
			fprintf(stdout, ", IAM");
			fprintf(stdout, ", NCI: 0x%02x", *mid++);
			val = *mid++;
			val <<= 8;
			val |= *mid++;
			fprintf(stdout, ", FCI: 0x%04x", val);
			fprintf(stdout, ", CPC: 0x%02x", *mid++);
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", USI: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			p = mid + *mid++;
			len = *p++ & 0xff;
			odd = (*p & 0x80) >> 7;
			fprintf(stdout, ", CDPN: 0x");
			for (i = 0; i < 2; i++)
				fprintf(stdout, "%02x", *p++);
			fprintf(stdout, " ");
			for (i = 0; i < (len - 2) * 2 - odd; i++) {
				if (i & 0x1) {
					val = *p++;
					val >>= 4;
					val &= 0x0f;
				} else {
					val = *p;
					val &= 0x0f;
				}
				if (val < 10)
					fprintf(stdout, "%c", val + '0');
				else
					fprintf(stdout, "%c", val - 10 + 'a');
			}
			if (odd)
				p++;
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_SAM:
			fprintf(stdout, ", SAM");
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_INR:
			fprintf(stdout, ", INR");
			fprintf(stdout, ", INRI: 0x%02x", *mid++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_INF:
			fprintf(stdout, ", INF");
			fprintf(stdout, ", INFI: 0x%02x", *mid++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_COT:
			fprintf(stdout, ", COT");
			fprintf(stdout, ", COTI: 0x%02x", *mid++);
			break;
		case ISUP_MT_ACM:
			fprintf(stdout, ", ACM");
			val = *mid++;
			val <<= 8;
			val |= *mid++;
			fprintf(stdout, ", BCI: 0x%04x", val);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_CON:
			fprintf(stdout, ", CON");
			val = *mid++;
			val <<= 8;
			val |= *mid++;
			fprintf(stdout, ", BCI: 0x%04x", val);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_FOT:
			fprintf(stdout, ", FOT");
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_ANM:
			fprintf(stdout, ", ANM");
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_REL:
			fprintf(stdout, ", REL");
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", CAUS: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_SUS:
			fprintf(stdout, ", SUS");
			fprintf(stdout, ", SRIS: 0x%02x", *mid++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_RES:
			fprintf(stdout, ", RES");
			fprintf(stdout, ", SRIS: 0x%02x", *mid++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_RLC:
			fprintf(stdout, ", RLC");
			if (mid < end)
				dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_CCR:
			fprintf(stdout, ", CCR");
			break;
		case ISUP_MT_RSC:
			fprintf(stdout, ", RSC");
			break;
		case ISUP_MT_BLO:
			fprintf(stdout, ", BLO");
			break;
		case ISUP_MT_UBL:
			fprintf(stdout, ", UBL");
			break;
		case ISUP_MT_BLA:
			fprintf(stdout, ", BLA");
			break;
		case ISUP_MT_UBA:
			fprintf(stdout, ", UBA");
			break;
		case ISUP_MT_GRS:
			fprintf(stdout, ", GRS");
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", R&S: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			break;
		case ISUP_MT_CGB:
			fprintf(stdout, ", CGB");
			fprintf(stdout, ", CGI: 0x%02x", *mid++);
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", R&S: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			break;
		case ISUP_MT_CGU:
			fprintf(stdout, ", CGU");
			fprintf(stdout, ", CGI: 0x%02x", *mid++);
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", R&S: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			break;
		case ISUP_MT_CGBA:
			fprintf(stdout, ", CGBA");
			fprintf(stdout, ", CGI: 0x%02x", *mid++);
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", R&S: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			break;
		case ISUP_MT_CGUA:
			fprintf(stdout, ", CGUA");
			fprintf(stdout, ", CGI: 0x%02x", *mid++);
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", R&S: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			break;
		case ISUP_MT_CMR:
			fprintf(stdout, ", CMR");
			break;
		case ISUP_MT_CMC:
			fprintf(stdout, ", CMC");
			break;
		case ISUP_MT_CMRJ:
			fprintf(stdout, ", CMRJ");
			break;
		case ISUP_MT_FAR:
			fprintf(stdout, ", FAR");
			fprintf(stdout, ", FACI: 0x%02x", *mid++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_FAA:
			fprintf(stdout, ", FAA");
			fprintf(stdout, ", FACI: 0x%02x", *mid++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_FRJ:
			fprintf(stdout, ", FRJ");
			fprintf(stdout, ", FACI: 0x%02x", *mid++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_FAD:
			fprintf(stdout, ", FAD");
			fprintf(stdout, ", FACI: 0x%02x", *mid++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_FAI:
			fprintf(stdout, ", FAI");
			fprintf(stdout, ", FACI: 0x%02x", *mid++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_LPA:
			fprintf(stdout, ", LPA");
			break;
		case ISUP_MT_CSVQ:
			fprintf(stdout, ", CSVQ");
			break;
		case ISUP_MT_CSVR:
			fprintf(stdout, ", CSVR");
			break;
		case ISUP_MT_DRS:
			fprintf(stdout, ", DRS");
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_PAM:
			fprintf(stdout, ", PAM");
			break;
		case ISUP_MT_GRA:
			fprintf(stdout, ", GRA");
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", R&S: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			break;
		case ISUP_MT_CQM:
			fprintf(stdout, ", CQM");
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", R&S: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			break;
		case ISUP_MT_CQR:
			fprintf(stdout, ", CQR");
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", R&S: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", CSI: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			break;
		case ISUP_MT_CPG:
			fprintf(stdout, ", CPG");
			fprintf(stdout, ", EVNT: 0x%02x", *mid++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_USR:
			fprintf(stdout, ", USR");
			break;
		case ISUP_MT_UCIC:
			fprintf(stdout, ", UCIC");
			break;
		case ISUP_MT_CFN:
			fprintf(stdout, ", CFN");
			p = mid + *mid++;
			len = *p++ & 0xff;
			fprintf(stdout, ", CAUS: 0x");
			for (i = 0; i < len; i++)
				fprintf(stdout, "%02x", *p++);
			break;
		case ISUP_MT_OLM:
			fprintf(stdout, ", ONM");
			break;
		case ISUP_MT_CRG:
			fprintf(stdout, ", CRG");
			break;
		case ISUP_MT_NRM:
			fprintf(stdout, ", NRM");
			break;
		case ISUP_MT_FAC:
			fprintf(stdout, ", FAC");
			break;
		case ISUP_MT_UPT:
			fprintf(stdout, ", UPT");
			break;
		case ISUP_MT_UPA:
			fprintf(stdout, ", UPA");
			break;
		case ISUP_MT_IDR:
			fprintf(stdout, ", IDR");
			break;
		case ISUP_MT_IRS:
			fprintf(stdout, ", IRS");
			break;
		case ISUP_MT_SGM:
			fprintf(stdout, ", SGM");
			break;
		case ISUP_MT_CRA:
			fprintf(stdout, ", CRA");
			break;
		case ISUP_MT_CRM:
			fprintf(stdout, ", CRM");
			fprintf(stdout, ", NCI: 0x%02x", *mid++);
			break;
		case ISUP_MT_CVR:
			fprintf(stdout, ", CVR");
			fprintf(stdout, ", CVRI: 0x%02x", *mid++);
			fprintf(stdout, ", CGCI: 0x%02x", *mid++);
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_CVT:
			fprintf(stdout, ", CVT");
			break;
		case ISUP_MT_EXM:
			fprintf(stdout, ", EXM");
			dump_oparms(mid + *mid++, end);
			break;
		case ISUP_MT_NON:
			fprintf(stdout, ", NON");
			break;
		case ISUP_MT_LLM:
			fprintf(stdout, ", LLM");
			break;
		case ISUP_MT_CAK:
			fprintf(stdout, ", CAK");
			break;
		case ISUP_MT_TCM:
			fprintf(stdout, ", TCM");
			break;
		case ISUP_MT_MCP:
			fprintf(stdout, ", MCP");
			break;
		default:
			fprintf(stdout, ", ???");
			dump_oparms(mid + *mid++, end);
			break;
		};
	};
};

Message *FileText::getmessage(void) { return (new MessageText(fp)); }
Message *FilePcap::getmessage(void) { return (new MessagePcap(pp)); }

class File {
      private:
	pcap_t *pp;
	FILE *fp;
	int state;
	File() { }
      public:
	File(char *path) {
		int c;
		if ((fp = fopen(path, "r")) == NULL) {
			fprintf(stderr, "%s: %s: %s\n", __PRETTY_FUNCTION__, path, strerror(errno));
			throw Error(1);
		}
		if ((c = fgetc(fp)) == EOF) {
			if (ferror(f)) {
				fprintf(stderr, "%s: %s: %s\n", __PRETTY_FUNCTION__, path, strerror(errno));
				throw Error(1);
			}
			if (feof(fp)) {
				fclose(fp);
				fp = NULL;
				state = EOF;
			}
		} else {
			if (c == '#') {
				if ((fp = freopen(path, "r", fp)) == NULL) {
					fprintf(stderr, "%s: %s: %s\n", __PRETTY_FUNCTION__, path, strerror(errno));
					throw Error(1);
				}
			} else {
				char errbuf[PCAP_ERRBUF_SIZE];
				if ((fp = freopen(path, "r", fp)) == NULL) {
					fprintf(stderr, "%s: %s: %s\n", __PRETTY_FUNCTION__, path, strerror(errno));
					throw Error(1);
				}
				if ((pp = pcap_fopen_offline(fp, errbuf)) == NULL) {
					fprintf(stderr, "%s: %s: %s\n", __PRETTY_FUNCTION__, path, errbuf);
					throw Error(1);
				}
				fp = NULL;
			}
		}
	}
	bool eof(void) {
		return (state == EOF);
	}
	Message *getmsg(void) {
		Message *m = NULL;
		if (pp != NULL)
			m = new MessagePcap(pp);
		if (fp != NULL)
			m = new MessageText(fp);
		return (m);
	}
	~File() {
		if (pp != NULL) {
			pcap_close(pp);
			pp = NULL;
		}
		if (fp != NULL) {
			fclose(fp);
			fp = NULL;
		}
	}
}

static vector<Message *> msgs;

inline bool tv_comp(const Message * m1, const Message * m2)
{
	if (m1->tv.tv_sec < m2->tv.tv_sec)
		return true;
	if (m1->tv.tv_sec != m2->tv.tv_sec)
		return false;
	if (m1->tv.tv_usec < m2->tv.tv_usec)
		return true;
	return false;
}

struct cid {
	unsigned int dpc;
	unsigned int opc;
	unsigned int cic;
};

enum CircuitState {
	CTS_UNINIT = 0,
	CTS_IDLE,
	CTS_WAIT_ACM,
	CTS_WAIT_ANM,
	CTS_ANSWERED,
	CTS_SUSPENDED,
	CTS_WAIT_RLC,
	CTS_SEND_RLC,
	CTS_COMPLETE,
};

class Call {
      public:
	struct cid cid;
	enum CircuitState state;
        list<Message *> messages;
	 Call(Message * msg) {
		cid.dpc = msg->dpc;
		cid.opc = msg->opc;
		cid.cic = msg->cic;
	};
	void add_msg(Message * msg, enum CircuitState s) {
		messages.push_back(msg);
		state = s;
	};
	void clear(enum CircuitState s) {
		messages.erase(messages.begin(), messages.end());
		state = s;
	};
	void dump(void) {
                list<Message *>::iterator imsg = messages.begin();
		if (state == CTS_IDLE && imsg != messages.end() &&
		    ((*imsg)->mt == ISUP_MT_IAM || (*imsg)->mt == ISUP_MT_CRM)) {
			while (imsg != messages.end()) {
				(*imsg)->dump();
				if (++imsg != messages.end()) {
					fprintf(stdout, ",\n");
					continue;
				}
				fprintf(stdout, "\n");
			}
			fprintf(stdout, "\n");
		}
	};
	void part(void) {
                list<Message *>::iterator imsg = messages.begin();
		if (state > CTS_IDLE && imsg != messages.end()) {
			for (; imsg != messages.end(); ++imsg)
				(*imsg)->part();
		}
	};
};

static list<Call *> calls;

class Circuit {
      public:
	struct cid cid;
	Call *call;
	enum CircuitState state;	// circuit state
	 Circuit(Message * msg) {
		cid.dpc = msg->dpc;
		cid.opc = msg->opc;
		cid.cic = msg->cic;
		call = NULL;
	};
	void add_msg(Message * msg) {
	      redo_message:
		if (call == NULL) {
			call = new Call(msg);
			if (call == NULL)
				throw Error(1);
			calls.push_back(call);
		}
		switch (msg->mt) {
		case ISUP_MT_IAM:
			switch (state) {
			case CTS_IDLE:
			case CTS_WAIT_ACM:
				break;
			default:
				goto restart_call;
			}
			state = CTS_WAIT_ACM;
			break;
		case ISUP_MT_SAM:
			goto discard;
		case ISUP_MT_INR:
		case ISUP_MT_INF:
		case ISUP_MT_COT:
			break;
		case ISUP_MT_EXM:
		case ISUP_MT_ACM:
			if (state != CTS_WAIT_ACM)
				goto discard;
			state = CTS_WAIT_ANM;
			break;
		case ISUP_MT_CON:
			if (state != CTS_WAIT_ANM)
				goto discard;
			state = CTS_ANSWERED;
			break;
		case ISUP_MT_FOT:
			break;
		case ISUP_MT_ANM:
			if (state != CTS_WAIT_ANM)
				goto discard;
			state = CTS_ANSWERED;
			break;
		case ISUP_MT_REL:
			state = CTS_WAIT_RLC;
			break;
		case ISUP_MT_SUS:
			if (state != CTS_ANSWERED && state != CTS_SUSPENDED)
				goto discard;
			state = CTS_SUSPENDED;
			break;
		case ISUP_MT_RES:
			if (state != CTS_ANSWERED && state != CTS_SUSPENDED)
				goto discard;
			state = CTS_ANSWERED;
			break;
		case ISUP_MT_RLC:
		case ISUP_MT_CCR:
			goto end_of_call;
		case ISUP_MT_RSC:
			if (state > CTS_WAIT_ANM)
				goto end_of_call;
			goto clear_call;
		case ISUP_MT_BLO:
			if (state < CTS_WAIT_ANM)
				goto clear_call;
			goto discard;
		case ISUP_MT_UBL:
			goto discard;
		case ISUP_MT_BLA:
			if (state < CTS_WAIT_ANM)
				goto clear_call;
			goto discard;
		case ISUP_MT_UBA:
			goto discard;
		case ISUP_MT_GRS:
			if (state > CTS_WAIT_ANM)
				goto end_of_call;
			goto clear_call;
		case ISUP_MT_CGU:
			goto discard;
		case ISUP_MT_CGBA:
			if (state < CTS_WAIT_ANM)
				goto clear_call;
			goto discard;
		case ISUP_MT_CGUA:
			goto discard;
		case ISUP_MT_CMR:
		case ISUP_MT_CMC:
		case ISUP_MT_CMRJ:
		case ISUP_MT_FAR:
		case ISUP_MT_FAA:
		case ISUP_MT_FRJ:
		case ISUP_MT_FAD:
		case ISUP_MT_FAI:
			goto discard;
		case ISUP_MT_LPA:
			break;
		case ISUP_MT_CSVQ:
		case ISUP_MT_CSVR:
		case ISUP_MT_DRS:
			break;
		case ISUP_MT_PAM:
			break;
		case ISUP_MT_GRA:
			if (state > CTS_WAIT_ANM)
				goto end_of_call;
			goto clear_call;
		case ISUP_MT_CQM:
			goto discard;
		case ISUP_MT_CQR:
			goto discard;
		case ISUP_MT_CPG:
			if (state != CTS_WAIT_ACM && state != CTS_WAIT_ANM)
				goto discard;
			break;
		case ISUP_MT_USR:
			goto discard;
		case ISUP_MT_UCIC:
			goto clear_call;
		case ISUP_MT_CFN:
			if (state < CTS_ANSWERED)
				goto clear_call;
			break;
		case ISUP_MT_OLM:
		case ISUP_MT_CRG:
		case ISUP_MT_NRM:
			goto discard;
		case ISUP_MT_FAC:
			break;
		case ISUP_MT_UPT:
		case ISUP_MT_UPA:
		case ISUP_MT_IDR:
		case ISUP_MT_IRS:
			goto discard;
		case ISUP_MT_SGM:
			break;
		case ISUP_MT_CRA:
			if (state != CTS_WAIT_ACM)
				goto discard;
			state = CTS_WAIT_ACM;
			break;
		case ISUP_MT_CRM:
			switch (state) {
			case CTS_IDLE:
			case CTS_WAIT_ACM:
				break;
			default:
				goto end_of_call;
			}
			state = CTS_WAIT_ACM;
			break;
		case ISUP_MT_CVR:
		case ISUP_MT_CVT:
			goto discard;
		case ISUP_MT_NON:
		case ISUP_MT_LLM:
		case ISUP_MT_CAK:
		case ISUP_MT_TCM:
		case ISUP_MT_MCP:
		default:
			goto discard;
		}
		call->add_msg(msg, state);
	      discard:
		return;
	      end_of_call:
		state = CTS_IDLE;
		call->add_msg(msg, state);
		call = NULL;
		return;
	      clear_call:
		state = CTS_IDLE;
		call->clear(state);
		return;
	      restart_call:
		state = CTS_IDLE;
		if (call->messages.begin() != call->messages.end())
			call = NULL;
		else
			call->clear(state);
		goto redo_message;
	};
};

struct cic_hastr {
	size_t operator() (struct cid c) const {
		return (c.dpc + c.opc + c.cic);
	};
};

struct cic_eqstr {
	bool operator() (const struct cid c1, const struct cid c2) const {
		return (c1.cic == c2.cic &&
			((c1.opc == c2.opc && c1.dpc == c2.dpc) ||
			 (c1.opc == c2.dpc && c1.dpc == c2.opc)));
	};
};

static hash_map<struct cid,Circuit *,cic_hastr,cic_eqstr> circuits;

// read an input line from the file
unsigned char *ss7readline(FILE * f, struct timeval *tv, unsigned char *buf, size_t max)
{
	int state = 0;
	int c;
	unsigned char byte;
	long secs = 0, usec = 0;
	unsigned char *ptr = buf;
	for (;;) {
		if (feof(f) || ferror(f))
			return (NULL);
		c = fgetc(f);
		if (output > 4 || debug)
			fprintf(stdout, "%c", (unsigned char) c);
		switch (state) {
		case 0:	// beg of line
			switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				secs = c - '0';
				state = 2;
				continue;
			case ' ':
			case '\t':
			case '\n':
				continue;
			case '#':
				state = 1;
				continue;
			}
			break;
		case 1:	// comment line
			switch (c) {
			case '\n':
				state = 0;
				continue;
			}
			continue;
		case 2:	// secs
			switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				secs = secs * 10 + (c - '0');
				continue;
			case '.':
				usec = 0;
				state = 3;
				continue;
			case '\n':
				// error
				state = 0;
				continue;
			}
			break;
		case 3:	// usecs
			switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				usec = usec * 10 + (c - '0');
				continue;
			case ' ':
			case '\t':
				state = 5;
				continue;
			case '\n':
				// error
				state = 0;
				continue;
			}
			break;
		case 4:	// space
			switch (c) {
			case ' ':
				state = 5;
				continue;
			case '\n':
				// error
				state = 0;
				continue;
			}
			break;
		case 5:	// message (nibble 1)
		case 6:	// message (nibble 2)
			switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (state == 5) {
					byte = c - '0';
					state = 6;
				} else {
					byte = (byte << 4) + c - '0';
					*ptr++ = byte;
					state = 5;
				}
				continue;
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				if (state == 5) {
					byte = c - 'a' + 10;
					state = 6;
				} else {
					byte = (byte << 4) + c - 'a' + 10;
					*ptr++ = byte;
					state = 5;
				}
				continue;
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
				if (state == 5) {
					byte = c - 'A' + 10;
					state = 6;
				} else {
					byte = (byte << 4) + c - 'A' + 10;
					*ptr++ = byte;
					state = 5;
				}
				continue;
			case '\n':
				if (state == 5) {
					tv->tv_sec = secs;
					tv->tv_usec = usec;
					return (ptr);
				}
				// error
				ptr = buf;
				state = 0;
				continue;
			case ' ':
			case '\t':
				if (state == 5) {
					state = 7;
					continue;
				}
				break;
			}
			break;
		case 7:	// trailing whitespace
			switch (c) {
			case ' ':
			case '\t':
				continue;
			case '#':
				state = 8;
				continue;
			case '\n':
				tv->tv_sec = secs;
				tv->tv_usec = usec;
				return (ptr);
			}
			break;
		case 8:	// trailing comment
			switch (c) {
			case '\n':
				tv->tv_sec = secs;
				tv->tv_usec = usec;
				return (ptr);
			}
			continue;
		case 9:	// error
			switch (c) {
			case '\n':
				state = 0;
				continue;
			}
			continue;
		case EOF:
		{
			int err = ferror(f);
			throw Error(err);
			return (NULL);
		}
		}
		// error
		if (output > 2 || debug)
			fprintf(stderr, "Gosh darn error state = %d\n", state);
		ptr = buf;
		state = 9;
	}
}

void isupcor(int nfiles, char **filev)
{
	int i;
	if (output > 2 || debug) {
		fprintf(stderr, "%d nfiles\n", nfiles);
		for (i = 0; i < nfiles; i++)
			fprintf(stderr, "%s\n", filev[i]);
	}
	if (outfile[0] != '\0') {
		// redirect standard output
		if (freopen(outfile, "w", stdout) == NULL) {
			perror(__FUNCTION__);
			exit(1);
		}
	}
	if (parfile[0] != '\0') {
		// open partial records file
		if ((stdlog = fopen(parfile, "w")) == NULL) {
			perror(__FUNCTION__);
			exit(1);
		}
	}
	for (i = 0; i < nfiles; i++) {
		if ((files[i] = fopen(filev[i], "r")) != NULL)
			continue;
		fprintf(stderr, "%s: %s: %s\n", __FUNCTION__, filev[i], strerror(errno));
		exit(1);
	}
	/* first we suck the files in - don't care if they're big, that's what virtual memory is
	   for */
	for (i = 0; i < nfiles; i++) {
		Message *m;
	      try_again:
		try {
			for (;;) {
				if ((m = new Message(files[i])) != NULL) {
					msgs.push_back(m);
					continue;
				}
				goto baderror;
			}
		}
		catch(Error e) {
			if (e.error == 0)
				goto try_again;
			if (output > 2 || debug)
				fprintf(stderr, "Got error %d\n", e.error);
		}
	}
	stable_sort(msgs.begin(), msgs.end(), tv_comp);
#if 0
	for (vector<Message *>::iterator it = msgs.begin(); it != msgs.end(); ++it)
		fprintf(stdout, "%012ld.%06ld\n", (*it)->tv.tv_sec, (*it)->tv.tv_usec);
#endif
	for (vector<Message *>::iterator it = msgs.begin(); it != msgs.end(); ++it) {
		struct cid cid = { (*it)->dpc, (*it)->opc, (*it)->cic };
                hash_map<struct cid,Circuit *,cic_hastr,cic_eqstr>::iterator cic;
		if (output > 5 || debug) {
			fprintf(stdout, "%012ld.%06ld ", (*it)->tv.tv_sec, (*it)->tv.tv_usec);
			fprintf(stdout, "%03u.%03u.%03u %03u.%03u.%03u %03u %05u %03u\n",
				((*it)->dpc >> 16) & 0xff, ((*it)->dpc >> 8) & 0xff,
				((*it)->dpc >> 9) & 0xff, ((*it)->opc >> 16) & 0xff,
				((*it)->opc >> 8) & 0xff, ((*it)->opc >> 9) & 0xff,
				(*it)->sls, (*it)->cic, (*it)->mt);
		}
		if ((cic = circuits.find(cid)) == circuits.end()) {
			// we don't have a circuit object for this message yet
			circuits[cid] = new Circuit(*it);
			if ((cic = circuits.find(cid)) == circuits.end())
				throw Error(1);
		}
		circuits[cid]->add_msg(*it);
	}
	for (list<Call *>::iterator icall = calls.begin(); icall != calls.end(); ++icall)
		(*icall)->dump();
	if (stdlog != NULL) {
	for (list<Call *>::iterator icall = calls.begin(); icall != calls.end(); ++icall)
		(*icall)->part();
	}
      baderror:
	for (i = 0; i < nfiles; i++) {
		if (files[i])
			fclose(files[i]);
	}
	return;
}

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
", NAME, PACKAGE, VERSION, PACKAGE_ENVR " " PACKAGE_DATE);
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
	int c, val;

	while (1) {
#if defined _GNU_SOURCE
		int option_index = 0;
                /* *INDENT-OFF* */
		static struct option long_options[] = {
			{"outfile",	required_argument,	NULL,	'o'},
			{"parfile",	required_argument,	NULL,	'p'},
			{"quiet",	no_argument,		NULL,	'q'},
			{"debug",	optional_argument,	NULL,	'D'},
			{"verbose",	optional_argument,	NULL,	'v'},
			{"help",	no_argument,		NULL,	'h'},
			{"version",	no_argument,		NULL,	'V'},
			{"copying",	no_argument,		NULL,	'C'},
			{"?",		no_argument,		NULL,	'H'},
                        {NULL,		0,			NULL,	 0 }
		};
                /* *INDENT-ON* */
		c = getopt_long(argc, argv, "o:p:qD::v::hVC?W:", long_options, &option_index);
#else                           /* defined _GNU_SOURCE */
		c = getopt(argc, argv, "o:p:qDvhVC?");
#endif                          /* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'o':	// -o, --outfile outfile
			if (debug)
				fprintf(stderr, "%s: setting outfile to %s\n", argv[0], optarg);
			strncpy(outfile, optarg, sizeof(outfile) - 1);
			break;
		case 'p':	// -p, --parfile parfile
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
		case 'v':	/* -v, --verbose */
			if (debug)
				fprintf(stderr, "%s: increasing output verbosity\n", argv[0]);
			if (optarg == NULL) {
				output += 1;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
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
	isupcor(argc - optind, &argv[optind]);
	exit(4);
}
