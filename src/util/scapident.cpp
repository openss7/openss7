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

// The purpose of this program is to identify the characteristics of a number of
// files.  It is the basis for the monitoring program.

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
#include <map>
#include <set>

using std::list;
using std::vector;
using __gnu_cxx::hash_map;
using std::map;
using std::set;

int output = -1;
int debug = 0;

class Error;
class MsgCount;
class MsgStat;
class File;
class FileText;
class FilePcap;
class Node;
class Path;
class Message;

class Error {
      private:
	int error;
      public:
	Error(int val) : error(val) { };
	virtual ~Error(void) { };
};

struct pseudohdr {
	unsigned char dir;
	unsigned char xsn;
	unsigned char lkno[2];
};

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
//  -------------------------------------------------------------------------
//
//  Count class
//
//  -------------------------------------------------------------------------
template<class _Message>
class Count {
      public:
	Count(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
	};
	virtual ~Count(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
	};
	virtual void identify(void) = 0;
	virtual void add(_Message& msg) = 0;
	virtual void complete(_Message& msg) = 0;
};
//  -------------------------------------------------------------------------
//
//  Stat class
//
//  -------------------------------------------------------------------------
struct itv_ltstr {
	bool operator() (const unsigned long i1, const unsigned long i2) const {
		return (i1 < i2);
	};
};
template<class _Count, class _Message>
class Stat  : public _Count {
      protected:
	map<unsigned long,_Count,itv_ltstr> intervals;
	unsigned long convert(struct timeval tv) {
		return ((tv.tv_sec + ((tv.tv_usec > 0) ? 1 : 0) + 299) / 300);
	};

      public:
	Stat(void) : _Count() {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
	};
	virtual ~Stat() {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
	};
	virtual void identify(void);
	virtual void add(_Message& msg);
	virtual void complete(_Message& msg);
};
//  -------------------------------------------------------------------------
//
//  MsgCount class
//
//  -------------------------------------------------------------------------
//  This is a base class for message counts.
struct mtypes {
	unsigned char si;
	unsigned char mt;
	unsigned char mp;
};
struct mt_ltstr {
	bool operator() (const struct mtypes m1, const struct mtypes m2) const {
		if (m1.si < m2.si) return (true);
		if (m1.si > m2.si) return (false);
		if (m1.mt < m2.mt) return (true);
		if (m1.mt > m2.mt) return (false);
		if (m1.mp < m2.mp) return (true);
		return (false);
	};
};
class MsgCount : public Count<Message> {
      protected:
	unsigned long msgs;
	unsigned long bytes;
	unsigned long octets;
	unsigned long bads;
	unsigned long fisus;
	unsigned long lssus;
	unsigned long lssu2s;
	unsigned long msus;
	unsigned long siosifs;
	friend class Message;
	unsigned long sis[16];
	unsigned long mps[4];
	unsigned long simps[16][4];
	unsigned long undec;
	typedef map<struct mtypes,unsigned long,mt_ltstr> counts_map;
	counts_map counts;
	struct timeval beg, end;


      public:
	MsgCount(void) : msgs(0), bytes(0), octets(0), bads(0), fisus(0),
		lssus(0), lssu2s(0), msus(0), siosifs(0), sis(), mps(), simps(),
		undec(0)
	{
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		beg.tv_sec = 0x7fffffff;
		beg.tv_usec = 0x7fffffff;
		end.tv_sec = 0;
		end.tv_usec = 0;
	};
	virtual ~MsgCount(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
	};
	virtual void identify(void);
	virtual void add(Message& msg);
	virtual void complete(Message& msg);
	unsigned long msuCounts(void);
	unsigned long siCounts(int si);
	unsigned long siMtCounts(int si, int mt);
	unsigned long siMtMpCounts(int si, int mt, int mp);
	unsigned long mpCounts(int mp);
};

//  -------------------------------------------------------------------------
//
//  MsgStat class
//
//  -------------------------------------------------------------------------
//  This is a base class for things that count messages.
class MsgStat : public Stat<MsgCount,Message> {
	typedef Stat<MsgCount,Message> _Stat;
      public:
	MsgStat(void) : _Stat() {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
	};
	virtual ~MsgStat(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
	};
};

//  -------------------------------------------------------------------------
//
//  File class
//
//  -------------------------------------------------------------------------
class File : public MsgStat {
      protected:
	File() { };
	FILE *fp;
	char fname[256];
	list<Path *> fpaths;
	void delall(void);

      public:
	File(char *path) : MsgStat(), fp(NULL), fpaths() {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		if ((fp = fopen(path, "r")) == NULL) {
			perror(__PRETTY_FUNCTION__);
			throw Error(1);
		}
		strncpy(fname, path, 255);
	};
	virtual ~File(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		delall();
	};
	void add(Path *path) {
		fpaths.push_back(path);
	};
	void del(Path *path) {
		for (list<Path *>::iterator it = fpaths.begin(); it != fpaths.end(); ++it) {
			if (*it == path) {
				if (debug > 1)
					fprintf(stderr, "I: %s: erasing path %p\n", __PRETTY_FUNCTION__, path);
				fpaths.erase(it);
				break;
			}
		}
	};
	virtual void add(Message& msg);
	virtual Message *readmsg(void) = 0;
	virtual void readmsgs(void);
	virtual void identify(void);
};

//  -------------------------------------------------------------------------
//
//  FileText class
//
//  -------------------------------------------------------------------------
//  Represents a text file.  When text files are used, each file can potentially
//  represent a different data link path.  However, we have no idea which data
//  link path that might be (no identifier) until we read the header.
//
class FileText : public File {
      private:
	FileText(void) { };
	uint coms;
	char buf[1024];
	int ppa;
	struct pseudohdr phdr;
	int line;

      public:
	FileText(char *path) : File(path), coms(0), ppa(0), line(1) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		int ret, i1, i2;
		char c1, c2;
		if ((ret = fscanf(fp, "%u.%u %c%c", &i1, &i2, &c1, &c2)) < 4) {
			if (debug)
				fprintf(stderr, "W: %s: cannot parse file %s\n", __PRETTY_FUNCTION__, path);
			throw Error(1);
		}
		if (c1 != '#' || c2 != ' ') {
			fprintf(stderr, "E: %s: first characters are '%c%c' not '# '\n", __PRETTY_FUNCTION__, c1, c2);
			throw Error(1);
		}
		rewind(fp);
	};
	virtual ~FileText(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		fclose(fp);
	};
	virtual Message *readmsg(void);
	virtual void identify(void);
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

      public:
	FilePcap(char *path) : File(path), pp(NULL) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		errbuf[0] = '\0';
		if ((pp = pcap_fopen_offline(fp, errbuf)) == NULL) {
			fprintf(stderr, "error: %s: %s\n", __PRETTY_FUNCTION__, errbuf);
			throw Error(1);
		}
	};
	virtual ~FilePcap(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		pcap_close(pp);
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
	virtual Message *readmsg(void);
	virtual void identify(void);
};

hash_map<int,Path *> paths;

//  -------------------------------------------------------------------------
//
//  Node class
//
//  -------------------------------------------------------------------------
class Node {
      private:
	int pcode;
	MsgStat orig;
	MsgStat dest;
	bool orig_tcap;
	bool term_tcap;
	bool orig_isup;
	bool term_isup;
	bool exch_sltm;
	set<int> queries;
	set<int> responds;
	set<int> adjacent;
	set<int> circuits;

      public:
	Node(void) : pcode(0), orig(), dest(),
	orig_tcap(false), term_tcap(false),
	orig_isup(false), term_isup(false),
	exch_sltm(false)
	{ };
	Node(int pc) : pcode(pc), orig(), dest(),
	orig_tcap(false), term_tcap(false),
	orig_isup(false), term_isup(false),
	exch_sltm(false)
	{ };
	~Node(void) { };
	void identify(void);
	void add_orig(Message& msg);
	void add_dest(Message& msg);
};

map<int,Node> nodes;

//  -------------------------------------------------------------------------
//
//  Path class
//
//  -------------------------------------------------------------------------
//  Each path object represents a channel of communication.
//
enum HeaderType {
	HT_UNKNOWN,
	HT_BASIC,
	HT_EXTENDED
};
enum RlType {
	RL_UNKNOWN,
	RL_14BIT_PC,
	RL_24BIT_PC
};
enum MsgPriority {
	MP_UNKNOWN,
	MP_JAPAN,
	MP_NATIONAL,
	MP_INTERNATIONAL
};
class Path : public MsgStat {
      private:
	// static hash_map<int,Path *> paths;
	File& file;
	int ppa;
	enum HeaderType ht;
	enum MsgPriority pr;
	enum RlType rt;
	list<Message *> messages;
	set<int> orig;
	set<int> dest;

