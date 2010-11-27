/*****************************************************************************

 @(#) $RCSfile: test-m3ua-raw.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:44:17 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 Unauthorized distribution or duplication is prohibited.

 This software and related documentation is protected by copyright and
 distributed under licenses restricting its use, copying, distribution and
 decompilation.  No part of this software or related documentation may be
 reproduced in any form by any means without the prior written authorization
 of the copyright holder, and licensors, if any.

 The recipient of this document, by its retention and use, warrants that the
 recipient will protect this information and keep it confidential, and will
 not disclose the information contained in this document without the written
 permission of its owner.

 The author reserves the right to revise this software and documentation for
 any reason, including but not limited to, conformity with standards
 promulgated by various agencies, utilization of advances in the state of the
 technical arts, or the reflection of changes in the design of any techniques,
 or procedures embodied, described, or referred to herein.  The author is
 under no obligation to provide any feature listed herein.

 -----------------------------------------------------------------------------

 As an exception to the above, this software may be distributed under the GNU
 Affero General Public License (AGPL) Version 3, so long as the software is
 distributed with, and only used for the testing of, OpenSS7 modules, drivers,
 and libraries.

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

 Last Modified $Date: 2009-06-21 11:44:17 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-m3ua-raw.c,v $
 Revision 1.1.2.1  2009-06-21 11:44:17  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: test-m3ua-raw.c,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:44:17 $";

/*
 *  This file is for testing raw M3UA in conjunction with the sctp_t driver.  It provides test cases
 *  for establishing and tearing down associations and for performing basic M3UA test functions:
 *  bringing up and down an ASP; registering and deregistering a routing key; and activating and
 *  deactivating an AS.
 */

#include <sys/types.h>
#include <stropts.h>
#include <stdlib.h>

#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#else
# ifdef HAVE_STDINT_H
#  include <stdint.h>
# endif
#endif

#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/uio.h>
#include <time.h>

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define NEED_T_USCALAR_T

#include <xti.h>
#include <tihdr.h>
#include <timod.h>
#include <xti_inet.h>
#include <sys/xti_sctp.h>

#if __BYTE_ORDER == __BIG_ENDIAN
#   define __constant_ntohl(x)	(x)
#   define __constant_ntohs(x)	(x)
#   define __constant_htonl(x)	(x)
#   define __constant_htons(x)	(x)
#else
#if __BYTE_ORDER == __LITTLE_ENDIAN
#   define __constant_ntohl(x)	__bswap_constant_32(x)
#   define __constant_ntohs(x)	__bswap_constant_16(x)
#   define __constant_htonl(x)	__bswap_constant_32(x)
#   define __constant_htons(x)	__bswap_constant_16(x)
#endif
#endif

#if 1
#define SCTP_VERSION_2 1

#ifndef SCTP_VERSION_2
#   ifndef sctp_addr_t
typedef struct sctp_addr {
	uint16_t port __attribute__ ((packed));
	uint32_t addr[0] __attribute__ ((packed));
} sctp_addr_t;

#	define sctp_addr_t sctp_addr_t
#   endif			/* sctp_addr_t */
#endif				/* SCTP_VERSION_2 */
#endif

/*
 *  -------------------------------------------------------------------------
 *
 *  Configuration
 *
 *  -------------------------------------------------------------------------
 */

static const char *lpkgname = "OpenSS7 SCTP Driver";

/* static const char *spkgname = "SCTP"; */
static const char *lstdname = "M3UA SIGTRAN for SCTP";
static const char *sstdname = "RFC 4666/4960";
static const char *shortname = "M3UA/SCTP";

static char devname[256] = "/dev/streams/clone/sctp_t";

static const int test_level = T_INET_SCTP;

static int repeat_verbose = 0;
static int repeat_on_success = 0;
static int repeat_on_failure = 0;
static int exit_on_failure = 0;

static int client_port_specified = 0;
static int server_port_specified = 0;
static int client_host_specified = 0;
static int server_host_specified = 0;

static int verbose = 1;

static int client_exec = 0;		/* execute client side */
static int server_exec = 0;		/* execute server side */

static int show_msg = 0;
static int show_acks = 0;
static int show_timeout = 0;

//static int show_data = 1;

static int last_prim = 0;
static int last_event = 0;
static int last_errno = 0;
static int last_retval = 0;
static int last_t_errno = 0;
static int last_qlen = 2;
static int last_sequence = 1;
static int last_servtype = T_COTS_ORD;
static int last_provflag = T_SENDZERO | T_ORDRELDATA | T_XPG4_1;
static int last_tstate = TS_UNBND;
struct T_info_ack last_info = { 0, };
static int last_prio = 0;

static int MORE_flag = 0;
static int DATA_flag = T_ODF_EX | T_ODF_MORE;

int test_fd[3] = { 0, 0, 0 };

static int iut_connects = 1;

/*
   some globals for compressing events 
 */
static int oldact = 0;			/* previous action */
static int cntact = 0;			/* repeats of previous action */
static int oldevt = 0;
static int cntevt = 0;

static int count = 0;
//static int tries = 0;

#define BUFSIZE 32*4096

#define SHORT_WAIT	  20	// 100 // 10
#define NORMAL_WAIT	 100	// 500 // 100
#define LONG_WAIT	 500	// 5000 // 500
#define LONGER_WAIT	1000	// 10000 // 5000
#define LONGEST_WAIT	5000	// 20000 // 10000
#define TEST_DURATION	20000
#define INFINITE_WAIT	-1

static ulong test_duration = TEST_DURATION;	/* wait on other side */

ulong seq[10] = { 0, };
ulong tok[10] = { 0, };
ulong tsn[10] = { 0, };
ulong sid[10] = { 0, };
ulong ssn[10] = { 0, };
ulong ppi[10] = { 0, };
ulong exc[10] = { 0, };

char cbuf[BUFSIZE];
char dbuf[BUFSIZE];

struct strbuf ctrl = { BUFSIZE, -1, cbuf };
struct strbuf data = { BUFSIZE, -1, dbuf };

static int test_pflags = MSG_BAND;	/* MSG_BAND | MSG_HIPRI */
static int test_pband = 0;
static int test_gflags = 0;		/* MSG_BAND | MSG_HIPRI */
static int test_gband = 0;
static int test_bufsize = 256;
static int test_tidu = 256;
static int test_mgmtflags = T_NEGOTIATE;
static struct sockaddr_in *test_addr = NULL;
static socklen_t test_alen = sizeof(*test_addr);
static const char *test_data = NULL;
static int test_dlen = 0;
static int test_resfd = -1;
static int test_timout = 200;
static void *test_opts = NULL;
static int test_olen = 0;
static int test_prio = 1;

struct strfdinsert fdi = {
	{BUFSIZE, 0, cbuf},
	{BUFSIZE, 0, dbuf},
	0,
	0,
	0
};
int flags = 0;

int dummy = 0;

#if 1
#ifndef SCTP_VERSION_2
typedef struct addr {
	uint16_t port __attribute__ ((packed));
	struct in_addr addr[3] __attribute__ ((packed));
} addr_t;
#endif				/* SCTP_VERSION_2 */
#endif

struct timeval when;

#define CHILD_PTU	0
#define CHILD_IUT	1

/*
 *  M3UA Message Definitions
 */

#define M3UA_PPI    3

#define UA_VERSION  1
#define UA_PAD4(__len) (((__len)+3)&~0x3)
#define UA_MHDR(__version, __spare, __class, __type) \
	(__constant_htonl(((__version)<<24)|((__spare)<<16)|((__class)<<8)|(__type)))
#define UA_TYPE(__hdr) (__constant_ntohl(__hdr)&0xff)

#define UA_MSG_VERS(__hdr) ((ntohl(__hdr)>>24)&0xff)
#define UA_MSG_CLAS(__hdr) ((ntohl(__hdr)>> 8)&0xff)
#define UA_MSG_TYPE(__hdr) ((ntohl(__hdr)>> 0)&0xff)

/*
 *  MESSAGE CLASSES:-
 */
#define UA_CLASS_MGMT	0x00	/* UA Management (MGMT) Message */
#define UA_CLASS_XFER	0x01	/* M3UA Data transfer message */
#define UA_CLASS_SNMM	0x02	/* Signalling Network Mgmt (SNM) Messages */
#define UA_CLASS_ASPS	0x03	/* ASP State Maintenance (ASPSM) Messages */
#define UA_CLASS_ASPT	0x04	/* ASP Traffic Maintenance (ASPTM) Messages */
#define UA_CLASS_Q921	0x05	/* Q.931 User Part Messages */
#define UA_CLASS_MAUP	0x06	/* M2UA Messages */
#define UA_CLASS_CNLS	0x07	/* SUA Connectionless Messages */
#define UA_CLASS_CONS	0x08	/* SUA Connection Oriented Messages */
#define UA_CLASS_RKMM	0x09	/* Routing Key Management Messages */
#define UA_CLASS_TDHM	0x0a	/* TUA Dialog Handling Mesages */
#define UA_CLASS_TCHM	0x0b	/* TUA Component Handling Messages */

/*
 *  MESSAGES DEFINED IN EACH CLASS:-
 */
#define UA_MGMT_ERR		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x00)
#define UA_MGMT_NTFY		UA_MHDR(UA_VERSION, 0, UA_CLASS_MGMT, 0x01)
#define UA_MGMT_LAST		0x01

#define UA_XFER_DATA		UA_MHDR(UA_VERSION, 0, UA_CLASS_XFER, 0x01)

#define UA_SNMM_DUNA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x01)
#define UA_SNMM_DAVA		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x02)
#define UA_SNMM_DAUD		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x03)
#define UA_SNMM_SCON		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x04)
#define UA_SNMM_DUPU		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x05)
#define UA_SNMM_DRST		UA_MHDR(UA_VERSION, 0, UA_CLASS_SNMM, 0x06)
#define UA_SNMM_LAST		0x06

#define UA_ASPS_ASPUP_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x01)
#define UA_ASPS_ASPDN_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x02)
#define UA_ASPS_HBEAT_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x03)
#define UA_ASPS_ASPUP_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x04)
#define UA_ASPS_ASPDN_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x05)
#define UA_ASPS_HBEAT_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPS, 0x06)
#define UA_ASPS_LAST		0x06

#define UA_ASPT_ASPAC_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x01)
#define UA_ASPT_ASPIA_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x02)
#define UA_ASPT_ASPAC_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x03)
#define UA_ASPT_ASPIA_ACK	UA_MHDR(UA_VERSION, 0, UA_CLASS_ASPT, 0x04)
#define UA_ASPT_LAST		0x04

#define UA_RKMM_REG_REQ		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x01)
#define UA_RKMM_REG_RSP		UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x02)
#define UA_RKMM_DEREG_REQ	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x03)
#define UA_RKMM_DEREG_RSP	UA_MHDR(UA_VERSION, 0, UA_CLASS_RKMM, 0x04)
#define UA_RKMM_LAST		0x04

#define UA_MHDR_SIZE (sizeof(uint32_t)*2)
#define UA_PHDR_SIZE (sizeof(uint32_t))
#define UA_MAUP_SIZE (UA_MHDR_SIZE + UA_PHDR_SIZE + sizeof(uint32_t))

#define UA_TAG_MASK		(__constant_htonl(0xffff0000))
#define UA_PTAG(__phdr)		((htonl(__phdr)>>16)&0xffff)
#define UA_PLEN(__phdr)		(htonl(__phdr)&0xffff)
#define UA_SIZE(__phdr)		(__constant_htonl(__phdr)&0xffff)
#define UA_TAG(__phdr)		((__constant_htonl(__phdr)>>16)&0xffff)
#define UA_PHDR(__phdr, __length) \
	(htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))
#define UA_CONST_PHDR(__phdr, __length) \
	(__constant_htonl(((__phdr)<<16)|((__length)+sizeof(uint32_t))))

/*
 *  COMMON PARAMETERS:-
 *
 *  Common parameters per draft-ietf-sigtran-m2ua-10.txt
 *  Common parameters per draft-ietf-sigtran-m3ua-08.txt
 *  Common parameters per draft-ietf-sigtran-sua-07.txt
 *  Common parameters per rfc3057.txt
 *  -------------------------------------------------------------------
 */
#define UA_PARM_RESERVED	UA_CONST_PHDR(0x0000,0)
#define UA_PARM_IID		UA_CONST_PHDR(0x0001,sizeof(uint32_t))
#define UA_PARM_IID_RANGE	UA_CONST_PHDR(0x0002,0)	/* m2ua-10 */
#define UA_PARM_DATA		UA_CONST_PHDR(0x0003,0)	/* sua-07 */
#define UA_PARM_IID_TEXT	UA_CONST_PHDR(0x0003,0)
#define UA_PARM_INFO		UA_CONST_PHDR(0x0004,0)
#define UA_PARM_APC		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_DLCI		UA_CONST_PHDR(0x0005,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_RC		UA_CONST_PHDR(0x0006,sizeof(uint32_t))
#define UA_PARM_DIAG		UA_CONST_PHDR(0x0007,0)
#define UA_PARM_IID_RANGE1	UA_CONST_PHDR(0x0008,0)	/* rfc3057 */
#define UA_PARM_HBDATA		UA_CONST_PHDR(0x0009,0)
#define UA_PARM_REASON		UA_CONST_PHDR(0x000a,sizeof(uint32_t))
#define UA_PARM_TMODE		UA_CONST_PHDR(0x000b,sizeof(uint32_t))
#define UA_PARM_ECODE		UA_CONST_PHDR(0x000c,sizeof(uint32_t))
#define UA_PARM_STATUS		UA_CONST_PHDR(0x000d,sizeof(uint32_t))
#define UA_PARM_PROT_DATA	UA_CONST_PHDR(0x000e,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_CONG_LEVEL	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* sua-07 */
#define UA_PARM_REL_REASON	UA_CONST_PHDR(0x000f,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_TEI_STATUS	UA_CONST_PHDR(0x0010,sizeof(uint32_t))	/* rfc3057 */
#define UA_PARM_ASPID		UA_CONST_PHDR(0x0011,sizeof(uint32_t))
#define UA_PARM_AFFECT_DEST	UA_CONST_PHDR(0x0012,sizeof(uint32_t))

/*
 *  Somewhat common field values:
 */
#define   UA_ECODE_INVALID_VERSION		(0x01)
#define   UA_ECODE_INVALID_IID			(0x02)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_CLASS	(0x03)
#define   UA_ECODE_UNSUPPORTED_MESSAGE_TYPE	(0x04)
#define   UA_ECODE_UNSUPPORTED_TRAFFIC_MODE	(0x05)
#define   UA_ECODE_UNEXPECTED_MESSAGE		(0x06)
#define   UA_ECODE_PROTOCOL_ERROR		(0x07)
#define   UA_ECODE_UNSUPPORTED_IID_TYPE		(0x08)
#define   UA_ECODE_INVALID_STREAM_IDENTIFIER	(0x09)
#define  IUA_ECODE_UNASSIGNED_TEI		(0x0a)
#define  IUA_ECODE_UNRECOGNIZED_SAPI		(0x0b)
#define  IUA_ECODE_INVALID_TEI_SAPI_COMBINATION	(0x0c)
#define   UA_ECODE_REFUSED_MANAGEMENT_BLOCKING	(0x0d)
#define   UA_ECODE_ASPID_REQUIRED		(0x0e)
#define   UA_ECODE_INVALID_ASPID		(0x0f)
#define M2UA_ECODE_ASP_ACTIVE_FOR_IIDS		(0x10)
#define   UA_ECODE_INVALID_PARAMETER_VALUE	(0x11)
#define   UA_ECODE_PARAMETER_FIELD_ERROR	(0x12)
#define   UA_ECODE_UNEXPECTED_PARAMETER		(0x13)
#define   UA_ECODE_DESTINATION_STATUS_UNKNOWN	(0x14)
#define   UA_ECODE_INVALID_NETWORK_APPEARANCE	(0x15)
#define   UA_ECODE_MISSING_PARAMETER		(0x16)
#define   UA_ECODE_ROUTING_KEY_CHANGE_REFUSED	(0x17)
#define   UA_ECODE_INVALID_ROUTING_CONTEXT	(0x19)
#define   UA_ECODE_NO_CONFIGURED_AS_FOR_ASP	(0x1a)
#define   UA_ECODE_SUBSYSTEM_STATUS_UNKNOWN	(0x1b)

#define UA_STATUS_AS_DOWN			(0x00010001)
#define UA_STATUS_AS_INACTIVE			(0x00010002)
#define UA_STATUS_AS_ACTIVE			(0x00010003)
#define UA_STATUS_AS_PENDING			(0x00010004)
#define UA_STATUS_AS_INSUFFICIENT_ASPS		(0x00020001)
#define UA_STATUS_ALTERNATE_ASP_ACTIVE		(0x00020002)
#define UA_STATUS_ASP_FAILURE			(0x00020003)
#define UA_STATUS_AS_MINIMUM_ASPS		(0x00020004)

#define UA_TMODE_OVERRIDE			(0x1)
#define UA_TMODE_LOADSHARE			(0x2)
#define UA_TMODE_BROADCAST			(0x3)
#define UA_TMODE_SB_OVERRIDE			(0x4)
#define UA_TMODE_SB_LOADSHARE			(0x5)
#define UA_TMODE_SB_BROADCAST			(0x6)

#define UA_RESULT_SUCCESS			(0x00)
#define UA_RESULT_FAILURE			(0x01)

/* M3UA Specific parameters. */

#define M3UA_PARM_NTWK_APP	UA_CONST_PHDR(0x0200,sizeof(uint32_t))
#define M3UA_PARM_PROT_DATA1	UA_CONST_PHDR(0x0201,0)
#define M3UA_PARM_PROT_DATA2	UA_CONST_PHDR(0x0202,0)
#define M3UA_PARM_AFFECT_DEST	UA_CONST_PHDR(0x0203,sizeof(uint32_t))
#define M3UA_PARM_USER_CAUSE	UA_CONST_PHDR(0x0204,sizeof(uint32_t))
#define M3UA_PARM_CONG_IND	UA_CONST_PHDR(0x0205,sizeof(uint32_t))
#define M3UA_PARM_CONCERN_DEST	UA_CONST_PHDR(0x0206,sizeof(uint32_t))
#define M3UA_PARM_ROUTING_KEY	UA_CONST_PHDR(0x0207,0)
#define M3UA_PARM_REG_RESULT	UA_CONST_PHDR(0x0208,sizeof(uint32_t))
#define M3UA_PARM_DEREG_RESULT	UA_CONST_PHDR(0x0209,sizeof(uint32_t)*4)
#define M3UA_PARM_LOC_KEY_ID	UA_CONST_PHDR(0x020a,sizeof(uint32_t))
#define M3UA_PARM_DPC		UA_CONST_PHDR(0x020b,sizeof(uint32_t))
#define M3UA_PARM_SI		UA_CONST_PHDR(0x020c,sizeof(uint32_t))
#define M3UA_PARM_SSN		UA_CONST_PHDR(0x020d,sizeof(uint32_t))
#define M3UA_PARM_OPC		UA_CONST_PHDR(0x020e,sizeof(uint32_t))
#define M3UA_PARM_CIC		UA_CONST_PHDR(0x020f,sizeof(uint32_t))
#define M3UA_PARM_PROT_DATA3	UA_CONST_PHDR(0x0210,0)	/* proposed */
#define M3UA_PARM_REG_STATUS	UA_CONST_PHDR(0x0212,sizeof(uint32_t))
#define M3UA_PARM_DEREG_STATUS	UA_CONST_PHDR(0x0213,sizeof(uint32_t))

struct ua_parm {
	union {
		unsigned char *cp;	/* pointer to parameter field */
		uint32_t *wp;		/* pointer to parameter field */
	};
	size_t len;			/* length of parameter field */
	uint32_t val;			/* value of first 4 bytes (host order) */
};

struct m3ua {
	struct ua_parm data;
	struct ua_parm info;
	struct ua_parm rc;
	struct ua_parm diag;
	struct ua_parm hbdata;
	struct ua_parm reason;
	struct ua_parm tmode;
	struct ua_parm ecode;
	struct ua_parm status;
	struct ua_parm aspid;
	struct ua_parm ntwk_app;
	struct ua_parm prot_data1;
	struct ua_parm prot_data2;
	struct ua_parm affect_dest;
	struct ua_parm user_cause;
	struct ua_parm cong_ind;
	struct ua_parm concern_dest;
	struct ua_parm routing_key;
	struct ua_parm reg_result;
	struct ua_parm dereg_result;
	struct ua_parm loc_key_id;
	struct ua_parm dpc;
	struct ua_parm si;
	struct ua_parm ssn;
	struct ua_parm opc;
	struct ua_parm cic;
	struct ua_parm prot_data3;
	struct ua_parm reg_status;
	struct ua_parm dereg_status;
} m3ua = {
	.rc = { { NULL }, 0, 22222 },
	// .tmode = { { NULL }, sizeof(uint32_t), UA_TMODE_LOADSHARE },
	// .aspid = { { NULL }, sizeof(uint32_t), 1 },
	.ntwk_app = { { NULL }, sizeof(uint32_t), 8 },
	.loc_key_id = { { NULL }, sizeof(uint32_t), 10099 },
	.dpc = { { NULL }, sizeof(uint32_t), 3026 },
	// .si = { { NULL }, sizeof(uint32_t), (3 << 24) },
	// .opc = { { NULL }, sizeof(uint32_t), 2240 },
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Events and Actions
 *
 *  -------------------------------------------------------------------------
 */
enum {
	__EVENT_EOF = -7, __EVENT_NO_MSG = -6, __EVENT_TIMEOUT = -5, __EVENT_UNKNOWN = -4,
	__RESULT_DECODE_ERROR = -3, __RESULT_SCRIPT_ERROR = -2,
	__RESULT_INCONCLUSIVE = -1, __RESULT_SUCCESS = 0, __RESULT_FAILURE = 1,
	__RESULT_NOTAPPL = 3, __RESULT_SKIPPED = 77,
};

/*
 *  -------------------------------------------------------------------------
 */

int show = 1;

enum {
	__TEST_CONN_REQ = 100, __TEST_CONN_RES, __TEST_DISCON_REQ,
	__TEST_DATA_REQ, __TEST_EXDATA_REQ, __TEST_INFO_REQ, __TEST_BIND_REQ,
	__TEST_UNBIND_REQ, __TEST_UNITDATA_REQ, __TEST_OPTMGMT_REQ,
	__TEST_ORDREL_REQ, __TEST_OPTDATA_REQ, __TEST_ADDR_REQ,
	__TEST_CAPABILITY_REQ, __TEST_CONN_IND, __TEST_CONN_CON,
	__TEST_DISCON_IND, __TEST_DATA_IND, __TEST_EXDATA_IND,
	__TEST_INFO_ACK, __TEST_BIND_ACK, __TEST_ERROR_ACK, __TEST_OK_ACK,
	__TEST_UNITDATA_IND, __TEST_UDERROR_IND, __TEST_OPTMGMT_ACK,
	__TEST_ORDREL_IND, __TEST_NRM_OPTDATA_IND, __TEST_EXP_OPTDATA_IND,
	__TEST_ADDR_ACK, __TEST_CAPABILITY_ACK, __TEST_WRITE, __TEST_WRITEV,
	__TEST_PUTMSG_DATA, __TEST_PUTPMSG_DATA, __TEST_PUSH, __TEST_POP,
	__TEST_READ, __TEST_READV, __TEST_GETMSG, __TEST_GETPMSG,
	__TEST_DATA,
	__TEST_DATACK_REQ, __TEST_DATACK_IND, __TEST_RESET_REQ,
	__TEST_RESET_IND, __TEST_RESET_RES, __TEST_RESET_CON,
	__TEST_O_TI_GETINFO, __TEST_O_TI_OPTMGMT, __TEST_O_TI_BIND,
	__TEST_O_TI_UNBIND,
	__TEST__O_TI_GETINFO, __TEST__O_TI_OPTMGMT, __TEST__O_TI_BIND,
	__TEST__O_TI_UNBIND, __TEST__O_TI_GETMYNAME, __TEST__O_TI_GETPEERNAME,
	__TEST__O_TI_XTI_HELLO, __TEST__O_TI_XTI_GET_STATE,
	__TEST__O_TI_XTI_CLEAR_EVENT, __TEST__O_TI_XTI_MODE,
	__TEST__O_TI_TLI_MODE,
	__TEST_TI_GETINFO, __TEST_TI_OPTMGMT, __TEST_TI_BIND,
	__TEST_TI_UNBIND, __TEST_TI_GETMYNAME, __TEST_TI_GETPEERNAME,
	__TEST_TI_SETMYNAME, __TEST_TI_SETPEERNAME, __TEST_TI_SYNC,
	__TEST_TI_GETADDRS, __TEST_TI_CAPABILITY,
	__TEST_TI_SETMYNAME_DATA, __TEST_TI_SETPEERNAME_DATA,
	__TEST_TI_SETMYNAME_DISC, __TEST_TI_SETPEERNAME_DISC,
	__TEST_TI_SETMYNAME_DISC_DATA, __TEST_TI_SETPEERNAME_DISC_DATA,
	__TEST_PRIM_TOO_SHORT, __TEST_PRIM_WAY_TOO_SHORT,
	__TEST_ASPUP_REQ, __TEST_ASPUP_ACK, __TEST_ASPDN_REQ, __TEST_ASPDN_ACK,
	__TEST_REG_REQ, __TEST_REG_RSP, __TEST_DEREG_REQ, __TEST_DEREG_RSP,
	__TEST_ASPAC_REQ, __TEST_ASPAC_ACK, __TEST_ASPIA_REQ, __TEST_ASPIA_ACK,
	__TEST_HBEAT_REQ, __TEST_HBEAT_ACK, __TEST_ERR_MSG, __TEST_NTFY_MSG, __TEST_DAUD_MSG,
	__TEST_DUNA_MSG, __TEST_DAVA_MSG, __TEST_SCON_MSG, __TEST_DUPU_MSG, __TEST_DRST_MSG,
	__TEST_XFER_DATA,
};

/*
 *  -------------------------------------------------------------------------
 *
 *  Timer Functions
 *
 *  -------------------------------------------------------------------------
 */

/*
 *  Timer values for tests: each timer has a low range (minus error margin)
 *  and a high range (plus error margin).
 */

static long timer_scale = 1;

#define TEST_TIMEOUT 5000

typedef struct timer_range {
	long lo;
	long hi;
} timer_range_t;

enum {
	t1 = 0, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12, t13, t14, t15,
	t16, t17, t18, t19, t20, t21, t22, t23, t24, t25, t26, t27, t28, t29,
	t30, t31, t32, t33, t34, t35, t36, t37, t38, tmax
};

long test_start = 0;

static int state = 0;
static const char *failure_string = NULL;

#define __stringify_1(x) #x
#define __stringify(x) __stringify_1(x)
#define FAILURE_STRING(string) "[" __stringify(__LINE__) "] " string

#if 1
#undef lockf
#define lockf(x,y,z) 0
#endif

#if 0
/*
 *  Return the current time in milliseconds.
 */
static long
dual_milliseconds(int child, int t1, int t2)
{
	long ret;
	struct timeval now;
	static const char *msgs[] = {
		"             %1$-6.6s !      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld     :                    [%7$d:%8$03d]\n",
		"                    :      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld     ! %1$-6.6s             [%7$d:%8$03d]\n",
		"                    :      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld  !  : %1$-6.6s             [%7$d:%8$03d]\n",
		"                    !  %1$-6.6s %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld  !                    [%7$d:%8$03d]\n",
	};
	static const char *blank[] = {
		"                    !                                   :                    \n",
		"                    :                                   !                    \n",
		"                    :                                !  :                    \n",
		"                    !                                   !                    \n",
	};
	static const char *plus[] = {
		"               +    !                                   :                    \n",
		"                    :                                   !    +               \n",
		"                    :                                !  :    +               \n",
		"                    !      +                            !                    \n",
	};

	gettimeofday(&now, NULL);
	if (!test_start)	/* avoid blowing over precision */
		test_start = now.tv_sec;
	ret = (now.tv_sec - test_start) * 1000;
	ret += (now.tv_usec + 500) / 1000;

	if (show && verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, blank[child]);
		fprintf(stdout, msgs[child], timer[t1].name, timer[t1].lo / 1000, timer[t1].lo - ((timer[t1].lo / 1000) * 1000), timer[t1].name, timer[t1].hi / 1000, timer[t1].hi - ((timer[t1].hi / 1000) * 1000), child, state);
		fprintf(stdout, plus[child]);
		fprintf(stdout, msgs[child], timer[t2].name, timer[t2].lo / 1000, timer[t2].lo - ((timer[t2].lo / 1000) * 1000), timer[t2].name, timer[t2].hi / 1000, timer[t2].hi - ((timer[t2].hi / 1000) * 1000), child, state);
		fprintf(stdout, blank[child]);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}

	return ret;
}

/*
 *  Return the current time in milliseconds.
 */
static long
milliseconds(int child, int t)
{
	long ret;
	struct timeval now;
	static const char *msgs[] = {
		"             %1$-6.6s !      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld     :                    [%7$d:%8$03d]\n",
		"                    :      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld     ! %1$-6.6s             [%7$d:%8$03d]\n",
		"                    :      %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld  !  : %1$-6.6s             [%7$d:%8$03d]\n",
		"                    !  %1$-6.6s %2$3ld.%3$03ld <= %4$-2.2s <= %5$3ld.%6$03ld  !                    [%7$d:%8$03d]\n",
	};
	static const char *blank[] = {
		"                    !                                   :                    \n",
		"                    :                                   !                    \n",
		"                    :                                !  :                    \n",
		"                    !                                   !                    \n",
	};

	gettimeofday(&now, NULL);
	if (!test_start)	/* avoid blowing over precision */
		test_start = now.tv_sec;
	ret = (now.tv_sec - test_start) * 1000;
	ret += (now.tv_usec + 500) / 1000;

	if (show && verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, blank[child]);
		fprintf(stdout, msgs[child], timer[t].name, timer[t].lo / 1000, timer[t].lo - ((timer[t].lo / 1000) * 1000), timer[t].name, timer[t].hi / 1000, timer[t].hi - ((timer[t].hi / 1000) * 1000), child, state);
		fprintf(stdout, blank[child]);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}

	return ret;
}

/*
 *  Check the current time against the beginning time provided as an argnument
 *  and see if the time inverval falls between the low and high values for the
 *  timer as specified by arguments.  Return SUCCESS if the interval is within
 *  the allowable range and FAILURE otherwise.
 */
static int
check_time(int child, const char *t, long beg, long lo, long hi)
{
	long i;
	struct timeval now;
	static const char *msgs[] = {
		"       check %1$-6.6s ? [%2$3ld.%3$03ld <= %4$3ld.%5$03ld <= %6$3ld.%7$03ld]   |                    [%8$d:%9$03d]\n",
		"                    | [%2$3ld.%3$03ld <= %4$3ld.%5$03ld <= %6$3ld.%7$03ld]   ? %1$-6.6s check       [%8$d:%9$03d]\n",
		"                    | [%2$3ld.%3$03ld <= %4$3ld.%5$03ld <= %6$3ld.%7$03ld]?  | %1$-6.6s check       [%8$d:%9$03d]\n",
		"       check %1$-6.6s ? [%2$3ld.%3$03ld <= %4$3ld.%5$03ld <= %6$3ld.%7$03ld]   ?                    [%8$d:%9$03d]\n",
	};

	if (gettimeofday(&now, NULL)) {
		printf("****ERROR: gettimeofday\n");
		printf("           %s: %s\n", __FUNCTION__, strerror(errno));
		fflush(stdout);
		return __RESULT_FAILURE;
	}

	i = (now.tv_sec - test_start) * 1000;
	i += (now.tv_usec + 500) / 1000;
	i -= beg;

	if (show && verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, msgs[child], t, (lo - 100) / 1000, (lo - 100) - (((lo - 100) / 1000) * 1000), i / 1000, i - ((i / 1000) * 1000), (hi + 100) / 1000, (hi + 100) - (((hi + 100) / 1000) * 1000), child, state);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}

	if (lo - 100 <= i && i <= hi + 100)
		return __RESULT_SUCCESS;
	else
		return __RESULT_FAILURE;
}
#endif

static int
time_event(int child, int event)
{
	static const char *msgs[] = {
		"                    ! %11.6g                |                    <%d:%03d>\n",
		"                    |                %11.6g !                    <%d:%03d>\n",
		"                    |             %11.6g !  |                    <%d:%03d>\n",
		"                    !        %11.6g         !                    <%d:%03d>\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg)) {
		float t, m;
		struct timeval now;

		gettimeofday(&now, NULL);
		if (!test_start)
			test_start = now.tv_sec;
		t = (now.tv_sec - test_start);
		m = now.tv_usec;
		m = m / 1000000;
		t += m;

		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, msgs[child], t, child, state);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	return (event);
}

static int timer_timeout = 0;
static int last_signum = 0;

static void
signal_handler(int signum)
{
	last_signum = signum;
	if (signum == SIGALRM)
		timer_timeout = 1;
	return;
}

static int
start_signals(void)
{
	sigset_t mask;
	struct sigaction act;

	act.sa_handler = signal_handler;
//      act.sa_flags = SA_RESTART | SA_ONESHOT;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		return __RESULT_FAILURE;
	if (sigaction(SIGPOLL, &act, NULL))
		return __RESULT_FAILURE;
	if (sigaction(SIGURG, &act, NULL))
		return __RESULT_FAILURE;
	if (sigaction(SIGPIPE, &act, NULL))
		return __RESULT_FAILURE;
	if (sigaction(SIGHUP, &act, NULL))
		return __RESULT_FAILURE;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigaddset(&mask, SIGPOLL);
	sigaddset(&mask, SIGURG);
	sigaddset(&mask, SIGPIPE);
	sigaddset(&mask, SIGHUP);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	siginterrupt(SIGALRM, 1);
	siginterrupt(SIGPOLL, 1);
	siginterrupt(SIGURG, 1);
	siginterrupt(SIGPIPE, 1);
	siginterrupt(SIGHUP, 1);
	return __RESULT_SUCCESS;
}

/*
 *  Start an interval timer as the overall test timer.
 */
static int
start_tt(long duration)
{
	struct itimerval setting = {
		{0, 0},
		{duration / 1000, (duration % 1000) * 1000}
	};

	if (duration == (long) INFINITE_WAIT)
		return __RESULT_SUCCESS;
	if (start_signals())
		return __RESULT_FAILURE;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return __RESULT_FAILURE;
	timer_timeout = 0;
	return __RESULT_SUCCESS;
}

#if 0
static int
start_st(long duration)
{
	long sdur = (duration + timer_scale - 1) / timer_scale;

	return start_tt(sdur);
}
#endif

static int
stop_signals(void)
{
	int result = __RESULT_SUCCESS;
	sigset_t mask;
	struct sigaction act;

	act.sa_handler = SIG_DFL;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		result = __RESULT_FAILURE;
	if (sigaction(SIGPOLL, &act, NULL))
		result = __RESULT_FAILURE;
	if (sigaction(SIGURG, &act, NULL))
		result = __RESULT_FAILURE;
	if (sigaction(SIGPIPE, &act, NULL))
		result = __RESULT_FAILURE;
	if (sigaction(SIGHUP, &act, NULL))
		result = __RESULT_FAILURE;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigaddset(&mask, SIGPOLL);
	sigaddset(&mask, SIGURG);
	sigaddset(&mask, SIGPIPE);
	sigaddset(&mask, SIGHUP);
	sigprocmask(SIG_BLOCK, &mask, NULL);
	return (result);
}

static int
stop_tt(void)
{
	struct itimerval setting = { {0, 0}, {0, 0} };
	int result = __RESULT_SUCCESS;

	if (setitimer(ITIMER_REAL, &setting, NULL))
		return __RESULT_FAILURE;
	if (stop_signals() != __RESULT_SUCCESS)
		result = __RESULT_FAILURE;
	timer_timeout = 0;
	return (result);
}

/*
 *  Addresses
 */
#if 1
#ifndef SCTP_VERSION_2
addr_t addrs[4];
#else				/* SCTP_VERSION_2 */
struct sockaddr_in addrs[4][3];
#endif				/* SCTP_VERSION_2 */
#else
struct sockaddr_in addrs[4];
#endif
int anums[4] = { 3, 3, 3, 3 };

#define TEST_PORT_NUMBER 18000
unsigned short ports[4] = { TEST_PORT_NUMBER + 0, TEST_PORT_NUMBER + 1, TEST_PORT_NUMBER + 2, TEST_PORT_NUMBER + 3 };
const char *addr_strings[4] = { "127.0.0.1", "127.0.0.2", "127.0.0.3", "127.0.0.4" };

/*
 *  Options
 */

/*
 * data options
 */
struct {
#if 1
	struct t_opthdr tos_hdr;
	union {
		unsigned char opt_val;
		t_scalar_t opt_fil;
	} tos_val;
	struct t_opthdr ttl_hdr;
	union {
		unsigned char opt_val;
		t_scalar_t opt_fil;
	} ttl_val;
#endif
	struct t_opthdr drt_hdr;
	t_scalar_t drt_val;
#if 0
	struct t_opthdr csm_hdr;
	t_scalar_t csm_val;
#endif
#if 1
	struct t_opthdr ppi_hdr;
	t_uscalar_t ppi_val;
	struct t_opthdr sid_hdr;
	t_scalar_t sid_val;
#endif
} opt_data = {
#if 1
	{
	sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TOS, T_SUCCESS}, { .opt_val = 0x0} , {
	sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TTL, T_SUCCESS}, { .opt_val = 64}, {
#endif
	sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_DONTROUTE, T_SUCCESS}, T_NO, {
#if 0
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), T_INET_UDP, T_UDP_CHECKSUM, T_SUCCESS}, T_NO, {
#endif
#if 1
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), T_INET_SCTP, T_SCTP_PPI, T_SUCCESS}, 3, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SID, T_SUCCESS}, 0
#endif
};

/*
 * connect options
 */