      public:
	Path(File& f, int a) : file(f), ppa(a), ht(HT_UNKNOWN),
		pr(MP_UNKNOWN), rt(RL_UNKNOWN)
	{
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		paths[ppa] = this;
		file.add(this);
		if (debug > 1)
			fprintf(stderr, "I: %s: created (%d:%d:%d) %p\n", __PRETTY_FUNCTION__,
					cardNum(),spanNum(),slotNum(),this);
		if (slotNum() == 0)
			setHeaderType(HT_EXTENDED);
		else
			setHeaderType(HT_BASIC);
	};
	virtual ~Path(void) {
		if (debug > 1)
			fprintf(stderr, "I: %s: %p\n", __PRETTY_FUNCTION__, this);
		file.del(this);
		paths.erase(ppa);
	};
	static Path& get(File& f, int a) {
		Path *p;
		hash_map<int,Path *>::iterator it;
		if ((it = paths.find(a)) == paths.end()) {
			if ((p = new Path(f, a)) == NULL)
				throw Error(1);
		} else
			p = paths[a];
		return (*p);
	};
	int cardNum(void) { return ((ppa >> 7) & 0x01); };
	int spanNum(void) { return ((ppa >> 5) & 0x03); };
	int slotNum(void) { return ((ppa >> 0) & 0x1f); };
	virtual void identify(void);
	virtual void add(Message& msg);
	virtual void complete(Message& msg);
	enum HeaderType headerType(void) { return (ht); }
	void setHeaderType(enum HeaderType t) {
		if (ht != t && t != HT_UNKNOWN) {
			switch (ht) {
			case HT_UNKNOWN:
				if (debug) {
					if (t == HT_BASIC)
						fprintf(stderr, "I: path header type set to basic\n");
					else if (t == HT_EXTENDED)
						fprintf(stderr, "I: path header type set to extended\n");
				}
				break;
			case HT_BASIC:
				if (debug && t == HT_EXTENDED)
					fprintf(stderr, "W: path header type change from basic to extended\n");
				break;
			case HT_EXTENDED:
				if (debug && t == HT_BASIC)
					fprintf(stderr, "W: path header type change from extended to basic\n");
				break;
			}
			ht = t;
		}
	};
	enum MsgPriority msgPriority(void) { return (pr); };
	void setMsgPriority(enum MsgPriority p) {
		if (pr != p && p != MP_UNKNOWN) {
			switch (pr) {
			case MP_UNKNOWN:
				if (debug) {
					if (p == MP_JAPAN)
						fprintf(stderr, "I: msg priority set to japan\n");
					else if (p == MP_NATIONAL)
						fprintf(stderr, "I: msg priority set to national\n");
					else if (p == MP_INTERNATIONAL)
						fprintf(stderr, "I: msg priority set to international\n");
				}
				break;
			case MP_JAPAN:
				if (debug) {
					if (p == MP_NATIONAL)
						fprintf(stderr, "W: msg priority change from japan to national\n");
					if (p == MP_INTERNATIONAL)
						fprintf(stderr, "W: msg priority change from japan to international\n");
				}
				break;
			case MP_NATIONAL:
				if (debug) {
					if (p == MP_JAPAN)
						fprintf(stderr, "W: msg priority change from national to japan\n");
					if (p == MP_INTERNATIONAL)
						fprintf(stderr, "W: msg priority change from national to international\n");
				}
				break;
			case MP_INTERNATIONAL:
				if (debug) {
					if (p == MP_NATIONAL)
						fprintf(stderr, "W: msg priority change from international to national\n");
					if (p == MP_JAPAN)
						fprintf(stderr, "W: msg priority change from international to japan\n");
				}
				break;
			}
			pr = p;
		}
	};
	enum RlType routingLabel(void) { return (rt); };
	void setRoutingLabel(enum RlType r) {
		if (rt != r && r != RL_UNKNOWN) {
			switch (rt) {
			case RL_UNKNOWN:
				if (debug) {
					if (r == RL_24BIT_PC)
						fprintf(stderr, "W: routing label set to 24-bit\n");
					if (r == RL_14BIT_PC)
						fprintf(stderr, "W: routing label set to 14-bit\n");
				}
				break;
			case RL_14BIT_PC:
				if (debug && r == RL_24BIT_PC)
					fprintf(stderr, "W: routing label change from 14-bit to 24-bit\n");
				break;
			case RL_24BIT_PC:
				if (debug && r == RL_14BIT_PC)
					fprintf(stderr, "W: routing label change from 24-bit to 14-bit\n");
				break;
			}
			rt = r;
		}
	};
	bool detecting(void) {
		if (ht == HT_UNKNOWN)
			return (true);
		if (pr == MP_UNKNOWN)
			return (true);
		if (rt == RL_UNKNOWN)
			return (true);
		return (false);
	};
};

//  -------------------------------------------------------------------------
//
//  Message class
//
//  -------------------------------------------------------------------------
//  Each message object represents an SS7 message.
//
enum MsgType {
	MT_UNKNOWN,
	MT_FISU,
	MT_LSSU,
	MT_LSSU2,
	MT_MSU
};
enum ProtocolTest {
	PT_YES,
	PT_MAYBE,
	PT_NO
};
class Message {
      private:
	Path& path;

      public:
	struct pcap_pkthdr hdr;
	unsigned char *dat;
	struct pseudohdr phdr;
	bool mtp2decode;
	bool mtp3decode;
	unsigned char *beg;
	unsigned char *mid;
	unsigned char *end;
	unsigned char *sio;
	unsigned char *sif;
	unsigned char *sdu;
	unsigned int bib, bsn, fib, fsn, li, li0, ni, mp, si, dpc, opc, sls, cic, mt, h0, h1, slc, dlen;

	Message(Path& p, struct pcap_pkthdr *h, unsigned char *d, struct pseudohdr *ph) :
		path(p), hdr(*h), dat(d), phdr(*ph), mtp2decode(false), mtp3decode(false),
		beg(d), mid(d), end(d+h->caplen)
	{
		if (debug > 3)
			fprintf(stderr, "I: %s\n", __PRETTY_FUNCTION__);
	};
	virtual ~Message(void) {
		if (debug > 3)
			fprintf(stderr, "I: %s\n", __PRETTY_FUNCTION__);
		free(dat);
	};
	int decode(void);
	bool isFisu(void) { return ((hdr.len == 3) || (hdr.len == 6)); };
	bool isLssu(void) { return ((hdr.len == 4) || (hdr.len == 7)); };
	bool isLssu2(void) { return ((hdr.len == 5) || (hdr.len == 6)); };
	bool isMsu(void) { return (hdr.len > 6); };
	enum MsgType messageType(void) {
		switch (hdr.len) {
		case 0: case 1: case 2: return (MT_UNKNOWN);
		case 3: case 6: return (MT_FISU);
		case 4: case 7: return (MT_LSSU);
		case 5: case 8: return (MT_LSSU2);
		default: return (MT_MSU);
		}
	};
	bool mtp2Decoded(void) { return mtp2decode; };
	bool mtp3Decoded(void) { return mtp3decode; };
	int siValue(void) {
		if (!mtp2decode)
			return (-1);
		return (si);
	};
	int mpValue(void) {
		if (!mtp3decode)
			return (-1);
		return (mp);
	};
	enum ProtocolTest reasonableItutIsupMsg(void);
	enum ProtocolTest reasonableAnsiIsupMsg(void);
	enum ProtocolTest reasonableItutSnmmMsg(void);
	enum ProtocolTest reasonableAnsiSnmmMsg(void);
	enum ProtocolTest reasonableItutSntmMsg(void);
	enum ProtocolTest reasonableAnsiSntmMsg(void);
	enum ProtocolTest reasonableItutSnsmMsg(void);
	enum ProtocolTest reasonableAnsiSnsmMsg(void);
};

void
File::delall(void)
{
	list<Path *>::iterator it;
	while ((it = fpaths.begin()) != fpaths.end()) {
		Path *p = (*it);
		if (debug > 1)
			fprintf(stderr, "I: %s: deleting path %p\n", __PRETTY_FUNCTION__, p);
		delete p;
	}
}

unsigned long
MsgCount::siCounts(int si)
{
	unsigned long cnt = 0;
	for (counts_map::iterator it = counts.begin(); it != counts.end(); ++it) {
		if (it->first.si == si)
			cnt += it->second;
		else if (it->first.si > si)
			break;
	}
	return (cnt);
}
unsigned long
MsgCount::siMtCounts(int si, int mt)
{
	unsigned long cnt = 0;
	for (counts_map::iterator it = counts.begin(); it != counts.end(); ++it) {
		if (it->first.si == si && it->first.mt == mt)
			cnt += it->second;
		else if ((it->first.si > si) || (it->first.si == si && it->first.mt > mt))
			break;
	}
	return (cnt);
}
unsigned long
MsgCount::siMtMpCounts(int si, int mt, int mp)
{
	unsigned long cnt = 0;
	for (counts_map::iterator it = counts.begin(); it != counts.end(); ++it) {
		if (it->first.si == si && it->first.mt == mt && it->first.mp == mp)
			cnt += it->second;
		else if ((it->first.si > si) || (it->first.si == si && it->first.mt > mt))
			break;
	}
	return (cnt);
}
unsigned long
MsgCount::mpCounts(int mp)
{
	unsigned long cnt = 0;
	for (counts_map::iterator it = counts.begin(); it != counts.end(); ++it) {
		if (it->first.mp == mp)
			cnt += it->second;
	}
	return (cnt);
}

void
MsgCount::add(Message& msg)
{
	msgs++;
	switch (msg.messageType()) {
	case MT_UNKNOWN:
		bads++;
		break;
	case MT_FISU:
		fisus++;
		break;
	case MT_LSSU:
		lssus++;
		break;
	case MT_LSSU2:
		lssu2s++;
		break;
	case MT_MSU:
		msus++;
		if (msg.siValue() != -1)
			sis[msg.siValue()]++;
		else
			undec++;
		break;
	}
	if (msg.hdr.len > 200) {
		fprintf(stderr, "W: message length is %u\n", msg.hdr.len);
	}
	bytes += msg.hdr.len;
	if ((msg.hdr.ts.tv_sec < beg.tv_sec) ||
	   ((msg.hdr.ts.tv_sec == beg.tv_sec) && (msg.hdr.ts.tv_usec < beg.tv_usec)))
		beg = msg.hdr.ts;
	if ((msg.hdr.ts.tv_sec > end.tv_sec) ||
	   ((msg.hdr.ts.tv_sec == end.tv_sec) && (msg.hdr.ts.tv_usec > end.tv_usec)))
		end = msg.hdr.ts;
}
void
MsgCount::complete(Message &msg)
{
	if (msg.mpValue() != -1) {
		mps[msg.mpValue()]++;
		simps[msg.siValue()][msg.mpValue()]++;
	}
	if (msg.mtp3Decoded()) {
		struct mtypes m = { msg.si, msg.mt, msg.mp };
		counts[m]++;
	}
}
template<class _Count, class _Message>
void
Stat<_Count,_Message>::add(_Message& msg)
{
	_Count::add(msg);
	intervals[convert(msg.hdr.ts)].add(msg);
}
template<class _Count, class _Message>
void
Stat<_Count, _Message>::complete(_Message& msg)
{
	_Count::complete(msg);
	intervals[convert(msg.hdr.ts)].complete(msg);
}
void
File::add(Message& msg)
{
	MsgStat::add(msg);
}
void
Path::add(Message& msg)
{
	if (detecting()) {
		MsgStat::add(msg);
		messages.push_back(&msg);
		return;
	}
	complete(msg);
}
void
Path::complete(Message& msg)
{
	messages.push_back(&msg);
	do {
		Message *m = messages.front();
		messages.pop_front();
		m->decode(); // redecode after detection
		MsgStat::complete(*m);
		if (m->isMsu() && m->mtp3Decoded()) {
			orig.insert(m->opc);
			dest.insert(m->dpc);
			nodes[msg.opc].add_orig(*m);
			nodes[msg.dpc].add_dest(*m);
		}
		delete m;
	} while (!messages.empty());
}
void
Node::add_orig(Message& msg)
{
	if (pcode == 0)
		pcode = msg.opc;
	orig.add(msg);
	orig.complete(msg);
	if (msg.mtp3Decoded()) {
		switch (msg.si) {
		case 0x01: // sntm
		case 0x02: // snsm
			switch (msg.mt) {
			case 0x11: // sltm
			case 0x12: // slta
				exch_sltm = true;
				adjacent.insert(msg.dpc);
				break;
			}
			break;
		case 0x03: // sccp
			switch (msg.mt) {
			case 0x09: // udt
			case 0x11: // xudt
			case 0x13: // ludt
				orig_tcap = true;
				queries.insert(msg.dpc);
				break;
			}
			break;
		case 0x05: // isup
			switch (msg.mt) {
			case 0x10: // relc
				orig_isup = true;
				circuits.insert(msg.dpc);
				break;
			}
		}
	}
}
void
Node::add_dest(Message& msg)
{
	if (pcode == 0)
		pcode = msg.dpc;
	dest.add(msg);
	dest.complete(msg);
	if (msg.mtp3Decoded()) {
		switch (msg.si) {
		case 0x01: // sntm
		case 0x02: // snsm
			switch (msg.mt) {
			case 0x11: // sltm
			case 0x12: // slta
				exch_sltm = true;
				adjacent.insert(msg.opc);
				break;
			}
			break;
		case 0x03: // sccp
			switch (msg.mt) {
			case 0x09: // udt
			case 0x11: // xudt
			case 0x13: // ludt
				term_tcap = true;
				responds.insert(msg.opc);
				break;
			}
			break;
		case 0x05: // isup
			switch (msg.mt) {
			case 0x10: // rlc
				term_isup = true;
				circuits.insert(msg.opc);
				break;
			}
		}
	}
}