struct {
	struct t_opthdr dbg_hdr;
	t_uscalar_t dbg_val;
	struct t_opthdr lin_hdr;
	struct t_linger lin_val;
	struct t_opthdr rbf_hdr;
	t_uscalar_t rbf_val;
	struct t_opthdr rlw_hdr;
	t_uscalar_t rlw_val;
	struct t_opthdr sbf_hdr;
	t_uscalar_t sbf_val;
	struct t_opthdr slw_hdr;
	t_uscalar_t slw_val;
	struct t_opthdr tos_hdr;
	union {
		unsigned char opt_val;
		t_scalar_t opt_fil;
	} tos_val;
	struct t_opthdr ttl_hdr;
	union {
		unsigned char opt_val;
		t_scalar_t opt_fil;
	} ttl_val;
	struct t_opthdr drt_hdr;
	t_scalar_t drt_val;
	struct t_opthdr bca_hdr;
	t_scalar_t bca_val;
	struct t_opthdr reu_hdr;
	t_scalar_t reu_val;
#if 1
	struct t_opthdr ist_hdr;
	t_scalar_t ist_val;
	struct t_opthdr ost_hdr;
	t_scalar_t ost_val;
#endif
} opt_conn = {
	{
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), XTI_GENERIC, XTI_DEBUG, T_SUCCESS} , 0x0, {
	sizeof(struct t_opthdr) + sizeof(struct t_linger), XTI_GENERIC, XTI_LINGER, T_SUCCESS}, { T_NO, T_UNSPEC} , {
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), XTI_GENERIC, XTI_RCVBUF, T_SUCCESS} , 32767, {
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), XTI_GENERIC, XTI_RCVLOWAT, T_SUCCESS} , 1, {
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), XTI_GENERIC, XTI_SNDBUF, T_SUCCESS} , 32767, {
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), XTI_GENERIC, XTI_SNDLOWAT, T_SUCCESS} , 1, {
	sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TOS, T_SUCCESS}, { .opt_val = 0x0} , {
	sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TTL, T_SUCCESS}, { .opt_val = 64} , {
	sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_DONTROUTE, T_SUCCESS}, T_NO, {
	sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_BROADCAST, T_SUCCESS}, T_NO, {
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), T_INET_IP, T_IP_REUSEADDR, T_SUCCESS} , T_NO
#if 1
	    , {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ISTREAMS, T_SUCCESS} , 1, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_OSTREAMS, T_SUCCESS} , 1
#endif
};

/*
 * management options
 */
struct {
#if 0
#if 1
	struct t_opthdr xdb_hdr;
	t_uscalar_t xdb_val;
#else
	struct t_opthdr dbg_hdr;
	t_uscalar_t dbg_val;
#endif
#endif
	struct t_opthdr lin_hdr;
	struct t_linger lin_val;
	struct t_opthdr rbf_hdr;
	t_uscalar_t rbf_val;
	struct t_opthdr rlw_hdr;
	t_uscalar_t rlw_val;
	struct t_opthdr sbf_hdr;
	t_uscalar_t sbf_val;
	struct t_opthdr slw_hdr;
	t_uscalar_t slw_val;
	struct t_opthdr tos_hdr;
	union {
		unsigned char opt_val;
		t_scalar_t opt_fil;
	} tos_val;
	struct t_opthdr ttl_hdr;
	union {
		unsigned char opt_val;
		t_scalar_t opt_fil;
	} ttl_val;
	struct t_opthdr drt_hdr;
	t_scalar_t drt_val;
	struct t_opthdr bca_hdr;
	t_scalar_t bca_val;
	struct t_opthdr reu_hdr;
	t_scalar_t reu_val;
#if 0
	struct t_opthdr csm_hdr;
	t_scalar_t csm_val;
#endif
#if 0
	struct t_opthdr ndl_hdr;
	t_scalar_t ndl_val;
	struct t_opthdr mxs_hdr;
	t_scalar_t mxs_val;
#endif
#if 0
	struct t_opthdr kpa_hdr;
	struct t_kpalive kpa_val;
#endif
#if 1
	struct t_opthdr nod_hdr;
	t_scalar_t nod_val;
	struct t_opthdr crk_hdr;
	t_scalar_t crk_val;
	struct t_opthdr ppi_hdr;
	t_scalar_t ppi_val;
	struct t_opthdr sid_hdr;
	t_scalar_t sid_val;
	struct t_opthdr rcv_hdr;
	t_scalar_t rcv_val;
	struct t_opthdr ckl_hdr;
	t_scalar_t ckl_val;
	struct t_opthdr skd_hdr;
	t_scalar_t skd_val;
	struct t_opthdr prt_hdr;
	t_scalar_t prt_val;
	struct t_opthdr art_hdr;
	t_scalar_t art_val;
	struct t_opthdr irt_hdr;
	t_scalar_t irt_val;
	struct t_opthdr hbi_hdr;
	t_scalar_t hbi_val;
	struct t_opthdr rin_hdr;
	t_scalar_t rin_val;
	struct t_opthdr rmn_hdr;
	t_scalar_t rmn_val;
	struct t_opthdr rmx_hdr;
	t_scalar_t rmx_val;
	struct t_opthdr ist_hdr;
	t_scalar_t ist_val;
	struct t_opthdr ost_hdr;
	t_scalar_t ost_val;
	struct t_opthdr cin_hdr;
	t_scalar_t cin_val;
	struct t_opthdr tin_hdr;
	t_scalar_t tin_val;
	struct t_opthdr mac_hdr;
	t_scalar_t mac_val;
	struct t_opthdr dbg_hdr;
	t_scalar_t dbg_val;
#endif
} opt_optm = {
	{
#if 0
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), XTI_GENERIC, XTI_DEBUG, T_SUCCESS} , 0x0, {
#endif
	sizeof(struct t_opthdr) + sizeof(struct t_linger), XTI_GENERIC, XTI_LINGER, T_SUCCESS}, { T_NO, T_UNSPEC} , {
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), XTI_GENERIC, XTI_RCVBUF, T_SUCCESS} , 32767, {
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), XTI_GENERIC, XTI_RCVLOWAT, T_SUCCESS} , 1, {
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), XTI_GENERIC, XTI_SNDBUF, T_SUCCESS} , 32767, {
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), XTI_GENERIC, XTI_SNDLOWAT, T_SUCCESS} , 1, {
	sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TOS, T_SUCCESS}, { .opt_val = 0x0} , {
	sizeof(struct t_opthdr) + sizeof(unsigned char), T_INET_IP, T_IP_TTL, T_SUCCESS}, { .opt_val = 64} , {
	sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_DONTROUTE, T_SUCCESS}, T_NO, {
	sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_BROADCAST, T_SUCCESS}, T_NO, {
	sizeof(struct t_opthdr) + sizeof(unsigned int), T_INET_IP, T_IP_REUSEADDR, T_SUCCESS}, T_NO, {
#if 0
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), T_INET_UDP, T_UDP_CHECKSUM, T_SUCCESS} , T_NO , {
#endif
#if 0
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_TCP, T_TCP_NODELAY, T_SUCCESS} , T_NO, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_TCP, T_TCP_MAXSEG, T_SUCCESS} , 576, {
#endif
#if 0
	sizeof(struct t_opthdr) + sizeof(struct t_kpalive), T_INET_TCP, T_TCP_KEEPALIVE, T_SUCCESS}, { T_NO, 0}, {
#endif
#if 1
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_NODELAY, T_SUCCESS} , T_YES, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_CORK, T_SUCCESS} , T_YES, {
	sizeof(struct t_opthdr) + sizeof(t_uscalar_t), T_INET_SCTP, T_SCTP_PPI, T_SUCCESS} , 10, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SID, T_SUCCESS} , 0, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RECVOPT, T_SUCCESS} , T_NO, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_COOKIE_LIFE, T_SUCCESS} , 60000, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_SACK_DELAY, T_SUCCESS} , 200, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_PATH_MAX_RETRANS, T_SUCCESS} , 5, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ASSOC_MAX_RETRANS, T_SUCCESS} , 12, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_MAX_INIT_RETRIES, T_SUCCESS} , 12, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_HEARTBEAT_ITVL, T_SUCCESS} , 1000, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_INITIAL, T_SUCCESS} , 200, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_MIN, T_SUCCESS} , 10, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_RTO_MAX, T_SUCCESS} , 2000, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_OSTREAMS, T_SUCCESS} , 257, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_ISTREAMS, T_SUCCESS} , 257, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_COOKIE_INC, T_SUCCESS} , 1000, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_THROTTLE_ITVL, T_SUCCESS} , 50, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_MAC_TYPE, T_SUCCESS} , T_SCTP_HMAC_NONE, {
	sizeof(struct t_opthdr) + sizeof(t_scalar_t), T_INET_SCTP, T_SCTP_DEBUG, T_SUCCESS} , 0
#endif
};

struct t_opthdr *
find_option(int level, int name, const char *cmd_buf, size_t opt_ofs, size_t opt_len)
{
	const char *opt_ptr = cmd_buf + opt_ofs;
	struct t_opthdr *oh = NULL;

	for (oh = _T_OPT_FIRSTHDR_OFS(opt_ptr, opt_len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(opt_ptr, opt_len, oh, 0)) {
		int len = oh->len - sizeof(*oh);

		if (len < 0) {
			oh = NULL;
			break;
		}
		if (oh->level != level)
			continue;
		if (oh->name != name)
			continue;
		break;
	}
	return (oh);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Printing things
 *
 *  -------------------------------------------------------------------------
 */

char *
errno_string(t_scalar_t err)
{
	switch (err) {
	case 0:
		return ("ok");
	case EPERM:
		return ("[EPERM]");
	case ENOENT:
		return ("[ENOENT]");
	case ESRCH:
		return ("[ESRCH]");
	case EINTR:
		return ("[EINTR]");
	case EIO:
		return ("[EIO]");
	case ENXIO:
		return ("[ENXIO]");
	case E2BIG:
		return ("[E2BIG]");
	case ENOEXEC:
		return ("[ENOEXEC]");
	case EBADF:
		return ("[EBADF]");
	case ECHILD:
		return ("[ECHILD]");
	case EAGAIN:
		return ("[EAGAIN]");
	case ENOMEM:
		return ("[ENOMEM]");
	case EACCES:
		return ("[EACCES]");
	case EFAULT:
		return ("[EFAULT]");
	case ENOTBLK:
		return ("[ENOTBLK]");
	case EBUSY:
		return ("[EBUSY]");
	case EEXIST:
		return ("[EEXIST]");
	case EXDEV:
		return ("[EXDEV]");
	case ENODEV:
		return ("[ENODEV]");
	case ENOTDIR:
		return ("[ENOTDIR]");
	case EISDIR:
		return ("[EISDIR]");
	case EINVAL:
		return ("[EINVAL]");
	case ENFILE:
		return ("[ENFILE]");
	case EMFILE:
		return ("[EMFILE]");
	case ENOTTY:
		return ("[ENOTTY]");
	case ETXTBSY:
		return ("[ETXTBSY]");
	case EFBIG:
		return ("[EFBIG]");
	case ENOSPC:
		return ("[ENOSPC]");
	case ESPIPE:
		return ("[ESPIPE]");
	case EROFS:
		return ("[EROFS]");
	case EMLINK:
		return ("[EMLINK]");
	case EPIPE:
		return ("[EPIPE]");
	case EDOM:
		return ("[EDOM]");
	case ERANGE:
		return ("[ERANGE]");
	case EDEADLK:
		return ("[EDEADLK]");
	case ENAMETOOLONG:
		return ("[ENAMETOOLONG]");
	case ENOLCK:
		return ("[ENOLCK]");
	case ENOSYS:
		return ("[ENOSYS]");
	case ENOTEMPTY:
		return ("[ENOTEMPTY]");
	case ELOOP:
		return ("[ELOOP]");
	case ENOMSG:
		return ("[ENOMSG]");
	case EIDRM:
		return ("[EIDRM]");
	case ECHRNG:
		return ("[ECHRNG]");
	case EL2NSYNC:
		return ("[EL2NSYNC]");
	case EL3HLT:
		return ("[EL3HLT]");
	case EL3RST:
		return ("[EL3RST]");
	case ELNRNG:
		return ("[ELNRNG]");
	case EUNATCH:
		return ("[EUNATCH]");
	case ENOCSI:
		return ("[ENOCSI]");
	case EL2HLT:
		return ("[EL2HLT]");
	case EBADE:
		return ("[EBADE]");
	case EBADR:
		return ("[EBADR]");
	case EXFULL:
		return ("[EXFULL]");
	case ENOANO:
		return ("[ENOANO]");
	case EBADRQC:
		return ("[EBADRQC]");
	case EBADSLT:
		return ("[EBADSLT]");
	case EBFONT:
		return ("[EBFONT]");
	case ENOSTR:
		return ("[ENOSTR]");
	case ENODATA:
		return ("[ENODATA]");
	case ETIME:
		return ("[ETIME]");
	case ENOSR:
		return ("[ENOSR]");
	case ENONET:
		return ("[ENONET]");
	case ENOPKG:
		return ("[ENOPKG]");
	case EREMOTE:
		return ("[EREMOTE]");
	case ENOLINK:
		return ("[ENOLINK]");
	case EADV:
		return ("[EADV]");
	case ESRMNT:
		return ("[ESRMNT]");
	case ECOMM:
		return ("[ECOMM]");
	case EPROTO:
		return ("[EPROTO]");
	case EMULTIHOP:
		return ("[EMULTIHOP]");
	case EDOTDOT:
		return ("[EDOTDOT]");
	case EBADMSG:
		return ("[EBADMSG]");
	case EOVERFLOW:
		return ("[EOVERFLOW]");
	case ENOTUNIQ:
		return ("[ENOTUNIQ]");
	case EBADFD:
		return ("[EBADFD]");
	case EREMCHG:
		return ("[EREMCHG]");
	case ELIBACC:
		return ("[ELIBACC]");
	case ELIBBAD:
		return ("[ELIBBAD]");
	case ELIBSCN:
		return ("[ELIBSCN]");
	case ELIBMAX:
		return ("[ELIBMAX]");
	case ELIBEXEC:
		return ("[ELIBEXEC]");
	case EILSEQ:
		return ("[EILSEQ]");
	case ERESTART:
		return ("[ERESTART]");
	case ESTRPIPE:
		return ("[ESTRPIPE]");
	case EUSERS:
		return ("[EUSERS]");
	case ENOTSOCK:
		return ("[ENOTSOCK]");
	case EDESTADDRREQ:
		return ("[EDESTADDRREQ]");
	case EMSGSIZE:
		return ("[EMSGSIZE]");
	case EPROTOTYPE:
		return ("[EPROTOTYPE]");
	case ENOPROTOOPT:
		return ("[ENOPROTOOPT]");
	case EPROTONOSUPPORT:
		return ("[EPROTONOSUPPORT]");
	case ESOCKTNOSUPPORT:
		return ("[ESOCKTNOSUPPORT]");
	case EOPNOTSUPP:
		return ("[EOPNOTSUPP]");
	case EPFNOSUPPORT:
		return ("[EPFNOSUPPORT]");
	case EAFNOSUPPORT:
		return ("[EAFNOSUPPORT]");
	case EADDRINUSE:
		return ("[EADDRINUSE]");
	case EADDRNOTAVAIL:
		return ("[EADDRNOTAVAIL]");
	case ENETDOWN:
		return ("[ENETDOWN]");
	case ENETUNREACH:
		return ("[ENETUNREACH]");
	case ENETRESET:
		return ("[ENETRESET]");
	case ECONNABORTED:
		return ("[ECONNABORTED]");
	case ECONNRESET:
		return ("[ECONNRESET]");
	case ENOBUFS:
		return ("[ENOBUFS]");
	case EISCONN:
		return ("[EISCONN]");
	case ENOTCONN:
		return ("[ENOTCONN]");
	case ESHUTDOWN:
		return ("[ESHUTDOWN]");
	case ETOOMANYREFS:
		return ("[ETOOMANYREFS]");
	case ETIMEDOUT:
		return ("[ETIMEDOUT]");
	case ECONNREFUSED:
		return ("[ECONNREFUSED]");
	case EHOSTDOWN:
		return ("[EHOSTDOWN]");
	case EHOSTUNREACH:
		return ("[EHOSTUNREACH]");
	case EALREADY:
		return ("[EALREADY]");
	case EINPROGRESS:
		return ("[EINPROGRESS]");
	case ESTALE:
		return ("[ESTALE]");
	case EUCLEAN:
		return ("[EUCLEAN]");
	case ENOTNAM:
		return ("[ENOTNAM]");
	case ENAVAIL:
		return ("[ENAVAIL]");
	case EISNAM:
		return ("[EISNAM]");
	case EREMOTEIO:
		return ("[EREMOTEIO]");
	case EDQUOT:
		return ("[EDQUOT]");
	case ENOMEDIUM:
		return ("[ENOMEDIUM]");
	case EMEDIUMTYPE:
		return ("[EMEDIUMTYPE]");
	default:
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "[%ld]", (long) err);
		return buf;
	}
	}
}

char *
terrno_string(t_uscalar_t terr, t_scalar_t uerr)
{
	switch (terr) {
	case TBADADDR:
		return ("[TBADADDR]");
	case TBADOPT:
		return ("[TBADOPT]");
	case TACCES:
		return ("[TACCES]");
	case TBADF:
		return ("[TBADF]");
	case TNOADDR:
		return ("[TNOADDR]");
	case TOUTSTATE:
		return ("[TOUTSTATE]");
	case TBADSEQ:
		return ("[TBADSEQ]");
	case TSYSERR:
		return errno_string(uerr);
	case TLOOK:
		return ("[TLOOK]");
	case TBADDATA:
		return ("[TBADDATA]");
	case TBUFOVFLW:
		return ("[TBUFOVFLW]");
	case TFLOW:
		return ("[TFLOW]");
	case TNODATA:
		return ("[TNODATA]");
	case TNODIS:
		return ("[TNODIS]");
	case TNOUDERR:
		return ("[TNOUDERR]");
	case TBADFLAG:
		return ("[TBADFLAG]");
	case TNOREL:
		return ("[TNOREL]");
	case TNOTSUPPORT:
		return ("[TNOTSUPPORT]");
	case TSTATECHNG:
		return ("[TSTATECHNG]");
	case TNOSTRUCTYPE:
		return ("[TNOSTRUCTYPE]");
	case TBADNAME:
		return ("[TBADNAME]");
	case TBADQLEN:
		return ("[TBADQLEN]");
	case TADDRBUSY:
		return ("[TADDRBUSY]");
	case TINDOUT:
		return ("[TINDOUT]");
	case TPROVMISMATCH:
		return ("[TPROVMISMATCH]");
	case TRESQLEN:
		return ("[TRESQLEN]");
	case TRESADDR:
		return ("[TRESADDR]");
	case TQFULL:
		return ("[TQFULL]");
	case TPROTO:
		return ("[TPROTO]");
	default:
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "[%lu]", (ulong) terr);
		return buf;
	}
	}
}

#define ICMP_ECHOREPLY		0	/* Echo Reply */
#define ICMP_DEST_UNREACH	3	/* Destination Unreachable */
#define ICMP_SOURCE_QUENCH	4	/* Source Quench */
#define ICMP_REDIRECT		5	/* Redirect (change route) */
#define ICMP_ECHO		8	/* Echo Request */
#define ICMP_TIME_EXCEEDED	11	/* Time Exceeded */
#define ICMP_PARAMETERPROB	12	/* Parameter Problem */
#define ICMP_TIMESTAMP		13	/* Timestamp Request */
#define ICMP_TIMESTAMPREPLY	14	/* Timestamp Reply */
#define ICMP_INFO_REQUEST	15	/* Information Request */
#define ICMP_INFO_REPLY		16	/* Information Reply */
#define ICMP_ADDRESS		17	/* Address Mask Request */
#define ICMP_ADDRESSREPLY	18	/* Address Mask Reply */
#define NR_ICMP_TYPES		18

/* Codes for UNREACH. */
#define ICMP_NET_UNREACH	0	/* Network Unreachable */
#define ICMP_HOST_UNREACH	1	/* Host Unreachable */
#define ICMP_PROT_UNREACH	2	/* Protocol Unreachable */
#define ICMP_PORT_UNREACH	3	/* Port Unreachable */
#define ICMP_FRAG_NEEDED	4	/* Fragmentation Needed/DF set */
#define ICMP_SR_FAILED		5	/* Source Route failed */
#define ICMP_NET_UNKNOWN	6
#define ICMP_HOST_UNKNOWN	7
#define ICMP_HOST_ISOLATED	8
#define ICMP_NET_ANO		9
#define ICMP_HOST_ANO		10
#define ICMP_NET_UNR_TOS	11
#define ICMP_HOST_UNR_TOS	12
#define ICMP_PKT_FILTERED	13	/* Packet filtered */
#define ICMP_PREC_VIOLATION	14	/* Precedence violation */
#define ICMP_PREC_CUTOFF	15	/* Precedence cut off */
#define NR_ICMP_UNREACH		15	/* instead of hardcoding immediate value */

/* Codes for REDIRECT. */
#define ICMP_REDIR_NET		0	/* Redirect Net */
#define ICMP_REDIR_HOST		1	/* Redirect Host */
#define ICMP_REDIR_NETTOS	2	/* Redirect Net for TOS */
#define ICMP_REDIR_HOSTTOS	3	/* Redirect Host for TOS */

/* Codes for TIME_EXCEEDED. */
#define ICMP_EXC_TTL		0	/* TTL count exceeded */
#define ICMP_EXC_FRAGTIME	1	/* Fragment Reass time exceeded */

char *
etype_string(t_uscalar_t etype)
{
	switch (etype) {
	case TBADADDR:
		return ("[TBADADDR]");
	case TBADOPT:
		return ("[TBADOPT]");
	case TACCES:
		return ("[TACCES]");
	case TBADF:
		return ("[TBADF]");
	case TNOADDR:
		return ("[TNOADDR]");
	case TOUTSTATE:
		return ("[TOUTSTATE]");
	case TBADSEQ:
		return ("[TBADSEQ]");
	case TSYSERR:
		return ("[TSYSERR]");
	case TLOOK:
		return ("[TLOOK]");
	case TBADDATA:
		return ("[TBADDATA]");
	case TBUFOVFLW:
		return ("[TBUFOVFLW]");
	case TFLOW:
		return ("[TFLOW]");
	case TNODATA:
		return ("[TNODATA]");
	case TNODIS:
		return ("[TNODIS]");
	case TNOUDERR:
		return ("[TNOUDERR]");
	case TBADFLAG:
		return ("[TBADFLAG]");
	case TNOREL:
		return ("[TNOREL]");
	case TNOTSUPPORT:
		return ("[TNOTSUPPORT]");
	case TSTATECHNG:
		return ("[TSTATECHNG]");
	case TNOSTRUCTYPE:
		return ("[TNOSTRUCTYPE]");
	case TBADNAME:
		return ("[TBADNAME]");
	case TBADQLEN:
		return ("[TBADQLEN]");
	case TADDRBUSY:
		return ("[TADDRBUSY]");
	case TINDOUT:
		return ("[TINDOUT]");
	case TPROVMISMATCH:
		return ("[TPROVMISMATCH]");
	case TRESQLEN:
		return ("[TRESQLEN]");
	case TRESADDR:
		return ("[TRESADDR]");
	case TQFULL:
		return ("[TQFULL]");
	case TPROTO:
		return ("[TPROTO]");
	default:
	{
		unsigned char code = ((etype >> 0) & 0x00ff);
		unsigned char type = ((etype >> 8) & 0x00ff);

		switch (type) {
		case ICMP_DEST_UNREACH:
			switch (code) {
			case ICMP_NET_UNREACH:
				return ("<ICMP_NET_UNREACH>");
			case ICMP_HOST_UNREACH:
				return ("<ICMP_HOST_UNREACH>");
			case ICMP_PROT_UNREACH:
				return ("<ICMP_PROT_UNREACH>");
			case ICMP_PORT_UNREACH:
				return ("<ICMP_PORT_UNREACH>");
			case ICMP_FRAG_NEEDED:
				return ("<ICMP_FRAG_NEEDED>");
			case ICMP_NET_UNKNOWN:
				return ("<ICMP_NET_UNKNOWN>");
			case ICMP_HOST_UNKNOWN:
				return ("<ICMP_HOST_UNKNOWN>");
			case ICMP_HOST_ISOLATED:
				return ("<ICMP_HOST_ISOLATED>");
			case ICMP_NET_ANO:
				return ("<ICMP_NET_ANO>");
			case ICMP_HOST_ANO:
				return ("<ICMP_HOST_ANO>");
			case ICMP_PKT_FILTERED:
				return ("<ICMP_PKT_FILTERED>");
			case ICMP_PREC_VIOLATION:
				return ("<ICMP_PREC_VIOLATION>");
			case ICMP_PREC_CUTOFF:
				return ("<ICMP_PREC_CUTOFF>");
			case ICMP_SR_FAILED:
				return ("<ICMP_SR_FAILED>");
			case ICMP_NET_UNR_TOS:
				return ("<ICMP_NET_UNR_TOS>");
			case ICMP_HOST_UNR_TOS:
				return ("<ICMP_HOST_UNR_TOS>");
			default:
				return ("<ICMP_DEST_UNREACH?>");
			}
		case ICMP_SOURCE_QUENCH:
			return ("<ICMP_SOURCE_QUENCH>");
		case ICMP_TIME_EXCEEDED:
			switch (code) {
			case ICMP_EXC_TTL:
				return ("<ICMP_EXC_TTL>");
			case ICMP_EXC_FRAGTIME:
				return ("<ICMP_EXC_FRAGTIME>");
			default:
				return ("<ICMP_TIME_EXCEEDED?>");
			}
		case ICMP_PARAMETERPROB:
			return ("<ICMP_PARAMETERPROB>");
		default:
			return ("<ICMP_???? >");
		}
	}
	}
}

const char *
event_string(int child, int event)
{
	switch (event) {
	case __EVENT_EOF:
		return ("END OF FILE");
	case __EVENT_NO_MSG:
		return ("NO MESSAGE");
	case __EVENT_TIMEOUT:
		return ("TIMEOUT");
	case __EVENT_UNKNOWN:
		return ("UNKNOWN");
	case __RESULT_DECODE_ERROR:
		return ("DECODE ERROR");
	case __RESULT_SCRIPT_ERROR:
		return ("SCRIPT ERROR");
	case __RESULT_INCONCLUSIVE:
		return ("INCONCLUSIVE");
	case __RESULT_SUCCESS:
		return ("SUCCESS");
	case __RESULT_FAILURE:
		return ("FAILURE");
	case __TEST_CONN_REQ:
		return ("T_CONN_REQ");
	case __TEST_CONN_RES:
		return ("T_CONN_RES");
	case __TEST_DISCON_REQ:
		return ("T_DISCON_REQ");
	case __TEST_DATA_REQ:
		return ("T_DATA_REQ");
	case __TEST_EXDATA_REQ:
		return ("T_EXDATA_REQ");
	case __TEST_OPTDATA_REQ:
		return ("T_OPTDATA_REQ");
	case __TEST_INFO_REQ:
		return ("T_INFO_REQ");
	case __TEST_BIND_REQ:
		return ("T_BIND_REQ");
	case __TEST_UNBIND_REQ:
		return ("T_UNBIND_REQ");
	case __TEST_UNITDATA_REQ:
		return ("T_UNITDATA_REQ");
	case __TEST_OPTMGMT_REQ:
		return ("T_OPTMGMT_REQ");
	case __TEST_ORDREL_REQ:
		return ("T_ORDREL_REQ");
	case __TEST_CONN_IND:
		return ("T_CONN_IND");
	case __TEST_CONN_CON:
		return ("T_CONN_CON");
	case __TEST_DISCON_IND:
		return ("T_DISCON_IND");
	case __TEST_DATA_IND:
		return ("T_DATA_IND");
	case __TEST_EXDATA_IND:
		return ("T_EXDATA_IND");
	case __TEST_NRM_OPTDATA_IND:
		return ("T_OPTDATA_IND(nrm)");
	case __TEST_EXP_OPTDATA_IND:
		return ("T_OPTDATA_IND(exp)");
	case __TEST_INFO_ACK:
		return ("T_INFO_ACK");
	case __TEST_BIND_ACK:
		return ("T_BIND_ACK");
	case __TEST_ERROR_ACK:
		return ("T_ERROR_ACK");
	case __TEST_OK_ACK:
		return ("T_OK_ACK");
	case __TEST_UNITDATA_IND:
		return ("T_UNITDATA_IND");
	case __TEST_UDERROR_IND:
		return ("T_UDERROR_IND");
	case __TEST_OPTMGMT_ACK:
		return ("T_OPTMGMT_ACK");
	case __TEST_ORDREL_IND:
		return ("T_ORDREL_IND");
	case __TEST_ADDR_REQ:
		return ("T_ADDR_REQ");
	case __TEST_ADDR_ACK:
		return ("T_ADDR_ACK");
	case __TEST_CAPABILITY_REQ:
		return ("T_CAPABILITY_REQ");
	case __TEST_CAPABILITY_ACK:
		return ("T_CAPABILITY_ACK");
	default:
		return ("(unexpected");
	}
}

const char *
ioctl_string(int cmd, intptr_t arg)
{
	switch (cmd) {
	case I_NREAD:
		return ("I_NREAD");
	case I_PUSH:
		return ("I_PUSH");
	case I_POP:
		return ("I_POP");
	case I_LOOK:
		return ("I_LOOK");
	case I_FLUSH:
		return ("I_FLUSH");
	case I_SRDOPT:
		return ("I_SRDOPT");
	case I_GRDOPT:
		return ("I_GRDOPT");
	case I_STR:
		if (arg) {
			struct strioctl *icp = (struct strioctl *) arg;

			switch (icp->ic_cmd) {
#if 0
			case _O_TI_BIND:
				return ("_O_TI_BIND");
			case O_TI_BIND:
				return ("O_TI_BIND");
			case _O_TI_GETINFO:
				return ("_O_TI_GETINFO");
			case O_TI_GETINFO:
				return ("O_TI_GETINFO");
			case _O_TI_GETMYNAME:
				return ("_O_TI_GETMYNAME");
			case _O_TI_GETPEERNAME:
				return ("_O_TI_GETPEERNAME");
			case _O_TI_OPTMGMT:
				return ("_O_TI_OPTMGMT");
			case O_TI_OPTMGMT:
				return ("O_TI_OPTMGMT");
			case _O_TI_TLI_MODE:
				return ("_O_TI_TLI_MODE");
			case _O_TI_UNBIND:
				return ("_O_TI_UNBIND");
			case O_TI_UNBIND:
				return ("O_TI_UNBIND");
			case _O_TI_XTI_CLEAR_EVENT:
				return ("_O_TI_XTI_CLEAR_EVENT");
			case _O_TI_XTI_GET_STATE:
				return ("_O_TI_XTI_GET_STATE");
			case _O_TI_XTI_HELLO:
				return ("_O_TI_XTI_HELLO");
			case _O_TI_XTI_MODE:
				return ("_O_TI_XTI_MODE");
			case TI_BIND:
				return ("TI_BIND");
			case TI_CAPABILITY:
				return ("TI_CAPABILITY");
			case TI_GETADDRS:
				return ("TI_GETADDRS");
			case TI_GETINFO:
				return ("TI_GETINFO");
			case TI_GETMYNAME:
				return ("TI_GETMYNAME");
			case TI_GETPEERNAME:
				return ("TI_GETPEERNAME");
			case TI_OPTMGMT:
				return ("TI_OPTMGMT");
			case TI_SETMYNAME:
				return ("TI_SETMYNAME");
			case TI_SETPEERNAME:
				return ("TI_SETPEERNAME");
			case TI_SYNC:
				return ("TI_SYNC");
			case TI_UNBIND:
				return ("TI_UNBIND");
#endif
			}
		}
		return ("I_STR");
	case I_SETSIG:
		return ("I_SETSIG");
	case I_GETSIG:
		return ("I_GETSIG");
	case I_FIND:
		return ("I_FIND");
	case I_LINK:
		return ("I_LINK");
	case I_UNLINK:
		return ("I_UNLINK");
	case I_RECVFD:
		return ("I_RECVFD");
	case I_PEEK:
		return ("I_PEEK");
	case I_FDINSERT:
		return ("I_FDINSERT");
	case I_SENDFD:
		return ("I_SENDFD");
#if 0
	case I_E_RECVFD:
		return ("I_E_RECVFD");
#endif
	case I_SWROPT:
		return ("I_SWROPT");
	case I_GWROPT:
		return ("I_GWROPT");
	case I_LIST:
		return ("I_LIST");
	case I_PLINK:
		return ("I_PLINK");
	case I_PUNLINK:
		return ("I_PUNLINK");
	case I_FLUSHBAND:
		return ("I_FLUSHBAND");
	case I_CKBAND:
		return ("I_CKBAND");
	case I_GETBAND:
		return ("I_GETBAND");
	case I_ATMARK:
		return ("I_ATMARK");
	case I_SETCLTIME:
		return ("I_SETCLTIME");
	case I_GETCLTIME:
		return ("I_GETCLTIME");
	case I_CANPUT:
		return ("I_CANPUT");
#if 0
	case I_SERROPT:
		return ("I_SERROPT");
	case I_GERROPT:
		return ("I_GERROPT");
	case I_ANCHOR:
		return ("I_ANCHOR");
#endif
#if 0
	case I_S_RECVFD:
		return ("I_S_RECVFD");
	case I_STATS:
		return ("I_STATS");
	case I_BIGPIPE:
		return ("I_BIGPIPE");
#endif
#if 0
	case I_GETTP:
		return ("I_GETTP");
	case I_AUTOPUSH:
		return ("I_AUTOPUSH");
	case I_HEAP_REPORT:
		return ("I_HEAP_REPORT");
	case I_FIFO:
		return ("I_FIFO");
	case I_PUTPMSG:
		return ("I_PUTPMSG");
	case I_GETPMSG:
		return ("I_GETPMSG");
	case I_FATTACH:
		return ("I_FATTACH");
	case I_FDETACH:
		return ("I_FDETACH");
	case I_PIPE:
		return ("I_PIPE");
#endif
	default:
		return ("(unexpected)");
	}
}

const char *
signal_string(int signum)
{
	switch (signum) {
	case SIGHUP:
		return ("SIGHUP");
	case SIGINT:
		return ("SIGINT");
	case SIGQUIT:
		return ("SIGQUIT");
	case SIGILL:
		return ("SIGILL");
	case SIGABRT:
		return ("SIGABRT");
	case SIGFPE:
		return ("SIGFPE");
	case SIGKILL:
		return ("SIGKILL");
	case SIGSEGV:
		return ("SIGSEGV");
	case SIGPIPE:
		return ("SIGPIPE");
	case SIGALRM:
		return ("SIGALRM");
	case SIGTERM:
		return ("SIGTERM");
	case SIGUSR1:
		return ("SIGUSR1");
	case SIGUSR2:
		return ("SIGUSR2");
	case SIGCHLD:
		return ("SIGCHLD");
	case SIGCONT:
		return ("SIGCONT");
	case SIGSTOP:
		return ("SIGSTOP");
	case SIGTSTP:
		return ("SIGTSTP");
	case SIGTTIN:
		return ("SIGTTIN");
	case SIGTTOU:
		return ("SIGTTOU");
	case SIGBUS:
		return ("SIGBUS");
	case SIGPOLL:
		return ("SIGPOLL");
	case SIGPROF:
		return ("SIGPROF");
	case SIGSYS:
		return ("SIGSYS");
	case SIGTRAP:
		return ("SIGTRAP");
	case SIGURG:
		return ("SIGURG");
	case SIGVTALRM:
		return ("SIGVTALRM");
	case SIGXCPU:
		return ("SIGXCPU");
	case SIGXFSZ:
		return ("SIGXFSZ");
	default:
		return ("unknown");
	}
}

const char *
poll_string(short events)
{
	if (events & POLLIN)
		return ("POLLIN");
	if (events & POLLPRI)
		return ("POLLPRI");
	if (events & POLLOUT)
		return ("POLLOUT");
	if (events & POLLRDNORM)
		return ("POLLRDNORM");
	if (events & POLLRDBAND)
		return ("POLLRDBAND");
	if (events & POLLWRNORM)
		return ("POLLWRNORM");
	if (events & POLLWRBAND)
		return ("POLLWRBAND");
	if (events & POLLERR)
		return ("POLLERR");
	if (events & POLLHUP)
		return ("POLLHUP");
	if (events & POLLNVAL)
		return ("POLLNVAL");
	if (events & POLLMSG)
		return ("POLLMSG");
	return ("none");
}

const char *
poll_events_string(short events)
{
	static char string[256] = "";
	int offset = 0, size = 256, len = 0;

	if (events & POLLIN) {
		len = snprintf(string + offset, size, "POLLIN|");
		offset += len;
		size -= len;
	}
	if (events & POLLPRI) {
		len = snprintf(string + offset, size, "POLLPRI|");
		offset += len;
		size -= len;
	}
	if (events & POLLOUT) {
		len = snprintf(string + offset, size, "POLLOUT|");
		offset += len;
		size -= len;
	}
	if (events & POLLRDNORM) {
		len = snprintf(string + offset, size, "POLLRDNORM|");
		offset += len;
		size -= len;
	}
	if (events & POLLRDBAND) {
		len = snprintf(string + offset, size, "POLLRDBAND|");
		offset += len;
		size -= len;
	}
	if (events & POLLWRNORM) {
		len = snprintf(string + offset, size, "POLLWRNORM|");
		offset += len;
		size -= len;
	}
	if (events & POLLWRBAND) {
		len = snprintf(string + offset, size, "POLLWRBAND|");
		offset += len;
		size -= len;
	}
	if (events & POLLERR) {
		len = snprintf(string + offset, size, "POLLERR|");
		offset += len;
		size -= len;
	}
	if (events & POLLHUP) {
		len = snprintf(string + offset, size, "POLLHUP|");
		offset += len;
		size -= len;
	}
	if (events & POLLNVAL) {
		len = snprintf(string + offset, size, "POLLNVAL|");
		offset += len;
		size -= len;
	}
	if (events & POLLMSG) {
		len = snprintf(string + offset, size, "POLLMSG|");
		offset += len;
		size -= len;
	}
	return (string);
}

const char *
service_type(t_uscalar_t type)
{
	switch (type) {
	case T_CLTS:
		return ("T_CLTS");
	case T_COTS:
		return ("T_COTS");
	case T_COTS_ORD:
		return ("T_COTS_ORD");
	default:
		return ("(unknown)");
	}
}

const char *
state_string(t_uscalar_t state)
{
	switch (state) {
	case TS_UNBND:
		return ("TS_UNBND");
	case TS_WACK_BREQ:
		return ("TS_WACK_BREQ");
	case TS_WACK_UREQ:
		return ("TS_WACK_UREQ");
	case TS_IDLE:
		return ("TS_IDLE");
	case TS_WACK_OPTREQ:
		return ("TS_WACK_OPTREQ");
	case TS_WACK_CREQ:
		return ("TS_WACK_CREQ");
	case TS_WCON_CREQ:
		return ("TS_WCON_CREQ");
	case TS_WRES_CIND:
		return ("TS_WRES_CIND");
	case TS_WACK_CRES:
		return ("TS_WACK_CRES");
	case TS_DATA_XFER:
		return ("TS_DATA_XFER");
	case TS_WIND_ORDREL:
		return ("TS_WIND_ORDREL");
	case TS_WREQ_ORDREL:
		return ("TS_WRES_ORDREL");
	case TS_WACK_DREQ6:
		return ("TS_WACK_DREQ6");
	case TS_WACK_DREQ7:
		return ("TS_WACK_DREQ7");
	case TS_WACK_DREQ9:
		return ("TS_WACK_DREQ9");
	case TS_WACK_DREQ10:
		return ("TS_WACK_DREQ10");
	case TS_WACK_DREQ11:
		return ("TS_WACK_DREQ11");
	default:
		return ("(unknown)");
	}
}

#ifndef SCTP_VERSION_2
void
print_addr(char *add_ptr, size_t add_len)
{
	sctp_addr_t *a = (sctp_addr_t *) add_ptr;
	size_t anum = add_len >= sizeof(a->port) ? (add_len - sizeof(a->port)) / sizeof(a->addr[0]) : 0;

	dummy = lockf(fileno(stdout), F_LOCK, 0);
	if (add_len) {
		int i;

		if (add_len != sizeof(a->port) + anum * sizeof(a->addr[0]))
			fprintf(stdout, "Aaarrg! add_len = %d, anum = %d, ", add_len, anum);
		fprintf(stdout, "[%d]", ntohs(a->port));
		for (i = 0; i < anum; i++) {
			fprintf(stdout, "%s%d.%d.%d.%d", i ? "," : "", (a->addr[i] >> 0) & 0xff, (a->addr[i] >> 8) & 0xff, (a->addr[i] >> 16) & 0xff, (a->addr[i] >> 24) & 0xff);
		}
	} else
		fprintf(stdout, "(no address)");
	fprintf(stdout, "\n");
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

char *
addr_string(char *add_ptr, size_t add_len)
{
	static char buf[128];
	size_t len = 0;
	sctp_addr_t *a = (sctp_addr_t *) add_ptr;
	size_t anum = add_len >= sizeof(a->port) ? (add_len - sizeof(a->port)) / sizeof(a->addr[0]) : 0;

	if (add_len) {
		int i;

		if (add_len != sizeof(a->port) + anum * sizeof(a->addr[0]))
			len += snprintf(buf + len, sizeof(buf) - len, "Aaarrg! add_len = %d, anum = %d, ", add_len, anum);
		len += snprintf(buf + len, sizeof(buf) - len, "[%d]", ntohs(a->port));
		for (i = 0; i < anum; i++) {
			len += snprintf(buf + len, sizeof(buf) - len, "%s%d.%d.%d.%d", i ? "," : "", (a->addr[i] >> 0) & 0xff, (a->addr[i] >> 8) & 0xff, (a->addr[i] >> 16) & 0xff, (a->addr[i] >> 24) & 0xff);
		}
	} else
		len += snprintf(buf + len, sizeof(buf) - len, "(no address)");
	/* len += snprintf(buf + len, sizeof(buf) - len, "\0"); */
	return buf;
}

void
print_addrs(int fd, char *add_ptr, size_t add_len)
{
	fprintf(stdout, "Stupid!\n");
}
#else				/* SCTP_VERSION_2 */
void
print_addr(char *add_ptr, size_t add_len)
{
	struct sockaddr_in *a = (struct sockaddr_in *) add_ptr;
	size_t anum = add_len / sizeof(*a);

	dummy = lockf(fileno(stdout), F_LOCK, 0);
	if (add_len > 0) {
		int i;

		if (add_len != anum * sizeof(*a))
			fprintf(stdout, "Aaarrg! add_len = %lu, anum = %lu, ", (ulong) add_len, (ulong) anum);
		fprintf(stdout, "[%d]", ntohs(a[0].sin_port));
		for (i = 0; i < anum; i++) {
			uint32_t addr = a[i].sin_addr.s_addr;

			fprintf(stdout, "%s%d.%d.%d.%d", i ? "," : "", (addr >> 0) & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
		}
	} else
		fprintf(stdout, "(no address)");
	fprintf(stdout, "\n");
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

char *
addr_string(char *add_ptr, size_t add_len)
{
	static char buf[128];
	size_t len = 0;
	struct sockaddr_in *a = (struct sockaddr_in *) add_ptr;
	size_t anum = add_len / sizeof(*a);

	if (add_len > 0) {
		int i;

		if (add_len != anum * sizeof(*a))
			len += snprintf(buf + len, sizeof(buf) - len, "Aaarrg! add_len = %lu, anum = %lu, ", (ulong) add_len, (ulong) anum);
		len += snprintf(buf + len, sizeof(buf) - len, "[%d]", ntohs(a[0].sin_port));
		for (i = 0; i < anum; i++) {
			uint32_t addr = a[i].sin_addr.s_addr;

			len += snprintf(buf + len, sizeof(buf) - len, "%s%d.%d.%d.%d", i ? "," : "", (addr >> 0) & 0xff, (addr >> 8) & 0xff, (addr >> 16) & 0xff, (addr >> 24) & 0xff);
		}
	} else
		len += snprintf(buf + len, sizeof(buf) - len, "(no address)");
	/* len += snprintf(buf + len, sizeof(buf) - len, "\0"); */
	return buf;
}
void print_string(int child, const char *string);
void
print_addrs(int child, char *add_ptr, size_t add_len)
{
	struct sockaddr_in *sin;

	if (verbose < 3 || !show)
		return;
	if (add_len == 0)
		print_string(child, "(no address)");
	for (sin = (typeof(sin)) add_ptr; add_len >= sizeof(*sin); sin++, add_len -= sizeof(*sin)) {
		char buf[128];

		snprintf(buf, sizeof(buf), "%d.%d.%d.%d:%d", (sin->sin_addr.s_addr >> 0) & 0xff, (sin->sin_addr.s_addr >> 8) & 0xff, (sin->sin_addr.s_addr >> 16) & 0xff, (sin->sin_addr.s_addr >> 24) & 0xff, ntohs(sin->sin_port));
		print_string(child, buf);
	}
}
#endif				/* SCTP_VERSION_2 */

char *
prot_string(char *pro_ptr, size_t pro_len)
{
	static char buf[128];
	size_t len = 0;
	int i;

	buf[0] = 0;
	for (i = 0; i < pro_len; i++) {
		len += snprintf(buf + len, sizeof(buf) - len, "%u ", (uint) (unsigned char) pro_ptr[i]);
	}
	return (buf);
}

void print_string(int child, const char *string);
void
print_prots(int child, char *pro_ptr, size_t pro_len)
{
	unsigned char *prot;

	if (verbose < 3 || !show)
		return;
	for (prot = (typeof(prot)) pro_ptr; pro_len > 0; prot++, pro_len--) {
		char buf[32];

		snprintf(buf, sizeof(buf), "<%u>", (unsigned int) *prot);
		print_string(child, buf);
	}
}

#if 1
char *
status_string(struct t_opthdr *oh)
{
	switch (oh->status) {
	case 0:
		return (NULL);
	case T_SUCCESS:
		return ("T_SUCCESS");
	case T_FAILURE:
		return ("T_FAILURE");
	case T_PARTSUCCESS:
		return ("T_PARTSUCCESS");
	case T_READONLY:
		return ("T_READONLY");
	case T_NOTSUPPORT:
		return ("T_NOTSUPPORT");
	default:
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "(unknown status %ld)", (long) oh->status);
		return buf;
	}
	}
}

#ifndef T_ALLLEVELS
#define T_ALLLEVELS -1
#endif

char *
level_string(struct t_opthdr *oh)
{
	switch (oh->level) {
	case T_ALLLEVELS:
		return ("T_ALLLEVELS");
	case XTI_GENERIC:
		return ("XTI_GENERIC");
	case T_INET_IP:
		return ("T_INET_IP");
	case T_INET_UDP:
		return ("T_INET_UDP");
	case T_INET_TCP:
		return ("T_INET_TCP");
	case T_INET_SCTP:
		return ("T_INET_SCTP");
	default:
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "(unknown level %ld)", (long) oh->level);
		return buf;
	}
	}
}

char *
name_string(struct t_opthdr *oh)
{
	if (oh->name == T_ALLOPT)
		return ("T_ALLOPT");
	switch (oh->level) {
	case XTI_GENERIC:
		switch (oh->name) {
		case XTI_DEBUG:
			return ("XTI_DEBUG");
		case XTI_LINGER:
			return ("XTI_LINGER");
		case XTI_RCVBUF:
			return ("XTI_RCVBUF");
		case XTI_RCVLOWAT:
			return ("XTI_RCVLOWAT");
		case XTI_SNDBUF:
			return ("XTI_SNDBUF");
		case XTI_SNDLOWAT:
			return ("XTI_SNDLOWAT");
		}
		break;
	case T_INET_IP:
		switch (oh->name) {
		case T_IP_OPTIONS:
			return ("T_IP_OPTIONS");
		case T_IP_TOS:
			return ("T_IP_TOS");
		case T_IP_TTL:
			return ("T_IP_TTL");
		case T_IP_REUSEADDR:
			return ("T_IP_REUSEADDR");
		case T_IP_DONTROUTE:
			return ("T_IP_DONTROUTE");
		case T_IP_BROADCAST:
			return ("T_IP_BROADCAST");
		case T_IP_ADDR:
			return ("T_IP_ADDR");
		}
		break;
	case T_INET_UDP:
		switch (oh->name) {
		case T_UDP_CHECKSUM:
			return ("T_UDP_CHECKSUM");
		}
		break;
	case T_INET_TCP:
		switch (oh->name) {
		case T_TCP_NODELAY:
			return ("T_TCP_NODELAY");
		case T_TCP_MAXSEG:
			return ("T_TCP_MAXSEG");
		case T_TCP_KEEPALIVE:
			return ("T_TCP_KEEPALIVE");
		case T_TCP_CORK:
			return ("T_TCP_CORK");
		case T_TCP_KEEPIDLE:
			return ("T_TCP_KEEPIDLE");
		case T_TCP_KEEPINTVL:
			return ("T_TCP_KEEPINTVL");
		case T_TCP_KEEPCNT:
			return ("T_TCP_KEEPCNT");
		case T_TCP_SYNCNT:
			return ("T_TCP_SYNCNT");
		case T_TCP_LINGER2:
			return ("T_TCP_LINGER2");
		case T_TCP_DEFER_ACCEPT:
			return ("T_TCP_DEFER_ACCEPT");
		case T_TCP_WINDOW_CLAMP:
			return ("T_TCP_WINDOW_CLAMP");
		case T_TCP_INFO:
			return ("T_TCP_INFO");
		case T_TCP_QUICKACK:
			return ("T_TCP_QUICKACK");
		}
		break;
	case T_INET_SCTP:
		switch (oh->name) {
		case T_SCTP_NODELAY:
			return ("T_SCTP_NODELAY");
		case T_SCTP_CORK:
			return ("T_SCTP_CORK");
		case T_SCTP_PPI:
			return ("T_SCTP_PPI");
		case T_SCTP_SID:
			return ("T_SCTP_SID");
		case T_SCTP_SSN:
			return ("T_SCTP_SSN");
		case T_SCTP_TSN:
			return ("T_SCTP_TSN");
		case T_SCTP_RECVOPT:
			return ("T_SCTP_RECVOPT");
		case T_SCTP_COOKIE_LIFE:
			return ("T_SCTP_COOKIE_LIFE");
		case T_SCTP_SACK_DELAY:
			return ("T_SCTP_SACK_DELAY");
		case T_SCTP_PATH_MAX_RETRANS:
			return ("T_SCTP_PATH_MAX_RETRANS");
		case T_SCTP_ASSOC_MAX_RETRANS:
			return ("T_SCTP_ASSOC_MAX_RETRANS");
		case T_SCTP_MAX_INIT_RETRIES:
			return ("T_SCTP_MAX_INIT_RETRIES");
		case T_SCTP_HEARTBEAT_ITVL:
			return ("T_SCTP_HEARTBEAT_ITVL");
		case T_SCTP_RTO_INITIAL:
			return ("T_SCTP_RTO_INITIAL");
		case T_SCTP_RTO_MIN:
			return ("T_SCTP_RTO_MIN");
		case T_SCTP_RTO_MAX:
			return ("T_SCTP_RTO_MAX");
		case T_SCTP_OSTREAMS:
			return ("T_SCTP_OSTREAMS");
		case T_SCTP_ISTREAMS:
			return ("T_SCTP_ISTREAMS");
		case T_SCTP_COOKIE_INC:
			return ("T_SCTP_COOKIE_INC");
		case T_SCTP_THROTTLE_ITVL:
			return ("T_SCTP_THROTTLE_ITVL");
		case T_SCTP_MAC_TYPE:
			return ("T_SCTP_MAC_TYPE");
		case T_SCTP_CKSUM_TYPE:
			return ("T_SCTP_CKSUM_TYPE");
		case T_SCTP_ECN:
			return ("T_SCTP_ECN");
		case T_SCTP_ALI:
			return ("T_SCTP_ALI");
		case T_SCTP_ADD:
			return ("T_SCTP_ADD");
		case T_SCTP_SET:
			return ("T_SCTP_SET");
		case T_SCTP_ADD_IP:
			return ("T_SCTP_ADD_IP");
		case T_SCTP_DEL_IP:
			return ("T_SCTP_DEL_IP");
		case T_SCTP_SET_IP:
			return ("T_SCTP_SET_IP");
		case T_SCTP_PR:
			return ("T_SCTP_PR");
		case T_SCTP_LIFETIME:
			return ("T_SCTP_LIFETIME");
		case T_SCTP_DISPOSITION:
			return ("T_SCTP_DISPOSITION");
		case T_SCTP_MAX_BURST:
			return ("T_SCTP_MAX_BURST");
		case T_SCTP_HB:
			return ("T_SCTP_HB");
		case T_SCTP_RTO:
			return ("T_SCTP_RTO");
		case T_SCTP_MAXSEG:
			return ("T_SCTP_MAXSEG");
		case T_SCTP_STATUS:
			return ("T_SCTP_STATUS");
		case T_SCTP_DEBUG:
			return ("T_SCTP_DEBUG");
		}
		break;
	}
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "(unknown name %ld)", (long) oh->name);
		return buf;
	}
}

char *
yesno_string(struct t_opthdr *oh)
{
	switch (*((t_uscalar_t *) T_OPT_DATA(oh))) {
	case T_YES:
		return ("T_YES");
	case T_NO:
		return ("T_NO");
	default:
		return ("(invalid)");
	}
}

char *
number_string(struct t_opthdr *oh)
{
	static char buf[32];

	snprintf(buf, 32, "%d", *((t_scalar_t *) T_OPT_DATA(oh)));
	return (buf);
}

char *
value_string(int child, struct t_opthdr *oh)
{
	static char buf[64] = "(invalid)";

	if (oh->len == sizeof(*oh))
		return (NULL);
	switch (oh->level) {
	case XTI_GENERIC:
		switch (oh->name) {
		case XTI_DEBUG:
			break;
		case XTI_LINGER:
			break;
		case XTI_RCVBUF:
			break;
		case XTI_RCVLOWAT:
			break;
		case XTI_SNDBUF:
			break;
		case XTI_SNDLOWAT:
			break;
		}
		break;
	case T_INET_IP:
		switch (oh->name) {
		case T_IP_OPTIONS:
			break;
		case T_IP_TOS:
			if (oh->len == sizeof(*oh) + sizeof(unsigned char))
				snprintf(buf, sizeof(buf), "0x%02x", *((unsigned char *) T_OPT_DATA(oh)));
			return buf;
		case T_IP_TTL:
			if (oh->len == sizeof(*oh) + sizeof(unsigned char))
				snprintf(buf, sizeof(buf), "0x%02x", *((unsigned char *) T_OPT_DATA(oh)));
			return buf;
		case T_IP_REUSEADDR:
			return yesno_string(oh);
		case T_IP_DONTROUTE:
			return yesno_string(oh);
		case T_IP_BROADCAST:
			return yesno_string(oh);
		case T_IP_ADDR:
			if (oh->len == sizeof(*oh) + sizeof(uint32_t)) {
				uint32_t addr = *((uint32_t *) T_OPT_DATA(oh));

				snprintf(buf, sizeof(buf), "%d.%d.%d.%d", (addr >> 0) & 0x00ff, (addr >> 8) & 0x00ff, (addr >> 16) & 0x00ff, (addr >> 24) & 0x00ff);
			}
			return buf;
		}
		break;
	case T_INET_UDP:
		switch (oh->name) {
		case T_UDP_CHECKSUM:
			return yesno_string(oh);
		}
		break;
	case T_INET_TCP:
		switch (oh->name) {
		case T_TCP_NODELAY:
			return yesno_string(oh);
		case T_TCP_MAXSEG:
			if (oh->len == sizeof(*oh) + sizeof(t_uscalar_t))
				snprintf(buf, sizeof(buf), "%lu", (ulong) *((t_uscalar_t *) T_OPT_DATA(oh)));
			return buf;
		case T_TCP_KEEPALIVE:
			break;
		case T_TCP_CORK:
			return yesno_string(oh);
		case T_TCP_KEEPIDLE:
			break;
		case T_TCP_KEEPINTVL:
			break;
		case T_TCP_KEEPCNT:
			break;
		case T_TCP_SYNCNT:
			break;
		case T_TCP_LINGER2:
			break;
		case T_TCP_DEFER_ACCEPT:
			break;
		case T_TCP_WINDOW_CLAMP:
			break;
		case T_TCP_INFO:
			break;
		case T_TCP_QUICKACK:
			break;
		}
		break;
	case T_INET_SCTP:
		switch (oh->name) {
		case T_SCTP_NODELAY:
			return yesno_string(oh);
		case T_SCTP_CORK:
			return yesno_string(oh);
		case T_SCTP_PPI:
			return number_string(oh);;
		case T_SCTP_SID:
#if 1
			sid[child] = *((t_uscalar_t *) T_OPT_DATA(oh));
#endif
			return number_string(oh);;
		case T_SCTP_SSN:
		case T_SCTP_TSN:
			return number_string(oh);;
		case T_SCTP_RECVOPT:
			return yesno_string(oh);
		case T_SCTP_COOKIE_LIFE:
		case T_SCTP_SACK_DELAY:
		case T_SCTP_PATH_MAX_RETRANS:
		case T_SCTP_ASSOC_MAX_RETRANS:
		case T_SCTP_MAX_INIT_RETRIES:
		case T_SCTP_HEARTBEAT_ITVL:
		case T_SCTP_RTO_INITIAL:
		case T_SCTP_RTO_MIN:
		case T_SCTP_RTO_MAX:
		case T_SCTP_OSTREAMS:
		case T_SCTP_ISTREAMS:
		case T_SCTP_COOKIE_INC:
		case T_SCTP_THROTTLE_ITVL:
			return number_string(oh);;
		case T_SCTP_MAC_TYPE:
			break;
		case T_SCTP_CKSUM_TYPE:
			break;
		case T_SCTP_ECN:
			break;
		case T_SCTP_ALI:
			break;
		case T_SCTP_ADD:
			break;
		case T_SCTP_SET:
			break;
		case T_SCTP_ADD_IP:
			break;
		case T_SCTP_DEL_IP:
			break;
		case T_SCTP_SET_IP:
			break;
		case T_SCTP_PR:
			break;
		case T_SCTP_LIFETIME:
		case T_SCTP_DISPOSITION:
		case T_SCTP_MAX_BURST:
		case T_SCTP_HB:
		case T_SCTP_RTO:
		case T_SCTP_MAXSEG:
			return number_string(oh);
		case T_SCTP_STATUS:
			break;
		case T_SCTP_DEBUG:
			break;
		}
		break;
	}
	return ("(unknown value)");
}
#endif

#if 1
void
parse_options(int fd, char *opt_ptr, size_t opt_len)
{
	struct t_opthdr *oh;

	for (oh = _T_OPT_FIRSTHDR_OFS(opt_ptr, opt_len, 0); oh; oh = _T_OPT_NEXTHDR_OFS(opt_ptr, opt_len, oh, 0)) {
		if (oh->len == sizeof(*oh))
			continue;
		switch (oh->level) {
		case T_INET_SCTP:
			switch (oh->name) {
			case T_SCTP_PPI:
				ppi[fd] = *((t_uscalar_t *) T_OPT_DATA(oh));
				continue;
			case T_SCTP_SID:
				sid[fd] = *((t_uscalar_t *) T_OPT_DATA(oh));
				continue;
			case T_SCTP_SSN:
				ssn[fd] = *((t_uscalar_t *) T_OPT_DATA(oh));
				continue;
			case T_SCTP_TSN:
				tsn[fd] = *((t_uscalar_t *) T_OPT_DATA(oh));
				continue;
			}
		}
	}
}
#endif

#if 1
char *
mgmtflag_string(t_uscalar_t flag)
{
	switch (flag) {
	case T_NEGOTIATE:
		return ("T_NEGOTIATE");
	case T_CHECK:
		return ("T_CHECK");
	case T_DEFAULT:
		return ("T_DEFAULT");
	case T_SUCCESS:
		return ("T_SUCCESS");
	case T_FAILURE:
		return ("T_FAILURE");
	case T_CURRENT:
		return ("T_CURRENT");
	case T_PARTSUCCESS:
		return ("T_PARTSUCCESS");
	case T_READONLY:
		return ("T_READONLY");
	case T_NOTSUPPORT:
		return ("T_NOTSUPPORT");
	}
	return "(unknown flag)";
}
#else
const char *
mgmtflag_string(np_ulong flag)
{
	switch (flag) {
	case 0:
		return ("(none)");
	case DEFAULT_RC_SEL:
		return ("DEFAULT_RC_SEL");
	default:
		return ("(unknown flags)");
	}
}
#endif

#if 1
char *
size_string(t_uscalar_t size)
{
	static char buf[128];

	switch (size) {
	case T_INFINITE:
		return ("T_INFINITE");
	case T_INVALID:
		return ("T_INVALID");
	case T_UNSPEC:
		return ("T_UNSPEC");
	}
	snprintf(buf, sizeof(buf), "%lu", (ulong) size);
	return buf;
}
#endif

const char *
prim_string(t_uscalar_t prim)
{
	switch (prim) {
	case T_CONN_REQ:
		return ("T_CONN_REQ------");
	case T_CONN_RES:
		return ("T_CONN_RES------");
	case T_DISCON_REQ:
		return ("T_DISCON_REQ----");
	case T_DATA_REQ:
		return ("T_DATA_REQ------");
	case T_EXDATA_REQ:
		return ("T_EXDATA_REQ----");
	case T_INFO_REQ:
		return ("T_INFO_REQ------");
	case T_BIND_REQ:
		return ("T_BIND_REQ------");
	case T_UNBIND_REQ:
		return ("T_UNBIND_REQ----");
	case T_UNITDATA_REQ:
		return ("T_UNITDATA_REQ--");
	case T_OPTMGMT_REQ:
		return ("T_OPTMGMT_REQ---");
	case T_ORDREL_REQ:
		return ("T_ORDREL_REQ----");
	case T_OPTDATA_REQ:
		return ("T_OPTDATA_REQ---");
	case T_ADDR_REQ:
		return ("T_ADDR_REQ------");
	case T_CAPABILITY_REQ:
		return ("T_CAPABILITY_REQ");
	case T_CONN_IND:
		return ("T_CONN_IND------");
	case T_CONN_CON:
		return ("T_CONN_CON------");
	case T_DISCON_IND:
		return ("T_DISCON_IND----");
	case T_DATA_IND:
		return ("T_DATA_IND------");
	case T_EXDATA_IND:
		return ("T_EXDATA_IND----");
	case T_INFO_ACK:
		return ("T_INFO_ACK------");
	case T_BIND_ACK:
		return ("T_BIND_ACK------");
	case T_ERROR_ACK:
		return ("T_ERROR_ACK-----");
	case T_OK_ACK:
		return ("T_OK_ACK--------");
	case T_UNITDATA_IND:
		return ("T_UNITDATA_IND--");
	case T_UDERROR_IND:
		return ("T_UDERROR_IND---");
	case T_OPTMGMT_ACK:
		return ("T_OPTMGMT_ACK---");
	case T_ORDREL_IND:
		return ("T_ORDREL_IND----");
	case T_OPTDATA_IND:
		return ("T_OPTDATA_IND---");
	case T_ADDR_ACK:
		return ("T_ADDR_ACK------");
	case T_CAPABILITY_ACK:
		return ("T_CAPABILITY_ACK");
	default:
		return ("T_????_??? -----");
	}
}

char *
t_errno_string(t_scalar_t err, t_scalar_t syserr)
{
	switch (err) {
	case 0:
		return ("ok");
	case TBADADDR:
		return ("[TBADADDR]");
	case TBADOPT:
		return ("[TBADOPT]");
	case TACCES:
		return ("[TACCES]");
	case TBADF:
		return ("[TBADF]");
	case TNOADDR:
		return ("[TNOADDR]");
	case TOUTSTATE:
		return ("[TOUTSTATE]");
	case TBADSEQ:
		return ("[TBADSEQ]");
	case TSYSERR:
		return errno_string(syserr);
	case TLOOK:
		return ("[TLOOK]");
	case TBADDATA:
		return ("[TBADDATA]");
	case TBUFOVFLW:
		return ("[TBUFOVFLW]");
	case TFLOW:
		return ("[TFLOW]");
	case TNODATA:
		return ("[TNODATA]");
	case TNODIS:
		return ("[TNODIS]");
	case TNOUDERR:
		return ("[TNOUDERR]");
	case TBADFLAG:
		return ("[TBADFLAG]");
	case TNOREL:
		return ("[TNOREL]");
	case TNOTSUPPORT:
		return ("[TNOTSUPPORT]");
	case TSTATECHNG:
		return ("[TSTATECHNG]");
	case TNOSTRUCTYPE:
		return ("[TNOSTRUCTYPE]");
	case TBADNAME:
		return ("[TBADNAME]");
	case TBADQLEN:
		return ("[TBADQLEN]");
	case TADDRBUSY:
		return ("[TADDRBUSY]");
	case TINDOUT:
		return ("[TINDOUT]");
	case TPROVMISMATCH:
		return ("[TPROVMISMATCH]");
	case TRESQLEN:
		return ("[TRESQLEN]");
	case TRESADDR:
		return ("[TRESADDR]");
	case TQFULL:
		return ("[TQFULL]");
	case TPROTO:
		return ("[TPROTO]");
	default:
	{
		static char buf[32];

		snprintf(buf, sizeof(buf), "[%ld]", (long) err);
		return buf;
	}
	}
}