const char *
si_string(int si)
{
	switch (si) {
	case 0: return ("snmm");
	case 1: return ("sntm");
	case 2: return ("snsm");
	case 3: return ("sccp");
	case 4: return ("tup ");
	case 5: return ("isup");
	case 6: return ("dup1");
	case 7: return ("dup2");
	case 8: return ("mtup");
	case 9: return ("bsup");
	case 10: return ("siup");
	case 11: return ("spep");
	case 12: return ("stc ");
	case 13: return ("si13");
	case 14: return ("si14");
	case 15: return ("si15");
	default: return ("????");
	}
}
const char *
mt_string(int si, int mt)
{
	switch (si) {
	case 0:
		switch (mt) {
		case 0x11: return ("coo ");
		case 0x12: return ("coa ");
		case 0x15: return ("cbd ");
		case 0x16: return ("cba ");
		case 0x21: return ("eco ");
		case 0x22: return ("eca ");
		case 0x31: return ("rct ");
		case 0x32: return ("tfc ");
		case 0x41: return ("tfp ");
		case 0x42: return ("tcp ");
		case 0x43: return ("tfr ");
		case 0x44: return ("tcr ");
		case 0x45: return ("tfa ");
		case 0x46: return ("tca ");
		case 0x51: return ("rst ");
		case 0x52: return ("rsr ");
		case 0x53: return ("rcp ");
		case 0x54: return ("rcr ");
		case 0x61: return ("lin ");
		case 0x62: return ("lun ");
		case 0x63: return ("lia ");
		case 0x64: return ("lua ");
		case 0x65: return ("lid ");
		case 0x66: return ("lfu ");
		case 0x67: return ("llt ");
		case 0x68: return ("lrt ");
		case 0x71: return ("tra ");
		case 0x72: return ("trw ");
		case 0x81: return ("dlc ");
		case 0x82: return ("css ");
		case 0x83: return ("cns ");
		case 0x84: return ("cnp ");
		case 0xa1: return ("upu ");
		case 0xa2: return ("upa ");
		case 0xa3: return ("upt ");
		default: return ("????");
		}
	case 1:
		switch (mt) {
		case 0x11: return ("sltm");
		case 0x12: return ("slta");
		default: return ("????");
		}
	case 2:
		switch (mt) {
		case 0x11: return ("sltm");
		case 0x12: return ("slta");
		default: return ("????");
		}
	case 3:
		switch (mt) {
		case 0x01: return ("cr  ");
		case 0x02: return ("cc  ");
		case 0x03: return ("cref");
		case 0x04: return ("rlsd");
		case 0x05: return ("rlc ");
		case 0x06: return ("dt1 ");
		case 0x07: return ("dt2 ");
		case 0x08: return ("ak  ");
		case 0x09: return ("udt ");
		case 0x0a: return ("udts");
		case 0x0b: return ("ed  ");
		case 0x0c: return ("ea  ");
		case 0x0d: return ("rsr ");
		case 0x0e: return ("rsc ");
		case 0x0f: return ("err ");
		case 0x10: return ("it  ");
		case 0x11: return ("xudt");
		case 0x12: return ("xuds");
		case 0x13: return ("ludt");
		case 0x14: return ("luds");
		default: return ("????");
		}
	case 4:
		return ("????");
	case 5:
		switch (mt) {
		case 0x01: return ("iam ");
		case 0x02: return ("sam ");
		case 0x03: return ("inr ");
		case 0x04: return ("inf ");
		case 0x05: return ("cot ");
		case 0x06: return ("acm ");
		case 0x07: return ("con ");
		case 0x08: return ("fot ");
		case 0x09: return ("anm ");
		case 0x0c: return ("rel ");
		case 0x0d: return ("sus ");
		case 0x0e: return ("res ");
		case 0x10: return ("rlc ");
		case 0x11: return ("ccr ");
		case 0x12: return ("rsc ");
		case 0x13: return ("blo ");
		case 0x14: return ("ubl ");
		case 0x15: return ("bla ");
		case 0x16: return ("uba ");
		case 0x17: return ("grs ");
		case 0x18: return ("cgb ");
		case 0x19: return ("cgu ");
		case 0x1a: return ("cgba");
		case 0x1b: return ("cgua");
		case 0x1c: return ("cmr ");
		case 0x1d: return ("cmc ");
		case 0x1e: return ("cmrj");
		case 0x1f: return ("far ");
		case 0x20: return ("faa ");
		case 0x21: return ("frj ");
		case 0x22: return ("fad ");
		case 0x23: return ("fai ");
		case 0x24: return ("lpa ");
		case 0x25: return ("csvq");
		case 0x26: return ("csvr");
		case 0x27: return ("drs ");
		case 0x28: return ("pam ");
		case 0x29: return ("gra ");
		case 0x2a: return ("cqm ");
		case 0x2b: return ("cqr ");
		case 0x2c: return ("cpg ");
		case 0x2d: return ("usr ");
		case 0x2e: return ("ucic");
		case 0x2f: return ("cfn ");
		case 0x30: return ("olm ");
		case 0x31: return ("crg ");
		case 0x32: return ("nrm ");
		case 0x33: return ("fac ");
		case 0x34: return ("upt ");
		case 0x35: return ("upa ");
		case 0x36: return ("idr ");
		case 0x37: return ("irs ");
		case 0x38: return ("sgm ");
		case 0xe9: return ("cra ");
		case 0xea: return ("crm ");
		case 0xeb: return ("cvr ");
		case 0xec: return ("cvt ");
		case 0xed: return ("exm ");
		case 0xf8: return ("non ");
		case 0xfc: return ("llm ");
		case 0xfd: return ("cak ");
		case 0xfe: return ("tcm ");
		case 0xff: return ("mcp ");
		default: return ("????");
		}
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	default:
		return ("????");
	}
}
int
mp_mask(int si, int mt)
{
	switch (si) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
		return (0xf);
	case 5:
		switch (mt) {
		case 0x01: return ((1<<0)|(1<<1)); // iam
		case 0x02: return (0x0); // sam
		case 0x03: return ((1<<1)); // inr
		case 0x04: return ((1<<1)); // inf
		case 0x05: return ((1<<1)); // cot
		case 0x06: return ((1<<1)); // acm
		case 0x07: return (0x0); // con
		case 0x08: return ((1<<1)); // fot
		case 0x09: return ((1<<2)); // anm
		case 0x0c: return ((1<<1)); // rel
		case 0x0d: return ((1<<1)); // sus
		case 0x0e: return ((1<<1)); // res
		case 0x10: return ((1<<2)); // rlc
		case 0x11: return ((1<<1)); // ccr
		case 0x12: return ((1<<0)); // rsc
		case 0x13: return ((1<<0)); // blo
		case 0x14: return ((1<<0)); // ubl
		case 0x15: return ((1<<0)); // bla
		case 0x16: return ((1<<0)); // uba
		case 0x17: return ((1<<0)); // grs
		case 0x18: return ((1<<0)); // cgb
		case 0x19: return ((1<<0)); // cgu
		case 0x1a: return ((1<<0)); // cgba
		case 0x1b: return ((1<<0)); // cgua
		case 0x1c: return (0x00); // cmr
		case 0x1d: return (0x00); // cmc
		case 0x1e: return (0x00); // cmrj
		case 0x1f: return (0x00); // far
		case 0x20: return (0x00); // faa
		case 0x21: return (0x00); // frj
		case 0x22: return (0x00); // fad
		case 0x23: return (0x00); // fai
		case 0x24: return ((1<<1)); // lpa
		case 0x25: return (0x00); // csvq
		case 0x26: return (0x00); // csvr
		case 0x27: return (0x00); // drs
		case 0x28: return ((1<<2)|(1<<1)|(1<<0)); // pam
		case 0x29: return ((1<<0)); // gra
		case 0x2a: return ((1<<0)); // cqm
		case 0x2b: return ((1<<0)); // cqr
		case 0x2c: return ((1<<1)); // cpg
		case 0x2d: return (0x00); // usr
		case 0x2e: return ((1<<1)); // ucic
		case 0x2f: return ((1<<0)); // cfn
		case 0x30: return (0x00); // olm
		case 0x31: return (0x00); // crg
		case 0x32: return (0x00); // nrm
		case 0x33: return ((1<<2)|(1<<1)|(1<<0)); // fac
		case 0x34: return (0x00); // upt
		case 0x35: return (0x00); // upa
		case 0x36: return (0x00); // idr
		case 0x37: return (0x00); // irs
		case 0x38: return ((1<<0)); // sgm
		case 0xe9: return ((1<<0)); // cra
		case 0xea: return ((1<<0)); // crm
		case 0xeb: return ((1<<0)); // cvr
		case 0xec: return ((1<<0)); // cvt
		case 0xed: return ((1<<2)|(1<<1)|(1<<0)); // exm
		case 0xf8: return (0x00); // non
		case 0xfc: return (0x00); // llm
		case 0xfd: return (0x00); // cak
		case 0xfe: return (0x00); // tcm
		case 0xff: return (0x00); // mcp
		};
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:
	default:
		return (0xf);
	}
}

void
MsgCount::identify(void)
{
	fprintf(stdout, "- - - - - - - - - - - - - - - - - - - - - -\n");
	if (beg.tv_sec != 0x7fffffff || beg.tv_usec != 0x7fffffff) {
		fprintf(stdout, "Timestamp beg %012ld.%06ld\n", beg.tv_sec, beg.tv_usec);
	}
	if (end.tv_sec != 0 || end.tv_usec != 0) {
		fprintf(stdout, "Timestamp end %012ld.%06ld\n", end.tv_sec, end.tv_usec);
	}
	if ((beg.tv_sec != 0x7fffffff || beg.tv_usec != 0x7fffffff) &&
	    (end.tv_sec != 0 || end.tv_usec != 0)) {
		struct timeval dur;
		dur.tv_sec = end.tv_sec - beg.tv_sec;
		dur.tv_usec = end.tv_usec - beg.tv_usec;
		if (dur.tv_usec < 0) {
			dur.tv_usec += 1000000;
			dur.tv_sec -= 1;
		}
		fprintf(stdout, "Timestamp dur %012ld.%06ld\n", dur.tv_sec, dur.tv_usec);
	}
	fprintf(stdout, "Messages count %lu\n", msgs);
	fprintf(stdout, "Messages bytes %lu\n", bytes);
	if (msgs > 0) {
		double avglen = (double) bytes/ (double) msgs;
		fprintf(stdout, "Messages length %g bytes (avg)\n", avglen);
	}
	fprintf(stdout, "Messages bad   %lu\n", bads);
	fprintf(stdout, "Messages fisu  %lu\n", fisus);
	fprintf(stdout, "Messages lssu  %lu\n", lssus);
	fprintf(stdout, "Messages lssu2 %lu\n", lssu2s);
	fprintf(stdout, "Messages msu   %lu\n", msus);
	if (mps[0]) fprintf(stdout, "         mp 0  %lu\n", mps[0]);
	if (mps[1]) fprintf(stdout, "         mp 1  %lu\n", mps[1]);
	if (mps[2]) fprintf(stdout, "         mp 2  %lu\n", mps[2]);
	if (mps[3]) fprintf(stdout, "         mp 3  %lu\n", mps[3]);
	if (sis[0]) fprintf(stdout, "         snmm  %lu\n", sis[0]);
	if (simps[0][0]) fprintf(stdout, "    mp 0 snmm  %lu\n", simps[0][0]);
	if (simps[0][1]) fprintf(stdout, "    mp 1 snmm  %lu\n", simps[0][1]);
	if (simps[0][2]) fprintf(stdout, "    mp 2 snmm  %lu\n", simps[0][2]);
	if (simps[0][3]) fprintf(stdout, "    mp 3 snmm  %lu\n", simps[0][3]);
	if (sis[1]) fprintf(stdout, "         sntm  %lu\n", sis[1]);
	if (simps[1][0]) fprintf(stdout, "    mp 0 sntm  %lu\n", simps[1][0]);
	if (simps[1][1]) fprintf(stdout, "    mp 1 sntm  %lu\n", simps[1][1]);
	if (simps[1][2]) fprintf(stdout, "    mp 2 sntm  %lu\n", simps[1][2]);
	if (simps[1][3]) fprintf(stdout, "    mp 3 sntm  %lu\n", simps[1][3]);
	if (sis[2]) fprintf(stdout, "         snsm  %lu\n", sis[2]);
	if (simps[2][0]) fprintf(stdout, "    mp 0 snsm  %lu\n", simps[2][0]);
	if (simps[2][1]) fprintf(stdout, "    mp 1 snsm  %lu\n", simps[2][1]);
	if (simps[2][2]) fprintf(stdout, "    mp 2 snsm  %lu\n", simps[2][2]);
	if (simps[2][3]) fprintf(stdout, "    mp 3 snsm  %lu\n", simps[2][3]);
	if (sis[3]) fprintf(stdout, "         sccp  %lu\n", sis[3]);
	if (simps[3][0]) fprintf(stdout, "    mp 0 sccp  %lu\n", simps[3][0]);
	if (simps[3][1]) fprintf(stdout, "    mp 1 sccp  %lu\n", simps[3][1]);
	if (simps[3][2]) fprintf(stdout, "    mp 2 sccp  %lu\n", simps[3][2]);
	if (simps[3][3]) fprintf(stdout, "    mp 3 sccp  %lu\n", simps[3][3]);
	if (sis[4]) fprintf(stdout, "         tup   %lu\n", sis[4]);
	if (simps[4][0]) fprintf(stdout, "    mp 0 tup   %lu\n", simps[4][0]);
	if (simps[4][1]) fprintf(stdout, "    mp 1 tup   %lu\n", simps[4][1]);
	if (simps[4][2]) fprintf(stdout, "    mp 2 tup   %lu\n", simps[4][2]);
	if (simps[4][3]) fprintf(stdout, "    mp 3 tup   %lu\n", simps[4][3]);
	if (sis[5]) fprintf(stdout, "         isup  %lu\n", sis[5]);
	if (simps[5][0]) fprintf(stdout, "    mp 0 isup  %lu\n", simps[5][0]);
	if (simps[5][1]) fprintf(stdout, "    mp 1 isup  %lu\n", simps[5][1]);
	if (simps[5][2]) fprintf(stdout, "    mp 2 isup  %lu\n", simps[5][2]);
	if (simps[5][3]) fprintf(stdout, "    mp 3 isup  %lu\n", simps[5][3]);
	if (sis[6]) fprintf(stdout, "         dup1  %lu\n", sis[6]);
	if (simps[6][0]) fprintf(stdout, "    mp 0 dup1  %lu\n", simps[6][0]);
	if (simps[6][1]) fprintf(stdout, "    mp 1 dup1  %lu\n", simps[6][1]);
	if (simps[6][2]) fprintf(stdout, "    mp 2 dup1  %lu\n", simps[6][2]);
	if (simps[6][3]) fprintf(stdout, "    mp 3 dup1  %lu\n", simps[6][3]);
	if (sis[7]) fprintf(stdout, "         dup2  %lu\n", sis[7]);
	if (simps[7][0]) fprintf(stdout, "    mp 0 dup2  %lu\n", simps[7][0]);
	if (simps[7][1]) fprintf(stdout, "    mp 1 dup2  %lu\n", simps[7][1]);
	if (simps[7][2]) fprintf(stdout, "    mp 2 dup2  %lu\n", simps[7][2]);
	if (simps[7][3]) fprintf(stdout, "    mp 3 dup2  %lu\n", simps[7][3]);
	if (sis[8]) fprintf(stdout, "         mtup  %lu\n", sis[8]);
	if (simps[8][0]) fprintf(stdout, "    mp 0 mtup  %lu\n", simps[8][0]);
	if (simps[8][1]) fprintf(stdout, "    mp 1 mtup  %lu\n", simps[8][1]);
	if (simps[8][2]) fprintf(stdout, "    mp 2 mtup  %lu\n", simps[8][2]);
	if (simps[8][3]) fprintf(stdout, "    mp 3 mtup  %lu\n", simps[8][3]);
	if (sis[9]) fprintf(stdout, "         bisup %lu\n", sis[9]);
	if (simps[9][0]) fprintf(stdout, "    mp 0 bisup %lu\n", simps[9][0]);
	if (simps[9][1]) fprintf(stdout, "    mp 1 bisup %lu\n", simps[9][1]);
	if (simps[9][2]) fprintf(stdout, "    mp 2 bisup %lu\n", simps[9][2]);
	if (simps[9][3]) fprintf(stdout, "    mp 3 bisup %lu\n", simps[9][3]);
	if (sis[10]) fprintf(stdout, "         siup  %lu\n", sis[10]);
	if (simps[10][0]) fprintf(stdout, "    mp 0 siup  %lu\n", simps[10][0]);
	if (simps[10][1]) fprintf(stdout, "    mp 1 siup  %lu\n", simps[10][1]);
	if (simps[10][2]) fprintf(stdout, "    mp 2 siup  %lu\n", simps[10][2]);
	if (simps[10][3]) fprintf(stdout, "    mp 3 siup  %lu\n", simps[10][3]);
	if (sis[11]) fprintf(stdout, "         spnep %lu\n", sis[11]);
	if (simps[11][0]) fprintf(stdout, "    mp 0 spnep %lu\n", simps[11][0]);
	if (simps[11][1]) fprintf(stdout, "    mp 1 spnep %lu\n", simps[11][1]);
	if (simps[11][2]) fprintf(stdout, "    mp 2 spnep %lu\n", simps[11][2]);
	if (simps[11][3]) fprintf(stdout, "    mp 3 spnep %lu\n", simps[11][3]);
	if (sis[12]) fprintf(stdout, "         stc   %lu\n", sis[12]);
	if (simps[12][0]) fprintf(stdout, "    mp 0 stc   %lu\n", simps[12][0]);
	if (simps[12][1]) fprintf(stdout, "    mp 1 stc   %lu\n", simps[12][1]);
	if (simps[12][2]) fprintf(stdout, "    mp 2 stc   %lu\n", simps[12][2]);
	if (simps[12][3]) fprintf(stdout, "    mp 3 stc   %lu\n", simps[12][3]);
	if (sis[13]) fprintf(stdout, "         si13  %lu\n", sis[13]);
	if (simps[13][0]) fprintf(stdout, "    mp 0 si13  %lu\n", simps[13][0]);
	if (simps[13][1]) fprintf(stdout, "    mp 1 si13  %lu\n", simps[13][1]);
	if (simps[13][2]) fprintf(stdout, "    mp 2 si13  %lu\n", simps[13][2]);
	if (simps[13][3]) fprintf(stdout, "    mp 3 si13  %lu\n", simps[13][3]);
	if (sis[14]) fprintf(stdout, "         si14  %lu\n", sis[14]);
	if (simps[14][0]) fprintf(stdout, "    mp 0 si14  %lu\n", simps[14][0]);
	if (simps[14][1]) fprintf(stdout, "    mp 1 si14  %lu\n", simps[14][1]);
	if (simps[14][2]) fprintf(stdout, "    mp 2 si14  %lu\n", simps[14][2]);
	if (simps[14][3]) fprintf(stdout, "    mp 3 si14  %lu\n", simps[14][3]);
	if (sis[15]) fprintf(stdout, "         si15  %lu\n", sis[15]);
	if (simps[15][0]) fprintf(stdout, "    mp 0 si15  %lu\n", simps[15][0]);
	if (simps[15][1]) fprintf(stdout, "    mp 1 si15  %lu\n", simps[15][1]);
	if (simps[15][2]) fprintf(stdout, "    mp 2 si15  %lu\n", simps[15][2]);
	if (simps[15][3]) fprintf(stdout, "    mp 3 si15  %lu\n", simps[15][3]);
	if (undec) fprintf(stdout, "         undec %lu\n", undec);

	map<struct mtypes,unsigned long,mt_ltstr>::iterator it;
	
	int si = -1, mt = -1;
	unsigned long to_cnt = 0, si_cnt = 0, mt_cnt = 0, to_num = 0, si_num = 0, mt_num = 0;
	unsigned long mp_cnt[4] = { 0, 0, 0, 0 };
	for (it = counts.begin(); it != counts.end(); ++it) {
		to_cnt += it->second;
		to_num += 1;
		if (si == it->first.si && mt == it->first.mt) {
			mt_cnt += it->second;
			mt_num += 1;
			si_cnt += it->second;
			si_num += 1;
		} else {
			if (mt != -1 && mt_num > 1) {
				if (debug && it->first.mt < mt && it->first.si == si)
					fprintf(stdout, "v_misordered mt si = %02x, cur = %02x, new = %02x\n", si, mt, it->first.mt);
				// print mt_cnt
				fprintf(stdout, "%s %s ---- %6lu\n", si_string(si), mt_string(si, mt), mt_cnt);
			}
			mt = it->first.mt;
			mt_cnt = it->second;
			mt_num = 1;
			if (si == it->first.si) { // mt is different
				si_cnt += it->second;
				si_num += 1;
			} else { // si is different
				if (si != -1 && si_num > 1) {
					if (debug && it->first.si < si)
						fprintf(stdout, "v_misordered si cur = %02x, new = %02x\n", si, it->first.si);
					// print si_cnt
					fprintf(stdout, "%s ---- ---- %6lu\n", si_string(si), si_cnt);
				}
				si = it->first.si;
				si_cnt = it->second;
				si_num = 1;
			}
		}
		mp_cnt[it->first.mp] += it->second;
		// print mp_cnt
		fprintf(stdout, "%s %s mp %d %6lu", si_string(si), mt_string(si, mt), it->first.mp, it->second);
		int mask;
		if (!((mask = mp_mask(si, mt)) & (1<<it->first.mp))) {
			fprintf(stdout, " <-- invalid");
			if (mask != 0) {
				fprintf(stdout, ", should be ");
				for (int i = 0; i < 3; i++) {
					if (mask & (1<<i)) {
						mask &= ~(1<<i);
						if (mask)
							fprintf(stdout, "%d|", i);
						else
							fprintf(stdout, "%d", i);
					}
				}
			}
		}
		fprintf(stdout, "\n");
	}
	if (mt != -1 && mt_num > 1) {
		// print mt_cnt
		fprintf(stdout, "%s %s ---- %6lu\n", si_string(si), mt_string(si, mt), mt_cnt);
	}
	if (si != -1 && si_num > 1) {
		// print si_cnt
		fprintf(stdout, "%s ---- ---- %6lu\n", si_string(si), si_cnt);
	}
	// print to_cnt
	if (to_num > 1)
		fprintf(stdout, "---- ---- ---- %6lu\n", to_cnt);
	for (int i = 0; i < 4; i++) {
		if (mp_cnt[i])
			fprintf(stdout, "---- ---- mp %d %6lu\n", i, mp_cnt[i]);
	}
}
template<class _Count, class _Message>
void
Stat<_Count,_Message>::identify(void)
{
	_Count::identify();
	if (output > 1) {
		typename map<unsigned long,_Count,itv_ltstr>::iterator it;
		for (it = intervals.begin(); it != intervals.end(); ++it)
			it->second.identify();
	}
}
struct pcode {
	int ntw;
	int cls;
	int mem;
};
struct pcode
pcode_convert(int pc)
{
	struct pcode p;
	if (pc & ~0x3fff) {
		p.ntw = (pc >> 16) & 0xff;
		p.cls = (pc >>  8) & 0xff;
		p.mem = (pc >>  0) & 0xff;
	} else {
		p.ntw = (pc >> 11) & 0x07;
		p.cls = (pc >>  3) & 0xff;
		p.mem = (pc >>  0) & 0x07;
	}
	return (p);
}
void
Node::identify(void)
{
	fprintf(stdout, "-------------------------------------------\n");
	struct pcode pc = pcode_convert(pcode);
	fprintf(stdout, "Node (%d-%d-%d):\n", pc.ntw, pc.cls, pc.mem);
	if (orig_tcap)
		fprintf(stdout, "Node originates TCAP traffic.\n");
	if (term_tcap)
		fprintf(stdout, "Node terminates TCAP traffic.\n");
	if (orig_isup)
		fprintf(stdout, "Node originates ISUP traffic.\n");
	if (term_isup)
		fprintf(stdout, "Node terminates ISUP traffic.\n");
	if (exch_sltm)
		fprintf(stdout, "Node exchanges SLTM/SLTA traffic.\n");
	set<int>::iterator it;
	const char *locale = exch_sltm ? "adjacent" : "distant";
	if (!orig_tcap && term_tcap && !orig_isup && !term_isup) {
		// GTT functions never originate, just terminate
		fprintf(stdout, "Node is %s STP with GTT function (or capablity code).\n", locale);
		for (it = responds.begin(); it != responds.end(); ++it) {
			struct pcode pc = pcode_convert(*it);
			fprintf(stdout, "Node (%d-%d-%d) translates through this node.\n", pc.ntw, pc.cls, pc.mem);
		}
	}
	if (orig_tcap && !term_tcap && !orig_isup && !term_isup) {
		// SCP's originate but never terminate
		fprintf(stdout, "Node is %s SCP behind STP GTT.\n", locale);
		for (it = queries.begin(); it != queries.end(); ++it) {
			struct pcode pc = pcode_convert(*it);
			fprintf(stdout, "Node (%d-%d-%d) is responded to by this node.\n", pc.ntw, pc.cls, pc.mem);
		}
	}
	if (orig_tcap && term_tcap && !orig_isup && !term_isup) {
		// SCP's have no ISUP traffic
		fprintf(stdout, "Node is local SCCP transaction alias.\n");
		for (it = queries.begin(); it != queries.end(); ++it) {
			struct pcode pc = pcode_convert(*it);
			fprintf(stdout, "Node (%d-%d-%d) is queried by this node.\n", pc.ntw, pc.cls, pc.mem);
		}
		for (it = responds.begin(); it != responds.end(); ++it) {
			struct pcode pc = pcode_convert(*it);
			fprintf(stdout, "Node (%d-%d-%d) repsonds to this node.\n", pc.ntw, pc.cls, pc.mem);
		}
	}
	if (orig_tcap && term_tcap && (orig_isup || term_isup)) {
		// SSPs originate and terminate all
		fprintf(stdout, "Node is %s SSP with TCAP (or local alias).\n", locale);
		for (it = circuits.begin(); it != circuits.end(); ++it) {
			struct pcode pc = pcode_convert(*it);
			fprintf(stdout, "Node (%d-%d-%d) has circuits to this node.\n", pc.ntw, pc.cls, pc.mem);
		}
		for (it = queries.begin(); it != queries.end(); ++it) {
			struct pcode pc = pcode_convert(*it);
			fprintf(stdout, "Node (%d-%d-%d) is queried by this node.\n", pc.ntw, pc.cls, pc.mem);
		}
		for (it = responds.begin(); it != responds.end(); ++it) {
			struct pcode pc = pcode_convert(*it);
			fprintf(stdout, "Node (%d-%d-%d) responds to this node.\n", pc.ntw, pc.cls, pc.mem);
		}
	}
	if (!(orig_tcap || term_tcap) && (orig_isup || term_isup)) {
		fprintf(stdout, "Node is %s SSP w/o TCAP.\n", locale);
		for (it = circuits.begin(); it != circuits.end(); ++it) {
			struct pcode pc = pcode_convert(*it);
			fprintf(stdout, "Node (%d-%d-%d) has circuits to this node.\n", pc.ntw, pc.cls, pc.mem);
		}
	}
	if (!(orig_tcap || term_tcap || orig_isup || term_isup) && exch_sltm) {
		fprintf(stdout, "Node is %s STP w/o GTT (or separate capility code).\n", locale);
	}
	for (it = adjacent.begin(); it != adjacent.end(); ++it) {
		struct pcode pc = pcode_convert(*it);
		fprintf(stdout, "Node (%d-%d-%d) is adjacent to this node.\n", pc.ntw, pc.cls, pc.mem);
	}

	fprintf(stdout, "-------------------------------------------\n");
	fprintf(stdout, "Origination (%d-%d-%d):\n", pc.ntw, pc.cls, pc.mem);
	orig.identify();
	fprintf(stdout, "-------------------------------------------\n");
	fprintf(stdout, "Destination (%d-%d-%d):\n", pc.ntw, pc.cls, pc.mem);
	dest.identify();
}
void
Path::identify(void)
{
	fprintf(stdout, "-------------------------------------------\n");
	fprintf(stdout, "Path (%d:%d:%d):\n", cardNum(), spanNum(), slotNum());
	set<int>::iterator it;
	struct pcode pc;
	fprintf(stdout, "-------------------------------------------\n");
	fprintf(stdout, "Originating nodes:\n");
	for (it = orig.begin(); it != orig.end(); ++it) {
		pc = pcode_convert(*it);
		fprintf(stdout, "Node (%d-%d-%d)\n", pc.ntw, pc.cls, pc.mem);
	}
	fprintf(stdout, "-------------------------------------------\n");
	fprintf(stdout, "Terminating nodes:\n");
	for (it = dest.begin(); it != dest.end(); ++it) {
		pc = pcode_convert(*it);
		fprintf(stdout, "Node (%d-%d-%d)\n", pc.ntw, pc.cls, pc.mem);
	}
	fprintf(stdout, "-------------------------------------------\n");
	MsgStat::identify();
}
void
File::identify(void)
{
	this->readmsgs();
	MsgStat::identify();
	for (list<Path *>::iterator it = fpaths.begin(); it != fpaths.end(); ++it) {
		(*it)->identify();
	}
}
void
FileText::identify(void)
{
	fprintf(stdout, "===========================================\n");
	fprintf(stdout, "File %s is a text file\n", fname);
	fprintf(stdout, "Comment lines %u\n", coms);
	File::identify();
	fprintf(stdout, "===========================================\n");
}
void
FilePcap::identify(void)
{
	fprintf(stdout, "===========================================\n");
	fprintf(stdout, "File %s is a pcap file\n", fname);
	fprintf(stdout, "Data link type %d\n", getDLT());
	fprintf(stdout, "Data link name %s\n", getDLTName());
	fprintf(stdout, "Data link desc %s\n", getDLTDesc());
	File::identify();
	fprintf(stdout, "===========================================\n");
}