void
print_simple(int child, const char *msgs[])
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child]);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_simple_int(int child, const char *msgs[], int val)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], val);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_double_int(int child, const char *msgs[], int val, int val2)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], val, val2);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_triple_int(int child, const char *msgs[], int val, int val2, int val3)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], val, val2, val3);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_simple_string(int child, const char *msgs[], const char *string)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_string_state(int child, const char *msgs[], const char *string)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_triple_string(int child, const char *msgs[], const char *string)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], "", child, state);
	fprintf(stdout, msgs[child], string, child, state);
	fprintf(stdout, msgs[child], "", child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_more(int child)
{
	show = 1;
}

void
print_less(int child)
{
	static const char *msgs[] = {
		"         . %1$6.6s . | <------         .         ------> :                    [%2$d:%3$03d]\n",
		"                    : <------         .         ------> | . %1$-6.6s .         [%2$d:%3$03d]\n",
		"                    : <------         .      ------> |  : . %1$-6.6s .         [%2$d:%3$03d]\n",
		"         . %1$6.6s . : <------         .      ------> :> : . %1$-6.6s .         [%2$d:%3$03d]\n",
	};

	if (show && verbose > 0)
		print_triple_string(child, msgs, "(more)");
	show = 0;
}

void
print_pipe(int child)
{
	static const char *msgs[] = {
		"  pipe()      ----->v  v<------------------------------>v                   \n",
		"                    v  v<------------------------------>v<-----     pipe()  \n",
		"                    .  .                                .                   \n",
	};

	if (show && verbose > 3)
		print_simple(child, msgs);
}

void
print_open(int child, const char *name)
{
	static const char *msgs[] = {
		"  open()      ----->v %-30.30s    .                   \n",
		"                    | %-30.30s    v<-----     open()  \n",
		"                    | %-30.30s v<-+------     open()  \n",
		"                    . %-30.30s .  .                   \n",
	};

	if (show && verbose > 3)
		print_simple_string(child, msgs, name);
}

void
print_close(int child)
{
	static const char *msgs[] = {
		"  close()     ----->X                                   |                   \n",
		"                    .                                   X<-----    close()  \n",
		"                    .                                X<-+------    close()  \n",
		"                    .                                .  .                   \n",
	};

	if (show && verbose > 3)
		print_simple(child, msgs);
}

void
print_preamble(int child)
{
	static const char *msgs[] = {
		"--------------------+-------------Preamble--------------+                   \n",
		"                    +-------------Preamble--------------+-------------------\n",
		"                    +-------------Preamble-----------+--+-------------------\n",
		"--------------------+-------------Preamble--------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_failure(int child, const char *string)
{
	static const char *msgs[] = {
		"....................|%-32.32s   |                    [%d:%03d]\n",
		"                    |%-32.32s   |................... [%d:%03d]\n",
		"                    |%-32.32s|...................... [%d:%03d]\n",
		"....................|%-32.32s...|................... [%d:%03d]\n",
	};

	if (string && strnlen(string, 32) > 0 && verbose > 0)
		print_string_state(child, msgs, string);
}

void
print_notapplicable(int child)
{
	static const char *msgs[] = {
		"X-X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-X-X|                    [%d:%03d]\n",
		"                    |X-X-X-X-X NOT APPLICABLE -X-X-X-X-X|X-X-X-X-X-X-X-X-X-X [%d:%03d]\n",
		"                    |X-X-X-X-X NOT APPLICABLE -X-X-X-|-X|X-X-X-X-X-X-X-X-X-X [%d:%03d]\n",
		"X-X-X-X-X-X-X-X-X-X-|X-X-X-X-X NOT APPLICABLE -X-X-X-X-X|X-X-X-X-X-X-X-X-X-X [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_skipped(int child)
{
	static const char *msgs[] = {
		"::::::::::::::::::::|:::::::::::: SKIPPED ::::::::::::::|                    [%d:%03d]\n",
		"                    |:::::::::::: SKIPPED ::::::::::::::|::::::::::::::::::: [%d:%03d]\n",
		"                    |:::::::::::: SKIPPED :::::::::::|::|::::::::::::::::::: [%d:%03d]\n",
		"::::::::::::::::::::|:::::::::::: SKIPPED ::::::::::::::|::::::::::::::::::: [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_inconclusive(int child)
{
	static const char *msgs[] = {
		"????????????????????|?????????? INCONCLUSIVE ???????????|                    [%d:%03d]\n",
		"                    |?????????? INCONCLUSIVE ???????????|??????????????????? [%d:%03d]\n",
		"                    |?????????? INCONCLUSIVE ????????|??|??????????????????? [%d:%03d]\n",
		"????????????????????|?????????? INCONCLUSIVE ???????????|??????????????????? [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_test(int child)
{
	static const char *msgs[] = {
		"--------------------+---------------Test----------------+                   \n",
		"                    +---------------Test----------------+-------------------\n",
		"                    +---------------Test-------------+--+-------------------\n",
		"--------------------+---------------Test----------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_failed(int child)
{
	static const char *msgs[] = {
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXX FAILED XXXXXXXXXXXXXX|                    [%d:%03d]\n",
		"                    |XXXXXXXXXXXXX FAILED XXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
		"                    |XXXXXXXXXXXXX FAILED XXXXXXXXXXX|XX|XXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
		"XXXXXXXXXXXXXXXXXXXX|XXXXXXXXXXXXX FAILED XXXXXXXXXXXXXX|XXXXXXXXXXXXXXXXXXX [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_script_error(int child)
{
	static const char *msgs[] = {
		"####################|########### SCRIPT ERROR ##########|                    [%d:%03d]\n",
		"                    |########### SCRIPT ERROR ##########|################### [%d:%03d]\n",
		"                    |########### SCRIPT ERROR #######|##|################### [%d:%03d]\n",
		"####################|########### SCRIPT ERROR ##########|################### [%d:%03d]\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_passed(int child)
{
	static const char *msgs[] = {
		"********************|************* PASSED **************|                    [%d:%03d]\n",
		"                    |************* PASSED **************|******************* [%d:%03d]\n",
		"                    |************* PASSED ***********|**|******************* [%d:%03d]\n",
		"********************|************* PASSED **************|******************* [%d:%03d]\n",
	};

	if (verbose > 2)
		print_double_int(child, msgs, child, state);
	print_failure(child, failure_string);
}

void
print_postamble(int child)
{
	static const char *msgs[] = {
		"--------------------+-------------Postamble-------------+                   \n",
		"                    +-------------Postamble-------------+-------------------\n",
		"                    +-------------Postamble----------+--+-------------------\n",
		"--------------------+-------------Postamble-------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_test_end(int child)
{
	static const char *msgs[] = {
		"--------------------+-----------------------------------+                   \n",
		"                    +-----------------------------------+-------------------\n",
		"                    +--------------------------------+--+-------------------\n",
		"--------------------+-----------------------------------+-------------------\n",
	};

	if (verbose > 0)
		print_simple(child, msgs);
}

void
print_terminated(int child, int signal)
{
	static const char *msgs[] = {
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@@@@|                    {%d:%03d}\n",
		"                    |@@@@@@@@@@@ TERMINATED @@@@@@@@@@@@|@@@@@@@@@@@@@@@@@@@ {%d:%03d}\n",
		"                    |@@@@@@@@@@@ TERMINATED @@@@@@@@@|@@|@@@@@@@@@@@@@@@@@@@ {%d:%03d}\n",
		"@@@@@@@@@@@@@@@@@@@@|@@@@@@@@@@@ TERMINATED @@@@@@@@@@@@|@@@@@@@@@@@@@@@@@@@ {%d:%03d}\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, signal);
}

void
print_stopped(int child, int signal)
{
	static const char *msgs[] = {
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&&&&|                    {%d:%03d}\n",
		"                    |&&&&&&&&&&&& STOPPED &&&&&&&&&&&&&&|&&&&&&&&&&&&&&&&&&& {%d:%03d}\n",
		"                    |&&&&&&&&&&&& STOPPED &&&&&&&&&&&|&&|&&&&&&&&&&&&&&&&&&& {%d:%03d}\n",
		"&&&&&&&&&&&&&&&&&&&&|&&&&&&&&&&&& STOPPED &&&&&&&&&&&&&&|&&&&&&&&&&&&&&&&&&& {%d:%03d}\n",
	};

	if (verbose > 0)
		print_double_int(child, msgs, child, signal);
}

void
print_timeout(int child)
{
	static const char *msgs[] = {
		"++++++++++++++++++++|++++++++++++ TIMEOUT! +++++++++++++|                    [%d:%03d]\n",
		"                    |++++++++++++ TIMEOUT! +++++++++++++|+++++++++++++++++++ [%d:%03d]\n",
		"                    |++++++++++++ TIMEOUT! ++++++++++|++|+++++++++++++++++++ [%d:%03d]\n",
		"++++++++++++++++++++|++++++++++++ TIMEOUT! +++++++++++++|+++++++++++++++++++ [%d:%03d]\n",
	};

	if (show_timeout || verbose > 1) {
		print_double_int(child, msgs, child, state);
		show_timeout--;
	}
}

void
print_nothing(int child)
{
	static const char *msgs[] = {
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - - -|                    [%d:%03d]\n",
		"                    |- - - - - - -nothing! - - - - - - -|- - - - - - - - - - [%d:%03d]\n",
		"                    |- - - - - - -nothing! - - - - - | -|- - - - - - - - - - [%d:%03d]\n",
		"- - - - - - - - - - |- - - - - - -nothing! - - - - - - -|- - - - - - - - - - [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_double_int(child, msgs, child, state);
}

void
print_syscall(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s----->|                                   |                    [%d:%03d]\n",
		"                    |                                   |<---%-14s  [%d:%03d]\n",
		"                    |                                |<-+----%-14s  [%d:%03d]\n",
		"                    |          %-14s           |                    [%d:%03d]\n",
	};

	if ((verbose && show) || (verbose > 5 && show_msg))
		print_string_state(child, msgs, command);
}

void
print_tx_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"--%16s->| - - - - - - - - - - - - - - - - ->|                    [%d:%03d]\n",
		"                    |<- - - - - - - - - - - - - - - - - |<-%16s- [%d:%03d]\n",
		"                    |<- - - - - - - - - - - - - - - -|<----%16s- [%d:%03d]\n",
		"                    |         %-16s          |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, command);
}

void
print_rx_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"<-%16s--|<- - - - - - - - - - - - - - - - - |                    [%d:%03d]\n",
		"                    |- - - - - - - - - - - - - - - - - >|--%16s> [%d:%03d]\n",
		"                    |- - - - - - - - - - - - - - - ->|--+--%16s> [%d:%03d]\n",
		"                    |         <%16s>        |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, command);
}

void
print_tx_msg(int child, const char *command)
{
	static const char *msgs[] = {
		"--%16s->| - - - - - - - - - - - - - - - - ->|                    [%d:%03d]\n",
		"                    |<- - - - - - - - - - - - - - - - - |<-%16s- [%d:%03d]\n",
		"                    |<- - - - - - - - - - - - - - - -|<----%16s- [%d:%03d]\n",
		"                    |         %-16s          |                    [%d:%03d]\n",
	};

	if ((verbose && show) || verbose > 4)
		print_string_state(child, msgs, command);
}

void
print_rx_msg(int child, const char *command)
{
	static const char *msgs[] = {
		"<-%16s--|<- - - - - - - - - - - - - - - - - |                    [%d:%03d]\n",
		"                    |- - - - - - - - - - - - - - - - - >|--%16s> [%d:%03d]\n",
		"                    |- - - - - - - - - - - - - - - ->|--+--%16s> [%d:%03d]\n",
		"                    |         <%16s>        |                    [%d:%03d]\n",
	};

	if ((verbose && show) || verbose > 4)
		print_string_state(child, msgs, command);
}

void
print_ack_prim(int child, const char *command)
{
	static const char *msgs[] = {
		"<-%16s-/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\-%16s> [%d:%03d]\n",
		"                    |                                |\\-+--%16s> [%d:%03d]\n",
		"                    |         <%16s>        |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, command);
}

void
print_long_state(int child, const char *msgs[], long value)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], value, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_no_prim(int child, long prim)
{
	static const char *msgs[] = {
		"????%4ld????  ?----?| ?- - - - - - - - - - - - - - - -? |                     [%d:%03d]\n",
		"                    | ?- - - - - - - - - - - - - - - -? |?--? ????%4ld????    [%d:%03d]\n",
		"                    | ?- - - - - - - -- - - - - - -? |?-+---? ????%4ld????    [%d:%03d]\n",
		"                    | ?- - - - - - -%4ld- - - - - -? |  |                     [%d:%03d]\n",
	};

	if (verbose > 1)
		print_long_state(child, msgs, prim);
}

void
print_signal(int child, int signum)
{
	static const char *msgs[] = {
		">>>>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8.8s <<<<<<<<<<<<<|                    [%d:%03d]\n",
		"                    |>>>>>>>>>>>> %-8.8s <<<<<<<<<<<<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		"                    |>>>>>>>>>>>> %-8.8s <<<<<<<<<<|<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
		">>>>>>>>>>>>>>>>>>>>|>>>>>>>>>>>> %-8.8s <<<<<<<<<<<<<|<<<<<<<<<<<<<<<<<<< [%d:%03d]\n",
	};

	if (verbose > 0)
		print_string_state(child, msgs, signal_string(signum));
}

void
print_command(int child, const char *command)
{
	static const char *msgs[] = {
		"%-14s----->|                                |  |                    [%d:%03d]\n",
		"  %-14s--->|                                |  |                    [%d:%03d]\n",
		"    %-14s->|                                |  |                    [%d:%03d]\n",
		"                    |          %-14s        |  |                    [%d:%03d]\n",
	};

	if (verbose > 3)
		print_string_state(child, msgs, command);
}

void
print_double_string_state(int child, const char *msgs[], const char *string1, const char *string2)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string1, string2, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_command_info(int child, const char *command, const char *info)
{
	static const char *msgs[] = {
		"%1$-14s----->|         %2$-16.16s          |                    [%3$d:%4$03d]\n",
		"                    |         %2$-16.16s          |<---%1$-14s  [%3$d:%4$03d]\n",
		"                    |         %2$-16.16s       |<-+----%1$-14s  [%3$d:%4$03d]\n",
		"                    | %1$-14s %2$-16.16s|  |                    [%3$d:%4$03d]\n",
	};

	if (show && verbose > 3)
		print_double_string_state(child, msgs, command, info);
}

void
print_string_int_state(int child, const char *msgs[], const char *string, int val)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], string, val, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_tx_data(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"--%1$16s->| - %2$5d bytes - - - - - - - - - ->|                    [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - - - |<-%1$16s- [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - -|<-+ -%1$16s- [%3$d:%4$03d]\n",
		"                    | - %2$5d bytes %1$16s    |                    [%3$d:%4$03d]\n",
	};

	if ((show && verbose > 1) || (verbose > 5 && show_msg))
		print_string_int_state(child, msgs, command, bytes);
}

void
print_rx_data(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"<-%1$16s--|<- -%2$4d bytes- - - - - - - - - - -|                    [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes- - - - - - - - - - >|--%1$16s> [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes- - - - - - - - ->|--+--%1$16s> [%3$d:%4$03d]\n",
		"                    |- - %2$4d bytes %1$16s    |                    [%3$d:%4$03d]\n",
	};

	if ((show && verbose > 1) || (verbose > 5 && show_msg))
		print_string_int_state(child, msgs, command, bytes);
}

void
print_errno(int child, long error)
{
	static const char *msgs[] = {
		"  %-14s<--/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\-->%14s  [%d:%03d]\n",
		"                    |                                |\\-+--->%14s  [%d:%03d]\n",
		"                    |          [%14s]         |                    [%d:%03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_string_state(child, msgs, errno_string(error));
}

void
print_success(int child)
{
	static const char *msgs[] = {
		"  ok          <----/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\---->         ok   [%d:%03d]\n",
		"                    |                                |\\-+----->         ok   [%d:%03d]\n",
		"                    |                 ok                |                    [%d:%03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_double_int(child, msgs, child, state);
}

void
print_success_value(int child, int value)
{
	static const char *msgs[] = {
		"  %10d  <----/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\---->  %10d  [%d:%03d]\n",
		"                    |                                |\\-+----->  %10d  [%d:%03d]\n",
		"                    |            [%10d]           |                    [%d:%03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_triple_int(child, msgs, value, child, state);
}

void
print_int_string_state(int child, const char *msgs[], const int value, const char *string)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], value, string, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_poll_value(int child, int value, short revents)
{
	static const char *msgs[] = {
		"  %1$10d  <----/| %2$-30.30s    |                    [%3$d:%4$03d]\n",
		"                    | %2$-30.30s    |\\---->  %1$10d  [%3$d:%4$03d]\n",
		"                    | %2$-30.30s |\\-+----->  %1$10d  [%3$d:%4$03d]\n",
		"                    | %2$-20.20s [%1$10d] |                    [%3$d:%4$03d]\n",
	};

	if (show && verbose > 3)
		print_int_string_state(child, msgs, value, poll_events_string(revents));
}

void
print_ti_ioctl(int child, int cmd, intptr_t arg)
{
	static const char *msgs[] = {
		"--ioctl(2)--------->|       %16s            |                    [%d:%03d]\n",
		"                    |       %16s            |<---ioctl(2)------  [%d:%03d]\n",
		"                    |       %16s         |<-+----ioctl(2)------  [%d:%03d]\n",
		"                    |       %16s ioctl(2)   |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, ioctl_string(cmd, arg));
}

void
print_ioctl(int child, int cmd, intptr_t arg)
{
	print_command_info(child, "ioctl(2)------", ioctl_string(cmd, arg));
}

void
print_poll(int child, short events)
{
	print_command_info(child, "poll(2)-------", poll_string(events));
}

void
print_datcall(int child, const char *command, size_t bytes)
{
	static const char *msgs[] = {
		"  %1$16s->| - %2$5d bytes - - - - - - - - - ->|                    [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - - - |<-%1$16s  [%3$d:%4$03d]\n",
		"                    |<- %2$5d bytes - - - - - - - - -|<-+--%1$16s  [%3$d:%4$03d]\n",
		"                    | - %2$5d bytes %1$16s    |                    [%3$d:%4$03d]\n",
	};

	if ((verbose > 4 && show) || (verbose > 5 && show_msg))
		print_string_int_state(child, msgs, command, bytes);
}

void
print_libcall(int child, const char *command)
{
	static const char *msgs[] = {
		"  %-16s->|                                   |                    [%d:%03d]\n",
		"                    |                                   |<-%16s  [%d:%03d]\n",
		"                    |                                |<-+--%16s  [%d:%03d]\n",
		"                    |        [%16s]         |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, command);
}

#if 0
void
print_terror(int child, long error, long terror)
{
	static const char *msgs[] = {
		"  %-14s<--/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\-->%14s  [%d:%03d]\n",
		"                    |                                |\\-+--->%14s  [%d:%03d]\n",
		"                    |          [%14s]         |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, t_errno_string(terror, error));
}
#endif

#if 0
void
print_tlook(int child, int tlook)
{
	static const char *msgs[] = {
		"  %-14s<--/|                                   |                    [%d:%03d]\n",
		"                    |                                   |\\-->%14s  [%d:%03d]\n",
		"                    |                                |\\-|--->%14s  [%d:%03d]\n",
		"                    |          [%14s]         |                    [%d:%03d]\n",
	};

	if (show && verbose > 1)
		print_string_state(child, msgs, t_look_string(tlook));
}
#endif

void
print_expect(int child, int want)
{
	static const char *msgs[] = {
		" (%-16s) |- - - - - -[Expected]- - - - - - - |                    [%d:%03d]\n",
		"                    |- - - - - -[Expected]- - - - - - - | (%-16s) [%d:%03d]\n",
		"                    |- - - - - -[Expected]- - - - - -|- | (%-16s) [%d:%03d]\n",
		"                    |- [Expected %-16s ] - - |                    [%d:%03d]\n",
	};

	if (verbose > 0 && show)
		print_string_state(child, msgs, event_string(child, want));
}

void
print_string(int child, const char *string)
{
	static const char *msgs[] = {
		"%-20s|                                   |                    \n",
		"                    |                                   |%-20s\n",
		"                    |                                |   %-20s\n",
		"                    |       %-20s        |                    \n",
	};

	if (show && verbose > 0)
		print_simple_string(child, msgs, string);
}

void
print_string_val(int child, const char *string, ulong val)
{
	static const char *msgs[] = {
		"%1$20.20s|          %2$15u          |                    \n",
		"                    |          %2$15u          |%1$-20.20s\n",
		"                    |          %2$15u       |   %1$-20.20s\n",
		"                    |%1$-20.20s%2$15u|                    \n",
	};

	if (show && verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, msgs[child], string, val);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
}

void
print_command_state(int child, const char *string)
{
	static const char *msgs[] = {
		"%20s|                                   |                    [%d:%03d]\n",
		"                    |                                   |%-20s[%d:%03d]\n",
		"                    |                                |   %-20s[%d:%03d]\n",
		"                    |       %-20s        |                    [%d:%03d]\n",
	};

	if (show && verbose > 0)
		print_string_state(child, msgs, string);
}

void
print_time_state(int child, const char *msgs[], ulong time)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], time, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_waiting(int child, ulong time)
{
	static const char *msgs[] = {
		"/ / / / / / / / / / | / / / Waiting %03lu seconds / / / / |                    [%d:%03d]\n",
		"                    | / / / Waiting %03lu seconds / / / / | / / / / / / / / /  [%d:%03d]\n",
		"                    | / / / Waiting %03lu seconds / / /|/ | / / / / / / / / /  [%d:%03d]\n",
		"/ / / / / / / / / / | / / / Waiting %03lu seconds / / / / | / / / / / / / / /  [%d:%03d]\n",
	};

	if (verbose > 1 && show)
		print_time_state(child, msgs, time);
}

void
print_float_state(int child, const char *msgs[], float time)
{
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, msgs[child], time, child, state);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
print_mwaiting(int child, struct timespec *time)
{
	static const char *msgs[] = {
		"/ / / / / / / / / / | / / Waiting %8.4f seconds/ / / |                    [%d:%03d]\n",
		"                    | / / Waiting %8.4f seconds/ / / | / / / / / / / / /  [%d:%03d]\n",
		"                    | / / Waiting %8.4f seconds/ /|/ / / / / / / / / / /  [%d:%03d]\n",
		"/ / / / / / / / / / | / / Waiting %8.4f seconds/ / / | / / / / / / / / /  [%d:%03d]\n",
	};

	if (verbose > 1 && show) {
		float delay;

		delay = time->tv_nsec;
		delay = delay / 1000000000;
		delay = delay + time->tv_sec;
		print_float_state(child, msgs, delay);
	}
}

void
print_mgmtflag(int child, t_uscalar_t flag)
{
	print_string(child, mgmtflag_string(flag));
}

#if 1
void
print_opt_level(int child, struct t_opthdr *oh)
{
	char *level = level_string(oh);

	if (level)
		print_string(child, level);
}

void
print_opt_name(int child, struct t_opthdr *oh)
{
	char *name = name_string(oh);

	if (name)
		print_string(child, name);
}

void
print_opt_status(int child, struct t_opthdr *oh)
{
	char *status = status_string(oh);

	if (status)
		print_string(child, status);
}

void
print_opt_length(int child, struct t_opthdr *oh)
{
	int len = oh->len - sizeof(*oh);

	if (len) {
		char buf[32];

		snprintf(buf, sizeof(buf), "(len=%d)", len);
		print_string(child, buf);
	}
}
void
print_opt_value(int child, struct t_opthdr *oh)
{
	char *value = value_string(child, oh);

	if (value)
		print_string(child, value);
}
#endif

void
print_options(int child, const char *cmd_buf, size_t opt_ofs, size_t opt_len)
{
	struct t_opthdr *oh;
	const char *opt_ptr = cmd_buf + opt_ofs;
	char buf[64];

	if (verbose < 4)
		return;
	if (opt_len == 0) {
		snprintf(buf, sizeof(buf), "(no options)");
		print_string(child, buf);
		return;
	}
	snprintf(buf, sizeof(buf), "opt len = %lu", (ulong) opt_len);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "opt ofs = %lu", (ulong) opt_ofs);
	print_string(child, buf);
	oh = _T_OPT_FIRSTHDR_OFS(opt_ptr, opt_len, 0);
	if (oh) {
		for (; oh; oh = _T_OPT_NEXTHDR_OFS(opt_ptr, opt_len, oh, 0)) {
			int len = oh->len - sizeof(*oh);

			print_opt_level(child, oh);
			print_opt_name(child, oh);
			print_opt_status(child, oh);
			print_opt_length(child, oh);
			if (len < 0)
				break;
			print_opt_value(child, oh);
		}
	} else {
		oh = (typeof(oh)) opt_ptr;
		print_opt_level(child, oh);
		print_opt_name(child, oh);
		print_opt_status(child, oh);
		print_opt_length(child, oh);
	}
}

void
print_info(int child, struct T_info_ack *info)
{
	char buf[64];

	if (verbose < 4)
		return;
	snprintf(buf, sizeof(buf), "TSDU  = %ld", (long) info->TSDU_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "ETSDU = %ld", (long) info->ETSDU_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "CDATA = %ld", (long) info->CDATA_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "DDATA = %ld", (long) info->DDATA_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "ADDR  = %ld", (long) info->ADDR_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "OPT   = %ld", (long) info->OPT_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "TIDU  = %ld", (long) info->TIDU_size);
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "<%s>", service_type(info->SERV_type));
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "<%s>", state_string(info->CURRENT_state));
	print_string(child, buf);
	snprintf(buf, sizeof(buf), "PROV  = %ld", (long) info->PROVIDER_flag);
	print_string(child, buf);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Driver actions.
 *
 *  -------------------------------------------------------------------------
 */

int
test_waitsig(int child)
{
	int signum;
	sigset_t set;

	sigemptyset(&set);
	while ((signum = last_signum) == 0)
		sigsuspend(&set);
	print_signal(child, signum);
	return (__RESULT_SUCCESS);

}

int
test_ioctl(int child, int cmd, intptr_t arg)
{
	print_ioctl(child, cmd, arg);
	for (;;) {
		if ((last_retval = ioctl(test_fd[child], cmd, arg)) == -1) {
			print_errno(child, (last_errno = errno));
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			return (__RESULT_FAILURE);
		}
		if (show && verbose > 3)
			print_success_value(child, last_retval);
		return (__RESULT_SUCCESS);
	}
}

int
test_insertfd(int child, int resfd, int offset, struct strbuf *ctrl, struct strbuf *data, int flags)
{
	struct strfdinsert fdi;

	if (ctrl) {
		fdi.ctlbuf.maxlen = ctrl->maxlen;
		fdi.ctlbuf.len = ctrl->len;
		fdi.ctlbuf.buf = ctrl->buf;
	} else {
		fdi.ctlbuf.maxlen = -1;
		fdi.ctlbuf.len = 0;
		fdi.ctlbuf.buf = NULL;
	}
	if (data) {
		fdi.databuf.maxlen = data->maxlen;
		fdi.databuf.len = data->len;
		fdi.databuf.buf = data->buf;
	} else {
		fdi.databuf.maxlen = -1;
		fdi.databuf.len = 0;
		fdi.databuf.buf = NULL;
	}
	fdi.flags = flags;
	fdi.fildes = resfd;
	fdi.offset = offset;
	if (show && verbose > 4) {
		int i;

		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "fdinsert to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
		fprintf(stdout, "[");
		for (i = 0; i < (ctrl ? ctrl->len : 0); i++)
			fprintf(stdout, "%02X", (uint8_t) ctrl->buf[i]);
		fprintf(stdout, "]\n");
		fprintf(stdout, "[");
		for (i = 0; i < (data ? data->len : 0); i++)
			fprintf(stdout, "%02X", (uint8_t) data->buf[i]);
		fprintf(stdout, "]\n");
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (test_ioctl(child, I_FDINSERT, (intptr_t) &fdi) != __RESULT_SUCCESS)
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

int
test_putmsg(int child, struct strbuf *ctrl, struct strbuf *data, int flags)
{
	print_datcall(child, "putmsg(2)-----", data ? data->len : -1);
	if ((last_retval = putmsg(test_fd[child], ctrl, data, flags)) == -1) {
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
	return (__RESULT_SUCCESS);
}

int
test_putpmsg(int child, struct strbuf *ctrl, struct strbuf *data, int band, int flags)
{
	if (flags & MSG_BAND || band) {
		if ((verbose > 3 && show) || (verbose > 5 && show_msg)) {
			int i;

			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "putpmsg to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
			fprintf(stdout, "[");
			for (i = 0; i < (ctrl ? ctrl->len : 0); i++)
				fprintf(stdout, "%02X", (uint8_t) ctrl->buf[i]);
			fprintf(stdout, "]\n");
			fprintf(stdout, "[");
			for (i = 0; i < (data ? data->len : 0); i++)
				fprintf(stdout, "%02X", (uint8_t) data->buf[i]);
			fprintf(stdout, "]\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		if (ctrl == NULL || data != NULL)
			print_datcall(child, "M_DATA----------", data ? data->len : 0);
		for (;;) {
			if ((last_retval = putpmsg(test_fd[child], ctrl, data, band, flags)) == -1) {
				if (last_errno == EINTR || last_errno == ERESTART)
					continue;
				print_errno(child, (last_errno = errno));
				return (__RESULT_FAILURE);
			}
			if ((verbose > 3 && show) || (verbose > 5 && show_msg))
				print_success_value(child, last_retval);
			return (__RESULT_SUCCESS);
		}
	} else {
		if ((verbose > 3 && show) || (verbose > 5 && show_msg)) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "putmsg to %d: [%d,%d]\n", child, ctrl ? ctrl->len : -1, data ? data->len : -1);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
			fflush(stdout);
		}
		if (ctrl == NULL || data != NULL)
			print_datcall(child, "M_DATA----------", data ? data->len : 0);
		for (;;) {
			if ((last_retval = putmsg(test_fd[child], ctrl, data, flags)) == -1) {
				if (last_errno == EINTR || last_errno == ERESTART)
					continue;
				print_errno(child, (last_errno = errno));
				return (__RESULT_FAILURE);
			}
			if ((verbose > 3 && show) || (verbose > 5 && show_msg))
				print_success_value(child, last_retval);
			return (__RESULT_SUCCESS);
		}
	}
}

int
test_write(int child, const void *buf, size_t len)
{
	print_syscall(child, "write(2)------");
	for (;;) {
		if ((last_retval = write(test_fd[child], buf, len)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_writev(int child, const struct iovec *iov, int num)
{
	print_syscall(child, "writev(2)-----");
	for (;;) {
		if ((last_retval = writev(test_fd[child], iov, num)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_getmsg(int child, struct strbuf *ctrl, struct strbuf *data, int *flagp)
{
	print_syscall(child, "getmsg(2)-----");
	for (;;) {
		if ((last_retval = getmsg(test_fd[child], ctrl, data, flagp)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_getpmsg(int child, struct strbuf *ctrl, struct strbuf *data, int *bandp, int *flagp)
{
	print_syscall(child, "getpmsg(2)----");
	for (;;) {
		if ((last_retval = getpmsg(test_fd[child], ctrl, data, bandp, flagp)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_read(int child, void *buf, size_t count)
{
	print_syscall(child, "read(2)-------");
	for (;;) {
		if ((last_retval = read(test_fd[child], buf, count)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_readv(int child, const struct iovec *iov, int count)
{
	print_syscall(child, "readv(2)------");
	for (;;) {
		if ((last_retval = readv(test_fd[child], iov, count)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_ti_ioctl(int child, int cmd, intptr_t arg)
{
	int tpi_error;

	if (cmd == I_STR && verbose > 3) {
		struct strioctl *icp = (struct strioctl *) arg;

		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "ioctl from %d: cmd=%d, timout=%d, len=%d, dp=%p\n", child, icp->ic_cmd, icp->ic_timout, icp->ic_len, icp->ic_dp);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	print_ti_ioctl(child, cmd, arg);
	for (;;) {
		if ((last_retval = ioctl(test_fd[child], cmd, arg)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		if (show && verbose > 3)
			print_success_value(child, last_retval);
		break;
	}
	if (cmd == I_STR && show && verbose > 3) {
		struct strioctl *icp = (struct strioctl *) arg;

		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "got ioctl from %d: cmd=%d, timout=%d, len=%d, dp=%p\n", child, icp->ic_cmd, icp->ic_timout, icp->ic_len, icp->ic_dp);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (last_retval == 0)
		return __RESULT_SUCCESS;
	tpi_error = last_retval & 0x00ff;
	if (tpi_error == TSYSERR)
		last_errno = (last_retval >> 8) & 0x00ff;
	else
		last_errno = 0;
	if (verbose) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "***************ERROR: ioctl failed\n");
		if (show && verbose > 3)
			fprintf(stdout, "                    : %s; result = %d\n", __FUNCTION__, last_retval);
		fprintf(stdout, "                    : %s; TPI error = %d\n", __FUNCTION__, tpi_error);
		if (tpi_error == TSYSERR)
			fprintf(stdout, "                    : %s; %s\n", __FUNCTION__, strerror(last_errno));
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
		fflush(stdout);
	}
	return (__RESULT_FAILURE);
}

int
test_nonblock(int child)
{
	long flags;

	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((flags = last_retval = fcntl(test_fd[child], F_GETFL)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((last_retval = fcntl(test_fd[child], F_SETFL, flags | O_NONBLOCK)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_block(int child)
{
	long flags;

	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((flags = last_retval = fcntl(test_fd[child], F_GETFL)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	print_syscall(child, "fcntl(2)------");
	for (;;) {
		if ((last_retval = fcntl(test_fd[child], F_SETFL, flags & ~O_NONBLOCK)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		}
		print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_isastream(int child)
{
	int result;

	print_syscall(child, "isastream(2)--");
	for (;;) {
	if ((result = last_retval = isastream(test_fd[child])) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_success_value(child, last_retval);
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_poll(int child, const short events, short *revents, long ms)
{
	struct pollfd pfd = {.fd = test_fd[child],.events = events,.revents = 0 };
	int result;

	print_poll(child, events);
	for (;;) {
	if ((result = last_retval = poll(&pfd, 1, ms)) == -1) {
			if (last_errno == EINTR || last_errno == ERESTART)
				continue;
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
	print_poll_value(child, last_retval, pfd.revents);
	if (last_retval == 1 && revents)
		*revents = pfd.revents;
		break;
	}
	return (__RESULT_SUCCESS);
}

int
test_pipe(int child)
{
	int fds[2];

	for (;;) {
		print_pipe(child);
		if (pipe(fds) >= 0) {
			test_fd[child + 0] = fds[0];
			test_fd[child + 1] = fds[1];
			print_success(child);
			return (__RESULT_SUCCESS);
		}
		if (last_errno == EINTR || last_errno == ERESTART)
			continue;
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
}

int
test_fopen(int child, const char *name, int flags)
{
	int fd;

	print_open(child, name);
	if ((fd = open(name, flags)) >= 0) {
		print_success(child);
		return (fd);
	}
	print_errno(child, (last_errno = errno));
	return (fd);
}

int
test_fclose(int child, int fd)
{
	print_close(child);
	if (close(fd) >= 0) {
		print_success(child);
		return __RESULT_SUCCESS;
	}
	print_errno(child, (last_errno = errno));
	return __RESULT_FAILURE;
}

int
test_open(int child, const char *name, int flags)
{
	int fd;

	for (;;) {
		print_open(child, name);
		if ((fd = open(name, flags)) >= 0) {
			test_fd[child] = fd;
			print_success(child);
			return (__RESULT_SUCCESS);
		}
		if (last_errno == EINTR || last_errno == ERESTART)
			continue;
		print_errno(child, (last_errno = errno));
		return (__RESULT_FAILURE);
	}
}

int
test_close(int child)
{
	int fd = test_fd[child];

	test_fd[child] = 0;
	for (;;) {
		print_close(child);
		if (close(fd) >= 0) {
			print_success(child);
			return __RESULT_SUCCESS;
		}
		if (last_errno == EINTR || last_errno == ERESTART)
			continue;
		print_errno(child, (last_errno = errno));
		return __RESULT_FAILURE;
	}
}

int
test_push(int child, const char *name)
{
	if (show && verbose > 1)
		print_command_state(child, ":push");
	if (test_ioctl(child, I_PUSH, (intptr_t) name))
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

int
test_pop(int child)
{
	if (show && verbose > 1)
		print_command_state(child, ":pop");
	if (test_ioctl(child, I_POP, (intptr_t) 0))
		return __RESULT_FAILURE;
	return __RESULT_SUCCESS;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  STREAM Initialization
 *
 *  -------------------------------------------------------------------------
 */

static int
stream_start(int child, int index)
{
	int offset = 3 * index;
	int i;

	for (i = 0; i < anums[3]; i++) {
#ifndef SCTP_VERSION_2
		addrs[3].port = htons(ports[3] + offset);
		inet_aton(addr_strings[i], &addrs[child].addr[i]);
#else				/* SCTP_VERSION_2 */
		addrs[3][i].sin_family = AF_INET;
		addrs[3][i].sin_port = htons(ports[3] + offset);
		inet_aton(addr_strings[i], &addrs[3][i].sin_addr);
#endif				/* SCTP_VERSION_2 */
	}
	switch (child) {
	case 1:
	case 2:
	case 0:
		for (i = 0; i < anums[child]; i++) {
#ifndef SCTP_VERSION_2
			addrs[child].port = htons(ports[child] + offset);
			inet_aton(addr_strings[i], &addrs[child].addr[i]);
#else				/* SCTP_VERSION_2 */
			addrs[child][i].sin_family = AF_INET;
			if ((child == 0 && !client_port_specified) || ((child == 1 || child == 2) && !server_port_specified))
				addrs[child][i].sin_port = htons(ports[child] + offset);
			else
				addrs[child][i].sin_port = htons(ports[child]);
			if ((child == 0 && !client_host_specified) || ((child == 1 || child == 2) && !server_host_specified))
				inet_aton(addr_strings[i], &addrs[child][i].sin_addr);
#endif				/* SCTP_VERSION_2 */
		}
		if (test_open(child, devname, O_NONBLOCK | O_RDWR) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		if (test_ioctl(child, I_SRDOPT, (intptr_t) RMSGD) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		return __RESULT_SUCCESS;
	default:
		return __RESULT_FAILURE;
	}
}

static int
stream_stop(int child)
{
	switch (child) {
	case 1:
	case 2:
	case 0:
		if (test_close(child) != __RESULT_SUCCESS)
			return __RESULT_FAILURE;
		return __RESULT_SUCCESS;
	default:
		return __RESULT_FAILURE;
	}
}

void
test_sleep(int child, unsigned long t)
{
	print_waiting(child, t);
	sleep(t);
}

void
test_msleep(int child, unsigned long m)
{
	struct timespec time;

	time.tv_sec = m / 1000;
	time.tv_nsec = (m % 1000) * 1000000;
	print_mwaiting(child, &time);
	nanosleep(&time, NULL);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Test harness initialization and termination.
 *
 *  -------------------------------------------------------------------------
 */

static int
begin_tests(int index)
{
	state = 0;
	if (stream_start(0, index) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (stream_start(1, index) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (stream_start(2, index) != __RESULT_SUCCESS)
		goto failure;
	state++;
	show_acks = 1;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
end_tests(int index)
{
	show_acks = 0;
	if (stream_stop(2) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (stream_stop(1) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (stream_stop(0) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Injected event encoding and display functions.
 *
 *  -------------------------------------------------------------------------
 */

int
do_signal(int child, int action)
{
	struct strbuf ctrl_buf, data_buf, *ctrl = &ctrl_buf, *data = &data_buf;
	char cbuf[BUFSIZE], dbuf[BUFSIZE];
	union T_primitives *p = (typeof(p)) cbuf;
	struct strioctl ic;
	uint32_t *m;

	ic.ic_cmd = 0;
	ic.ic_timout = test_timout;
	ic.ic_len = sizeof(cbuf);
	ic.ic_dp = cbuf;
	ctrl->maxlen = 0;
	ctrl->buf = cbuf;
	data->maxlen = 0;
	data->buf = dbuf;
	test_pflags = MSG_BAND;
	test_pband = 0;
	switch (action) {
	case __TEST_WRITE:
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Write test data.");
		return test_write(child, dbuf, data->len);
	case __TEST_WRITEV:
	{
		struct iovec vector[4];

		vector[0].iov_base = dbuf;
		vector[0].iov_len = sprintf(vector[0].iov_base, "Writev test datum for vector 0.");
		vector[1].iov_base = dbuf + vector[0].iov_len;
		vector[1].iov_len = sprintf(vector[1].iov_base, "Writev test datum for vector 1.");
		vector[2].iov_base = dbuf + vector[1].iov_len;
		vector[2].iov_len = sprintf(vector[2].iov_base, "Writev test datum for vector 2.");
		vector[3].iov_base = dbuf + vector[2].iov_len;
		vector[3].iov_len = sprintf(vector[3].iov_base, "Writev test datum for vector 3.");
		return test_writev(child, vector, 4);
	}
	}
	switch (action) {
	case __TEST_PUSH:
		return test_ti_ioctl(child, I_PUSH, (intptr_t) "tirdwr");
	case __TEST_POP:
		return test_ti_ioctl(child, I_POP, (intptr_t) NULL);
	case __TEST_PUTMSG_DATA:
		ctrl = NULL;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Putmsg test data.");
		test_pflags = MSG_BAND;
		test_pband = 0;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_PUTPMSG_DATA:
		ctrl = NULL;
		data->len = snprintf(dbuf, BUFSIZE, "%s", "Putpmsg band test data.");
		test_pflags = MSG_BAND;
		test_pband = 1;
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_REQ:
		ctrl->len = sizeof(p->conn_req)
		    + (test_addr ? test_alen : 0)
		    + (test_opts ? test_olen : 0);
		p->conn_req.PRIM_type = T_CONN_REQ;
		p->conn_req.DEST_length = test_addr ? test_alen : 0;
		p->conn_req.DEST_offset = test_addr ? sizeof(p->conn_req) : 0;
		p->conn_req.OPT_length = test_opts ? test_olen : 0;
		p->conn_req.OPT_offset = test_opts ? sizeof(p->conn_req) + p->conn_req.DEST_length : 0;
		if (test_addr)
			bcopy(test_addr, ctrl->buf + p->conn_req.DEST_offset, p->conn_req.DEST_length);
		if (test_opts)
			bcopy(test_opts, ctrl->buf + p->conn_req.OPT_offset, p->conn_req.OPT_length);
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->conn_req.DEST_offset, p->conn_req.DEST_length));
#else
		print_addrs(child, cbuf + p->conn_req.DEST_offset, p->conn_req.DEST_length);
#endif
		print_options(child, cbuf, p->conn_req.OPT_offset, p->conn_req.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_IND:
		ctrl->len = sizeof(p->conn_ind);
		p->conn_ind.PRIM_type = T_CONN_IND;
		p->conn_ind.SRC_length = 0;
		p->conn_ind.SRC_offset = 0;
		p->conn_ind.OPT_length = 0;
		p->conn_ind.OPT_offset = 0;
		p->conn_ind.SEQ_number = last_sequence;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_options(child, cbuf, p->conn_ind.OPT_offset, p->conn_ind.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CONN_RES:
		ctrl->len = sizeof(p->conn_res)
		    + (test_opts ? test_olen : 0);
		p->conn_res.PRIM_type = T_CONN_RES;
		p->conn_res.ACCEPTOR_id = 0;
		p->conn_res.OPT_length = test_opts ? test_olen : 0;
		p->conn_res.OPT_offset = test_opts ? sizeof(p->conn_res) : 0;
		p->conn_res.SEQ_number = last_sequence;
		if (test_opts)
			bcopy(test_opts, ctrl->buf + p->conn_res.OPT_offset, p->conn_res.OPT_length);
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_options(child, cbuf, p->conn_res.OPT_offset, p->conn_res.OPT_length);
		if (test_resfd == -1)
			return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
		else
			return test_insertfd(child, test_resfd, 4, ctrl, data, 0);
	case __TEST_CONN_CON:
		ctrl->len = sizeof(p->conn_con);
		p->conn_con.PRIM_type = T_CONN_CON;
		p->conn_con.RES_length = 0;
		p->conn_con.RES_offset = 0;
		p->conn_con.OPT_length = 0;
		p->conn_con.OPT_offset = 0;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_options(child, cbuf, p->conn_con.OPT_offset, p->conn_con.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DISCON_REQ:
		ctrl->len = sizeof(p->discon_req);
		p->discon_req.PRIM_type = T_DISCON_REQ;
		p->discon_req.SEQ_number = last_sequence;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DISCON_IND:
		ctrl->len = sizeof(p->discon_ind);
		p->discon_ind.PRIM_type = T_DISCON_IND;
		p->discon_ind.DISCON_reason = 0;
		p->discon_ind.SEQ_number = last_sequence;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATA_REQ:
		ctrl->len = sizeof(p->data_req);
		p->data_req.PRIM_type = T_DATA_REQ;
		p->data_req.MORE_flag = MORE_flag;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else {
			if (test_dlen)
				data->len = test_dlen;
			else
				data = NULL;
		}
		test_pflags = MSG_BAND;
		test_pband = 0;
		if ((verbose && show) || verbose > 4) {
			print_tx_prim(child, prim_string(p->type));
			if (data)
				print_tx_data(child, "M_DATA----------", data->len);
		}
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_DATA_IND:
		ctrl->len = sizeof(p->data_ind);
		p->data_ind.PRIM_type = T_DATA_IND;
		p->data_ind.MORE_flag = MORE_flag;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		if ((verbose && show) || verbose > 4)
			print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_EXDATA_REQ:
		ctrl->len = sizeof(p->exdata_req);
		p->exdata_req.PRIM_type = T_EXDATA_REQ;
		p->exdata_req.MORE_flag = MORE_flag;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 1;
		if ((verbose && show) || verbose > 4) {
			print_tx_prim(child, prim_string(p->type));
			if (data)
				print_tx_data(child, "M_DATA----------", data->len);
		}
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_EXDATA_IND:
		ctrl->len = sizeof(p->exdata_ind);
		p->data_ind.PRIM_type = T_EXDATA_IND;
		p->data_ind.MORE_flag = MORE_flag;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 1;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_INFO_REQ:
		ctrl->len = sizeof(p->info_req);
		p->info_req.PRIM_type = T_INFO_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_INFO_ACK:
		ctrl->len = sizeof(p->info_ack);
		p->info_ack.PRIM_type = T_INFO_ACK;
		p->info_ack.TSDU_size = test_bufsize;
		p->info_ack.ETSDU_size = test_bufsize;
		p->info_ack.CDATA_size = test_bufsize;
		p->info_ack.DDATA_size = test_bufsize;
		p->info_ack.ADDR_size = test_bufsize;
		p->info_ack.OPT_size = test_bufsize;
		p->info_ack.TIDU_size = test_tidu;
		p->info_ack.SERV_type = last_servtype;
		p->info_ack.CURRENT_state = last_tstate;
		p->info_ack.PROVIDER_flag = last_provflag;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_info(child, &p->info_ack);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_BIND_REQ:
		ctrl->len = sizeof(p->bind_req) + (test_addr ? test_alen : 0);
		p->bind_req.PRIM_type = T_BIND_REQ;
		p->bind_req.ADDR_length = test_addr ? test_alen : 0;
		p->bind_req.ADDR_offset = test_addr ? sizeof(p->bind_req) : 0;
		p->bind_req.CONIND_number = last_qlen;
		if (test_addr)
			bcopy(test_addr, ctrl->buf + p->bind_req.ADDR_offset, p->bind_req.ADDR_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length));
#else
		print_addrs(child, cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length);
#endif
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_BIND_ACK:
		ctrl->len = sizeof(p->bind_ack);
		p->bind_ack.PRIM_type = T_BIND_ACK;
		p->bind_ack.ADDR_length = 0;
		p->bind_ack.ADDR_offset = 0;
		p->bind_ack.CONIND_number = last_qlen;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length));
#else
		print_addrs(child, cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length);
#endif
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNBIND_REQ:
		ctrl->len = sizeof(p->unbind_req);
		p->unbind_req.PRIM_type = T_UNBIND_REQ;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ERROR_ACK:
		ctrl->len = sizeof(p->error_ack);
		p->error_ack.PRIM_type = T_ERROR_ACK;
		p->error_ack.ERROR_prim = last_prim;
		p->error_ack.TLI_error = last_t_errno;
		p->error_ack.UNIX_error = last_errno;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_string(child, terrno_string(p->error_ack.TLI_error, p->error_ack.UNIX_error));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OK_ACK:
		ctrl->len = sizeof(p->ok_ack);
		p->ok_ack.PRIM_type = T_OK_ACK;
		p->ok_ack.CORRECT_prim = 0;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNITDATA_REQ:
		ctrl->len = sizeof(p->unitdata_req)
		    + (test_addr ? test_alen : 0)
		    + (test_opts ? test_olen : 0);
		p->unitdata_req.PRIM_type = T_UNITDATA_REQ;
		p->unitdata_req.DEST_length = test_addr ? test_alen : 0;
		p->unitdata_req.DEST_offset = test_addr ? sizeof(p->unitdata_req) : 0;
		p->unitdata_req.OPT_length = test_opts ? test_olen : 0;
		p->unitdata_req.OPT_offset = test_opts ? sizeof(p->unitdata_req) + p->unitdata_req.DEST_length : 0;
		if (test_addr)
			bcopy(test_addr, ctrl->buf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length);
		if (test_opts)
			bcopy(test_opts, ctrl->buf + p->unitdata_req.OPT_offset, p->unitdata_req.OPT_length);
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length));
#else
		print_addrs(child, cbuf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length);
#endif
		print_options(child, cbuf, p->unitdata_req.OPT_offset, p->unitdata_req.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UNITDATA_IND:
		ctrl->len = sizeof(p->unitdata_ind);
		p->unitdata_ind.PRIM_type = T_UNITDATA_IND;
		p->unitdata_ind.SRC_length = 0;
		p->unitdata_ind.SRC_offset = 0;
		p->unitdata_ind.OPT_length = 0;
		p->unitdata_ind.OPT_offset = 0;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->unitdata_ind.SRC_offset, p->unitdata_ind.SRC_length));
#else
		print_addrs(child, cbuf + p->unitdata_ind.SRC_offset, p->unitdata_ind.SRC_length);
#endif
		print_options(child, cbuf, p->unitdata_ind.OPT_offset, p->unitdata_ind.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_UDERROR_IND:
		ctrl->len = sizeof(p->uderror_ind);
		p->uderror_ind.PRIM_type = T_UDERROR_IND;
		p->uderror_ind.DEST_length = 0;
		p->uderror_ind.DEST_offset = 0;
		p->uderror_ind.OPT_length = 0;
		p->uderror_ind.OPT_offset = 0;
		p->uderror_ind.ERROR_type = 0;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OPTMGMT_REQ:
		ctrl->len = sizeof(p->optmgmt_req)
		    + (test_opts ? test_olen : 0);
		p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
		p->optmgmt_req.OPT_length = test_opts ? test_olen : 0;
		p->optmgmt_req.OPT_offset = test_opts ? sizeof(p->optmgmt_req) : 0;
		p->optmgmt_req.MGMT_flags = test_mgmtflags;
		if (test_opts)
			bcopy(test_opts, ctrl->buf + p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_mgmtflag(child, p->optmgmt_req.MGMT_flags);
		print_options(child, cbuf, p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OPTMGMT_ACK:
		ctrl->len = sizeof(p->optmgmt_ack);
		p->optmgmt_ack.PRIM_type = T_OPTMGMT_ACK;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		print_mgmtflag(child, p->optmgmt_ack.MGMT_flags);
		print_options(child, cbuf, p->optmgmt_ack.OPT_offset, p->optmgmt_ack.OPT_length);
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ORDREL_REQ:
		ctrl->len = sizeof(p->ordrel_req);
		p->ordrel_req.PRIM_type = T_ORDREL_REQ;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ORDREL_IND:
		ctrl->len = sizeof(p->ordrel_ind);
		p->ordrel_ind.PRIM_type = T_ORDREL_IND;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_OPTDATA_REQ:
test_optdata_req:
		ctrl->len = sizeof(p->optdata_req)
		    + (test_opts ? test_olen : 0);
		p->optdata_req.PRIM_type = T_OPTDATA_REQ;
		p->optdata_req.DATA_flag = DATA_flag;
		p->optdata_req.OPT_length = test_opts ? test_olen : 0;
		p->optdata_req.OPT_offset = test_opts ? sizeof(p->optdata_req) : 0;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else {
			if (test_dlen)
				data->len = test_dlen;
			else
				data = NULL;
		}
		if (test_opts)
			bcopy(test_opts, ctrl->buf + p->optdata_req.OPT_offset, p->optdata_req.OPT_length);
		test_pflags = MSG_BAND;
		test_pband = (DATA_flag & T_ODF_EX) ? 1 : 0;
		if (p->optdata_req.DATA_flag & T_ODF_EX) {
			if ((verbose && show) || verbose > 4) {
				if (p->optdata_req.DATA_flag & T_ODF_MORE)
					print_tx_prim(child, "T_OPTDATA_REQ!+ ");
				else
					print_tx_prim(child, "T_OPTDATA_REQ!  ");
			}
		} else {
			if ((verbose && show) || verbose > 4) {
				if (p->optdata_req.DATA_flag & T_ODF_MORE)
					print_tx_prim(child, "T_OPTDATA_REQ+  ");
				else
					print_tx_prim(child, "T_OPTDATA_REQ   ");
			}
		}
		if ((verbose && show) || verbose > 4) {
			print_options(child, cbuf, p->optdata_req.OPT_offset, p->optdata_req.OPT_length);
			if (data)
				print_tx_data(child, "M_DATA----------", data->len);
		}
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_NRM_OPTDATA_IND:
		ctrl->len = sizeof(p->optdata_ind);
		p->optdata_ind.PRIM_type = T_OPTDATA_IND;
		p->optdata_ind.DATA_flag = 0;
		p->optdata_ind.OPT_length = 0;
		p->optdata_ind.OPT_offset = 0;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		if ((verbose && show) || verbose > 4) {
			if (p->optdata_ind.DATA_flag & T_ODF_MORE)
				print_tx_prim(child, "T_OPTDATA_IND+  ");
			else
				print_tx_prim(child, "T_OPTDATA_IND   ");
			print_options(child, cbuf, p->optdata_ind.OPT_offset, p->optdata_ind.OPT_length);
			if (data)
				print_tx_data(child, "M_DATA----------", data->len);
		}
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_EXP_OPTDATA_IND:
		ctrl->len = sizeof(p->optdata_ind);
		p->optdata_ind.PRIM_type = T_OPTDATA_IND;
		p->optdata_ind.DATA_flag = T_ODF_EX;
		p->optdata_ind.OPT_length = 0;
		p->optdata_ind.OPT_offset = 0;
		if (test_data)
			data->len = snprintf(dbuf, BUFSIZE, "%s", test_data);
		else
			data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 1;
		if ((verbose && show) || verbose > 4) {
			if (p->optdata_ind.DATA_flag & T_ODF_MORE)
				print_tx_prim(child, "T_OPTDATA_IND!+ ");
			else
				print_tx_prim(child, "T_OPTDATA_IND!  ");
			print_options(child, cbuf, p->optdata_ind.OPT_offset, p->optdata_ind.OPT_length);
			if (data)
				print_tx_data(child, "M_DATA----------", data->len);
		}
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ADDR_REQ:
		ctrl->len = sizeof(p->addr_req);
		p->addr_req.PRIM_type = T_ADDR_REQ;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_ADDR_ACK:
		ctrl->len = sizeof(p->addr_ack);
		p->addr_ack.PRIM_type = T_ADDR_ACK;
		p->addr_ack.LOCADDR_length = 0;
		p->addr_ack.LOCADDR_offset = 0;
		p->addr_ack.REMADDR_length = 0;
		p->addr_ack.REMADDR_offset = 0;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
		print_string(child, addr_string(cbuf + p->addr_ack.LOCADDR_offset, p->addr_ack.LOCADDR_length));
		print_string(child, addr_string(cbuf + p->addr_ack.REMADDR_offset, p->addr_ack.REMADDR_length));
#else
		print_addrs(child, cbuf + p->addr_ack.LOCADDR_offset, p->addr_ack.LOCADDR_length);
		print_addrs(child, cbuf + p->addr_ack.REMADDR_offset, p->addr_ack.REMADDR_length);
#endif
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CAPABILITY_REQ:
		ctrl->len = sizeof(p->capability_req);
		p->capability_req.PRIM_type = T_CAPABILITY_REQ;
		p->capability_req.CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
		data = NULL;
		test_pflags = test_prio ? MSG_HIPRI : MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_CAPABILITY_ACK:
		ctrl->len = sizeof(p->capability_ack);
		p->capability_ack.PRIM_type = T_CAPABILITY_ACK;
		p->capability_ack.CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
		p->capability_ack.INFO_ack.TSDU_size = test_bufsize;
		p->capability_ack.INFO_ack.ETSDU_size = test_bufsize;
		p->capability_ack.INFO_ack.CDATA_size = test_bufsize;
		p->capability_ack.INFO_ack.DDATA_size = test_bufsize;
		p->capability_ack.INFO_ack.ADDR_size = test_bufsize;
		p->capability_ack.INFO_ack.OPT_size = test_bufsize;
		p->capability_ack.INFO_ack.TIDU_size = test_tidu;
		p->capability_ack.INFO_ack.SERV_type = last_servtype;
		p->capability_ack.INFO_ack.CURRENT_state = last_tstate;
		p->capability_ack.INFO_ack.PROVIDER_flag = last_provflag;
		p->capability_ack.ACCEPTOR_id = 0;
		data = NULL;
		test_pflags = MSG_HIPRI;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_PRIM_TOO_SHORT:
		ctrl->len = sizeof(p->type);
		p->type = last_prim;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_PRIM_WAY_TOO_SHORT:
		ctrl->len = sizeof(p->type) >> 1;
		p->type = last_prim;
		data = NULL;
		test_pflags = MSG_BAND;
		test_pband = 0;
		print_tx_prim(child, prim_string(p->type));
		return test_putpmsg(child, ctrl, data, test_pband, test_pflags);
	case __TEST_O_TI_GETINFO:
		ic.ic_cmd = O_TI_GETINFO;
		ic.ic_len = sizeof(p->info_ack);
		p->info_req.PRIM_type = T_INFO_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_O_TI_OPTMGMT:
		ic.ic_cmd = O_TI_OPTMGMT;
		ic.ic_len = sizeof(p->optmgmt_ack)
		    + (test_opts ? test_olen : 0);
		p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
		p->optmgmt_req.OPT_length = test_opts ? test_olen : 0;
		p->optmgmt_req.OPT_offset = test_opts ? sizeof(p->optmgmt_req) : 0;
		p->optmgmt_req.MGMT_flags = test_mgmtflags;
		if (test_opts)
			bcopy(test_opts, ctrl->buf + p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_O_TI_BIND:
		ic.ic_cmd = O_TI_BIND;
		ic.ic_len = sizeof(p->bind_ack);
		p->bind_req.PRIM_type = T_BIND_REQ;
		p->bind_req.ADDR_length = 0;
		p->bind_req.ADDR_offset = 0;
		p->bind_req.CONIND_number = last_qlen;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_O_TI_UNBIND:
		ic.ic_cmd = O_TI_UNBIND;
		ic.ic_len = sizeof(p->ok_ack);
		p->unbind_req.PRIM_type = T_UNBIND_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST__O_TI_GETINFO:
		ic.ic_cmd = _O_TI_GETINFO;
		ic.ic_len = sizeof(p->info_ack);
		p->info_req.PRIM_type = T_INFO_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST__O_TI_OPTMGMT:
		ic.ic_cmd = _O_TI_OPTMGMT;
		ic.ic_len = sizeof(p->optmgmt_ack)
		    + (test_opts ? test_olen : 0);
		p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
		p->optmgmt_req.OPT_length = test_opts ? test_olen : 0;
		p->optmgmt_req.OPT_offset = test_opts ? sizeof(p->optmgmt_req) : 0;
		p->optmgmt_req.MGMT_flags = test_mgmtflags;
		if (test_opts)
			bcopy(test_opts, ctrl->buf + p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST__O_TI_BIND:
		ic.ic_cmd = _O_TI_BIND;
		ic.ic_len = sizeof(p->bind_ack);
		p->bind_req.PRIM_type = T_BIND_REQ;
		p->bind_req.ADDR_length = 0;
		p->bind_req.ADDR_offset = 0;
		p->bind_req.CONIND_number = last_qlen;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST__O_TI_UNBIND:
		ic.ic_cmd = _O_TI_UNBIND;
		ic.ic_len = sizeof(p->ok_ack);
		p->unbind_req.PRIM_type = T_UNBIND_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST__O_TI_GETMYNAME:
		ic.ic_cmd = _O_TI_GETMYNAME;
		ic.ic_len = sizeof(p->addr_ack);
		p->addr_req.PRIM_type = T_ADDR_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST__O_TI_GETPEERNAME:
		ic.ic_cmd = _O_TI_GETPEERNAME;
		ic.ic_len = sizeof(p->addr_ack);
		p->addr_req.PRIM_type = T_ADDR_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST__O_TI_XTI_HELLO:
		ic.ic_cmd = _O_TI_XTI_HELLO;
		ic.ic_len = 0;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST__O_TI_XTI_GET_STATE:
		ic.ic_cmd = _O_TI_XTI_GET_STATE;
		ic.ic_len = 0;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST__O_TI_XTI_CLEAR_EVENT:
		ic.ic_cmd = _O_TI_XTI_CLEAR_EVENT;
		ic.ic_len = 0;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST__O_TI_XTI_MODE:
		ic.ic_cmd = _O_TI_XTI_MODE;
		ic.ic_len = 0;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST__O_TI_TLI_MODE:
		ic.ic_cmd = _O_TI_TLI_MODE;
		ic.ic_len = 0;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_GETINFO:
		ic.ic_cmd = TI_GETINFO;
		ic.ic_len = sizeof(p->info_ack);
		p->info_req.PRIM_type = T_INFO_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_OPTMGMT:
		ic.ic_cmd = TI_OPTMGMT;
		ic.ic_len = sizeof(p->optmgmt_ack)
		    + (test_opts ? test_olen : 0);
		p->optmgmt_req.PRIM_type = T_OPTMGMT_REQ;
		p->optmgmt_req.OPT_length = test_opts ? test_olen : 0;
		p->optmgmt_req.OPT_offset = test_opts ? sizeof(p->optmgmt_req) : 0;
		p->optmgmt_req.MGMT_flags = test_mgmtflags;
		if (test_opts)
			bcopy(test_opts, ctrl->buf + p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_BIND:
		ic.ic_cmd = TI_BIND;
		ic.ic_len = sizeof(p->bind_ack);
		p->bind_req.PRIM_type = T_BIND_REQ;
		p->bind_req.ADDR_length = 0;
		p->bind_req.ADDR_offset = 0;
		p->bind_req.CONIND_number = last_qlen;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_UNBIND:
		ic.ic_cmd = TI_UNBIND;
		ic.ic_len = sizeof(p->ok_ack);
		p->unbind_req.PRIM_type = T_UNBIND_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_GETMYNAME:
		ic.ic_cmd = TI_GETMYNAME;
		ic.ic_len = sizeof(p->addr_ack);
		p->addr_req.PRIM_type = T_ADDR_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_GETPEERNAME:
		ic.ic_cmd = TI_GETPEERNAME;
		ic.ic_len = sizeof(p->addr_ack);
		p->addr_req.PRIM_type = T_ADDR_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_SETMYNAME:
		ic.ic_cmd = TI_SETMYNAME;
		ic.ic_len = sizeof(p->conn_res);
		p->conn_res.PRIM_type = T_CONN_RES;
		p->conn_res.ACCEPTOR_id = 0;
		p->conn_res.OPT_length = 0;
		p->conn_res.OPT_offset = 0;
		p->conn_res.SEQ_number = last_sequence;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_SETPEERNAME:
		ic.ic_cmd = TI_SETPEERNAME;
		ic.ic_len = sizeof(p->conn_req);
		p->conn_req.PRIM_type = T_CONN_REQ;
		p->conn_req.DEST_length = 0;
		p->conn_req.DEST_offset = 0;
		p->conn_req.OPT_length = 0;
		p->conn_req.OPT_offset = 0;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_SETMYNAME_DISC:
		ic.ic_cmd = TI_SETMYNAME;
		ic.ic_len = sizeof(p->discon_req);
		p->discon_req.PRIM_type = T_DISCON_REQ;
		p->discon_req.SEQ_number = last_sequence;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_SETPEERNAME_DISC:
		ic.ic_cmd = TI_SETPEERNAME;
		ic.ic_len = sizeof(p->discon_req);
		p->discon_req.PRIM_type = T_DISCON_REQ;
		p->discon_req.SEQ_number = last_sequence;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_SETMYNAME_DATA:
		ic.ic_cmd = TI_SETMYNAME;
		ic.ic_len = sizeof(p->conn_res) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
		p->conn_res.PRIM_type = T_CONN_RES;
		p->conn_res.ACCEPTOR_id = 0;
		p->conn_res.OPT_length = 0;
		p->conn_res.OPT_offset = 0;
		p->conn_res.SEQ_number = last_sequence;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_SETPEERNAME_DATA:
		ic.ic_cmd = TI_SETPEERNAME;
		ic.ic_len = sizeof(p->conn_req) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
		p->conn_req.PRIM_type = T_CONN_REQ;
		p->conn_req.DEST_length = 0;
		p->conn_req.DEST_offset = 0;
		p->conn_req.OPT_length = 0;
		p->conn_req.OPT_offset = 0;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_SETMYNAME_DISC_DATA:
		ic.ic_cmd = TI_SETMYNAME;
		ic.ic_len = sizeof(p->discon_req) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
		p->discon_req.PRIM_type = T_DISCON_REQ;
		p->discon_req.SEQ_number = last_sequence;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_SETPEERNAME_DISC_DATA:
		ic.ic_cmd = TI_SETPEERNAME;
		ic.ic_len = sizeof(p->discon_req) + sprintf(cbuf + sizeof(p->conn_res), "IO control test data.");
		p->discon_req.PRIM_type = T_DISCON_REQ;
		p->discon_req.SEQ_number = last_sequence;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_SYNC:
	{
		union {
			struct ti_sync_req req;
			struct ti_sync_ack ack;
		} *s = (typeof(s)) p;

		ic.ic_cmd = TI_SYNC;
		ic.ic_len = sizeof(*s);
		s->req.tsr_flags = TSRF_INFO_REQ | TSRF_IS_EXP_IN_RCVBUF | TSRF_QLEN_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	}
	case __TEST_TI_GETADDRS:
		ic.ic_cmd = TI_GETADDRS;
		ic.ic_len = sizeof(p->addr_ack);
		p->addr_req.PRIM_type = T_ADDR_REQ;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_TI_CAPABILITY:
		ic.ic_cmd = TI_CAPABILITY;
		ic.ic_len = sizeof(p->capability_ack);
		p->capability_req.PRIM_type = T_CAPABILITY_REQ;
		p->capability_req.CAP_bits1 = TC1_INFO | TC1_ACCEPTOR_ID;
		return test_ti_ioctl(child, I_STR, (intptr_t) &ic);
	case __TEST_ERR_MSG:
		/* set up message in data buffer */
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ UA_SIZE(UA_PARM_ECODE)
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.ntwk_app.len ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0)
			+ (m3ua.affect_dest.len ? UA_SIZE(UA_PARM_AFFECT_DEST) : 0)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0)
			;
		m[0] = UA_MGMT_ERR;
		m[1] = htonl(test_dlen);
		m[2] = UA_PARM_ECODE;
		m[3] = htonl(m3ua.ecode.val);
		m += 4;
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.ntwk_app.len) {
			m[0] = M3UA_PARM_NTWK_APP;
			m[1] = htonl(m3ua.ntwk_app.val);
			m += 2;
		}
		if (m3ua.affect_dest.len) {
			m[0] = UA_PARM_AFFECT_DEST;
			m[1] = htonl(m3ua.affect_dest.val);
			m += 2;
		}
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "MGMT ERR--------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_NTFY_MSG:
		/* set up message in data buffer */
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ UA_SIZE(UA_PARM_STATUS)
			+ (m3ua.aspid.len ? UA_SIZE(UA_PARM_ASPID) : 0)
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0)
			;
		m[0] = UA_MGMT_NTFY;
		m[1] = htonl(test_dlen);
		m[2] = UA_PARM_STATUS;
		m[3] = htonl(m3ua.status.val);
		m += 4;
		if (m3ua.aspid.len) {
			m[0] = UA_PARM_ASPID;
			m[1] = htonl(m3ua.aspid.val);
			m += 2;
		}
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "MGMT NTFY-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_HBEAT_REQ:
		/* set up message in data buffer */
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ UA_PHDR_SIZE + UA_PAD4(m3ua.hbdata.len);
		bzero(m, test_dlen);
		m[0] = UA_ASPS_HBEAT_REQ;
		m[1] = htonl(test_dlen);
		m[2] = UA_PHDR(UA_TAG(UA_PARM_HBDATA), m3ua.hbdata.len);
		m += 3;
		bcopy(m3ua.hbdata.cp, m, m3ua.hbdata.len);
		m += ((m3ua.info.len + 3) >> 2);
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "HBEAT REQ-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_HBEAT_ACK:
		/* set up message in data buffer */
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ UA_PHDR_SIZE + UA_PAD4(m3ua.hbdata.len);
		bzero(m, test_dlen);
		m[0] = UA_ASPS_HBEAT_ACK;
		m[1] = htonl(test_dlen);
		m[2] = UA_PHDR(UA_TAG(UA_PARM_HBDATA), m3ua.hbdata.len);
		m += 3;
		bcopy(m3ua.hbdata.cp, m, m3ua.hbdata.len);
		m += ((m3ua.info.len + 3) >> 2);
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "HBEAT ACK-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_ASPUP_REQ:
		/* set up message in data buffer */
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.aspid.len ? UA_SIZE(UA_PARM_ASPID) : 0)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0);
		m[0] = UA_ASPS_ASPUP_REQ;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.aspid.len) {
			m[0] = UA_PARM_ASPID;
			m[1] = htonl(m3ua.aspid.val);
			m += 2;
		}
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "ASPUP REQ-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_ASPUP_ACK:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.aspid.len ? UA_SIZE(UA_PARM_ASPID) : 0)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0);
		m[0] = UA_ASPS_ASPUP_ACK;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.aspid.len) {
			m[0] = UA_PARM_ASPID;
			m[1] = htonl(m3ua.aspid.val);
			m += 2;
		}
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "ASPUP ACK-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_ASPDN_REQ:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.aspid.len ? UA_SIZE(UA_PARM_ASPID) : 0)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0);
		m[0] = UA_ASPS_ASPDN_REQ;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.aspid.len) {
			m[0] = UA_PARM_ASPID;
			m[1] = htonl(m3ua.aspid.val);
			m += 2;
		}
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "ASPDN REQ-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_ASPDN_ACK:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.aspid.len ? UA_SIZE(UA_PARM_ASPID) : 0)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0);
		m[0] = UA_ASPS_ASPDN_ACK;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.aspid.len) {
			m[0] = UA_PARM_ASPID;
			m[1] = htonl(m3ua.aspid.val);
			m += 2;
		}
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "ASPDN ACK-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_REG_REQ:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ UA_PHDR_SIZE
			+ UA_SIZE(M3UA_PARM_LOC_KEY_ID)
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.tmode.len ? UA_SIZE(UA_PARM_TMODE) : 0)
			+ UA_SIZE(M3UA_PARM_DPC)
			+ (m3ua.ntwk_app.len ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0)
			+ (m3ua.si.len ? UA_PHDR_SIZE + UA_PAD4(1) : 0)
			+ (m3ua.opc.len ? UA_PHDR_SIZE + UA_PAD4(4) : 0);
		m[0] = UA_RKMM_REG_REQ;
		m[1] = htonl(test_dlen);
		m[2] = UA_PHDR(UA_TAG(M3UA_PARM_ROUTING_KEY), test_dlen - UA_MHDR_SIZE - UA_PHDR_SIZE);
		m[3] = M3UA_PARM_LOC_KEY_ID;
		m[4] = htonl(m3ua.loc_key_id.val);
		m += 5;
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.tmode.len) {
			m[0] = UA_PARM_TMODE;
			m[1] = htonl(m3ua.tmode.val);
			m += 2;
		}
		m[0] = M3UA_PARM_DPC;
		m[1] = htonl(m3ua.dpc.val);
		m += 2;
		if (m3ua.ntwk_app.len) {
			m[0] = M3UA_PARM_NTWK_APP;
			m[1] = htonl(m3ua.ntwk_app.val);
			m += 2;
		}
		if (m3ua.si.len) {
			m[0] = UA_PHDR(UA_TAG(M3UA_PARM_SI),1);
			m[1] = htonl(m3ua.si.val);
			m += 2;
		}
		if (m3ua.opc.len) {
			m[0] = UA_PHDR(UA_TAG(M3UA_PARM_OPC),4);
			m[1] = htonl(m3ua.opc.val);
			m += 2;
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "REG REQ---------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_REG_RSP:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ UA_PHDR_SIZE
			+ UA_SIZE(M3UA_PARM_LOC_KEY_ID)
			+ UA_SIZE(M3UA_PARM_REG_STATUS)
			+ UA_SIZE(UA_PARM_RC);
		m[0] = UA_RKMM_REG_RSP;
		m[1] = htonl(test_dlen);
		m[2] = UA_PHDR(UA_TAG(M3UA_PARM_REG_RESULT),test_dlen - UA_MHDR_SIZE - UA_PHDR_SIZE);
		m[3] = M3UA_PARM_LOC_KEY_ID;
		m[4] = htonl(m3ua.loc_key_id.val);
		m[5] = M3UA_PARM_REG_STATUS;
		m[6] = htonl(m3ua.reg_status.val);
		m[7] = UA_PARM_RC;
		m[8] = htonl(m3ua.rc.val);
		m += 9;
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "REG RSP---------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_DEREG_REQ:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ UA_SIZE(UA_PARM_RC);
		m[0] = UA_RKMM_DEREG_REQ;
		m[1] = htonl(test_dlen);
		m[2] = UA_PARM_RC;
		m[3] = htonl(m3ua.rc.val);
		m += 4;
		DATA_flag = 0;
		print_tx_msg(child, "DEREG REQ-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_DEREG_RSP:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ UA_PHDR_SIZE
			+ UA_SIZE(UA_PARM_RC)
			+ UA_SIZE(M3UA_PARM_DEREG_STATUS);
		m[0] = UA_RKMM_DEREG_RSP;
		m[1] = htonl(test_dlen);
		m[2] = UA_PHDR(UA_TAG(M3UA_PARM_REG_RESULT),test_dlen - UA_MHDR_SIZE - UA_PHDR_SIZE);
		m[3] = UA_PARM_RC;
		m[4] = htonl(m3ua.rc.val);
		m[5] = M3UA_PARM_DEREG_STATUS;
		m[6] = htonl(m3ua.dereg_status.val);
		m += 7;
		DATA_flag = 0;
		print_tx_msg(child, "DEREG RSP-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_ASPAC_REQ:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.tmode.len ? UA_SIZE(UA_PARM_TMODE) : 0)
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0);
		m[0] = UA_ASPT_ASPAC_REQ;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.tmode.len) {
			m[0] = UA_PARM_TMODE;
			m[1] = htonl(m3ua.tmode.val);
			m += 2;
		}
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "ASPAC REQ-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_ASPAC_ACK:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.tmode.len ? UA_SIZE(UA_PARM_TMODE) : 0)
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0);
		m[0] = UA_ASPT_ASPAC_ACK;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.tmode.len) {
			m[0] = UA_PARM_TMODE;
			m[1] = htonl(m3ua.tmode.val);
			m += 2;
		}
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "ASPAC ACK-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_ASPIA_REQ:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0);
		m[0] = UA_ASPT_ASPIA_REQ;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "ASPIA REQ-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_ASPIA_ACK:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0);
		m[0] = UA_ASPT_ASPIA_ACK;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "ASPIA ACK-------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_DAUD_MSG:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.ntwk_app.len ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0)
			+ UA_SIZE(UA_PARM_AFFECT_DEST)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0)
			;
		m[0] = UA_SNMM_DAUD;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.ntwk_app.len) {
			m[0] = M3UA_PARM_NTWK_APP;
			m[1] = htonl(m3ua.ntwk_app.val);
			m += 2;
		}
		m[0] = UA_PARM_AFFECT_DEST;
		m[1] = htonl(m3ua.affect_dest.val);
		m += 2;
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "DAUD------------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_DAVA_MSG:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.ntwk_app.len ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0)
			+ UA_SIZE(UA_PARM_AFFECT_DEST)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0)
			;
		m[0] = UA_SNMM_DAVA;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.ntwk_app.len) {
			m[0] = M3UA_PARM_NTWK_APP;
			m[1] = htonl(m3ua.ntwk_app.val);
			m += 2;
		}
		m[0] = UA_PARM_AFFECT_DEST;
		m[1] = htonl(m3ua.affect_dest.val);
		m += 2;
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "DAVA------------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_DUNA_MSG:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.ntwk_app.len ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0)
			+ UA_SIZE(UA_PARM_AFFECT_DEST)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0)
			;
		m[0] = UA_SNMM_DUNA;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.ntwk_app.len) {
			m[0] = M3UA_PARM_NTWK_APP;
			m[1] = htonl(m3ua.ntwk_app.val);
			m += 2;
		}
		m[0] = UA_PARM_AFFECT_DEST;
		m[1] = htonl(m3ua.affect_dest.val);
		m += 2;
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "DUNA------------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_DRST_MSG:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.ntwk_app.len ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0)
			+ UA_SIZE(UA_PARM_AFFECT_DEST)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0)
			;
		m[0] = UA_SNMM_DRST;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.ntwk_app.len) {
			m[0] = M3UA_PARM_NTWK_APP;
			m[1] = htonl(m3ua.ntwk_app.val);
			m += 2;
		}
		m[0] = UA_PARM_AFFECT_DEST;
		m[1] = htonl(m3ua.affect_dest.val);
		m += 2;
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "DRST------------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_DUPU_MSG:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ (m3ua.ntwk_app.len ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0)
			+ UA_SIZE(UA_PARM_AFFECT_DEST)
			+ UA_SIZE(M3UA_PARM_USER_CAUSE)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0)
			;
		m[0] = UA_SNMM_DRST;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.ntwk_app.len) {
			m[0] = M3UA_PARM_NTWK_APP;
			m[1] = htonl(m3ua.ntwk_app.val);
			m += 2;
		}
		m[0] = UA_PARM_AFFECT_DEST;
		m[1] = htonl(m3ua.affect_dest.val);
		m += 2;
		m[0] = M3UA_PARM_USER_CAUSE;
		m[1] = htonl(m3ua.user_cause.val);
		m += 2;
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "DUPU------------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	case __TEST_SCON_MSG:
		m = (typeof(m)) data->buf;
		test_dlen = UA_MHDR_SIZE
			+ (m3ua.ntwk_app.len ? UA_SIZE(M3UA_PARM_NTWK_APP) : 0)
			+ (m3ua.rc.len ? UA_SIZE(UA_PARM_RC) : 0)
			+ UA_SIZE(UA_PARM_AFFECT_DEST)
			+ (m3ua.concern_dest.len ? UA_SIZE(M3UA_PARM_CONCERN_DEST) : 0)
			+ (m3ua.cong_ind.len ? UA_SIZE(M3UA_PARM_CONG_IND) : 0)
			+ (m3ua.info.len ? UA_PHDR_SIZE + UA_PAD4(m3ua.info.len) : 0)
			;
		m[0] = UA_SNMM_SCON;
		m[1] = htonl(test_dlen);
		m += 2;
		if (m3ua.rc.len) {
			m[0] = UA_PARM_RC;
			m[1] = htonl(m3ua.rc.val);
			m += 2;
		}
		if (m3ua.ntwk_app.len) {
			m[0] = M3UA_PARM_NTWK_APP;
			m[1] = htonl(m3ua.ntwk_app.val);
			m += 2;
		}
		m[0] = UA_PARM_AFFECT_DEST;
		m[1] = htonl(m3ua.affect_dest.val);
		m += 2;
		if (m3ua.concern_dest.len) {
			m[0] = M3UA_PARM_CONCERN_DEST;
			m[1] = htonl(m3ua.concern_dest.val);
			m += 2;
		}
		if (m3ua.cong_ind.len) {
			m[0] = M3UA_PARM_CONG_IND;
			m[1] = htonl(m3ua.cong_ind.val);
			m += 2;
		}
		if (m3ua.info.len) {
			m[0] = UA_PHDR(UA_TAG(UA_PARM_INFO), m3ua.info.len);
			m++;
			bcopy(m3ua.info.cp, m, m3ua.info.len);
			m += ((m3ua.info.len + 3) >> 2);
		}
		test_dlen = (char *)m - data->buf;
		DATA_flag = 0;
		print_tx_msg(child, "SCON------------");
		test_opts = &opt_data;
		test_olen = sizeof(opt_data);
		goto test_optdata_req;
	default:
		if (show && verbose > 1)
			print_command_state(child, ":????????");
		return __RESULT_SCRIPT_ERROR;
	}
	return __RESULT_SCRIPT_ERROR;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Received event decoding and display functions.
 *
 *  -------------------------------------------------------------------------
 */
void
clear_parms(struct m3ua *m3ua)
{
	struct ua_parm *uap = (typeof(uap)) m3ua;
	int i;

	for (i = 0; i < sizeof(*m3ua) / sizeof(*uap); i++) {
		uap->cp = NULL;
		uap->len = 0;
	}
	return;
}
static int
do_decode_parm(int child, uint32_t *wp, uint32_t *e)
{
	int len, plen;

	for (wp += 2; wp < e; wp += plen) {
		len = UA_PLEN(*wp);
		if (len < 4)
			return __RESULT_DECODE_ERROR;
		plen = (len + 3) >> 2;
		if (wp + plen > e)
			return __RESULT_DECODE_ERROR;
		switch (UA_TAG(*wp)) {
		case UA_TAG(UA_PARM_DATA):
			m3ua.data.wp = wp + 1;
			m3ua.data.len = len - 4;
			m3ua.data.val = ntohl(wp[1]);
			print_string(child, "DATA");
			break;
		case UA_TAG(UA_PARM_INFO):
			m3ua.info.wp = wp + 1;
			m3ua.info.len = len - 4;
			m3ua.info.val = ntohl(wp[1]);
			print_string(child, "INFO");
			break;
		case UA_TAG(UA_PARM_RC):
			if (len < UA_PLEN(UA_PARM_RC))
				return __RESULT_DECODE_ERROR;
			m3ua.rc.wp = wp + 1;
			m3ua.rc.len = len - 4;
			m3ua.rc.val = ntohl(wp[1]);
			print_string(child, "RC");
			break;
		case UA_TAG(UA_PARM_DIAG):
			m3ua.diag.wp = wp + 1;
			m3ua.diag.len = len - 4;
			m3ua.diag.val = ntohl(wp[1]);
			print_string(child, "DIAG");
			break;
		case UA_TAG(UA_PARM_HBDATA):
			m3ua.hbdata.wp = wp + 1;
			m3ua.hbdata.len = len - 4;
			m3ua.hbdata.val = ntohl(wp[1]);
			print_string(child, "HBDATA");
			break;
		case UA_TAG(UA_PARM_REASON):
			if (len < UA_PLEN(UA_PARM_REASON))
				return __RESULT_DECODE_ERROR;
			m3ua.reason.wp = wp + 1;
			m3ua.reason.len = len - 4;
			m3ua.reason.val = ntohl(wp[1]);
			print_string(child, "REASON");
			break;
		case UA_TAG(UA_PARM_TMODE):
			if (len < UA_PLEN(UA_PARM_TMODE))
				return __RESULT_DECODE_ERROR;
			m3ua.tmode.wp = wp + 1;
			m3ua.tmode.len = len - 4;
			m3ua.tmode.val = ntohl(wp[1]);
			print_string(child, "TMODE");
			break;
		case UA_TAG(UA_PARM_ECODE):
			if (len < UA_PLEN(UA_PARM_ECODE))
				return __RESULT_DECODE_ERROR;
			m3ua.ecode.wp = wp + 1;
			m3ua.ecode.len = len - 4;
			m3ua.ecode.val = ntohl(wp[1]);
			print_string(child, "ECODE");
			break;
		case UA_TAG(UA_PARM_STATUS):
			if (len < UA_PLEN(UA_PARM_STATUS))
				return __RESULT_DECODE_ERROR;
			m3ua.status.wp = wp + 1;
			m3ua.status.len = len - 4;
			m3ua.status.val = ntohl(wp[1]);
			print_string(child, "STATUS");
			break;
		case UA_TAG(UA_PARM_ASPID):
			if (len < UA_PLEN(UA_PARM_ASPID))
				return __RESULT_DECODE_ERROR;
			m3ua.aspid.wp = wp + 1;
			m3ua.aspid.len = len - 4;
			m3ua.aspid.val = ntohl(wp[1]);
			print_string(child, "ASPID");
			break;
		case UA_TAG(M3UA_PARM_NTWK_APP):
			if (len < UA_PLEN(M3UA_PARM_NTWK_APP))
				return __RESULT_DECODE_ERROR;
			m3ua.ntwk_app.wp = wp + 1;
			m3ua.ntwk_app.len = len - 4;
			m3ua.ntwk_app.val = ntohl(wp[1]);
			print_string(child, "NA");
			break;
		case UA_TAG(M3UA_PARM_PROT_DATA1):
			m3ua.prot_data1.wp = wp + 1;
			m3ua.prot_data1.len = len - 4;
			m3ua.prot_data1.val = ntohl(wp[1]);
			print_string(child, "PROT_DATA1");
			break;
		case UA_TAG(M3UA_PARM_PROT_DATA2):
			m3ua.prot_data2.wp = wp + 1;
			m3ua.prot_data2.len = len - 4;
			m3ua.prot_data2.val = ntohl(wp[1]);
			print_string(child, "PROT_DATA2");
			break;
		case UA_TAG(UA_PARM_AFFECT_DEST):
			if (len < UA_PLEN(UA_PARM_AFFECT_DEST))
				return __RESULT_DECODE_ERROR;
			m3ua.affect_dest.wp = wp + 1;
			m3ua.affect_dest.len = len - 4;
			m3ua.affect_dest.val = ntohl(wp[1]);
			print_string(child, "APC");
			break;
		case UA_TAG(M3UA_PARM_USER_CAUSE):
			if (len < UA_PLEN(M3UA_PARM_USER_CAUSE))
				return __RESULT_DECODE_ERROR;
			m3ua.user_cause.wp = wp + 1;
			m3ua.user_cause.len = len - 4;
			m3ua.user_cause.val = ntohl(wp[1]);
			print_string(child, "CAUSE");
			break;
		case UA_TAG(M3UA_PARM_CONG_IND):
			if (len < UA_PLEN(M3UA_PARM_CONG_IND))
				return __RESULT_DECODE_ERROR;
			m3ua.cong_ind.wp = wp + 1;
			m3ua.cong_ind.len = len - 4;
			m3ua.cong_ind.val = ntohl(wp[1]);
			print_string(child, "CONG_STATUS");
			break;
		case UA_TAG(M3UA_PARM_CONCERN_DEST):
			if (len < UA_PLEN(M3UA_PARM_CONCERN_DEST))
				return __RESULT_DECODE_ERROR;
			m3ua.concern_dest.wp = wp + 1;
			m3ua.concern_dest.len = len - 4;
			m3ua.concern_dest.val = ntohl(wp[1]);
			print_string(child, "CPC");
			break;
		case UA_TAG(M3UA_PARM_ROUTING_KEY):
			m3ua.routing_key.wp = wp + 1;
			m3ua.routing_key.len = len - 4;
			m3ua.routing_key.val = ntohl(wp[1]);
			/* meaningless nesting */
			if (do_decode_parm(child, wp + 1, wp + plen) == __RESULT_DECODE_ERROR)
				return __RESULT_DECODE_ERROR;
			print_string(child, "RK");
			break;
		case UA_TAG(M3UA_PARM_REG_RESULT):
			if (len < UA_PLEN(M3UA_PARM_REG_RESULT))
				return __RESULT_DECODE_ERROR;
			m3ua.reg_result.wp = wp + 1;
			m3ua.reg_result.len = len - 4;
			m3ua.reg_result.val = ntohl(wp[1]);
			/* meaningless nesting */
			if (do_decode_parm(child, wp + 1, wp + plen) == __RESULT_DECODE_ERROR)
				return __RESULT_DECODE_ERROR;
			print_string(child, "REG RESULT");
			break;
		case UA_TAG(M3UA_PARM_DEREG_RESULT):
			if (len < UA_PLEN(M3UA_PARM_DEREG_RESULT))
				return __RESULT_DECODE_ERROR;
			m3ua.dereg_result.wp = wp + 1;
			m3ua.dereg_result.len = len - 4;
			m3ua.dereg_result.val = ntohl(wp[1]);
			/* meaningless nesting */
			if (do_decode_parm(child, wp + 1, wp + plen) == __RESULT_DECODE_ERROR)
				return __RESULT_DECODE_ERROR;
			print_string(child, "DEREG RESULT");
			break;
		case UA_TAG(M3UA_PARM_LOC_KEY_ID):
			if (len < UA_PLEN(M3UA_PARM_LOC_KEY_ID))
				return __RESULT_DECODE_ERROR;
			m3ua.loc_key_id.wp = wp + 1;
			m3ua.loc_key_id.len = len - 4;
			m3ua.loc_key_id.val = ntohl(wp[1]);
			print_string(child, "KEYID");
			break;
		case UA_TAG(M3UA_PARM_DPC):
			m3ua.dpc.wp = wp + 1;
			m3ua.dpc.len = len - 4;
			m3ua.dpc.val = ntohl(wp[1]);
			print_string(child, "DPC");
			break;
		case UA_TAG(M3UA_PARM_SI):
			m3ua.si.wp = wp + 1;
			m3ua.si.len = len - 4;
			m3ua.si.val = ntohl(wp[1]);
			print_string(child, "SI");
			break;
		case UA_TAG(M3UA_PARM_SSN):
			m3ua.ssn.wp = wp + 1;
			m3ua.ssn.len = len - 4;
			m3ua.ssn.val = ntohl(wp[1]);
			print_string(child, "SSN");
			break;
		case UA_TAG(M3UA_PARM_OPC):
			m3ua.opc.wp = wp + 1;
			m3ua.opc.len = len - 4;
			m3ua.opc.val = ntohl(wp[1]);
			print_string(child, "OPC");
			break;
		case UA_TAG(M3UA_PARM_CIC):
			m3ua.cic.wp = wp + 1;
			m3ua.cic.len = len - 4;
			m3ua.cic.val = ntohl(wp[1]);
			print_string(child, "CIC");
			break;
		case UA_TAG(M3UA_PARM_PROT_DATA3):
			m3ua.prot_data3.wp = wp + 1;
			m3ua.prot_data3.len = len - 4;
			m3ua.prot_data3.val = ntohl(wp[1]);
			print_string(child, "PROT_DATA3");
			break;
		case UA_TAG(M3UA_PARM_REG_STATUS):
			if (len < UA_PLEN(M3UA_PARM_REG_STATUS))
				return __RESULT_DECODE_ERROR;
			m3ua.reg_status.wp = wp + 1;
			m3ua.reg_status.len = len - 4;
			m3ua.reg_status.val = ntohl(wp[1]);
			print_string(child, "REG_STATUS");
			break;
		case UA_TAG(M3UA_PARM_DEREG_STATUS):
			if (len < UA_PLEN(M3UA_PARM_DEREG_STATUS))
				return __RESULT_DECODE_ERROR;
			m3ua.dereg_status.wp = wp + 1;
			m3ua.dereg_status.len = len - 4;
			m3ua.dereg_status.val = ntohl(wp[1]);
			print_string(child, "DEREG_STATUS");
			break;
		default:
			/* ignore it */
			print_string(child, "(unknown)");
			break;
		}
	}
	if (wp > e)
		return __RESULT_DECODE_ERROR;
	return __RESULT_SUCCESS;
}
static int
do_decode_data(int child, struct strbuf *ctrl, struct strbuf *data)
{
	int event = __RESULT_DECODE_ERROR;

	if (data->len >= 0) {
		event = __TEST_DATA;
		print_rx_data(child, "M_DATA----------", data->len);

		if (data->len >= UA_MHDR_SIZE) {
			uint32_t *wp = (typeof(wp)) data->buf;
			uint32_t *e = (typeof(e)) (data->buf + UA_PAD4(ntohl(wp[1])));

			if ((char *) e > (char *) wp + data->len) {
				print_rx_msg(child, "(BAD MSG)-------");
				return __RESULT_DECODE_ERROR;
			}
			switch (UA_MSG_CLAS(wp[0])) {
			case UA_CLASS_MGMT:
				switch (UA_MSG_TYPE(wp[0])) {
				case UA_TYPE(UA_MGMT_ERR):
					print_rx_msg(child, "ERROR-----------");
					event = __TEST_ERR_MSG;
					break;
				case UA_TYPE(UA_MGMT_NTFY):
					print_rx_msg(child, "NOTIFY----------");
					event = __TEST_NTFY_MSG;
					break;
				default:
					print_rx_msg(child, "MGMT UKNOWN-----");
					event = __RESULT_DECODE_ERROR;
					break;
				}
				break;
			case UA_CLASS_ASPS:
				switch (UA_MSG_TYPE(wp[0])) {
				case UA_TYPE(UA_ASPS_HBEAT_REQ):
					print_rx_msg(child, "BEAT REQ--------");
					event = __TEST_HBEAT_REQ;
					break;
				case UA_TYPE(UA_ASPS_HBEAT_ACK):
					print_rx_msg(child, "BEAT ACK--------");
					event = __TEST_HBEAT_ACK;
					break;
				case UA_TYPE(UA_ASPS_ASPUP_REQ):
					print_rx_msg(child, "ASPUP REQ-------");
					event = __TEST_ASPUP_REQ;
					break;
				case UA_TYPE(UA_ASPS_ASPUP_ACK):
					print_rx_msg(child, "ASPUP ACK-------");
					event = __TEST_ASPUP_ACK;
					break;
				case UA_TYPE(UA_ASPS_ASPDN_REQ):
					print_rx_msg(child, "ASPDN REQ-------");
					event = __TEST_ASPDN_REQ;
					break;
				case UA_TYPE(UA_ASPS_ASPDN_ACK):
					print_rx_msg(child, "ASPDN ACK-------");
					event = __TEST_ASPDN_ACK;
					break;
				default:
					print_rx_msg(child, "ASPS UNKNOWN----");
					event = __RESULT_DECODE_ERROR;
					break;
				}
				break;
			case UA_CLASS_ASPT:
				switch (UA_MSG_TYPE(wp[0])) {
				case UA_TYPE(UA_ASPT_ASPAC_REQ):
					print_rx_msg(child, "ASPAC REQ-------");
					event = __TEST_ASPAC_REQ;
					break;
				case UA_TYPE(UA_ASPT_ASPAC_ACK):
					print_rx_msg(child, "ASPAC ACK-------");
					event = __TEST_ASPAC_ACK;
					break;
				case UA_TYPE(UA_ASPT_ASPIA_REQ):
					print_rx_msg(child, "ASPIA REQ-------");
					event = __TEST_ASPIA_REQ;
					break;
				case UA_TYPE(UA_ASPT_ASPIA_ACK):
					print_rx_msg(child, "ASPIA ACK-------");
					event = __TEST_ASPIA_ACK;
					break;
				default:
					print_rx_msg(child, "ASPT UNKNOWN----");
					event = __RESULT_DECODE_ERROR;
					break;
				}
				break;
			case UA_CLASS_RKMM:
				switch (UA_MSG_TYPE(wp[0])) {
				case UA_TYPE(UA_RKMM_REG_REQ):
					print_rx_msg(child, "REG REQ---------");
					event = __TEST_REG_REQ;
					break;
				case UA_TYPE(UA_RKMM_REG_RSP):
					print_rx_msg(child, "REG RSP---------");
					event = __TEST_REG_RSP;
					break;
				case UA_TYPE(UA_RKMM_DEREG_REQ):
					print_rx_msg(child, "DEREG REQ-------");
					event = __TEST_DEREG_REQ;
					break;
				case UA_TYPE(UA_RKMM_DEREG_RSP):
					print_rx_msg(child, "DEREG RSP-------");
					event = __TEST_DEREG_RSP;
					break;
				default:
					print_rx_msg(child, "RKMM UNKNOWN----");
					event = __RESULT_DECODE_ERROR;
					break;
				}
				break;
			case UA_CLASS_XFER:
				switch (UA_MSG_TYPE(wp[0])) {
				case UA_TYPE(UA_XFER_DATA):
					print_rx_msg(child, "XFER DATA-------");
					event = __TEST_XFER_DATA;
					break;
				default:
					print_rx_msg(child, "XFER UNKNOWN----");
					event = __RESULT_DECODE_ERROR;
					break;
				}
				break;
			case UA_CLASS_SNMM:
				switch (UA_MSG_TYPE(wp[0])) {
				case UA_TYPE(UA_SNMM_DUNA):
					print_rx_msg(child, "DUNA------------");
					event = __TEST_DUNA_MSG;
					break;
				case UA_TYPE(UA_SNMM_DAVA):
					print_rx_msg(child, "DAVA------------");
					event = __TEST_DAVA_MSG;
					break;
				case UA_TYPE(UA_SNMM_DAUD):
					print_rx_msg(child, "DAUD------------");
					event = __TEST_DAUD_MSG;
					break;
				case UA_TYPE(UA_SNMM_SCON):
					print_rx_msg(child, "SCON------------");
					event = __TEST_SCON_MSG;
					break;
				case UA_TYPE(UA_SNMM_DUPU):
					print_rx_msg(child, "DUPU------------");
					event = __TEST_DUPU_MSG;
					break;
				case UA_TYPE(UA_SNMM_DRST):
					print_rx_msg(child, "DRST------------");
					event = __TEST_DRST_MSG;
					break;
				default:
					print_rx_msg(child, "SNMM UNKNOWN----");
					event = __RESULT_DECODE_ERROR;
					break;
				}
				break;
			default:
				print_rx_msg(child, "M3UA UNKNOWN----");
				break;
			}
			if (do_decode_parm(child, wp, e) == __RESULT_DECODE_ERROR)
				event = __RESULT_DECODE_ERROR;
		}
	}
	return ((last_event = event));
}

static int
do_decode_ctrl(int child, struct strbuf *ctrl, struct strbuf *data)
{
	int event = __RESULT_DECODE_ERROR;
	union T_primitives *p = (union T_primitives *) ctrl->buf;

	if (ctrl->len >= sizeof(p->type)) {
		switch ((last_prim = p->type)) {
		case T_CONN_REQ:
			event = __TEST_CONN_REQ;
			print_rx_prim(child, prim_string(p->type));
			print_options(child, cbuf, p->conn_req.OPT_offset, p->conn_req.OPT_length);
			break;
		case T_CONN_RES:
			event = __TEST_CONN_RES;
			print_rx_prim(child, prim_string(p->type));
			print_options(child, cbuf, p->conn_res.OPT_offset, p->conn_res.OPT_length);
			break;
		case T_DISCON_REQ:
			event = __TEST_DISCON_REQ;
			last_sequence = p->discon_req.SEQ_number;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_DATA_REQ:
			event = __TEST_DATA_REQ;
			MORE_flag = p->data_req.MORE_flag;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_EXDATA_REQ:
			event = __TEST_EXDATA_REQ;
			MORE_flag = p->data_req.MORE_flag;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_INFO_REQ:
			event = __TEST_INFO_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_BIND_REQ:
			event = __TEST_BIND_REQ;
			last_qlen = p->bind_req.CONIND_number;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_UNBIND_REQ:
			event = __TEST_UNBIND_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_UNITDATA_REQ:
			event = __TEST_UNITDATA_REQ;
			print_rx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length));
#else
			print_addrs(child, cbuf + p->unitdata_req.DEST_offset, p->unitdata_req.DEST_length);
#endif
			print_options(child, cbuf, p->unitdata_req.OPT_offset, p->unitdata_req.OPT_length);
			break;
		case T_OPTMGMT_REQ:
			event = __TEST_OPTMGMT_REQ;
			print_rx_prim(child, prim_string(p->type));
			print_mgmtflag(child, p->optmgmt_req.MGMT_flags);
			print_options(child, cbuf, p->optmgmt_req.OPT_offset, p->optmgmt_req.OPT_length);
			break;
		case T_ORDREL_REQ:
			event = __TEST_ORDREL_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_OPTDATA_REQ:
			event = __TEST_OPTDATA_REQ;
			if (p->optdata_req.DATA_flag & T_ODF_EX) {
				if (p->optdata_req.DATA_flag & T_ODF_MORE)
					print_rx_prim(child, "T_OPTDATA_REQ!+ ");
				else
					print_rx_prim(child, "T_OPTDATA_REQ!  ");
			} else {
				if (p->optdata_req.DATA_flag & T_ODF_MORE)
					print_rx_prim(child, "T_OPTDATA_REQ+  ");
				else
					print_rx_prim(child, "T_OPTDATA_REQ   ");
			}
			print_options(child, cbuf, p->optdata_req.OPT_offset, p->optdata_req.OPT_length);
			break;
		case T_ADDR_REQ:
			event = __TEST_ADDR_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_CAPABILITY_REQ:
			event = __TEST_CAPABILITY_REQ;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_CONN_IND:
			event = __TEST_CONN_IND;
			last_sequence = p->conn_ind.SEQ_number;
			print_rx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->conn_ind.SRC_offset, p->conn_ind.SRC_length));
#else
			print_addrs(child, cbuf + p->conn_ind.SRC_offset, p->conn_ind.SRC_length);
#endif
			print_options(child, cbuf, p->conn_ind.OPT_offset, p->conn_ind.OPT_length);
			break;
		case T_CONN_CON:
			event = __TEST_CONN_CON;
			print_rx_prim(child, prim_string(p->type));
			print_options(child, cbuf, p->conn_con.OPT_offset, p->conn_con.OPT_length);
			break;
		case T_DISCON_IND:
			event = __TEST_DISCON_IND;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_DATA_IND:
			event = __TEST_DATA_IND;
			if ((verbose && show) || verbose > 4)
				print_rx_prim(child, prim_string(p->type));
			break;
		case T_EXDATA_IND:
			event = __TEST_EXDATA_IND;
			if ((verbose && show) || verbose > 4)
				print_rx_prim(child, prim_string(p->type));
			break;
		case T_INFO_ACK:
			event = __TEST_INFO_ACK;
			last_info = p->info_ack;
			print_ack_prim(child, prim_string(p->type));
			print_info(child, &p->info_ack);
			break;
		case T_BIND_ACK:
			event = __TEST_BIND_ACK;
			last_qlen = p->bind_ack.CONIND_number;
			print_ack_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length));
#else
			print_addrs(child, cbuf + p->bind_ack.ADDR_offset, p->bind_ack.ADDR_length);
#endif
			break;
		case T_ERROR_ACK:
			event = __TEST_ERROR_ACK;
			last_t_errno = p->error_ack.TLI_error;
			last_errno = p->error_ack.UNIX_error;
			print_ack_prim(child, prim_string(p->type));
			print_string(child, terrno_string(p->error_ack.TLI_error, p->error_ack.UNIX_error));
			break;
		case T_OK_ACK:
			event = __TEST_OK_ACK;
			print_ack_prim(child, prim_string(p->type));
			break;
		case T_UNITDATA_IND:
			event = __TEST_UNITDATA_IND;
			print_rx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->unitdata_ind.SRC_offset, p->unitdata_ind.SRC_length));
#else
			print_addrs(child, cbuf + p->unitdata_ind.SRC_offset, p->unitdata_ind.SRC_length);
#endif
			print_options(child, cbuf, p->unitdata_ind.OPT_offset, p->unitdata_ind.OPT_length);
			break;
		case T_UDERROR_IND:
			event = __TEST_UDERROR_IND;
			print_rx_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->uderror_ind.DEST_offset, p->uderror_ind.DEST_length));
#else
			print_addrs(child, cbuf + p->uderror_ind.DEST_offset, p->uderror_ind.DEST_length);
#endif
			print_options(child, cbuf, p->uderror_ind.OPT_offset, p->uderror_ind.OPT_length);
			print_string(child, etype_string(p->uderror_ind.ERROR_type));
			break;
		case T_OPTMGMT_ACK:
			event = __TEST_OPTMGMT_ACK;
			test_mgmtflags = p->optmgmt_ack.MGMT_flags;
			print_ack_prim(child, prim_string(p->type));
			print_mgmtflag(child, p->optmgmt_ack.MGMT_flags);
			print_options(child, cbuf, p->optmgmt_ack.OPT_offset, p->optmgmt_ack.OPT_length);
			break;
		case T_ORDREL_IND:
			event = __TEST_ORDREL_IND;
			print_rx_prim(child, prim_string(p->type));
			break;
		case T_OPTDATA_IND:
			test_dlen = data ? data->len : 0;
			if (p->optdata_ind.DATA_flag & T_ODF_EX) {
				event = __TEST_EXP_OPTDATA_IND;
				if ((verbose && show) || verbose > 4) {
					if (p->optdata_ind.DATA_flag & T_ODF_MORE)
						print_rx_prim(child, "T_OPTDATA_IND!+ ");
					else
						print_rx_prim(child, "T_OPTDATA_IND!  ");
				}
			} else {
				event = __TEST_NRM_OPTDATA_IND;
				if ((verbose && show) || verbose > 4) {
					if (p->optdata_ind.DATA_flag & T_ODF_MORE)
						print_rx_prim(child, "T_OPTDATA_IND+  ");
					else
						print_rx_prim(child, "T_OPTDATA_IND   ");
				}
			}
			if (p->optdata_ind.OPT_length) {
				struct t_opthdr *oh;
				unsigned char *op = (unsigned char *) p + p->optdata_ind.OPT_offset;
				int olen = p->optdata_ind.OPT_length;

				for (oh = _T_OPT_FIRSTHDR_OFS(op, olen, 0); oh; oh = _T_OPT_NEXTHDR_OFS(op, olen, oh, 0)) {
					if (oh->level == T_INET_SCTP) {
						switch (oh->name) {
						case T_SCTP_SID:
							sid[child] = (*((t_scalar_t *) (oh + 1))) & 0xffff;
							opt_data.sid_val = sid[child];
							break;
						case T_SCTP_PPI:
							ppi[child] = (*((t_scalar_t *) (oh + 1))) & 0xffffffff;
							opt_data.ppi_val = ppi[child];
							break;
						}
					}
				}
			}
			if ((verbose && show) || verbose > 4)
				print_options(child, cbuf, p->optdata_ind.OPT_offset, p->optdata_ind.OPT_length);
			break;
		case T_ADDR_ACK:
			event = __TEST_ADDR_ACK;
			print_ack_prim(child, prim_string(p->type));
#ifndef SCTP_VERSION_2
			print_string(child, addr_string(cbuf + p->addr_ack.LOCADDR_offset, p->addr_ack.LOCADDR_length));
			print_string(child, addr_string(cbuf + p->addr_ack.REMADDR_offset, p->addr_ack.REMADDR_length));
#else
			print_addrs(child, cbuf + p->addr_ack.LOCADDR_offset, p->addr_ack.LOCADDR_length);
			print_addrs(child, cbuf + p->addr_ack.REMADDR_offset, p->addr_ack.REMADDR_length);
#endif
			break;
		case T_CAPABILITY_ACK:
			event = __TEST_CAPABILITY_ACK;
			last_info = p->capability_ack.INFO_ack;
			print_ack_prim(child, prim_string(p->type));
			break;
		default:
			event = __EVENT_UNKNOWN;
			print_no_prim(child, p->type);
			break;
		}
		if (data && data->len >= 0)
			event = do_decode_data(child, ctrl, data);
	}
	return ((last_event = event));
}

static int
do_decode_msg(int child, struct strbuf *ctrl, struct strbuf *data)
{
	if (ctrl->len > 0) {
		if ((last_event = do_decode_ctrl(child, ctrl, data)) != __EVENT_UNKNOWN)
			return time_event(child, last_event);
	} else if (data->len > 0) {
		if ((last_event = do_decode_data(child, ctrl, data)) != __EVENT_UNKNOWN)
			return time_event(child, last_event);
	}
	return ((last_event = __EVENT_NO_MSG));
}

int
wait_event(int child, int wait)
{
	while (1) {
		struct pollfd pfd[] = { {test_fd[child], POLLIN | POLLPRI, 0} };

		if (timer_timeout) {
			timer_timeout = 0;
			print_timeout(child);
			last_event = __EVENT_TIMEOUT;
			return time_event(child, __EVENT_TIMEOUT);
		}
		if (verbose > 4)
			print_syscall(child, "poll()");
		pfd[0].fd = test_fd[child];
		pfd[0].events = POLLIN | POLLPRI | POLLRDNORM | POLLRDBAND | POLLMSG | POLLERR | POLLHUP;
		pfd[0].revents = 0;
		switch (poll(pfd, 1, wait)) {
		case -1:
			if (last_errno == EAGAIN || last_errno == EINTR || last_errno == ERESTART)
				break;
			print_errno(child, (last_errno = errno));
			return (__RESULT_FAILURE);
		case 0:
			if (verbose > 4)
				print_success(child);
			print_nothing(child);
			last_event = __EVENT_NO_MSG;
			return time_event(child, __EVENT_NO_MSG);
		case 1:
			if (verbose > 4)
				print_success(child);
			if (pfd[0].revents) {
				int ret;

				ctrl.maxlen = BUFSIZE;
				ctrl.len = -1;
				ctrl.buf = cbuf;
				data.maxlen = BUFSIZE;
				data.len = -1;
				data.buf = dbuf;
				flags = 0;
				for (;;) {
					if ((verbose > 3 && show) || (verbose > 5 && show_msg))
						print_syscall(child, "getmsg()");
					if ((ret = getmsg(test_fd[child], &ctrl, &data, &flags)) >= 0)
						break;
					if (last_errno == EINTR || last_errno == ERESTART)
						continue;
					print_errno(child, (last_errno = errno));
					if (last_errno == EAGAIN)
						break;
					return __RESULT_FAILURE;
				}
				if (ret < 0)
					break;
				if (ret == 0) {
					if ((verbose > 3 && show) || (verbose > 5 && show_msg))
						print_success(child);
					if ((verbose > 3 && show) || (verbose > 5 && show_msg)) {
						int i;

						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "gotmsg from %d [%d,%d]:\n", child, ctrl.len, data.len);
						fprintf(stdout, "[");
						for (i = 0; i < ctrl.len; i++)
							fprintf(stdout, "%02X", ctrl.buf[i]);
						fprintf(stdout, "]\n");
						fprintf(stdout, "[");
						for (i = 0; i < data.len; i++)
							fprintf(stdout, "%02X", data.buf[i]);
						fprintf(stdout, "]\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
					}
					last_prio = (flags == RS_HIPRI);
					if ((last_event = do_decode_msg(child, &ctrl, &data)) != __EVENT_NO_MSG)
						return time_event(child, last_event);
				}
			}
		default:
			break;
		}
	}
	return __EVENT_UNKNOWN;
}

int
get_event(int child)
{
	return wait_event(child, -1);
}

int
get_data(int child, int action)
{
	int ret = 0;

	switch (action) {
	case __TEST_READ:
	{
		char buf[BUFSIZE];

		test_read(child, buf, BUFSIZE);
		ret = last_retval;
		break;
	}
	case __TEST_READV:
	{
		char buf[BUFSIZE];
		static const size_t count = 4;
		struct iovec vector[] = {
			{buf, (BUFSIZE >> 2)},
			{buf + (BUFSIZE >> 2), (BUFSIZE >> 2)},
			{buf + (BUFSIZE >> 2) + (BUFSIZE >> 2), (BUFSIZE >> 2)},
			{buf + (BUFSIZE >> 2) + (BUFSIZE >> 2) + (BUFSIZE >> 2), (BUFSIZE >> 2)}
		};

		test_readv(child, vector, count);
		ret = last_retval;
		break;
	}
	case __TEST_GETMSG:
	{
		char buf[BUFSIZE];
		struct strbuf data = { BUFSIZE, 0, buf };

		if (test_getmsg(child, NULL, &data, &test_gflags) == __RESULT_FAILURE) {
			ret = last_retval;
			break;
		}
		ret = data.len;
		break;
	}
	case __TEST_GETPMSG:
	{
		char buf[BUFSIZE];
		struct strbuf data = { BUFSIZE, 0, buf };

		if (test_getpmsg(child, NULL, &data, &test_gband, &test_gflags) == __RESULT_FAILURE) {
			ret = last_retval;
			break;
		}
		ret = data.len;
		break;
	}
	}
	return (ret);
}

int
expect(int child, int wait, int want)
{
	if ((last_event = wait_event(child, wait)) == want)
		return (__RESULT_SUCCESS);
	print_expect(child, want);
	return (__RESULT_FAILURE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Preambles and postambles
 *
 *  -------------------------------------------------------------------------
 */
static int
preamble_none(int child)
{
	switch (child) {
	case CHILD_PTU:
		break;
	case CHILD_IUT:
		break;
	}
	return __RESULT_SUCCESS;
}

static int
postamble_none(int child)
{
	switch (child) {
	case CHILD_PTU:
		break;
	case CHILD_IUT:
		break;
	}
	return __RESULT_SUCCESS;
}

static int
preamble_base(int child)
{
	if (preamble_none(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

static int
postamble_base(int child)
{
	int failed = 0;

	while (1) {
		expect(child, SHORT_WAIT, __EVENT_NO_MSG);
		switch (last_event) {
		case __EVENT_NO_MSG:
		case __EVENT_TIMEOUT:
			break;
		case __RESULT_FAILURE:
			break;
		default:
			failed = failed ? : state;
			state++;
			continue;
		}
		break;
	}
	state++;
	if (postamble_none(child) != __RESULT_SUCCESS)
		failed = failed ? : state;
	state++;
	if (failed)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	state = failed;
	return (__RESULT_FAILURE);
}

static int
preamble_info(int child)
{
	if (do_signal(child, __TEST_INFO_REQ))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK))
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

static int
postamble_info(int child)
{
	if (do_signal(child, __TEST_INFO_REQ))
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK))
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

static int
preamble_unbound(int child)
{
	if (preamble_info(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (last_info.CURRENT_state != TS_UNBND) {
		failure_string = FAILURE_STRING("bad CURRENT_state");
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
postamble_unbound(int child)
{
	if (postamble_info(child) != __RESULT_SUCCESS)
		goto failure;
	if (last_info.CURRENT_state != TS_UNBND) {
		failure_string = FAILURE_STRING("bad CURRENT_state");
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return (__RESULT_FAILURE);
}

static int
preamble_options(int child)
{
	if (preamble_unbound(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_mgmtflags = T_NEGOTIATE;
	test_opts = &opt_optm;
	test_olen = sizeof(opt_optm);
	if (do_signal(child, __TEST_OPTMGMT_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OPTMGMT_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (test_mgmtflags != T_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return (__RESULT_FAILURE);
}

static int
postamble_options(int child)
{
	if (postamble_unbound(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return (__RESULT_FAILURE);
}

static int
preamble_bind(int child)
{
	if (preamble_options(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_addr = addrs[child];
	test_alen = anums[child] * sizeof(addrs[child][0]);
	last_qlen = (child == CHILD_IUT) ? !iut_connects : iut_connects;
	if (do_signal(child, __TEST_BIND_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, SHORT_WAIT, __TEST_BIND_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

static int
postamble_unbind(int child)
{
	int failed = 0;

	if (stop_tt())
		failed = failed ? : state;
	state++;
	if (do_signal(child, __TEST_UNBIND_REQ))
		failed = failed ? : state;
	state++;
	if (expect(child, NORMAL_WAIT, __TEST_OK_ACK))
		failed = failed ? : state;
	state++;
	if (postamble_options(child) != __RESULT_SUCCESS)
		failed = failed ? : state;
	if (failed) {
		state = failed;
		goto failure;
	}
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
preamble_connect(int child)
{
	if (preamble_bind(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if ((child == CHILD_PTU && iut_connects == 0) || (child == CHILD_IUT && iut_connects != 0)) {
		int other = (child + 1) % 2;

		/* keep trying to connect until successful or the child times out */
		for (;;) {
			test_addr = addrs[other];
			test_alen = anums[other] * sizeof(addrs[other][0]);
			test_data = NULL;
			test_opts = &opt_conn;
			test_olen = sizeof(opt_conn);
			if (expect(child, LONG_WAIT, __EVENT_NO_MSG) != __RESULT_SUCCESS)
				goto failure;
			state++;
			if (do_signal(child, __TEST_CONN_REQ) != __RESULT_SUCCESS)
				goto failure;
			state++;
			if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
				goto failure;
			state++;
			if (expect(child, INFINITE_WAIT, __TEST_CONN_CON) != __RESULT_SUCCESS) {
				if (last_event == __TEST_DISCON_IND) {
					state++;
					test_msleep(child, NORMAL_WAIT);
					state++;
					continue;
				}
				goto failure;
			}
			state++;
			break;
		}
		if (do_signal(child, __TEST_INFO_REQ))
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK))
			goto failure;
		state++;
		if (last_info.CURRENT_state != TS_DATA_XFER) {
			failure_string = FAILURE_STRING("bad CURRENT_state");
			goto failure;
		}
	} else {
		if (expect(child, INFINITE_WAIT, __TEST_CONN_IND) != __RESULT_SUCCESS)
			goto failure;
		state++;
		test_resfd = test_fd[child];
		test_data = NULL;
		test_opts = &opt_conn;
		test_olen = sizeof(opt_conn);
		if (do_signal(child, __TEST_CONN_RES) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
			goto failure;
		state++;
		if (do_signal(child, __TEST_INFO_REQ))
			goto failure;
		state++;
		if (expect(child, NORMAL_WAIT, __TEST_INFO_ACK))
			goto failure;
		state++;
		if (last_info.CURRENT_state != TS_DATA_XFER) {
			failure_string = FAILURE_STRING("bad CURRENT_state");
			goto failure;
		}
	}
	return (__RESULT_SUCCESS);
      failure:
	return (__RESULT_FAILURE);
}

int
postamble_disconnect(int child)
{
	int failed = 0;
	const char *fail_string = NULL;

	for (;;) {
		expect(child, SHORT_WAIT, __EVENT_NO_MSG);
		switch (last_event) {
		case __EVENT_NO_MSG:
		case __EVENT_TIMEOUT:
			break;
		case __RESULT_FAILURE:
			failed = failed ? : state;
			break;
		case __TEST_DISCON_IND:
			if ((child == CHILD_IUT && iut_connects) || child == CHILD_PTU)
				goto got_disconnect;
			failed = failed ? : state;
			break;
		default:
			failed = failed ? : state;
			state++;
			continue;
		}
		break;
	}
	state++;
	if ((child == CHILD_IUT && iut_connects) || child == CHILD_PTU) {
		test_data = NULL;
		last_sequence = 0;
		if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
			failed = failed ? : state;
		state++;
		if (expect(child, SHORT_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
			failed = failed ? : state;
	} else {
		if (expect(child, LONGEST_WAIT, __TEST_DISCON_IND) != __RESULT_SUCCESS) {
			failed = failed ? : state;
			fail_string = fail_string ? : "No T_DISCON_IND.";
			test_data = NULL;
			last_sequence = 0;
			if (do_signal(child, __TEST_DISCON_REQ) != __RESULT_SUCCESS)
				failed = failed ? : state;
			state++;
			if (expect(child, NORMAL_WAIT, __TEST_OK_ACK) != __RESULT_SUCCESS)
				failed = failed ? : state;
		}
	}
      got_disconnect:
	state++;
	if (postamble_unbind(child) != __RESULT_SUCCESS)
		failed = failed ? : state;
	state++;
	if (stop_tt() != __RESULT_SUCCESS)
		failed = failed ? : state;
	state++;
	if (failed)
		goto failure;
	return __RESULT_SUCCESS;
      failure:
	state = failed;
	failure_string = failure_string ? : fail_string;
	return __RESULT_FAILURE;
}

int
postamble_release(int child)
{
	int failed = 0;
	const char *fail_string = NULL;

	for (;;) {
		expect(child, SHORT_WAIT, __EVENT_NO_MSG);
		switch (last_event) {
		case __EVENT_NO_MSG:
		case __EVENT_TIMEOUT:
			break;
		case __RESULT_FAILURE:
			failed = failed ? : state;
			break;
		case __TEST_ORDREL_IND:
			if ((child == CHILD_IUT && iut_connects) || child == CHILD_PTU)
				goto got_ordrelease;
			failed = failed ? : state;
			break;
		default:
			failed = failed ? : state;
			state++;
			continue;
		}
		break;
	}
	state++;
	if ((child == CHILD_IUT && iut_connects) || child == CHILD_PTU) {
		test_data = NULL;
		if (do_signal(child, __TEST_ORDREL_REQ) != __RESULT_SUCCESS)
			failed = failed ? : state;
		state++;
		if (expect(child, INFINITE_WAIT, __TEST_ORDREL_IND) != __RESULT_SUCCESS)
			failed = failed ? : state;
	} else {
		if (expect(child, LONGEST_WAIT, __TEST_ORDREL_IND) != __RESULT_SUCCESS) {
			failed = failed ? : state;
			fail_string = fail_string ? : "No T_ORDREL_IND.";
			test_data = NULL;
			if (do_signal(child, __TEST_ORDREL_REQ) != __RESULT_SUCCESS)
				failed = failed ? : state;
			state++;
			if (expect(child, INFINITE_WAIT, __TEST_ORDREL_IND) != __RESULT_SUCCESS)
				failed = failed ? : state;
		} else {
      got_ordrelease:
			test_data = NULL;
			state++;
			if (do_signal(child, __TEST_ORDREL_REQ) != __RESULT_SUCCESS)
				failed = failed ? : state;
		}
	}
	state++;
	if (failed) {
		state += 10;
		if (postamble_disconnect(child) != __RESULT_SUCCESS)
			failed = failed ? : state;
	} else {
		if (postamble_unbind(child) != __RESULT_SUCCESS)
			failed = failed ? : state;
	}
	state++;
	if (stop_tt() != __RESULT_SUCCESS)
		failed = failed ? : state;
	state++;
	if (failed)
		goto failure;
	return (__RESULT_SUCCESS);
      failure:
	state = failed;
	failure_string = failure_string ? : fail_string;
	return (__RESULT_FAILURE);
}

int
preamble_aspup(int child)
{
	if (preamble_connect(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (child == CHILD_PTU) {
		for (;;) {
			if (expect(child, INFINITE_WAIT, __TEST_ASPUP_REQ) != __RESULT_SUCCESS) {
				switch (last_event) {
				case __TEST_HBEAT_REQ:
					do_signal(child, __TEST_HBEAT_ACK);
				case __TEST_HBEAT_ACK:
					state++;
					continue;
				}
				goto failure;
			}
			break;
		}
		state++;
		if (do_signal(child, __TEST_ASPUP_ACK) != __RESULT_SUCCESS)
			goto failure;
#if 0
		state++;
		opt_data.sid_val = 1;
		if (do_signal(child, __TEST_HBEAT_REQ) != __RESULT_SUCCESS)
			goto failure;
#endif
	}
	if (child == CHILD_IUT) {
		if (do_signal(child, __TEST_ASPUP_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		for (;;) {
			if (expect(child, INFINITE_WAIT, __TEST_ASPUP_ACK) != __RESULT_SUCCESS) {
				switch (last_event) {
				case __TEST_HBEAT_REQ:
					do_signal(child, __TEST_HBEAT_ACK);
				case __TEST_HBEAT_ACK:
					state++;
					continue;
				}
				goto failure;
			}
			break;
		}
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
postamble_aspdn(int child)
{
	int failed = 0;

	if (child == CHILD_PTU) {
		for (;;) {
			if (expect(child, INFINITE_WAIT, __TEST_ASPDN_REQ) != __RESULT_SUCCESS) {
				switch (last_event) {
				case __TEST_HBEAT_REQ:
					do_signal(child, __TEST_HBEAT_ACK);
				case __TEST_HBEAT_ACK:
					state++;
					continue;
				}
				failed = failed ? : state;
			} else if (do_signal(child, __TEST_ASPDN_ACK) != __RESULT_SUCCESS)
				failed = failed ? : state;
			break;
		}
	}
	if (child == CHILD_IUT) {
		if (do_signal(child, __TEST_ASPDN_REQ) != __RESULT_SUCCESS)
			failed = failed ? : state;
		else
			for (;;) {
				if (expect(child, INFINITE_WAIT, __TEST_ASPDN_ACK) != __RESULT_SUCCESS) {
					switch (last_event) {
					case __TEST_HBEAT_REQ:
						do_signal(child, __TEST_HBEAT_ACK);
					case __TEST_HBEAT_ACK:
					case __TEST_NTFY_MSG:
						state++;
						continue;
					}
					failed = failed ? : state;
				}
				break;
			}
	}
	state++;
	if (postamble_release(child) != __RESULT_SUCCESS)
		failed = failed ? : state;
	state++;
	if (failed)
		goto failure;
	return __RESULT_SUCCESS;
      failure:
	state = failed;
	return __RESULT_FAILURE;
}

int
preamble_reg(int child)
{
	if (preamble_aspup(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (child == CHILD_PTU) {
		for (;;) {
			if (expect(child, INFINITE_WAIT, __TEST_REG_REQ) != __RESULT_SUCCESS) {
				switch (last_event) {
				case __TEST_HBEAT_REQ:
					do_signal(child, __TEST_HBEAT_ACK);
				case __TEST_HBEAT_ACK:
					state++;
					continue;
				}
				goto failure;
			}
			break;
		}
		state++;
		if (do_signal(child, __TEST_REG_RSP) != __RESULT_SUCCESS)
			goto failure;
	}
	if (child == CHILD_IUT) {
		if (do_signal(child, __TEST_REG_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		for (;;) {
			if (expect(child, INFINITE_WAIT, __TEST_REG_RSP) != __RESULT_SUCCESS) {
				switch (last_event) {
				case __TEST_HBEAT_REQ:
					do_signal(child, __TEST_HBEAT_ACK);
				case __TEST_HBEAT_ACK:
				case __TEST_NTFY_MSG:
					state++;
					continue;
				}
				goto failure;
			}
			break;
		}
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
postamble_dereg(int child)
{
	int failed = 0;

	if (child == CHILD_PTU) {
		for (;;) {
			if (expect(child, INFINITE_WAIT, __TEST_DEREG_REQ) != __RESULT_SUCCESS) {
				switch (last_event) {
				case __TEST_HBEAT_REQ:
					do_signal(child, __TEST_HBEAT_ACK);
				case __TEST_HBEAT_ACK:
					state++;
					continue;
				}
				failed = failed ? : state;
			}
			break;
		}
		state++;
		if (do_signal(child, __TEST_DEREG_RSP) != __RESULT_SUCCESS)
			failed = failed ? : state;
	}
	if (child == CHILD_IUT) {
		if (do_signal(child, __TEST_DEREG_REQ) != __RESULT_SUCCESS) {
			failed = failed ? : state;
		} else
			for (;;) {
				if (expect(child, INFINITE_WAIT, __TEST_DEREG_RSP) != __RESULT_SUCCESS) {
					switch (last_event) {
					case __TEST_HBEAT_REQ:
						do_signal(child, __TEST_HBEAT_ACK);
					case __TEST_HBEAT_ACK:
					case __TEST_NTFY_MSG:
						state++;
						continue;
					}
					failed = failed ? : state;
				}
				break;
			}
	}
	state++;
	if (postamble_aspdn(child) != __RESULT_SUCCESS)
		failed = failed ? : state;
	state++;
	if (failed)
		goto failure;
	return __RESULT_SUCCESS;
      failure:
	state = failed;
	return __RESULT_FAILURE;
}

int
preamble_active(int child)
{
	if (preamble_reg(child) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (child == CHILD_PTU) {
		for (;;) {
			if (expect(child, INFINITE_WAIT, __TEST_ASPAC_REQ) != __RESULT_SUCCESS) {
				switch (last_event) {
				case __TEST_HBEAT_REQ:
					do_signal(child, __TEST_HBEAT_ACK);
					state++;
					continue;
				case __TEST_HBEAT_ACK:
					state++;
					continue;
				}
				goto failure;
			}
			break;
		}
		state++;
		if (do_signal(child, __TEST_ASPAC_ACK) != __RESULT_SUCCESS)
			goto failure;
	}
	if (child == CHILD_IUT) {
		if (do_signal(child, __TEST_ASPAC_REQ) != __RESULT_SUCCESS)
			goto failure;
		state++;
		for (;;) {
			if (expect(child, INFINITE_WAIT, __TEST_ASPAC_ACK) != __RESULT_SUCCESS) {
				switch (last_event) {
				case __TEST_HBEAT_REQ:
					do_signal(child, __TEST_HBEAT_ACK);
				case __TEST_HBEAT_ACK:
				case __TEST_NTFY_MSG:
				case __TEST_DAVA_MSG:
				case __TEST_DUNA_MSG:
				case __TEST_DRST_MSG:
				case __TEST_SCON_MSG:
				case __TEST_DUPU_MSG:
				case __TEST_XFER_DATA:
					state++;
					continue;
				}
				goto failure;
			}
			break;
		}
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
postamble_inactive(int child)
{
	int failed = 0;

	if (child == CHILD_PTU) {
		for (;;) {
			if (expect(child, INFINITE_WAIT, __TEST_ASPIA_REQ) != __RESULT_SUCCESS) {
				switch (last_event) {
				case __TEST_HBEAT_REQ:
					do_signal(child, __TEST_HBEAT_ACK);
				case __TEST_HBEAT_ACK:
				case __TEST_NTFY_MSG:
				case __TEST_DAVA_MSG:
				case __TEST_DUNA_MSG:
				case __TEST_DRST_MSG:
				case __TEST_SCON_MSG:
				case __TEST_DUPU_MSG:
				case __TEST_XFER_DATA:
					state++;
					continue;
				}
				failed = failed ? : state;
			} else if (do_signal(child, __TEST_ASPIA_ACK) != __RESULT_SUCCESS)
				failed = failed ? : state;
			break;
		}
	}
	if (child == CHILD_IUT) {
		if (do_signal(child, __TEST_ASPIA_REQ) != __RESULT_SUCCESS)
			failed = failed ? : state;
		else
			for (;;) {
				if (expect(child, INFINITE_WAIT, __TEST_ASPIA_ACK) != __RESULT_SUCCESS) {
					switch (last_event) {
					case __TEST_HBEAT_REQ:
						do_signal(child, __TEST_HBEAT_ACK);
					case __TEST_HBEAT_ACK:
					case __TEST_NTFY_MSG:
					case __TEST_DAVA_MSG:
					case __TEST_DUNA_MSG:
					case __TEST_DRST_MSG:
					case __TEST_SCON_MSG:
					case __TEST_DUPU_MSG:
					case __TEST_XFER_DATA:
						state++;
						continue;
					}
					failed = failed ? : state;
				}
				break;
			}
	}
	state++;
	if (postamble_dereg(child) != __RESULT_SUCCESS)
		failed = failed ? : state;
	state++;
	if (failed)
		goto failure;
	return __RESULT_SUCCESS;
      failure:
	state = failed;
	return __RESULT_FAILURE;
}


/*
 *  =========================================================================
 *
 *  The Test Cases...
 *
 *  =========================================================================
 */

struct test_stream {
	int (*preamble) (int);		/* test preamble */
	int (*testcase) (int);		/* test case */
	int (*postamble) (int);		/* test postamble */
};

/*
 *  Check test case guard timer.
 */
#define test_group_0 "Sanity checks"

#define test_group_0_1 "0.1. Guard timers"

#define tgrp_case_0_1 test_group_0
#define sgrp_case_0_1 test_group_0_1
#define numb_case_0_1 "0.1"
#define name_case_0_1 "Check test case guard timer."
#define xtra_case_0_1 NULL
#define sref_case_0_1 "(none)"
#define desc_case_0_1 "\
Checks that the test case guard timer will fire and bring down the children."

int
test_0_1_ptu(int child)
{
	if (test_duration == INFINITE_WAIT)
		return __RESULT_NOTAPPL;
	test_msleep(child, test_duration + 1000);
	state++;
	return (__RESULT_SUCCESS);
}

int
test_0_1_iut(int child)
{
	if (test_duration == INFINITE_WAIT)
		return __RESULT_NOTAPPL;
	test_msleep(child, test_duration + 1000);
	state++;
	return (__RESULT_SUCCESS);
}

struct test_stream test_case_0_1_ptu = { preamble_none, &test_0_1_ptu, postamble_none };
struct test_stream test_case_0_1_iut = { preamble_none, &test_0_1_iut, postamble_none };

#define test_case_0_1 { &test_case_0_1_ptu, &test_case_0_1_iut, NULL }

#define test_group_0_2 "0.2. Preambles and Postambles"

#define tgrp_case_0_2_1 test_group_0
#define sgrp_case_0_2_1 test_group_0_2
#define numb_case_0_2_1 "0.2.1"
#define name_case_0_2_1 "Base preamble, base postamble."
#define xtra_case_0_2_1 NULL
#define sref_case_0_2_1 "(none)"
#define desc_case_0_2_1 "\
Checks that Streams can be opened and closed."

int
test_0_2_1_ptu(int child)
{
	return __RESULT_SUCCESS;
}

int
test_0_2_1_iut(int child)
{
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_1_ptu = { preamble_base, test_0_2_1_ptu, postamble_base };
struct test_stream test_case_0_2_1_iut = { preamble_base, test_0_2_1_iut, postamble_base };

#define test_case_0_2_1 { &test_case_0_2_1_ptu, &test_case_0_2_1_iut, NULL }

#define tgrp_case_0_2_2 test_group_0
#define sgrp_case_0_2_2 test_group_0_2
#define numb_case_0_2_2 "0.2.2"
#define name_case_0_2_2 "Info preamble, info postamble."
#define xtra_case_0_2_2 NULL
#define sref_case_0_2_2 "(none)"
#define desc_case_0_2_2 "\
Checks that Streams can be opened, information obtained, and closed."

int
test_0_2_2_ptu(int child)
{
	return __RESULT_SUCCESS;
}

int
test_0_2_2_iut(int child)
{
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_2_ptu = { preamble_info, test_0_2_2_ptu, postamble_info };
struct test_stream test_case_0_2_2_iut = { preamble_info, test_0_2_2_iut, postamble_info };

#define test_case_0_2_2 { &test_case_0_2_2_ptu, &test_case_0_2_2_iut, NULL }

#define tgrp_case_0_2_3 test_group_0
#define sgrp_case_0_2_3 test_group_0_2
#define numb_case_0_2_3 "0.2.3"
#define name_case_0_2_3 "Unbound preamble, unbound postamble."
#define xtra_case_0_2_3 NULL
#define sref_case_0_2_3 "(none)"
#define desc_case_0_2_3 "\
Checks that Streams can be opened, information obtained in the unbound state, and closed."

int
test_0_2_3_ptu(int child)
{
	return __RESULT_SUCCESS;
}

int
test_0_2_3_iut(int child)
{
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_3_ptu = { preamble_unbound, test_0_2_3_ptu, postamble_unbound };
struct test_stream test_case_0_2_3_iut = { preamble_unbound, test_0_2_3_iut, postamble_unbound };

#define test_case_0_2_3 { &test_case_0_2_3_ptu, &test_case_0_2_3_iut, NULL }

#define tgrp_case_0_2_4 test_group_0
#define sgrp_case_0_2_4 test_group_0_2
#define numb_case_0_2_4 "0.2.4"
#define name_case_0_2_4 "Options preamble, options postamble."
#define xtra_case_0_2_4 NULL
#define sref_case_0_2_4 "(none)"
#define desc_case_0_2_4 "\
Checks that Streams can be opened, information obtained in the unbound state, options applied,\n\
and closed."

int
test_0_2_4_ptu(int child)
{
	return __RESULT_SUCCESS;
}

int
test_0_2_4_iut(int child)
{
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_4_ptu = { preamble_options, test_0_2_4_ptu, postamble_options };
struct test_stream test_case_0_2_4_iut = { preamble_options, test_0_2_4_iut, postamble_options };

#define test_case_0_2_4 { &test_case_0_2_4_ptu, &test_case_0_2_4_iut, NULL }

#define tgrp_case_0_2_5 test_group_0
#define sgrp_case_0_2_5 test_group_0_2
#define numb_case_0_2_5 "0.2.5"
#define name_case_0_2_5 "Bind preamble, unbind postamble."
#define xtra_case_0_2_5 NULL
#define sref_case_0_2_5 "(none)"
#define desc_case_0_2_5 "\
Checks that Streams can be opened, information obtained in the unbound state, options applied,\n\
streams bound, unbound, and closed."

int
test_0_2_5_ptu(int child)
{
	return __RESULT_SUCCESS;
}

int
test_0_2_5_iut(int child)
{
	return __RESULT_SUCCESS;
}

struct test_stream test_case_0_2_5_ptu = { preamble_bind, test_0_2_5_ptu, postamble_unbind };
struct test_stream test_case_0_2_5_iut = { preamble_bind, test_0_2_5_iut, postamble_unbind };

#define test_case_0_2_5 { &test_case_0_2_5_ptu, &test_case_0_2_5_iut, NULL }

#define test_group_1 "SCTP connections"

#define test_group_1_1 "1.1. Connection preambles and postambles."

#define tgrp_case_1_1_1 test_group_1
#define sgrp_case_1_1_1 test_group_1_1
#define numb_case_1_1_1 "1.1.1"
#define name_case_1_1_1 "Connect preamble, disconnect postamble."
#define xtra_case_1_1_1 NULL
#define sref_case_1_1_1 "RFC 4960"
#define desc_case_1_1_1 "\
Checks that streams can be opened, information obtained in the unbound state, options applied,\n\
connected, disconnected, unbound and closed."

int
test_1_1_1_ptu(int child)
{
	return __RESULT_SUCCESS;
}
int
test_1_1_1_iut(int child)
{
	return __RESULT_SUCCESS;
}

struct test_stream test_case_1_1_1_ptu = { preamble_connect, test_1_1_1_ptu, postamble_disconnect };
struct test_stream test_case_1_1_1_iut = { preamble_connect, test_1_1_1_iut, postamble_disconnect };

#define test_case_1_1_1 { &test_case_1_1_1_ptu, &test_case_1_1_1_iut, NULL }

#define tgrp_case_1_1_2 test_group_1
#define sgrp_case_1_1_2 test_group_1_1
#define numb_case_1_1_2 "1.1.2"
#define name_case_1_1_2 "Connect preamble, release postamble."
#define xtra_case_1_1_2 NULL
#define sref_case_1_1_2 "RFC 4960"
#define desc_case_1_1_2 "\
Checks that streams can be opened, information obtained in the unbound state, options applied,\n\
connected, released, unbound and closed."

int
test_1_1_2_ptu(int child)
{
	return __RESULT_SUCCESS;
}
int
test_1_1_2_iut(int child)
{
	return __RESULT_SUCCESS;
}

struct test_stream test_case_1_1_2_ptu = { preamble_connect, test_1_1_2_ptu, postamble_release };
struct test_stream test_case_1_1_2_iut = { preamble_connect, test_1_1_2_iut, postamble_release };

#define test_case_1_1_2 { &test_case_1_1_2_ptu, &test_case_1_1_2_iut, NULL }

#define test_group_2 "M3UA ASP Up and Down procedures."

#define test_group_2_1 "2.1 ASP Up procedure."

#define tgrp_case_2_1_1 test_group_2
#define sgrp_case_2_1_1 test_group_2_1
#define numb_case_2_1_1 "2.1.1"
#define name_case_2_1_1 "ASP Up message."
#define xtra_case_2_1_1 NULL
#define sref_case_2_1_1 "RFC 4666"
#define desc_case_2_1_1 "\
Checks that an SCTP connection can be made and the ASP places into the\n\
ASP Up state."

int
test_2_1_1_ptu(int child)
{
	if (expect(child, INFINITE_WAIT, __TEST_ASPUP_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_2_1_1_iut(int child)
{
	if (do_signal(child, __TEST_ASPUP_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_msleep(child, LONGEST_WAIT);
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_2_1_1_ptu = { preamble_connect, test_2_1_1_ptu, postamble_release };
struct test_stream test_case_2_1_1_iut = { preamble_connect, test_2_1_1_iut, postamble_release };

#define test_case_2_1_1 { &test_case_2_1_1_ptu, &test_case_2_1_1_iut, NULL }

#define tgrp_case_2_1_2 test_group_2
#define sgrp_case_2_1_2 test_group_2_1
#define numb_case_2_1_2 "2.1.2"
#define name_case_2_1_2 "ASP Up Ack message."
#define xtra_case_2_1_2 NULL
#define sref_case_2_1_2 "RFC 4666"
#define desc_case_2_1_2 "\
Checks that an SCTP connection can be made and the ASP places into the\n\
ASP Up state."

int
test_2_1_2_ptu(int child)
{
	if (expect(child, INFINITE_WAIT, __TEST_ASPUP_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_ASPUP_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_2_1_2_iut(int child)
{
	if (do_signal(child, __TEST_ASPUP_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __TEST_ASPUP_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_2_1_2_ptu = { preamble_connect, test_2_1_2_ptu, postamble_release };
struct test_stream test_case_2_1_2_iut = { preamble_connect, test_2_1_2_iut, postamble_release };

#define test_case_2_1_2 { &test_case_2_1_2_ptu, &test_case_2_1_2_iut, NULL }

#define test_group_2_2 "2.2 ASP Down Procedures."

#define tgrp_case_2_2_1 test_group_2
#define sgrp_case_2_2_1 test_group_2_2
#define numb_case_2_2_1 "2.2.1"
#define name_case_2_2_1 "ASP Down message."
#define xtra_case_2_2_1 NULL
#define sref_case_2_2_1 "RFC 4666"
#define desc_case_2_2_1 "\
Checks that an SCTP connection can be made and the ASP places into the\n\
ASP Up state and then the ASP Down state."

int
test_2_2_1_ptu(int child)
{
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_ASPDN_REQ) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_2_2_1_iut(int child)
{
	if (do_signal(child, __TEST_ASPDN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_2_2_1_ptu = { preamble_aspup, test_2_2_1_ptu, postamble_release };
struct test_stream test_case_2_2_1_iut = { preamble_aspup, test_2_2_1_iut, postamble_release };

#define test_case_2_2_1 { &test_case_2_2_1_ptu, &test_case_2_2_1_iut, NULL }

#define tgrp_case_2_2_2 test_group_2
#define sgrp_case_2_2_2 test_group_2_2
#define numb_case_2_2_2 "2.2.2"
#define name_case_2_2_2 "ASP Down Ack message."
#define xtra_case_2_2_2 NULL
#define sref_case_2_2_2 "RFC 4666"
#define desc_case_2_2_2 "\
Checks that an SCTP connection can be made and the ASP places into the\n\
ASP Up state and then the ASP Down state."

int
test_2_2_2_ptu(int child)
{
	if (expect(child, INFINITE_WAIT, __TEST_ASPDN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_ASPDN_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_2_2_2_iut(int child)
{
	if (do_signal(child, __TEST_ASPDN_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (expect(child, INFINITE_WAIT, __TEST_ASPDN_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_2_2_2_ptu = { preamble_aspup, test_2_2_2_ptu, postamble_release };
struct test_stream test_case_2_2_2_iut = { preamble_aspup, test_2_2_2_iut, postamble_release };

#define test_case_2_2_2 { &test_case_2_2_2_ptu, &test_case_2_2_2_iut, NULL }

#define test_group_2_3 "2.3 Heartbeat Procedrues."

#define tgrp_case_2_3_1 test_group_2
#define sgrp_case_2_3_1 test_group_2_3
#define numb_case_2_3_1 "2.3.1"
#define name_case_2_3_1 "BEAT message from IUT."
#define xtra_case_2_3_1 NULL
#define sref_case_2_3_1 "RFC 4666"
#define desc_case_2_3_1 "\
Checks that heartbeat messages can be exchanged once the has issued the\n\
ASP Up message."

int
test_2_3_1_ptu(int child)
{
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_HBEAT_REQ) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_ACK:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	if (do_signal(child, __TEST_HBEAT_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_2_3_1_iut(int child)
{
	if (do_signal(child, __TEST_HBEAT_REQ) != __RESULT_SUCCESS)
		goto failure;
	for (;;) { 
		if (expect(child, INFINITE_WAIT, __TEST_HBEAT_ACK) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_NTFY_MSG:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_2_3_1_ptu = { preamble_aspup, test_2_3_1_ptu, postamble_aspdn };
struct test_stream test_case_2_3_1_iut = { preamble_aspup, test_2_3_1_iut, postamble_aspdn };

#define test_case_2_3_1 { &test_case_2_3_1_ptu, &test_case_2_3_1_iut, NULL }

#define tgrp_case_2_3_2 test_group_2
#define sgrp_case_2_3_2 test_group_2_3
#define numb_case_2_3_2 "2.3.2"
#define name_case_2_3_2 "BEAT message from PTU."
#define xtra_case_2_3_2 NULL
#define sref_case_2_3_2 "RFC 4666"
#define desc_case_2_3_2 "\
Checks that heartbeat messages can be exchanged once the has issued the\n\
ASP Up message."

#define test_case_2_3_2 { &test_case_2_3_1_iut, &test_case_2_3_1_ptu, NULL }

#define tgrp_case_2_3_3 test_group_2
#define sgrp_case_2_3_3 test_group_2_3
#define numb_case_2_3_3 "2.3.3"
#define name_case_2_3_3 "BEAT message exchange."
#define xtra_case_2_3_3 NULL
#define sref_case_2_3_3 "RFC 4666"
#define desc_case_2_3_3 "\
Checks that heartbeat messages can be exchanged once the has issued the\n\
ASP Up message."

int
test_2_3_3_ptu(int child)
{
	if (do_signal(child, __TEST_HBEAT_REQ) != __RESULT_SUCCESS)
		goto failure;
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_HBEAT_ACK) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_2_3_3_iut(int child)
{
	if (do_signal(child, __TEST_HBEAT_REQ) != __RESULT_SUCCESS)
		goto failure;
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_HBEAT_ACK) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
			case __TEST_NTFY_MSG:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_2_3_3_ptu = { preamble_aspup, test_2_3_3_ptu, postamble_aspdn };
struct test_stream test_case_2_3_3_iut = { preamble_aspup, test_2_3_3_iut, postamble_aspdn };

#define test_case_2_3_3 { &test_case_2_3_3_ptu, &test_case_2_3_3_iut, NULL }

#define test_group_3 "M3UA Registration and Deregistration procedures."

#define test_group_3_1 "3.1 Registration procedure."

#define tgrp_case_3_1_1 test_group_3
#define sgrp_case_3_1_1 test_group_3_1
#define numb_case_3_1_1 "3.1.1"
#define name_case_3_1_1 "Registration Request message"
#define xtra_case_3_1_1 NULL
#define sref_case_3_1_1 "RFC 4666"
#define desc_case_3_1_1 "\
Checks that a routing key can be registered from the ASP Up state."

int
test_3_1_1_ptu(child)
{
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_REG_REQ) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
			case __TEST_ERR_MSG:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_3_1_1_iut(child)
{
	if (do_signal(child, __TEST_REG_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_msleep(child, LONG_WAIT);
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_3_1_1_ptu = { preamble_aspup, test_3_1_1_ptu, postamble_aspdn };
struct test_stream test_case_3_1_1_iut = { preamble_aspup, test_3_1_1_iut, postamble_aspdn };

#define test_case_3_1_1 { &test_case_3_1_1_ptu, &test_case_3_1_1_iut, NULL }

#define tgrp_case_3_1_2 test_group_3
#define sgrp_case_3_1_2 test_group_3_1
#define numb_case_3_1_2 "3.1.2"
#define name_case_3_1_2 "Registration Response message"
#define xtra_case_3_1_2 NULL
#define sref_case_3_1_2 "RFC 4666"
#define desc_case_3_1_2 "\
Checks that a routing key can be registered from the ASP Up state."

int
test_3_1_2_ptu(child)
{
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_REG_REQ) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
			case __TEST_NTFY_MSG:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	m3ua.reg_status.val = 0;
	if (do_signal(child, __TEST_REG_RSP) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_3_1_2_iut(child)
{
	if (do_signal(child, __TEST_REG_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_REG_RSP) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
			case __TEST_NTFY_MSG:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	if (m3ua.reg_status.len < sizeof(uint32_t))
		goto failure;
	state++;
	if (m3ua.reg_status.val != 0)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_3_1_2_ptu = { preamble_aspup, test_3_1_2_ptu, postamble_aspdn };
struct test_stream test_case_3_1_2_iut = { preamble_aspup, test_3_1_2_iut, postamble_aspdn };

#define test_case_3_1_2 { &test_case_3_1_2_ptu, &test_case_3_1_2_iut, NULL }


#define test_group_3_2 "3.2 Deregistration procedure."

#define tgrp_case_3_2_1 test_group_3
#define sgrp_case_3_2_1 test_group_3_2
#define numb_case_3_2_1 "3.2.1"
#define name_case_3_2_1 "Deregistration Request message"
#define xtra_case_3_2_1 NULL
#define sref_case_3_2_1 "RFC 4666"
#define desc_case_3_2_1 "\
Checks that a routing key can be deregistered from the ASP Up state."

int
test_3_2_1_ptu(child)
{
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_DEREG_REQ) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
			case __TEST_NTFY_MSG:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_3_2_1_iut(child)
{
	if (do_signal(child, __TEST_DEREG_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_msleep(child, LONG_WAIT);
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_3_2_1_ptu = { preamble_reg, test_3_2_1_ptu, postamble_aspdn };
struct test_stream test_case_3_2_1_iut = { preamble_reg, test_3_2_1_iut, postamble_aspdn };

#define test_case_3_2_1 { &test_case_3_2_1_ptu, &test_case_3_2_1_iut, NULL }

#define tgrp_case_3_2_2 test_group_3
#define sgrp_case_3_2_2 test_group_3_2
#define numb_case_3_2_2 "3.2.2"
#define name_case_3_2_2 "Deregistration Response message"
#define xtra_case_3_2_2 NULL
#define sref_case_3_2_2 "RFC 4666"
#define desc_case_3_2_2 "\
Checks that a routing key can be deregistered from the ASP Up state."

int
test_3_2_2_ptu(child)
{
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_DEREG_REQ) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	m3ua.dereg_status.val = 0;
	if (do_signal(child, __TEST_DEREG_RSP) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_3_2_2_iut(child)
{
	if (do_signal(child, __TEST_DEREG_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	m3ua.dereg_status.len = 0;
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_DEREG_RSP) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
			case __TEST_NTFY_MSG:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	if (m3ua.dereg_status.len < sizeof(uint32_t))
		goto failure;
	state++;
	if (m3ua.dereg_status.val != 0)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_3_2_2_ptu = { preamble_reg, test_3_2_2_ptu, postamble_aspdn };
struct test_stream test_case_3_2_2_iut = { preamble_reg, test_3_2_2_iut, postamble_aspdn };

#define test_case_3_2_2 { &test_case_3_2_2_ptu, &test_case_3_2_2_iut, NULL }


#define test_group_4 "M3UA ASP Active and Inactive procedures."

#define test_group_4_1 "4.1 ASP Active procedure."

#define tgrp_case_4_1_1 test_group_4
#define sgrp_case_4_1_1 test_group_4_1
#define numb_case_4_1_1 "4.1.1"
#define name_case_4_1_1 "ASP Active message."
#define xtra_case_4_1_1 NULL
#define sref_case_4_1_1 "RFC 4666"
#define desc_case_4_1_1 "\
Checks that an ASP can be activated from the inactive state."

int
test_4_1_1_ptu(child)
{
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_ASPAC_REQ) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_4_1_1_iut(child)
{
	if (do_signal(child, __TEST_ASPAC_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_msleep(child, LONG_WAIT);
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_4_1_1_ptu = { preamble_reg, test_4_1_1_ptu, postamble_dereg };
struct test_stream test_case_4_1_1_iut = { preamble_reg, test_4_1_1_iut, postamble_dereg };

#define test_case_4_1_1 { &test_case_4_1_1_ptu, &test_case_4_1_1_iut, NULL }

#define tgrp_case_4_1_2 test_group_4
#define sgrp_case_4_1_2 test_group_4_1
#define numb_case_4_1_2 "4.1.2"
#define name_case_4_1_2 "ASP Active Ack message."
#define xtra_case_4_1_2 NULL
#define sref_case_4_1_2 "RFC 4666"
#define desc_case_4_1_2 "\
Checks that an ASP can be activated from the inactive state."

int
test_4_1_2_ptu(child)
{
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_ASPAC_REQ) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	if (do_signal(child, __TEST_ASPAC_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_4_1_2_iut(child)
{
	if (do_signal(child, __TEST_ASPAC_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_ASPAC_ACK) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
			case __TEST_NTFY_MSG:
				/* it is permitted for the SG to send SNMM between ASPAC and ASPAC Ack, and DATA might
				   overtake the ASPAC Ack. */
			case __TEST_DUNA_MSG:
			case __TEST_DAVA_MSG:
			case __TEST_DUPU_MSG:
			case __TEST_SCON_MSG:
			case __TEST_DRST_MSG:
			case __TEST_XFER_DATA:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	test_msleep(child, LONG_WAIT);	/* hold it in this state for a moment */
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_4_1_2_ptu = { preamble_reg, test_4_1_2_ptu, postamble_dereg };
struct test_stream test_case_4_1_2_iut = { preamble_reg, test_4_1_2_iut, postamble_dereg };

#define test_case_4_1_2 { &test_case_4_1_2_ptu, &test_case_4_1_2_iut, NULL }

#define test_group_4_2 "4.2 ASP Inactive procedure."

#define tgrp_case_4_2_1 test_group_4
#define sgrp_case_4_2_1 test_group_4_2
#define numb_case_4_2_1 "4.2.1"
#define name_case_4_2_1 "ASP Inactive message."
#define xtra_case_4_2_1 NULL
#define sref_case_4_2_1 "RFC 4666"
#define desc_case_4_2_1 "\
Checks that an ASP can be deactivated from the active state."

int
test_4_2_1_ptu(child)
{
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_ASPIA_REQ) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
			case __TEST_DAUD_MSG:
			case __TEST_SCON_MSG:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_4_2_1_iut(child)
{
	if (do_signal(child, __TEST_ASPIA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	test_msleep(child, LONG_WAIT);
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_4_2_1_ptu = { preamble_active, test_4_2_1_ptu, postamble_dereg };
struct test_stream test_case_4_2_1_iut = { preamble_active, test_4_2_1_iut, postamble_dereg };

#define test_case_4_2_1 { &test_case_4_2_1_ptu, &test_case_4_2_1_iut, NULL }

#define tgrp_case_4_2_2 test_group_4
#define sgrp_case_4_2_2 test_group_4_2
#define numb_case_4_2_2 "4.2.2"
#define name_case_4_2_2 "ASP Inactive message."
#define xtra_case_4_2_2 NULL
#define sref_case_4_2_2 "RFC 4666"
#define desc_case_4_2_2 "\
Checks that an ASP can be deactivated from the active state."

int
test_4_2_2_ptu(child)
{
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_ASPIA_REQ) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
			case __TEST_DAUD_MSG:
			case __TEST_SCON_MSG:
			case __TEST_XFER_DATA:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	if (do_signal(child, __TEST_ASPIA_ACK) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

int
test_4_2_2_iut(child)
{
	if (do_signal(child, __TEST_ASPIA_REQ) != __RESULT_SUCCESS)
		goto failure;
	state++;
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_ASPIA_ACK) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
			case __TEST_NTFY_MSG:
				/* it is permitted for the SG to send SNMM between ASPAC and ASPAC Ack, and DATA might
				   overtake the ASPAC Ack. */
			case __TEST_DUNA_MSG:
			case __TEST_DAVA_MSG:
			case __TEST_SCON_MSG:
			case __TEST_DUPU_MSG:
			case __TEST_DRST_MSG:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_4_2_2_ptu = { preamble_active, test_4_2_2_ptu, postamble_dereg };
struct test_stream test_case_4_2_2_iut = { preamble_active, test_4_2_2_iut, postamble_dereg };

#define test_case_4_2_2 { &test_case_4_2_2_ptu, &test_case_4_2_2_iut, NULL }


#define test_group_5 "M3UA SNMM procedures."

#define test_group_5_1 "5.1 DAUD procedure."

#define tgrp_case_5_1_1 test_group_5
#define sgrp_case_5_1_1 test_group_5_1
#define numb_case_5_1_1 "5.1.1"
#define name_case_5_1_1 "DAUD message."
#define xtra_case_5_1_1 NULL
#define sref_case_5_1_1 "RFC 4666"
#define desc_case_5_1_1 "\
Check that ASP can audit the AS from the active state."

int
test_5_1_1_ptu(int child)
{
	for (;;) {
		if (expect(child, INFINITE_WAIT, __TEST_DAUD_MSG)) {
			switch (last_event) {
			case __TEST_HBEAT_REQ:
				do_signal(child, __TEST_HBEAT_ACK);
			case __TEST_HBEAT_ACK:
			case __TEST_XFER_DATA:
				state++;
				continue;
			}
			goto failure;
		}
		break;
	}
	state++;
	if (do_signal(child, __TEST_SCON_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_DAVA_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_DUNA_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	if (do_signal(child, __TEST_DRST_MSG) != __RESULT_SUCCESS)
		goto failure;
	state++;
	return __RESULT_SUCCESS;
failure:
	return __RESULT_FAILURE;
}

int
test_5_1_1_iut(int child)
{
	m3ua.affect_dest.val = 0xffffffff;
	if (do_signal(child, __TEST_DAUD_MSG) != __RESULT_SUCCESS)
		goto failure;
	for (;;) {
		if (expect(child, LONGEST_WAIT, __EVENT_TIMEOUT) != __RESULT_SUCCESS) {
			switch (last_event) {
			case __TEST_SCON_MSG:
			case __TEST_DAVA_MSG:
			case __TEST_DUNA_MSG:
			case __TEST_DRST_MSG:
			case __TEST_XFER_DATA:
			case __TEST_NTFY_MSG:
				state++;
				continue;
			case __EVENT_NO_MSG:
				break;
			default:
				goto failure;
			}
		}
		break;
	}
	state++;
	return __RESULT_SUCCESS;
      failure:
	return __RESULT_FAILURE;
}

struct test_stream test_case_5_1_1_ptu = { preamble_active, test_5_1_1_ptu, postamble_inactive };
struct test_stream test_case_5_1_1_iut = { preamble_active, test_5_1_1_iut, postamble_inactive };

#define test_case_5_1_1 { &test_case_5_1_1_ptu, &test_case_5_1_1_iut, NULL }


/*
 *  -------------------------------------------------------------------------
 *
 *  Test case child scheduler
 *
 *  -------------------------------------------------------------------------
 */
int
run_stream(int child, struct test_stream *stream)
{
	int result = __RESULT_SCRIPT_ERROR;
	int pre_result = __RESULT_SCRIPT_ERROR;
	int post_result = __RESULT_SCRIPT_ERROR;

	oldact = 0;		/* previous action for this child */
	cntact = 0;		/* count of this action for this child */
	oldevt = 0;		/* previous return for this child */
	cntevt = 0;		/* count of this return for this child */

	print_preamble(child);
	state = 100;
	failure_string = NULL;
	if (stream->preamble && (pre_result = stream->preamble(child)) != __RESULT_SUCCESS) {
		switch (pre_result) {
		case __RESULT_NOTAPPL:
			print_notapplicable(child);
			result = __RESULT_NOTAPPL;
			break;
		case __RESULT_SKIPPED:
			print_skipped(child);
			result = __RESULT_SKIPPED;
			break;
		default:
			print_inconclusive(child);
			result = __RESULT_INCONCLUSIVE;
			break;
		}
	} else {
		print_test(child);
		state = 200;
		failure_string = NULL;
		switch (stream->testcase(child)) {
		default:
		case __RESULT_INCONCLUSIVE:
			print_inconclusive(child);
			result = __RESULT_INCONCLUSIVE;
			break;
		case __RESULT_NOTAPPL:
			print_notapplicable(child);
			result = __RESULT_NOTAPPL;
			break;
		case __RESULT_SKIPPED:
			print_skipped(child);
			result = __RESULT_SKIPPED;
			break;
		case __RESULT_FAILURE:
			print_failed(child);
			result = __RESULT_FAILURE;
			break;
		case __RESULT_SCRIPT_ERROR:
			print_script_error(child);
			result = __RESULT_SCRIPT_ERROR;
			break;
		case __RESULT_SUCCESS:
			print_passed(child);
			result = __RESULT_SUCCESS;
			break;
		}
		print_postamble(child);
		state = 300;
		failure_string = NULL;
		if (stream->postamble && (post_result = stream->postamble(child)) != __RESULT_SUCCESS) {
			switch (post_result) {
			case __RESULT_NOTAPPL:
				print_notapplicable(child);
				result = __RESULT_NOTAPPL;
				break;
			case __RESULT_SKIPPED:
				print_skipped(child);
				result = __RESULT_SKIPPED;
				break;
			default:
				print_inconclusive(child);
				if (result == __RESULT_SUCCESS)
					result = __RESULT_INCONCLUSIVE;
				break;
			}
		}
	}
	print_test_end(child);
	exit(result);
}

/*
 *  Fork multiple children to do the actual testing.
 *  The conn child (child[0]) is the connecting process, the resp child
 *  (child[1]) is the responding process.
 */

int
test_run(struct test_stream *stream[], ulong duration)
{
	int children = 0;
	pid_t this_child, child[3] = { 0, };
	int this_status, status[3] = { 0, };

	if (start_tt(duration) != __RESULT_SUCCESS)
		goto inconclusive;
	if (server_exec && stream[2]) {
		switch ((child[2] = fork())) {
		case 00:	/* we are the child */
			exit(run_stream(2, stream[2]));	/* execute stream[2] state machine */
		case -1:	/* error */
			if (child[0])
				kill(child[0], SIGKILL);	/* toast stream[0] child */
			if (child[1])
				kill(child[1], SIGKILL);	/* toast stream[1] child */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			break;
		}
	} else
		status[2] = __RESULT_SUCCESS;
	if (server_exec && stream[1]) {
		switch ((child[1] = fork())) {
		case 00:	/* we are the child */
			exit(run_stream(1, stream[1]));	/* execute stream[1] state machine */
		case -1:	/* error */
			if (child[0])
				kill(child[0], SIGKILL);	/* toast stream[0] child */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			break;
		}
	} else
		status[1] = __RESULT_SUCCESS;
	if (client_exec && stream[0]) {
		switch ((child[0] = fork())) {
		case 00:	/* we are the child */
			exit(run_stream(0, stream[0]));	/* execute stream[0] state machine */
		case -1:	/* error */
			return __RESULT_FAILURE;
		default:	/* we are the parent */
			children++;
			break;
		}
	} else
		status[0] = __RESULT_SUCCESS;
	for (; children > 0; children--) {
	      waitagain:
		if ((this_child = wait(&this_status)) > 0) {
			if (WIFEXITED(this_status)) {
				if (this_child == child[0]) {
					child[0] = 0;
					if ((status[0] = WEXITSTATUS(this_status)) != __RESULT_SUCCESS) {
						if (child[1])
							kill(child[1], SIGKILL);
						if (child[2])
							kill(child[2], SIGKILL);
					}
				}
				if (this_child == child[1]) {
					child[1] = 0;
					if ((status[1] = WEXITSTATUS(this_status)) != __RESULT_SUCCESS) {
						if (child[0])
							kill(child[0], SIGKILL);
						if (child[2])
							kill(child[2], SIGKILL);
					}
				}
				if (this_child == child[2]) {
					child[2] = 0;
					if ((status[2] = WEXITSTATUS(this_status)) != __RESULT_SUCCESS) {
						if (child[0])
							kill(child[0], SIGKILL);
						if (child[1])
							kill(child[1], SIGKILL);
					}
				}
			} else if (WIFSIGNALED(this_status)) {
				int signal = WTERMSIG(this_status);

				if (this_child == child[0]) {
					print_terminated(0, signal);
					if (child[1])
						kill(child[1], SIGKILL);
					if (child[2])
						kill(child[2], SIGKILL);
					status[0] = (signal == SIGKILL) ? __RESULT_INCONCLUSIVE : __RESULT_FAILURE;
					child[0] = 0;
				}
				if (this_child == child[1]) {
					print_terminated(1, signal);
					if (child[0])
						kill(child[0], SIGKILL);
					if (child[2])
						kill(child[2], SIGKILL);
					status[1] = (signal == SIGKILL) ? __RESULT_INCONCLUSIVE : __RESULT_FAILURE;
					child[1] = 0;
				}
				if (this_child == child[2]) {
					print_terminated(2, signal);
					if (child[0])
						kill(child[0], SIGKILL);
					if (child[1])
						kill(child[1], SIGKILL);
					status[2] = (signal == SIGKILL) ? __RESULT_INCONCLUSIVE : __RESULT_FAILURE;
					child[2] = 0;
				}
			} else if (WIFSTOPPED(this_status)) {
				int signal = WSTOPSIG(this_status);

				if (this_child == child[0]) {
					print_stopped(0, signal);
					if (child[0])
						kill(child[0], SIGKILL);
					if (child[1])
						kill(child[1], SIGKILL);
					if (child[2])
						kill(child[2], SIGKILL);
					status[0] = __RESULT_FAILURE;
					child[0] = 0;
				}
				if (this_child == child[1]) {
					print_stopped(1, signal);
					if (child[0])
						kill(child[0], SIGKILL);
					if (child[1])
						kill(child[1], SIGKILL);
					if (child[2])
						kill(child[2], SIGKILL);
					status[1] = __RESULT_FAILURE;
					child[1] = 0;
				}
				if (this_child == child[2]) {
					print_stopped(2, signal);
					if (child[0])
						kill(child[0], SIGKILL);
					if (child[1])
						kill(child[1], SIGKILL);
					if (child[2])
						kill(child[2], SIGKILL);
					status[2] = __RESULT_FAILURE;
					child[2] = 0;
				}
			}
		} else {
			if (timer_timeout) {
				timer_timeout = 0;
				print_timeout(3);
			}
			if (child[0])
				kill(child[0], SIGKILL);
			if (child[1])
				kill(child[1], SIGKILL);
			if (child[2])
				kill(child[2], SIGKILL);
			goto waitagain;
		}
	}
	if (stop_tt() != __RESULT_SUCCESS)
		goto inconclusive;
	if (status[0] == __RESULT_NOTAPPL || status[1] == __RESULT_NOTAPPL || status[2] == __RESULT_NOTAPPL)
		return (__RESULT_NOTAPPL);
	if (status[0] == __RESULT_SKIPPED || status[1] == __RESULT_SKIPPED || status[2] == __RESULT_SKIPPED)
		return (__RESULT_SKIPPED);
	if (status[0] == __RESULT_FAILURE || status[1] == __RESULT_FAILURE || status[2] == __RESULT_FAILURE)
		return (__RESULT_FAILURE);
	if (status[0] == __RESULT_SUCCESS && status[1] == __RESULT_SUCCESS && status[2] == __RESULT_SUCCESS)
		return (__RESULT_SUCCESS);
      inconclusive:
	return (__RESULT_INCONCLUSIVE);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  Test case lists
 *
 *  -------------------------------------------------------------------------
 */

struct test_case {
	const char *numb;		/* test case number */
	const char *tgrp;		/* test case group */
	const char *sgrp;		/* test case subgroup */
	const char *name;		/* test case name */
	const char *xtra;		/* test case extra information */
	const char *desc;		/* test case description */
	const char *sref;		/* test case standards section reference */
	struct test_stream *stream[3];	/* test streams */
	int (*start) (int);		/* start function */
	int (*stop) (int);		/* stop function */
	ulong duration;			/* maximum duration */
	int run;			/* whether to run this test */
	int result;			/* results of test */
	int expect;			/* expected result */
} tests[] = {
	{
	numb_case_0_1, tgrp_case_0_1, sgrp_case_0_1, name_case_0_1, xtra_case_0_1, desc_case_0_1, sref_case_0_1, test_case_0_1, &begin_tests, &end_tests, 5000, 0, 0, __RESULT_INCONCLUSIVE,}, {
	numb_case_0_2_1, tgrp_case_0_2_1, sgrp_case_0_2_1, name_case_0_2_1, xtra_case_0_2_1, desc_case_0_2_1, sref_case_0_2_1, test_case_0_2_1, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_0_2_2, tgrp_case_0_2_2, sgrp_case_0_2_2, name_case_0_2_2, xtra_case_0_2_2, desc_case_0_2_2, sref_case_0_2_2, test_case_0_2_2, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_0_2_3, tgrp_case_0_2_3, sgrp_case_0_2_3, name_case_0_2_3, xtra_case_0_2_3, desc_case_0_2_3, sref_case_0_2_3, test_case_0_2_3, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_0_2_4, tgrp_case_0_2_4, sgrp_case_0_2_4, name_case_0_2_4, xtra_case_0_2_4, desc_case_0_2_4, sref_case_0_2_4, test_case_0_2_4, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_0_2_5, tgrp_case_0_2_5, sgrp_case_0_2_5, name_case_0_2_5, xtra_case_0_2_5, desc_case_0_2_5, sref_case_0_2_5, test_case_0_2_5, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_1_1, tgrp_case_1_1_1, sgrp_case_1_1_1, name_case_1_1_1, xtra_case_1_1_1, desc_case_1_1_1, sref_case_1_1_1, test_case_1_1_1, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_1_1_2, tgrp_case_1_1_2, sgrp_case_1_1_2, name_case_1_1_2, xtra_case_1_1_2, desc_case_1_1_2, sref_case_1_1_2, test_case_1_1_2, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_1_1, tgrp_case_2_1_1, sgrp_case_2_1_1, name_case_2_1_1, xtra_case_2_1_1, desc_case_2_1_1, sref_case_2_1_1, test_case_2_1_1, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_1_2, tgrp_case_2_1_2, sgrp_case_2_1_2, name_case_2_1_2, xtra_case_2_1_2, desc_case_2_1_2, sref_case_2_1_2, test_case_2_1_2, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_2_1, tgrp_case_2_2_1, sgrp_case_2_2_1, name_case_2_2_1, xtra_case_2_2_1, desc_case_2_2_1, sref_case_2_2_1, test_case_2_2_1, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_2_2, tgrp_case_2_2_2, sgrp_case_2_2_2, name_case_2_2_2, xtra_case_2_2_2, desc_case_2_2_2, sref_case_2_2_2, test_case_2_2_2, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_3_1, tgrp_case_2_3_1, sgrp_case_2_3_1, name_case_2_3_1, xtra_case_2_3_1, desc_case_2_3_1, sref_case_2_3_1, test_case_2_3_1, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_3_2, tgrp_case_2_3_2, sgrp_case_2_3_2, name_case_2_3_2, xtra_case_2_3_2, desc_case_2_3_2, sref_case_2_3_2, test_case_2_3_2, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_2_3_3, tgrp_case_2_3_3, sgrp_case_2_3_3, name_case_2_3_3, xtra_case_2_3_3, desc_case_2_3_3, sref_case_2_3_3, test_case_2_3_3, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_3_1_1, tgrp_case_3_1_1, sgrp_case_3_1_1, name_case_3_1_1, xtra_case_3_1_1, desc_case_3_1_1, sref_case_3_1_1, test_case_3_1_1, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_3_1_2, tgrp_case_3_1_2, sgrp_case_3_1_2, name_case_3_1_2, xtra_case_3_1_2, desc_case_3_1_2, sref_case_3_1_2, test_case_3_1_2, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_3_2_1, tgrp_case_3_2_1, sgrp_case_3_2_1, name_case_3_2_1, xtra_case_3_2_1, desc_case_3_2_1, sref_case_3_2_1, test_case_3_2_1, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_3_2_2, tgrp_case_3_2_2, sgrp_case_3_2_2, name_case_3_2_2, xtra_case_3_2_2, desc_case_3_2_2, sref_case_3_2_2, test_case_3_2_2, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_4_1_1, tgrp_case_4_1_1, sgrp_case_4_1_1, name_case_4_1_1, xtra_case_4_1_1, desc_case_4_1_1, sref_case_4_1_1, test_case_4_1_1, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_4_1_2, tgrp_case_4_1_2, sgrp_case_4_1_2, name_case_4_1_2, xtra_case_4_1_2, desc_case_4_1_2, sref_case_4_1_2, test_case_4_1_2, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_4_2_1, tgrp_case_4_2_1, sgrp_case_4_2_1, name_case_4_2_1, xtra_case_4_2_1, desc_case_4_2_1, sref_case_4_2_1, test_case_4_2_1, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_4_2_2, tgrp_case_4_2_2, sgrp_case_4_2_2, name_case_4_2_2, xtra_case_4_2_2, desc_case_4_2_2, sref_case_4_2_2, test_case_4_2_2, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	numb_case_5_1_1, tgrp_case_5_1_1, sgrp_case_5_1_1, name_case_5_1_1, xtra_case_5_1_1, desc_case_5_1_1, sref_case_5_1_1, test_case_5_1_1, &begin_tests, &end_tests, 0, 0, 0, __RESULT_SUCCESS,}, {
	NULL,}
};

static int summary = 0;

void
print_header(void)
{
	if (verbose <= 0)
		return;
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "\n%s - %s - %s - Conformance Test Suite\n", lstdname, lpkgname, shortname);
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

int
do_tests(int num_tests)
{
	int i;
	int result = __RESULT_INCONCLUSIVE;
	int notapplicable = 0;
	int inconclusive = 0;
	int successes = 0;
	int failures = 0;
	int skipped = 0;
	int notselected = 0;
	int aborted = 0;
	int repeat = 0;
	int oldverbose = verbose;

	print_header();
	show = 0;
	if (verbose > 0) {
		dummy = lockf(fileno(stdout), F_LOCK, 0);
		fprintf(stdout, "\nUsing device %s\n\n", devname);
		fflush(stdout);
		dummy = lockf(fileno(stdout), F_ULOCK, 0);
	}
	if (num_tests == 1 || begin_tests(0) == __RESULT_SUCCESS) {
		if (num_tests != 1)
			end_tests(0);
		show = 1;
		for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
		      rerun:
			if (!tests[i].run) {
				tests[i].result = __RESULT_INCONCLUSIVE;
				notselected++;
				continue;
			}
			if (aborted) {
				tests[i].result = __RESULT_INCONCLUSIVE;
				inconclusive++;
				continue;
			}
			if (verbose > 0) {
				dummy = lockf(fileno(stdout), F_LOCK, 0);
				if (verbose > 1 && tests[i].tgrp)
					fprintf(stdout, "\nTest Group: %s", tests[i].tgrp);
				if (verbose > 1 && tests[i].sgrp)
					fprintf(stdout, "\nTest Subgroup: %s", tests[i].sgrp);
				if (tests[i].xtra)
					fprintf(stdout, "\nTest Case %s-%s/%s: %s (%s)\n", sstdname, shortname, tests[i].numb, tests[i].name, tests[i].xtra);
				else
				fprintf(stdout, "\nTest Case %s-%s/%s: %s\n", sstdname, shortname, tests[i].numb, tests[i].name);
				if (verbose > 1 && tests[i].sref)
					fprintf(stdout, "Test Reference: %s\n", tests[i].sref);
				if (verbose > 1 && tests[i].desc)
					fprintf(stdout, "%s\n", tests[i].desc);
				fprintf(stdout, "\n");
				fflush(stdout);
				dummy = lockf(fileno(stdout), F_ULOCK, 0);
			}
			if ((result = tests[i].result) == 0) {
				ulong duration = test_duration;

				if (duration > tests[i].duration) {
					if (tests[i].duration && duration > tests[i].duration)
						duration = tests[i].duration;
				if ((result = (*tests[i].start) (i)) != __RESULT_SUCCESS)
					goto inconclusive;
				result = test_run(tests[i].stream, duration);
				(*tests[i].stop) (i);
				} else
					result = __RESULT_SKIPPED;
				if (result == tests[i].expect) {
					switch (result) {
					case __RESULT_SUCCESS:
					case __RESULT_NOTAPPL:
					case __RESULT_SKIPPED:
						/* autotest can handle these */
						break;
					default:
					case __RESULT_INCONCLUSIVE:
					case __RESULT_FAILURE:
						/* these are expected failures */
						result = __RESULT_SUCCESS;
						break;
					}
				}
			} else {
				if (result == tests[i].expect) {
					switch (result) {
					case __RESULT_SUCCESS:
					case __RESULT_NOTAPPL:
					case __RESULT_SKIPPED:
						/* autotest can handle these */
						break;
					default:
					case __RESULT_INCONCLUSIVE:
					case __RESULT_FAILURE:
						/* these are expected failures */
						result = __RESULT_SUCCESS;
						break;
					}
				}
				switch (result) {
				case __RESULT_SUCCESS:
					print_passed(3);
					break;
				case __RESULT_FAILURE:
					print_failed(3);
					break;
				case __RESULT_NOTAPPL:
					print_notapplicable(3);
					break;
				case __RESULT_SKIPPED:
					print_skipped(3);
					break;
				default:
				case __RESULT_INCONCLUSIVE:
					print_inconclusive(3);
					break;
				}
			}
			switch (result) {
			case __RESULT_SUCCESS:
				successes++;
				if (verbose > 0) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "\n");
					fprintf(stdout, "*********\n");
					fprintf(stdout, "********* Test Case SUCCESSFUL\n");
					fprintf(stdout, "*********\n\n");
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			case __RESULT_FAILURE:
				if (!repeat_verbose || repeat)
					failures++;
				if (verbose > 0) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "\n");
					fprintf(stdout, "XXXXXXXXX\n");
					fprintf(stdout, "XXXXXXXXX Test Case FAILED\n");
					fprintf(stdout, "XXXXXXXXX\n\n");
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			case __RESULT_NOTAPPL:
				notapplicable++;
				if (verbose > 0) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "\n");
					fprintf(stdout, "XXXXXXXXX\n");
					fprintf(stdout, "XXXXXXXXX Test Case NOT APPLICABLE\n");
					fprintf(stdout, "XXXXXXXXX\n\n");
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			case __RESULT_SKIPPED:
				skipped++;
				if (verbose > 0) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "\n");
					fprintf(stdout, "XXXXXXXXX\n");
					fprintf(stdout, "XXXXXXXXX Test Case SKIPPED\n");
					fprintf(stdout, "XXXXXXXXX\n\n");
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			default:
			case __RESULT_INCONCLUSIVE:
			      inconclusive:
				if (!repeat_verbose || repeat)
					inconclusive++;
				if (verbose > 0) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "\n");
					fprintf(stdout, "?????????\n");
					fprintf(stdout, "????????? Test Case INCONCLUSIVE\n");
					fprintf(stdout, "?????????\n\n");
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
				}
				break;
			}
			if (repeat_on_failure && (result == __RESULT_FAILURE || result == __RESULT_INCONCLUSIVE))
				goto rerun;
			if (repeat_on_success && (result == __RESULT_SUCCESS))
				goto rerun;
			if (repeat) {
				repeat = 0;
				verbose = oldverbose;
			} else if (repeat_verbose && (result == __RESULT_FAILURE || result == __RESULT_INCONCLUSIVE)) {
				repeat = 1;
				verbose = 5;
				goto rerun;
			}
			tests[i].result = result;
			if (exit_on_failure && (result == __RESULT_FAILURE || result == __RESULT_INCONCLUSIVE))
				aborted = 1;
		}
		if (summary && verbose) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
			for (i = 0; i < (sizeof(tests) / sizeof(struct test_case)) && tests[i].numb; i++) {
				if (tests[i].run) {
					dummy = lockf(fileno(stdout), F_LOCK, 0);
					fprintf(stdout, "Test Case %s-%s/%-10s ", sstdname, shortname, tests[i].numb);
					fflush(stdout);
					dummy = lockf(fileno(stdout), F_ULOCK, 0);
					switch (tests[i].result) {
					case __RESULT_SUCCESS:
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "SUCCESS\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
						break;
					case __RESULT_FAILURE:
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "FAILURE\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
						break;
					case __RESULT_NOTAPPL:
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "NOT APPLICABLE\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
						break;
					case __RESULT_SKIPPED:
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "SKIPPED\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
						break;
					default:
					case __RESULT_INCONCLUSIVE:
						dummy = lockf(fileno(stdout), F_LOCK, 0);
						fprintf(stdout, "INCONCLUSIVE\n");
						fflush(stdout);
						dummy = lockf(fileno(stdout), F_ULOCK, 0);
						break;
					}
				}
			}
		}
		if (verbose > 0 && num_tests > 1) {
			dummy = lockf(fileno(stdout), F_LOCK, 0);
			fprintf(stdout, "\n");
			fprintf(stdout, "========= %3d successes     \n", successes);
			fprintf(stdout, "========= %3d failures      \n", failures);
			fprintf(stdout, "========= %3d inconclusive  \n", inconclusive);
			fprintf(stdout, "========= %3d not applicable\n", notapplicable);
			fprintf(stdout, "========= %3d skipped       \n", skipped);
			fprintf(stdout, "========= %3d not selected  \n", notselected);
			fprintf(stdout, "============================\n");
			fprintf(stdout, "========= %3d total         \n", successes + failures + inconclusive + notapplicable + skipped + notselected);
			if (!(aborted + failures))
				fprintf(stdout, "\nDone.\n\n");
			fflush(stdout);
			dummy = lockf(fileno(stdout), F_ULOCK, 0);
		}
		if (aborted) {
			dummy = lockf(fileno(stderr), F_LOCK, 0);
			if (verbose > 0)
				fprintf(stderr, "\n");
			fprintf(stderr, "Test Suite aborted due to failure.\n");
			if (verbose > 0)
				fprintf(stderr, "\n");
			fflush(stderr);
			dummy = lockf(fileno(stderr), F_ULOCK, 0);
		} else if (failures) {
			dummy = lockf(fileno(stderr), F_LOCK, 0);
			if (verbose > 0)
				fprintf(stderr, "\n");
			fprintf(stderr, "Test Suite failed.\n");
			if (verbose > 0)
				fprintf(stderr, "\n");
			fflush(stderr);
			dummy = lockf(fileno(stderr), F_ULOCK, 0);
		}
		if (num_tests == 1) {
			if (successes)
				return (0);
			if (failures)
				return (1);
			if (inconclusive)
				return (1);
			if (notapplicable)
				return (0);
			if (skipped)
				return (77);
		}
		return (aborted);
	} else {
		end_tests(0);
		show = 1;
		dummy = lockf(fileno(stderr), F_LOCK, 0);
		fprintf(stderr, "Test Suite setup failed!\n");
		fflush(stderr);
		dummy = lockf(fileno(stderr), F_ULOCK, 0);
		return (2);
	}
}

void
copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	print_header();
	fprintf(stdout, "\
\n\
Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>\n\
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
Unauthorized distribution or duplication is prohibited.\n\
\n\
This software and related documentation is protected by copyright and distribut-\n\
ed under licenses restricting its use,  copying, distribution and decompilation.\n\
No part of this software or related documentation may  be reproduced in any form\n\
by any means without the prior  written  authorization of the  copyright holder,\n\
and licensors, if any.\n\
\n\
The recipient of this document,  by its retention and use, warrants that the re-\n\
cipient  will protect this  information and  keep it confidential,  and will not\n\
disclose the information contained  in this document without the written permis-\n\
sion of its owner.\n\
\n\
The author reserves the right to revise  this software and documentation for any\n\
reason,  including but not limited to, conformity with standards  promulgated by\n\
various agencies, utilization of advances in the state of the technical arts, or\n\
the reflection of changes  in the design of any techniques, or procedures embod-\n\
ied, described, or  referred to herein.   The author  is under no  obligation to\n\
provide any feature listed herein.\n\
\n\
As an exception to the above,  this software may be  distributed  under the  GNU\n\
Affero  General  Public  License  (AGPL)  Version  3, so long as the software is\n\
distributed with, and only used for the testing of, OpenSS7 modules, drivers and\n\
libraries.\n\
\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the  U.S. Government  (\"Government\"),  the following provisions apply to you.\n\
If the Software is  supplied by the Department of Defense (\"DoD\"), it is classi-\n\
fied as  \"Commercial Computer Software\"  under paragraph 252.227-7014 of the DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the license rights  granted\n\
herein (the license  rights customarily  provided to non-Government  users).  If\n\
the Software is supplied to any unit or agency of the Government other than DoD,\n\
it is classified as  \"Restricted Computer Software\" and the  Government's rights\n\
in the  Software are defined in  paragraph 52.227-19 of the Federal  Acquisition\n\
Regulations  (\"FAR\") (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of the  NASA Supplement  to the  FAR (or  any  successor\n\
regulations).\n\
\n\
");
}

void
version(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s:\n\
    %2$s\n\
    Copyright (c) 2008-2010  Monavacon Limited.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under AGPL Version 3,\n\
    incorporated here by reference.\n\
\n\
    See `%1$s --copying' for copying permission.\n\
\n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
Options:\n\
    -u, --iut\n\
        IUT connects instead of PT.\n\
    -F, --decl-std [STANDARD]\n\
        specify the SS7 standard version to test.\n\
    -D, --draft [DRAFT]\n\
        specify the M3UA draft or RFC version to test.\n\
    -c, --client\n\
        execute client side (PTU) of test case only.\n\
    -S, --server\n\
        execute server side (IUT) of test case only.\n\
    -a, --again\n\
        repeat failed tests verbose.\n\
    -w, --wait\n\
        have server wait indefinitely.\n\
    -r, --repeat\n\
        repeat test cases on success or failure.\n\
    -R, --repeat-fail\n\
        repeat test cases on failure.\n\
    -p, --client-port [PORT]\n\
        port number from which to connect [default: %3$d+index*3]\n\
    -P, --server-port [PORT]\n\
        port number to which to connect or upon which to listen\n\
        [default: %3$d+index*3+2]\n\
    -i, --client-host [HOSTNAME[,HOSTNAME]*]\n\
        client host names(s) or IP numbers\n\
        [default: 127.0.0.1,127.0.0.2,127.0.0.3]\n\
    -I, --server-host [HOSTNAME[,HOSTNAME]*]\n\
        server host names(s) or IP numbers\n\
        [default: 127.0.0.1,127.0.0.2,127.0.0.3]\n\
    -d, --device DEVICE\n\
        device name to open [default: %2$s].\n\
    -e, --exit\n\
        exit on the first failed or inconclusive test case.\n\
    -l, --list [RANGE]\n\
        list test case names within a range [default: all] and exit.\n\
    -f, --fast [SCALE]\n\
        increase speed of tests by scaling timers [default: 50]\n\
    -s, --summary\n\
        print a test case summary at end of testing [default: off]\n\
    -o, --onetest [TESTCASE]\n\
        run a single test case.\n\
    -t, --tests [RANGE]\n\
        run a range of test cases.\n\
    -m, --messages\n\
        display messages. [default: off]\n\
    -q, --quiet\n\
        suppress normal output (equivalent to --verbose=0)\n\
    -v, --verbose [LEVEL]\n\
        increase verbosity or set to LEVEL [default: 1]\n\
        this option may be repeated.\n\
    -h, --help, -?, --?\n\
        print this usage message and exit\n\
    -V, --version\n\
        print version and exit\n\
    -C, --copying\n\
        print copying permission and exit\n\
\n\
", argv[0], devname, TEST_PORT_NUMBER);
}

#define HOST_BUF_LEN 128

int
main(int argc, char *argv[])
{
	size_t l, n;
	int range = 0;
	struct test_case *t;
	int tests_to_run = 0;

	char *hostc = "127.0.0.1,127.0.0.2,127.0.0.3";
	char *hosts = "127.0.0.1,127.0.0.2,127.0.0.3";
	char hostbufc[HOST_BUF_LEN];
	char hostbufs[HOST_BUF_LEN];
	struct hostent *haddr;

	for (t = tests; t->numb; t++) {
		if (!t->result) {
			t->run = 1;
			tests_to_run++;
		}
	}
	for (;;) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"rc",		optional_argument,	NULL, 'T'},
			{"aspid",	optional_argument,	NULL, 'A'},
			{"iut",		no_argument,		NULL, 'u'},
			{"draft",	required_argument,	NULL, 'D'},
			{"decl-std",	required_argument,	NULL, 'F'},
			{"client",	no_argument,		NULL, 'c'},
			{"server",	no_argument,		NULL, 'S'},
			{"again",	no_argument,		NULL, 'a'},
			{"wait",	no_argument,		NULL, 'w'},
			{"client-port",	required_argument,	NULL, 'p'},
			{"server-port",	required_argument,	NULL, 'P'},
			{"client-host",	required_argument,	NULL, 'i'},
			{"server-host",	required_argument,	NULL, 'I'},
			{"repeat",	no_argument,		NULL, 'r'},
			{"repeat-fail",	no_argument,		NULL, 'R'},
			{"device",	required_argument,	NULL, 'd'},
			{"transport",	required_argument,	NULL, 'x'},
			{"exit",	no_argument,		NULL, 'e'},
			{"list",	optional_argument,	NULL, 'l'},
			{"fast",	optional_argument,	NULL, 'f'},
			{"summary",	no_argument,		NULL, 's'},
			{"onetest",	required_argument,	NULL, 'o'},
			{"tests",	required_argument,	NULL, 't'},
			{"messages",	no_argument,		NULL, 'm'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{"tmode",	optional_argument,	NULL, '\1'},
			{"dpc",		optional_argument,	NULL, '\2'},
			{"opc",		optional_argument,	NULL, '\3'},
			{"si",		optional_argument,	NULL, '\4'},
			{"na",		optional_argument,	NULL, '\5'},
			{"dynamic",	no_argument,		NULL, '\6'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "T:A:uD:F:cSawp:P:i:I:rRd:x:el::f::so:t:mqvhVC?", long_options, &option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "T:A:uD:F:cSawp:P:i:I:rRd:x:el::f::so:t:mqvhVC?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 'T':	/* -T, --rc RC */
			if (optarg) {
				m3ua.rc.val = strtoul(optarg, NULL, 0);
				m3ua.rc.len = sizeof(uint32_t);
			} else {
				m3ua.rc.len = 0;
			}
			break;
		case 'A':	/* -A, --aspid ASPID */
			if (optarg) {
				m3ua.aspid.val = strtoul(optarg, NULL, 0);
				m3ua.aspid.len = sizeof(uint32_t);
			} else {
				m3ua.aspid.len = 0;
			}
			break;
		case '\1':	/* --tmode TMODE */
			if (optarg) {
				m3ua.tmode.val = strtoul(optarg, NULL, 0);
				m3ua.tmode.len = sizeof(uint32_t);
			} else {
				m3ua.tmode.len = 0;
			}
			break;
		case '\2':	/* --dpc TMODE */
			if (optarg) {
				m3ua.tmode.val = strtoul(optarg, NULL, 0);
				m3ua.tmode.len = sizeof(uint32_t);
			} else {
				m3ua.tmode.len = 0;
			}
			break;
		case '\3':	/* --opc TMODE */
			if (optarg) {
				m3ua.tmode.val = strtoul(optarg, NULL, 0);
				m3ua.tmode.len = sizeof(uint32_t);
			} else {
				m3ua.tmode.len = 0;
			}
			break;
		case '\4':	/* --si TMODE */
			if (optarg) {
				m3ua.si.val = (strtoul(optarg, NULL, 0) << 24);
				m3ua.si.len = 1;
			} else {
				m3ua.si.len = 0;
			}
			break;
		case '\5':	/* --na NA */
			if (optarg) {
				m3ua.ntwk_app.val = strtoul(optarg, NULL, 0);
				m3ua.ntwk_app.len = sizeof(uint32_t);
			} else {
				m3ua.ntwk_app.len = 0;
			}
			break;
		case '\6':	/* --dynamic */
			m3ua.rc.len = 0;
			break;
		case 'u':	/* -u, --iut */
			iut_connects = 1;
			break;
		case 'F':	/* -F, --decl-std */
			break;
		case 'D':	/* -D, --draft */
			break;
		case 'c':	/* -c, --client */
			client_exec = 1;
			break;
		case 'S':	/* -S, --server */
			server_exec = 1;
			break;
		case 'a':	/* -a, --again */
			repeat_verbose = 1;
			break;
		case 'w':	/* -w, --wait */
			test_duration = INFINITE_WAIT;
			break;
		case 'p':	/* -p, --client-port [PORT] */
			client_port_specified = 1;
			ports[3] = atoi(optarg);
			ports[0] = ports[3];
			break;
		case 'P':	/* -P, --server-port [PORT] */
			server_port_specified = 1;
			ports[3] = atoi(optarg);
			ports[1] = ports[3];
			ports[2] = ports[3] + 1;
			break;
		case 'i':	/* -i, --client-host [HOSTNAME[,HOSTNAME]*] */
			client_host_specified = 1;
			strncpy(hostbufc, optarg, HOST_BUF_LEN);
			hostc = hostbufc;
			break;
		case 'I':	/* -I, --server-host [HOSTNAME[,HOSTNAME]*] */
			server_host_specified = 1;
			strncpy(hostbufs, optarg, HOST_BUF_LEN);
			hosts = hostbufs;
			break;
		case 'r':	/* -r, --repeat */
			repeat_on_success = 1;
			repeat_on_failure = 1;
			break;
		case 'R':	/* -R, --repeat-fail */
			repeat_on_failure = 1;
			break;
		case 'd':	/* -d, --device [DEVICE] */
			if (optarg) {
				snprintf(devname, sizeof(devname), "%s", optarg);
				break;
			}
			goto bad_option;
		case 'x':	/* -x, --transport [DEVICE] */
			if (optarg) {
				snprintf(devname, sizeof(devname), "%s", optarg);
				break;
			}
			goto bad_option;
		case 'e':	/* -e, --exit */
			exit_on_failure = 1;
			break;
		case 'l':	/* -l, --list [RANGE] */
			if (optarg) {
				l = strnlen(optarg, 16);
				fprintf(stdout, "\n");
				for (n = 0, t = tests; t->numb; t++)
					if (!strncmp(t->numb, optarg, l)) {
						if (verbose > 2 && t->tgrp)
							fprintf(stdout, "Test Group: %s\n", t->tgrp);
						if (verbose > 2 && t->sgrp)
							fprintf(stdout, "Test Subgroup: %s\n", t->sgrp);
						if (t->xtra)
							fprintf(stdout, "Test Case %s-%s/%s: %s (%s)\n", sstdname, shortname, t->numb, t->name, t->xtra);
						else
						fprintf(stdout, "Test Case %s-%s/%s: %s\n", sstdname, shortname, t->numb, t->name);
						if (verbose > 2 && t->sref)
							fprintf(stdout, "Test Reference: %s\n", t->sref);
						if (verbose > 1 && t->desc)
							fprintf(stdout, "%s\n\n", t->desc);
						fflush(stdout);
						n++;
					}
				if (!n) {
					fprintf(stderr, "WARNING: specification `%s' matched no test\n", optarg);
					fflush(stderr);
					goto bad_option;
				}
				if (verbose <= 1)
					fprintf(stdout, "\n");
				fflush(stdout);
				exit(0);
			} else {
				fprintf(stdout, "\n");
				for (t = tests; t->numb; t++) {
					if (verbose > 2 && t->tgrp)
						fprintf(stdout, "Test Group: %s\n", t->tgrp);
					if (verbose > 2 && t->sgrp)
						fprintf(stdout, "Test Subgroup: %s\n", t->sgrp);
					if (t->xtra)
						fprintf(stdout, "Test Case %s-%s/%s: %s (%s)\n", sstdname, shortname, t->numb, t->name, t->xtra);
					else
					fprintf(stdout, "Test Case %s-%s/%s: %s\n", sstdname, shortname, t->numb, t->name);
					if (verbose > 2 && t->sref)
						fprintf(stdout, "Test Reference: %s\n", t->sref);
					if (verbose > 1 && t->desc)
						fprintf(stdout, "%s\n\n", t->desc);
					fflush(stdout);
				}
				if (verbose <= 1)
					fprintf(stdout, "\n");
				fflush(stdout);
				exit(0);
			}
			break;
		case 'f':	/* -f, --fast [SCALE] */
			if (optarg)
				timer_scale = atoi(optarg);
			else
				timer_scale = 50;
			fprintf(stderr, "WARNING: timers are scaled by a factor of %ld\n", (long) timer_scale);
			break;
		case 's':	/* -s, --summary */
			summary = 1;
			break;
		case 'o':	/* -o, --onetest [TESTCASE] */
			if (optarg) {
				if (!range) {
					for (t = tests; t->numb; t++)
						t->run = 0;
					tests_to_run = 0;
				}
				range = 1;
				for (n = 0, t = tests; t->numb; t++)
					if (!strncmp(t->numb, optarg, 16)) {
						// if (!t->result) {
						t->run = 1;
						n++;
						tests_to_run++;
						// }
					}
				if (!n) {
					fprintf(stderr, "WARNING: specification `%s' matched no test\n", optarg);
					fflush(stderr);
					goto bad_option;
				}
				break;
			}
			goto bad_option;
		case 'q':	/* -q, --quiet */
			verbose = 0;
			break;
		case 'v':	/* -v, --verbose [LEVEL] */
			if (optarg == NULL) {
				verbose++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			verbose = val;
			break;
		case 't':	/* -t, --tests [RANGE] */
			l = strnlen(optarg, 16);
			if (!range) {
				for (t = tests; t->numb; t++)
					t->run = 0;
				tests_to_run = 0;
			}
			range = 1;
			for (n = 0, t = tests; t->numb; t++)
				if (!strncmp(t->numb, optarg, l)) {
					// if (!t->result) {
					t->run = 1;
					n++;
					tests_to_run++;
					// }
				}
			if (!n) {
				fprintf(stderr, "WARNING: specification `%s' matched no test\n", optarg);
				fflush(stderr);
				goto bad_option;
			}
			break;
		case 'm':	/* -m, --messages */
			show_msg = 1;
			break;
		case 'H':	/* -H */
		case 'h':	/* -h, --help, -?, --? */
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		case 'C':	/* -C, --copying */
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
		      bad_nonopt:
			if (optind < argc && verbose) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				while (optind < argc)
					fprintf(stderr, "%s ", argv[optind++]);
				fprintf(stderr, "\n");
				fflush(stderr);
			}
			goto bad_usage;
		      bad_usage:
			usage(argc, argv);
			exit(2);
		}
	}
	/* 
	 * dont' ignore non-option arguments
	 */
	if (optind < argc)
		goto bad_nonopt;
	switch (tests_to_run) {
	case 0:
		if (verbose > 0) {
			fprintf(stderr, "%s: error: no tests to run\n", argv[0]);
			fflush(stderr);
		}
		exit(2);
	case 1:
		break;
	default:
		copying(argc, argv);
	}
	if (client_exec == 0 && server_exec == 0) {
		client_exec = 1;
		server_exec = 1;
	}
	if (client_host_specified) {
		char *token = hostc, *next_token, *delim = NULL;

		fprintf(stdout, "Specified client address => %s\n", token);
		do {
			if ((delim = index(token, ','))) {
				delim[0] = '\0';
				next_token = delim + 1;
			} else
				next_token = NULL;
			haddr = gethostbyname(token);
			addrs[0][count].sin_family = AF_INET;
			addrs[3][count].sin_family = AF_INET;
			if (client_port_specified) {
				addrs[0][count].sin_port = htons(ports[0]);
				addrs[3][count].sin_port = htons(ports[3]);
			} else {
				addrs[0][count].sin_port = 0;
				addrs[3][count].sin_port = 0;
			}
			addrs[0][count].sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);
			addrs[3][count].sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);
			count++;
		} while ((token = next_token) && count < 4);
		anums[0] = count;
		anums[3] = count;
		fprintf(stdout, "%d client addresses assigned\n", count);
	}
	if (server_host_specified) {
		char *token = hosts, *next_token, *delim = NULL;

		fprintf(stdout, "Specified server address => %s\n", token);
		do {
			if ((delim = index(token, ','))) {
				delim[0] = '\0';
				next_token = delim + 1;
			} else
				next_token = NULL;
			haddr = gethostbyname(token);
			addrs[1][count].sin_family = AF_INET;
			addrs[2][count].sin_family = AF_INET;
			addrs[3][count].sin_family = AF_INET;
			if (server_port_specified) {
				addrs[1][count].sin_port = htons(ports[1]);
				addrs[2][count].sin_port = htons(ports[2]);
				addrs[3][count].sin_port = htons(ports[3]);
			} else {
				addrs[1][count].sin_port = 0;
				addrs[2][count].sin_port = 0;
				addrs[3][count].sin_port = 0;
			}
			addrs[1][count].sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);
			addrs[2][count].sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);
			addrs[3][count].sin_addr.s_addr = *(uint32_t *) (haddr->h_addr);
			count++;
		} while ((token = next_token) && count < 4);
		anums[1] = count;
		anums[2] = count;
		anums[3] = count;
		fprintf(stdout, "%d server addresses assigned\n", count);
	}
	exit(do_tests(tests_to_run));
}