Message *
FileText::readmsg(void)
{
	Message *m = NULL;
	size_t len, len1, len2;
	char *s;

	for (; (s = fgets(buf, 1024, fp)) != NULL; line++) {
		len = strnlen(buf, 1024);
		if (len < 22) {
			// aborted files can have short lines
			if (debug > 0) {
				fprintf(stderr, "W: syntax error line %d: %s\n", line, buf);
				fprintf(stderr, "W: line too short: len = %zu\n", len);
			}
			continue;
		}
		// note buf includes the \n (unless file was aborted)
		if (buf[len-1] != '\n') {
			// aborted files can have lines with no termination
			if (debug > 0) {
				fprintf(stderr, "W: syntax error line %d: %s\n", line, buf);
				fprintf(stderr, "W: no line termination\n");
			}
			continue;
		}
		buf[len-1] = '\0';
		if (buf[19] != ' ') {
			fprintf(stderr, "E: syntax error line %d: %s", line, buf);
			fprintf(stderr, "E: character position 19 is '%c' not ' '\n", buf[19]);
			continue;
		}
		if (buf[20] == '#') {
			// comment line
			coms++;
			if (strncmp("device:", &buf[22], 7) == 0) {
				int card, span, slot;
				if (sscanf(&buf[44], "(%d:%d:%d)", &card, &span, &slot) == 3) {
					ppa = ((card & 0x0f) << 12) |
						((span & 0x0f) << 8) |
						((slot & 0xff) << 0);
					phdr.lkno[0] = (ppa >> 8) & 0xff;
					phdr.lkno[1] = (ppa >> 0) & 0xff;
				}
			}
			continue;
		}
		len2 = strnlen(&buf[20], 1024-20);
		if (len2 & 0x01) {
			fprintf(stderr, "E: syntax error line %d: %s", line, buf);
			fprintf(stderr, "E: message length %zu is odd\n", len2);
			continue;
		}
		if ((len1 = strspn(&buf[20], "0123456789abcdefABCDEF")) != len2) {
			fprintf(stderr, "E: syntax error line %d: %s", line, buf);
			fprintf(stderr, "E: len1 = %zu, len2 = %zu\n", len1, len2);
			continue;
		}
		struct pcap_pkthdr hdr;
		unsigned char *dat;
		sscanf(buf, "%ld.%ld", &hdr.ts.tv_sec, &hdr.ts.tv_usec);
		hdr.caplen = hdr.len = strnlen(&buf[20], 1024-20) / 2;
		if ((dat = (unsigned char *)malloc(hdr.len)) == NULL)
			throw Error(1);
		for (uint i = 0; i < hdr.len * 2; i+=2) {
			unsigned char val = 0;
			for (int j = 0; j < 2; j++, val<<=4) {
				unsigned char c = buf[20+i+j];
				if ('0' <= c && c <= '9')
					val += c - '0';
				else if ('a' <= c && c <= 'f')
					val += c - 'a' + 10;
				else if ('A' <= c && c <= 'F')
					val += c - 'A' + 10;
			}
			dat[i/2] = val;
		}
		Path &p = Path::get(*this, ppa);
		if ((m = new Message(p, &hdr, dat, &phdr)) == NULL)
			throw Error(1);
		m->decode();
		p.add(*m);
		break;
	}
	return (m);
}

Message *
FilePcap::readmsg(void)
{
	Message *m = NULL;
	int ret;
	struct pcap_pkthdr *pcap_hdr, hdr;
	struct pseudohdr *phdr;
	const u_char *dat;
        unsigned char *buf;
	int ppa;

	if ((ret = pcap_next_ex(pp, &pcap_hdr, &dat)) == 1) {
		phdr = (struct pseudohdr *)dat;
		dat += sizeof(*phdr);
		hdr = *pcap_hdr;
		hdr.caplen -= sizeof(*phdr);
		hdr.len -= sizeof(*phdr);
		ppa = (phdr->lkno[1] << 8) | (phdr->lkno[1] << 0);
		Path &p = Path::get(*this, ppa);
		if ((buf = (unsigned char *)malloc(hdr.len)) == NULL)
			throw Error(1);
		memcpy(buf, dat, hdr.len);
		if ((m = new Message(p, &hdr, buf, phdr)) == NULL)
			throw Error(1);
		m->decode();
		p.add(*m);
	}
	return (m);
}

void
File::readmsgs(void)
{
	while (this->readmsg() != NULL) ;
}

int
Message::decode(void)
{
	if (!mtp2decode) {
		if (3 <= hdr.len && hdr.len <= 5)
			path.setHeaderType(HT_BASIC);
		if (6 <= hdr.len && hdr.len <= 8)
			path.setHeaderType(HT_EXTENDED);
		switch (path.headerType()) {
		case HT_UNKNOWN:
			fprintf(stderr, "unknown header type\n");
			return (0);
		case HT_BASIC:
			// basic link header
			bsn = *mid & 0x7f;
			bib = (*mid++ & 0x80) >> 7;
			fsn = *mid & 0x7f;
			bib = (*mid++ & 0x80) >> 7;
			li = *mid & 0x3f;
			if ((li != hdr.len - 3) && (hdr.len - 3 <= 63 || li != 63))
				fprintf(stderr, "bad length indicator %d != %d\n", li, hdr.len - 3);
			li0 = (*mid++ & 0xc0) >> 6;
			if (li0 != 0) {
				path.setRoutingLabel(RL_24BIT_PC);
				path.setMsgPriority(MP_JAPAN);
			}
			break;
		case HT_EXTENDED:
			// annex a link header
			bsn = (*mid++ & 0xff);
			bsn |= ((*mid & 0x0f) << 8);
			bib = (*mid++ & 0x80) >> 7;
			fsn = (*mid++ & 0xff);
			fsn |= ((*mid & 0x0f) << 8);
			fib = (*mid++ & 0x80) >> 7;
			li = (*mid++ & 0xff);
			li |= ((*mid & 0x01) << 8);
			if ((li != hdr.len - 6) && (hdr.len - 6 <= 63 || li != 63))
				fprintf(stderr, "bad length indicator %d != %d\n", li, hdr.len - 6);
			li0 = (*mid++ & 0xc0) >> 6;
			if (li0 != 0) {
				path.setRoutingLabel(RL_24BIT_PC);
				path.setMsgPriority(MP_JAPAN);
			}
			break;
		}
		sio = mid;
		mid++;
		sif = mid;
		mtp2decode = true;
	}
	switch (li) {
	case 0:
		return (1);
	case 1:
		si = (*sio++ & 0xff);
		return (1);
	case 2:
		si = (*sio++ & 0xff);
		return (1);
	}
	si = *sio & 0x0f;
	ni = (*sio & 0xc0) >> 6;
	if (ni == 0) {
		path.setRoutingLabel(RL_14BIT_PC);
		path.setMsgPriority(MP_INTERNATIONAL);
	}
	switch (path.msgPriority()) {
	case MP_UNKNOWN:
		mp = (*sio & 0x30) >> 4;
		if (mp != 0 && path.routingLabel() == RL_24BIT_PC)
			path.setMsgPriority(MP_NATIONAL);
		break;
	case MP_JAPAN:
		mp = li0;
		break;
	case MP_NATIONAL:
		mp = (*sio & 0x30) >> 4;
		break;
	case MP_INTERNATIONAL:
		mp = 0;
		break;
	}
	if (li < 6) {
		fprintf(stderr, "too short for RL, li = %d", li);
		return (-1);
	}
	if (li < 9)
		path.setRoutingLabel(RL_14BIT_PC);
	else if (si == 0x05 && li < 11)
		path.setRoutingLabel(RL_14BIT_PC);
	switch (si) {
	case 0:		// snmm
		switch (reasonableAnsiSnmmMsg()) {
		case PT_YES:
			switch (reasonableItutSnmmMsg()) {
			case PT_YES:
				break;
			case PT_MAYBE:
			case PT_NO:
				if (debug && path.routingLabel() != RL_24BIT_PC) {
					fprintf(stderr, "I: setting 24-bit on snmm h1h0 %02x, li = %u\n", sif[7], li);
					fprintf(stderr, "I: snmm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_24BIT_PC);
				break;
			}
			break;
		case PT_MAYBE:
			switch (reasonableItutSnmmMsg()) {
			case PT_YES:
				if (debug && path.routingLabel() != RL_14BIT_PC) {
					fprintf(stderr, "I: setting 14-bit on snmm h1h0 0x%02x, li = %u\n", sif[4], li);
					fprintf(stderr, "I: snmm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_14BIT_PC);
				break;
			case PT_MAYBE:
				break;
			case PT_NO:
				if (debug && path.routingLabel() != RL_24BIT_PC) {
					fprintf(stderr, "I: setting 24-bit on snmm h1h0 0x%02x, li = %u\n", sif[7], li);
					fprintf(stderr, "I: snmm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_24BIT_PC);
				break;
			}
			break;
		case PT_NO:
			switch (reasonableItutSnmmMsg()) {
			case PT_YES:
			case PT_MAYBE:
				if (debug && path.routingLabel() != RL_14BIT_PC) {
					fprintf(stderr, "I: setting 14-bit on snmm h1h0 0x%02x, li = %u\n", sif[4], li);
					fprintf(stderr, "I: snmm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_14BIT_PC);
				break;
			case PT_NO:
				fprintf(stderr, "snmm decoding error\n");
				fprintf(stderr, "snmm message is: ");
				for (unsigned char *p = sif; p < end; p++)
					fprintf(stderr, "%02x", *p);
				fprintf(stderr, "\n");
				return (-1);
			}
			break;
		}
		break;
	case 1:		// sntm
		switch (reasonableAnsiSntmMsg()) {
		case PT_YES:
			switch (reasonableItutSntmMsg()) {
			case PT_YES:
				break;
			case PT_MAYBE:
			case PT_NO:
				if (debug && path.routingLabel() != RL_24BIT_PC) {
					fprintf(stderr, "I: setting 24-bit on sntm h1h0 %02x, li = %u\n", sif[7], li);
					fprintf(stderr, "I: sntm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_24BIT_PC);
				break;
			}
			break;
		case PT_MAYBE:
			switch (reasonableItutSntmMsg()) {
			case PT_YES:
				if (debug && path.routingLabel() != RL_14BIT_PC) {
					fprintf(stderr, "I: setting 14-bit on sntm h1h0 0x%02x, li = %u\n", sif[4], li);
					fprintf(stderr, "I: sntm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_14BIT_PC);
				break;
			case PT_MAYBE:
				break;
			case PT_NO:
				if (debug && path.routingLabel() != RL_24BIT_PC) {
					fprintf(stderr, "I: setting 24-bit on sntm h1h0 0x%02x, li = %u\n", sif[7], li);
					fprintf(stderr, "I: sntm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_24BIT_PC);
				break;
			}
			break;
		case PT_NO:
			switch (reasonableItutSntmMsg()) {
			case PT_YES:
			case PT_MAYBE:
				if (debug && path.routingLabel() != RL_14BIT_PC) {
					fprintf(stderr, "I: setting 14-bit on sntm h1h0 0x%02x, li = %u\n", sif[4], li);
					fprintf(stderr, "I: sntm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_14BIT_PC);
				break;
			case PT_NO:
				fprintf(stderr, "sntm decoding error\n");
				fprintf(stderr, "sntm message is: ");
				for (unsigned char *p = sif; p < end; p++)
					fprintf(stderr, "%02x", *p);
				fprintf(stderr, "\n");
				return (-1);
			}
			break;
		}
		break;
		break;
	case 2:		// snsm
		switch (reasonableAnsiSnsmMsg()) {
		case PT_YES:
			switch (reasonableItutSnsmMsg()) {
			case PT_YES:
				break;
			case PT_MAYBE:
			case PT_NO:
				if (debug && path.routingLabel() != RL_24BIT_PC) {
					fprintf(stderr, "I: setting 24-bit on snsm h1h0 %02x, li = %u\n", sif[7], li);
					fprintf(stderr, "I: snsm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_24BIT_PC);
				break;
			}
			break;
		case PT_MAYBE:
			switch (reasonableItutSnsmMsg()) {
			case PT_YES:
				if (debug && path.routingLabel() != RL_14BIT_PC) {
					fprintf(stderr, "I: setting 14-bit on snsm h1h0 0x%02x, li = %u\n", sif[4], li);
					fprintf(stderr, "I: snsm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_14BIT_PC);
				break;
			case PT_MAYBE:
				break;
			case PT_NO:
				if (debug && path.routingLabel() != RL_24BIT_PC) {
					fprintf(stderr, "I: setting 24-bit on snsm h1h0 0x%02x, li = %u\n", sif[7], li);
					fprintf(stderr, "I: snsm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_24BIT_PC);
				break;
			}
			break;
		case PT_NO:
			switch (reasonableItutSnsmMsg()) {
			case PT_YES:
			case PT_MAYBE:
				if (debug && path.routingLabel() != RL_14BIT_PC) {
					fprintf(stderr, "I: setting 14-bit on snsm h1h0 0x%02x, li = %u\n", sif[4], li);
					fprintf(stderr, "I: snsm message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_14BIT_PC);
				break;
			case PT_NO:
				fprintf(stderr, "snsm decoding error\n");
				fprintf(stderr, "snsm message is: ");
				for (unsigned char *p = sif; p < end; p++)
					fprintf(stderr, "%02x", *p);
				fprintf(stderr, "\n");
				return (-1);
			}
			break;
		}
		break;
	case 3:		// sccp
	case 4:		// tup
		break;
	case 5:		// isup
		switch (reasonableAnsiIsupMsg()) {
		case PT_YES:
			switch (reasonableItutIsupMsg()) {
			case PT_YES:
				break;
			case PT_MAYBE:
			case PT_NO:
				if (debug && path.routingLabel() != RL_24BIT_PC) {
					fprintf(stderr, "I: setting 24-bit on isup mt 0x%02x, li = %u\n", sif[9], li);
					fprintf(stderr, "I: isup message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_24BIT_PC);
				break;
			}
			break;
		case PT_MAYBE:
			switch (reasonableItutIsupMsg()) {
			case PT_YES:
				if (debug && path.routingLabel() != RL_14BIT_PC) {
					fprintf(stderr, "I: setting 14-bit on isup mt 0x%02x, li = %u\n", sif[6], li);
					fprintf(stderr, "I: isup message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_14BIT_PC);
				break;
			case PT_MAYBE:
				break;
			case PT_NO:
				if (debug && path.routingLabel() != RL_24BIT_PC) {
					fprintf(stderr, "I: setting 24-bit on isup mt 0x%02x, li = %u\n", sif[9], li);
					fprintf(stderr, "I: isup message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_24BIT_PC);
				break;
			}
			break;
		case PT_NO:
			switch (reasonableItutIsupMsg()) {
			case PT_YES:
			case PT_MAYBE:
				if (debug && path.routingLabel() != RL_14BIT_PC) {
					fprintf(stderr, "I: setting 14-bit on isup mt 0x%02x, li = %u\n", sif[6], li);
					fprintf(stderr, "I: isup message number %lu\n", path.sis[si] + 1);
					fprintf(stderr, "I: msu  message number %lu\n", path.msus + 1);
				}
				path.setRoutingLabel(RL_14BIT_PC);
				break;
			case PT_NO:
				fprintf(stderr, "isup decoding error\n");
				fprintf(stderr, "isup message is: ");
				for (unsigned char *p = sif; p < end; p++)
					fprintf(stderr, "%02x", *p);
				fprintf(stderr, "\n");
				return (-1);
			}
			break;
		}
		break;
	default:
		break;
	}
	if (!mtp3decode) {
		if (path.detecting())
			return (0);
		mid = sif;
		switch (path.routingLabel()) {
		case RL_UNKNOWN:
			return (0);	// cannot decode further, yet
		case RL_14BIT_PC:
			// SSSSOOOO OOOOOOOO OODDDDDD DDDDDDDD
			if (li < 6)
				fprintf(stderr, "too short for RL, li = %d\n", li);
			dpc = (*mid++ & 0xff);
			dpc |= ((*mid & 0x3f) << 8);
			opc = ((*mid++ & 0xc0) >> 6);
			opc |= ((*mid++ & 0xff) << 2);
			opc |= ((*mid & 0x0f) << 10);
			sls = (*mid++ * 0xf0) >> 4;
			break;
		case RL_24BIT_PC:
			if (li < 9)
				fprintf(stderr, "too short for RL, li = %d\n", li);
			dpc = (*mid++ & 0x0ff);
			dpc |= ((*mid++ & 0xff) << 8);
			dpc |= ((*mid++ & 0xff) << 16);
			opc = (*mid++ & 0x0ff);
			opc |= ((*mid++ & 0xff) << 8);
			opc |= ((*mid++ & 0xff) << 16);
			sls = *mid++ & 0xff;
			break;
		}
		sdu = mid;
		switch (si) {
		case 0:	// snmm
			h0 = (*mid & 0x0f);
			h1 = (*mid++ & 0xf0) >> 4;
			mt = ((h0 << 4) | h1);
			switch (mt) {
			case 0x11:	// coo (link)
			case 0x12:	// coa (link)
				return (-1);
			case 0x15:	// cbd
				// link specific
				if (path.routingLabel() == RL_14BIT_PC)
					slc = sls;
				else
					slc = (*mid++ & 0xf);
				mid++;
				//path.setLink(*this);
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
		case 1:	// sntm
		case 2:	// snsm
			h0 = (*mid & 0x0f);
			h1 = (*mid++ & 0xf0) >> 4;
			mt = ((h0 << 4) | h1);
			// check for SLTM/SLTA SSLTM/SSLTA message
			switch (mt) {
			case 0x12:	// (s)slta
				/* well.... */
			case 0x11:	// (s)sltm
				if (path.routingLabel() == RL_14BIT_PC)
					slc = sls;
				else
					slc = (*mid & 0x0f);
				dlen = (*mid++ & 0xf0) >> 4;
				//path.setLink(*this);
				break;
			}
			break;
		case 3:	// sccp
			mt = (*mid++ & 0xff);
			break;
		case 4:	// tup
			return (-1);	// unexpected (obsolete)
		case 5:	// isup
			cic = (*mid++ & 0xff);
			cic |= ((*mid++ & 0xff) << 8);
			mt = (*mid++ & 0xff);
			break;
		case 6:	// dup1
		case 7:	// dup2
			return (-1);	// unexpected (obsolete)
		case 8:	// mtup
			break;
		case 9:	// bisup
		case 10:	// siup
		case 11:	// spneup
		case 12:	// stc
		case 13:	// unused
		case 14:	// unused
		case 15:	// unused
			return (-1);	// unexpected
		}
		mtp3decode = true;
	}
	return (1);
}

enum ProtocolTest
Message::reasonableItutSnmmMsg(void)
{
	size_t size = end - sif;

	switch (size) {
	case 5:		// trx or slm message
		switch (sif[4]) {	// ITU H0H1 byte
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
			return (PT_YES);
		}
		return (PT_NO);
	case 6:		// com or cbm message
		switch (sif[4]) {	// ITU H0H1 byte
		case 0x11:	// coo
		case 0x21:	// coa
		case 0x51:	// cbd
		case 0x61:	// cba
			return (PT_YES);
		}
		return (PT_NO);
	case 7:		// tfc or tfm or dlc message
		switch (sif[4]) {
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
			return (PT_YES);
		}
		return (PT_NO);
	case 8:
		switch (sif[4]) {	// ITU H0H1 byte
		case 0x1a:	// upu
		case 0x2a:	// upa
		case 0x3a:	// upt
			return (PT_YES);
		}
		return (PT_NO);
	}
	return (PT_NO);
}
enum ProtocolTest
Message::reasonableAnsiSnmmMsg(void)
{
	size_t size = end - sif;

	if (size < 8)
		return (PT_NO);

	switch (size) {
	case 8:
		switch (sif[7]) {
		case 0x17:	// tra
		case 0x27:	// trw
		case 0x13:	// rct
			return (PT_YES);
		}
		return (PT_NO);
	case 9:		// slm message
		switch (sif[7]) {
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
			return (PT_YES);
		}
		return (PT_NO);
	case 10:		// com or cbm message
		switch (sif[7]) {
		case 0x11:	// coo
		case 0x21:	// coa
		case 0x51:	// cbd
		case 0x61:	// cba
			return (PT_YES);
		}
		return (PT_NO);
	case 11:		// tfm or dlc message
		switch (sif[7]) {
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
			return (PT_YES);
		}
		return (PT_NO);
	case 12:		// tfc or upm message
		switch (sif[7]) {
		case 0x23:	// tfc
		case 0x1a:	// upu
		case 0x2a:	// upa
		case 0x3a:	// upt
			return (PT_YES);
		}
		return (PT_NO);
	default:
		return (PT_NO);
	}
}

enum ProtocolTest
Message::reasonableItutSntmMsg(void)
{
	size_t size = end - sif;

	if (6 > size || size > 21)
		return (PT_NO);

	switch (sif[4]) {
	case 0x11:
	case 0x21:
		break;
	default:
		return (PT_NO);
	}
	switch (size) {
	case 6:
	case 7:
	case 8:
		if ((sif[5] >> 4) == size - 6)
			return (PT_YES);
		return (PT_NO);
	}
	if ((sif[5] >> 4) == size - 6)
		return (PT_MAYBE);
	return (PT_NO);
}
enum ProtocolTest
Message::reasonableAnsiSntmMsg(void)
{
	size_t size = end - sif;

	if (9 > size || size > 24) {
		if (debug)
			fprintf(stderr, "%s: bad size = %d\n", __PRETTY_FUNCTION__, (int)size);
		return (PT_NO);
	}
	switch (sif[7]) {
	case 0x11:
	case 0x21:
		break;
	default:
		if (debug)
			fprintf(stderr, "%s: wrong message type 0x%02x\n", __PRETTY_FUNCTION__, sif[7]);
		return (PT_NO);
	}
	switch (size) {
	case 22:
	case 23:
	case 24:
		if ((sif[8] >> 4) == size - 9)
			return (PT_YES);
		if (debug)
			fprintf(stderr, "%s: bad li = %d, size = %d\n", __PRETTY_FUNCTION__, (int)(sif[8] >> 4), (int)size);
		return (PT_NO);
	}
	if ((sif[8] >> 4) == size - 9)
		return (PT_MAYBE);
	if (debug)
		fprintf(stderr, "%s: bad li = %d, size = %d\n", __PRETTY_FUNCTION__, (int)(sif[8] >> 4), (int)size);
	return (PT_NO);
}
enum ProtocolTest
Message::reasonableItutSnsmMsg(void)
{
	return (PT_NO);
}
enum ProtocolTest
Message::reasonableAnsiSnsmMsg(void)
{
	return reasonableAnsiSntmMsg();
}

// Check whether a message that is too large to definitely be an ITUT message
// (i.e. >= 10 SIF octets) is possibly an ITUT message.
enum ProtocolTest
Message::reasonableItutIsupMsg(void)
{
	// sif must point to the beginning of the routing label
	size_t size = end - sif; // length of SIF field (excluding SIO)

	// need 7 bytes just to have a RL+CIC+MT
	if (size < 7)
		return (PT_NO);

	switch (sif[6]) {
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
			return (PT_NO);
		return (PT_YES);
	case ISUP_MT_COT:	// 0b00000101 - Continuity
		// These all have a single octet fixed parameter and no optional
		// parameters, so cannot be longer than one plus the message type.
		// RL(4)+CIC(2)+MT(1)+MFP(1) == 8.
		if (size != 8)
			return (PT_NO);
		return (PT_YES);
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
			return (PT_NO);
		if (size > 8) {
			if (sif[7] == 0)
				return (PT_NO);
			if (size < 10)
				return (PT_YES);
			return (PT_MAYBE);
		}
		if (sif[7] != 0)
			return (PT_NO);
		return (PT_YES);
	case ISUP_MT_INR:	// ITUT: MFP(1) O()
	case ISUP_MT_INF:	// ITUT: MFP(1) O()
	case ISUP_MT_ACM:	// ITUT: MFP(1) O()
	case ISUP_MT_SUS:	// ITUT: MFP(1) O()
	case ISUP_MT_RES:	// ITUT: MFP(1) O()
	case ISUP_MT_CMR:	// ITUT: MFP(1) O()
	case ISUP_MT_CMC:	// ITUT: MFP(1) O()
	case ISUP_MT_CMRJ:	// ITUT: MFP(1) O()
	case ISUP_MT_FAR:	// ITUT: MFP(1) O()
	case ISUP_MT_FAA:	// ITUT: MFP(1) O()
	case ISUP_MT_CPG:	// ITUT: MFP(1) O()
		// These have one-octet fixed parameter and optional parameter list.
		// These all have a one-octet fixed parameter and optional parameter list
		// and are often sent with no optional parameters.  When the optional
		// parameter list is empty (pointer is zero), these must be of a fixed
		// size: RL(4)+CIC(2)+MT(1)+MFP(1)+EOP(0) == 9.
		if (size < 9)
			return (PT_NO);
		if (size > 9) {
			if (sif[7] == 0)
				return (PT_NO);
			return (PT_MAYBE);
		}
		if (sif[7] != 0)
			return (PT_NO);
		return (PT_YES);
	case ISUP_MT_IAM:	// ITUT: F(NCI(1) FCI(2) CPC(1) TMR(1)) V(CDPN(2+N(2min)) O() >= 17
		if (size < 17)
			return (PT_NO);
		return (PT_MAYBE);
	case ISUP_MT_FRJ:	// ITUT: F(FACI(1)) V(CAUS(2+N(2min))) O() >= 13
	case ISUP_MT_CQR:	// ITUT: V(RS(2+N(1min)) CSI(2+N(1min))) >= 13
		if (size < 13)
			return (PT_NO);
		return (PT_MAYBE);
	case ISUP_MT_SAM:	// ITUT: V(SUBN(2+N(2min))) O() >= 12
	case ISUP_MT_REL:	// ITUT: V(CAUS(2+N(2min))) O() >= 12
	case ISUP_MT_CFN:	// ITUT: V(CAUS(2+N(2min))) O() >= 12
		if (size < 12)
			return (PT_NO);
		return (PT_MAYBE);
	case ISUP_MT_GRS:	// ITUT: V(RS(2+N(min1))) O() >= 11
	case ISUP_MT_USR:	// ITUT: V(UUI(2+N(1min))) O() >= 11
	case ISUP_MT_CGB:	// ITUT: F(CGI(1)) V(RS(2+N(1min))) >= 11
	case ISUP_MT_CGBA:	// ITUT: F(CGI(1)) V(RS(2+N(1min))) >= 11
	case ISUP_MT_CGU:	// ITUT: F(CGI(1)) V(RS(2+N(1min))) >= 11
	case ISUP_MT_CGUA:	// ITUT: F(CGI(1)) V(RS(2+N(1min))) >= 11
		if (size < 11)
			return (PT_NO);
		return (PT_MAYBE);
	case ISUP_MT_CON:	// ITUT: F(BCI(2)) O() >= 10
		if (size < 10)
			return (PT_NO);
		if (size > 10) {
			if (sif[9] == 0)
				return (PT_NO);
			return (PT_MAYBE);
		}
		if (sif[9] != 0)
			return (PT_NO);
		return (PT_YES);
	case ISUP_MT_GRA:	// ITUT: V(RS(2+N(min1))) >= 10
	case ISUP_MT_CQM:	// ITUT: V(RS(2+N(min1))) >= 10
		if (size < 10)
			return (PT_NO);
		return (PT_MAYBE);
	case ISUP_MT_PAM:	// 0b00101000 - Pass along
		// not really expected
		return (PT_MAYBE);
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
		return (PT_NO);
	}
}

// Check whether a message that is large enough to be an ANSI message
// (i.e. >= 10 SIF octets) is possibly an ANSI message.
enum ProtocolTest
Message::reasonableAnsiIsupMsg(void)
{
	// sif must point to the beginning of the routing label
	size_t size = end - sif;

	// need 10 bytes just to have a RL+CIC+MT
	if (size < 10)
		return (PT_NO);

	switch (sif[9]) {
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
			return (PT_NO);
		return (PT_MAYBE);
	case ISUP_MT_EXM: // ANSI: O() >= 11
		if (size < 11)
			return (PT_NO);
		if (size > 11) {
			if (sif[10] == 0)
				return (PT_NO);
			return (PT_MAYBE);
		}
		if (sif[10] != 0)
			return (PT_NO);
		return (PT_MAYBE);
	case ISUP_MT_COT: // ANSI: F(COTI(1)) >= 11
	case ISUP_MT_CRM: // ANSI: F(NCI(1)) >= 11
		if (size != 11)
			return (PT_NO);
		return (PT_MAYBE);
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
		return (PT_MAYBE);
	}
}

static void
scapident(int num, char *fnames[])
{
	for (int i = 0; i < num; i++) {
		File *f;

		try {
			f = new FileText(fnames[i]);
		}
		catch(Error e1) {
			try {
				f = new FilePcap(fnames[i]);
			}
			catch(Error e2) {
				f = NULL;
			}
		}
		if (f == NULL) {
			fprintf(stderr, "E: error opening file %s\n", fnames[i]);
			continue;
		}
		f->identify();
		delete f;
		f = NULL;
		map<int,Node>::iterator it;
		for (it = nodes.begin(); it != nodes.end(); ++it)
			it->second.identify();
		nodes.clear();
	}
}

//  =========================================================================
//
//  MAIN and OPTIONS HANDLING
//
//  =========================================================================

void
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
    %1$s [options] files [...]\n\
    %1$s {-h|--help}\n\
    %1$s {-V|--version}\n\
    %1$s {-C|--copying}\n\
Arguments:\n\
    files [...]\n\
        one or more files to use for input\n\
Options:\n\
  General Options:\n\
    -q, --quiet                       (default: off)\n\
        suppress normal output\n\
    -D, --debug [LEVEL]               (default: %2$d)\n\
        increment or set debug LEVEL\n\
    -v, --verbose [LEVEL]             (default: %3$d)\n\
        increase or set verbosity LEVEL\n\
    -h, --help\n\
        prints this usage information and exits\n\
    -V, --version\n\
        prints version and exits\n\
    -C, --copying\n\
        print copying permission and exit\n\
", argv[0], debug, output);
}

int
main(int argc, char **argv)
{
	int c, val;

	while (1) {
#if defined _GNU_SOURCE
		int
		 option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
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

		c = getopt_long(argc, argv, "qD::v::hVC?W:", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "qDvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
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
			debug = val;
			break;
		case 'v':	// -v, --verbose [LEVEL]
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
	scapident(argc - optind, &argv[optind]);
	exit(4);
}
